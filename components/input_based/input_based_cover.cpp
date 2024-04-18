#include "input_based_cover.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace input_based {

static const char *const TAG = "input_based.cover";

void InputBasedCover::setup() {
  this->open_output_->set_level(0.0f);
  this->close_output_->set_level(0.0f);

  this->position = this->state_input_->get_state();
  this->publish_state(false);
}

void InputBasedCover::loop() {
  uint32_t now = millis();
  this->position = this->state_input_->get_state();

  if (now - this->last_update_ > this->update_interval_) {
    this->last_update_ = now;
    this->publish_state(false);
  }

  if (this->moving_) {
    // Moving check
    if (now - this->moving_time_ > this->moving_timeout_) {
      if (abs(this->position - this->moving_start_) < this->moving_difference_) {
        ESP_LOGW(TAG, "The cover has not moved %0.2f in %lums, FORCED STOPPED", this->moving_difference_,
                 this->moving_timeout_);
        this->moving_ = false;
        this->open_output_->set_level(0.0f);
        this->close_output_->set_level(0.0f);
        return;
      }
    }

    // At target check
    if (this->at_position_(this->target_margin_)) {
      this->moving_ = false;
      this->open_output_->set_level(0.0f);
      this->close_output_->set_level(0.0f);
      return;
    }

    // Almost at target check
    if (this->at_position_(this->slow_target_margin_)) {
      if (this->target_position_ > this->position) {
        this->open_output_->set_level(this->slow_target_speed_);
        this->close_output_->set_level(0.0f);
      } else {
        this->close_output_->set_level(this->slow_target_speed_);
        this->open_output_->set_level(0.0f);
      }
      return;
    }

    // Still not at target
    if (this->target_position_ > this->position) {
      this->open_output_->set_level(1.0f);
      this->close_output_->set_level(0.0f);
    } else {
      this->close_output_->set_level(1.0f);
      this->open_output_->set_level(0.0f);
    }
  }
}

void InputBasedCover::dump_config() { LOG_COVER("", "Input Based Cover", this); }

float InputBasedCover::get_setup_priority() const { return setup_priority::DATA; }

cover::CoverTraits InputBasedCover::get_traits() {
  auto traits = cover::CoverTraits();
  traits.set_supports_stop(true);
  traits.set_supports_position(true);
  return traits;
}

void InputBasedCover::control(const cover::CoverCall &call) {
  if (call.get_stop()) {
    this->open_output_->set_level(0.0f);
    this->close_output_->set_level(0.0f);
    this->moving_ = false;
  }

  if (call.get_position().has_value()) {
    this->target_position_ = *call.get_position();
    if (this->target_position_ != this->position) {
      this->moving_ = true;
      this->moving_time_ = millis();
      this->moving_start_ = this->position;
    }
  }
}

bool InputBasedCover::at_position_(float margin) { return abs(this->position - this->target_position_) < margin; }

}  // namespace input_based
}  // namespace esphome
