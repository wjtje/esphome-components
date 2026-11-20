import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import CONF_BRIGHTNESS, CONF_ID, CONF_LAMBDA, CONF_ROTATION

from . import kortlinje_ns

CODEOWNERS = [""]
DEPENDENCIES = ["display"]

CONF_CLOCK_PINS = "clock_pins"
CONF_DATA_PINS = "data_pins"

KortlinjeDisplay = kortlinje_ns.class_("KortlinjeDisplay", display.Display)

CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(KortlinjeDisplay),
        cv.Required(CONF_CLOCK_PINS): cv.ensure_list(pins.gpio_output_pin_schema),
        cv.Required(CONF_DATA_PINS): cv.ensure_list(pins.gpio_output_pin_schema),
        cv.Optional(CONF_ROTATION, default=0): display.validate_rotation,
        cv.Optional(CONF_BRIGHTNESS, default=7): cv.int_range(min=0, max=7),
    }
)


def _validate_pins(config):
    clock = config[CONF_CLOCK_PINS]
    data = config[CONF_DATA_PINS]
    if not 1 <= len(clock) <= 3:
        raise cv.Invalid("clock_pins needs 1 to 3 entries")
    if len(clock) != len(data):
        raise cv.Invalid("clock_pins and data_pins must have the same length")
    return config


FINAL_VALIDATE_SCHEMA = _validate_pins


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)

    for conf in config[CONF_CLOCK_PINS]:
        await cg.add_pin(var.add_clock_pin, conf)
    for conf in config[CONF_DATA_PINS]:
        await cg.add_pin(var.add_data_pin, conf)

    cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))

    if hasattr(display, "add_writer"):
        await display.add_writer(var, config)
    elif (lambda_config := config.get(CONF_LAMBDA)) is not None:
        lambda_ = await cg.process_lambda(
            lambda_config, [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
