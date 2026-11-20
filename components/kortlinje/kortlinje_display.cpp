#include "kortlinje_display.h"

namespace esphome {
namespace kortlinje {

static const char *const TAG = "kortlinje";
static constexpr uint8_t INDICATOR_ADDR[5][2] = {{1, 127}, {2, 126}, {1, 126}, {0, 127}, {0, 126}};

void KortlinjeDisplay::delay_us_(uint32_t us) {
  esp_rom_delay_us(us);
}

void KortlinjeDisplay::clock_(bool level) {
  for (auto *pin : this->clock_pins_)
    pin->digital_write(level);
}

void KortlinjeDisplay::data_(bool level) {
  for (auto *pin : this->data_pins_)
    pin->digital_write(level);
}

void KortlinjeDisplay::start_() {
  this->clock_(false);
  this->data_(true);
  this->delay_us_(this->half_us_);
  this->clock_(true);
  this->delay_us_(this->half_us_);
  this->data_(false);
  this->delay_us_(this->half_us_);
  this->clock_(false);
}

void KortlinjeDisplay::stop_() {
  this->clock_(false);
  this->data_(false);
  this->delay_us_(this->half_us_);
  this->clock_(true);
  this->delay_us_(this->half_us_);
  this->data_(true);
  this->delay_us_(this->half_us_);
}

void KortlinjeDisplay::write_bits_(const uint8_t *values) {
  const size_t n = this->data_pins_.size();
  for (uint8_t bit = 0; bit < 8; bit++) {
    this->clock_(false);
    for (size_t i = 0; i < n; i++)
      this->data_pins_[i]->digital_write(((values[i] >> bit) & 1) != 0);
    this->delay_us_(this->half_us_);
    this->clock_(true);
    this->delay_us_(this->half_us_);
    this->clock_(false);
  }
}

void KortlinjeDisplay::command_(uint8_t value) {
  const uint8_t values[KORTLINJE_MAX_CHIPS] = {value, value, value};
  this->start_();
  this->write_bits_(values);
  this->stop_();
}

void KortlinjeDisplay::send_display_on_() { this->command_(uint8_t(0x88 | this->brightness_)); }

void KortlinjeDisplay::flush_() {
  if (this->data_pins_.empty())
    return;

  this->command_(0x40);  // auto-increment write mode
  this->start_();
  const uint8_t start_address[KORTLINJE_MAX_CHIPS] = {0xC0, 0xC0, 0xC0};
  this->write_bits_(start_address);
  for (uint8_t address = 0; address < 16; address++) {
    uint8_t row[KORTLINJE_MAX_CHIPS] = {0, 0, 0};
    for (size_t chip = 0; chip < this->data_pins_.size(); chip++)
      row[chip] = this->ram_[chip][address];
    this->write_bits_(row);
  }
  this->stop_();
  this->send_display_on_();
}

void KortlinjeDisplay::setup() {
  for (auto *pin : this->clock_pins_) {
    pin->setup();
    pin->digital_write(false);
  }

  for (auto *pin : this->data_pins_) {
    pin->setup();
    pin->digital_write(true);
  }

  const size_t n = std::min<size_t>(KORTLINJE_MAX_CHIPS, this->data_pins_.size());
  this->native_width_ = int(KORTLINJE_COLS_PER_CHIP * n);

  this->clock_(false);
  this->data_(false);
  this->delay_us_(this->half_us_);

  this->stop_();
  this->command_(0x80);
  this->clear();
  this->flush_();
  ESP_LOGI(TAG, "Initialized %dx%d VK1640 matrix (%u chip(s))", this->native_width_, KORTLINJE_ROWS,
           unsigned(n));
}

void KortlinjeDisplay::update() {
  this->do_update_();
  this->flush_();
}

void KortlinjeDisplay::clear() { memset(this->ram_, 0, sizeof(this->ram_)); }

void KortlinjeDisplay::fill(Color color) {
  memset(this->ram_, color_is_on_(color) ? 0xFF : 0x00, sizeof(this->ram_));
}

void KortlinjeDisplay::set_indicator(uint8_t indicator, bool on) {
  if (indicator > 4 || this->data_pins_.size() != KORTLINJE_MAX_CHIPS)
    return;
  uint8_t &value = this->ram_[INDICATOR_ADDR[indicator][0]][INDICATOR_ADDR[indicator][1] >> 3];
  const uint8_t mask = uint8_t(1U << (INDICATOR_ADDR[indicator][1] & 7));
  value = on ? (value | mask) : (value & uint8_t(~mask));
}

void KortlinjeDisplay::draw_pixel_at(int x, int y, Color color) {
  const int nw = this->get_width_internal();
  const int nh = this->get_height_internal();
  const auto rotation = this->get_rotation();
  const bool swapped = display::rotation_swaps_axes(rotation);

  if (x < 0 || y < 0 || x >= (swapped ? nh : nw) || y >= (swapped ? nw : nh))
    return;
  if (this->is_clipping() && !this->get_clipping().inside(x, y))
    return;

  switch (rotation) {
    case display::DISPLAY_ROTATION_90_DEGREES: {
      const int t = x;
      x = y;
      y = nw - 1 - t;
      break;
    }
    case display::DISPLAY_ROTATION_180_DEGREES:
      x = nw - 1 - x;
      y = nh - 1 - y;
      break;
    case display::DISPLAY_ROTATION_270_DEGREES: {
      const int t = x;
      x = nh - 1 - y;
      y = t;
      break;
    }
    default:
      break;
  }

  const uint8_t n = uint8_t(this->data_pins_.size());
  const uint8_t chip = uint8_t((n - 1) - uint8_t(x / KORTLINJE_COLS_PER_CHIP));
  if (chip >= KORTLINJE_MAX_CHIPS)
    return;
  const uint8_t local_x = uint8_t(x % KORTLINJE_COLS_PER_CHIP);
  const uint8_t index = local_x < 6 ? uint8_t(125 - 6 * y - local_x) : uint8_t(71 - 8 * y - (local_x - 6));

  uint8_t &value = this->ram_[chip][index >> 3];
  const uint8_t mask = uint8_t(1U << (index & 7));
  value = color_is_on_(color) ? (value | mask) : (value & uint8_t(~mask));
}

int KortlinjeDisplay::get_width() {
  return display::rotation_swaps_axes(this->get_rotation()) ? this->get_height_internal()
                                                            : this->get_width_internal();
}

int KortlinjeDisplay::get_height() {
  return display::rotation_swaps_axes(this->get_rotation()) ? this->get_width_internal()
                                                            : this->get_height_internal();
}

display::DisplayType KortlinjeDisplay::get_display_type() {
  return display::DISPLAY_TYPE_BINARY;
}

void KortlinjeDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "KORTLINJE VK1640 dot matrix");
  ESP_LOGCONFIG(TAG, "  Size: %dpx x %dpx native", this->get_width_internal(), this->get_height_internal());
  ESP_LOGCONFIG(TAG, "  Chips: %u", unsigned(this->data_pins_.size()));
  ESP_LOGCONFIG(TAG, "  Rotation: %d", int(this->get_rotation()));
  ESP_LOGCONFIG(TAG, "  Brightness: %u/7, half period: %uus", unsigned(this->brightness_),
                unsigned(this->half_us_));
}

}  // namespace kortlinje
}  // namespace esphome
