import esphome.codegen as cg
from esphome.components import pace_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = ["pace_modbus", "binary_sensor", "sensor", "text_sensor"]
CODEOWNERS = ["@SaschaKP"]
MULTI_CONF = True

CONF_PACE_BMS_ID = "pace_bms_id"
CONF_OVERRIDE_CELL_COUNT = "override_cell_count"

DEFAULT_PROTOCOL_VERSION = 0x25
DEFAULT_ADDRESS = 0x01

pace_bms_ns = cg.esphome_ns.namespace("pace_bms")
PaceBms = pace_bms_ns.class_(
    "PaceBms", cg.PollingComponent, pace_modbus.PaceModbusDevice
)

PACE_BMS_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_PACE_BMS_ID): cv.use_id(PaceBms),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PaceBms),
            cv.Optional(CONF_OVERRIDE_CELL_COUNT, default=0): cv.int_range(
                min=0, max=16
            ),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(
        pace_modbus.pace_modbus_device_schema(
            DEFAULT_PROTOCOL_VERSION, DEFAULT_ADDRESS
        )
    )
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await pace_modbus.register_pace_modbus_device(var, config)

    cg.add(var.set_override_cell_count(config[CONF_OVERRIDE_CELL_COUNT]))
