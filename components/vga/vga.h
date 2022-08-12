#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/log.h"

#include <VGA/VGA6Bit.h>
#include <VGA/VGA3Bit.h>

namespace esphome {
namespace vga {
class VGA : public PollingComponent, public display::DisplayBuffer {
 public:
  void setup() override;

  void display();

  void update() override;

  void set_vsync(int vsync_pin) { this->vsync_pin_ = vsync_pin; }
  void set_hsync(int hsync_pin) { this->hsync_pin_ = hsync_pin; }
  void set_red_p0(int red_p0) { this->red_p0_ = red_p0; }
  void set_red_p1(int red_p1) { this->red_p1_ = red_p1; }
  void set_green_p0(int green_p0) { this->green_p0_ = green_p0; }
  void set_green_p1(int green_p1) { this->green_p1_ = green_p1; }
  void set_blue_p0(int blue_p0) { this->blue_p0_ = blue_p0; }
  void set_blue_p1(int blue_p1) { this->blue_p1_ = blue_p1; }
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;

  int vsync_pin_;
  int hsync_pin_;
  int red_p0_;
  int red_p1_;
  int green_p0_;
  int green_p1_;
  int blue_p0_;
  int blue_p1_;

#ifdef USE_VGA_3BIT
  VGA3Bit vga_;
#elif defined(USE_VGA_6BIT)
  VGA6Bit vga_;
#endif
};
}  // namespace vga
}  // namespace esphome
