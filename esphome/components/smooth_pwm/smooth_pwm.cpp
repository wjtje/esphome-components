#include "smooth_pwm.h"

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace smooth_pwm {

static const char *const TAG = "smooth_PWM";

void SmoothPWM::setup() {
  ESP_LOGI(TAG, "Setting up Smooth PWM");
  this->turn_off();
  pinMode(this->pin_, OUTPUT);
}

void SmoothPWM::dump_config() {
  ESP_LOGCONFIG(TAG, "Smooth PWM:");
  ESP_LOGCONFIG(TAG, "  Pin: '%i'", this->pin_);
  ESP_LOGCONFIG(TAG, "  Duration: '%i'", this->duration_);
  ESP_LOGCONFIG(TAG, "  Deadzone: '%i'", this->deadzone_);
}

void SmoothPWM::loop() {
  uint32_t now = millis();

  if (state_changed) {
    float progress = this->smoothed_progress((float) (now - this->start_millis) / (float) (this->duration_));

    if (progress >= 1.0f || this->target_state == this->state) {
      this->state = this->target_state;
      this->state_changed = false;
    } else {
      this->state = lerp(progress, this->prev_state, this->target_state);
    }

    write_analog(this->state);
  }
}

void SmoothPWM::write_state(float state) {
  ESP_LOGI(TAG, "Setting target state to: %f", state);

  this->target_state = state;
  this->prev_state = this->state;
  this->state_changed = true;
  this->start_millis = millis();
}

void SmoothPWM::write_analog(float state) {
  int pwm = (int) (this->deadzone_ + (255 - this->deadzone_) * state);
  // Make sure that 0 is 0
  if (state == 0.0f) {
    pwm = 0;
  }
  ESP_LOGV(TAG, "Setting state to: %i", pwm);
  analogWrite(this->pin_, pwm);
}
}  // namespace smooth_pwm
}  // namespace esphome