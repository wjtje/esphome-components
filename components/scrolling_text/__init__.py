import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_VALUE, CONF_WAIT_TIME
import esphome.components.font as font

CONF_FONT = "font"
CONF_SCROLL_SPEED = "scroll_speed"

CODEOWNERS = ["@wjtje"]

DEPENDENCIES = ["display", "font"]
MULTI_CONF = True

scrolling_text_ns = cg.esphome_ns.namespace("scrolling_text")
ScrollingText = scrolling_text_ns.class_("ScrollingText", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.declare_id(ScrollingText),
        cv.Required(CONF_VALUE): cv.string,
        cv.Required(CONF_FONT): cv.use_id(font),
        cv.Optional(
            CONF_WAIT_TIME, default="1000ms"
        ): cv.positive_time_period_milliseconds,
        cv.Optional(
            CONF_SCROLL_SPEED, default="250ms"
        ): cv.positive_time_period_milliseconds,
    }
)


async def to_code(config):
    font_obj = await cg.get_variable(config[CONF_FONT])

    var = cg.new_Pvariable(config[CONF_ID])

    cg.add(var.set_font(font_obj))
    cg.add(var.set_scroll_speed(config[CONF_SCROLL_SPEED]))
    cg.add(var.set_value(config[CONF_VALUE]))
    cg.add(var.set_wait_time(config[CONF_WAIT_TIME]))

    await cg.register_component(var, config)
