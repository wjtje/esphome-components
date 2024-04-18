from esphome.components import output
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import (
    CONF_DURATION,
    CONF_ID,
    CONF_PIN,
)

CONF_DEADZONE = "deadzone"
CONF_PAIR_OUTPUT = "pair_output"

DEPENDENCIES = ["esp8266"]

smooth_pwm_ns = cg.esphome_ns.namespace("smooth_pwm")
SmoothPWM = smooth_pwm_ns.class_("SmoothPWM", output.FloatOutput, cg.Component)

CONFIG_SCHEMA = cv.All(
    output.FLOAT_OUTPUT_SCHEMA.extend(
        {
            cv.Required(CONF_ID): cv.declare_id(SmoothPWM),
            cv.Required(CONF_PIN): cv.int_,
            cv.Optional(CONF_DURATION, 1000): cv.int_,
            cv.Optional(CONF_DEADZONE, 0): cv.int_,
            cv.Optional(CONF_PAIR_OUTPUT): cv.use_id(SmoothPWM)
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

    if CONF_PAIR_OUTPUT in config:
        out = await cg.get_variable(config[CONF_PAIR_OUTPUT])
        cg.add(var.set_pair_output(out))
