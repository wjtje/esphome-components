#pragma once
#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include <stdbool.h>
#include <stdint.h>
#include "qrcodegen.h"

namespace esphome {
// forward declare DisplayBuffer
namespace display {
class DisplayBuffer;
}  // namespace display

namespace qr_code {

class QRCode : public Component {
 public:
  void draw(display::DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color color);

  void setup() override;
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  void dump_config() override;

  void set_value(const std::string &value);

 protected:
  std::string value_;
  uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
  enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_LOW;
};
}  // namespace qr_code
}  // namespace esphome
