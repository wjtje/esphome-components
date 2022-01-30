#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace bare {

class BareSensor : public sensor::Sensor, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_value(int value);
};

template<typename... Ts> class BareSensorSetValueAction : public Action<Ts...> {
 public:
  BareSensorSetValueAction(BareSensor *baresensor) : baresensor_(baresensor) {}
  TEMPLATABLE_VALUE(int, value)

  void play(Ts... x) override { this->baresensor_->set_value(this->value_.value(x...)); }

 protected:
  BareSensor *baresensor_;
};
}  // namespace bare
}  // namespace esphome
