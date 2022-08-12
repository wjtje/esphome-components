from esphome.components import output
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_DURATION,
    CONF_ID,
    CONF_PIN,
)

CONF_DEADZONE = "deadzone"

DEPENDENCIES = ["esp8266"]


smooth_pwm_ns = cg.esphome_ns.namespace("smooth_pwm")
SmoothPWM = smooth_pwm_ns.class_("SmoothPWM", output.FloatOutput, cg.Component)

CONFIG_SCHEMA = cv.All(
    output.FLOAT_OUTPUT_SCHEMA.extend(
        {
            cv.Required(CONF_ID): cv.declare_id(SmoothPWM),
            cv.Required(CONF_PIN): cv.int_,
            cv.Optional(CONF_DURATION): cv.int_,
            cv.Optional(CONF_DEADZONE): cv.int_,
        }
    ).extend(cv.COMPONENT_SCHEMA),
    cv.require_framework_version(
        esp8266_arduino=cv.Version(2, 4, 0),
    ),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await output.register_output(var, config)

    cg.add(var.set_pin(config[CONF_PIN]))
    cg.add(var.set_duration(config[CONF_DURATION]))
    cg.add(var.set_deadzone(config[CONF_DEADZONE]))
