#pragma once

#ifdef USE_ESP8266

#include "esphome/core/component.h"
#include "esphome/core/optional.h"
#include "esphome/components/output/float_output.h"
#include <Arduino.h>

namespace esphome {
namespace smooth_pwm {
class SmoothPWM : public output::FloatOutput, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_pin(int pin) { this->pin_ = pin; }
  void set_duration(int duration) { this->duration_ = duration; }
  void set_deadzone(int deadzone) { this->deadzone_ = deadzone; }
  void set_pair_output(SmoothPWM *output) { this->pair_output_ = output; }

  int get_pin() { return this->pin_; }
  float get_state() { return this->state_; }

  void write_state(float state) override;
  void force_write(float state);

 protected:
  void write_analog(float state);

  uint32_t start_millis_ = 0;

  float prev_state_ = 0.0f;
  float target_state_ = 0.0f;
  float state_ = 0.0f;

  bool state_changed_ = false;

  int pin_;
  int duration_ = 1000;
  int deadzone_ = 0;
  optional<SmoothPWM *> pair_output_;
};
}  // namespace smooth_pwm
}  // namespace esphome

#endif
