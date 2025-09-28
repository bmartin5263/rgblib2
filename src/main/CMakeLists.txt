idf_component_register(
        REQUIRES
        spi_flash led_strip esp_timer driver LovyanGFX

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
        "../led/LEDCore.h"
        "../led/LEDMatrix.h"
        "../led/LEDMatrix2.h"
        "../led/LEDStrip.h"
        "../led/MatrixStrategy.h"

        "../pixel/Pixel.h"
        "../pixel/PixelGrid.h"
        "../pixel/PixelList.cpp"
        "../pixel/PixelList.h"
        "../pixel/PixelSlice.cpp"
        "../pixel/PixelSlice.h"

        "../system/System.h"

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
        "../pixel"
        "../system"
        "../time"
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC
        RGB_DEBUG
)
