#include "vga.h"

namespace esphome {
namespace vga {
static const char *const TAG = "vga";

void VGA::setup() {
  ESP_LOGD(TAG, "Setting up VGA display");
  vga_.setFrameBufferCount(2);

  vga_.init(
#ifdef USE_VGA_SIZE_200X150
      vga_.MODE200x150,
#elif defined(USE_VGA_SIZE_320X240)
      vga_.MODE320x240,
#endif
#ifdef USE_VGA_3BIT
      this->red_p0_, this->green_p0_, this->blue_p0_, this->hsync_pin_, this->vsync_pin_);
#elif defined(USE_VGA_6BIT)
      this->red_p0_, this->red_p1_, this->green_p0_, this->green_p1_, this->blue_p0_, this->blue_p1_, this->hsync_pin_,
      this->vsync_pin_);
#endif
}

void HOT VGA::draw_absolute_pixel_internal(int x, int y, Color color) {
  vga_.dot(x, y, vga_.RGBA(color.r, color.g, color.b));
}

int VGA::get_width_internal() { return this->vga_.xres; }
int VGA::get_height_internal() { return this->vga_.yres; }

void VGA::update() {
  this->do_update_();
  vga_.show();
}

void VGA::dump_config() {
#ifdef USE_VGA_SIZE_200X150
  Mode mode = vga_.MODE200x150;
#elif defined(USE_VGA_SIZE_320X240)
  Mode mode = vga_.MODE320x240;
#endif

  LOG_DISPLAY("", "VGA", this);
  LOG_UPDATE_INTERVAL(this);
  ESP_LOGCONFIG(TAG, "  hFront: %i", mode.hFront);
  ESP_LOGCONFIG(TAG, "  hSync: %i", mode.hSync);
  ESP_LOGCONFIG(TAG, "  hBack: %i", mode.hBack);
  ESP_LOGCONFIG(TAG, "  vFront: %i", mode.vFront);
  ESP_LOGCONFIG(TAG, "  vSync: %i", mode.vSync);
  ESP_LOGCONFIG(TAG, "  vBackPorch: %i", mode.vBack);
  ESP_LOGCONFIG(TAG, "  vRes: %i", mode.vRes);
  ESP_LOGCONFIG(TAG, "  hDiv: %i", mode.vDiv);
  ESP_LOGCONFIG(TAG, "  pixelClock: %lu", mode.pixelClock);
  ESP_LOGCONFIG(TAG, "  hSyncPolarity: %i", mode.hSyncPolarity);
  ESP_LOGCONFIG(TAG, "  vSyncPolarity: %i", mode.vSyncPolarity);
  ESP_LOGCONFIG(TAG, "  VSync: GPIO%i", this->vsync_pin_);
  ESP_LOGCONFIG(TAG, "  HSync: GPIO%i", this->hsync_pin_);
  ESP_LOGCONFIG(TAG, "  Red P0: GPIO%i", this->red_p0_);
  ESP_LOGCONFIG(TAG, "  Green P0: GPIO%i", this->green_p0_);
  ESP_LOGCONFIG(TAG, "  Blue P0: GPIO%i", this->blue_p0_);
  ESP_LOGCONFIG(TAG, "  Red P1: GPIO%i", this->red_p1_);
  ESP_LOGCONFIG(TAG, "  Green P1: GPIO%i", this->green_p1_);
  ESP_LOGCONFIG(TAG, "  Blue P1: GPIO%i", this->blue_p1_);
}
}  // namespace vga
}  // namespace esphome
