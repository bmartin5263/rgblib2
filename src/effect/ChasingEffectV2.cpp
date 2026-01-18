//
// Created by Brandon on 8/17/25.
//

#include "ChasingEffectV2.h"
#include "Clock.h"

namespace rgb {

auto ChasingEffectV2::reset() -> void {
}

auto ChasingEffectV2::update() -> void {

}

auto ChasingEffectV2::step() -> void {
}

auto ChasingEffectV2::draw(PixelList& pixels) -> void {
  auto pixelLength = pixels.length();

  uint actualTrailLength;
  if (std::holds_alternative<normal>(trailLength.variant)) {
    actualTrailLength = static_cast<uint>(static_cast<normal>(pixels.length()) * get<normal>(trailLength.variant));
  }
  else {
    actualTrailLength = get<uint>(trailLength.variant);
  }

  auto params = ShaderParameters {
    .now = Clock::Now(),
    .duration = progression.duration,
    .trailLength = actualTrailLength,
    .pixelPosition = 0,
    .trailPosition = 0,
    .positionRatio = 0.0f,
  };

  Duration duration;
  if (progression.isDelay) {
    duration = Duration{progression.duration.value * pixelLength};
  }
  else {
    duration = progression.duration;
  }

  auto headPercent = Clock::Now().percentOf(Timestamp{duration.value});
  auto effectPosition = static_cast<uint>(static_cast<float>(pixelLength) * headPercent); // round down

  if (buildup) {
    for (auto trailPosition = 0; trailPosition < actualTrailLength; ++trailPosition) {
      if (trailPosition > effectPosition) {
        // Anything before effectPosition is not drawn
        break;
      }
      auto pixelPosition = (effectPosition - trailPosition + shift) % pixelLength; // draw head first
      params.trailPosition = trailPosition;
      params.positionRatio = static_cast<float>(actualTrailLength - trailPosition) / static_cast<float>(actualTrailLength);
      params.pixelPosition = pixelPosition;
      pixels.set(pixelPosition, shader(pixels.get(pixelPosition), params));
    }
  }
  else {
    for (auto trailPosition = 0; trailPosition < actualTrailLength; ++trailPosition) {
      auto pixelPosition = (effectPosition + trailPosition + shift) % pixelLength; // draw tail first
      params.trailPosition = actualTrailLength - 1 - trailPosition;
      params.positionRatio = static_cast<float>(trailPosition + 1) / static_cast<float>(actualTrailLength);
      params.pixelPosition = pixelPosition;
      pixels.set(pixelPosition, shader(pixels.get(pixelPosition), params));
    }
  }
}

auto ChasingEffectV2::draw(Iterable<PixelList*> pixelLists) -> void {
  for (auto list : pixelLists) {
    draw(*list);
  }
}

}