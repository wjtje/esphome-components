from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover
from esphome.const import CONF_ID, CONF_TRIGGER_ID
from esphome.components.adc.sensor import ADCSensor
from esphome.components.smooth_pwm.output import SmoothPWM

CONF_PWM_IN = "pwm_in"
CONF_PWM_OUT = "pwm_out"
CONF_POT = "pot"
CONF_ALMOST = "pot_almost"
CONF_ON_FORCE_STOP = "on_force_stop"

DEPENDENCIES = ["esp8266"]

bhgta_ns = cg.esphome_ns.namespace("bhgta")
BhgtaCover = bhgta_ns.class_("BhgtaCover", cover.Cover, cg.Component)

CoverForceStopTrigger = bhgta_ns.class_(
    "CoverForceStopTrigger", automation.Trigger.template()
)

CONFIG_SCHEMA = cv.All(
    cover.COVER_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(BhgtaCover),
            # Connect to other components
            cv.Required(CONF_PWM_IN): cv.use_id(SmoothPWM),
            cv.Required(CONF_PWM_OUT): cv.use_id(SmoothPWM),
            cv.Required(CONF_POT): cv.use_id(ADCSensor),
            # Calibration data
            cv.Required(CONF_ALMOST): cv.float_,
            cv.Required(CONF_ON_FORCE_STOP): automation.validate_automation(
                {cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(CoverForceStopTrigger)}
            ),
        }
    ).extend(cv.COMPONENT_SCHEMA),
    cv.require_framework_version(esp8266_arduino=cv.Version(2, 4, 0)),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cover.register_cover(var, config)

    pwm_in = await cg.get_variable(config[CONF_PWM_IN])
    cg.add(var.set_pwm_in(pwm_in))

    pwm_out = await cg.get_variable(config[CONF_PWM_OUT])
    cg.add(var.set_pwm_out(pwm_out))

    pot = await cg.get_variable(config[CONF_POT])
    cg.add(var.set_pot(pot))

    cg.add(var.set_almost(config[CONF_ALMOST]))

    for conf in config.get(CONF_ON_FORCE_STOP, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], conf)
