/*

Still working on this

*/

(function() {
    require.paths.unshift(__dirname + "/../lib");

    var sys = require('sys'),
        assert = require('assert'),
		total_tests = 8,
		undef,
		called,
		Proxy = require("node-proxy"),
		createProxy = function(handlers) {
			called = "createProxy";
			return Proxy.create({
				getOwnPropertyDescriptor:function(name){
					called = "getOwnPropertyDescriptor";
					return handlers.hasOwnProperty(name) ? handlers[name] : undef;
				},
				getPropertyDescriptor:function(name){
					called = "getPropertyDescriptor";
					return name in handlers ? handlers[name] : undef;
				},
				defineProperty:function(name, pd){
					called = "defineProperty";
					if (name in handlers && !handlers[name].configurable) {
						return null;
					}
					handlers[name] = pd;
					return null;
				},
				getOwnPropertyNames:function(){
					called = "getOwnPropertyNames";
					return Object.getOwnPropertyNames(handlers);
				},
				"delete":function(name){
					called = "delete";
					return !(name in handlers) ? true : (delete handlers[name]);
				},
				enumerate:function(){
					called = "enumerate";
					return Object.keys(handlers);
				},
				fix:function(){
					called = "fix";
					return handlers;
				},
				has:function(name){
					called = "has";
					return (name in handlers);
				},
				hasOwn:function(name){
					called = "hasOwn";
					return handlers.hasOwnProperty(name);
				},
				get:function(receiver, name){
					called = "get";
					if (!(name in handlers)) {
						return undef;
					}
					return "get" in handlers[name] && typeof(handlers[name].get) == "function" ? 
							handlers[name].get.call(receiver) : 
							(handlers[name].value || undef);
				},
				set:function(receiver, name, val){
					called = "set";
					if (!(name in handlers)) {
						handlers[name] = {
							configurable:true,
							writable:true,
							enumerable:true,
							value:val,
							get:function(){return val},
							set:function(v){val=v}
						};
						return true;
					}
					if (!handlers[name].configurable) {
						return false;
					}
					if ("set" in handlers[name]) {
						handlers[name].set.call(receiver, val);
					}
				
					handlers[name].value = val;
					return true;
				},
				keys:function(){
					called = "keys";
					return Object.getOwnPropertyNames(handlers);
				},
			});
		},
		protoProxy = function(){},
		proxyTest,
		firstValue = "firstProp",
		names, count,
		handlers = {
			first: {
				get:function(){return firstValue},
				set:function(val){firstValue = val}
			}
		};
	
	sys.puts("Running tests...");
	sys.puts("Test 1 of " + total_tests + ": Creating proxy");
	proxyTest = createProxy(handlers);
	assert.equal(called, "createProxy", "createProxy was called");
	
	sys.puts("Test 2 of " + total_tests + ": has");
	assert.ok("first" in proxyTest);
	assert.equal(called, "has", "has was called");
	
	sys.puts("Test 3 of " + total_tests + ": Accessing getter");
	assert.equal(proxyTest.first, firstValue);
	assert.equal(called, "get", "get was called");
	
	sys.puts("Test 4 of " + total_tests + ": Accessing setter");
	proxyTest.first = "changed";
	assert.equal(called, "set", "set was called");
	assert.equal(proxyTest.first, firstValue, "proxyTest.first == firstValue");
	
	sys.puts("Test 5 of " + total_tests + ": set property");
	proxyTest.second = "secondProp";
	assert.equal(called, "set", "set new property");
	
	sys.puts("Test 6 of " + total_tests + ": Iterate property names");
	count = 0;
	for (p in proxyTest){++count;}
	assert.equal(count, 2, "there are 2 properties on proxyTest");
	
	sys.puts("Test 7 of " + total_tests + ": getOwnPropertyNames");
	names = Object.getOwnPropertyNames(proxyTest);
	assert.equal(called, "enumerate", "Object.getOwnPropertyNames invokes enumerate");
	assert.ok(names instanceof Array);
	assert.equal(names.length, 2, "2 property names were returned");
	assert.equal(names[0], "first", "The first property name is 'first'");
	assert.equal(names[1], "second", "The second property name is 'second'");
	
	sys.puts("Test 8 of " + total_tests + ": keys");
	names = Object.keys(proxyTest);
	assert.equal(called, "enumerate", "Object.keys invokes 'enumerate'");
	assert.ok(names instanceof Array);
	assert.equal(names.length, 2, "2 property names were returned");
	assert.equal(names[0], "first", "The first property name is 'first'");
	assert.equal(names[1], "second", "The second property name is 'second'");
	
	
	process.exit(0);
}());