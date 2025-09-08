idf_component_register(
        SRCS
        "main.cpp"

        "../core/Assertions.h"
        "../core/Brightness.cpp"
        "../core/Brightness.h"
        "../core/Clock.cpp"
        "../core/Clock.h"
        "../core/ClockTime.h"
        "../core/Color.h"
        "../core/Debug.cpp"
        "../core/Debug.h"
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

        REQUIRES
        spi_flash led_strip

        INCLUDE_DIRS
        "../core"
        "../effect"
        "../event"
        "../led"
)
