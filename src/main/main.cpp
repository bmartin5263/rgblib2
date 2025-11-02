#include "MyApplication.h"
#include "HubApplication.h"
#include "LiteApplication.h"

using namespace rgb;

extern "C" auto app_main() -> void {
  static MyApplication myApplication;
  myApplication.run();

  static home::HubApplication hubApplication;
  hubApplication.run();

  static home::LiteApplication liteApplication;
  liteApplication.run();
}

// Hub Application
//extern "C" auto app_main() -> void {
//  INFO("Starting Vehicle Application");
//  myApplication.run();
//}

// Controller Application
//extern "C" auto app_main() -> void {
//  INFO("Starting Vehicle Application");
//  myApplication.run();
//}
