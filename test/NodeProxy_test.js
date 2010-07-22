/*

Still working on this

*/

(function() {
    require.paths.unshift(__dirname + "/../lib");

    var sys = require('sys'),
        assert = require('assert');

    var total_tests = 10;
    var tmpl, fn, pending_callbacks = 0;
	
	var Proxy = require("../build/default/NodeProxy"),
	fn,cn,rn,undefined,p,
	myProxyProperties = {
		
	},
	myFunctionProxyProperties = {
		
	},
	myProxy = Proxy.create({
	  getOwnPropertyDescriptor: function(name) {
		  sys.puts("myProxy.getOwnPropertyDescriptor called with '"+name+"'");
		  return myProxyProperties[name];
	  },
	  getPropertyDescriptor: function(name) {
		  sys.puts("myProxy.getPropertyDescriptor called with '"+name+"'");
		  return myProxyProperties[name];
	  },
	  defineProperty: function(name, pd){
		  sys.puts("myProxy.defineProperty called with '"+name+"'");
		  sys.inspect(pd);
		  return undefined;
	  },
	  getOwnPropertyNames: function() {
		  sys.puts("myProxy.getOwnPropertyNames called");
		  return Object.keys(myProxyProperties);
	  },
	  "delete": function(name) {
		  sys.puts("myProxy.delete called with '"+name+"'");
		  return (delete myProxyProperties[name]);
	  },
	  enumerate: function() {
		  sys.puts("myProxy.enumerate called");
		  return Object.keys(myProxyProperties);
	  },
	  fix: function() {
		  sys.puts("myProxy.fix called");
		  return Object.keys(myProxyProperties);
	  },
	  has: function (name) {
		sys.puts("myProxy.has "+name);
		return name in myProxyProperties;
	  },
	}, String.prototype),
	myProxyFunction = Proxy.createFunction({
	  getOwnPropertyDescriptor: function(name) {
		  sys.puts("myProxyFunction.getOwnPropertyDescriptor called with '"+name+"'");
		  return myFunctionProxyProperties[name];
	  },
	  getPropertyDescriptor: function(name) {
		  sys.puts("myProxyFunction.getPropertyDescriptor called with '"+name+"'");
		  return myFunctionProxyProperties[name];
	  },
	  defineProperty: function(name, pd){
		  sys.puts("myProxyFunction.defineProperty called with '"+name+"'");
		  sys.inspect(pd);
		  return undefined;
	  },
	  getOwnPropertyNames: function() {
		  sys.puts("myProxyFunction.getOwnPropertyNames called");
		  return Object.keys(myFunctionProxyProperties);
	  },
	  "delete": function(name) {
		  sys.puts("myProxyFunction.delete called with '"+name+"'");
		  return (delete myFunctionProxyProperties[name]);
	  },
	  enumerate: function() {
		  sys.puts("myProxyFunction.enumerate called");
		  return Object.keys(myFunctionProxyProperties);
	  },
	  fix: function() {
		  sys.puts("myProxyFunction.fix called");
		  return Object.keys(myFunctionProxyProperties);
	  },
	}, function() {
		sys.puts("callTrap called as " + (this instanceof myProxyFunction ? "callTrap" : "constructorTrap"));
		return myProxy;
	}),
	mySecondProxy = function(){};

mySecondProxy.prototype = myProxy;



//mySecondProxy.prototype.__defineProperty__("getter", function(){return "getter value"});
	

//myProxy.replace(/45/,"3");
fn = myProxyFunction();
cn = new myProxyFunction();
rn = new mySecondProxy();

for (p in rn)sys.puts(p+": "+rn[p]);
for (p in fn)sys.puts(p+": "+fn[p]);
for(p in cn)sys.puts(p+": "+cn[p]);

fn.tester = {};
cn.tester = {};

sys.puts("tester" in fn);
sys.puts("tester" in cn);
sys.puts("secondrn" in rn);

sys.puts(rn.second);

sys.puts(({}).hasOwnProperty.call(fn, "tester"));
sys.puts(({}).hasOwnProperty.call(cn, "tester"));
sys.puts(({}).hasOwnProperty.call(rn, "secondrn"));


delete fn.tester;
delete cn.tester;
delete rn.second;

    function expect_callback() {
        pending_callbacks += 1;
    }

    function receive_callback() {
        pending_callbacks -= 1;
    }


    sys.puts("Running tests...");

    ////
    // Test 1:
    sys.puts("Test 1 of " + total_tests + ": Inheritance by explicit setting test.");
    assert.ok(myProxy instanceof String);

    ////
    // Test 2:
    sys.puts("Test 2 of " + total_tests + ": Inheritance by prototype test.");
    assert.ok(new mySecondProxy() instanceof String);

    ////
    // Test 3:
    sys.puts("Test 3 of " + total_tests + ": Set value on Proxy.");
    mySecondProxy.prototype.second = "great";

    ////
    // Test 4:
    sys.puts("Test 3 of " + total_tests + ": Get value on Proxy instance.");
    assert.equal((new mySecondProxy()).second, "great");

    ////
    // Test 5: Somewhat simple synchronous string with looping.
    sys.puts("Test 5 of " + total_tests + ": Somewhat simple synchronous string with looping.");
    

    // assert that all callbacks were called within the alloted time and exit
    setTimeout(function () {
        assert.equal(0, pending_callbacks, "Some callbacks didn't call.");
        sys.puts("Every test passed.");
        process.exit();
    }, 100);
}());