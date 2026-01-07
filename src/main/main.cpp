#include "LincolnApplication.h"

using namespace rgb;

extern "C" auto app_main() -> void {
  static LincolnApplication myApplication;
  myApplication.run();
}
