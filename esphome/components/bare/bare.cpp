#include "bare.h"

namespace esphome {
namespace bare {
static const char *const TAG = "bare_sensor";

void BareSensor::setup() { this->publish_state(0); }
void BareSensor::dump_config() { LOG_SENSOR("", "Bare Sensor", this); }
void BareSensor::set_value(int value) { this->publish_state(value); }
float BareSensor::get_setup_priority() const { return setup_priority::DATA; }
}  // namespace bare
}  // namespace esphome
