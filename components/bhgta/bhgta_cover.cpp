#include "bhgta_cover.h"
#include "esphome/core/log.h"

namespace esphome {
namespace bhgta {
static const char *const TAG = "bhgta.cover";

cover::CoverTraits BhgtaCover::get_traits() {
  auto traits = cover::CoverTraits();
  traits.set_is_assumed_state(false);
  traits.set_supports_position(true);
  traits.set_supports_tilt(false);
  traits.set_supports_toggle(false);
  return traits;
}

void BhgtaCover::control(const cover::CoverCall &call) {
  if (call.get_stop()) {
    this->current_operation = cover::CoverOperation::COVER_OPERATION_IDLE;
    this->publish_state(true);
    this->pwm_in_->write_state(0.0f);
    this->pwm_out_->write_state(0.0f);
  }

  if (call.get_position().has_value()) {
    auto pos = *call.get_position();
    if (pos == this->position) {
      // already at target
    } else {
      auto op = pos < this->position ? cover::CoverOperation::COVER_OPERATION_CLOSING
                                     : cover::CoverOperation::COVER_OPERATION_OPENING;
      this->target_position_ = pos;
      this->current_operation = op;
      this->last_position_time_ = millis();
      this->publish_state(true);
    }
  }
}

bool BhgtaCover::is_at_target_() {
  if (this->current_operation == cover::COVER_OPERATION_OPENING)
    return this->position > this->target_position_ - 0.1f;

  if (this->current_operation == cover::COVER_OPERATION_CLOSING)
    return this->position < this->target_position_ + 0.1f;

  if (this->current_operation == cover::CoverOperation::COVER_OPERATION_IDLE)
    return true;

  return false;
}

void BhgtaCover::setup() {
  this->position = this->calculate_position_();
  this->last_position_ = position;
  this->publish_state(false);
}

void BhgtaCover::loop() {
  const uint32_t now = millis();
  this->position = this->calculate_position_();

  // Send current position every second
  if (now - this->last_publish_time_ > 1000) {
    this->publish_state(false);
    this->last_publish_time_ = now;
  }

  if (this->current_operation == cover::CoverOperation::COVER_OPERATION_IDLE) {
    this->pwm_in_->write_state(0.0f);
    this->pwm_out_->write_state(0.0f);
    this->last_position_time_ = 0;
  } else {
    // Check for changes
    if (this->last_position_ != this->position) {
      this->last_position_ = this->position;
      this->last_position_time_ = now;
    }

    if (now - this->last_position_time_ > 4000 && this->last_position_time_ != 0) {
      ESP_LOGW(TAG, "Thing has not moved for 4 seconds");
      this->pwm_in_->force_write(0.0f);
      this->pwm_out_->force_write(0.0f);
      this->current_operation = cover::CoverOperation::COVER_OPERATION_IDLE;
      this->publish_state(true);
      this->force_stop_callback_.call();
    }
  }

  if (this->current_operation == cover::CoverOperation::COVER_OPERATION_OPENING) {
    // Check if the closing signal is still active
    if (this->pwm_in_->get_state() != 0.0f) {
      this->pwm_in_->write_state(0.0f);
      return;
    }

    if (this->is_open_() || (this->target_position_ != 1.0f && this->is_at_target_())) {
      this->current_operation = cover::CoverOperation::COVER_OPERATION_IDLE;
      this->publish_state(true);
      this->pwm_out_->write_state(0.0f);
    } else if (this->is_almost_open_()) {
      this->pwm_out_->write_state(this->almost_power_);
    } else {
      this->pwm_out_->write_state(1.0f);
    }
  }

  if (this->current_operation == cover::CoverOperation::COVER_OPERATION_CLOSING) {
    // Check if the opening signal is still active
    if (this->pwm_out_->get_state() != 0.0f) {
      this->pwm_out_->write_state(0.0f);
      return;
    }

    if (this->is_closed_() || (this->target_position_ != 0.0f && this->is_at_target_())) {
      this->current_operation = cover::CoverOperation::COVER_OPERATION_IDLE;
      this->publish_state(true);
      this->pwm_in_->write_state(0.0f);
    } else if (this->is_almost_closed_()) {
      this->pwm_in_->write_state(this->almost_power_);
    } else {
      this->pwm_in_->write_state(1.0f);
    }
  }
}

void BhgtaCover::dump_config() {
  LOG_COVER("", "BHGTA Cover", this);
  ESP_LOGCONFIG(TAG, "  PWM in: '%i'", this->pwm_in_->get_pin());
  ESP_LOGCONFIG(TAG, "  PWM out: '%i'", this->pwm_out_->get_pin());
}

float BhgtaCover::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace bhgta
}  // namespace esphome
