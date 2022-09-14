#include "motion_cover.h"
#include "esphome/core/log.h"
#include <Arduino.h>

namespace esphome {
namespace motion {
static const char *const TAG = "motion.cover";

void MotionCover::setup() {
  this->position = this->calculate_position_();
  this->last_position_ = this->position;
  this->publish_state(false);
  this->stop_trigger_->trigger();
}

void MotionCover::loop() {
  const uint32_t now = millis();
  this->position = this->calculate_position_();

  // Update the position every update interval
  if (now - this->last_publish_time_ > this->update_interval_) {
    ESP_LOGD(TAG, "New calculated position: %.2f", this->position);
    this->publish_state(false);
    this->last_publish_time_ = now;
  }

  switch (this->current_operation) {
    case cover::CoverOperation::COVER_OPERATION_IDLE:
      if (this->current_action_ != MotionCoverAction::STOPPED) {
        ESP_LOGD(TAG, "Stopped cover");
        this->stop_trigger_->trigger();
        this->last_position_time_ = 0;
        this->current_action_ = MotionCoverAction::STOPPED;
      }
      break;

    case cover::CoverOperation::COVER_OPERATION_OPENING:
      // Check if the cover is moving
      if (!this->movement_check_())
        break;

      // Check if the cover can open
      if (!this->can_open_()) {
        this->stop_trigger_->trigger();
        break;
      }

      // Check if the cover is fully open or at the target position
      if (this->is_open_() || (this->target_position_ != 1.0f && this->is_at_target_position_())) {
        ESP_LOGI(TAG, "Reached target position");
        this->current_operation = cover::CoverOperation::COVER_OPERATION_IDLE;
        this->publish_state(true);
        break;
      }

      // Check if the cover is almost fully open
      if (this->is_almost_open_() && this->current_action_ != MotionCoverAction::SLOW_OPENING) {
        ESP_LOGD(TAG, "Almost open, slowing down speed");
        this->almost_open_trigger_->trigger();
        this->current_action_ = MotionCoverAction::SLOW_OPENING;
        break;
      }

      // Just normaly opening
      if (this->current_action_ != MotionCoverAction::OPENING) {
        ESP_LOGD(TAG, "Opening");
        this->open_trigger_->trigger();
        this->current_action_ = MotionCoverAction::OPENING;
      }
      break;

    case cover::CoverOperation::COVER_OPERATION_CLOSING:
      // Check if the cover is moving
      if (!this->movement_check_())
        break;

      // Check if the cover can close
      if (!this->can_close_()) {
        this->stop_trigger_->trigger();
        break;
      }

      // Check if the cover is fully closed or at the target position
      if (this->is_closed_() || (this->target_position_ != 0.0f && this->is_at_target_position_())) {
        if (this->close_time_ = 0) {
          ESP_LOGI(TAG, "Reached target position");
          this->close_time_ = now;
        }
        if (now - this->close_time_ > this->extra_close_duration_) {
          ESP_LOGI(TAG, "Finished extra close time");
          this->close_time_ = 0;
          this->current_operation = cover::CoverOperation::COVER_OPERATION_IDLE;
          this->publish_state(true);
        }
        break;
      }

      // Check if the cover is almost fully closed
      if (this->is_almost_closed_() && this->current_action_ != MotionCoverAction::SLOW_CLOSING) {
        ESP_LOGD(TAG, "Almost closed, slowing down speed");
        this->almost_closed_trigger_->trigger();
        this->current_action_ = MotionCoverAction::SLOW_CLOSING;
        break;
      }

      // Just normally closing
      if (this->current_action_ != MotionCoverAction::CLOSING) {
        ESP_LOGD(TAG, "Closing");
        this->close_trigger_->trigger();
        this->current_action_ = MotionCoverAction::CLOSING;
      }
      break;

    default:
      ESP_LOGW(TAG, "Unknown cover operation");
      break;
  }
}

void MotionCover::dump_config() {
  LOG_COVER("", "Motion Cover", this);
  ESP_LOGCONFIG(TAG, "Almost percentage: %.1f", this->almost_closed_);
  ESP_LOGCONFIG(TAG, "Update interval: %i", this->update_interval_);
  ESP_LOGCONFIG(TAG, "Target margin: %.1f", this->target_margin_);
}

float MotionCover::get_setup_priority() const { return setup_priority::DATA; }

cover::CoverTraits MotionCover::get_traits() {
  auto traits = cover::CoverTraits();
  traits.set_is_assumed_state(false);
  traits.set_supports_position(true);
  traits.set_supports_tilt(false);
  traits.set_supports_toggle(false);
  return traits;
}

void MotionCover::control(const cover::CoverCall &call) {
  if (call.get_stop()) {
    this->current_operation = cover::CoverOperation::COVER_OPERATION_IDLE;
    this->publish_state(true);
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

float MotionCover::calculate_position_() {
  auto val = this->position_();

  if (val < 0.0f) {
    return 0.0f;
  } else if (val > 1.0f) {
    return 1.0f;
  }

  return roundf(val * 100) / 100;
}

bool MotionCover::movement_check_() {
  uint32_t now = millis();

  // Check for changes
  if (this->last_position_ != this->position) {
    this->last_position_ = this->position;
    this->last_position_time_ = now;
  }

  if (now - this->last_position_time_ > 4000 && this->last_position_time_ != 0) {
    ESP_LOGW(TAG, "Thing has not moved for 4 seconds");
    this->current_operation = cover::CoverOperation::COVER_OPERATION_IDLE;
    this->current_action_ = MotionCoverAction::STOPPED;
    this->publish_state(true);
    this->force_stop_trigger_->trigger();
    return false;
  }

  return true;
}

bool MotionCover::is_at_target_position_() {
  if (this->current_operation == cover::COVER_OPERATION_OPENING)
    return this->position > this->target_position_ - this->target_margin_;

  if (this->current_operation == cover::COVER_OPERATION_CLOSING)
    return this->position < this->target_position_ + this->target_margin_;

  if (this->current_operation == cover::CoverOperation::COVER_OPERATION_IDLE)
    return true;

  return false;
}

}  // namespace motion
}  // namespace esphome
