
#    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
#    Copyright (C) 2022-2023 Ole Richter - University of Groningen
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.


import logging
import struct
from .header import *


class Packet:
    """ 
    Packet class to be used as a base class for all packets acting as an interface.
    it contains the static method from_bytearray to construct a packet from a bytearray
    and chooses the correct packet type depending on the header
    """
    def __init__(self):
        self._header = 0
        self._exec_time = 0
        self.check_and_log()
    
    @classmethod
    def from_bytearray(cls, byte_array):
        """ method to construct a packet from a bytearray
            depending on the header the correct packet type is returned
            @param byte_array: (bytearray) the bytearray to construct the packet from
            @return: the constructed packet of the coresponing sub type
        """
        try:
            unpacked = struct.unpack("<BII", byte_array)
            # from pyhton 3.12 the in operator can be used to do the same check
            # until then:
            try:
                header = Data32bitHeader(unpacked[0]);
                logging.debug("detected: Data32bit : "+ str(header))
                return Data32bitPacket.from_bytearray(byte_array)
            except ValueError:
                try:
                    header = DataI2CHeader(unpacked[0]);
                    logging.debug("detected: DataI2C : "+ str(header))
                    return DataI2CPacket.from_bytearray(byte_array)
                except ValueError:
                    try:
                        header = PinHeader(unpacked[0]);
                        logging.debug("detected: Pin : "+ str(header))
                        return PinPacket.from_bytearray(byte_array)
                    except ValueError:
                        try:
                            header = ConfigMainHeader(unpacked[0]);
                            logging.debug("detected: Config Main : "+ str(header))
                            return ConfigPacket.from_bytearray(byte_array)
                        except ValueError:
                            try:
                                header = ErrorHeader(unpacked[0]);
                                logging.debug("detected: Error : "+ str(header))
                                return ErrorPacket.from_bytearray(byte_array)
                            except ValueError:
                                logging.error("Header "+ str(unpacked[0]) +" is not a valid header, Packet:" + str(byte_array) +" "+ str(unpacked[0]) +" "+ str(unpacked[1]) +" "+ str(unpacked[2]))
                                raise ValueError("Header "+ str(unpacked[0]) +" is not a valid header")
        except Exception as e:
            logging.error(e)
    def header(self):
        """getter method for the header attribute

        :return: the header of the packet
        :rtype: EnumInt entry or int
        """
        return self._header
    def time(self):
        """getter method for the time attribute

        :return: the time of execution of the packet
        :rtype: EnumInt entry or int
        """
        return self._exec_time

    def set_header(self, header):
        """setter method for the header attribute
        @param header: (byte/uint_8) the header of the packet
        """
        if (header < 256 and header >= 0 and isinstance(header, int)):
            self._header=header
        else:
            logging.error("header "+str(header)+" is not a valid header")
    
    def set_exec_time(self, time):
        """setter method for the execution time attribute
        @param time: (uint_32) the time of execution of the packet
        """
        if (time < 2**32 and time >= 0 and isinstance(time, int)):
            self._exec_time=time
        else:
            logging.error("exec_time "+str(time)+" is not a valid unsigned integer of 4 bytes")

    def to_bytearray(self):
        """ placeholder for converting the package to a bytearray
        @return: the packet as a bytearray
        """
        logging.warning("the method to_bytearray needs to be implemented by the child class")

    def check_and_log(self):
        """check_and_log 
        logs the packet if the header is in the logging lists
        """
        if self._header in LOGGING_WARNING_LIST:
            logging.warning(str(self))
        if self._header in LOGGING_INFO_LIST:
            logging.info(str(self))
        #if self._header in LOGGING_ERROR_LIST:
        #    logging.error(str(self))

class Data32bitPacket(Packet): 
    """ The Data32bitPacket is used to send 32bit data instructions to the uC
        all availible instructions are defined in the Data32bitHeader  
    """

    def __init__(self, header, value=0, time=0):
        """ constructor for the Data32bitPacket
        @param header: (Data32bitHeader) the header of the packet
        @param value: (uint_32) the value to be sent (optional, default = 0)
        @param time: (uint_32) the time of execution of the packet (optional, default = 0)
        """
        self.set_header(header)
        self.set_value(value)
        self.set_exec_time(time)
        self.check_and_log()

    def value(self):
        """ getter method for the value attribute
            @return: the value of the packet
        """
        return self._value

    def set_header(self, header):
        if (header in Data32bitHeader):
            self._header=header
        else:
            logging.error("header "+str(header)+" is not a valid header")

    def set_value(self, value):
        if (value < 2**32 and value >= 0 and isinstance(value, int)):
            self._value=value
        else:
            logging.error("value "+str(value)+" is not a valid unsigned integer of 4 bytes")

    def to_bytearray(self):
        return struct.pack("<BII", self._header, self._exec_time, self._value)
    
    def __str__(self):
        return "[Packet]: data 32bit: header = "+ str(self._header) +", value = "+ str(self._value) + ", at time = "+ str(self._exec_time) +"us"

    @classmethod
    def from_bytearray(self, byte_array):
        """
        constructs a Data32bitPacket from a bytearray
        @param byte_array: 9 byte bytearray to construct the Packet from
        @return: Data32bitPacket
        @raise Exception: if package construction fails
        """
        unpacked = struct.unpack("<BII", byte_array)
        return Data32bitPacket(header=Data32bitHeader(unpacked[0]),
        value = unpacked[2],
        time = unpacked[1])

    
class DataI2CPacket(Packet): 
    """ The DataI2CPacket is used for I2C communication 
        and all types which need 3x 8bit values instread of 1x 32bit value
        all availible instructions are defined in the DataI2CHeader
    """ 
    def __init__(self, header, device_address, register_address, read=False, value=0, time=0):
        self.set_header(header)
        self.set_value(value)
        self.set_exec_time(time)
        self.set_device_address(device_address)
        self.set_register_address(register_address)
        self.set_read(read)
        self.check_and_log()

    def value(self):
        """ getter method for the value attribute
            @return: the value of the packet
        """
        return self._value_ls+ self._value_ms<<8

    def read(self):
        """ getter method for the 1bit read/write attribute
            @return: the read flag of the i2c packet
        """
        return self._read
    
    def device_address(self):
        """ getter method for the 7bit device_address attribute
            @return: the device_address of the i2c packet
        """
        return self._device_address

    def register_address(self):
        """ getter method for the 8bit register_address attribute
            @return: the register_address of the i2c packet
        """
        return self._register_address

    def set_header(self, header):
        """ setter method for the header attribute
            @param header: (DataI2CHeader or uint8) the header of the packet
        """
        if (header in DataI2CHeader):
            self._header=header
        else:
            logging.error("header "+str(header)+" is not a valid header")

    def set_value(self, value):
        """ setter method for the value attribute
            @param value: (uint_16) the value to be sent
        """
        if (value < 2**16 and value >= 0 and isinstance(value, int)):
            self._value_ls = value & 0xF
            self._value_ms= value >> 8
        else:
            logging.error("value "+str(value)+" is not a valid unsigned integer of 2 byte")

    def set_device_address(self, value):
        """ setter method for the 7bit device address attribute
            @param value: (uint_8) the 7bit device address to be sent to
        """
        if (value < 2**7 and value >= 0 and isinstance(value, int)):
            self._device_address = value
        else:
            logging.error("device_address "+str(value)+" is not a valid unsigned integer of 7 bit")

    def set_read(self, value):
        """ setter method for the 1bit read/write attribute
            @param value: (bool) the read flag of the i2c packet
        """
        if value:
                self._read = 1
        else:
                self._read = 0
    def set_register_address(self, value):
        """ setter method for the 8bit register address attribute
            @param value: (uint_8) the 8bit register address to be sent to
        """
        if (value < 2**8 and value >= 0 and isinstance(value, int)):
            self._register_address = value
        else:
            logging.error("register_address "+str(value)+" is not a valid unsigned integer of 8 bit")
    def to_bytearray(self):
        """ method to convert the packet to a bytearray in the correct format of <header><exec_time><addess+read><value ms/ls>
            @return: the packet as a bytearray
        """
        return struct.pack("<BIBBBB", self._header, self._exec_time, self._device_address<<1+self._read ,self._register_address,self._value_ms,self._value_ls)
    
    def __str__(self):
        return "[Packet]: data i2c: header = "+ str(self._header) +",device_address = "+ str(self._device_address) +",register address = "+ str(self._register_address) +",read = "+ str(self._read) +", value = "+ str(self._value_ls+(self._value_ms<<8)) + ", at time = "+ str(self._exec_time) +"us"
    
    @classmethod
    def from_bytearray(self, byte_array):
        """
        constructs a DataI2CPacket from a bytearray
        @param byte_array:  9 byte bytearray to construct the Packet from
        @return: DataI2CPacket
        @raise Exception: if package construction fails
        """
        unpacked = struct.unpack("<BIBBBB", byte_array)
        print(unpacked)
        print((unpacked[4] << 8) | unpacked[5])
        return DataI2CPacket(header=DataI2CHeader(unpacked[0]),
        device_address=unpacked[2]>>1,
        register_address=unpacked[3], read=unpacked[2] & 0x1, value=((unpacked[4] << 8) | unpacked[5]),
        time = unpacked[1])


"""
value: (int) 0 to set pin LOW, 1 to set pin HIGH.
"""
class PinPacket(Packet): 
    """ The PinPacket is used to comunicate pin instructions with the uC
        all availible instructions are defined in the PinHeader
    """
    def __init__(self, header, pin_id, value=0, time=0):
        """ constructor for the PinPacket
        @param header: (PinHeader or UINT8) the header of the packet
        @param pin_id: (uint_8) the pin id to be adressed
        @param value: (uint_8) the value to be sent (optional, default = 0) -  0 to set pin LOW, (>=) 1 to set pin HIGH.
        @param time: (uint_32) the time of execution of the packet (optional, default = 0)
        """
        self.set_header(header)
        self.set_pin_id(pin_id)
        self.set_value(value)
        self.set_exec_time(time)
        self.check_and_log()
    
    def pin_id(self):
        """ getter method for the pin_id attribute
            @return: the pin_id of the pin
        """
        return self._pin_id

    def value(self):
        """ getter method for the value attribute
            @return: the value of the pin
        """
        return self._value

    def set_header(self, header):
        """ setter method for the header attribute
            @param header: (PinHeader or uint8) the header of the packet
        """
        if (header in PinHeader):
            self._header=header
        else:
            logging.error("header "+str(header)+" is not a valid header")

    def set_pin_id(self, pin_id):
        """ setter method for the pin_id attribute
            @param pin_id: (uint_8) the pin id to be adressed
        """
        if (pin_id < 55 and pin_id >= 0 and isinstance(pin_id, int)):
            self._pin_id=pin_id
        else:
            logging.error("di "+str(pin_id)+" is not a valid unsigned integer of 1 byte + id < 55")


    def set_value(self, value):
        """ setter method for the pin value attribute
            @param value: (uint_8) the value 
        """
        if (value < 2**8 and value >= 0 and isinstance(value, int)):
            self._value=value
        else:
            logging.error("value "+str(value)+" is not a valid unsigned integer of 1 byte")

    def to_bytearray(self):
        """ method to convert the packet to a bytearray in the correct format of <header><exec_time><pin_id><value>
            @return: the packet as a bytearray
        """
        return struct.pack("<BIBBBB", self._header, self._exec_time, self._pin_id, self._value,0,0)
    
    def __str__(self):
        return "[Packet]: pin: header = "+ str(self._header) +", pin_id = "+ str(self._pin_id) + ", value = "+ str(self._value) + ", at time = "+ str(self._exec_time) +"us"

    @classmethod
    def from_bytearray(self, byte_array):
        """
        constructs a PinPacket from a bytearray
        @param byte_array: 9 byte bytearray to construct the Packet from
        @return: PinPacket
        @raise Exception: if package construction fails
        """
        unpacked = struct.unpack("<BIBBBB", byte_array)
        return PinPacket(header=PinHeader(unpacked[0]),
        pin_id = unpacked[2],
        value = unpacked[3],
        time = unpacked[1])


"""
header: (ConfigMainHeader) what's to be configured (e.g. PIN, SPI,...).
config_header: (ConfigSubHeader) configuration properties.
"""
class ConfigPacket(Packet): 
    """ The ConfigPacket is used to cumunicate configuration instructions with the uC
        all availible instructions are defined in the ConfigMainHeader and ConfigSubHeader
    """
    def __init__(self, header, config_header, value=0, time=0):
        self.set_header(header)
        self.set_config_header(config_header)
        self.set_value(value)
        self.set_exec_time(time)
        self.check_and_log()

    def value(self):
        """ getter method for the value attribute
            @return: the value
        """
        return self._value

    def config_header(self):
        """ getter method for the sub instruction config_header attribute
            @return: the config_sub_header
        """
        return self._config_header

    def set_header(self, header):
        """ setter method for the header attribute
            @param header: (ConfigMainHeader or uint8) the header of the packet
        """
        if (header in ConfigMainHeader):
            self._header=header
        else:
            logging.error("header "+str(header)+" is not a valid header")
            
    def set_config_header(self, config_header):
        """ setter method for the sub instruction config_header attribute
            @param config_header: (ConfigSubHeader or uint8) the config_sub_header
        """
        if (config_header in ConfigSubHeader):
            self._config_header=config_header
        else:
            logging.error("config sub header "+str(header)+" is not a valid header")

    def set_value(self, value):
        """ setter method for the value attribute
            @param value: (uint_8) the value
        """
        if (value < 2**8 and value >= 0 and isinstance(value, int)):
            self._value=value
        else:
            logging.error("value "+str(value)+" is not a valid unsigned integer of 1 byte")

    def to_bytearray(self):
        """ method to convert the packet to a bytearray in the correct format of <header><exec_time><config_header><value>
            @return: the packet as a bytearray
        """
        return struct.pack("<BIBBBB", self._header, self._exec_time, self._config_header ,self._value,0,0)
    
    def __str__(self):
        return "[Packet]: config: header = "+ str(self._header) +", config = "+ str(self._config_header) + ", value = "+ str(self._value) + ", at time = "+ str(self._exec_time) +"us"

    @classmethod
    def from_bytearray(self, byte_array):
        """
        constructs a ConfigPacket from a bytearray
        @param byte_array: 9 byte bytearray to construct the Packet from
        @return: ConfigPacket
        @raise Exception: if package construction fails
        """
        unpacked = struct.unpack("<BIBBBB", byte_array)
        return ConfigPacket(header=ConfigMainHeader(unpacked[0]),
        config_header = ConfigSubHeader(unpacked[2]),
        value = unpacked[3],
        time = unpacked[1])



class ErrorPacket(Packet): 
    """ The ErrorPacket is used by the uC to send errors to the API
        all availible errors are defined in the ErrorHeader
    """
    def __init__(self, header, original_header, value=0, original_sub_header=0, skip_header_matching=False, print_errors=True):
        """ constructor for the ErrorPacket
        @param header: (ErrorHeader or uint8) the header indicating the error
        @param original_header: (uint_8) the original header of the packet causing the error
        @param value: (uint_32) the value associated (optional, default = 0)
        @param original_sub_header: (uint_8) the original sub header causeing the error (optional, default = 0) in case of a config 
        @param skip_header_matching: (bool) if the causing headers should be converted into human readable objects (optional, default = False)
        @param print_errors: (bool) if the error should be logged (optional, default = True)
        """
        self.set_header(header)
        if self._header == ErrorHeader.OUT_ALIGN_SUCCESS_VERSION:
            self.set_org_header(original_header, True)
            self.set_value(value)
            self.set_org_sub_header(original_sub_header, True)
        else:
            self.set_org_header(original_header)
            self.set_org_sub_header(original_sub_header)
            self.set_value(value)
            if (print_errors):
                logging.error(self.__str__())
        self.check_and_log()

    def value(self):
        """ getter method for the value attribute
            @return: the value
        """
        return self._value

    def original_header(self):
        """ getter method for the original header attribute
            @return: the original header causing the error
        """
        return self._org_header
    
    def original_sub_header(self):
        """ getter method for the original sub header attribute
            @return: the original sub header causing the error
        """
        return self._org_sub_header

    def set_header(self, header):
        """ setter method for the header attribute
            @param header: (ErrorHeader or uint8) the header indicating the error
        """
        if (header in ErrorHeader):
            self._header=header
        else:
            logging.error("header "+str(header)+" is not a valid header")
            
    def set_org_header(self, header, skip_header_matching=False):
        """ setter method for the original header attribute
            @param header: (uint_8) the original header of the packet causing the error
            @param skip_header_matching: (bool) if the causing headers should be converted into human readable objects (optional, default = False)
        """
        if skip_header_matching:
            self._org_header = header
        else:
            # find the header in human readable form
            try:
                self._org_header = Data32bitHeader(header);
            except ValueError:
                try:
                    self._org_header = DataI2CHeader(header);
                except ValueError:
                    try:
                        self._org_header = PinHeader(header);
                    except ValueError:
                        try:
                            self._org_header = ConfigMainHeader(header);
                        except ValueError:
                            try:
                                self._org_header = ErrorHeader(header);
                            except ValueError:
                                logging.error("source Header "+ str(header) +" is not a valid header,")
                                self._org_header = None;

    def set_org_sub_header(self, header,skip_header_matching=False):
        """ setter method for the original sub header attribute
            @param header: (uint_8) the original sub header causeing the error
            @param skip_header_matching: (bool) if the causing headers should be converted into human readable objects (optional, default = False)
        """
        if skip_header_matching:
            self._org_sub_header = header
        else:
            # find the header in human readable form, start with config sub header, and only if that fails try the other headers - not unique
            try:
                self._org_sub_header = ConfigSubHeader(header);
            except ValueError:
                try:
                    self._org_sub_header = Data32bitHeader(header);
                except ValueError:
                    try:
                        self._org_sub_header = DataI2CHeader(header);
                    except ValueError:
                        try:
                            self._org_sub_header = PinHeader(header);
                        except ValueError:
                            try:
                                self._org_sub_header = ConfigMainHeader(header);
                            except ValueError:
                                try:
                                    self._org_sub_header = ErrorHeader(header);
                                except ValueError:
                                    logging.error("source Sub Header "+ str(header) +" is not a valid header,")
                                    self._org_sub_header = None;


    def set_value(self, value):
        """ setter method for the value attribute
            @param value: (uint_32) the value associated
        """
        if (value < 2**32 and value >= 0 and isinstance(value, int)):
            self._value=value
        else:
            logging.error("value "+str(value)+" is not a valid unsigned integer of 1 byte")
    def __str__(self):
        return "[uC Error]: "+ str(self._header) +" caused by "+ str(self._org_header)+ " with value "+ str(self._value) + ": " + str(self._header.__doc__)

    def to_bytearray(self):
        """ method to convert the packet to a bytearray in the correct format of <header><exec_time><org_header><value><org_sub_header>
            @return: the packet as a bytearray
        """
        return struct.pack("<BBIBBB", self._header, self._org_header, self._value,self._org_sub_header,0,0)

    @classmethod
    def from_bytearray(self, byte_array):
        """
        constructs a ErrorPacket from a bytearray
        @param byte_array: 9 byte bytearray to construct the Packet from
        @return: ErrorPacket
        @raise Exception: if package construction fails
        """
        unpacked = struct.unpack("<BBIBBB", byte_array)
        return ErrorPacket(header=ErrorHeader(unpacked[0]),
        original_header = unpacked[1],
        value = unpacked[2],
        original_sub_header = unpacked[3])

