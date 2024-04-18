#include "smooth_pwm.h"

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace smooth_pwm {

static const char *const TAG = "smooth_PWM";

void SmoothPWM::setup() {
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
  if (!state_changed_)
    return;

  uint32_t now = millis();

  // Calculate the duration of the 'animation', this keeps the time it take to speed up, and slow down always the same
  float duration = (float) this->duration_ * fabs(this->prev_state_ - this->target_state_);
  // Calculate the progress with a linear curve
  float progress = (float) (now - this->start_millis_) / duration;

  if (progress >= 1.0f || this->target_state_ == this->state_) {
    this->state_ = this->target_state_;
    this->state_changed_ = false;
  } else {
    this->state_ = lerp(progress, this->prev_state_, this->target_state_);
  }

  write_analog(this->state_);
}

void SmoothPWM::write_state(float state) {
  if (this->target_state_ == state)
    return;

  ESP_LOGD(TAG, "Setting target state to: %f", state);

  this->target_state_ = state;
  this->prev_state_ = this->state_;
  this->state_changed_ = true;
  this->start_millis_ = millis();
}

void SmoothPWM::force_write(float state) {
  // Directly write the state to the pwm output
  this->write_analog(state);
  this->state_changed_ = false;
  this->state_ = state;
}

void SmoothPWM::write_analog(float state) {
  int pwm = (int) (this->deadzone_ + (255 - this->deadzone_) * state);
  // Make sure that 0 is 0
  if (state == 0.0f) {
    pwm = 0;
  }

  if (pwm != 0 && this->pair_output_.has_value()) {
    if (this->pair_output_.value()->get_state() != 0.0f) {
      ESP_LOGI(TAG, "Pair output not zero, force write to 0");
      this->pair_output_.value()->force_write(0.0f);
    }
  }

  ESP_LOGV(TAG, "Setting state to: %i", pwm);
  analogWrite(this->pin_, pwm);
}

}  // namespace smooth_pwm
}  // namespace esphome
