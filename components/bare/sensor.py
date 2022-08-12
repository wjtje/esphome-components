from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_VALUE, STATE_CLASS_NONE

bare_ns = cg.esphome_ns.namespace("bare")

BareSensor = bare_ns.class_("BareSensor", sensor.Sensor, cg.Component)
BareSensorSetValueAction = bare_ns.class_("BareSensorSetValueAction", automation.Action)

CONFIG_SCHEMA = sensor.sensor_schema(
    accuracy_decimals=0, state_class=STATE_CLASS_NONE
).extend({cv.GenerateID(): cv.declare_id(BareSensor)})


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)


@automation.register_action(
    "sensor.bare.set_value",
    BareSensorSetValueAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(sensor.Sensor),
            cv.Required(CONF_VALUE): cv.templatable(cv.int_),
        }
    ),
)
async def sensor_template_publish_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_VALUE], args, int)
    cg.add(var.set_value(template_))
    return var
