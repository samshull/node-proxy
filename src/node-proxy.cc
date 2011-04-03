/**
 *
 *
 *
 *    @author Sam Shull <http://samshull.blogspot.com/>
 *    @version 0.1
 *
 *    @copyright Copyright (c) 2009 Sam Shull <http://samshull.blogspot.com/>
 *    @license <http://www.opensource.org/licenses/mit-license.html>
 *
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *    THE SOFTWARE.
 *
 *
 *    CHANGES:
 */

#include "./node-proxy.h"

namespace v8 {  // this was much easier
    // fundamental traps
Persistent<String> NodeProxy::getOwnPropertyDescriptor;
Persistent<String> NodeProxy::getPropertyDescriptor;
Persistent<String> NodeProxy::getOwnPropertyNames;
Persistent<String> NodeProxy::getPropertyNames;
Persistent<String> NodeProxy::defineProperty;
Persistent<String> NodeProxy::delete_;
Persistent<String> NodeProxy::fix;
    // derived traps
Persistent<String> NodeProxy::has;
Persistent<String> NodeProxy::hasOwn;
Persistent<String> NodeProxy::get;
Persistent<String> NodeProxy::set;
Persistent<String> NodeProxy::enumerate;
Persistent<String> NodeProxy::keys;
    // string identifiers
Persistent<String> NodeProxy::callTrap;
Persistent<String> NodeProxy::constructorTrap;
Persistent<String> NodeProxy::value;
Persistent<String> NodeProxy::writable;
Persistent<String> NodeProxy::enumerable;
Persistent<String> NodeProxy::configurable;
Persistent<String> NodeProxy::name;
Persistent<String> NodeProxy::trapping;
Persistent<String> NodeProxy::sealed;
Persistent<String> NodeProxy::frozen;
Persistent<String> NodeProxy::extensible;
Persistent<String> NodeProxy::seal;
Persistent<String> NodeProxy::freeze;
Persistent<String> NodeProxy::preventExtensions;
Persistent<String> NodeProxy::isTrapping;
Persistent<String> NodeProxy::isSealed;
Persistent<String> NodeProxy::isFrozen;
Persistent<String> NodeProxy::isExtensible;
Persistent<String> NodeProxy::isProxy;
Persistent<String> NodeProxy::hidden;
Persistent<String> NodeProxy::hiddenPrivate;

/**
 *
 *
 *
 *
 */
NodeProxy::NodeProxy() {
}

/**
 *
 *
 *
 *
 */
NodeProxy::~NodeProxy() {
}

/**
 *    Validate a ProxyHandler Object for compliance with NodeProxy
 *
 *    For compliance with NodeProxy a ProxyHandler object MUST have:
 *        getPropertyDescriptor: a function that accepts one String argument
 *                that represents the name of the property being accessed
 *                and returns a PropertyDescriptor object or undefined
 *        enumerate: a function that returns an Array of zero or more Strings
 *                representing the names of the properties handled by the ProxyHandler
 *        delete: a function that accepts one String argument that represents the
 *                name of the property being deleted and returns a Boolean value
 *                indicating the success of the deletion
 *    Additionally, a ProxyHandler MAY have:
 *        get: a function that accepts one String argument that represents the name
 *                of the property and returns the value of the named property for the
 *                given property name
 *        set: a function that accepts two arguments the first is a String representing the
 *                name of the property and the second is a mixed type representing the desired
 *                value of the property
 *        has: a function that accepts one String argument that represents the name of the
 *                property and returns a Boolean value indicating that the ProxyHandler
 *                will handle that property
 *        name: (NodeProxy only) a String representing the name of the function created by createFunction
 *
 *    A ProxyHandler object used by NodeProxy does not have to implement the fully compliant specification
 *    presented in http://wiki.ecmascript.org/doku.php?id=harmony:proxies due to the nature of the V8 engine
 *
 *    @param ProxyHandler
 *    @returns Boolean | Error
 */
Handle<Value> NodeProxy::ValidateProxyHandler(Local<Object> handler) {
    HandleScope scope;

    if (!handler->Has(NodeProxy::enumerate)) {
        return THREXC("A ProxyHandler must have the property 'enumerate'");
    }

    if (!handler->Get(NodeProxy::enumerate)->IsFunction()) {
        return THREXC("ProxyHandler.enumerate must be a Function");
    }

    if (!handler->Has(NodeProxy::delete_)) {
        return THREXC("A ProxyHandler must have the property 'delete'");
    }

    if (!handler->Get(NodeProxy::delete_)->IsFunction()) {
        return THREXC("ProxyHandler.delete must be a Function");
    }

    if (!handler->Has(NodeProxy::fix)) {
        return THREXC("A ProxyHandler must have the property 'fix'");
    }

    if (!handler->Get(NodeProxy::fix)->IsFunction()) {
        return THREXC("ProxyHandler.fix must be a Function");
    }

    // these should be required, but they can't be
    // handled according to the spec
    if (handler->Has(NodeProxy::getPropertyDescriptor) &&
        !handler->Get(NodeProxy::getPropertyDescriptor)->IsFunction()) {
        return THREXC("getOwnPropertyDescriptor must be a Function");
    }

    if (handler->Has(NodeProxy::getOwnPropertyDescriptor) &&
        !handler->Get(NodeProxy::getOwnPropertyDescriptor)->IsFunction()) {
        return THREXC("ProxyHandler.getPropertyDescriptor must be a Function");
    }

    if (handler->Has(NodeProxy::defineProperty) &&
        !handler->Get(NodeProxy::defineProperty)->IsFunction()) {
        return THREXC("ProxyHandler.defineProperty must be a Function");
    }

    if (handler->Has(NodeProxy::getOwnPropertyNames) &&
        !handler->Get(NodeProxy::getOwnPropertyNames)->IsFunction()) {
        return THREXC("ProxyHandler.getOwnPropertyNames must be a Function");
    }
    // these properties are not required
    if (handler->Has(NodeProxy::get) &&
                     !handler->Get(NodeProxy::get)->IsFunction()) {
        return THREXC("ProxyHandler.get must be a Function.");
    }

    if (handler->Has(NodeProxy::set) &&
                     !handler->Get(NodeProxy::set)->IsFunction()) {
        return THREXC("ProxyHandler.set must be a Function.");
    }

    if (handler->Has(NodeProxy::has) &&
                     !handler->Get(NodeProxy::has)->IsFunction()) {
        return THREXC("ProxyHandler.has must be a Function.");
    }

    if (handler->Has(NodeProxy::hasOwn) &&
                     !handler->Get(NodeProxy::hasOwn)->IsFunction()) {
        return THREXC("ProxyHandler.hasOwn must be a Function.");
    }

    if (handler->Has(NodeProxy::enumerateOwn) &&
                     !handler->Get(NodeProxy::enumerateOwn)->IsFunction()) {
        return THREXC("ProxyHandler.enumerateOwn must be a Function.");
    }
    // gotta require some sort of property retriever
    if (!handler->Has(NodeProxy::getPropertyDescriptor) &&
                      !handler->Has(NodeProxy::get) &&
                      !handler->Has(NodeProxy::getOwnPropertyDescriptor)) {
        return THREXC("getOwnPropertyDescriptor must be a Function");
    }
    return True();
}

/**
 * Causing Error
 *
 *
Local<Value> NodeProxy::CorrectPropertyDescriptor(Local<Object> pd) {
    //HandleScope scope;

    Local<Value> undef = Local<Value>::New(Undefined());

    // pd->Set(NodeProxy::value,
               // pd->Has(NodeProxy::value) ?
               // pd->Get(NodeProxy::value) :
               // undef);
return Local<Value>::New(THREXC("CorrectPropertyDescriptor"));
    pd->Set(NodeProxy::writable,
            pd->Has(NodeProxy::writable) ?
            pd->Get(NodeProxy::writable)->ToBoolean() :
            True());

    pd->Set(NodeProxy::enumerable,
            pd->Has(NodeProxy::enumerable) ?
            pd->Get(NodeProxy::enumerable)->ToBoolean() :
            True());

    pd->Set(NodeProxy::configurable,
            pd->Has(NodeProxy::configurable) ?
            pd->Get(NodeProxy::configurable)->ToBoolean() :
            True());

    if (pd->Has(NodeProxy::get)) {
        Local<Value> getter = pd->Get(NodeProxy::get);
        pd->Set(NodeProxy::get, getter->IsFunction() ? getter : undef);
    } else {
        pd->Set(NodeProxy::get, undef);
    }

    if (pd->Has(NodeProxy::set)) {
        Local<Value> setter = pd->Get(NodeProxy::set);
        pd->Set(NodeProxy::set, setter->IsFunction() ? setter : undef);
    } else {
        pd->Set(NodeProxy::set, undef);
    }
    return pd;
}

/**
 *    Used for creating a shallow copy of an object
 *
 *
 *    @param mixed
 *    @returns mixed
 *    @throws Error
 */
Handle<Value> NodeProxy::Clone(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        return THREXC("clone requires at least one (1) argument.");
    }

    if (args[0]->IsString()) {
        return args[0]->ToObject()->Clone()->ToString();

    } else if (args[0]->IsBoolean()) {
        return args[0]->ToObject()->Clone()->ToBoolean();

    } else if (args[0]->IsNumber()
                || args[0]->IsInt32()
                || args[0]->IsUint32()) {
        return args[0]->ToObject()->Clone()->ToNumber();

    } else if (args[0]->IsArray()) {
        return Local<Array>::Cast(args[0]->ToObject()->Clone());

    } else if (args[0]->IsDate()) {
        return Local<Date>::Cast(args[0]->ToObject()->Clone());

    } else if (args[0]->IsFunction()) {
        return Local<Function>::Cast(args[0])->Clone();

    } else if (args[0]->IsNull()) {
        return Local<Value>::New(Null());

    } else if (args[0]->IsUndefined()) {
        return Local<Value>::New(Undefined());

    } else if (args[0]->IsObject()) {
        return args[0]->ToObject()->Clone();
    }

    return THREXC("clone cannot determine the type of the argument.");
}

/**
 *    Set or Retrieve the value of a hidden
 *    property on a given object
 *    Passing two arguments to this function
 *    returns the value of the hidden property
 *    While passing three arguments to this function
 *    results in the setting of the hidden property
 *    and returns a Boolean value indicating successful
 *    setting of value
 *
 *    @param Object
 *    @param String name
 *    @param mixed value - optional
 *    @returns mixed
 *    @throws Error
 */
Handle<Value> NodeProxy::Hidden(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 2) {
        return THREXC("hidden requires at least two (2) arguments.");
    }

    Local<Object> obj = args[0]->ToObject();

    if (args.Length() < 3) {
        return obj->GetHiddenValue(
                    String::Concat(NodeProxy::hidden,
                                   args[1]->ToString()));
    }

    return Boolean::New(
                obj->SetHiddenValue(String::Concat(NodeProxy::hidden,
                                                   args[1]->ToString()),
                args[2]));
}

/**
 *    Set the prototype of an object
 *
 *    @param Object
 *    @param Object
 *    @returns Boolean
 *    @throws Error
 */
Handle<Value> NodeProxy::SetPrototype(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 2) {
        return THREXC("setPrototype requires at least two (2) arguments.");
    }
    return Boolean::New(args[0]->ToObject()->SetPrototype(args[1]));
}

/**
 *    Determine if an Object was created by Proxy
 *
 *    @param Object
 *    @returns Boolean
 */
Handle<Value> NodeProxy::IsProxy(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 1) {
        return THREXC("isProxy requires at least one (1) argument.");
    }

    Local<Object> obj = args[0]->ToObject();

    if (obj->InternalFieldCount() > 0) {
        Local<Value> temp = obj->GetInternalField(0);

        if (!temp.IsEmpty() && temp->IsObject()) {
            Handle<Value> ret = ValidateProxyHandler(temp->ToObject());
            return Boolean::New(ret->IsBoolean() && ret->BooleanValue());
        }
    }

    return False();
}

/**
 *    Create an object that has ProxyHandler intercepts attached and
 *    optionally implements the prototype of another object
 *
 *    * ProxyHandler intercepts override the property handlers for any
 *    * given prototype. So, the ProxyHandler will be invoked for access
 *    * to the prototype's properties as well
 *
 *    @param ProxyHandler - @see NodeProxy::ValidateProxyHandler
 *    @param Object - optional, the prototype object to implement
 *    @returns Object
 *    @throws Error, TypeError
 */
Handle<Value> NodeProxy::Create(const Arguments& args) {
    HandleScope scope;

    Local<Object> proxyHandler;

    if (args.Length() < 1) {
        return THREXC("create requires at least one (1) argument.");
    }

    if (!args[0]->IsObject()) {
        return THR_TYPE_ERROR(
                "create requires the first argument to be an Object.");
    }

    // cloning here allows maintaining reference to original functions
    proxyHandler = args[0]->ToObject()->Clone();

    if (args.Length() > 1 && !args[1]->IsObject()) {
        return THR_TYPE_ERROR(
                "create requires the second argument to be an Object.");
    }

    // manage locking states
    proxyHandler->SetHiddenValue(NodeProxy::trapping, True());
    proxyHandler->SetHiddenValue(NodeProxy::extensible, True());
    proxyHandler->SetHiddenValue(NodeProxy::sealed, False());
    proxyHandler->SetHiddenValue(NodeProxy::frozen, False());

    Local<ObjectTemplate> temp = ObjectTemplate::New();

    temp->SetInternalFieldCount(1);

    // named property handlers
    temp->SetNamedPropertyHandler(GetNamedProperty,
                                  SetNamedProperty,
// different versions of V8 require different return types
// 0.1.97 is where the switch occurred in v8,
// but NODE_*_VERSION wasn't added until 0.1.100
#ifndef NODE_MAJOR_VERSION
                                  QueryNamedProperty,
#elif PROXY_NODE_VERSION_AT_LEAST(0, 1, 98)
                                  QueryNamedPropertyInteger,
#else
                                  QueryNamedProperty,
#endif
                                  DeleteNamedProperty,
                                  EnumerateNamedProperties);

    // indexed property handlers
    temp->SetIndexedPropertyHandler(GetIndexedProperty,
                                    SetIndexedProperty,

// different versions of V8 require different return types
// 0.2.0 is where the switch occurred
#ifndef NODE_MAJOR_VERSION
                                  QueryIndexedProperty,
#elif PROXY_NODE_VERSION_AT_LEAST(0, 2, 0)
                                  QueryIndexedPropertyInteger,
#else
                                  QueryIndexedProperty,
#endif
                                    DeleteIndexedProperty);

    Local<Object> instance = temp->NewInstance();

    instance->SetInternalField(0, proxyHandler);

    if (args.Length() > 1) {
        instance->SetPrototype(args[1]);
    }

    return instance;
}

/**
 *    Create a function that has ProxyHandler intercepts attached and
 *    sets a call trap function for invokation as well as an optional
 *    constructor trap
 *
 *
 *    @param ProxyHandler - @see NodeProxy::ValidateProxyHandler
 *    @param Function - call trap
 *    @param Function - optional, constructor trap
 *    @returns Function
 *    @throws Error, TypeError
 */
Handle<Value> NodeProxy::CreateFunction(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 2) {
        return THREXC("createFunction requires at least two (2) arguments.");
    }

    if (!args[0]->IsObject()) {
        return THR_TYPE_ERROR(
            "createFunction requires the first argument to be an Object.");
    }
    // cloning here allows maintaining reference to original functions
    Local<Object> proxyHandler = args[0]->ToObject()->Clone();

    if (!args[1]->IsFunction()) {
        return THR_TYPE_ERROR(
            "createFunction requires the second argument to be a Function.");
    }

    if (args.Length() > 2 && !args[2]->IsFunction()) {
        return THR_TYPE_ERROR(
            "createFunction requires the second argument to be a Function.");
    }

    proxyHandler->SetHiddenValue(NodeProxy::callTrap, args[1]);
    proxyHandler->SetHiddenValue(NodeProxy::constructorTrap,
                                 args.Length() > 2
                                 ? args[2]
                                 : Local<Value>::New(Undefined()));
    // manage locking states
    proxyHandler->SetHiddenValue(NodeProxy::trapping, True());
    proxyHandler->SetHiddenValue(NodeProxy::extensible, True());
    proxyHandler->SetHiddenValue(NodeProxy::sealed, False());
    proxyHandler->SetHiddenValue(NodeProxy::frozen, False());


  Local<ObjectTemplate> instance = ObjectTemplate::New();
  instance->SetCallAsFunctionHandler(New, proxyHandler);
  instance->SetInternalFieldCount(1);

    instance->SetNamedPropertyHandler(GetNamedProperty,
                                      SetNamedProperty,
// different versions of V8 require different return types
// 0.1.97 is where the switch occurred in v8,
// but NODE_*_VERSION wasn't added until 0.1.100
#ifndef NODE_MAJOR_VERSION
                                      QueryNamedProperty,
#elif PROXY_NODE_VERSION_AT_LEAST(0, 1, 98)
                                      QueryNamedPropertyInteger,
#else
                                      QueryNamedProperty,
#endif
                                      DeleteNamedProperty,
                                      EnumerateNamedProperties);

    instance->SetIndexedPropertyHandler(GetIndexedProperty,
                                        SetIndexedProperty,
// different versions of V8 require different return types
// 0.2.0 is where the switch occurred
#ifndef NODE_MAJOR_VERSION
                                      QueryIndexedProperty,
#elif PROXY_NODE_VERSION_AT_LEAST(0, 2, 0)
                                      QueryIndexedPropertyInteger,
#else
                                      QueryIndexedProperty,
#endif
                                        DeleteIndexedProperty);

  assert(!V8::IsDead());
  Local<Object> fn = instance->NewInstance();
  fn->SetPrototype(args[1]->ToObject()->GetPrototype());

  assert(fn->HasNamedLookupInterceptor());
  assert(fn->HasIndexedLookupInterceptor());
  assert(fn->InternalFieldCount() > 0);

  fn->SetInternalField(0, proxyHandler);

    return fn;
}

/**
 *    Used as a handler for freeze, seal, and preventExtensions
 *    to lock the state of a Proxy created object
 *
 *    @param Object
 *    @returns Boolean
 *    @throws Error, TypeError
 */
Handle<Value> NodeProxy::Freeze(const Arguments& args) {
    HandleScope scope;

    Local<String> name = args.Callee()->GetName()->ToString();

    if (args.Length() < 1) {
        return THREXCW(String::Concat(name,
                       String::New(" requires at least one (1) argument.")));
    }

    Local<Object> obj = args[0]->ToObject();

    if (obj->InternalFieldCount() < 1) {
        return THR_TYPE_ERROR(
            "Locking functions expect first "
            "argument to be intialized by Proxy");
    }

    Local<Value> hide = obj->GetInternalField(0);

    if (hide.IsEmpty() || !hide->IsObject()) {
        return THR_TYPE_ERROR(
            "Locking functions expect first "
            "argument to be intialized by Proxy");
    }

    Local<Object> handler = hide->ToObject();

    // if the object already meets the requirements of the function call
    if (name->Equals(NodeProxy::freeze)) {
        if (handler->GetHiddenValue(NodeProxy::frozen)->BooleanValue()) {
            return True();
        }

    } else if (name->Equals(NodeProxy::seal)) {
        if (handler->GetHiddenValue(NodeProxy::sealed)->BooleanValue()) {
            return True();
        }

    } else if (name->Equals(NodeProxy::preventExtensions)) {
        if (handler->GetHiddenValue(NodeProxy::extensible)->BooleanValue()) {
            return True();
        }
    }

    // if this object is not trapping, just set the appropriate parameters
    if (!handler->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
        if (name->Equals(NodeProxy::freeze)) {
            handler->SetHiddenValue(NodeProxy::frozen, True());
            handler->SetHiddenValue(NodeProxy::sealed, True());
            handler->SetHiddenValue(NodeProxy::extensible, False());
            return True();

        } else if (name->Equals(NodeProxy::seal)) {
            handler->SetHiddenValue(NodeProxy::sealed, True());
            handler->SetHiddenValue(NodeProxy::extensible, False());
            return True();

        } else if (name->Equals(NodeProxy::preventExtensions)) {
            handler->SetHiddenValue(NodeProxy::extensible, False());
            return True();
        }
    }

    // Harmony Proxy handling of fix
    Local<Function> fix = Local<Function>::Cast(handler->Get(NodeProxy::fix));
    Local<Value> argv[0];

    Local<Value> pieces = fix->Call(args[0]->ToObject(), 0, argv);

    if (pieces.IsEmpty() || !pieces->IsObject()) {
        return THR_TYPE_ERROR("Cannot lock object.");
    }

    Local<Object> parts = pieces->ToObject();

    // set the appropriate parameters
    if (name->Equals(NodeProxy::freeze)) {
        parts->SetHiddenValue(NodeProxy::frozen, True());
        parts->SetHiddenValue(NodeProxy::sealed, True());
        parts->SetHiddenValue(NodeProxy::extensible, False());

    } else if (name->Equals(NodeProxy::seal)) {
        parts->SetHiddenValue(NodeProxy::sealed, True());
        parts->SetHiddenValue(NodeProxy::extensible, False());

    } else if (name->Equals(NodeProxy::preventExtensions)) {
        parts->SetHiddenValue(NodeProxy::extensible, False());
    }

    parts->SetHiddenValue(NodeProxy::trapping, False());

    // overwrite the handler, making handler available for GC
    obj->SetInternalField(0, parts);

    return True();
}

/**
 *    Used as a handler for determining isTrapped,
 *    isFrozen, isSealed, and isExtensible
 *
 *    @param Object
 *    @returns Boolean
 *    @throws Error, TypeError
 */
Handle<Value> NodeProxy::IsLocked(const Arguments& args) {
    HandleScope scope;

    Local<String> name = args.Callee()->GetName()->ToString();

    if (args.Length() < 1) {
        return THREXCW(String::Concat(name,
                       String::New(" requires at least one (1) argument.")));
    }

    Local<Object> arg = args[0]->ToObject();

    if (arg->InternalFieldCount() < 1) {
        return THR_TYPE_ERROR(
           "Locking functions expect first argument "
           "to be intialized by Proxy");
    }

    Local<Value> hide = arg->GetInternalField(0);

    if (hide.IsEmpty() || !hide->IsObject()) {
        return THR_TYPE_ERROR(
            "Locking functions expect first argument "
            "to be intialized by Proxy");
    }

    Local<Object> obj = hide->ToObject();

    if (name->Equals(NodeProxy::isExtensible)) {
        return obj->GetHiddenValue(NodeProxy::extensible)->ToBoolean();

    } else if (name->Equals(NodeProxy::isSealed)) {
        return obj->GetHiddenValue(NodeProxy::sealed)->ToBoolean();

    } else if (name->Equals(NodeProxy::isTrapping)) {
        return obj->GetHiddenValue(NodeProxy::trapping)->ToBoolean();

    } else if (name->Equals(NodeProxy::isFrozen)) {
        return obj->GetHiddenValue(NodeProxy::frozen)->ToBoolean();
    }

    return False();
}

/**
 *    Part of ECMAScript 5, but only for use on
 *    Objects and Functions created by Proxy
 *
 *    @param Object
 *    @param String - the name of the property
 *    @returns PropertyDescriptor
 *    @throws Error, TypeError
 */
Handle<Value> NodeProxy::GetOwnPropertyDescriptor(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 2) {
        return THREXC("getOwnPropertyDescriptor requires "
                "at least two (2) arguments.");
    }

    if (!args[1]->IsString() && !args[1]->IsNumber()) {
        return THR_TYPE_ERROR("getOwnPropertyDescriptor requires "
                   "the second argument to be a String or a Number.");
    }

    Local<Object> obj = args[0]->ToObject();
    Local<String> name = args[1]->ToString();

    if (obj->InternalFieldCount() < 1) {
        return THR_TYPE_ERROR("getOwnPropertyDescriptor expects "
                      "first argument to be intialized by Proxy");
    }

    Local<Value> temp = obj->GetInternalField(0);

    if (temp.IsEmpty() || !temp->IsObject()) {
        return THR_TYPE_ERROR("getOwnPropertyDescriptor expects "
                      "first argument to be intialized by Proxy");
    }

    Local<Object> handler = temp->ToObject();

    if (!handler->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
        return handler->Get(name);
    }

    Local<Function> getOwn =
      Local<Function>::Cast(
           handler->Get(NodeProxy::getOwnPropertyDescriptor));

    Local<Value> argv[1] = {args[1]};
    return getOwn->Call(obj, 1, argv);
}

/**
 *    Part of ECMAScript 5, but only for use on
 *    Objects and Functions created by Proxy
 *
 *    @param Object
 *    @param String - the name of the property
 *    @param PropertyDescriptor
 *    @returns Boolean
 *    @throws Error, TypeError
 */
Handle<Value> NodeProxy::DefineProperty(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 3) {
        return THREXC("defineProperty requires at least three (3) arguments.");
    }

    if (!args[1]->IsString() && !args[1]->IsNumber()) {
        return THR_TYPE_ERROR("defineProperty requires the "
                              "second argument to be a String or a Number.");
    }

    if (!args[2]->IsObject()) {
        return THR_TYPE_ERROR("defineProperty requires the third argument "
                        "to be an Object of the type PropertyDescriptor.");
    }

    Local<Object> obj = args[0]->ToObject();

    if (obj->InternalFieldCount() < 1) {
        return THR_TYPE_ERROR("defineProperty expects first "
                              "argument to be intialized by Proxy");
    }

    Local<Value> temp = obj->GetInternalField(0);

    if (temp.IsEmpty() || !temp->IsObject()) {
        return THR_TYPE_ERROR("defineProperty expects first argument "
                              "to be intialized by Proxy");
    }

    Local<String> name = args[1]->ToString();
    Local<Object> handler = temp->ToObject();

    if (handler->GetHiddenValue(NodeProxy::sealed)->BooleanValue() ||
    !handler->Has(NodeProxy::defineProperty)) {
        return False();
    }

    if (!handler->GetHiddenValue(NodeProxy::extensible)->BooleanValue() &&
                !handler->Has(name)) {
        return False();
    }

    if (!handler->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
        Local<Object> desc = handler->Get(name)->ToObject();

        if (desc->Get(NodeProxy::configurable)->BooleanValue()) {
            return Boolean::New(
                     handler->Set(name, args[2]->ToObject()));
        }
        return False();
    }

    Local<Function> def = Local<Function>::Cast(
                                    handler->Get(NodeProxy::defineProperty));

    Local<Value> argv[2] = {args[1], args[2]->ToObject()};

    return def->Call(obj, 2, argv)->ToBoolean();
}

/**
 *    Part of ECMAScript 5, but only for use on
 *    Objects and Functions created by Proxy
 *
 *    @param Object
 *    @param Object - name/PropertyDescriptor pairs
 *    @returns Boolean
 *    @throws Error, TypeError
 */
Handle<Value> NodeProxy::DefineProperties(const Arguments& args) {
    HandleScope scope;

    if (args.Length() < 2) {
        return THREXC("defineProperty requires at least three (3) arguments.");
    }

    if (!args[1]->IsObject()) {
        return THR_TYPE_ERROR("defineProperty requires the third argument "
                         "to be an Object of the type PropertyDescriptor.");
    }

    Local<Object> obj = args[0]->ToObject();

    if (obj->InternalFieldCount() < 1) {
        return THR_TYPE_ERROR("defineProperty expects first "
                                "argument to be intialized by Proxy");
    }

    Local<Value> temp = obj->GetInternalField(0);

    if (!temp.IsEmpty() && temp->IsObject()) {
        Local<Object> props = args[1]->ToObject();
        Local<Object> handler = temp->ToObject();

        if (handler->GetHiddenValue(NodeProxy::sealed)->BooleanValue()) {
            return False();
        }

        bool extensible = handler->GetHiddenValue(
                                    NodeProxy::extensible)->BooleanValue();
        Local<Array> names = props->GetPropertyNames();
        uint32_t i = 0, l = names->Length();

        if (!handler->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
            for (;i < l; ++i) {
                Local<Object> name = names->CloneElementAt(i);

                if (handler->Has(name->ToString()) &&
                        handler->Get(name->ToString())->IsObject()
                ) {
                    Local<Object> tempObj =
                        handler->Get(name->ToString())->ToObject();

                    if (tempObj->Get(NodeProxy::configurable)->BooleanValue()) {
                        if (!handler->Set(name->ToString(),
                                          props->Get(name->ToString()))) {
                            return THREXCW(
                                String::Concat(
                                    String::New("Unable to define property: "),
                                    name->ToString()));
                        }
                    }
                } else {
                    return THREXCW(String::Concat(
                                    String::New("Unable to define property: "),
                                    name->ToString()));
                }
            }
            return True();
        }

        Local<Function> def =
            Local<Function>::Cast(handler->Get(NodeProxy::defineProperty));

        TryCatch firstTry;
        for (;i < l; ++i) {
            Local<Value> name = names->Get(i);

            if (extensible || obj->Has(name->ToString())) {
        Local<Value> pd = props->Get(name->ToString());
        Local<Value> argv[2] = {name, pd};
                def->Call(obj, 2, argv);

                if (firstTry.HasCaught()) {
                    return firstTry.ReThrow();
                }
            }
        }
        return True();
    }
    return False();
}

/**
 *    Function used for a constructor and invocation
 *    handler of a Proxy created function
 *    Calls the appropriate function attached when the Proxy was created
 *
 *    @param ...args
 *    @returns mixed
 *    @throws Error
 */
Handle<Value> NodeProxy::New(const Arguments& args) {
    HandleScope scope;

    if (args.Callee()->InternalFieldCount() < 1 && args.Data().IsEmpty()) {
        return THR_TYPE_ERROR("defineProperty expects first "
                                "argument to be intialized by Proxy");
    }

    Local<Value> info, ret,
                 data = args.Callee()->InternalFieldCount() > 0 ?
            args.Callee()->GetInternalField(0) : 
            args.Data();

    if (data.IsEmpty() || !data->IsObject()) {
        return THREXC("Invalid reference to Proxy#constructor");
    }

    Local<Function> fn;
    Local<Object> obj = data->ToObject();

    if (args.IsConstructCall()) {
        info = obj->GetHiddenValue(NodeProxy::constructorTrap);

        if (!info.IsEmpty() && info->IsFunction()) {
            fn = Local<Function>::Cast(info);
        } else {
            fn = Local<Function>::Cast(
                    obj->GetHiddenValue(NodeProxy::callTrap));
        }
    } else {
        fn = Local<Function>::Cast(obj->GetHiddenValue(NodeProxy::callTrap));
    }

    int i = 0, l = args.Length();
    Local<Value>* argv = new Local<Value>[l];

    for (; i < l; ++i) {
        argv[i] = args[i];
    }

    ret = fn->Call(args.This(), args.Length(), argv);

    if (args.IsConstructCall()) {
        if (!ret.IsEmpty()) {
            return ret;
        }
        return args.This();
    }
    return ret;
}

/**
 *    Invoked for accessing the named properties of an object
 *
 *
 *
 */
Handle<Value> NodeProxy::GetNamedProperty(Local<String> name,
                                          const AccessorInfo &info) {
    HandleScope scope;

    if (info.This()->InternalFieldCount() < 1 || info.Data().IsEmpty()) {
        return THR_TYPE_ERROR("SetNamedProperty intercepted "
                                "by non-Proxy object");
    }

    Local<Value> argv[2] = {info.This(), name},
                 argv1[1] = {name},
                 temp, ret, undef,
                 data = info.This()->InternalFieldCount() > 0 ?
            info.This()->GetInternalField(0) :
            info.Data();

    if (!data.IsEmpty() && data->IsObject()) {
        Local<Function> fn;
        Local<Object> obj = data->ToObject();

        // if the Proxy isn't trapping, return
        // the value set on the property descriptor
        if (!obj->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
            if (obj->Has(name)) {
                temp = obj->Get(name);

                if (temp.IsEmpty() || !temp->IsObject()) {
                    return undef;
                }

                Local<Object> tempObj = temp->ToObject();

                if (tempObj->Has(NodeProxy::get)) {
                    Local<Value> get = tempObj->Get(NodeProxy::get);

                    if (get->IsFunction()) {
                        Local<Function> fn =
                            Local<Function>::Cast(get);

                        return fn->Call(info.This(), 2, argv);
                    }
                }
                return tempObj->Get(NodeProxy::value);
            }
            return undef;
        }

        TryCatch firstTry;

        if (obj->Has(NodeProxy::get)) {
            temp = obj->Get(NodeProxy::get);

            if (!temp.IsEmpty() && temp->IsFunction()) {
                fn = Local<Function>::Cast(temp);

                ret = fn->Call(info.This(), 2, argv);

                if (firstTry.HasCaught()) {
                    return firstTry.ReThrow();
                }

                if (ret.IsEmpty() || ret->IsUndefined()) {
                    return undef;
                }
                return ret;
            }
        }

        if (obj->Has(NodeProxy::getOwnPropertyDescriptor)) {
            temp = obj->Get(NodeProxy::getOwnPropertyDescriptor);
            fn = Local<Function>::Cast(temp);
            Local<Value> desc = fn->Call(info.This(), 1, argv1);

            if (firstTry.HasCaught()) {
                return firstTry.ReThrow();
            }

            if (!desc.IsEmpty() && desc->IsObject()) {
                Local<Object> prop = desc->ToObject();

                if (prop->Has(NodeProxy::get) &&
                    prop->Get(NodeProxy::get)->IsFunction()) {
                    temp = prop->Get(NodeProxy::get);
                    fn = Local<Function>::Cast(temp);
                    return fn->Call(info.This(), 1, argv1);
                } else if (prop->Has(NodeProxy::value)) {
                    return prop->Get(NodeProxy::value);
                }
            }
        }

        if (obj->Has(NodeProxy::getPropertyDescriptor)) {
            temp = obj->Get(NodeProxy::getPropertyDescriptor);
            fn = Local<Function>::Cast(temp);
            Local<Value> desc = fn->Call(info.This(), 1, argv1);

            if (firstTry.HasCaught()) {
                return firstTry.ReThrow();
            }

            if (!desc.IsEmpty() && desc->IsObject()) {
                Local<Object> prop = desc->ToObject();

                if (prop->Has(NodeProxy::get) &&
                    prop->Get(NodeProxy::get)->IsFunction()
                ) {
                    temp = prop->Get(NodeProxy::get);
                    fn = Local<Function>::Cast(temp);
                    return fn->Call(info.This(), 1, argv1);

                } else if (prop->Has(NodeProxy::value)) {
                    return prop->Get(NodeProxy::value);
                }
            }
        }
    }
    return undef;
}

/**
 *    Invoked for setting the named properties of an object
 *
 *
 *
 */
Handle<Value> NodeProxy::SetNamedProperty(Local<String> name,
                                          Local<Value> value,
                                          const AccessorInfo &info) {
    HandleScope scope;

    if (info.This()->InternalFieldCount() < 1 || info.Data().IsEmpty()) {
        return THR_TYPE_ERROR("SetNamedProperty intercepted "
                                "by non-Proxy object");
    }

    Local<Function> fn;
    Local<Value> argv2[2] = {name, value},
                 undef, temp,
                 data = info.This()->InternalFieldCount() > 0 ?
            info.This()->GetInternalField(0) :
             info.Data();

    if (!data.IsEmpty() && data->IsObject()) {
        Local<Object> obj = data->ToObject();
        TryCatch firstTry;
        // if the Proxy isn't trapping, return the
        // value set on the property descriptor
        if (!obj->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
            if (obj->GetHiddenValue(NodeProxy::extensible)->BooleanValue() ||
                obj->Has(name)
            ) {
                temp = obj->Get(name);

                if (temp.IsEmpty() || !temp->IsObject()) {
                    return undef;
                }

                Local<Object> tempObj = temp->ToObject();

                if (!tempObj->GetHiddenValue(
                            NodeProxy::writable)->BooleanValue()
                ) {
                    return THREXCW(
                                String::Concat(
                                    String::New("In accessible property: "),
                                          name));
                }

                if (tempObj->Has(NodeProxy::set)) {
                    Local<Value> set = tempObj->Get(NodeProxy::set);

                    if (set->IsFunction()) {
                        Local<Function> fn = Local<Function>::Cast(set);
                        fn->Call(info.This(), 2, argv2);

                        if (firstTry.HasCaught()) {
                            return firstTry.ReThrow();
                        }

                        return value;
                    }
                }

                if (tempObj->Set(NodeProxy::value, value)) {
                    return value;
                }
                return undef;
            }
            return undef;
        }

        // does the ProxyHandler have a set method?
        if (obj->Has(NodeProxy::set)) {
            temp = obj->Get(NodeProxy::set);

            if (!temp.IsEmpty() && temp->IsFunction()) {
                fn = Local<Function>::Cast(temp);
                Local<Value> argv3[3] = {info.This(), name, value};
                fn->Call(info.This(), 3, argv3);

                if (firstTry.HasCaught()) {
                    return firstTry.ReThrow();
                }

                return value;
            }
        }

        if (obj->Has(NodeProxy::getOwnPropertyDescriptor)) {
            temp = obj->Get(NodeProxy::getOwnPropertyDescriptor);
            fn = Local<Function>::Cast(temp);
            Local<Value> argv[1] = {name};
            Local<Value> desc = fn->Call(info.This(), 1, argv);

            if (firstTry.HasCaught()) {
                return firstTry.ReThrow();
            }

            if (!desc.IsEmpty() && desc->IsObject()) {
                Local<Object> prop = desc->ToObject();

                // if the PropertyDescriptor has a set method
                if (prop->Has(NodeProxy::set) &&
                    prop->Get(NodeProxy::set)->IsFunction()) {
                    temp = prop->Get(NodeProxy::set);
                    fn = Local<Function>::Cast(temp);
                    fn->Call(info.This(), 2, argv2);

                    if (firstTry.HasCaught()) {
                        return firstTry.ReThrow();
                    }

                    return value;

                // otherwise change value if writable
                } else if (prop->Has(NodeProxy::writable) &&
                            prop->Get(NodeProxy::writable)->BooleanValue()) {
                    if (prop->Set(NodeProxy::value, value)) {
                        return value;
                    }
                }
            }
        }

        if (obj->Has(NodeProxy::getPropertyDescriptor)) {
            temp = obj->Get(NodeProxy::getPropertyDescriptor);
            fn = Local<Function>::Cast(temp);
            Local<Value> argv[1] = {name};
            Local<Value> desc = fn->Call(info.This(), 1, argv);

            if (firstTry.HasCaught()) {
                return firstTry.ReThrow();
            }

            if (!desc.IsEmpty() && desc->IsObject()) {
                Local<Object> prop = desc->ToObject();

                // if the PropertyDescriptor has a set method
                if (prop->Has(NodeProxy::set) &&
                    prop->Get(NodeProxy::set)->IsFunction()) {
                    temp = prop->Get(NodeProxy::set);
                    fn = Local<Function>::Cast(temp);
                    fn->Call(info.This(), 2, argv2);

                    if (firstTry.HasCaught()) {
                        return firstTry.ReThrow();
                    }

                    return value;

                // otherwise change value if writable
                } else if (prop->Has(NodeProxy::writable) &&
                            prop->Get(NodeProxy::writable)->BooleanValue()) {
                    if (prop->Set(NodeProxy::value, value)) {
                        return value;
                    }
                }
            }
        }
    }
    return undef;
}

/**
 *    Invoked for determining if an object has a specific property
 *
 *
 *
 */
Handle<Boolean> NodeProxy::QueryNamedProperty(Local<String> name,
                                              const AccessorInfo &info) {
    HandleScope scope;

  if (name->Equals(String::New("test"))) {
    assert(info.Data().IsEmpty() || !info.Data()->IsObject() || !info.Data()->ToObject()->Has(NodeProxy::getOwnPropertyDescriptor));
  }

    if (info.This()->InternalFieldCount() < 1 || !info.Data().IsEmpty()) {
        Local<Value> argv[1] = {name}, temp;
        Local<Value> data = info.This()->InternalFieldCount() > 0 ?
              info.This()->GetInternalField(0) :
               info.Data();
        Local<Function> fn;

        if (!data.IsEmpty() && data->IsObject()) {
            Local<Object> obj = data->ToObject();

            // if the Proxy isn't trapping,
            // return the value set on the property descriptor
            if (!obj->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
                return Boolean::New(obj->Has(name));
            }

            if (obj->Has(NodeProxy::getOwnPropertyDescriptor)) {
                temp = obj->Get(NodeProxy::getOwnPropertyDescriptor);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    fn = Local<Function>::Cast(temp);
                    temp = fn->Call(info.This(), 1, argv);

                    if (temp->IsBoolean()) {
                        return temp->ToBoolean();
                    }
                }
            }

            if (obj->Has(NodeProxy::getPropertyDescriptor)) {
                temp = obj->Get(NodeProxy::getPropertyDescriptor);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    fn = Local<Function>::Cast(temp);
                    temp = fn->Call(info.This(), 1, argv);

                    if (temp->IsBoolean()) {
                        return temp->ToBoolean();
                    }
                }
            }

            // check the ProxyHandler for the has method
            if (obj->Has(NodeProxy::hasOwn)) {
                temp = obj->Get(NodeProxy::hasOwn);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    fn = Local<Function>::Cast(temp);
                    temp = fn->Call(info.This(), 1, argv);

                    if (temp->IsBoolean()) {
                        return temp->ToBoolean();
                    }
                }
            }

            // check the ProxyHandler for the has method
            if (obj->Has(NodeProxy::has)) {
                temp = obj->Get(NodeProxy::has);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    fn = Local<Function>::Cast(temp);
                    temp = fn->Call(info.This(), 1, argv);

                    if (temp->IsBoolean()) {
                        return temp->ToBoolean();
                    }
                }
            }
        }
    }
    return False();
}

/**
 *    Invoked for determining if an object has a specific property
 *
 *
 *
 */
Handle<Integer> NodeProxy::QueryNamedPropertyInteger(Local<String> name,
                                                     const AccessorInfo &info) {
    HandleScope scope;

    Local<Integer> DoesntHavePropertyResponse;
    Local<Integer> HasPropertyResponse = Integer::New(None);

    if (info.This()->InternalFieldCount() > 0 || !info.Data().IsEmpty()) {
        Local<Value> temp, 
          data = info.This()->InternalFieldCount() > 0 ?
              info.This()->GetInternalField(0) :
               info.Data();
        Local<Function> fn;

        if (!data.IsEmpty() && data->IsObject()) {
            Local<Object> obj = data->ToObject();

            // if the Proxy isn't trapping,
            // return the value set on the property descriptor
            if (!obj->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
                if (obj->Has(name)) {
                    Local<Value> pd = obj->Get(name);

                    if (pd->IsObject()) {
                        return GetPropertyAttributeFromPropertyDescriptor(
                                        pd->ToObject());
                    }
                    return HasPropertyResponse;
                }
                return DoesntHavePropertyResponse;
            }

            Local<Value> argv[1] = {name};
            TryCatch firstTry;

            if (obj->Has(NodeProxy::getOwnPropertyDescriptor)) {
                temp = obj->Get(NodeProxy::getOwnPropertyDescriptor);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    fn = Local<Function>::Cast(temp);
                    temp = fn->Call(info.This(), 1, argv);

                    if (firstTry.HasCaught()) {
                        firstTry.ReThrow();
                        return DoesntHavePropertyResponse;
                    }

                    if (!temp.IsEmpty() && temp->IsObject()) {
                        return GetPropertyAttributeFromPropertyDescriptor(
                                        temp->ToObject());
                    }
                }
            }

            if (obj->Has(NodeProxy::getPropertyDescriptor)) {
                temp = obj->Get(NodeProxy::getPropertyDescriptor);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    fn = Local<Function>::Cast(temp);
                    temp = fn->Call(info.This(), 1, argv);

                    if (firstTry.HasCaught()) {
                        firstTry.ReThrow();
                        return DoesntHavePropertyResponse;
                    }

                    if (!temp.IsEmpty() && temp->IsObject()) {
                        return GetPropertyAttributeFromPropertyDescriptor(
                                            temp->ToObject());
                    } else if (temp->IsUndefined()) {
                        return DoesntHavePropertyResponse;
                    }
                }
            }

            if (obj->Has(NodeProxy::hasOwn)) {
                temp = obj->Get(NodeProxy::hasOwn);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    fn = Local<Function>::Cast(temp);
                    temp = fn->Call(info.This(), 1, argv);

                    if (firstTry.HasCaught()) {
                        firstTry.ReThrow();
                        return DoesntHavePropertyResponse;
                    }

                    if (temp->IsBoolean()) {
                        return HasPropertyResponse;
                    }
                }
            }

            if (obj->Has(NodeProxy::has)) {
                temp = obj->Get(NodeProxy::has);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    fn = Local<Function>::Cast(temp);
                    temp = fn->Call(info.This(), 1, argv);

                    if (firstTry.HasCaught()) {
                        firstTry.ReThrow();
                        return DoesntHavePropertyResponse;
                    }

                    if (temp->IsBoolean()) {
                        return HasPropertyResponse;
                    }
                }
            }

            if (firstTry.HasCaught()) {
                firstTry.ReThrow();
                return DoesntHavePropertyResponse;
            }
        }
    }
    return DoesntHavePropertyResponse;
}

/**
 *    Find the appropriate PropertyAttribute
 *    for a given PropertyDescriptor object
 *
 *
 */
Handle<Integer>
NodeProxy::GetPropertyAttributeFromPropertyDescriptor(Local<Object> pd) {
    HandleScope scope;
    uint32_t ret = None;

    if (pd->Has(NodeProxy::configurable) &&
                !pd->Get(NodeProxy::configurable)->BooleanValue()) {
        // return Integer::New(DontDelete);
        ret &= DontDelete;
    }

    if (pd->Has(NodeProxy::enumerable) &&
                       !pd->Get(NodeProxy::enumerable)->BooleanValue()) {
        // return Integer::New(DontEnum);
        ret &= DontEnum;
    }

    if (pd->Has(NodeProxy::writable) &&
                       !pd->Get(NodeProxy::writable)->BooleanValue()) {
       // return Integer::New(ReadOnly);
       ret &= ReadOnly;
    }

    return Integer::New(ret);
}

/**
 *    Invoked when deleting the named property of an object
 *
 *
 *
 */
Handle<Boolean> NodeProxy::DeleteNamedProperty(Local<String> name,
                                               const AccessorInfo &info) {
    HandleScope scope;

    if (info.This()->InternalFieldCount() > 0 || !info.Data().IsEmpty()) {
        Local<Value> temp, 
          data = info.This()->InternalFieldCount() > 0 ?
              info.This()->GetInternalField(0) :
               info.Data();

        if (!data.IsEmpty() && data->IsObject()) {
            Local<Object> obj = data->ToObject();
            // if the Proxy isn't trapping,
            // return the value set on the property descriptor
            if (!obj->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
                if (!obj->GetHiddenValue(NodeProxy::frozen)->BooleanValue()) {
                    if (obj->Has(name)) {
                        temp = obj->Get(name);

                        if (temp->IsObject()) {
                            Local<Object> tempObj = temp->ToObject();

                            if (tempObj->Has(NodeProxy::configurable) &&
                                tempObj->Get(
                                    NodeProxy::configurable)->BooleanValue()
                            ) {
                                return Boolean::New(obj->Delete(name));
                            }
                        }
                    }
                }
                return False();
            }

            if (obj->Has(NodeProxy::delete_)) {
                temp = obj->Get(NodeProxy::delete_);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    Local<Function> fn = Local<Function>::Cast(temp);
                    Local<Value> argv[1] = {name};
                    return fn->Call(info.This(), 1, argv)->ToBoolean();
                }
            }
        }
    }
    return False();
}

/**
 *    Invoked for enumerating all properties of an object
 *
 *
 *
 */
Handle<Array> NodeProxy::EnumerateNamedProperties(const AccessorInfo &info) {
    HandleScope scope;

    if (info.This()->InternalFieldCount() > 0 || !info.Data().IsEmpty()) {
        Local<Value> data = info.This()->InternalFieldCount() > 0 ?
              info.This()->GetInternalField(0) :
               info.Data();

        if (!data.IsEmpty() && data->IsObject()) {
            Local<Object> obj = data->ToObject();
            // if the Proxy isn't trapping,
            // return the value set on the property descriptor
            if (!obj->GetHiddenValue(NodeProxy::trapping)->BooleanValue()) {
                return obj->GetPropertyNames();
            }

            if (obj->Has(NodeProxy::enumerate)) {
                Local<Value> temp = obj->Get(NodeProxy::enumerate);

                if (!temp.IsEmpty() && temp->IsFunction()) {
                    Local<Function> fn = Local<Function>::Cast(temp);
                    Local<Value> argv[0];
                    temp = fn->Call(info.This(), 0, argv);

                    if (temp->IsArray()) {
                        return Local<Array>::Cast(temp->ToObject());
                    }
                }
            }
        }
    }
    return Array::New();
}

/**
 *    Invoked for accessing the given indexed property of an object
 *
 *
 *
 */
Handle<Value> NodeProxy::GetIndexedProperty(uint32_t index,
                                            const AccessorInfo &info) {
    HandleScope scope;

    return GetNamedProperty(Local<String>::Cast(
                                Integer::NewFromUnsigned(index)),
                            info);
}

/**
 *    Invoked for setting the given indexed property of an object
 *
 *
 *
 */
Handle<Value> NodeProxy::SetIndexedProperty(uint32_t index,
                                            Local<Value> value,
                                            const AccessorInfo &info) {
    HandleScope scope;

    return SetNamedProperty(Local<String>::Cast(
                                    Integer::NewFromUnsigned(index)),
                            value,
                            info);
}

/**
 *    Invoked for determining if an object has a given indexed property
 *
 *
 *
 */
Handle<Boolean> NodeProxy::QueryIndexedProperty(uint32_t index,
                                                const AccessorInfo &info) {
    HandleScope scope;

    return QueryNamedProperty(
                Local<String>::Cast(Integer::NewFromUnsigned(index)),
                            info);
}
Handle<Integer> NodeProxy::QueryIndexedPropertyInteger(uint32_t index,
                                                const AccessorInfo &info) {
    HandleScope scope;

    return QueryNamedPropertyInteger(
                Local<String>::Cast(Integer::NewFromUnsigned(index)),
                info);
}

/**
 *    Invoked for deleting a given indexed property
 *
 *
 *
 */
Handle<Boolean> NodeProxy::DeleteIndexedProperty(uint32_t index,
                                                 const AccessorInfo &info) {
    HandleScope scope;

    return DeleteNamedProperty(
                    Local<String>::Cast(Integer::NewFromUnsigned(index)),
                    info);
}

/**
 *    Initialize the NodeProxy Strings and functions
 *
 *
 *
 */
void NodeProxy::Init(Handle<Object> target) {
    HandleScope scope;

// required properties
    NodeProxy::getOwnPropertyDescriptor =
        PROXY_NODE_PSYMBOL("getOwnPropertyDescriptor");
    NodeProxy::getPropertyDescriptor =
        PROXY_NODE_PSYMBOL("getPropertyDescriptor");
    NodeProxy::getOwnPropertyNames =
        PROXY_NODE_PSYMBOL("getOwnPropertyNames");
    NodeProxy::getPropertyNames =
        PROXY_NODE_PSYMBOL("getPropertyNames");
    NodeProxy::defineProperty =
        PROXY_NODE_PSYMBOL("defineProperty");
    NodeProxy::delete_ =
        PROXY_NODE_PSYMBOL("delete");
    NodeProxy::fix =
        PROXY_NODE_PSYMBOL("fix");
// optional properties
    NodeProxy::has =
        PROXY_NODE_PSYMBOL("has");
    NodeProxy::hasOwn =
        PROXY_NODE_PSYMBOL("hasOwn");
    NodeProxy::get =
        PROXY_NODE_PSYMBOL("get");
    NodeProxy::set =
        PROXY_NODE_PSYMBOL("set");
    NodeProxy::enumerate =
        PROXY_NODE_PSYMBOL("enumerate");
    NodeProxy::keys =
        PROXY_NODE_PSYMBOL("keys");
// createFunction
    NodeProxy::callTrap =
        PROXY_NODE_PSYMBOL("callTrap");
    NodeProxy::constructorTrap =
        PROXY_NODE_PSYMBOL("constructorTrap");
// properties of PropertyDescriptor
    NodeProxy::value =
        PROXY_NODE_PSYMBOL("value");
    NodeProxy::writable =
        PROXY_NODE_PSYMBOL("writable");
    NodeProxy::enumerable =
        PROXY_NODE_PSYMBOL("enumerable");
    NodeProxy::configurable =
        PROXY_NODE_PSYMBOL("configurable");
// misc
    NodeProxy::name =
        PROXY_NODE_PSYMBOL("name");
// hidden property names
    NodeProxy::trapping =
        PROXY_NODE_PSYMBOL("trapping");
    NodeProxy::sealed =
        PROXY_NODE_PSYMBOL("sealed");
    NodeProxy::frozen =
        PROXY_NODE_PSYMBOL("frozen");
    NodeProxy::extensible =
        PROXY_NODE_PSYMBOL("extensible");
// fixable calls
    NodeProxy::seal =
        PROXY_NODE_PSYMBOL("seal");
    NodeProxy::freeze =
        PROXY_NODE_PSYMBOL("freeze");
    NodeProxy::preventExtensions =
        PROXY_NODE_PSYMBOL("preventExtensions");
// fixed checks
    NodeProxy::isSealed =
        PROXY_NODE_PSYMBOL("isSealed");
    NodeProxy::isFrozen =
        PROXY_NODE_PSYMBOL("isFrozen");
    NodeProxy::isExtensible =
        PROXY_NODE_PSYMBOL("isExtensible");
    NodeProxy::isTrapping =
        PROXY_NODE_PSYMBOL("isTrapping");
    NodeProxy::isProxy =
        PROXY_NODE_PSYMBOL("isProxy");
// namespacing for hidden properties of visible objects
    NodeProxy::hidden =
        PROXY_NODE_PSYMBOL("NodeProxy::hidden::");
    NodeProxy::hiddenPrivate =
        PROXY_NODE_PSYMBOL("NodeProxy::hiddenPrivate::");

// function creation

// main functions
    Local<String> createName =
        String::New("create");
    Local<Function> create =
        FunctionTemplate::New(Create)->GetFunction();
    create->SetName(createName);
    target->Set(createName, create, DontDelete);

    Local<String> createFunctionName =
        String::New("createFunction");
    Local<Function> createFunction =
        FunctionTemplate::New(CreateFunction)->GetFunction();
    create->SetName(createFunctionName);
    target->Set(createFunctionName, createFunction, DontDelete);

// freeze function assignment
    Local<Function> freeze =
        FunctionTemplate::New(Freeze)->GetFunction();
    freeze->SetName(NodeProxy::freeze);
    target->Set(NodeProxy::freeze, freeze, DontDelete);

    Local<Function> seal =
        FunctionTemplate::New(Freeze)->GetFunction();
    seal->SetName(NodeProxy::seal);
    target->Set(NodeProxy::seal, seal, DontDelete);

    Local<Function> prevent =
        FunctionTemplate::New(Freeze)->GetFunction();
    prevent->SetName(NodeProxy::preventExtensions);
    target->Set(NodeProxy::preventExtensions, prevent, DontDelete);

// check function assignment
    Local<Function> isfrozen =
        FunctionTemplate::New(IsLocked)->GetFunction();
    isfrozen->SetName(NodeProxy::isFrozen);
    target->Set(NodeProxy::isFrozen, isfrozen, DontDelete);

    Local<Function> issealed =
        FunctionTemplate::New(IsLocked)->GetFunction();
    issealed->SetName(NodeProxy::isSealed);
    target->Set(NodeProxy::isSealed, issealed, DontDelete);

    Local<Function> isextensible =
        FunctionTemplate::New(IsLocked)->GetFunction();
    isextensible->SetName(NodeProxy::isExtensible);
    target->Set(NodeProxy::isExtensible, isextensible, DontDelete);

// part of harmony proxies
    Local<Function> istrapping =
        FunctionTemplate::New(IsLocked)->GetFunction();
    istrapping->SetName(NodeProxy::isTrapping);
    target->Set(NodeProxy::isTrapping, istrapping, DontDelete);

// ECMAScript 5
    Local<String> getOwnPropertyDescriptorName =
        String::New("getOwnPropertyDescriptor");
    Local<Function> getOwnPropertyDescriptor =
        FunctionTemplate::New(GetOwnPropertyDescriptor)->GetFunction();
    getOwnPropertyDescriptor->SetName(getOwnPropertyDescriptorName);
    target->Set(getOwnPropertyDescriptorName,
                getOwnPropertyDescriptor,
                DontDelete);

    Local<String> definePropertyName =
        String::New("defineProperty");
    Local<Function> defineProperty =
        FunctionTemplate::New(DefineProperty)->GetFunction();
    defineProperty->SetName(definePropertyName);
    target->Set(definePropertyName, defineProperty, DontDelete);

    Local<String> definePropertiesName =
        String::New("defineProperties");
    Local<Function> defineProperties =
        FunctionTemplate::New(DefineProperties)->GetFunction();
    defineProperties->SetName(definePropertiesName);
    target->Set(definePropertiesName, defineProperties, DontDelete);

// additional functions
    Local<String> cloneName =
        String::New("clone");
    Local<Function> clone =
        FunctionTemplate::New(Clone)->GetFunction();
    clone->SetName(cloneName);
    target->Set(cloneName, clone, DontDelete);

    Local<String> hiddenName =
        String::New("hidden");
    Local<Function> hidden =
        FunctionTemplate::New(Hidden)->GetFunction();
    hidden->SetName(hiddenName);
    target->Set(hiddenName, hidden, DontDelete);

    Local<String> setPrototypeName =
        String::New("setPrototype");
    Local<Function> setPrototype =
        FunctionTemplate::New(SetPrototype)->GetFunction();
    setPrototype->SetName(setPrototypeName);
    target->Set(setPrototypeName, setPrototype, DontDelete);

    Local<Function> isProxy_ =
        FunctionTemplate::New(IsProxy)->GetFunction();
    hidden->SetName(NodeProxy::isProxy);
    target->Set(NodeProxy::isProxy, isProxy_, DontDelete);
}

}  //  end namespace

/**
 * Required by Node for initializing the module
 *
 */
extern "C" void init(v8::Handle<v8::Object> target) {
    v8::NodeProxy::Init(target);
}
