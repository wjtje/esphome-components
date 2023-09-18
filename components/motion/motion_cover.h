#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/cover/cover.h"

namespace esphome {
namespace motion {

enum MotionCoverAction { OPENING, SLOW_OPENING, STOPPED, SLOW_CLOSING, CLOSING };

class MotionCover : public cover::Cover, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  cover::CoverTraits get_traits() override;

  Trigger<> *get_open_trigger() const { return this->open_trigger_; }
  Trigger<> *get_almost_open_trigger() const { return this->almost_open_trigger_; }
  Trigger<> *get_close_trigger() const { return this->close_trigger_; }
  Trigger<> *get_almost_closed_trigger() const { return this->almost_closed_trigger_; }
  Trigger<> *get_stop_trigger() const { return this->stop_trigger_; }
  Trigger<> *get_force_stop_trigger() const { return this->force_stop_trigger_; }

  void set_almost_at_target(float almost_at_target) { this->almost_at_target_ = almost_at_target; };
  void set_target_margin(float target_margin) { this->target_margin_ = target_margin; };

  void set_position(const std::function<float()> &&lambda) { this->position_ = lambda; };
  void set_can_open(const std::function<bool()> &&lambda) { this->can_open_ = lambda; }
  void set_can_close(const std::function<bool()> &&lambda) { this->can_close_ = lambda; }
  void set_extra_close_duration(int extra_close_duration) { this->extra_close_duration_ = extra_close_duration; }

 protected:
  // Default esphome function
  void control(const cover::CoverCall &call) override;

  // Helper function
  bool at_position_(float offset) {
    if (this->current_operation == cover::COVER_OPERATION_OPENING) {
      if (this->target_position_ == 1.0f)
        return this->position >= 1.0f;
      return this->position > this->target_position_ - offset;
    }

    if (this->target_position_ == 0.0f)
      return this->position <= 0.0f;
    return this->position < this->target_position_ + offset;
  }

  float calculate_position_();

  bool movement_check_();

  // Callback managers
  Trigger<> *open_trigger_{new Trigger<>()};
  Trigger<> *almost_open_trigger_{new Trigger<>()};
  Trigger<> *close_trigger_{new Trigger<>()};
  Trigger<> *almost_closed_trigger_{new Trigger<>()};
  Trigger<> *stop_trigger_{new Trigger<>()};
  Trigger<> *force_stop_trigger_{new Trigger<>()};

  std::function<float()> position_{};
  std::function<bool()> can_open_{};
  std::function<bool()> can_close_{};

  float almost_at_target_ = 0.1f;
  float target_margin_ = 0.05f;

  int update_interval_ = 500;  // 1s
  int extra_close_duration_;

  float last_position_;
  uint32_t last_publish_time_;
  uint32_t last_position_time_;
  uint32_t close_time_ = 0;
  float target_position_;
  MotionCoverAction current_action_{MotionCoverAction::STOPPED};
};

}  // namespace motion
}  // namespace esphome
