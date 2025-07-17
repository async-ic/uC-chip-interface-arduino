# This file is part of the Firmware project to interface with small Async or Neuromorphic chips
# Copyright (C) 2022-2023 Ole Richter - University of Groningen
# Copyright (C) 2024 Vincent Jassies - University of Groningen
#
# Last Update: 2025/03/04 - Vincent Jassies
# Added header for recurrency mapping table (SAVE_MAPPING_TABLE = 99)
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program.
# If not, see <https://www.gnu.org/licenses/>.

import enum

# The packet header identifies the instruction to be executed by the microcontroller.
# The headers are divided into sections for:
#   - General commands
#   - Interface-specific commands (PIN, AER, SPI, I2C, etc.)
#   - Configuration commands
#
# For simplicity, commands use the Data32bitPacket class with an empty value field.


@enum.unique
class Data32bitHeader(enum.IntEnum):
    """Headers for Data32bitPacket commands. Each element includes a docstring explaining its function."""

    def __new__(cls, value, doc=None):
        # Enable per-enum docstrings by accepting a second argument.
        self = int.__new__(cls, value)
        self._value_ = value
        if doc is not None:
            self.__doc__ = doc
        return self

    UC_CLOSE_CONNECTION = 255, """
    Instructs the PC buffer to close the connection to the microcontroller.
    """
    IN_RESET = 254, """
    Software reset of the microcontroller (clears configuration without a hardware reset).
    """
    IN_READ = 0, """
    Request to read all available output packets from the output buffer and clear it.
    (The value field is ignored.)
    """
    IN_SET_TIME = 1, """
    Sets the execution time. A value of 0 halts recording and execution; a value ≥ 1 starts execution from that timestep.
    Maximum experiment time is 2^32 microseconds.
    """
    IN_READ_TIME = 2, """
    Requests the current execution time to be sent to the output buffer.
    (The value field is ignored.)
    """
    IN_READ_INSTRUCTIONS = 3, """
    Requests to read all available input packets (unprocessed instructions) from the input buffer.
    (The value field is ignored.)
    """
    IN_READ_LAST = 4, """
    Reads only the last packet without removing it from the ring buffer.
    (The value field is ignored.)
    """
    IN_FREE_INSTRUCTION_SPOTS = 5, """
    Requests the number of free spots in the instruction ring buffer.
    """
    IN_CONF_READ_ON_REQUEST = 6, """
    Legacy mode: disables automatic sending of the output buffer.
      - Value 1: read on request (legacy)
      - Value 0: continuous read (default)
    """
    OUT_TIME = 100, """
    Response to a READ_TIME request. Returns:
      - exec_time: the current runtime
      - value: the system time (without offset)
    """
    IN_SPI0 = 20, """
    Sends a 32-bit word over the SPI0 interface.
    """
    IN_SPI1 = 21, """
    Sends a 32-bit word over the SPI1 interface.
    """
    IN_SPI2 = 22, """
    Sends a 32-bit word over the SPI2 interface.
    """
    OUT_SPI0 = 120, """
    Response to the SPI0 command.
      - Returns a 32-bit word read from the SPI, along with the current runtime.
    """
    OUT_SPI1 = 121, """
    Response to the SPI1 command.
    """
    OUT_SPI2 = 122, """
    Response to the SPI2 command.
    """
    IN_ASYNC_TO_CHIP0 = 30, """
    Sends a 0–32-bit word on the ASYNC_TO_CHIP0 interface.
      - If the configured width is >32, the most-significant bits are ignored.
    """
    IN_ASYNC_TO_CHIP1 = 31, """
    Sends a 0–32-bit word on the ASYNC_TO_CHIP1 interface.
    """
    IN_ASYNC_TO_CHIP2 = 32, """
    Sends a 0–32-bit word on the ASYNC_TO_CHIP2 interface.
    """
    IN_ASYNC_TO_CHIP3 = 33, """
    Sends a 0–32-bit word on the ASYNC_TO_CHIP3 interface.
    """
    IN_ASYNC_TO_CHIP4 = 34, """
    Sends a 0–32-bit word on the ASYNC_TO_CHIP4 interface.
    """
    IN_ASYNC_TO_CHIP5 = 35, """
    Sends a 0–32-bit word on the ASYNC_TO_CHIP5 interface.
    """
    IN_ASYNC_TO_CHIP6 = 36, """
    Sends a 0–32-bit word on the ASYNC_TO_CHIP6 interface.
    """
    IN_ASYNC_TO_CHIP7 = 37, """
    Sends a 0–32-bit word on the ASYNC_TO_CHIP7 interface.
    """
    OUT_ASYNC_FROM_CHIP0 = 130, """
    Event from ASYNC_FROM_CHIP0:
      - Returns a 0–32-bit word and the current runtime.
    """
    OUT_ASYNC_FROM_CHIP1 = 131, """
    Event from ASYNC_FROM_CHIP1.
    """
    OUT_ASYNC_FROM_CHIP2 = 132, """
    Event from ASYNC_FROM_CHIP2.
    """
    OUT_ASYNC_FROM_CHIP3 = 133, """
    Event from ASYNC_FROM_CHIP3.
    """
    OUT_ASYNC_FROM_CHIP4 = 134, """
    Event from ASYNC_FROM_CHIP4.
    """
    OUT_ASYNC_FROM_CHIP5 = 135, """
    Event from ASYNC_FROM_CHIP5.
    """
    OUT_ASYNC_FROM_CHIP6 = 136, """
    Event from ASYNC_FROM_CHIP6.
    """
    OUT_ASYNC_FROM_CHIP7 = 137, """
    Event from ASYNC_FROM_CHIP7.
    """
    IN_MAPPER_KEY = 190, """
    Switches into sequence transmission mode: the next packet is treated as a key, and subsequent packets as values,
    until IN_MAPPER_END is received.
    """
    IN_MAPPER_END = 191, """
    Switches back to normal packet execution mode.
    """
    OUT_BUFFER_LAST_READ = 250, """
    Used to distinguish between an empty and a full buffer.
      - Value should be 1 (ignored).
    """
    OUT_FREE_INSTRUCTION_SPOTS = 101, """
    Reports the number of free spots in the instruction ring buffer.
    """


@enum.unique
class PinHeader(enum.IntEnum):
    """Headers for pin control packets."""

    def __new__(cls, value, doc=None):
        self = int.__new__(cls, value)
        self._value_ = value
        if doc is not None:
            self.__doc__ = doc
        return self

    IN_PIN = 10, """
    Command to modify an output pin.
      - 'pin' specifies the pin number.
      - 'value' is 0 (low) or 1 (high).
    """
    IN_PIN_READ = 11, """
    Command to read an input pin.
      - 'pin' specifies the pin number.
      - 'value' is ignored.
    """
    OUT_PIN_LOW = 110, """
    Indicates a pin change (low).
      - Provides the time of change, pin ID, and new state.
    """
    OUT_PIN_HIGH = 111, """
    Indicates a pin change (high).
      - Provides the time of change, pin ID, and new state.
    """


@enum.unique
class DataI2CHeader(enum.IntEnum):
    """Headers for I2C data packets."""

    def __new__(cls, value, doc=None):
        self = int.__new__(cls, value)
        self._value_ = value
        if doc is not None:
            self.__doc__ = doc
        return self

    IN_I2C0 = 25, """
    Sends an 8- or 16-bit word or requests a specified number of bytes on the I2C interface.
      - 'device_address' is the 7-bit address (MSB) with the LSB indicating Read (1) or Write (0).
      - 'register_address' is the 8-bit register address.
      - 'value_ms' is the most-significant 8 bits.
      - 'value_ls' is the least-significant 8 bits or the number of bytes to read.
    """
    IN_I2C1 = 26, """
    Similar to IN_I2C0, but for a different I2C interface.
    """
    IN_I2C2 = 27, """
    Similar to IN_I2C0, but for a different I2C interface.
    """
    OUT_I2C0 = 125, """
    Response to IN_I2C0.
      - Returns an 8- or 16-bit word read from the I2C along with the current runtime.
    """
    OUT_I2C1 = 126, """
    Response to IN_I2C1.
    """
    OUT_I2C2 = 127, """
    Response to IN_I2C2.
    """


@enum.unique
class ConfigMainHeader(enum.IntEnum):
    """Main headers for configuration packets, used in conjunction with ConfigSubHeader."""

    def __new__(cls, value, doc=None):
        self = int.__new__(cls, value)
        self._value_ = value
        if doc is not None:
            self.__doc__ = doc
        return self

    IN_CONF_READ_ON_REQUEST = 6, """
    Legacy mode to disable automatic output buffer sending.
      - Value 1 for read on request.
      - Value 0 for continuous read.
    """
    IN_CONF_PIN = 50, """
    Configures a pin.
      - Uses a sub-header to specify the configuration state.
      - 'value' is the pin ID.
    """
    IN_CONF_SPI0 = 60, """
    Configures SPI0.
      - Uses a sub-header to specify the configuration state.
      - 'value' is interpreted per sub-header.
    """
    IN_CONF_SPI1 = 61, """
    Configures SPI1.
    """
    IN_CONF_SPI2 = 62, """
    Configures SPI2.
    """
    IN_CONF_I2C0 = 65, """
    Configures I2C0.
    """
    IN_CONF_I2C1 = 66, """
    Configures I2C1.
    """
    IN_CONF_I2C2 = 67, """
    Configures I2C2.
    """
    IN_CONF_ASYNC_TO_CHIP0 = 70, """
    Configures ASYNC_TO_CHIP0.
    """
    IN_CONF_ASYNC_TO_CHIP1 = 71, """
    Configures ASYNC_TO_CHIP1.
    """
    IN_CONF_ASYNC_TO_CHIP2 = 72, """
    Configures ASYNC_TO_CHIP2.
    """
    IN_CONF_ASYNC_TO_CHIP3 = 73, """
    Configures ASYNC_TO_CHIP3.
    """
    IN_CONF_ASYNC_TO_CHIP4 = 74, """
    Configures ASYNC_TO_CHIP4.
    """
    IN_CONF_ASYNC_TO_CHIP5 = 75, """
    Configures ASYNC_TO_CHIP5.
    """
    IN_CONF_ASYNC_TO_CHIP6 = 76, """
    Configures ASYNC_TO_CHIP6.
    """
    IN_CONF_ASYNC_TO_CHIP7 = 77, """
    Configures ASYNC_TO_CHIP7.
    """
    IN_CONF_ASYNC_FROM_CHIP0 = 80, """
    Configures ASYNC_FROM_CHIP0.
    """
    IN_CONF_ASYNC_FROM_CHIP1 = 81, """
    Configures ASYNC_FROM_CHIP1.
    """
    IN_CONF_ASYNC_FROM_CHIP2 = 82, """
    Configures ASYNC_FROM_CHIP2.
    """
    IN_CONF_ASYNC_FROM_CHIP3 = 83, """
    Configures ASYNC_FROM_CHIP3.
    """
    IN_CONF_ASYNC_FROM_CHIP4 = 84, """
    Configures ASYNC_FROM_CHIP4.
    """
    IN_CONF_ASYNC_FROM_CHIP5 = 85, """
    Configures ASYNC_FROM_CHIP5.
    """
    IN_CONF_ASYNC_FROM_CHIP6 = 86, """
    Configures ASYNC_FROM_CHIP6.
    """
    IN_CONF_ASYNC_FROM_CHIP7 = 87, """
    Configures ASYNC_FROM_CHIP7.
    """

@enum.unique
class ErrorHeader(enum.IntEnum):
    """
    Headers for error packets issued by the microcontroller.
    These indicate what went wrong and include additional context in the packet.
    """

    def __new__(cls, value, doc=None):
        self = int.__new__(cls, value)
        self._value_ = value
        if doc is not None:
            self.__doc__ = doc
        return self

    OUT_ERROR = 200, """
    Unspecified error: something went wrong without a specific cause.
    """
    OUT_ERROR_PIN_ALREADY_INUSE = 201, """
    The requested pin or interface is already in use.
    To reset, close the connection via the API and reconnect after the microcontroller resets.
    """
    OUT_ERROR_PIN_NOT_CONFIGURED = 202, """
    The requested pin is not configured. Initialize it first using the appropriate API or configuration packets.
    """
    OUT_ERROR_INPUT_FULL = 203, """
    The instruction buffer is full; new instructions are discarded.
    """
    OUT_ERROR_OUTPUT_FULL = 204, """
    The output buffer is full; packets are being dropped.
    The value indicates the number of dropped packets.
    """
    OUT_ERROR_INTERFACE_ALREADY_ACTIVE = 205, """
    The interface is already active. To reconfigure, close the connection and reconnect after reset.
    """
    OUT_ERROR_UNKNOWN_INSTRUCTION = 206, """
    Unrecognized instruction header, possibly due to an API/firmware version mismatch.
    """
    OUT_ERROR_INTERFACE_NOT_ACTIVE = 207, """
    The requested interface is not configured. Initialize it first.
    """
    OUT_ERROR_UNKNOWN_CONFIGURATION = 208, """
    Unrecognized configuration header, possibly due to an API/firmware version mismatch.
    """
    OUT_ERROR_ASYNC_HS_TIMEOUT = 209, """
    The asynchronous interface did not receive an acknowledgment in time. Please restart or reset the device.
    """
    OUT_ERROR_PERIPHERAL_INTERFACE_NOT_READY = 210, """
    A peripheral interface (e.g., I2C for the MCP23017) is not ready.
    """
    OUT_ERROR_CONFIGURATION_OUT_OF_BOUNDS = 211, """
    The configuration ID exceeds available microcontroller resources.
    """
    OUT_ERROR_DATA_OUT_OF_BOUNDS = 212, """
    The provided data exceeds the configured bit/byte width of the interface.
    """
    OUT_WARNING_DATA_COLLECTION_SQUEUED = 213, """
    Data collection is delayed due to excessive requests. The microcontroller will pause collection briefly
    to transmit pending packets.
    """
    OUT_ALIGN_SUCCESS_VERSION = 253, """
    Confirmation response to an alignment request.
      - org_header: major version (8-bit)
      - sub_header: minor version (8-bit)
      - value: patch version (32-bit)
    """


class ConfigSubHeader(enum.IntEnum):
    """
    Sub-headers for configuration packets.
    These specify the property of an interface to be configured.
    """

    def __new__(cls, value, doc=None):
        self = int.__new__(cls, value)
        self._value_ = value
        if doc is not None:
            self.__doc__ = doc
        return self

    CONF_ACTIVE = 60, """
    Activates an interface; once active, pins/width cannot be changed.
    (Deactivation is not implemented.)
    """
    CONF_OUTPUT = 61, """
    Configures a pin as output.
      - 'value' is the pin ID.
    """
    CONF_INPUT = 62, """
    Configures a pin as input, enabling interrupts to record changes.
      - 'value' is the pin ID.
    """
    CONF_REQ = 70, """
    Sets the Request (Req) pin for an AER interface.
      - 'value' is the pin ID.
    """
    CONF_ACK = 71, """
    Sets the Acknowledgment (Ack) pin for an AER interface.
      - 'value' is the pin ID.
    """
    CONF_WIDTH = 72, """
    Sets the bit width for an AER interface (0–32).
      - 'value' is the width.
    """
    CONF_REQ_DELAY = 73, """
    Sets the delay on the Request line (in multiples of 20 ns).
    """
    CONF_BYTE_ORDER = 74, """
    Sets the byte order for data transmission.
      - 0 for LSB-first, 1 for MSB-first (default is 0).
    """
    CONF_SPEED_CLASS = 75, """
    Specifies the interface speed class (refer to interface documentation).
    """
    CONF_TYPE = 76, """
    Specifies the interface type (refer to interface documentation; default is 0).
    """
    
    CONF_NONE = 255, "Indicates no sub-category."
    # Asynchronous data channel pin settings:
    CONF_CHANNEL0 = 0, "Sets the pin for Async data channel 0."
    CONF_CHANNEL1 = 1, "Sets the pin for Async data channel 1."
    CONF_CHANNEL2 = 2, "Sets the pin for Async data channel 2."
    CONF_CHANNEL3 = 3, "Sets the pin for Async data channel 3."
    CONF_CHANNEL4 = 4, "Sets the pin for Async data channel 4."
    CONF_CHANNEL5 = 5, "Sets the pin for Async data channel 5."
    CONF_CHANNEL6 = 6, "Sets the pin for Async data channel 6."
    CONF_CHANNEL7 = 7, "Sets the pin for Async data channel 7."
    CONF_CHANNEL8 = 8, "Sets the pin for Async data channel 8."
    CONF_CHANNEL9 = 9, "Sets the pin for Async data channel 9."
    CONF_CHANNEL10 = 10, "Sets the pin for Async data channel 10."
    CONF_CHANNEL11 = 11, "Sets the pin for Async data channel 11."
    CONF_CHANNEL12 = 12, "Sets the pin for Async data channel 12."
    CONF_CHANNEL13 = 13, "Sets the pin for Async data channel 13."
    CONF_CHANNEL14 = 14, "Sets the pin for Async data channel 14."
    CONF_CHANNEL15 = 15, "Sets the pin for Async data channel 15."
    CONF_CHANNEL16 = 16, "Sets the pin for Async data channel 16."
    CONF_CHANNEL17 = 17, "Sets the pin for Async data channel 17."
    CONF_CHANNEL18 = 18, "Sets the pin for Async data channel 18."
    CONF_CHANNEL19 = 19, "Sets the pin for Async data channel 19."
    CONF_CHANNEL20 = 20, "Sets the pin for Async data channel 20."
    CONF_CHANNEL21 = 21, "Sets the pin for Async data channel 21."
    CONF_CHANNEL22 = 22, "Sets the pin for Async data channel 22."
    CONF_CHANNEL23 = 23, "Sets the pin for Async data channel 23."
    CONF_CHANNEL24 = 24, "Sets the pin for Async data channel 24."
    CONF_CHANNEL25 = 25, "Sets the pin for Async data channel 25."
    CONF_CHANNEL26 = 26, "Sets the pin for Async data channel 26."
    CONF_CHANNEL27 = 27, "Sets the pin for Async data channel 27."
    CONF_CHANNEL28 = 28, "Sets the pin for Async data channel 28."
    CONF_CHANNEL29 = 29, "Sets the pin for Async data channel 29."
    CONF_CHANNEL30 = 30, "Sets the pin for Async data channel 30."
    CONF_CHANNEL31 = 31, "Sets the pin for Async data channel 31."


# Alignment byte array for synchronizing communication between the PC and microcontroller.
ALIGN_BYTEARRAY = b'\xff\xff\xff\xff\xff\xff\xff\xff\xff\xfd\x00\x00\x00\x00\x00\x00\x00\x00'

# Lists of headers to log on arrival (errors are logged by default)
LOGGING_WARNING_LIST = [Data32bitHeader.IN_RESET]
LOGGING_INFO_LIST = []


def subscribe_warning(header):
    """
    Add a header to the list of logged warnings.
    """
    LOGGING_WARNING_LIST.append(header)


def unsubscribe_warning(header):
    """
    Remove a header from the list of logged warnings.
    """
    LOGGING_WARNING_LIST.remove(header)


def subscribe_info(header):
    """
    Add a header to the list of logged informational messages.
    """
    LOGGING_INFO_LIST.append(header)


def unsubscribe_info(header):
    """
    Remove a header from the list of logged informational messages.
    """
    LOGGING_INFO_LIST.remove(header)
