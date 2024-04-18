#pragma once

#include "esphome/core/component.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace input_based {

class InputBasedCover : public cover::Cover, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  cover::CoverTraits get_traits() override;

  void set_open_output(output::FloatOutput *output) { this->open_output_ = output; }
  void set_close_output(output::FloatOutput *output) { this->close_output_ = output; }
  void set_state_input(sensor::Sensor *input) { this->state_input_ = input; }

  void set_update_interval(uint32_t interval) { this->update_interval_ = interval; }
  void set_moving_timeout(uint32_t timeout) { this->moving_timeout_ = timeout; }
  void set_moving_difference(float value) { this->moving_difference_ = value; }
  void set_slow_target_margin(float value) { this->slow_target_margin_ = value; }
  void set_slow_target_speed(float value) { this->slow_target_speed_ = value; }
  void set_target_margin(float value) { this->target_margin_ = value; }

 protected:
  void control(const cover::CoverCall &call) override;

  /**
   * @brief Returns true when the current position is the same as the target. With an optional
   * margin.
   *
   * @param margin[in]
   */
  bool at_position_(float margin);

  output::FloatOutput *open_output_;
  output::FloatOutput *close_output_;
  sensor::Sensor *state_input_;

  uint32_t moving_timeout_{2000};  /// @brief The time (is ms) where is the cover should move, else we will stop
  float moving_difference_{0.0f};  /// @brief The amount that the cover should move, before registing as a move

  float slow_target_margin_{0.05f};
  float slow_target_speed_{0.25f};
  float target_margin_{0.0f};

  uint32_t update_interval_;
  uint32_t last_update_{0};

  float target_position_{0.0f};

  bool moving_{false};        /// @brief This boolean is true when the cover is allowed to move
  uint32_t moving_time_{0};   /// @brief The time (in ms) when the cover (should) started moving
  float moving_start_{0.0f};  /// @brief The position before the cover started moving
};

}  // namespace input_based
}  // namespace esphome
