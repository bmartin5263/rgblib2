//
// Created by Brandon on 9/14/25.
//

#ifndef RGBLIB_LEDCORE_H
#define RGBLIB_LEDCORE_H

#include <led_strip_types.h>
#include <functional>

namespace rgb {

using NativePixelFormat = led_pixel_format_t;
using NativeLEDHandle = led_strip_handle_t;

struct RgbFormat {
  using WriteFunction = void(*)(NativeLEDHandle, uint32_t, const Pixel&);
  const NativePixelFormat nativeFormat;
  const WriteFunction writer;
};

namespace format {
constexpr static RgbFormat GRB = RgbFormat{
  LED_PIXEL_FORMAT_GRB,
  [](NativeLEDHandle strip, uint32_t index, const Pixel& pixel){
    led_strip_set_pixel(strip, index, FloatToByte(pixel.r), FloatToByte(pixel.g), FloatToByte(pixel.b));
  }
};
constexpr static RgbFormat GRBW = RgbFormat{
  LED_PIXEL_FORMAT_GRBW,
  [](NativeLEDHandle strip, uint32_t index, const Pixel& pixel){
    led_strip_set_pixel_rgbw(strip, index, FloatToByte(pixel.r), FloatToByte(pixel.g), FloatToByte(pixel.b), FloatToByte(pixel.w));
  }
};
}

}

#endif //RGBLIB_LEDCORE_H
