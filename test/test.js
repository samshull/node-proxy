/*jslint forin: true, onevar: true, immed: true */

(function () {
    require.paths.unshift(__dirname + "/../lib");

    var sys = require('sys'),
        assert = require('assert'),
        total_tests = 11,
        undef,
        called, p,
        Proxy = require("node-proxy"),
        createProxy = function (handlers) {
            called = "createProxy";
            
            function defineProperty(name, pd){
                called = "defineProperty";
                if (name in handlers && !handlers[name].configurable) {
                    return null;
                }
                handlers[name] = pd;
                return null;
            }
            
            return Proxy.create({
                getOwnPropertyDescriptor:function (name){
                    called = "getOwnPropertyDescriptor";
                    return handlers.hasOwnProperty(name) ? handlers[name] : undef;
                },
                getPropertyDescriptor:function (name){
                    called = "getPropertyDescriptor";
                    return name in handlers ? handlers[name] : undef;
                },
                defineProperty: defineProperty,
                getOwnPropertyNames:function (){
                    called = "getOwnPropertyNames";
                    return Object.getOwnPropertyNames(handlers);
                },
                "delete":function (name){
                    called = "delete";
                    var r = true;
                    if (name in handlers) {
                        r = (delete handlers[name]);
                    }
                    return r;
                },
                enumerate:function (){
                    called = "enumerate";
                    return Object.keys(handlers);
                },
                fix:function (){
                    called = "fix";
                    return handlers;
                },
                has:function (name){
                    called = "has";
                    //sys.puts("has called on: "+name);
                    //sys.puts(name in handlers)
                    return (name in handlers);
                },
                hasOwn:function (name){
                    called = "hasOwn";
                    return handlers.hasOwnProperty(name);
                },
                get:function (receiver, name){
                    called = "get";
                    //sys.puts(arguments.callee.caller)
                    if (!(name in handlers)) {
                        return undef;
                    }
                    return "get" in handlers[name] && typeof(handlers[name].get) == "function" ? 
                            handlers[name].get.call(receiver) : 
                            (handlers[name].value || undef);
                },
                set:function (receiver, name, val){
                    called = "set";
                    if (!(name in handlers)) {
                        defineProperty.call(this, name, {
                            configurable:true,
                            writable:true,
                            enumerable:true,
                            value:val,
                            get:function (){return val;},
                            set:function (v){val=v;}
                        });
                        called = "set";
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
                keys:function (){
                    called = "keys";
                    return Object.getOwnPropertyNames(handlers);
                }
            });
        },
        proxyTest,
        firstValue = "firstProp",
        names, count,
        handlers = {
            first: {
                get:function (){return firstValue;},
                set:function (val){firstValue = val;}
            }
        };
    
    sys.puts("Running tests...");
    sys.puts("Test 1 of " + total_tests + ": Creating proxy");
    proxyTest = createProxy(handlers);
    assert.equal(called, "createProxy", "createProxy was not the last method called");
    assert.ok(typeof proxyTest == "object");
    
    sys.puts("Test 2 of " + total_tests + ": has");
    assert.ok("first" in proxyTest, "proxyTest does not have a property named 'first'");
    //assert.equal(called, "has", "the has method was not the last method called");
    
    sys.puts("Test 3 of " + total_tests + ": Accessing getter");
    assert.equal(proxyTest.first, firstValue);
    assert.equal(called, "get", "the get method was not the last method called");
    
    sys.puts("Test 4 of " + total_tests + ": Accessing setter");
    proxyTest.first = "changed";
    assert.equal(called, "set", "the set method was not the last method called");
    assert.equal(proxyTest.first, firstValue, "proxyTest.first != firstValue");
    
    sys.puts("Test 5 of " + total_tests + ": set property");
    proxyTest.second = "secondProp";
    assert.equal(called, "set", "the set method was not the last method called");
    
    sys.puts("Test 6 of " + total_tests + ": Iterate property names");
    count = 0;
    for (p in proxyTest){++count;}
    assert.equal(count, 2, "there are not 2 properties on proxyTest");
    
    sys.puts("Test 7 of " + total_tests + ": getOwnPropertyNames");
    names = Object.getOwnPropertyNames(proxyTest);
    assert.equal(called, "enumerate", "Object.getOwnPropertyNames did not invoke enumerate");
    assert.ok(names instanceof Array);
    assert.equal(names.length, 2, "2 property names were not returned");
    assert.equal(names[0], "first", "The first property name is not 'first'");
    assert.equal(names[1], "second", "The second property name is not 'second'");
    
    sys.puts("Test 8 of " + total_tests + ": keys");
    names = Object.keys(proxyTest);
    assert.equal(called, "enumerate", "Object.keys did not invoke 'enumerate'");
    assert.ok(names instanceof Array);
    assert.equal(names.length, 2, "2 property names were not returned");
    assert.equal(names[0], "first", "The first property name is not 'first'");
    assert.equal(names[1], "second", "The second property name is not 'second'");
    
    sys.puts("Test 9 of " + total_tests + ": delete");
    assert.ok((delete proxyTest.second), "Delete the property 'second' from the proxy");
    assert.equal(called, "delete", "the delete method was not the last method called");
    assert.ok(!Object.prototype.hasOwnProperty.call(proxyTest, "second"), "proxyTest still hasOwnProperty the property 'second'");
    assert.ok(!("second" in proxyTest), "proxyTest still has the property 'second'");
    
    sys.puts("Test 10 of " + total_tests + ": defineProperty");
	Proxy.defineProperty(proxyTest, 'third', {
		get: function() {
			return "third";
		}
	});
	assert.equal(called, "defineProperty", "defineProperty was not called: "+called);
    assert.ok("third" in proxyTest);
	assert.equal(proxyTest.third, "third", "proxyTest.third != 'third'");
    
    sys.puts("Test 11 of " + total_tests + ": defineProperties");
	Proxy.defineProperties(proxyTest, {
		fourth: {
			get: function() {
				return "fourth";
			}
		},
		fifth: {
			get: function() {
				return "fifth";
			}
		}
	});
	assert.equal(called, "defineProperty", "defineProperty was not called: "+called);
    assert.ok("fourth" in proxyTest);
	assert.ok("fifth" in proxyTest);
	assert.equal(proxyTest.fourth, "fourth", "proxyTest.fourth != 'fourth'");
	assert.equal(proxyTest.fifth, "fifth", "proxyTest.fifth != 'fifth'");
	assert.ok("fourth" in handlers, "'fourth' is not in handlers");
	assert.ok("fifth" in handlers, "'fifth' is not in handlers");
    
    
    
    process.exit(0);
}());