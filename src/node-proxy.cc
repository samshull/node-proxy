/**
 *
 *
 *
 *  @author Sam Shull <http://samshull.blogspot.com/>
 *  @version 0.1
 *
 *  @copyright Copyright (c) 2009 Sam Shull <http://samshull.blogspot.com/>
 *  @license <http://www.opensource.org/licenses/mit-license.html>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 *
 *
 *  CHANGES:
 */

#include "./node-proxy.h"

Persistent<ObjectTemplate> NodeProxy::ObjectCreator;
Persistent<ObjectTemplate> NodeProxy::FunctionCreator;

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
 *  Used for creating a shallow copy of an object
 *
 *
 *  @param mixed
 *  @returns mixed
 *  @throws Error
 */
NAN_METHOD(NodeProxy::Clone) {
  NanScope();

  if (args.Length() < 1) {
    NanThrowError("clone requires at least one (1) argument.");
  }

  if (args[0]->IsString()) {
    NanReturnValue(args[0]->ToObject()->Clone()->ToString());

  } else if (args[0]->IsBoolean()) {
    NanReturnValue(args[0]->ToObject()->Clone()->ToBoolean());

  } else if (args[0]->IsNumber()
        || args[0]->IsInt32()
        || args[0]->IsUint32()) {
    NanReturnValue(args[0]->ToObject()->Clone()->ToNumber());

  } else if (args[0]->IsArray()) {
    NanReturnValue(Local<Array>::Cast(args[0]->ToObject()->Clone()));

  } else if (args[0]->IsDate()) {
    NanReturnValue(Local<Date>::Cast(args[0]->ToObject()->Clone()));

  } else if (args[0]->IsFunction()) {
    NanReturnValue(Local<Function>::Cast(args[0])->Clone());

  } else if (args[0]->IsNull()) {
    NanReturnNull();

  } else if (args[0]->IsUndefined()) {
    NanReturnUndefined();

  } else if (args[0]->IsObject()) {
    NanReturnValue(args[0]->ToObject()->Clone());
  }

  NanThrowError("clone cannot determine the type of the argument.");
  NanReturnUndefined(); // <-- silence warnings for 0.10.x
}

/**
 *  Set or Retrieve the value of a hidden
 *  property on a given object
 *  Passing two arguments to this function
 *  returns the value of the hidden property
 *  While passing three arguments to this function
 *  results in the setting of the hidden property
 *  and returns a Boolean value indicating successful
 *  setting of value
 *
 *  @param Object
 *  @param String name
 *  @param mixed value - optional
 *  @returns mixed
 *  @throws Error
 */
NAN_METHOD(NodeProxy::Hidden) {
  NanScope();

  if (args.Length() < 2) {
    NanThrowError("hidden requires at least two (2) arguments.");
  }

  Local<Object> obj = args[0]->ToObject();

  if (args.Length() < 3) {
    NanReturnValue(obj->GetHiddenValue(
          String::Concat(NanSymbol("NodeProxy::hidden:"),
                   args[1]->ToString())));
  }

  NanReturnValue(
    NanNew<Boolean>(
        obj->SetHiddenValue(String::Concat(NanSymbol("NodeProxy::hidden:"),
                           args[1]->ToString()),
        args[2])));
}

/**
 *  Set the prototype of an object
 *
 *  @param Object
 *  @param Object
 *  @returns Boolean
 *  @throws Error
 */
NAN_METHOD(NodeProxy::SetPrototype) {
  NanScope();

  if (args.Length() < 2) {
    NanThrowError("setPrototype requires at least two (2) arguments.");
  }
  NanReturnValue(NanNew<Boolean>(args[0]->ToObject()->SetPrototype(args[1])));
}

/**
 *  Determine if an Object was created by Proxy
 *
 *  @param Object
 *  @returns Boolean
 */
NAN_METHOD(NodeProxy::IsProxy) {
  NanScope();

  if (args.Length() < 1) {
    NanThrowError("isProxy requires at least one (1) argument.");
  }

  Local<Object> obj = args[0]->ToObject();

  if (obj->InternalFieldCount() > 0) {
    Local<Value> temp = obj->GetInternalField(0);

    NanReturnValue(NanNew<Boolean>(!temp.IsEmpty() && temp->IsObject()));
  }

  NanReturnValue(NanFalse());
}

/**
 *  Create an object that has ProxyHandler intercepts attached and
 *  optionally implements the prototype of another object
 *
 *  * ProxyHandler intercepts override the property handlers for any
 *  * given prototype. So, the ProxyHandler will be invoked for access
 *  * to the prototype's properties as well
 *
 *  @param ProxyHandler - @see NodeProxy::ValidateProxyHandler
 *  @param Object - optional, the prototype object to implement
 *  @returns Object
 *  @throws Error, TypeError
 */
NAN_METHOD(NodeProxy::Create) {
  NanScope();

  Local<Object> proxyHandler;

  if (args.Length() < 1) {
    NanThrowError("create requires at least one (1) argument.");
  }

  if (!args[0]->IsObject()) {
    NanThrowTypeError(
        "create requires the first argument to be an Object.");
  }

  proxyHandler = args[0]->ToObject();

  if (args.Length() > 1 && !args[1]->IsObject()) {
    NanThrowTypeError(
        "create requires the second argument to be an Object.");
  }

  // manage locking states
  proxyHandler->SetHiddenValue(NanSymbol("trapping"), NanTrue());
  proxyHandler->SetHiddenValue(NanSymbol("extensible"), NanTrue());
  proxyHandler->SetHiddenValue(NanSymbol("sealed"), NanFalse());
  proxyHandler->SetHiddenValue(NanSymbol("frozen"), NanFalse());

  Local<Object> instance = NanNew<ObjectTemplate>(ObjectCreator)->NewInstance();

  instance->SetInternalField(0, proxyHandler);

  if (args.Length() > 1) {
    instance->SetPrototype(args[1]);
  }

  NanReturnValue(instance);
}

/**
 *  Create a function that has ProxyHandler intercepts attached and
 *  sets a call trap function for invokation as well as an optional
 *  constructor trap
 *
 *
 *  @param ProxyHandler - @see NodeProxy::ValidateProxyHandler
 *  @param Function - call trap
 *  @param Function - optional, constructor trap
 *  @returns Function
 *  @throws Error, TypeError
 */
NAN_METHOD(NodeProxy::CreateFunction) {
  NanScope();

  if (args.Length() < 2) {
    NanThrowError("createFunction requires at least two (2) arguments.");
  }

  if (!args[0]->IsObject()) {
    NanThrowTypeError("createFunction requires the first argument to be an Object.");
  }
  Local<Object> proxyHandler = args[0]->ToObject();

  if (!args[1]->IsFunction()) {
    NanThrowTypeError("createFunction requires the second argument to be a Function.");
  }

  if (args.Length() > 2 && !args[2]->IsFunction()) {
    NanThrowTypeError("createFunction requires the second argument to be a Function.");
  }

  proxyHandler->SetHiddenValue(NanSymbol("callTrap"), args[1]);
  proxyHandler->SetHiddenValue(NanSymbol("constructorTrap"),
                 args.Length() > 2
                 ? args[2]
                 : NanNew(NanUndefined()).As<Value>());

  // manage locking states
  proxyHandler->SetHiddenValue(NanSymbol("trapping"), NanTrue());
  proxyHandler->SetHiddenValue(NanSymbol("extensible"), NanTrue());
  proxyHandler->SetHiddenValue(NanSymbol("sealed"), NanFalse());
  proxyHandler->SetHiddenValue(NanSymbol("frozen"), NanFalse());


  Local<Object> fn = NanNew<ObjectTemplate>(FunctionCreator)->NewInstance();
  fn->SetPrototype(args[1]->ToObject()->GetPrototype());

  fn->SetInternalField(0, proxyHandler);

  NanReturnValue(fn);
}

/**
 *  Used as a handler for freeze, seal, and preventExtensions
 *  to lock the state of a Proxy created object
 *
 *  @param Object
 *  @returns Boolean
 *  @throws Error, TypeError
 */
NAN_METHOD(NodeProxy::Freeze) {
  NanScope();

  Local<String> name = args.Callee()->GetName()->ToString();

  if (args.Length() < 1) {
    NanThrowError(String::Concat(name,
             NanNew<String>(" requires at least one (1) argument.")));
  }

  Local<Object> obj = args[0]->ToObject();

  if (obj->InternalFieldCount() < 1) {
    NanThrowTypeError(
      "Locking functions expect first "
      "argument to be intialized by Proxy");
  }

  Local<Value> hide = obj->GetInternalField(0);

  if (hide.IsEmpty() || !hide->IsObject()) {
    NanThrowTypeError(
      "Locking functions expect first "
      "argument to be intialized by Proxy");
  }

  Local<Object> handler = hide->ToObject();

  // if the object already meets the requirements of the function call
  if (name->Equals(NanSymbol("freeze"))) {
    if (handler->GetHiddenValue(NanSymbol("frozen"))->BooleanValue()) {
      NanReturnValue(NanTrue());
    }

  } else if (name->Equals(NanSymbol("seal"))) {
    if (handler->GetHiddenValue(NanSymbol("sealed"))->BooleanValue()) {
      NanReturnValue(NanTrue());
    }

  } else if (name->Equals(NanSymbol("preventExtensions"))) {
    if (handler->GetHiddenValue(NanSymbol("extensible"))->BooleanValue()) {
      NanReturnValue(NanTrue());
    }
  }

  // if this object is not trapping, just set the appropriate parameters
  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    if (name->Equals(NanSymbol("freeze"))) {
      handler->SetHiddenValue(NanSymbol("frozen"), NanTrue());
      handler->SetHiddenValue(NanSymbol("sealed"), NanTrue());
      handler->SetHiddenValue(NanSymbol("extensible"), NanFalse());
      NanReturnValue(NanTrue());

    } else if (name->Equals(NanSymbol("seal"))) {
      handler->SetHiddenValue(NanSymbol("sealed"), NanTrue());
      handler->SetHiddenValue(NanSymbol("extensible"), NanFalse());
      NanReturnValue(NanTrue());

    } else if (name->Equals(NanSymbol("preventExtensions"))) {
      handler->SetHiddenValue(NanSymbol("extensible"), NanFalse());
      NanReturnValue(NanTrue());
    }
  }

  // Harmony Proxy handling of fix
  Local<Function> fix = Local<Function>::Cast(handler->Get(NanSymbol("fix")));
#ifdef _WIN32
  // On windows you get "error C2466: cannot allocate an array of constant size 0" and we use a pointer
  Local<Value>* argv;
#else
  Local<Value> argv[0];
#endif
  Local<Value> pieces = fix->Call(args[0]->ToObject(), 0, argv);

  if (pieces.IsEmpty() || !pieces->IsObject()) {
    NanThrowTypeError("Cannot lock object.");
  }

  Local<Object> parts = pieces->ToObject();

  // set the appropriate parameters
  if (name->Equals(NanSymbol("freeze"))) {
    parts->SetHiddenValue(NanSymbol("frozen"), NanTrue());
    parts->SetHiddenValue(NanSymbol("sealed"), NanTrue());
    parts->SetHiddenValue(NanSymbol("extensible"), NanFalse());

  } else if (name->Equals(NanSymbol("seal"))) {
    parts->SetHiddenValue(NanSymbol("sealed"), NanTrue());
    parts->SetHiddenValue(NanSymbol("extensible"), NanFalse());

  } else if (name->Equals(NanSymbol("preventExtensions"))) {
    parts->SetHiddenValue(NanSymbol("extensible"), NanFalse());
  }

  parts->SetHiddenValue(NanSymbol("trapping"), NanFalse());

  // overwrite the handler, making handler available for GC
  obj->SetInternalField(0, parts);

  NanReturnValue(NanTrue());
}

/**
 *  Used as a handler for determining isTrapped,
 *  isFrozen, isSealed, and isExtensible
 *
 *  @param Object
 *  @returns Boolean
 *  @throws Error, TypeError
 */
NAN_METHOD(NodeProxy::IsLocked) {
  NanScope();

  Local<String> name = args.Callee()->GetName()->ToString();

  if (args.Length() < 1) {
    NanThrowError(String::Concat(name,
             NanNew<String>(" requires at least one (1) argument.")));
  }

  Local<Object> arg = args[0]->ToObject();

  if (arg->InternalFieldCount() < 1) {
    NanThrowTypeError(
       "Locking functions expect first argument "
       "to be intialized by Proxy");
  }

  Local<Value> hide = arg->GetInternalField(0);

  if (hide.IsEmpty() || !hide->IsObject()) {
    NanThrowTypeError(
      "Locking functions expect first argument "
      "to be intialized by Proxy");
  }

  Local<Object> obj = hide->ToObject();

  if (name->Equals(NanSymbol("isExtensible"))) {
    NanReturnValue(obj->GetHiddenValue(NanSymbol("extensible"))->ToBoolean());

  } else if (name->Equals(NanSymbol("isSealed"))) {
    NanReturnValue(obj->GetHiddenValue(NanSymbol("sealed"))->ToBoolean());

  } else if (name->Equals(NanSymbol("isTrapping"))) {
    NanReturnValue(obj->GetHiddenValue(NanSymbol("trapping"))->ToBoolean());

  } else if (name->Equals(NanSymbol("isFrozen"))) {
    NanReturnValue(obj->GetHiddenValue(NanSymbol("frozen"))->ToBoolean());
  }

  NanReturnValue(NanFalse());
}

/**
 *  Part of ECMAScript 5, but only for use on
 *  Objects and Functions created by Proxy
 *
 *  @param Object
 *  @param String - the name of the property
 *  @returns PropertyDescriptor
 *  @throws Error, TypeError
 */
NAN_METHOD(NodeProxy::GetOwnPropertyDescriptor) {
  NanScope();

  if (args.Length() < 2) {
    NanThrowError("getOwnPropertyDescriptor requires "
        "at least two (2) arguments.");
  }

  if (!args[1]->IsString() && !args[1]->IsNumber()) {
    NanThrowTypeError("getOwnPropertyDescriptor requires "
           "the second argument to be a String or a Number.");
  }

  Local<Object> obj = args[0]->ToObject();
  Local<String> name = args[1]->ToString();

  if (obj->InternalFieldCount() < 1) {
    NanThrowTypeError("getOwnPropertyDescriptor expects "
            "first argument to be intialized by Proxy");
  }

  Local<Value> temp = obj->GetInternalField(0);

  if (temp.IsEmpty() || !temp->IsObject()) {
    NanThrowTypeError("getOwnPropertyDescriptor expects "
            "first argument to be intialized by Proxy");
  }

  Local<Object> handler = temp->ToObject();

  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    NanReturnValue(handler->Get(name));
  }

  Local<Function> getOwn = Local<Function>::Cast(
       handler->Get(NanSymbol("getOwnPropertyDescriptor")));

  Local<Value> argv[1] = {args[1]};
  NanReturnValue(getOwn->Call(obj, 1, argv));
}

/**
 *  Part of ECMAScript 5, but only for use on
 *  Objects and Functions created by Proxy
 *
 *  @param Object
 *  @param String - the name of the property
 *  @param PropertyDescriptor
 *  @returns Boolean
 *  @throws Error, TypeError
 */
NAN_METHOD(NodeProxy::DefineProperty) {
  NanScope();

  if (args.Length() < 3) {
    NanThrowError("defineProperty requires at least three (3) arguments.");
  }

  if (!args[1]->IsString() && !args[1]->IsNumber()) {
    NanThrowTypeError("defineProperty requires the "
                "second argument to be a String or a Number.");
  }

  if (!args[2]->IsObject()) {
    NanThrowTypeError("defineProperty requires the third argument "
            "to be an Object of the type PropertyDescriptor.");
  }

  Local<Object> obj = args[0]->ToObject();

  if (obj->InternalFieldCount() < 1) {
    NanThrowTypeError("defineProperty expects first "
                "argument to be intialized by Proxy");
  }

  Local<Value> temp = obj->GetInternalField(0);

  if (temp.IsEmpty() || !temp->IsObject()) {
    NanThrowTypeError("defineProperty expects first argument "
                "to be intialized by Proxy");
  }

  Local<String> name = args[1]->ToString();
  Local<Object> handler = temp->ToObject();

  if (handler->GetHiddenValue(NanSymbol("sealed"))->BooleanValue() ||
  !handler->Has(NanSymbol("defineProperty"))) {
    NanReturnValue(NanFalse());
  }

  if (!handler->GetHiddenValue(NanSymbol("extensible"))->BooleanValue() &&
        !handler->Has(name)) {
    NanReturnValue(NanFalse());
  }

  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    Local<Object> desc = handler->Get(name)->ToObject();

    if (desc->Get(NanSymbol("configurable"))->BooleanValue()) {
      NanReturnValue(NanNew<Boolean>(
          handler->Set(name, args[2]->ToObject())));
    }
    NanReturnValue(NanFalse());
  }

  Local<Function> def = Local<Function>::Cast(
                  handler->Get(NanSymbol("defineProperty")));

  Local<Value> argv[2] = {args[1], args[2]->ToObject()};

  NanReturnValue(def->Call(obj, 2, argv)->ToBoolean());
}

/**
 *  Part of ECMAScript 5, but only for use on
 *  Objects and Functions created by Proxy
 *
 *  @param Object
 *  @param Object - name/PropertyDescriptor pairs
 *  @returns Boolean
 *  @throws Error, TypeError
 */
NAN_METHOD(NodeProxy::DefineProperties) {
  NanScope();

  if (args.Length() < 2) {
    NanThrowError("defineProperty requires at least three (3) arguments.");
  }

  if (!args[1]->IsObject()) {
    NanThrowTypeError("defineProperty requires the third argument "
             "to be an Object of the type PropertyDescriptor.");
  }

  Local<Object> obj = args[0]->ToObject();

  if (obj->InternalFieldCount() < 1) {
    NanThrowTypeError("defineProperty expects first "
                "argument to be intialized by Proxy");
  }

  Local<Value> temp = obj->GetInternalField(0);

  if (!temp.IsEmpty() && temp->IsObject()) {
    Local<Object> props = args[1]->ToObject();
    Local<Object> handler = temp->ToObject();

    if (handler->GetHiddenValue(NanSymbol("sealed"))->BooleanValue()) {
      NanReturnValue(NanFalse());
    }

    bool extensible = handler->GetHiddenValue(
                  NanSymbol("extensible"))->BooleanValue();
    Local<Array> names = props->GetPropertyNames();
    uint32_t i = 0, l = names->Length();

    if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
      for (;i < l; ++i) {
        Local<Object> name = names->CloneElementAt(i);

        if (handler->Has(name->ToString()) &&
            handler->Get(name->ToString())->IsObject()
        ) {
          Local<Object> tempObj =         handler->Get(name->ToString())->ToObject();

          if (tempObj->Get(NanSymbol("configurable"))->BooleanValue()) {
            if (!handler->Set(name->ToString(),
                      props->Get(name->ToString()))) {
              NanThrowError(
                String::Concat(
                  NanNew<String>("Unable to define property: "),
                  name->ToString()));
            }
          }
        } else {
          NanThrowError(String::Concat(
                  NanNew<String>("Unable to define property: "),
                  name->ToString()));
        }
      }
      NanReturnValue(NanTrue());
    }

    Local<Function> def =   Local<Function>::Cast(handler->Get(NanSymbol("defineProperty")));

    TryCatch firstTry;
    for (;i < l; ++i) {
      Local<Value> name = names->Get(i);

      if (extensible || obj->Has(name->ToString())) {
        Local<Value> pd = props->Get(name->ToString());
        Local<Value> argv[2] = {name, pd};
        def->Call(obj, 2, argv);

        if (firstTry.HasCaught()) {
          NanReturnValue(firstTry.ReThrow());
        }
      }
    }
    NanReturnValue(NanTrue());
  }
  NanReturnValue(NanFalse());
}

/**
 *  Function used for a constructor and invocation
 *  handler of a Proxy created function
 *  Calls the appropriate function attached when the Proxy was created
 *
 *  @param ...args
 *  @returns mixed
 *  @throws Error
 */
NAN_METHOD(NodeProxy::New) {
  NanScope();

  if (args.Callee()->InternalFieldCount() < 1 && args.Data().IsEmpty()) {
    NanThrowTypeError("defineProperty expects first "
                "argument to be intialized by Proxy");
  }

  Local<Value> info, ret, data =  args.Holder()->GetInternalField(0);

  if (data.IsEmpty() || !data->IsObject()) {
    NanThrowError("Invalid reference to Proxy#constructor");
  }

  Local<Function> fn;
  Local<Object> obj = data->ToObject();

  if (args.IsConstructCall()) {
    info = obj->GetHiddenValue(NanSymbol("constructorTrap"));

    if (!info.IsEmpty() && info->IsFunction()) {
      fn = Local<Function>::Cast(info);
    } else {
      fn = Local<Function>::Cast(
          obj->GetHiddenValue(NanSymbol("callTrap")));
    }
  } else {
    fn = Local<Function>::Cast(obj->GetHiddenValue(NanSymbol("callTrap")));
  }

  int i = 0, l = args.Length();
  Local<Value>* argv = new Local<Value>[l];

  for (; i < l; ++i) {
    argv[i] = args[i];
  }

  ret = fn->Call(args.This(), args.Length(), argv);

  if (args.IsConstructCall()) {
    if (!ret.IsEmpty()) {
      NanReturnValue(ret);
    }
    NanReturnValue(args.This());
  }
  NanReturnValue(ret);
}

/**
 *  Invoked for accessing the named properties of an object
 *
 *
 *
 */
NAN_PROPERTY_GETTER(NodeProxy::GetNamedProperty) {
  NanScope();

  if (args.This()->InternalFieldCount() < 1 || args.Data().IsEmpty()) {
    NanThrowTypeError("SetNamedProperty intercepted "
                "by non-Proxy object");
  }

  Local<Value> argv1[1] = {property};
  Local<Value> data = args.This()->InternalFieldCount() > 0 ?
                      args.This()->GetInternalField(0) :
                      args.Data();

  if (!data->IsObject()) {
    NanReturnUndefined();
  }

  Local<Function> fn;
  Local<Object> handler = data->ToObject();

  // if the Proxy isn't trapping, return
  // the value set on the property descriptor
  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    NanReturnValue(CallPropertyDescriptorGet(handler->Get(property), args.This(), argv1));
  }

  Local<Value> get = handler->Get(NanSymbol("get"));
  if (get->IsFunction()) {
    fn = Local<Function>::Cast(get);
    Local<Value> argv[2] = {args.This(), property};

    NanReturnValue(fn->Call(handler, 2, argv));
  }

  Local<Value> getPropertyDescriptor = handler->Get(NanSymbol("getPropertyDescriptor"));
  if (getPropertyDescriptor->IsFunction()) {
    fn = Local<Function>::Cast(getPropertyDescriptor);

    NanReturnValue(CallPropertyDescriptorGet(fn->Call(handler, 1, argv1), args.This(), argv1));
  }

  Local<Value> getOwnPropertyDescriptor = handler->Get(NanSymbol("getOwnPropertyDescriptor"));
  if (getOwnPropertyDescriptor->IsFunction()) {
    fn = Local<Function>::Cast(getOwnPropertyDescriptor);

    NanReturnValue(CallPropertyDescriptorGet(fn->Call(handler, 1, argv1), args.This(), argv1));
  }
  NanReturnUndefined(); // <-- silence warnings for 0.10.x
}

NAN_INLINE Local<Value> NodeProxy::CallPropertyDescriptorGet(Local<Value> descriptor, Handle<Object> context, Local<Value> args[1]) {
  if (descriptor->IsObject()) {
    Local<Value> get = descriptor->ToObject()->Get(NanSymbol("get"));

    if (get->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(get);
      return fn->Call(context, 1, args);
    }

    return descriptor->ToObject()->Get(NanSymbol("value"));
  }

  return NanNew(NanUndefined());
}

/**
 *  Invoked for setting the named properties of an object
 *
 *
 *
 */
NAN_PROPERTY_SETTER(NodeProxy::SetNamedProperty) {
  NanScope();

  if (args.This()->InternalFieldCount() < 1 || args.Data().IsEmpty()) {
    NanThrowTypeError("SetNamedProperty intercepted "
                "by non-Proxy object");
  }

  Local<Value> argv2[2] = {property, value};
  Local<Value> data = args.This()->InternalFieldCount() > 0 ?
                      args.This()->GetInternalField(0) :
                      args.Data();

  if (!data->IsObject()) {
    NanReturnUndefined();
  }

  Local<Object> handler = data->ToObject();

  // if the Proxy isn't trapping, return the
  // value set on the property descriptor
  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    if (handler->GetHiddenValue(NanSymbol("extensible"))->BooleanValue() ||
      handler->Has(property)
    ) {
      Local<Value> pd = handler->Get(property);

      if (!pd->IsObject()) {
        NanReturnUndefined();
      }

      Local<Object> pd_obj = pd->ToObject();

      if (!pd_obj->GetHiddenValue(
            NanSymbol("writable"))->BooleanValue()
      ) {
        NanThrowError(
              String::Concat(
                NanNew<String>("In accessible property: "),
                    property));
      }

      Local<Value> set = pd_obj->Get(NanSymbol("set"));
      if (set->IsFunction()) {
        Local<Function> fn = Local<Function>::Cast(set);
        fn->Call(args.This(), 2, argv2);

        NanReturnValue(value);
      }

      if (pd_obj->Set(NanSymbol("value"), value)) {
        NanReturnValue(value);
      }
      NanReturnUndefined();
    }
    NanReturnUndefined();
  }

  // does the ProxyHandler have a set method?
  Local<Value> set = handler->Get(NanSymbol("set"));
  if (set->IsFunction()) {
    Local<Function> set_fn = Local<Function>::Cast(set);
    Local<Value> argv3[3] = {args.This(), property, value};
    set_fn->Call(handler, 3, argv3);

    NanReturnValue(value);
  }

  Local<Value> getOwnPropertyDescriptor = handler->Get(NanSymbol("getOwnPropertyDescriptor"));
  if (getOwnPropertyDescriptor->IsFunction()) {
    Local<Function> gopd_fn = Local<Function>::Cast(getOwnPropertyDescriptor);
    Local<Value> argv[1] = {property};
    NanReturnValue(CallPropertyDescriptorSet(gopd_fn->Call(handler, 1, argv), args.This(), property, value));
  }

  Local<Value> getPropertyDescriptor = handler->Get(NanSymbol("getPropertyDescriptor"));
  if (getPropertyDescriptor->IsFunction()) {
    Local<Function> gpd_fn = Local<Function>::Cast(getPropertyDescriptor);
    Local<Value> argv[1] = {property};
    NanReturnValue(CallPropertyDescriptorSet(gpd_fn->Call(handler, 1, argv), args.This(), property, value));
  }

  NanReturnUndefined();
}

NAN_INLINE Local<Value> NodeProxy::CallPropertyDescriptorSet(Local<Value> descriptor, Handle<Object> context, Local<Value> name, Local<Value> value) {
  if (descriptor->IsObject()) {
    Local<Object> pd = descriptor->ToObject();
    Local<Value> set = pd->Get(NanSymbol("set"));

    if (set->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(set);
      Local<Value> args[2] = { name, value };
      return fn->Call(context, 2, args);

    } else if (pd->Get(NanSymbol("writable"))->BooleanValue()) {
      if (pd->Set(NanSymbol("value"), value)) {
        return value;
      }
    }
  }

  return NanNew(NanUndefined());
}


/**
 *  Invoked for determining if an object has a specific property
 *
 *
 *
 */
NAN_PROPERTY_QUERY(NodeProxy::QueryNamedPropertyInteger) {
  NanScope();

  Local<Integer> DoesntHavePropertyResponse;
  Local<Integer> HasPropertyResponse = NanNew<Integer>(None);

  if (args.This()->InternalFieldCount() > 0 || !args.Data().IsEmpty()) {
    Local<Value> data = args.This()->InternalFieldCount() > 0 ?
                 args.This()->GetInternalField(0) :
                 args.Data();

    if (!data->IsObject()) {
      NanReturnValue(DoesntHavePropertyResponse);
    }

    Local<Object> handler = data->ToObject();

    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
      if (handler->Has(property)) {
        Local<Value> pd = handler->Get(property);

        if (pd->IsObject()) {
          NanReturnValue(GetPropertyAttributeFromPropertyDescriptor(pd->ToObject()));
        }
        NanReturnValue(HasPropertyResponse);
      }
      NanReturnValue(DoesntHavePropertyResponse);
    }

    Local<Value> argv[1] = {property};

    Local<Value> hasOwn = handler->Get(NanSymbol("hasOwn"));
    if (hasOwn->IsFunction()) {
      Local<Function> hasOwn_fn = Local<Function>::Cast(hasOwn);
      NanReturnValue(hasOwn_fn->Call(handler, 1, argv)->BooleanValue() ?
                     HasPropertyResponse :
                     DoesntHavePropertyResponse);
    }

    Local<Value> has = handler->Get(NanSymbol("has"));
    if (has->IsFunction()) {
      Local<Function> has_fn = Local<Function>::Cast(has);
      NanReturnValue(has_fn->Call(handler, 1, argv)->BooleanValue() ?
                     HasPropertyResponse :
                     DoesntHavePropertyResponse);
    }

    Local<Value> getOwnPropertyDescriptor = handler->Get(NanSymbol("getOwnPropertyDescriptor"));
    if (getOwnPropertyDescriptor->IsFunction()) {
      Local<Function> gopd_fn = Local<Function>::Cast(getOwnPropertyDescriptor);
      Local<Value> gopd_pd = gopd_fn->Call(handler, 1, argv);

      if (gopd_pd->IsObject()) {
        NanReturnValue(GetPropertyAttributeFromPropertyDescriptor(gopd_pd->ToObject()));
      }
    }

    Local<Value> getPropertyDescriptor = handler->Get(NanSymbol("getPropertyDescriptor"));
    if (handler->Has(NanSymbol("getPropertyDescriptor"))) {
      Local<Function> gpd_fn = Local<Function>::Cast(getPropertyDescriptor);
      Local<Value> gpd_pd = gpd_fn->Call(handler, 1, argv);

      if (gpd_pd->IsObject()) {
        NanReturnValue(GetPropertyAttributeFromPropertyDescriptor(gpd_pd->ToObject()));
      } else if (gpd_pd->IsUndefined()) {
        NanReturnValue(DoesntHavePropertyResponse);
      }
    }
  }

  NanReturnValue(DoesntHavePropertyResponse);
}

/**
 *  Find the appropriate PropertyAttribute
 *  for a given PropertyDescriptor object
 *
 *
 */
Handle<Integer>
NodeProxy::GetPropertyAttributeFromPropertyDescriptor(Local<Object> pd) {
  NanScope();
  uint32_t ret = None;

  if (pd->Get(NanSymbol("configurable"))->IsBoolean() &&
        !pd->Get(NanSymbol("configurable"))->BooleanValue()) {
    // return NanNew<Integer>(DontDelete);
    ret &= DontDelete;
  }

  if (pd->Get(NanSymbol("enumerable"))->IsBoolean() &&
             !pd->Get(NanSymbol("enumerable"))->BooleanValue()) {
    // return NanNew<Integer>(DontEnum);
    ret &= DontEnum;
  }

  if (pd->Get(NanSymbol("writable"))->IsBoolean() &&
             !pd->Get(NanSymbol("writable"))->BooleanValue()) {
     // return NanNew<Integer>(ReadOnly);
     ret &= ReadOnly;
  }

  return NanNew<Integer>(ret);
}

/**
 *  Invoked when deleting the named property of an object
 *
 *
 *
 */
NAN_PROPERTY_DELETER(NodeProxy::DeleteNamedProperty) {
  NanScope();

  if (args.This()->InternalFieldCount() > 0 || !args.Data().IsEmpty()) {
    Local<Value> data = args.This()->InternalFieldCount() > 0 ?
                 args.This()->GetInternalField(0) :
                 args.Data();

    if (!data->IsObject()) {
      NanReturnValue(NanFalse());
    }

    Local<Object> handler = data->ToObject();
    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
      if (!handler->GetHiddenValue(NanSymbol("frozen"))->BooleanValue()) {
        Local<Value> pd = handler->Get(property);

        if (pd->IsObject()) {
          Local<Object> pd_obj = pd->ToObject();

          if (pd_obj->Get(NanSymbol("configurable"))->IsBoolean() &&
              pd_obj->Get(NanSymbol("configurable"))->BooleanValue()
          ) {
            NanReturnValue(NanNew<Boolean>(handler->Delete(property)));
          }
        }
      }
      NanReturnValue(NanFalse());
    }

    Local<Value> delete_ = handler->Get(NanSymbol("delete"));
    if (delete_->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(delete_);
      Local<Value> argv[1] = {property};
      NanReturnValue(fn->Call(handler, 1, argv)->ToBoolean());
    }
  }

  NanReturnValue(NanFalse());
}

/**
 *  Invoked for enumerating all properties of an object
 *
 *
 *
 */
NAN_PROPERTY_ENUMERATOR(NodeProxy::EnumerateNamedProperties) {
  NanScope();

  if (args.This()->InternalFieldCount() > 0 || !args.Data().IsEmpty()) {
    Local<Value> data = args.This()->InternalFieldCount() > 0 ?
        args.This()->GetInternalField(0) :
         args.Data();

    if (!data->IsObject()) {
      NanReturnValue(NanNew<Array>());
    }

    Local<Object> handler = data->ToObject();
#ifdef _WIN32
    // On windows you get "error C2466: cannot allocate an array of constant size 0" and we use a pointer
    Local<Value>* argv;
#else
    Local<Value> argv[0];
#endif

    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
      NanReturnValue(handler->GetPropertyNames());
    }

    Local<Value> enumerate = handler->Get(NanSymbol("enumerate"));
    if (enumerate->IsFunction()) {
      Local<Function> enumerate_fn = Local<Function>::Cast(enumerate);
      Local<Value> names = enumerate_fn->Call(handler, 0, argv);

      if (names->IsArray()) {
        NanReturnValue(Local<Array>::Cast(names->ToObject()));
      }
    }

    Local<Value> keys = handler->Get(NanSymbol("keys"));
    if (keys->IsFunction()) {
      Local<Function> keys_fn = Local<Function>::Cast(enumerate);
      Local<Value> names = keys_fn->Call(handler, 0, argv);

      if (names->IsArray()) {
        NanReturnValue(Local<Array>::Cast(names->ToObject()));
      }
    }

    Local<Value> getPropertyNames = handler->Get(NanSymbol("getPropertyNames"));
    if (getPropertyNames->IsFunction()) {
      Local<Function> gpn_fn = Local<Function>::Cast(getPropertyNames);
      Local<Value> names = gpn_fn->Call(handler, 0, argv);

      if (names->IsArray()) {
        NanReturnValue(Local<Array>::Cast(names->ToObject()));
      }
    }
  }

  NanReturnValue(NanNew<Array>());
}

/**
 *  Invoked for accessing the given indexed property of an object
 *
 *
 *
 */
NAN_INDEX_GETTER(NodeProxy::GetIndexedProperty) {
  NanScope();

  if (args.This()->InternalFieldCount() < 1 || args.Data().IsEmpty()) {
    NanThrowTypeError("SetNamedProperty intercepted "
                "by non-Proxy object");
  }

  Local<Integer> idx = NanNew<Integer>(index);
  Local<Value> argv1[1] = {idx};
  Local<Value> data = args.This()->InternalFieldCount() > 0 ?
                      args.This()->GetInternalField(0) :
                      args.Data();

  if (!data->IsObject()) {
    NanReturnUndefined();
  }

  Local<Function> fn;
  Local<Object> handler = data->ToObject();

  // if the Proxy isn't trapping, return
  // the value set on the index descriptor
  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    NanReturnValue(CallPropertyDescriptorGet(handler->Get(idx), args.This(), argv1));
  }

  Local<Value> get = handler->Get(NanSymbol("get"));
  if (get->IsFunction()) {
    fn = Local<Function>::Cast(get);
    Local<Value> argv[2] = {args.This(), idx};

    NanReturnValue(fn->Call(handler, 2, argv));
  }

  Local<Value> getPropertyDescriptor = handler->Get(NanSymbol("getPropertyDescriptor"));
  if (getPropertyDescriptor->IsFunction()) {
    fn = Local<Function>::Cast(getPropertyDescriptor);

    NanReturnValue(CallPropertyDescriptorGet(fn->Call(handler, 1, argv1), args.This(), argv1));
  }

  Local<Value> getOwnPropertyDescriptor = handler->Get(NanSymbol("getOwnPropertyDescriptor"));
  if (getOwnPropertyDescriptor->IsFunction()) {
    fn = Local<Function>::Cast(getOwnPropertyDescriptor);

    NanReturnValue(CallPropertyDescriptorGet(fn->Call(handler, 1, argv1), args.This(), argv1));
  }
  NanReturnUndefined(); // <-- silence warnings for 0.10.x
}

/**
 *  Invoked for setting the given indexed property of an object
 *
 *
 *
 */
NAN_INDEX_SETTER(NodeProxy::SetIndexedProperty) {
  NanScope();

  if (args.This()->InternalFieldCount() < 1 || args.Data().IsEmpty()) {
    NanThrowTypeError("SetNamedProperty intercepted "
                "by non-Proxy object");
  }

  Local<Integer> idx = NanNew<Integer>(index);
  Local<Value> argv2[2] = {idx, value};
  Local<Value> data = args.This()->InternalFieldCount() > 0 ?
                      args.This()->GetInternalField(0) :
                      args.Data();

  if (!data->IsObject()) {
    NanReturnUndefined();
  }

  Local<Object> handler = data->ToObject();

  // if the Proxy isn't trapping, return the
  // value set on the index descriptor
  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    if (handler->GetHiddenValue(NanSymbol("extensible"))->BooleanValue() ||
      handler->Has(index)
    ) {
      Local<Value> pd = handler->Get(index);

      if (!pd->IsObject()) {
        NanReturnUndefined();
      }

      Local<Object> pd_obj = pd->ToObject();

      if (!pd_obj->GetHiddenValue(
            NanSymbol("writable"))->BooleanValue()
      ) {
        NanThrowError(
              String::Concat(
                NanNew<String>("In accessible index: "),
                    Local<String>::Cast(idx)));
      }

      Local<Value> set = pd_obj->Get(NanSymbol("set"));
      if (set->IsFunction()) {
        Local<Function> fn = Local<Function>::Cast(set);
        fn->Call(args.This(), 2, argv2);

        NanReturnValue(value);
      }

      if (pd_obj->Set(NanSymbol("value"), value)) {
        NanReturnValue(value);
      }
      NanReturnUndefined();
    }
    NanReturnUndefined();
  }

  // does the ProxyHandler have a set method?
  Local<Value> set = handler->Get(NanSymbol("set"));
  if (set->IsFunction()) {
    Local<Function> set_fn = Local<Function>::Cast(set);
    Local<Value> argv3[3] = {args.This(), idx, value};
    set_fn->Call(handler, 3, argv3);

    NanReturnValue(value);
  }

  Local<Value> getOwnPropertyDescriptor = handler->Get(NanSymbol("getOwnPropertyDescriptor"));
  if (getOwnPropertyDescriptor->IsFunction()) {
    Local<Function> gopd_fn = Local<Function>::Cast(getOwnPropertyDescriptor);
    Local<Value> argv[1] = {idx};
    NanReturnValue(CallPropertyDescriptorSet(gopd_fn->Call(handler, 1, argv), args.This(), idx, value));
  }

  Local<Value> getPropertyDescriptor = handler->Get(NanSymbol("getPropertyDescriptor"));
  if (getPropertyDescriptor->IsFunction()) {
    Local<Function> gpd_fn = Local<Function>::Cast(getPropertyDescriptor);
    Local<Value> argv[1] = {idx};
    NanReturnValue(CallPropertyDescriptorSet(gpd_fn->Call(handler, 1, argv), args.This(), idx, value));
  }

  NanReturnUndefined();
}

/**
 *  Invoked for determining if an object has a given indexed property
 *
 *
 *
 */
NAN_INDEX_QUERY(NodeProxy::QueryIndexedPropertyInteger) {
  NanScope();

  Local<Integer> idx = NanNew<Integer>(index);
  Local<Integer> DoesntHavePropertyResponse;
  Local<Integer> HasPropertyResponse = NanNew<Integer>(None);

  if (args.This()->InternalFieldCount() > 0 || !args.Data().IsEmpty()) {
    Local<Value> data = args.This()->InternalFieldCount() > 0 ?
                 args.This()->GetInternalField(0) :
                 args.Data();

    if (!data->IsObject()) {
      NanReturnValue(DoesntHavePropertyResponse);
    }

    Local<Object> handler = data->ToObject();

    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
      if (handler->Has(idx)) {
        Local<Value> pd = handler->Get(idx);

        if (pd->IsObject()) {
          NanReturnValue(GetPropertyAttributeFromPropertyDescriptor(pd->ToObject()));
        }
        NanReturnValue(HasPropertyResponse);
      }
      NanReturnValue(DoesntHavePropertyResponse);
    }

    Local<Value> argv[1] = {idx};

    Local<Value> hasOwn = handler->Get(NanSymbol("hasOwn"));
    if (hasOwn->IsFunction()) {
      Local<Function> hasOwn_fn = Local<Function>::Cast(hasOwn);
      NanReturnValue(hasOwn_fn->Call(handler, 1, argv)->BooleanValue() ?
                     HasPropertyResponse :
                     DoesntHavePropertyResponse);
    }

    Local<Value> has = handler->Get(NanSymbol("has"));
    if (has->IsFunction()) {
      Local<Function> has_fn = Local<Function>::Cast(has);
      NanReturnValue(has_fn->Call(handler, 1, argv)->BooleanValue() ?
                     HasPropertyResponse :
                     DoesntHavePropertyResponse);
    }

    Local<Value> getOwnPropertyDescriptor = handler->Get(NanSymbol("getOwnPropertyDescriptor"));
    if (getOwnPropertyDescriptor->IsFunction()) {
      Local<Function> gopd_fn = Local<Function>::Cast(getOwnPropertyDescriptor);
      Local<Value> gopd_pd = gopd_fn->Call(handler, 1, argv);

      if (gopd_pd->IsObject()) {
        NanReturnValue(GetPropertyAttributeFromPropertyDescriptor(gopd_pd->ToObject()));
      }
    }

    Local<Value> getPropertyDescriptor = handler->Get(NanSymbol("getPropertyDescriptor"));
    if (handler->Has(NanSymbol("getPropertyDescriptor"))) {
      Local<Function> gpd_fn = Local<Function>::Cast(getPropertyDescriptor);
      Local<Value> gpd_pd = gpd_fn->Call(handler, 1, argv);

      if (gpd_pd->IsObject()) {
        NanReturnValue(GetPropertyAttributeFromPropertyDescriptor(gpd_pd->ToObject()));
      } else if (gpd_pd->IsUndefined()) {
        NanReturnValue(DoesntHavePropertyResponse);
      }
    }
  }

  NanReturnValue(DoesntHavePropertyResponse);
}

/**
 *  Invoked for deleting a given indexed property
 *
 *
 *
 */
NAN_INDEX_DELETER(NodeProxy::DeleteIndexedProperty) {
  NanScope();

  if (args.This()->InternalFieldCount() > 0 || !args.Data().IsEmpty()) {
    Local<Value> data = args.This()->InternalFieldCount() > 0 ?
                 args.This()->GetInternalField(0) :
                 args.Data();

    if (!data->IsObject()) {
      NanReturnValue(NanFalse());
    }

    Local<Integer> idx = NanNew<Integer>(index);
    Local<Object> handler = data->ToObject();
    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
      if (!handler->GetHiddenValue(NanSymbol("frozen"))->BooleanValue()) {
        Local<Value> pd = handler->Get(idx);

        if (pd->IsObject()) {
          Local<Object> pd_obj = pd->ToObject();

          if (pd_obj->Get(NanSymbol("configurable"))->IsBoolean() &&
              pd_obj->Get(NanSymbol("configurable"))->BooleanValue()
          ) {
            NanReturnValue(NanNew<Boolean>(handler->Delete(idx)));
          }
        }
      }
      NanReturnValue(NanFalse());
    }

    Local<Value> delete_ = handler->Get(NanSymbol("delete"));
    if (delete_->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(delete_);
      Local<Value> argv[1] = {idx};
      NanReturnValue(fn->Call(handler, 1, argv)->ToBoolean());
    }
  }

  NanReturnValue(NanNew<Boolean>(false));
}

/**
 *  Initialize the NodeProxy Strings and functions
 *
 *
 *
 */
void NodeProxy::Init(Handle<Object> target) {
  NanScope();

// function creation

// main functions
  Local<Function> create = NanNew<FunctionTemplate>(Create)->GetFunction();
  Local<String> _create = NanSymbol("create");
  create->SetName(_create);
  target->Set(_create, create, DontDelete);

  Local<Function> createFunction = NanNew<FunctionTemplate>(CreateFunction)->GetFunction();
  Local<String> _createFunction = NanSymbol("createFunction");
  create->SetName(_createFunction);
  target->Set(_createFunction, createFunction, DontDelete);

// freeze function assignment
  Local<Function> freeze = NanNew<FunctionTemplate>(Freeze)->GetFunction();
  Local<String> _freeze = NanSymbol("freeze");
  freeze->SetName(_freeze);
  target->Set(_freeze, freeze, DontDelete);

  Local<Function> seal = NanNew<FunctionTemplate>(Freeze)->GetFunction();
  Local<String> _seal = NanSymbol("seal");
  seal->SetName(_seal);
  target->Set(_seal, seal, DontDelete);

  Local<Function> prevent = NanNew<FunctionTemplate>(Freeze)->GetFunction();
  Local<String> _preventExtensions = NanSymbol("preventExtensions");
  prevent->SetName(_preventExtensions);
  target->Set(_preventExtensions, prevent, DontDelete);

// check function assignment
  Local<Function> isfrozen = NanNew<FunctionTemplate>(IsLocked)->GetFunction();
  Local<String> _isFrozen = NanSymbol("isFrozen");
  isfrozen->SetName(_isFrozen);
  target->Set(_isFrozen, isfrozen, DontDelete);

  Local<Function> issealed = NanNew<FunctionTemplate>(IsLocked)->GetFunction();
  Local<String> _isSealed = NanSymbol("isSealed");
  issealed->SetName(_isSealed);
  target->Set(_isSealed, issealed, DontDelete);

  Local<Function> isextensible = NanNew<FunctionTemplate>(IsLocked)->GetFunction();
  Local<String> _isExtensible = NanSymbol("isExtensible");
  isextensible->SetName(_isExtensible);
  target->Set(_isExtensible, isextensible, DontDelete);

// part of harmony proxies
  Local<Function> istrapping = NanNew<FunctionTemplate>(IsLocked)->GetFunction();
  Local<String> _isTrapping = NanSymbol("isTrapping");
  istrapping->SetName(_isTrapping);
  target->Set(_isTrapping, istrapping, DontDelete);

// ECMAScript 5
  Local<Function> getOwnPropertyDescriptor = NanNew<FunctionTemplate>(GetOwnPropertyDescriptor)->GetFunction();
  Local<String> _getOwnPropertyDescriptor = NanSymbol("getOwnPropertyDescriptor");
  getOwnPropertyDescriptor->SetName(_getOwnPropertyDescriptor);
  target->Set(_getOwnPropertyDescriptor, getOwnPropertyDescriptor, DontDelete);

  Local<Function> defineProperty = NanNew<FunctionTemplate>(DefineProperty)->GetFunction();
  Local<String> _defineProperty = NanSymbol("defineProperty");
  defineProperty->SetName(_defineProperty);
  target->Set(_defineProperty, defineProperty, DontDelete);

  Local<Function> defineProperties = NanNew<FunctionTemplate>(DefineProperties)->GetFunction();
  Local<String> _defineProperties = NanSymbol("defineProperties");
  defineProperties->SetName(_defineProperties);
  target->Set(_defineProperties, defineProperties, DontDelete);

// additional functions
  Local<Function> clone = NanNew<FunctionTemplate>(Clone)->GetFunction();
  Local<String> _clone = NanSymbol("clone");
  clone->SetName(_clone);
  target->Set(_clone, clone, DontDelete);

  Local<Function> hidden = NanNew<FunctionTemplate>(Hidden)->GetFunction();
  Local<String> _hidden = NanSymbol("hidden");
  hidden->SetName(_hidden);
  target->Set(_hidden, hidden, DontDelete);

  Local<Function> setPrototype = NanNew<FunctionTemplate>(SetPrototype)->GetFunction();
  Local<String> _setPrototype = NanSymbol("setPrototype");
  setPrototype->SetName(_setPrototype);
  target->Set(_setPrototype, setPrototype, DontDelete);

  Local<Function> isProxy_ = NanNew<FunctionTemplate>(IsProxy)->GetFunction();
  Local<String> _isProxy = NanSymbol("isProxy");
  hidden->SetName(_isProxy);
  target->Set(_isProxy, isProxy_, DontDelete);

  Local<ObjectTemplate> temp = NanNew<ObjectTemplate>();

  temp->SetInternalFieldCount(1);

  // named property handlers
  temp->SetNamedPropertyHandler(GetNamedProperty,
                  SetNamedProperty,
                  QueryNamedPropertyInteger,
                  DeleteNamedProperty,
                  EnumerateNamedProperties);

  // indexed property handlers
  // TODO: properly implement arrays
  temp->SetIndexedPropertyHandler(GetIndexedProperty,
                  SetIndexedProperty,
                  QueryIndexedPropertyInteger,
                  DeleteIndexedProperty);

  Local<ObjectTemplate> _objectCreator = NanNew(temp);
  NanAssignPersistent(ObjectCreator, _objectCreator);

  Local<ObjectTemplate> instance = NanNew<ObjectTemplate>();
  instance->SetCallAsFunctionHandler(NodeProxy::New);
  instance->SetInternalFieldCount(1);

  instance->SetNamedPropertyHandler(GetNamedProperty,
                    SetNamedProperty,
                    QueryNamedPropertyInteger,
                    DeleteNamedProperty,
                    EnumerateNamedProperties);

  instance->SetIndexedPropertyHandler(GetIndexedProperty,
                    SetIndexedProperty,
                    QueryIndexedPropertyInteger,
                    DeleteIndexedProperty);

  Local<ObjectTemplate> _functionCreator = Handle<ObjectTemplate>(instance);
  NanAssignPersistent(FunctionCreator, _functionCreator);
}

/**
 * Required by Node for initializing the module
 *
 */
void init(Handle<Object> exports) {
  NodeProxy::Init(exports);
}
/* Required by windows Node version to detect the entry method */
NODE_MODULE(nodeproxy, init)
