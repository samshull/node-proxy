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

#ifndef NODE_PROXY_H // NOLINT
#define NODE_PROXY_H


#include <v8.h>
#include <node.h>
#include <node_version.h>
#include "nan.h"

using namespace v8;
using namespace node;

// had to redefine NODE_VERSION_AT_LEAST here because of missing parenthesis
#define PROXY_NODE_VERSION_AT_LEAST(major, minor, patch) \
  (((major) < NODE_MAJOR_VERSION) \
    || ((major) == NODE_MAJOR_VERSION && (minor) < NODE_MINOR_VERSION) \
    || ((major) == NODE_MAJOR_VERSION && (minor) == NODE_MINOR_VERSION && \
    (patch) <= NODE_PATCH_VERSION))

class NodeProxy {
  public:
  static Persistent<ObjectTemplate> ObjectCreator;
  static Persistent<ObjectTemplate> FunctionCreator;
  static void Init(Handle<Object> target);

  protected:
  NodeProxy();
  ~NodeProxy();
  static Handle<Integer>
    GetPropertyAttributeFromPropertyDescriptor(Local<Object> pd);
  static Local<Value> CorrectPropertyDescriptor(Local<Object> pd);
  static NAN_METHOD(ValidateProxyHandler);
  static NAN_METHOD(Clone);
  static NAN_METHOD(Hidden);
  static NAN_METHOD(Create);
  static NAN_METHOD(SetPrototype);
  static NAN_METHOD(CreateFunction);
  static NAN_METHOD(Freeze);
  static NAN_METHOD(IsLocked);
  static NAN_METHOD(IsProxy);
  static NAN_METHOD(GetOwnPropertyDescriptor);
  static NAN_METHOD(DefineProperty);
  static NAN_METHOD(DefineProperties);
  static NAN_METHOD(New);
  static NAN_PROPERTY_GETTER(GetNamedProperty);
  static NAN_PROPERTY_SETTER(SetNamedProperty);
  static NAN_PROPERTY_QUERY(QueryNamedPropertyInteger);
  static NAN_PROPERTY_DELETER(DeleteNamedProperty);
  static NAN_PROPERTY_ENUMERATOR(EnumerateNamedProperties);
  static NAN_INDEX_GETTER(GetIndexedProperty);
  static NAN_INDEX_SETTER(SetIndexedProperty);
  static NAN_INDEX_QUERY(QueryIndexedPropertyInteger);
  static NAN_INDEX_DELETER(DeleteIndexedProperty);

  static NAN_INLINE(Local<Value> CallPropertyDescriptorGet(Local<Value> descriptor,
              Handle<Object> context,
              Local<Value> args[1]));
  static NAN_INLINE(Local<Value> CallPropertyDescriptorSet(Local<Value> descriptor,
              Handle<Object> context,
              Local<Value> name,
              Local<Value> value));
};


extern "C" void init(v8::Handle<v8::Object> target);

#endif // NODE_CLASSTEMPLATE_H // NOLINT
