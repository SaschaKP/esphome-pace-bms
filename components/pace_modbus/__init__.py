from esphome import pins
import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_FLOW_CONTROL_PIN, CONF_ID
from esphome.cpp_helpers import gpio_pin_expression

DEPENDENCIES = ["uart"]
MULTI_CONF = True

CONF_PACE_MODBUS_ID = "pace_modbus_id"
CONF_RX_TIMEOUT = "rx_timeout"
CONF_PROTOCOL_VERSION = "protocol_version"
CONF_OVERRIDE_PACK = "override_pack"

pace_modbus_ns = cg.esphome_ns.namespace("pace_modbus")
PaceModbus = pace_modbus_ns.class_("PaceModbus", cg.Component, uart.UARTDevice)
PaceModbusDevice = pace_modbus_ns.class_("PaceModbusDevice")

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(PaceModbus),
            cv.Optional(
                CONF_RX_TIMEOUT, default="500ms"
            ): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    cg.add_global(pace_modbus_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))
    if CONF_FLOW_CONTROL_PIN in config:
        pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
        cg.add(var.set_flow_control_pin(pin))


def pace_modbus_device_schema(default_protocol_version, default_address):
    schema = {
        cv.GenerateID(CONF_PACE_MODBUS_ID): cv.use_id(PaceModbus),
        cv.Optional(CONF_ADDRESS, default=default_address): cv.hex_uint8_t,
        cv.Optional(
            CONF_PROTOCOL_VERSION, default=default_protocol_version
        ): cv.hex_uint8_t,
        cv.Optional(CONF_OVERRIDE_PACK): cv.hex_uint8_t,
    }
    return cv.Schema(schema)


async def register_pace_modbus_device(var, config):
    parent = await cg.get_variable(config[CONF_PACE_MODBUS_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
    cg.add(parent.register_device(var))

    if CONF_OVERRIDE_PACK in config:
        cg.add(var.set_pack(config[CONF_OVERRIDE_PACK]))
    else:
        cg.add(var.set_pack(config[CONF_ADDRESS]))
