//
// Created by Brandon on 10/25/25.
//

#ifndef RGBLIB_SENSOR_H
#define RGBLIB_SENSOR_H


namespace rgb {

class Sensor {
public:
  Sensor() = default;
  virtual ~Sensor() = default;
  Sensor(const Sensor& rhs) = default;
  Sensor(Sensor&& rhs) noexcept = default;
  Sensor& operator=(const Sensor& rhs) = default;
  Sensor& operator=(Sensor&& rhs) noexcept = default;

  auto doStart() -> bool;
  auto doRead() -> void;
protected:
  virtual auto start() -> bool = 0;
  virtual auto read() -> void = 0;

private:

};

}


#endif //RGBLIB_SENSOR_H
