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
    CONF_TRIGGER_ID,
)

CONF_ALMOST_OPEN_ACTION = "almost_open_action"
CONF_ALMOST_CLOSED_ACTION = "almost_closed_action"
CONF_FORCE_STOP_ACTION = "force_stop_action"
CONF_ALMOST_CLOSED = "almost_closed"
CONF_CAN_OPEN = "can_open"
CONF_CAN_CLOSE = "can_close"

motion_ns = cg.esphome_ns.namespace("motion")
MotionCover = motion_ns.class_("MotionCover", cover.Cover, cg.Component)

CoverOpenAction = motion_ns.class_("CoverOpenAction", automation.Trigger.template())
CoverAlmostOpenAction = motion_ns.class_(
    "CoverAlmostOpenAction", automation.Trigger.template()
)
CoverCloseAction = motion_ns.class_("CoverCloseAction", automation.Trigger.template())
CoverAlmostCloseAction = motion_ns.class_(
    "CoverAlmostCloseAction", automation.Trigger.template()
)
CoverStopAction = motion_ns.class_("CoverStopAction", automation.Trigger.template())
CoverForceStopAction = motion_ns.class_(
    "CoverForceStopAction", automation.Trigger.template()
)

CONFIG_SCHEMA = cover.COVER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(MotionCover),
        cv.Required(CONF_OPEN_ACTION): automation.validate_automation(
            {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CoverOpenAction)}
        ),
        cv.Required(CONF_ALMOST_OPEN_ACTION): automation.validate_automation(
            {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CoverAlmostOpenAction)}
        ),
        cv.Required(CONF_CLOSE_ACTION): automation.validate_automation(
            {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CoverCloseAction)}
        ),
        cv.Required(CONF_ALMOST_CLOSED_ACTION): automation.validate_automation(
            {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CoverAlmostCloseAction)}
        ),
        cv.Required(CONF_STOP_ACTION): automation.validate_automation(
            {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CoverStopAction)}
        ),
        cv.Required(CONF_FORCE_STOP_ACTION): automation.validate_automation(
            {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CoverForceStopAction)}
        ),
        cv.Required(CONF_POSITION): cv.returning_lambda,
        cv.Required(CONF_ALMOST_CLOSED): cv.float_,
        cv.Required(CONF_CAN_OPEN): cv.returning_lambda,
        cv.Required(CONF_CAN_CLOSE): cv.returning_lambda,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cover.register_cover(var, config)

    cg.add(var.set_almost_closed(config[CONF_ALMOST_CLOSED]))

    for conf in config.get(CONF_OPEN_ACTION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ALMOST_OPEN_ACTION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_CLOSE_ACTION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_ALMOST_CLOSED_ACTION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_STOP_ACTION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    for conf in config.get(CONF_FORCE_STOP_ACTION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)

    template_pos_ = await cg.process_lambda(
        config[CONF_POSITION], [], return_type=cg.optional.template(float)
    )
    cg.add(var.set_position(template_pos_))

    template_ope_ = await cg.process_lambda(
        config[CONF_CAN_OPEN], [], return_type=cg.optional.template(float)
    )
    cg.add(var.set_position(template_ope_))

    template_clo_ = await cg.process_lambda(
        config[CONF_CAN_OPEN], [], return_type=cg.optional.template(float)
    )
    cg.add(var.set_position(template_clo_))
