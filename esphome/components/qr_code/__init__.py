import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_VALUE

CODEOWNERS = ["@wjtje"]

DEPENDENCIES = ["display"]
MULTI_CONF = True

qr_code_ns = cg.esphome_ns.namespace("qr_code")
QRCode = qr_code_ns.class_("QRCode", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.declare_id(QRCode),
        cv.Required(CONF_VALUE): cv.string,
    }
)


async def to_code(config):
    cg.add_library("https://github.com/wjtje/QR-Code-generator-esphome", "1.7.0")

    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_value(config[CONF_VALUE]))
    await cg.register_component(var, config)

    cg.add_define("USE_QR_CODE")
