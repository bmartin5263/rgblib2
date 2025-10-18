idf_component_register(
        REQUIRES
        spi_flash led_strip esp_timer driver

        SRCS
        "main.cpp"

        "../app/Application.h"
        "../app/ApplicationBuilder.h"

        "../core/App.cpp"
        "../core/App.h"
        "../core/AppBuilder.cpp"
        "../core/AppBuilder.h"
        "../core/Assertions.h"
        "../core/Brightness.cpp"
        "../core/Brightness.h"
        "../core/Clock.cpp"
        "../core/Clock.h"
        "../core/Color.h"
        "../core/Config.h"
        "../core/Debug.cpp"
        "../core/Debug.h"
        "../core/Enumerate.h"
        "../core/Every.h"
        "../core/Func.h"
        "../core/GPIO.h"
        "../core/Handle.h"
        "../core/Iterable.h"
        "../core/Log.cpp"
        "../core/Log.h"
        "../core/NullScene.cpp"
        "../core/NullScene.h"
        "../core/Pin.cpp"
        "../core/Pin.h"
        "../core/Point.h"
        "../core/Scene.h"
        "../core/System.h"
        "../core/Trigger.h"
        "../core/Types.h"
        "../core/Util.h"
        "../core/priv/GPIOBase.h"
        "../core/priv/GPIOESP32.h"
        "../core/priv/GPIOImpl.h"
        "../core/priv/HostSystemBase.h"
        "../core/priv/HostSystemESP32.h"
        "../core/priv/HostSystemImpl.h"
        "../core/priv/LoggingSystem.h"
        "../core/priv/LoggingSystemBase.cpp"
        "../core/priv/LoggingSystemBase.h"
        "../core/threading/CircularCommandQueue.cpp"
        "../core/threading/CircularCommandQueue.h"
        "../core/threading/CircularIndex.cpp"
        "../core/threading/CircularIndex.h"
        "../core/threading/CircularLog.h"

        "../effect/ChasingEffect.cpp"
        "../effect/ChasingEffect.h"
        "../effect/FillEffect.cpp"
        "../effect/FillEffect.h"
        "../effect/Length.h"
        "../effect/TrailingEffect.cpp"
        "../effect/TrailingEffect.h"

        "../event/Event.h"

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

        "../sensor/Button.cpp"
        "../sensor/Button.h"
        "../sensor/ButtonState.h"
        "../sensor/IRButton.cpp"
        "../sensor/IRButton.h"
        "../sensor/IRButtonType.h"
        "../sensor/OBD.cpp"
        "../sensor/OBD.h"
        "../sensor/PushButton.cpp"
        "../sensor/PushButton.h"
        "../sensor/Vehicle.cpp"
        "../sensor/Vehicle.h"

        "../time/Stopwatch.cpp"
        "../time/Stopwatch.h"
        "../time/Timer.cpp"
        "../time/Timer.h"
        "../time/TimerHandle.cpp"
        "../time/TimerHandle.h"
        "../time/TimerNode.cpp"
        "../time/TimerNode.h"

        INCLUDE_DIRS
        "../app"
        "../core"
        "../core/debug"
        "../core/threading"
        "../effect"
        "../event"
        "../gfx"
        "../led"
        "../pixel"
        "../sensor"
        "../time"

        PRIV_INCLUDE_DIRS
        "../core/priv"
)





target_compile_definitions(${COMPONENT_LIB} PUBLIC
        RGB_DEBUG
)
target_compile_options(${COMPONENT_LIB} PRIVATE
        -Werror=switch
)
