// Copyright 2017 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#ifndef NATIVEUI_BROWSER_H_
#define NATIVEUI_BROWSER_H_

#include <map>
#include <string>

#include "base/values.h"
#include "nativeui/util/function_caller.h"
#include "nativeui/view.h"

namespace nu {

class NATIVEUI_EXPORT Browser : public View {
 public:
  using ExecutionCallback = std::function<void(bool, base::Value)>;
  using BindingFunc = std::function<void(base::Value)>;

  Browser();

  // View class name.
  static const char kClassName[];

  // View:
  const char* GetClassName() const override;

  void LoadURL(const std::string& url);
  void LoadHTML(const std::string& html, const std::string& base_url);
  void ExecuteJavaScript(const std::string& code,
                         const ExecutionCallback& callback);

  void SetBindingName(const std::string& name);
  void AddRawBinding(const std::string& name, const BindingFunc& func);
  void RemoveBinding(const std::string& name);

  // Automatically deduce argument types.
  template<typename Sig>
  void AddBinding(const std::string& name, const std::function<Sig>& func) {
    AddRawBinding(name, [func](base::Value args) {
      internal::Dispatcher<Sig>::DispatchToCallback(func, args);
    });
  }

  // Events.
  Signal<void(Browser*)> on_close;
  Signal<void(Browser*)> on_finish_navigation;

  // Private: Called from web pages to invoke native bindings.
  bool InvokeBindings(const std::string& key,
                      const std::string& name,
                      base::Value args);

  // Private: Generate the user script to inject bindings.
  std::string GetBindingScript();

  // Private: Access to bindings properties.
  bool stop_serving() const { return stop_serving_; }

 protected:
  ~Browser() override;

 private:
  void PlatformInit();
  void PlatformDestroy();
  void PlatformUpdateBindings();

  // Prevent malicous calls to native bindings.
  std::string security_key_;
  bool stop_serving_ = false;

  std::string binding_name_;
  std::map<std::string, BindingFunc> bindings_;
};

}  // namespace nu

#endif  // NATIVEUI_BROWSER_H_
