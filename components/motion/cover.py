from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import (
    CONF_CLOSE_ACTION,
    CONF_ID,
    CONF_OPEN_ACTION,
    CONF_POSITION,
    CONF_STOP_ACTION,
)

CONF_ALMOST_OPEN_ACTION = "almost_open_action"
CONF_ALMOST_CLOSED_ACTION = "almost_closed_action"
CONF_FORCE_STOP_ACTION = "force_stop_action"
CONF_ALMOST_AT_TARGET = "almost_at_target"
CONF_CAN_OPEN = "can_open"
CONF_CAN_CLOSE = "can_close"
CONF_EXTRA_CLOSE_DURATION = "extra_close_duration"
CONF_TARGET_MARGIN = "target_margin"

motion_ns = cg.esphome_ns.namespace("motion")
MotionCover = motion_ns.class_("MotionCover", cover.Cover, cg.Component)

CONFIG_SCHEMA = cover.COVER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(MotionCover),
        cv.Required(CONF_OPEN_ACTION): automation.validate_automation(single=True),
        cv.Required(CONF_ALMOST_OPEN_ACTION): automation.validate_automation(
            single=True
        ),
        cv.Required(CONF_CLOSE_ACTION): automation.validate_automation(single=True),
        cv.Required(CONF_ALMOST_CLOSED_ACTION): automation.validate_automation(
            single=True
        ),
        cv.Required(CONF_STOP_ACTION): automation.validate_automation(single=True),
        cv.Required(CONF_FORCE_STOP_ACTION): automation.validate_automation(
            single=True
        ),
        cv.Required(CONF_POSITION): cv.returning_lambda,
        cv.Optional(CONF_ALMOST_AT_TARGET, 0.1): cv.float_,
        cv.Optional(CONF_TARGET_MARGIN, 0.02): cv.float_,
        cv.Required(CONF_CAN_OPEN): cv.returning_lambda,
        cv.Required(CONF_CAN_CLOSE): cv.returning_lambda,
        cv.Required(CONF_EXTRA_CLOSE_DURATION): cv.int_,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cover.register_cover(var, config)

    await automation.build_automation(
        var.get_open_trigger(), [], config[CONF_OPEN_ACTION]
    )
    await automation.build_automation(
        var.get_almost_open_trigger(), [], config[CONF_ALMOST_OPEN_ACTION]
    )
    await automation.build_automation(
        var.get_close_trigger(), [], config[CONF_CLOSE_ACTION]
    )
    await automation.build_automation(
        var.get_almost_closed_trigger(), [], config[CONF_ALMOST_CLOSED_ACTION]
    )
    await automation.build_automation(
        var.get_stop_trigger(), [], config[CONF_STOP_ACTION]
    )
    await automation.build_automation(
        var.get_force_stop_trigger(), [], config[CONF_FORCE_STOP_ACTION]
    )

    lambda_position_ = await cg.process_lambda(
        config[CONF_POSITION], [], return_type=cg.float_
    )
    cg.add(var.set_position(lambda_position_))

    lambda_can_open_ = await cg.process_lambda(
        config[CONF_CAN_OPEN], [], return_type=cg.bool_
    )
    cg.add(var.set_can_open(lambda_can_open_))

    lambda_can_close_ = await cg.process_lambda(
        config[CONF_CAN_CLOSE], [], return_type=cg.bool_
    )
    cg.add(var.set_can_close(lambda_can_close_))

    cg.add(var.set_almost_at_target(config[CONF_ALMOST_AT_TARGET]))
    cg.add(var.set_target_margin(config[CONF_TARGET_MARGIN]))
    cg.add(var.set_extra_close_duration(config[CONF_EXTRA_CLOSE_DURATION]))
