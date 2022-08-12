#pragma once
#include "esphome/core/component.h"
#include "esphome/core/color.h"

namespace esphome {
// forward declare DisplayBuffer, Font, Glyph & TextAlign
namespace display {
class DisplayBuffer;
class Font;
class Glyph;
enum class TextAlign;
}  // namespace display

namespace scrolling_text {

class ScrollingText : public Component {
 public:
  void draw(display::DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, uint16_t width, Color color);

  void dump_config() override;

  void set_font(display::Font *font);
  void set_scroll_speed(uint32_t scroll_speed);
  void set_value(const std::string &value);
  void set_wait_time(uint32_t wait_time);

 protected:
  std::string value_;
  display::Font *font_;
  int offset_;
  /// The previous time the offset has been changed (in mills)
  uint32_t previous_update_;
  /// The time when the wait period started (in mills)
  uint32_t wait_;
  /// The time it takes to move 1px (in mills)
  uint32_t scroll_speed_;
  /// The time to wait at the beginning or end (in mills)
  uint32_t wait_time_;
};
}  // namespace scrolling_text
}  // namespace esphome