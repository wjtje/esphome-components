import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover, output, sensor
from esphome.const import (
    CONF_ID,
    CONF_UPDATE_INTERVAL
)

input_based_ns = cg.esphome_ns.namespace("input_based")
InputBasedCover = input_based_ns.class_("InputBasedCover", cover.Cover, cg.Component)

CONF_OPEN_OUTPUT = "open_output"
CONF_CLOSE_OUTPUT = "close_output"
CONF_STATE_INPUT = "state_input"
CONF_MOVING_TIMEOUT = "moving_timeout"
CONF_MOVING_DIFFERENCE = "moving_difference"
CONF_SLOW_TARGET_MARGIN = "slow_target_margin"
CONF_SLOW_TARGET_SPEED = "slow_target_speed"
CONF_TARGET_MARGIN = "target_margin"

CONFIG_SCHEMA = cover.COVER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(InputBasedCover),
        cv.Required(CONF_OPEN_OUTPUT): cv.use_id(output.FloatOutput),
        cv.Required(CONF_CLOSE_OUTPUT): cv.use_id(output.FloatOutput),
        cv.Required(CONF_STATE_INPUT): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
        cv.Optional(CONF_MOVING_TIMEOUT, default="2000ms"): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_MOVING_DIFFERENCE, default="0.05"): cv.float_,
        cv.Optional(CONF_SLOW_TARGET_MARGIN, default="0.10"): cv.float_,
        cv.Optional(CONF_SLOW_TARGET_SPEED, default="0.25"): cv.float_,
        cv.Optional(CONF_TARGET_MARGIN, default="0.01"): cv.float_,
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cover.register_cover(var, config)

    open_out = await cg.get_variable(config[CONF_OPEN_OUTPUT])
    cg.add(var.set_open_output(open_out))

    close_out = await cg.get_variable(config[CONF_CLOSE_OUTPUT])
    cg.add(var.set_close_output(close_out))

    state_inp = await cg.get_variable(config[CONF_STATE_INPUT])
    cg.add(var.set_state_input(state_inp))

    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))
    cg.add(var.set_moving_timeout(config[CONF_MOVING_TIMEOUT]))
    cg.add(var.set_moving_difference(config[CONF_MOVING_DIFFERENCE]))
    cg.add(var.set_slow_target_margin(config[CONF_SLOW_TARGET_MARGIN]))
    cg.add(var.set_slow_target_speed(config[CONF_SLOW_TARGET_SPEED]))
    cg.add(var.set_target_margin(config[CONF_TARGET_MARGIN]))
