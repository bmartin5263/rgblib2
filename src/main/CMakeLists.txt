idf_component_register(
        SRCS
        "main.cpp"

        "../core/Assertions.h"
        "../core/Brightness.cpp"
        "../core/Brightness.h"
        "../core/Color.h"
        "../core/Clock.cpp"
        "../core/Clock.h"
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
        "../led/PixelSlice.h"
        "../led/PixelSlice.cpp"

        REQUIRES
        spi_flash led_strip

        INCLUDE_DIRS
        "../led"
        "../core"

)
