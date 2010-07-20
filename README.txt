node-proxy is an implementation of Harmony Proxies <http://wiki.ecmascript.org/doku.php?id=harmony:proxies>,
that allows the developer to create "catch-all" property handlers for an object or a function.

Methods:

	Object create(handler [, proto ] ) throws Error, TypeError

	Function createFunction(handler, callTrap [, constructTrap ] ) throws Error, TypeError

	Boolean isTrapping(void) throws Error


Additional Methods (for ECMAScript 5 compatibliity):

	Boolean freeze(void) throws Error

	Boolean seal(void) throws Error

	Boolean preventExtensions(void) throws Error

	Boolean isFrozen(void) throws Error

	Boolean isSealed(void) throws Error

	Boolean isExtensible(void) throws Error

	PropertyDescriptor getOwnPropertyDescriptor(String name) throws Error, TypeError

	Boolean defineProperty(String name, PropertyDescriptor pd) throws Error, TypeError

	Boolean defineProperties(Object descriptors) throws Error, TypeError


More methods:

	Object hidden(Object obj, String name [, Object value] ) throws Error
		- Set or retrieve a hidden property on an Object

	Object clone(Object obj) throws Error
		- Create a shallow copy of an Object
