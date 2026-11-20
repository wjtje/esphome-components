#pragma once

#include <algorithm>
#include <cstring>
#include <vector>

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/core/gpio.h"
#include "esphome/components/display/display.h"

#if defined(USE_ESP32)
#include <esp_rom_sys.h>
#endif

namespace esphome {
namespace kortlinje {

static constexpr uint8_t KORTLINJE_COLS_PER_CHIP = 14;
static constexpr uint8_t KORTLINJE_ROWS = 9;
static constexpr uint8_t KORTLINJE_MAX_CHIPS = 3;

enum Indicator : uint8_t {
  LEFT_TOP = 0,
  LEFT_MIDDLE = 1,
  LEFT_BOTTOM = 2,
  RIGHT_TOP = 3,
  RIGHT_BOTTOM = 4,
};

class KortlinjeDisplay : public display::Display {
 public:
  void add_clock_pin(GPIOPin *pin) { this->clock_pins_.push_back(pin); }
  void add_data_pin(GPIOPin *pin) { this->data_pins_.push_back(pin); }

  void set_brightness(uint8_t brightness) { this->brightness_ = brightness & 0x07; }
  void set_half_us(uint16_t half_us) { this->half_us_ = half_us; }

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }

  void clear() override;
  void fill(Color color) override;
  void draw_pixel_at(int x, int y, Color color) override;

  int get_width() override;
  int get_height() override;

  display::DisplayType get_display_type() override;

  void set_indicator(uint8_t indicator, bool on = true);
  void flush() { this->flush_(); }

 protected:
  static bool color_is_on_(const Color &color) {
    return color.r != 0 || color.g != 0 || color.b != 0;
  }

  int get_width_internal() override { return this->native_width_; }
  int get_height_internal() override { return KORTLINJE_ROWS; }

  void flush_();
  void send_display_on_();
  void clock_(bool level);
  void data_(bool level);
  void start_();
  void stop_();
  void write_bits_(const uint8_t *values);
  void command_(uint8_t value);
  void delay_us_(uint32_t us);

  std::vector<GPIOPin *> clock_pins_{};
  std::vector<GPIOPin *> data_pins_{};
  uint8_t ram_[KORTLINJE_MAX_CHIPS][16]{};
  int native_width_{KORTLINJE_COLS_PER_CHIP * KORTLINJE_MAX_CHIPS};
  uint8_t brightness_{7};
  uint16_t half_us_{10};
};

}  // namespace kortlinje
}  // namespace esphome
