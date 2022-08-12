#pragma once

#include "esphome/core/component.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/smooth_pwm/smooth_pwm.h"
#include "esphome/components/adc/adc_sensor.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace bhgta {

class BhgtaCover : public cover::Cover, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_pwm_in(smooth_pwm::SmoothPWM *pwm_in) { this->pwm_in_ = pwm_in; }
  void set_pwm_out(smooth_pwm::SmoothPWM *pwm_out) { this->pwm_out_ = pwm_out; }
  void set_pot(adc::ADCSensor *pot) { this->pot_ = pot; }
  void set_almost(float almost) { this->almost_ = almost; }
  void add_on_force_stop_callback(std::function<void()> &&callback) {
    this->force_stop_callback_.add(std::move(callback));
  };

  cover::CoverTraits get_traits() override;

 protected:
  void control(const cover::CoverCall &call) override;

  bool is_open_() { return this->pot_->get_state() >= 1.0f; }
  bool is_almost_open_() { return this->pot_->get_state() >= 1.0f - this->almost_; }
  bool is_closed_() { return this->pot_->get_state() <= 0.0f; }
  bool is_almost_closed_() { return this->pot_->get_state() <= this->almost_; }
  bool is_at_target_();

  float calculate_position_() {
    float state = this->pot_->state;

    if (state < 0.0f) {
      return 0.0f;
    } else if (state > 1.0f) {
      return 1.0f;
    } else {
      return roundf(state * 100) / 100;
    }
  }

  smooth_pwm::SmoothPWM *pwm_in_;
  smooth_pwm::SmoothPWM *pwm_out_;
  adc::ADCSensor *pot_;

  float almost_;
  float almost_power_{0.1f};

  float target_position_{0};

  uint32_t last_publish_time_{0};

  float last_position_{0};
  uint32_t last_position_time_{0};

  CallbackManager<void()> force_stop_callback_{};
};

class CoverForceStopTrigger : public Trigger<> {
 public:
  CoverForceStopTrigger(BhgtaCover *cover) {
    cover->add_on_force_stop_callback([this]() { this->trigger(); });
  }
};

}  // namespace bhgta
}  // namespace esphome
