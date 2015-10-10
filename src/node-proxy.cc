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

Nan::Persistent<ObjectTemplate> NodeProxy::ObjectCreator;
Nan::Persistent<ObjectTemplate> NodeProxy::FunctionCreator;

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

  if (info.Length() < 1) {
    Nan::ThrowError("clone requires at least one (1) argument.");
    return;
  }

  if (info[0]->IsString()) {
    info.GetReturnValue().Set(info[0]->ToObject()->Clone()->ToString());
    return;

  } else if (info[0]->IsBoolean()) {
    info.GetReturnValue().Set(info[0]->ToObject()->Clone()->ToBoolean());
    return;

  } else if (info[0]->IsNumber()
        || info[0]->IsNumberObject()
        || info[0]->IsInt32()
        || info[0]->IsUint32()) {
    info.GetReturnValue().Set(Nan::New<v8::Number>(info[0]->NumberValue()));
    return;

  } else if (info[0]->IsArray()) {
    info.GetReturnValue().Set(Local<Array>::Cast(info[0]->ToObject()->Clone()));
    return;

  } else if (info[0]->IsDate()) {
    info.GetReturnValue().Set(Local<Date>::Cast(info[0]->ToObject()->Clone()));
    return;

  } else if (info[0]->IsFunction()) {
    info.GetReturnValue().Set(Local<Function>::Cast(info[0])->Clone());
    return;

  } else if (info[0]->IsNull()) {
    info.GetReturnValue().SetNull();
    return;

  } else if (info[0]->IsUndefined()) {
    info.GetReturnValue().SetUndefined();
    return;

  } else if (info[0]->IsObject()) {
    info.GetReturnValue().Set(info[0]->ToObject()->Clone());
    return;
  }

  Nan::ThrowError("clone cannot determine the type of the argument.");
  return;
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

  if (info.Length() < 2) {
    Nan::ThrowError("hidden requires at least two (2) arguments.");
    return;
  }

  Local<Object> obj = info[0]->ToObject();

  if (info.Length() < 3) {
    info.GetReturnValue().Set(obj->GetHiddenValue(
          String::Concat(Nan::New<String>("NodeProxy::hidden:").ToLocalChecked(),
                   Nan::To<v8::String>(info[1]).ToLocalChecked())));
    return;
  }

  info.GetReturnValue().Set(
    Nan::New<Boolean>(
        obj->SetHiddenValue(String::Concat(Nan::New<String>("NodeProxy::hidden:").ToLocalChecked(),
                           Nan::To<v8::String>(info[1]).ToLocalChecked()),
        info[2])));
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

  if (info.Length() < 2) {
    Nan::ThrowError("setPrototype requires at least two (2) arguments.");
    return;
  }
  info.GetReturnValue().Set(Nan::New<Boolean>(info[0]->ToObject()->SetPrototype(info[1])));
}

/**
 *  Determine if an Object was created by Proxy
 *
 *  @param Object
 *  @returns Boolean
 */
NAN_METHOD(NodeProxy::IsProxy) {

  if (info.Length() < 1) {
    Nan::ThrowError("isProxy requires at least one (1) argument.");
    return;
  }

  Local<Object> obj = info[0]->ToObject();

  if (obj->InternalFieldCount() > 0) {
    Local<Value> temp = obj->GetInternalField(0);

    info.GetReturnValue().Set(Nan::New<Boolean>(!temp.IsEmpty() && temp->IsObject()));
    return;
  }

  info.GetReturnValue().Set(Nan::False());
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

  Local<Object> proxyHandler;

  if (info.Length() < 1) {
    Nan::ThrowError("create requires at least one (1) argument.");
    return;
  }

  if (!info[0]->IsObject()) {
    Nan::ThrowTypeError(
        "create requires the first argument to be an Object.");
    return;
  }

  proxyHandler = info[0]->ToObject();

  if (info.Length() > 1 && !info[1]->IsObject()) {
    Nan::ThrowTypeError(
        "create requires the second argument to be an Object.");
    return;
  }

  // manage locking states
  proxyHandler->SetHiddenValue(Nan::New<String>("trapping").ToLocalChecked(), Nan::True());
  proxyHandler->SetHiddenValue(Nan::New<String>("extensible").ToLocalChecked(), Nan::True());
  proxyHandler->SetHiddenValue(Nan::New<String>("sealed").ToLocalChecked(), Nan::False());
  proxyHandler->SetHiddenValue(Nan::New<String>("frozen").ToLocalChecked(), Nan::False());

  Local<Object> instance = Nan::New<ObjectTemplate>(ObjectCreator)->NewInstance();

  instance->SetInternalField(0, proxyHandler);

  if (info.Length() > 1) {
    instance->SetPrototype(info[1]);
  }

  info.GetReturnValue().Set(instance);
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

  if (info.Length() < 2) {
    Nan::ThrowError("createFunction requires at least two (2) arguments.");
    return;
  }

  if (!info[0]->IsObject()) {
    Nan::ThrowTypeError("createFunction requires the first argument to be an Object.");
    return;
  }
  Local<Object> proxyHandler = info[0]->ToObject();

  if (!info[1]->IsFunction()) {
    Nan::ThrowTypeError("createFunction requires the second argument to be a Function.");
    return;
  }

  if (info.Length() > 2 && !info[2]->IsFunction()) {
    Nan::ThrowTypeError("createFunction requires the second argument to be a Function.");
    return;
  }

  proxyHandler->SetHiddenValue(Nan::New<String>("callTrap").ToLocalChecked(), info[1]);
  Local<Value> constructorTrap;
  if(info.Length() > 2) {
    constructorTrap = info[2];
  } else {
    constructorTrap = Nan::Undefined();
  }
  proxyHandler->SetHiddenValue(Nan::New<String>("constructorTrap").ToLocalChecked(), constructorTrap);

  // manage locking states
  proxyHandler->SetHiddenValue(Nan::New<String>("trapping").ToLocalChecked(), Nan::True());
  proxyHandler->SetHiddenValue(Nan::New<String>("extensible").ToLocalChecked(), Nan::True());
  proxyHandler->SetHiddenValue(Nan::New<String>("sealed").ToLocalChecked(), Nan::False());
  proxyHandler->SetHiddenValue(Nan::New<String>("frozen").ToLocalChecked(), Nan::False());


  Local<Object> fn = Nan::New<ObjectTemplate>(FunctionCreator)->NewInstance();
  fn->SetPrototype(info[1]->ToObject()->GetPrototype());

  fn->SetInternalField(0, proxyHandler);

  info.GetReturnValue().Set(fn);
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

  Local<String> name = info.Callee()->GetName()->ToString();

  if (info.Length() < 1) {
    Nan::ThrowError(String::Concat(name,
             Nan::New<String>(" requires at least one (1) argument.").ToLocalChecked()));
    return;
  }

  Local<Object> obj = info[0]->ToObject();

  if (obj->InternalFieldCount() < 1) {
    Nan::ThrowTypeError(
      "Locking functions expect first "
      "argument to be intialized by Proxy");
    return;
  }

  Local<Value> hide = obj->GetInternalField(0);

  if (hide.IsEmpty() || !hide->IsObject()) {
    Nan::ThrowTypeError(
      "Locking functions expect first "
      "argument to be intialized by Proxy");
    return;
  }

  Local<Object> handler = hide->ToObject();

  // if the object already meets the requirements of the function call
  if (name->Equals(Nan::New<String>("freeze").ToLocalChecked())) {
    if (handler->GetHiddenValue(Nan::New<String>("frozen").ToLocalChecked())->BooleanValue()) {
      info.GetReturnValue().Set(Nan::True());
      return;
    }

  } else if (name->Equals(Nan::New<String>("seal").ToLocalChecked())) {
    if (handler->GetHiddenValue(Nan::New<String>("sealed").ToLocalChecked())->BooleanValue()) {
      info.GetReturnValue().Set(Nan::True());
      return;
    }

  } else if (name->Equals(Nan::New<String>("preventExtensions").ToLocalChecked())) {
    if (handler->GetHiddenValue(Nan::New<String>("extensible").ToLocalChecked())->BooleanValue()) {
      info.GetReturnValue().Set(Nan::True());
      return;
    }
  }

  // if this object is not trapping, just set the appropriate parameters
  if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
    if (name->Equals(Nan::New<String>("freeze").ToLocalChecked())) {
      handler->SetHiddenValue(Nan::New<String>("frozen").ToLocalChecked(), Nan::True());
      handler->SetHiddenValue(Nan::New<String>("sealed").ToLocalChecked(), Nan::True());
      handler->SetHiddenValue(Nan::New<String>("extensible").ToLocalChecked(), Nan::False());
      info.GetReturnValue().Set(Nan::True());
      return;

    } else if (name->Equals(Nan::New<String>("seal").ToLocalChecked())) {
      handler->SetHiddenValue(Nan::New<String>("sealed").ToLocalChecked(), Nan::True());
      handler->SetHiddenValue(Nan::New<String>("extensible").ToLocalChecked(), Nan::False());
      info.GetReturnValue().Set(Nan::True());
      return;

    } else if (name->Equals(Nan::New<String>("preventExtensions").ToLocalChecked())) {
      handler->SetHiddenValue(Nan::New<String>("extensible").ToLocalChecked(), Nan::False());
      info.GetReturnValue().Set(Nan::True());
      return;
    }
  }

  // Harmony Proxy handling of fix
  Local<Function> fix = Local<Function>::Cast(handler->Get(Nan::New<String>("fix").ToLocalChecked()));
#ifdef _WIN32
  // On windows you get "error C2466: cannot allocate an array of constant size 0" and we use a pointer
  Local<Value>* argv;
#else
  Local<Value> argv[0];
#endif
  Local<Value> pieces = fix->Call(info[0]->ToObject(), 0, argv);

  if (pieces.IsEmpty() || !pieces->IsObject()) {
    Nan::ThrowTypeError("Cannot lock object.");
    return;
  }

  Local<Object> parts = pieces->ToObject();

  // set the appropriate parameters
  if (name->Equals(Nan::New<String>("freeze").ToLocalChecked())) {
    parts->SetHiddenValue(Nan::New<String>("frozen").ToLocalChecked(), Nan::True());
    parts->SetHiddenValue(Nan::New<String>("sealed").ToLocalChecked(), Nan::True());
    parts->SetHiddenValue(Nan::New<String>("extensible").ToLocalChecked(), Nan::False());

  } else if (name->Equals(Nan::New<String>("seal").ToLocalChecked())) {
    parts->SetHiddenValue(Nan::New<String>("sealed").ToLocalChecked(), Nan::True());
    parts->SetHiddenValue(Nan::New<String>("extensible").ToLocalChecked(), Nan::False());

  } else if (name->Equals(Nan::New<String>("preventExtensions").ToLocalChecked())) {
    parts->SetHiddenValue(Nan::New<String>("extensible").ToLocalChecked(), Nan::False());
  }

  parts->SetHiddenValue(Nan::New<String>("trapping").ToLocalChecked(), Nan::False());

  // overwrite the handler, making handler available for GC
  obj->SetInternalField(0, parts);

  info.GetReturnValue().Set(Nan::True());
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

  Local<String> name = info.Callee()->GetName()->ToString();

  if (info.Length() < 1) {
    Nan::ThrowError(String::Concat(name,
             Nan::New<String>(" requires at least one (1) argument.").ToLocalChecked()));
    return;
  }

  Local<Object> arg = info[0]->ToObject();

  if (arg->InternalFieldCount() < 1) {
    Nan::ThrowTypeError(
       "Locking functions expect first argument "
       "to be intialized by Proxy");
    return;
  }

  Local<Value> hide = arg->GetInternalField(0);

  if (hide.IsEmpty() || !hide->IsObject()) {
    Nan::ThrowTypeError(
      "Locking functions expect first argument "
      "to be intialized by Proxy");
    return;
  }

  Local<Object> obj = hide->ToObject();

  if (name->Equals(Nan::New<String>("isExtensible").ToLocalChecked())) {
    info.GetReturnValue().Set(obj->GetHiddenValue(Nan::New<String>("extensible").ToLocalChecked())->ToBoolean());
    return;

  } else if (name->Equals(Nan::New<String>("isSealed").ToLocalChecked())) {
    info.GetReturnValue().Set(obj->GetHiddenValue(Nan::New<String>("sealed").ToLocalChecked())->ToBoolean());
    return;

  } else if (name->Equals(Nan::New<String>("isTrapping").ToLocalChecked())) {
    info.GetReturnValue().Set(obj->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->ToBoolean());
    return;

  } else if (name->Equals(Nan::New<String>("isFrozen").ToLocalChecked())) {
    info.GetReturnValue().Set(obj->GetHiddenValue(Nan::New<String>("frozen").ToLocalChecked())->ToBoolean());
    return;
  }

  info.GetReturnValue().Set(Nan::False());
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

  if (info.Length() < 2) {
    Nan::ThrowError("getOwnPropertyDescriptor requires "
        "at least two (2) arguments.");
    return;
  }

  if (!info[1]->IsString() && !info[1]->IsNumber()) {
    Nan::ThrowTypeError("getOwnPropertyDescriptor requires "
           "the second argument to be a String or a Number.");
    return;
  }

  Local<Object> obj = info[0]->ToObject();
  Local<String> name = info[1]->ToString();

  if (obj->InternalFieldCount() < 1) {
    Nan::ThrowTypeError("getOwnPropertyDescriptor expects "
            "first argument to be intialized by Proxy");
    return;
  }

  Local<Value> temp = obj->GetInternalField(0);

  if (temp.IsEmpty() || !temp->IsObject()) {
    Nan::ThrowTypeError("getOwnPropertyDescriptor expects "
            "first argument to be intialized by Proxy");
    return;
  }

  Local<Object> handler = temp->ToObject();

  if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
    info.GetReturnValue().Set(handler->Get(name));
    return;
  }

  Local<Function> getOwn = Local<Function>::Cast(
       handler->Get(Nan::New<String>("getOwnPropertyDescriptor").ToLocalChecked()));

  Local<Value> argv[1] = {info[1]};
  info.GetReturnValue().Set(getOwn->Call(obj, 1, argv));
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

  if (info.Length() < 3) {
    Nan::ThrowError("defineProperty requires at least three (3) arguments.");
  }

  if (!info[1]->IsString() && !info[1]->IsNumber()) {
    Nan::ThrowTypeError("defineProperty requires the "
                "second argument to be a String or a Number.");
    return;
  }

  if (!info[2]->IsObject()) {
    Nan::ThrowTypeError("defineProperty requires the third argument "
            "to be an Object of the type PropertyDescriptor.");
    return;
  }

  Local<Object> obj = info[0]->ToObject();

  if (obj->InternalFieldCount() < 1) {
    Nan::ThrowTypeError("defineProperty expects first "
                "argument to be intialized by Proxy");
    return;
  }

  Local<Value> temp = obj->GetInternalField(0);

  if (temp.IsEmpty() || !temp->IsObject()) {
    Nan::ThrowTypeError("defineProperty expects first argument "
                "to be intialized by Proxy");
    return;
  }

  Local<String> name = info[1]->ToString();
  Local<Object> handler = temp->ToObject();

  if (handler->GetHiddenValue(Nan::New<String>("sealed").ToLocalChecked())->BooleanValue() ||
  !handler->Has(Nan::New<String>("defineProperty").ToLocalChecked())) {
    info.GetReturnValue().Set(Nan::False());
    return;
  }

  if (!handler->GetHiddenValue(Nan::New<String>("extensible").ToLocalChecked())->BooleanValue() &&
        !handler->Has(name)) {
    info.GetReturnValue().Set(Nan::False());
    return;
  }

  if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
    Local<Object> desc = handler->Get(name)->ToObject();

    if (desc->Get(Nan::New<String>("configurable").ToLocalChecked())->BooleanValue()) {
      info.GetReturnValue().Set(Nan::New<Boolean>(
          handler->Set(name, info[2]->ToObject())));
      return;
    }
    info.GetReturnValue().Set(Nan::False());
    return;
  }

  Local<Function> def = Local<Function>::Cast(
                  handler->Get(Nan::New<String>("defineProperty").ToLocalChecked()));

  Local<Value> argv[2] = {info[1], info[2]->ToObject()};

  info.GetReturnValue().Set(def->Call(obj, 2, argv)->ToBoolean());
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

  if (info.Length() < 2) {
    Nan::ThrowError("defineProperty requires at least three (3) arguments.");
    return;
  }

  if (!info[1]->IsObject()) {
    Nan::ThrowTypeError("defineProperty requires the third argument "
             "to be an Object of the type PropertyDescriptor.");
    return;
  }

  Local<Object> obj = info[0]->ToObject();

  if (obj->InternalFieldCount() < 1) {
    Nan::ThrowTypeError("defineProperty expects first "
                "argument to be intialized by Proxy");
    return;
  }

  Local<Value> temp = obj->GetInternalField(0);

  if (!temp.IsEmpty() && temp->IsObject()) {
    Local<Object> props = info[1]->ToObject();
    Local<Object> handler = temp->ToObject();

    if (handler->GetHiddenValue(Nan::New<String>("sealed").ToLocalChecked())->BooleanValue()) {
      info.GetReturnValue().Set(Nan::False());
      return;
    }

    bool extensible = handler->GetHiddenValue(
                  Nan::New<String>("extensible").ToLocalChecked())->BooleanValue();
    Local<Array> names = props->GetPropertyNames();
    uint32_t i = 0, l = names->Length();

    if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
      for (;i < l; ++i) {
        Local<Object> name = names->CloneElementAt(i);

        if (handler->Has(name->ToString()) &&
            handler->Get(name->ToString())->IsObject()
        ) {
          Local<Object> tempObj =         handler->Get(name->ToString())->ToObject();

          if (tempObj->Get(Nan::New<String>("configurable").ToLocalChecked())->BooleanValue()) {
            if (!handler->Set(name->ToString(),
                      props->Get(name->ToString()))) {
              Nan::ThrowError(
                String::Concat(
                  Nan::New<String>("Unable to define property: ").ToLocalChecked(),
                  name->ToString()));
              return;
            }
          }
        } else {
          Nan::ThrowError(String::Concat(
                  Nan::New<String>("Unable to define property: ").ToLocalChecked(),
                  name->ToString()));
          return;
        }
      }
      info.GetReturnValue().Set(Nan::True());
      return;
    }

    Local<Function> def =   Local<Function>::Cast(handler->Get(Nan::New<String>("defineProperty").ToLocalChecked()));

    TryCatch firstTry;
    for (;i < l; ++i) {
      Local<Value> name = names->Get(i);

      if (extensible || obj->Has(name->ToString())) {
        Local<Value> pd = props->Get(name->ToString());
        Local<Value> argv[2] = {name, pd};
        def->Call(obj, 2, argv);

        if (firstTry.HasCaught()) {
          firstTry.ReThrow();
          return;
        }
      }
    }
    info.GetReturnValue().Set(Nan::True());
    return;
  }
  info.GetReturnValue().Set(Nan::False());
}

/**
 *  Function used for a constructor and invocation
 *  handler of a Proxy created function
 *  Calls the appropriate function attached when the Proxy was created
 *
 *  @param ...info
 *  @returns mixed
 *  @throws Error
 */
NAN_METHOD(NodeProxy::New) {

  if (info.Callee()->InternalFieldCount() < 1 && info.Data().IsEmpty()) {
    Nan::ThrowTypeError("defineProperty expects first "
                "argument to be intialized by Proxy");
    return;
  }

  Local<Value> v, ret, data =  info.Holder()->GetInternalField(0);

  if (data.IsEmpty() || !data->IsObject()) {
    Nan::ThrowError("Invalid reference to Proxy#constructor");
    return;
  }

  Local<Function> fn;
  Local<Object> obj = data->ToObject();

  if (info.IsConstructCall()) {
    v = obj->GetHiddenValue(Nan::New<String>("constructorTrap").ToLocalChecked());

    if (!v.IsEmpty() && v->IsFunction()) {
      fn = Local<Function>::Cast(v);
    } else {
      fn = Local<Function>::Cast(
          obj->GetHiddenValue(Nan::New<String>("callTrap").ToLocalChecked()));
    }
  } else {
    fn = Local<Function>::Cast(obj->GetHiddenValue(Nan::New<String>("callTrap").ToLocalChecked()));
  }

  int i = 0, l = info.Length();
  Local<Value>* argv = new Local<Value>[l];

  for (; i < l; ++i) {
    argv[i] = info[i];
  }

  ret = fn->Call(info.This(), info.Length(), argv);

  if (info.IsConstructCall()) {
    if (!ret.IsEmpty()) {
      info.GetReturnValue().Set(ret);
      return;
    }
    info.GetReturnValue().Set(info.This());
    return;
  }
  info.GetReturnValue().Set(ret);
}

/**
 *  Invoked for accessing the named properties of an object
 *
 *
 *
 */
NAN_PROPERTY_GETTER(NodeProxy::GetNamedProperty) {

  if (info.This()->InternalFieldCount() < 1 || info.Data().IsEmpty()) {
    Nan::ThrowTypeError("SetNamedProperty intercepted "
                "by non-Proxy object");
    return;
  }

  Local<Value> argv1[1] = {property};
  Local<Value> data = info.This()->InternalFieldCount() > 0 ?
                      info.This()->GetInternalField(0) :
                      info.Data();

  if (!data->IsObject()) {
    info.GetReturnValue().SetUndefined();
    return;
  }

  Local<Function> fn;
  Local<Object> handler = data->ToObject();

  // if the Proxy isn't trapping, return
  // the value set on the property descriptor
  if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
    info.GetReturnValue().Set(CallPropertyDescriptorGet(handler->Get(property), info.This(), argv1));
    return;
  }

  Local<Value> get = handler->Get(Nan::New<String>("get").ToLocalChecked());
  if (get->IsFunction()) {
    fn = Local<Function>::Cast(get);
    Local<Value> argv[2] = {info.This(), property};

    info.GetReturnValue().Set(fn->Call(handler, 2, argv));
    return;
  }

  Local<Value> getPropertyDescriptor = handler->Get(Nan::New<String>("getPropertyDescriptor").ToLocalChecked());
  if (getPropertyDescriptor->IsFunction()) {
    fn = Local<Function>::Cast(getPropertyDescriptor);

    info.GetReturnValue().Set(CallPropertyDescriptorGet(fn->Call(handler, 1, argv1), info.This(), argv1));
    return;
  }

  Local<Value> getOwnPropertyDescriptor = handler->Get(Nan::New<String>("getOwnPropertyDescriptor").ToLocalChecked());
  if (getOwnPropertyDescriptor->IsFunction()) {
    fn = Local<Function>::Cast(getOwnPropertyDescriptor);

    info.GetReturnValue().Set(CallPropertyDescriptorGet(fn->Call(handler, 1, argv1), info.This(), argv1));
    return;
  }
  info.GetReturnValue().SetUndefined(); // <-- silence warnings for 0.10.x
}

NAN_INLINE Local<Value> NodeProxy::CallPropertyDescriptorGet(Local<Value> descriptor, Handle<Object> context, Local<Value> info[1]) {
  Nan::EscapableHandleScope scope;
  if (descriptor->IsObject()) {
    Local<Value> get = descriptor->ToObject()->Get(Nan::New<String>("get").ToLocalChecked());

    if (get->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(get);
      return fn->Call(context, 1, info);
    }

    return descriptor->ToObject()->Get(Nan::New<String>("value").ToLocalChecked());
  }

  return scope.Escape(Nan::Undefined());
}

/**
 *  Invoked for setting the named properties of an object
 *
 *
 *
 */
NAN_PROPERTY_SETTER(NodeProxy::SetNamedProperty) {

  if (info.This()->InternalFieldCount() < 1 || info.Data().IsEmpty()) {
    Nan::ThrowTypeError("SetNamedProperty intercepted "
                "by non-Proxy object");
    return;
  }

  Local<Value> argv2[2] = {property, value};
  Local<Value> data = info.This()->InternalFieldCount() > 0 ?
                      info.This()->GetInternalField(0) :
                      info.Data();

  if (!data->IsObject()) {
    info.GetReturnValue().SetUndefined();
    return;
  }

  Local<Object> handler = data->ToObject();

  // if the Proxy isn't trapping, return the
  // value set on the property descriptor
  if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
    if (handler->GetHiddenValue(Nan::New<String>("extensible").ToLocalChecked())->BooleanValue() ||
      handler->Has(property)
    ) {
      Local<Value> pd = handler->Get(property);

      if (!pd->IsObject()) {
        info.GetReturnValue().SetUndefined();
        return;
      }

      Local<Object> pd_obj = pd->ToObject();

      if (!pd_obj->GetHiddenValue(
            Nan::New<String>("writable").ToLocalChecked())->BooleanValue()
      ) {
        Nan::ThrowError(
              String::Concat(
                Nan::New<String>("In accessible property: ").ToLocalChecked(),
                    property));
        return;
      }

      Local<Value> set = pd_obj->Get(Nan::New<String>("set").ToLocalChecked());
      if (set->IsFunction()) {
        Local<Function> fn = Local<Function>::Cast(set);
        fn->Call(info.This(), 2, argv2);

        info.GetReturnValue().Set(value);
        return;
      }

      if (pd_obj->Set(Nan::New<String>("value").ToLocalChecked(), value)) {
        info.GetReturnValue().Set(value);
        return;
      }
      info.GetReturnValue().SetUndefined();
      return;
    }
    info.GetReturnValue().SetUndefined();
    return;
  }

  // does the ProxyHandler have a set method?
  Local<Value> set = handler->Get(Nan::New<String>("set").ToLocalChecked());
  if (set->IsFunction()) {
    Local<Function> set_fn = Local<Function>::Cast(set);
    Local<Value> argv3[3] = {info.This(), property, value};
    set_fn->Call(handler, 3, argv3);

    info.GetReturnValue().Set(value);
    return;
  }

  Local<Value> getOwnPropertyDescriptor = handler->Get(Nan::New<String>("getOwnPropertyDescriptor").ToLocalChecked());
  if (getOwnPropertyDescriptor->IsFunction()) {
    Local<Function> gopd_fn = Local<Function>::Cast(getOwnPropertyDescriptor);
    Local<Value> argv[1] = {property};
    info.GetReturnValue().Set(CallPropertyDescriptorSet(gopd_fn->Call(handler, 1, argv), info.This(), property, value));
    return;
  }

  Local<Value> getPropertyDescriptor = handler->Get(Nan::New<String>("getPropertyDescriptor").ToLocalChecked());
  if (getPropertyDescriptor->IsFunction()) {
    Local<Function> gpd_fn = Local<Function>::Cast(getPropertyDescriptor);
    Local<Value> argv[1] = {property};
    info.GetReturnValue().Set(CallPropertyDescriptorSet(gpd_fn->Call(handler, 1, argv), info.This(), property, value));
    return;
  }

  info.GetReturnValue().SetUndefined();
}

NAN_INLINE Local<Value> NodeProxy::CallPropertyDescriptorSet(Local<Value> descriptor, Handle<Object> context, Local<Value> name, Local<Value> value) {
  Nan::EscapableHandleScope scope;
  if (descriptor->IsObject()) {
    Local<Object> pd = descriptor->ToObject();
    Local<Value> set = pd->Get(Nan::New<String>("set").ToLocalChecked());

    if (set->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(set);
      Local<Value> info[2] = { name, value };
      return fn->Call(context, 2, info);

    } else if (pd->Get(Nan::New<String>("writable").ToLocalChecked())->BooleanValue()) {
      if (pd->Set(Nan::New<String>("value").ToLocalChecked(), value)) {
        return value;
      }
    }
  }

  return scope.Escape(Nan::Undefined());
}


/**
 *  Invoked for determining if an object has a specific property
 *
 *
 *
 */
NAN_PROPERTY_QUERY(NodeProxy::QueryNamedPropertyInteger) {

  Local<Integer> DoesntHavePropertyResponse;
  Local<Integer> HasPropertyResponse = Nan::New<Integer>(None);

  if (info.This()->InternalFieldCount() > 0 || !info.Data().IsEmpty()) {
    Local<Value> data = info.This()->InternalFieldCount() > 0 ?
                 info.This()->GetInternalField(0) :
                 info.Data();

    if (!data->IsObject()) {
      info.GetReturnValue().Set(DoesntHavePropertyResponse);
      return;
    }

    Local<Object> handler = data->ToObject();

    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
      if (handler->Has(property)) {
        Local<Value> pd = handler->Get(property);

        if (pd->IsObject()) {
          info.GetReturnValue().Set(GetPropertyAttributeFromPropertyDescriptor(pd->ToObject()));
          return;
        }
        info.GetReturnValue().Set(HasPropertyResponse);
        return;
      }
      info.GetReturnValue().Set(DoesntHavePropertyResponse);
      return;
    }

    Local<Value> argv[1] = {property};

    Local<Value> hasOwn = handler->Get(Nan::New<String>("hasOwn").ToLocalChecked());
    if (hasOwn->IsFunction()) {
      Local<Function> hasOwn_fn = Local<Function>::Cast(hasOwn);
      info.GetReturnValue().Set(hasOwn_fn->Call(handler, 1, argv)->BooleanValue() ?
                     HasPropertyResponse :
                     DoesntHavePropertyResponse);
      return;
    }

    Local<Value> has = handler->Get(Nan::New<String>("has").ToLocalChecked());
    if (has->IsFunction()) {
      Local<Function> has_fn = Local<Function>::Cast(has);
      info.GetReturnValue().Set(has_fn->Call(handler, 1, argv)->BooleanValue() ?
                     HasPropertyResponse :
                     DoesntHavePropertyResponse);
      return;
    }

    Local<Value> getOwnPropertyDescriptor = handler->Get(Nan::New<String>("getOwnPropertyDescriptor").ToLocalChecked());
    if (getOwnPropertyDescriptor->IsFunction()) {
      Local<Function> gopd_fn = Local<Function>::Cast(getOwnPropertyDescriptor);
      Local<Value> gopd_pd = gopd_fn->Call(handler, 1, argv);

      if (gopd_pd->IsObject()) {
        info.GetReturnValue().Set(GetPropertyAttributeFromPropertyDescriptor(gopd_pd->ToObject()));
        return;
      }
    }

    Local<Value> getPropertyDescriptor = handler->Get(Nan::New<String>("getPropertyDescriptor").ToLocalChecked());
    if (handler->Has(Nan::New<String>("getPropertyDescriptor").ToLocalChecked())) {
      Local<Function> gpd_fn = Local<Function>::Cast(getPropertyDescriptor);
      Local<Value> gpd_pd = gpd_fn->Call(handler, 1, argv);

      if (gpd_pd->IsObject()) {
        info.GetReturnValue().Set(GetPropertyAttributeFromPropertyDescriptor(gpd_pd->ToObject()));
        return;
      } else if (gpd_pd->IsUndefined()) {
        info.GetReturnValue().Set(DoesntHavePropertyResponse);
        return;
      }
    }
  }

  info.GetReturnValue().Set(DoesntHavePropertyResponse);
}

/**
 *  Find the appropriate PropertyAttribute
 *  for a given PropertyDescriptor object
 *
 *
 */
Local<Integer>
NodeProxy::GetPropertyAttributeFromPropertyDescriptor(Local<Object> pd) {
  uint32_t ret = None;

  if (pd->Get(Nan::New<String>("configurable").ToLocalChecked())->IsBoolean() &&
        !pd->Get(Nan::New<String>("configurable").ToLocalChecked())->BooleanValue()) {
    ret &= DontDelete;
  }

  if (pd->Get(Nan::New<String>("enumerable").ToLocalChecked())->IsBoolean() &&
             !pd->Get(Nan::New<String>("enumerable").ToLocalChecked())->BooleanValue()) {
    // return Nan::New<Integer>(DontEnum);
    ret &= DontEnum;
  }

  if (pd->Get(Nan::New<String>("writable").ToLocalChecked())->IsBoolean() &&
             !pd->Get(Nan::New<String>("writable").ToLocalChecked())->BooleanValue()) {
     // return Nan::New<Integer>(ReadOnly);
     ret &= ReadOnly;
  }

  return Nan::New<Integer>(ret);
}

/**
 *  Invoked when deleting the named property of an object
 *
 *
 *
 */
NAN_PROPERTY_DELETER(NodeProxy::DeleteNamedProperty) {

  if (info.This()->InternalFieldCount() > 0 || !info.Data().IsEmpty()) {
    Local<Value> data = info.This()->InternalFieldCount() > 0 ?
                 info.This()->GetInternalField(0) :
                 info.Data();

    if (!data->IsObject()) {
      info.GetReturnValue().Set(Nan::False());
      return;
    }

    Local<Object> handler = data->ToObject();
    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
      if (!handler->GetHiddenValue(Nan::New<String>("frozen").ToLocalChecked())->BooleanValue()) {
        Local<Value> pd = handler->Get(property);

        if (pd->IsObject()) {
          Local<Object> pd_obj = pd->ToObject();

          if (pd_obj->Get(Nan::New<String>("configurable").ToLocalChecked())->IsBoolean() &&
              pd_obj->Get(Nan::New<String>("configurable").ToLocalChecked())->BooleanValue()
          ) {
            info.GetReturnValue().Set(Nan::New<Boolean>(handler->Delete(property)));
            return;
          }
        }
      }
      info.GetReturnValue().Set(Nan::False());
      return;
    }

    Local<Value> delete_ = handler->Get(Nan::New<String>("delete").ToLocalChecked());
    if (delete_->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(delete_);
      Local<Value> argv[1] = {property};
      info.GetReturnValue().Set(fn->Call(handler, 1, argv)->ToBoolean());
      return;
    }
  }

  info.GetReturnValue().Set(Nan::False());
}

/**
 *  Invoked for enumerating all properties of an object
 *
 *
 *
 */
NAN_PROPERTY_ENUMERATOR(NodeProxy::EnumerateNamedProperties) {

  if (info.This()->InternalFieldCount() > 0 || !info.Data().IsEmpty()) {
    Local<Value> data = info.This()->InternalFieldCount() > 0 ?
        info.This()->GetInternalField(0) :
         info.Data();

    if (!data->IsObject()) {
      info.GetReturnValue().Set(Nan::New<Array>());
      return;
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
    if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
      info.GetReturnValue().Set(handler->GetPropertyNames());
      return;
    }

    Local<Value> enumerate = handler->Get(Nan::New<String>("enumerate").ToLocalChecked());
    if (enumerate->IsFunction()) {
      Local<Function> enumerate_fn = Local<Function>::Cast(enumerate);
      Local<Value> names = enumerate_fn->Call(handler, 0, argv);

      if (names->IsArray()) {
        info.GetReturnValue().Set(Local<Array>::Cast(names->ToObject()));
        return;
      }
    }

    Local<Value> keys = handler->Get(Nan::New<String>("keys").ToLocalChecked());
    if (keys->IsFunction()) {
      Local<Function> keys_fn = Local<Function>::Cast(enumerate);
      Local<Value> names = keys_fn->Call(handler, 0, argv);

      if (names->IsArray()) {
        info.GetReturnValue().Set(Local<Array>::Cast(names->ToObject()));
        return;
      }
    }

    Local<Value> getPropertyNames = handler->Get(Nan::New<String>("getPropertyNames").ToLocalChecked());
    if (getPropertyNames->IsFunction()) {
      Local<Function> gpn_fn = Local<Function>::Cast(getPropertyNames);
      Local<Value> names = gpn_fn->Call(handler, 0, argv);

      if (names->IsArray()) {
        info.GetReturnValue().Set(Local<Array>::Cast(names->ToObject()));
        return;
      }
    }
  }

  info.GetReturnValue().Set(Nan::New<Array>());
}

/**
 *  Invoked for accessing the given indexed property of an object
 *
 *
 *
 */
NAN_INDEX_GETTER(NodeProxy::GetIndexedProperty) {

  if (info.This()->InternalFieldCount() < 1 || info.Data().IsEmpty()) {
    Nan::ThrowTypeError("SetNamedProperty intercepted "
                "by non-Proxy object");
    return;
  }

  Local<Integer> idx = Nan::New<Integer>(index);
  Local<Value> argv1[1] = {idx};
  Local<Value> data = info.This()->InternalFieldCount() > 0 ?
                      info.This()->GetInternalField(0) :
                      info.Data();

  if (!data->IsObject()) {
    info.GetReturnValue().SetUndefined();
    return;
  }

  Local<Function> fn;
  Local<Object> handler = data->ToObject();

  // if the Proxy isn't trapping, return
  // the value set on the index descriptor
  if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
    info.GetReturnValue().Set(CallPropertyDescriptorGet(handler->Get(idx), info.This(), argv1));
    return;
  }

  Local<Value> get = handler->Get(Nan::New<String>("get").ToLocalChecked());
  if (get->IsFunction()) {
    fn = Local<Function>::Cast(get);
    Local<Value> argv[2] = {info.This(), idx};

    info.GetReturnValue().Set(fn->Call(handler, 2, argv));
    return;
  }

  Local<Value> getPropertyDescriptor = handler->Get(Nan::New<String>("getPropertyDescriptor").ToLocalChecked());
  if (getPropertyDescriptor->IsFunction()) {
    fn = Local<Function>::Cast(getPropertyDescriptor);

    info.GetReturnValue().Set(CallPropertyDescriptorGet(fn->Call(handler, 1, argv1), info.This(), argv1));
    return;
  }

  Local<Value> getOwnPropertyDescriptor = handler->Get(Nan::New<String>("getOwnPropertyDescriptor").ToLocalChecked());
  if (getOwnPropertyDescriptor->IsFunction()) {
    fn = Local<Function>::Cast(getOwnPropertyDescriptor);

    info.GetReturnValue().Set(CallPropertyDescriptorGet(fn->Call(handler, 1, argv1), info.This(), argv1));
    return;
  }
  info.GetReturnValue().SetUndefined(); // <-- silence warnings for 0.10.x
}

/**
 *  Invoked for setting the given indexed property of an object
 *
 *
 *
 */
NAN_INDEX_SETTER(NodeProxy::SetIndexedProperty) {

  if (info.This()->InternalFieldCount() < 1 || info.Data().IsEmpty()) {
    Nan::ThrowTypeError("SetNamedProperty intercepted "
                "by non-Proxy object");
    return;
  }

  Local<Integer> idx = Nan::New<Integer>(index);
  Local<Value> argv2[2] = {idx, value};
  Local<Value> data = info.This()->InternalFieldCount() > 0 ?
                      info.This()->GetInternalField(0) :
                      info.Data();

  if (!data->IsObject()) {
    info.GetReturnValue().SetUndefined();
    return;
  }

  Local<Object> handler = data->ToObject();

  // if the Proxy isn't trapping, return the
  // value set on the index descriptor
  if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
    if (handler->GetHiddenValue(Nan::New<String>("extensible").ToLocalChecked())->BooleanValue() ||
      handler->Has(index)
    ) {
      Local<Value> pd = handler->Get(index);

      if (!pd->IsObject()) {
        info.GetReturnValue().SetUndefined();
        return;
      }

      Local<Object> pd_obj = pd->ToObject();

      if (!pd_obj->GetHiddenValue(
            Nan::New<String>("writable").ToLocalChecked())->BooleanValue()
      ) {
        Nan::ThrowError(
              String::Concat(
                Nan::New<String>("In accessible index: ").ToLocalChecked(),
                    Local<String>::Cast(idx)));
        return;
      }

      Local<Value> set = pd_obj->Get(Nan::New<String>("set").ToLocalChecked());
      if (set->IsFunction()) {
        Local<Function> fn = Local<Function>::Cast(set);
        fn->Call(info.This(), 2, argv2);

        info.GetReturnValue().Set(value);
        return;
      }

      if (pd_obj->Set(Nan::New<String>("value").ToLocalChecked(), value)) {
        info.GetReturnValue().Set(value);
        return;
      }
      info.GetReturnValue().SetUndefined();
      return;
    }
    info.GetReturnValue().SetUndefined();
    return;
  }

  // does the ProxyHandler have a set method?
  Local<Value> set = handler->Get(Nan::New<String>("set").ToLocalChecked());
  if (set->IsFunction()) {
    Local<Function> set_fn = Local<Function>::Cast(set);
    Local<Value> argv3[3] = {info.This(), idx, value};
    set_fn->Call(handler, 3, argv3);

    info.GetReturnValue().Set(value);
    return;
  }

  Local<Value> getOwnPropertyDescriptor = handler->Get(Nan::New<String>("getOwnPropertyDescriptor").ToLocalChecked());
  if (getOwnPropertyDescriptor->IsFunction()) {
    Local<Function> gopd_fn = Local<Function>::Cast(getOwnPropertyDescriptor);
    Local<Value> argv[1] = {idx};
    info.GetReturnValue().Set(CallPropertyDescriptorSet(gopd_fn->Call(handler, 1, argv), info.This(), idx, value));
    return;
  }

  Local<Value> getPropertyDescriptor = handler->Get(Nan::New<String>("getPropertyDescriptor").ToLocalChecked());
  if (getPropertyDescriptor->IsFunction()) {
    Local<Function> gpd_fn = Local<Function>::Cast(getPropertyDescriptor);
    Local<Value> argv[1] = {idx};
    info.GetReturnValue().Set(CallPropertyDescriptorSet(gpd_fn->Call(handler, 1, argv), info.This(), idx, value));
    return;
  }

  info.GetReturnValue().SetUndefined();
}

/**
 *  Invoked for determining if an object has a given indexed property
 *
 *
 *
 */
NAN_INDEX_QUERY(NodeProxy::QueryIndexedPropertyInteger) {

  Local<Integer> idx = Nan::New<Integer>(index);
  Local<Integer> DoesntHavePropertyResponse;
  Local<Integer> HasPropertyResponse = Nan::New<Integer>(None);

  if (info.This()->InternalFieldCount() > 0 || !info.Data().IsEmpty()) {
    Local<Value> data = info.This()->InternalFieldCount() > 0 ?
                 info.This()->GetInternalField(0) :
                 info.Data();

    if (!data->IsObject()) {
      info.GetReturnValue().Set(DoesntHavePropertyResponse);
      return;
    }

    Local<Object> handler = data->ToObject();

    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
      if (handler->Has(index)) {
        Local<Value> pd = handler->Get(index);

        if (pd->IsObject()) {
          info.GetReturnValue().Set(GetPropertyAttributeFromPropertyDescriptor(pd->ToObject()));
          return;
        }
        info.GetReturnValue().Set(HasPropertyResponse);
        return;
      }
      info.GetReturnValue().Set(DoesntHavePropertyResponse);
      return;
    }

    Local<Value> argv[1] = {idx};

    Local<Value> hasOwn = handler->Get(Nan::New<String>("hasOwn").ToLocalChecked());
    if (hasOwn->IsFunction()) {
      Local<Function> hasOwn_fn = Local<Function>::Cast(hasOwn);
      info.GetReturnValue().Set(hasOwn_fn->Call(handler, 1, argv)->BooleanValue() ?
                     HasPropertyResponse :
                     DoesntHavePropertyResponse);
      return;
    }

    Local<Value> has = handler->Get(Nan::New<String>("has").ToLocalChecked());
    if (has->IsFunction()) {
      Local<Function> has_fn = Local<Function>::Cast(has);
      info.GetReturnValue().Set(has_fn->Call(handler, 1, argv)->BooleanValue() ?
                     HasPropertyResponse :
                     DoesntHavePropertyResponse);
      return;
    }

    Local<Value> getOwnPropertyDescriptor = handler->Get(Nan::New<String>("getOwnPropertyDescriptor").ToLocalChecked());
    if (getOwnPropertyDescriptor->IsFunction()) {
      Local<Function> gopd_fn = Local<Function>::Cast(getOwnPropertyDescriptor);
      Local<Value> gopd_pd = gopd_fn->Call(handler, 1, argv);

      if (gopd_pd->IsObject()) {
        info.GetReturnValue().Set(GetPropertyAttributeFromPropertyDescriptor(gopd_pd->ToObject()));
        return;
      }
    }

    Local<Value> getPropertyDescriptor = handler->Get(Nan::New<String>("getPropertyDescriptor").ToLocalChecked());
    if (handler->Has(Nan::New<String>("getPropertyDescriptor").ToLocalChecked())) {
      Local<Function> gpd_fn = Local<Function>::Cast(getPropertyDescriptor);
      Local<Value> gpd_pd = gpd_fn->Call(handler, 1, argv);

      if (gpd_pd->IsObject()) {
        info.GetReturnValue().Set(GetPropertyAttributeFromPropertyDescriptor(gpd_pd->ToObject()));
        return;
      } else if (gpd_pd->IsUndefined()) {
        info.GetReturnValue().Set(DoesntHavePropertyResponse);
        return;
      }
    }
  }

  info.GetReturnValue().Set(DoesntHavePropertyResponse);
}

/**
 *  Invoked for deleting a given indexed property
 *
 *
 *
 */
NAN_INDEX_DELETER(NodeProxy::DeleteIndexedProperty) {

  if (info.This()->InternalFieldCount() > 0 || !info.Data().IsEmpty()) {
    Local<Value> data = info.This()->InternalFieldCount() > 0 ?
                 info.This()->GetInternalField(0) :
                 info.Data();

    if (!data->IsObject()) {
      info.GetReturnValue().Set(Nan::False());
      return;
    }

    Local<Integer> idx = Nan::New<Integer>(index);
    Local<Object> handler = data->ToObject();
    // if the Proxy isn't trapping,
    // return the value set on the property descriptor
    if (!handler->GetHiddenValue(Nan::New<String>("trapping").ToLocalChecked())->BooleanValue()) {
      if (!handler->GetHiddenValue(Nan::New<String>("frozen").ToLocalChecked())->BooleanValue()) {
        Local<Value> pd = handler->Get(idx);

        if (pd->IsObject()) {
          Local<Object> pd_obj = pd->ToObject();

          if (pd_obj->Get(Nan::New<String>("configurable").ToLocalChecked())->IsBoolean() &&
              pd_obj->Get(Nan::New<String>("configurable").ToLocalChecked())->BooleanValue()
          ) {
            info.GetReturnValue().Set(Nan::New<Boolean>(handler->Delete(index)));
            return;
          }
        }
      }
      info.GetReturnValue().Set(Nan::False());
      return;
    }

    Local<Value> delete_ = handler->Get(Nan::New<String>("delete").ToLocalChecked());
    if (delete_->IsFunction()) {
      Local<Function> fn = Local<Function>::Cast(delete_);
      Local<Value> argv[1] = {idx};
      info.GetReturnValue().Set(fn->Call(handler, 1, argv)->ToBoolean());
      return;
    }
  }

  info.GetReturnValue().Set(Nan::New<Boolean>(false));
}

/**
 *  Initialize the NodeProxy Strings and functions
 *
 *
 *
 */
void NodeProxy::Init(Handle<Object> target) {
  Nan::HandleScope scope;

// function creation

// main functions
  Local<Function> create = Nan::New<FunctionTemplate>(Create)->GetFunction();
  Local<String> _create = Nan::New<String>("create").ToLocalChecked();
  create->SetName(_create);
  target->Set(_create, create);

  Local<Function> createFunction = Nan::New<FunctionTemplate>(CreateFunction)->GetFunction();
  Local<String> _createFunction = Nan::New<String>("createFunction").ToLocalChecked();
  create->SetName(_createFunction);
  target->Set(_createFunction, createFunction);

// freeze function assignment
  Local<Function> freeze = Nan::New<FunctionTemplate>(Freeze)->GetFunction();
  Local<String> _freeze = Nan::New<String>("freeze").ToLocalChecked();
  freeze->SetName(_freeze);
  target->Set(_freeze, freeze);

  Local<Function> seal = Nan::New<FunctionTemplate>(Freeze)->GetFunction();
  Local<String> _seal = Nan::New<String>("seal").ToLocalChecked();
  seal->SetName(_seal);
  target->Set(_seal, seal);

  Local<Function> prevent = Nan::New<FunctionTemplate>(Freeze)->GetFunction();
  Local<String> _preventExtensions = Nan::New<String>("preventExtensions").ToLocalChecked();
  prevent->SetName(_preventExtensions);
  target->Set(_preventExtensions, prevent);

// check function assignment
  Local<Function> isfrozen = Nan::New<FunctionTemplate>(IsLocked)->GetFunction();
  Local<String> _isFrozen = Nan::New<String>("isFrozen").ToLocalChecked();
  isfrozen->SetName(_isFrozen);
  target->Set(_isFrozen, isfrozen);

  Local<Function> issealed = Nan::New<FunctionTemplate>(IsLocked)->GetFunction();
  Local<String> _isSealed = Nan::New<String>("isSealed").ToLocalChecked();
  issealed->SetName(_isSealed);
  target->Set(_isSealed, issealed);

  Local<Function> isextensible = Nan::New<FunctionTemplate>(IsLocked)->GetFunction();
  Local<String> _isExtensible = Nan::New<String>("isExtensible").ToLocalChecked();
  isextensible->SetName(_isExtensible);
  target->Set(_isExtensible, isextensible);

// part of harmony proxies
  Local<Function> istrapping = Nan::New<FunctionTemplate>(IsLocked)->GetFunction();
  Local<String> _isTrapping = Nan::New<String>("isTrapping").ToLocalChecked();
  istrapping->SetName(_isTrapping);
  target->Set(_isTrapping, istrapping);

// ECMAScript 5
  Local<Function> getOwnPropertyDescriptor = Nan::New<FunctionTemplate>(GetOwnPropertyDescriptor)->GetFunction();
  Local<String> _getOwnPropertyDescriptor = Nan::New<String>("getOwnPropertyDescriptor").ToLocalChecked();
  getOwnPropertyDescriptor->SetName(_getOwnPropertyDescriptor);
  target->Set(_getOwnPropertyDescriptor, getOwnPropertyDescriptor);

  Local<Function> defineProperty = Nan::New<FunctionTemplate>(DefineProperty)->GetFunction();
  Local<String> _defineProperty = Nan::New<String>("defineProperty").ToLocalChecked();
  defineProperty->SetName(_defineProperty);
  target->Set(_defineProperty, defineProperty);

  Local<Function> defineProperties = Nan::New<FunctionTemplate>(DefineProperties)->GetFunction();
  Local<String> _defineProperties = Nan::New<String>("defineProperties").ToLocalChecked();
  defineProperties->SetName(_defineProperties);
  target->Set(_defineProperties, defineProperties);

// additional functions
  Local<Function> clone = Nan::New<FunctionTemplate>(Clone)->GetFunction();
  Local<String> _clone = Nan::New<String>("clone").ToLocalChecked();
  clone->SetName(_clone);
  target->Set(_clone, clone);

  Local<Function> hidden = Nan::New<FunctionTemplate>(Hidden)->GetFunction();
  Local<String> _hidden = Nan::New<String>("hidden").ToLocalChecked();
  hidden->SetName(_hidden);
  target->Set(_hidden, hidden);

  Local<Function> setPrototype = Nan::New<FunctionTemplate>(SetPrototype)->GetFunction();
  Local<String> _setPrototype = Nan::New<String>("setPrototype").ToLocalChecked();
  setPrototype->SetName(_setPrototype);
  target->Set(_setPrototype, setPrototype);

  Local<Function> isProxy_ = Nan::New<FunctionTemplate>(IsProxy)->GetFunction();
  Local<String> _isProxy = Nan::New<String>("isProxy").ToLocalChecked();
  hidden->SetName(_isProxy);
  target->Set(_isProxy, isProxy_);

  Local<ObjectTemplate> temp = Nan::New<ObjectTemplate>();

  temp->SetInternalFieldCount(1);

  // named property handlers
  Nan::SetNamedPropertyHandler(
    temp,
    GetNamedProperty,
    SetNamedProperty,
    QueryNamedPropertyInteger,
    DeleteNamedProperty,
    EnumerateNamedProperties);

  // indexed property handlers
  // TODO: properly implement arrays
  Nan::SetIndexedPropertyHandler(
    temp,
    GetIndexedProperty,
    SetIndexedProperty,
    QueryIndexedPropertyInteger,
    DeleteIndexedProperty);
  
  ObjectCreator.Reset(temp);

  Local<ObjectTemplate> instance = Nan::New<ObjectTemplate>();
  Nan::SetCallAsFunctionHandler(instance, NodeProxy::New);
  instance->SetInternalFieldCount(1);

  Nan::SetNamedPropertyHandler(
    instance,
    GetNamedProperty,
    SetNamedProperty,
    QueryNamedPropertyInteger,
    DeleteNamedProperty,
    EnumerateNamedProperties);

  Nan::SetIndexedPropertyHandler(
    instance,
    GetIndexedProperty,
    SetIndexedProperty,
    QueryIndexedPropertyInteger,
    DeleteIndexedProperty);

  FunctionCreator.Reset(instance);
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
