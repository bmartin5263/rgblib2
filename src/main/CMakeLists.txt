idf_component_register(
        REQUIRES
        spi_flash led_strip esp_timer driver

        SRCS
        "main.cpp"

        "../core/Assertions.h"
        "../core/Brightness.cpp"
        "../core/Brightness.h"
        "../core/Clock.cpp"
        "../core/Clock.h"
        "../core/ClockTime.h"
        "../core/Color.h"
        "../core/Config.h"
        "../core/Debug.cpp"
        "../core/Debug.h"
        "../core/Enumerate.h"
        "../core/Every.h"
        "../core/Func.h"
        "../core/Handle.h"
        "../core/Iterable.h"
        "../core/Log.cpp"
        "../core/Log.h"
        "../core/Point.h"
        "../core/Types.h"
        "../core/Util.h"

        "../led/LEDCircuit.h"
        "../led/LEDMatrix.h"
        "../led/LEDMatrix2.h"
        "../led/LEDStrip.h"
        "../led/MatrixStrategy.h"
        "../led/Pixel.h"
        "../led/PixelGrid.h"
        "../led/PixelList.cpp"
        "../led/PixelList.h"
        "../led/PixelSlice.cpp"
        "../led/PixelSlice.h"

        "../time/Stopwatch.cpp"
        "../time/Stopwatch.h"
        "../time/Timer.cpp"
        "../time/Timer.h"
        "../time/TimerHandle.cpp"
        "../time/TimerHandle.h"
        "../time/TimerNode.cpp"
        "../time/TimerNode.h"

        INCLUDE_DIRS
        "../core"
        "../effect"
        "../event"
        "../led"
        "../time"
)
