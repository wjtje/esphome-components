#include "scrolling_text.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/color.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace scrolling_text {

static const char *const TAG = "scrolling_text";

void ScrollingText::dump_config() {
  ESP_LOGCONFIG(TAG, "Scrolling text:");
  ESP_LOGCONFIG(TAG, "  Value: '%s'", this->value_.c_str());
}

void ScrollingText::set_font(display::Font *font) { this->font_ = font; }

void ScrollingText::set_scroll_speed(uint32_t scroll_speed) { this->scroll_speed_ = scroll_speed; }

void ScrollingText::set_value(const std::string &value) { this->value_ = value; }

void ScrollingText::set_wait_time(uint32_t wait_time) { this->wait_time_ = wait_time; }

void ScrollingText::draw(display::DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, uint16_t width,
                         Color color) {
  const char *text = this->value_.c_str();

  int x_start, y_start;
  int text_width, text_height;
  buff->get_text_bounds(x_offset, y_offset, text, this->font_, display::TextAlign::TOP_LEFT, &x_start, &y_start,
                        &text_width, &text_height);

  const uint32_t now = millis();

  // Check if we are in a wait period
  if (this->wait_ != 0) {
    // Check if the wait period is over
    if (now - this->wait_ > this->wait_time_) {
      // If we are at the end, move to the beginning
      if (this->offset_ != 0) {
        this->wait_ = now;
        this->offset_ = 0;
      } else {
        // Stop waiting if we are at the beginning
        this->wait_ = 0;
      }
    }
  } else {
    // Check if we need to update
    if (now - this->previous_update_ > this->scroll_speed_) {
      this->offset_++;
      this->previous_update_ = now;
    }

    // Check if we are at the end
    if (this->offset_ > text_width - width) {
      this->offset_ = text_width - width;
      this->wait_ = now;
    }
  }

  int i = 0;
  int x_at = x_start - this->offset_;
  int x_max = x_offset + width;
  while (text[i] != '\0') {
    int match_length;
    int glyph_n = this->font_->match_next_glyph(text + i, &match_length);
    if (glyph_n < 0) {
      // Unknown char, skip
      ESP_LOGW(TAG, "Encountered character without representation in font: '%c'", text[i]);
      if (!this->font_->get_glyphs().empty()) {
        int scan_x1, scan_y1, scan_width, scan_height;
        this->font_->get_glyphs()[0].scan_area(&scan_x1, &scan_y1, &scan_width, &scan_height);
        for (int glyph_x = 0; glyph_x < scan_width; glyph_x++) {
          for (int glyph_y = 0; glyph_y < text_height; glyph_y++)
            if (glyph_x + x_at >= x_offset && glyph_x + x_at < x_max) {
              buff->draw_pixel_at(glyph_x + x_at, glyph_y + y_start, color);
            }
        }
        x_at += scan_width;
      }

      i++;
      continue;
    }

    const display::Glyph &glyph = this->font_->get_glyphs()[glyph_n];
    int scan_x1, scan_y1, scan_width, scan_height;
    glyph.scan_area(&scan_x1, &scan_y1, &scan_width, &scan_height);

    for (int glyph_x = scan_x1; glyph_x < scan_x1 + scan_width; glyph_x++) {
      for (int glyph_y = scan_y1; glyph_y < scan_y1 + scan_height; glyph_y++) {
        if (glyph.get_pixel(glyph_x, glyph_y) && glyph_x + x_at >= x_offset && glyph_x + x_at < x_max) {
          buff->draw_pixel_at(glyph_x + x_at, glyph_y + y_start, color);
        }
      }
    }

    x_at += scan_width + scan_x1;

    i += match_length;
  }
}
}  // namespace scrolling_text
}  // namespace esphome