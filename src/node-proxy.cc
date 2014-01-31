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
    Boolean::New(
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
  NanReturnValue(Boolean::New(args[0]->ToObject()->SetPrototype(args[1])));
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

    NanReturnValue(Boolean::New(!temp.IsEmpty() && temp->IsObject()));
  }

  NanReturnValue(False());
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
  proxyHandler->SetHiddenValue(NanSymbol("trapping"), True());
  proxyHandler->SetHiddenValue(NanSymbol("extensible"), True());
  proxyHandler->SetHiddenValue(NanSymbol("sealed"), False());
  proxyHandler->SetHiddenValue(NanSymbol("frozen"), False());

  Local<Object> instance = NanPersistentToLocal<ObjectTemplate>(ObjectCreator)->NewInstance();

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
                 : NanNewLocal<Value>(Undefined()));
  // manage locking states
  proxyHandler->SetHiddenValue(NanSymbol("trapping"), True());
  proxyHandler->SetHiddenValue(NanSymbol("extensible"), True());
  proxyHandler->SetHiddenValue(NanSymbol("sealed"), False());
  proxyHandler->SetHiddenValue(NanSymbol("frozen"), False());


  Local<Object> fn = NanPersistentToLocal<ObjectTemplate>(FunctionCreator)->NewInstance();
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
             String::New(" requires at least one (1) argument.")));
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
      NanReturnValue(True());
    }

  } else if (name->Equals(NanSymbol("seal"))) {
    if (handler->GetHiddenValue(NanSymbol("sealed"))->BooleanValue()) {
      NanReturnValue(True());
    }

  } else if (name->Equals(NanSymbol("preventExtensions"))) {
    if (handler->GetHiddenValue(NanSymbol("extensible"))->BooleanValue()) {
      NanReturnValue(True());
    }
  }

  // if this object is not trapping, just set the appropriate parameters
  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    if (name->Equals(NanSymbol("freeze"))) {
      handler->SetHiddenValue(NanSymbol("frozen"), True());
      handler->SetHiddenValue(NanSymbol("sealed"), True());
      handler->SetHiddenValue(NanSymbol("extensible"), False());
      NanReturnValue(True());

    } else if (name->Equals(NanSymbol("seal"))) {
      handler->SetHiddenValue(NanSymbol("sealed"), True());
      handler->SetHiddenValue(NanSymbol("extensible"), False());
      NanReturnValue(True());

    } else if (name->Equals(NanSymbol("preventExtensions"))) {
      handler->SetHiddenValue(NanSymbol("extensible"), False());
      NanReturnValue(True());
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
    parts->SetHiddenValue(NanSymbol("frozen"), True());
    parts->SetHiddenValue(NanSymbol("sealed"), True());
    parts->SetHiddenValue(NanSymbol("extensible"), False());

  } else if (name->Equals(NanSymbol("seal"))) {
    parts->SetHiddenValue(NanSymbol("sealed"), True());
    parts->SetHiddenValue(NanSymbol("extensible"), False());

  } else if (name->Equals(NanSymbol("preventExtensions"))) {
    parts->SetHiddenValue(NanSymbol("extensible"), False());
  }

  parts->SetHiddenValue(NanSymbol("trapping"), False());

  // overwrite the handler, making handler available for GC
  obj->SetInternalField(0, parts);

  NanReturnValue(True());
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
             String::New(" requires at least one (1) argument.")));
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

  NanReturnValue(False());
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
    NanReturnValue(False());
  }

  if (!handler->GetHiddenValue(NanSymbol("extensible"))->BooleanValue() &&
        !handler->Has(name)) {
    NanReturnValue(False());
  }

  if (!handler->GetHiddenValue(NanSymbol("trapping"))->BooleanValue()) {
    Local<Object> desc = handler->Get(name)->ToObject();

    if (desc->Get(NanSymbol("configurable"))->BooleanValue()) {
      NanReturnValue(Boolean::New(
          handler->Set(name, args[2]->ToObject())));
    }
    NanReturnValue(False());
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
      NanReturnValue(False());
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
                  String::New("Unable to define property: "),
                  name->ToString()));
            }
          }
        } else {
          NanThrowError(String::Concat(
                  String::New("Unable to define property: "),
                  name->ToString()));
        }
      }
      NanReturnValue(True());
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
    NanReturnValue(True());
  }
  NanReturnValue(False());
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

Local<Value> NodeProxy::CallPropertyDescriptorGet(Local<Value> descriptor, Handle<Object> context, Local<Value> args[1]) {
  if (descriptor->IsObject()) {
    Local<Value> get = descriptor->ToObject()->Get(NanSymbol("get"));

    if (get->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(get);
      return fn->Call(context, 1, args);
    }

    return descriptor->ToObject()->Get(NanSymbol("value"));
  }

  return NanNewLocal<Value>(Undefined());
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
                String::New("In accessible property: "),
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

Local<Value> NodeProxy::CallPropertyDescriptorSet(Local<Value> descriptor, Handle<Object> context, Local<Value> name, Local<Value> value) {
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

  return NanNewLocal<Value>(Undefined());
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
  Local<Integer> HasPropertyResponse = Integer::New(None);

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
    // return Integer::New(DontDelete);
    ret &= DontDelete;
  }

  if (pd->Get(NanSymbol("enumerable"))->IsBoolean() &&
             !pd->Get(NanSymbol("enumerable"))->BooleanValue()) {
    // return Integer::New(DontEnum);
    ret &= DontEnum;
  }

  if (pd->Get(NanSymbol("writable"))->IsBoolean() &&
             !pd->Get(NanSymbol("writable"))->BooleanValue()) {
     // return Integer::New(ReadOnly);
     ret &= ReadOnly;
  }

  return Integer::New(ret);
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
      NanReturnValue(False());
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
            NanReturnValue(Boolean::New(handler->Delete(property)));
          }
        }
      }
      NanReturnValue(False());
    }

    Local<Value> delete_ = handler->Get(NanSymbol("delete"));
    if (delete_->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(delete_);
      Local<Value> argv[1] = {property};
      NanReturnValue(fn->Call(handler, 1, argv)->ToBoolean());
    }
  }

  NanReturnValue(Boolean::New(false));
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
      NanReturnValue(Array::New());
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

  NanReturnValue(Array::New());
}

/**
 *  Invoked for accessing the given indexed property of an object
 *
 *
 *
 */
NAN_INDEX_GETTER(NodeProxy::GetIndexedProperty) {
  NanScope();

  return GetNamedProperty(Local<String>::Cast(
                Integer::NewFromUnsigned(index)),
              args);
}

/**
 *  Invoked for setting the given indexed property of an object
 *
 *
 *
 */
NAN_INDEX_SETTER(NodeProxy::SetIndexedProperty) {
  NanScope();

  return SetNamedProperty(Local<String>::Cast(
                  Integer::NewFromUnsigned(index)),
              value,
              args);
}

/**
 *  Invoked for determining if an object has a given indexed property
 *
 *
 *
 */
NAN_INDEX_QUERY(NodeProxy::QueryIndexedPropertyInteger) {
  NanScope();

  return QueryNamedPropertyInteger(
        Local<String>::Cast(Integer::NewFromUnsigned(index)),
        args);
}

/**
 *  Invoked for deleting a given indexed property
 *
 *
 *
 */
NAN_INDEX_DELETER(NodeProxy::DeleteIndexedProperty) {
  NanScope();

  return DeleteNamedProperty(
          Local<String>::Cast(Integer::NewFromUnsigned(index)),
          args);
}

/**
 *  Initialize the NodeProxy Strings and functions
 *
 *
 *
 */
void NodeProxy::Init(Handle<Object> target) {
  NanScope();

// required properties
  //NanSymbol("getPropertyDescriptor");
  //NanSymbol("getOwnPropertyNames");
  //NanSymbol("getPropertyNames");
  //NanSymbol("defineProperty");
  //NanSymbol("delete");
  //NanSymbol("fix");
// optional properties
  //NanSymbol("has");
  //NanSymbol("hasOwn");
  //NanSymbol("get");
  //NanSymbol("set");
  //NanSymbol("enumerate");
  //NanSymbol("keys");
// createFunction
  //NanSymbol("callTrap");
  //NanSymbol("constructorTrap");
// properties of PropertyDescriptor
  //NanSymbol("value");
  //NanSymbol("writable");
  //NanSymbol("enumerable");
  //NanSymbol("configurable");
// hidden property names
  //NanSymbol("trapping");
  //NanSymbol("sealed");
  //NanSymbol("frozen");
  //NanSymbol("extensible");
// fixable calls
  //NanSymbol("seal");
  //NanSymbol("freeze");
  //NanSymbol("preventExtensions");
// fixed checks
  //NanSymbol("isSealed");
  //NanSymbol("isFrozen");
  //NanSymbol("isExtensible");
  //NanSymbol("isTrapping");
  //NanSymbol("isProxy");
// namespacing for hidden properties of visible objects
  //NanSymbol("NodeProxy::hidden::");

// function creation

// main functions
  Local<String> createName = String::New("create");
  Local<Function> create = FunctionTemplate::New(Create)->GetFunction();
  create->SetName(createName);
  target->Set(createName, create, DontDelete);

  Local<String> createFunctionName = String::New("createFunction");
  Local<Function> createFunction = FunctionTemplate::New(CreateFunction)->GetFunction();
  create->SetName(createFunctionName);
  target->Set(createFunctionName, createFunction, DontDelete);

// freeze function assignment
  Local<Function> freeze = FunctionTemplate::New(Freeze)->GetFunction();
  freeze->SetName(NanSymbol("freeze"));
  target->Set(NanSymbol("freeze"), freeze, DontDelete);

  Local<Function> seal = FunctionTemplate::New(Freeze)->GetFunction();
  seal->SetName(NanSymbol("seal"));
  target->Set(NanSymbol("seal"), seal, DontDelete);

  Local<Function> prevent = FunctionTemplate::New(Freeze)->GetFunction();
  prevent->SetName(NanSymbol("preventExtensions"));
  target->Set(NanSymbol("preventExtensions"), prevent, DontDelete);

// check function assignment
  Local<Function> isfrozen = FunctionTemplate::New(IsLocked)->GetFunction();
  isfrozen->SetName(NanSymbol("isFrozen"));
  target->Set(NanSymbol("isFrozen"), isfrozen, DontDelete);

  Local<Function> issealed = FunctionTemplate::New(IsLocked)->GetFunction();
  issealed->SetName(NanSymbol("isSealed"));
  target->Set(NanSymbol("isSealed"), issealed, DontDelete);

  Local<Function> isextensible = FunctionTemplate::New(IsLocked)->GetFunction();
  isextensible->SetName(NanSymbol("isExtensible"));
  target->Set(NanSymbol("isExtensible"), isextensible, DontDelete);

// part of harmony proxies
  Local<Function> istrapping = FunctionTemplate::New(IsLocked)->GetFunction();
  istrapping->SetName(NanSymbol("isTrapping"));
  target->Set(NanSymbol("isTrapping"), istrapping, DontDelete);

// ECMAScript 5
  Local<String> getOwnPropertyDescriptorName = String::New("getOwnPropertyDescriptor");
  Local<Function> getOwnPropertyDescriptor = FunctionTemplate::New(GetOwnPropertyDescriptor)->GetFunction();
  getOwnPropertyDescriptor->SetName(getOwnPropertyDescriptorName);
  target->Set(getOwnPropertyDescriptorName,
        getOwnPropertyDescriptor,
        DontDelete);

  Local<String> definePropertyName = String::New("defineProperty");
  Local<Function> defineProperty = FunctionTemplate::New(DefineProperty)->GetFunction();
  defineProperty->SetName(definePropertyName);
  target->Set(definePropertyName, defineProperty, DontDelete);

  Local<String> definePropertiesName = String::New("defineProperties");
  Local<Function> defineProperties = FunctionTemplate::New(DefineProperties)->GetFunction();
  defineProperties->SetName(definePropertiesName);
  target->Set(definePropertiesName, defineProperties, DontDelete);

// additional functions
  Local<String> cloneName = String::New("clone");
  Local<Function> clone = FunctionTemplate::New(Clone)->GetFunction();
  clone->SetName(cloneName);
  target->Set(cloneName, clone, DontDelete);

  Local<String> hiddenName = String::New("hidden");
  Local<Function> hidden = FunctionTemplate::New(Hidden)->GetFunction();
  hidden->SetName(hiddenName);
  target->Set(hiddenName, hidden, DontDelete);

  Local<String> setPrototypeName = String::New("setPrototype");
  Local<Function> setPrototype = FunctionTemplate::New(SetPrototype)->GetFunction();
  setPrototype->SetName(setPrototypeName);
  target->Set(setPrototypeName, setPrototype, DontDelete);

  Local<Function> isProxy_ = FunctionTemplate::New(IsProxy)->GetFunction();
  hidden->SetName(NanSymbol("isProxy"));
  target->Set(NanSymbol("isProxy"), isProxy_, DontDelete);

  Local<ObjectTemplate> temp = ObjectTemplate::New();

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

  Local<ObjectTemplate> _objectCreator = NanNewLocal<ObjectTemplate>(temp);
  NanAssignPersistent(ObjectTemplate, ObjectCreator, _objectCreator);

  Local<ObjectTemplate> instance = ObjectTemplate::New();
  instance->SetCallAsFunctionHandler(New, Undefined());
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

  Local<ObjectTemplate> _functionCreator = NanNewLocal<ObjectTemplate>(instance);
  NanAssignPersistent(ObjectTemplate, FunctionCreator, _functionCreator);
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