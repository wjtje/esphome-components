#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/cover/cover.h"

namespace esphome {
namespace motion {

enum MotionCoverAction { OPENING, SLOW_OPENING, STOPPED, SLOW_CLOSING, CLOSING };

class MotionCover : public cover::Cover, public Component {
 public:
  // Default esphome function
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;
  cover::CoverTraits get_traits() override;

  // Setters
  void set_open_action(std::function<void()> &&callback) { this->open_action_.add(std::move(callback)); };
  void set_almost_open_action(std::function<void()> &&callback) { this->almost_open_action_.add(std::move(callback)); };
  void set_close_action(std::function<void()> &&callback) { this->close_action_.add(std::move(callback)); };
  void set_almost_closed_action(std::function<void()> &&callback) {
    this->almost_closed_action_.add(std::move(callback));
  };
  void set_stop_action(std::function<void()> &&callback) { this->stop_action_.add(std::move(callback)); };
  void set_force_stop_action(std::function<void()> &&callback) { this->force_stop_action_.add(std::move(callback)); };
  void set_position(const std::function<float()> &&lambda) { this->position_ = lambda; };
  void set_almost_closed(float almost_closed) { this->almost_closed_ = almost_closed; };
  void set_can_open(const std::function<bool()> &&lambda) { this->can_open_ = lambda; }
  void set_can_close(const std::function<bool()> &&lambda) { this->can_close_ = lambda; }

 protected:
  // Default esphome function
  void control(const cover::CoverCall &call) override;

  // Helper function
  bool is_open_() { return this->position >= 1.0f; }
  bool is_almost_open_() { return this->position >= 1.0f - this->almost_closed_; }
  bool is_closed_() { return this->position <= 0.0f; }
  bool is_almost_closed_() { return this->position <= this->almost_closed_; }

  float calculate_position_();

  bool movement_check_();
  bool is_at_target_position_();

  // Callback managers
  CallbackManager<void()> open_action_{};
  CallbackManager<void()> almost_open_action_{};
  CallbackManager<void()> close_action_{};
  CallbackManager<void()> almost_closed_action_{};
  CallbackManager<void()> stop_action_{};
  CallbackManager<void()> force_stop_action_{};

  std::function<float()> position_{};
  std::function<bool()> can_open_{};
  std::function<bool()> can_close_{};
  float almost_closed_;
  int update_interval_ = 1000;  // 1s
  float target_margin_ = 0.1f;

  float last_position_;
  uint32_t last_publish_time_;
  uint32_t last_position_time_;
  float target_position_;
  MotionCoverAction current_action_{MotionCoverAction::STOPPED};
};

class CoverOpenAction : public Trigger<> {
 public:
  CoverOpenAction(MotionCover *cover) {
    cover->set_open_action([this]() { this->trigger(); });
  }
};

class CoverAlmostOpenAction : public Trigger<> {
 public:
  CoverAlmostOpenAction(MotionCover *cover) {
    cover->set_almost_open_action([this]() { this->trigger(); });
  }
};

class CoverCloseAction : public Trigger<> {
 public:
  CoverCloseAction(MotionCover *cover) {
    cover->set_close_action([this]() { this->trigger(); });
  }
};

class CoverAlmostCloseAction : public Trigger<> {
 public:
  CoverAlmostCloseAction(MotionCover *cover) {
    cover->set_almost_closed_action([this]() { this->trigger(); });
  }
};

class CoverStopAction : public Trigger<> {
 public:
  CoverStopAction(MotionCover *cover) {
    cover->set_stop_action([this]() { this->trigger(); });
  }
};

class CoverForceStopAction : public Trigger<> {
 public:
  CoverForceStopAction(MotionCover *cover) {
    cover->set_force_stop_action([this]() { this->trigger(); });
  }
};

}  // namespace motion
}  // namespace esphome
