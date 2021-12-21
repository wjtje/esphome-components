import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import (
    CONF_ID,
    CONF_LAMBDA,
    CONF_MODE,
    CONF_PAGES,
    CONF_SIZE,
    CONF_UPDATE_INTERVAL,
)

CODEOWNERS = ["@wjtje"]

vga_ns = cg.esphome_ns.namespace("vga")
VGA = vga_ns.class_("VGA", cg.Component, display.DisplayBuffer)

VGA_MODEL = cv.one_of("3BIT", "6BIT", upper=True)
VGA_SIZE = cv.one_of("200X150", "320X240", upper=True)

CONF_HSYNC = "hsync"
CONF_VSYNC = "vsync"

CONF_RED0 = "red0"
CONF_RED1 = "red1"
CONF_GREEN0 = "green0"
CONF_GREEN1 = "green1"
CONF_BLUE0 = "blue0"
CONF_BLUE1 = "blue1"


def _validate(value):
    mode = value[CONF_MODE]
    if mode in ("6BIT"):
        # All 6-bit modes need at least 2 red, green and blue pins
        if CONF_RED1 not in value:
            raise cv.Invalid("Red channel 2 is required for 6-bit mode")
        if CONF_GREEN1 not in value:
            raise cv.Invalid("Green channel 2 is required for 6-bit mode")
        if CONF_BLUE1 not in value:
            raise cv.Invalid("Blue channel 2 is required for 6-bit mode")

    return value


CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(VGA),
            cv.Optional(CONF_MODE, default="3BIT"): VGA_MODEL,
            cv.Optional(CONF_SIZE, default="200X150"): VGA_SIZE,
            cv.Optional(
                CONF_UPDATE_INTERVAL, default="41ms"
            ): cv.positive_time_period_milliseconds,
            cv.Required(CONF_HSYNC): pins.internal_gpio_output_pin_number,
            cv.Required(CONF_VSYNC): pins.internal_gpio_output_pin_number,
            cv.Required(CONF_RED0): pins.internal_gpio_output_pin_number,
            cv.Optional(CONF_RED1): pins.internal_gpio_output_pin_number,
            cv.Required(CONF_GREEN0): pins.internal_gpio_output_pin_number,
            cv.Optional(CONF_GREEN1): pins.internal_gpio_output_pin_number,
            cv.Required(CONF_BLUE0): pins.internal_gpio_output_pin_number,
            cv.Optional(CONF_BLUE1): pins.internal_gpio_output_pin_number,
        }
    ),
    cv.has_at_least_one_key(CONF_PAGES, CONF_LAMBDA),
    _validate,
)


async def to_code(config):
    cg.add_library("https://github.com/wjtje/ESP32Lib.git", "0.3.4")

    var = cg.new_Pvariable(config[CONF_ID])

    cg.add_define("USE_VGA_" + config[CONF_MODE])
    cg.add_define("USE_VGA_SIZE_" + config[CONF_SIZE])

    if CONF_VSYNC in config:
        cg.add(var.set_vsync(cg.int_(config["vsync"])))
    if CONF_HSYNC in config:
        cg.add(var.set_hsync(cg.int_(config["hsync"])))
    if CONF_RED0 in config:
        cg.add(var.set_red_p0(cg.int_(config["red0"])))
    if CONF_RED1 in config:
        cg.add(var.set_red_p1(cg.int_(config["red1"])))
    if CONF_GREEN0 in config:
        cg.add(var.set_green_p0(cg.int_(config["green0"])))
    if CONF_GREEN1 in config:
        cg.add(var.set_green_p1(cg.int_(config["green1"])))
    if CONF_BLUE0 in config:
        cg.add(var.set_blue_p0(cg.int_(config["blue0"])))
    if CONF_BLUE1 in config:
        cg.add(var.set_blue_p1(cg.int_(config["blue1"])))

    await cg.register_component(var, config)
    await display.register_display(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
