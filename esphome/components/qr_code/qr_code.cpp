#include "qr_code.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/color.h"
#include "esphome/core/log.h"

namespace esphome {
namespace qr_code {

using namespace display;

static const char *const TAG = "qr_code";

void QRCode::setup() { ESP_LOGCONFIG(TAG, "Setting up QR code..."); }

void QRCode::dump_config() {
  ESP_LOGCONFIG(TAG, "QR code:");
  ESP_LOGCONFIG(TAG, "  Value: '%s'", this->value_.c_str());
}

void QRCode::set_value(const std::string &value) {
  this->value_ = value;
  uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
  bool ok = qrcodegen_encodeText(this->value_.c_str(), tempBuffer, qrcode, errCorLvl, qrcodegen_VERSION_MIN,
                                 qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);
}

void QRCode::draw(DisplayBuffer *buff, uint16_t x_offset, uint16_t y_offset, Color color) {
  int size = qrcodegen_getSize(this->qrcode);

  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      if (qrcodegen_getModule(this->qrcode, x, y)) {
        buff->draw_pixel_at(x_offset + x, y_offset + y, color);
      }
    }
  }
}
}  // namespace qr_code
}  // namespace esphome
