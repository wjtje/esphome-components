#pragma once

#ifdef USE_ESP8266

#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"
#include <Arduino.h>

namespace esphome {
namespace smooth_pwm {
class SmoothPWM : public output::FloatOutput, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_pin(int pin) { pin_ = pin; }
  void set_duration(int duration) { duration_ = duration; }
  void set_deadzone(int deadzone) { deadzone_ = deadzone; }

  void write_state(float state) override;
  void write_analog(float state);

 protected:
  // This looks crazy, but it reduces to 6x^5 - 15x^4 + 10x^3 which is just a smooth sigmoid-like
  // transition from 0 to 1 on x = [0, 1]
  static float smoothed_progress(float x) { return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f); }

  uint32_t start_millis = 0;

  float prev_state = 0.0f;
  float target_state = 0.0f;
  float state = 0.0f;

  bool state_changed = false;

  int pin_;
  int duration_ = 1000;
  int deadzone_ = 0;
};
}  // namespace smooth_pwm
}  // namespace esphome

#endif