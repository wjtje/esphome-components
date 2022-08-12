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
  if (!state_changed_)
    return;

  uint32_t now = millis();

  // Caculate the duration of the 'animation', this keeps the time it take to speed up, and slow down always the same
  float duration = (float) this->duration_ * fabs(this->prev_state_ - this->target_state_);
  // Caclate the progress with a linear curve
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

  ESP_LOGI(TAG, "Setting target state to: %f", state);

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
  ESP_LOGV(TAG, "Setting state to: %i", pwm);
  analogWrite(this->pin_, pwm);
}
}  // namespace smooth_pwm
}  // namespace esphome
