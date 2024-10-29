
#    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
#    Copyright (C) 2023 Ole Richter - University of Groningen
#    Copyright (C) 2024 Vincent Jassies - University of Groningen
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


from .header import ConfigMainHeader, Data32bitHeader, ConfigSubHeader
from .packet import ConfigPacket, Data32bitPacket
import logging, time

class Interface_Async:
    def __init__(self, api_object, interface_id, direction):
        """ constructor for the async interface
            @param api_object: uC_api parent object
            @param interface_id: id of the interface this object is responcible for
            @param direction: "TO_CHIP" or "FROM_CHIP"
        """
        # status of the interface
        # 0 means not activated
        # 1 means activation pending
        # 2 means activted
        # -1 means error
        self.__status = 0
        self.__status_timestamp = 0
        # mode of the interface
        # "4Phase_Chigh_Dhigh" means 4 phase clock with high active clock and high active data
        # "4Phase_Clow_Dhigh" means 4 phase clock with low active clock and high active data
        # "2Phase" means 2 phase clock with high active data
        # "4Phase_MCP23017" means 4 phase clock with high active clock and high active data and MCP23017 port extender
        # "NONE" means not set
        self.__mode = "NONE"
        self.__mode_timestamp = 0
        # request pin id on the uC
        self.__req_pin = -1
        self.__req_pin_timestamp = 0
        # ack pin id on the uC
        self.__ack_pin = -1
        self.__ack_pin_timestamp = 0
        # data width of the interface
        self.__data_width = -1
        self.__data_width_timestamp = 0
        # data pin ids of the interface
        self.__data_pins = [-1]*32
        self.__data_pins_timestamp = [0]*32
        # how much the request of the handshake is delayed multiplied by 20us
        self.__req_delay = -1
        self.__req_delay_timestamp = 0
        # data send to the chip
        self.__data_from_chip = []
        self.__data_from_chip_times = []
        # data recived from the chip
        self.__data_to_chip = []
        self.__data_to_chip_times = []
        # errors
        self.__errors = []
        # the parent object
        self.__api = api_object
        # the direction of the interface
        # "TO_CHIP" or "FROM_CHIP"
        self.__direction = direction
        # the human readable name of the interface
        self.__name = "ASYNC_"+direction+str(interface_id)
        if direction == "TO_CHIP":
            self.__name = "ASYNC_"+direction+str(interface_id)
            # set the headers this object is responcible for
            if interface_id == 0:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_TO_CHIP0, Data32bitHeader.IN_ASYNC_TO_CHIP0]
            elif interface_id == 1:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_TO_CHIP1, Data32bitHeader.IN_ASYNC_TO_CHIP1] 
            elif interface_id == 2:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_TO_CHIP2, Data32bitHeader.IN_ASYNC_TO_CHIP2]
            elif interface_id == 3:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_TO_CHIP3, Data32bitHeader.IN_ASYNC_TO_CHIP3]
            elif interface_id == 4:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_TO_CHIP4, Data32bitHeader.IN_ASYNC_TO_CHIP4]
            elif interface_id == 5:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_TO_CHIP5, Data32bitHeader.IN_ASYNC_TO_CHIP5]
            elif interface_id == 6:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_TO_CHIP6, Data32bitHeader.IN_ASYNC_TO_CHIP6]
            elif interface_id == 7:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_TO_CHIP7, Data32bitHeader.IN_ASYNC_TO_CHIP7]
            else:
                logging.error("only 8 AER to chip interfaces are supported at the moment")
        elif direction == "FROM_CHIP":
            if interface_id == 0:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_FROM_CHIP0, Data32bitHeader.OUT_ASYNC_FROM_CHIP0]
            elif interface_id == 1:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_FROM_CHIP1, Data32bitHeader.OUT_ASYNC_FROM_CHIP1] 
            elif interface_id == 2:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_FROM_CHIP2, Data32bitHeader.OUT_ASYNC_FROM_CHIP2]
            elif interface_id == 3:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_FROM_CHIP3, Data32bitHeader.OUT_ASYNC_FROM_CHIP3]
            elif interface_id == 4:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_FROM_CHIP4, Data32bitHeader.OUT_ASYNC_FROM_CHIP4]
            elif interface_id == 5:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_FROM_CHIP5, Data32bitHeader.OUT_ASYNC_FROM_CHIP5]
            elif interface_id == 6:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_FROM_CHIP6, Data32bitHeader.OUT_ASYNC_FROM_CHIP6]
            elif interface_id == 7:
                self.__header = [ConfigMainHeader.IN_CONF_ASYNC_FROM_CHIP7, Data32bitHeader.OUT_ASYNC_FROM_CHIP7]
            else:
                logging.error("only 8 AER to chip interfaces are supported at the moment")
        else:
            logging.error("AER unknown direction only TO_CHIP and FROM CHIP allowed")

    def __str__(self):
        self.update()
        state_str = ("active" if self.__status == 2 else ("activation pending" if self.__status == 1 else ("not active" if self.__status == 0 else "error" )))
        return "ASYNC_" + str(self.__direction) + \
            "\nHeader: " + str(self.__header) + \
            "\nStatus: " + state_str + " at " + str(self.__status_timestamp) + "us" + \
            "\nType "+ str(self.__mode) +" at " + str(self.__mode_timestamp) + "us" + \
            "\nHS req pin "+ str(self.__req_pin) +" at " + str(self.__req_pin_timestamp) + "us with delay of 20us*" + str(self.__req_delay) +" at " + str(self.__req_delay_timestamp) + "us" + \
            "\nHS ack pin "+ str(self.__ack_pin) +" at " + str(self.__ack_pin_timestamp) + \
            "\nData width "+ str(self.__data_width) +" at " + str(self.__data_width_timestamp) + "us" + \
            "\nData pins "+ str(self.__data_pins[:self.__data_width]) +" at " + str(self.__data_pins_timestamp[:self.__data_width]) + "us" + \
            "\nSend: "+ str(self.__data_to_chip) +" at " + str(self.__data_to_chip_times) + "us" + \
            "\nRecived: "+ str(self.__data_from_chip) +" at " + str(self.__data_from_chip_times) + "us" + \
            "\nERRORS: "+str(self.__errors) + "\n"

    def header(self):
        """ get the header this object is responcible for
            @return: list of headers
        """
        return self.__header

    def status(self):
        """ get the human readable status of the interface
            @return: tuple of status and timestamp
        """
        self.update()
        state_str = ("active" if self.__status == 2 else ("activation pending" if self.__status == 1 else ("not active" if self.__status == 0 else "error" )))
        return (state_str,self.__status_timestamp)
    
    def interface_type(self):
        """ get the human readable type of the interface
            @return: tuple of type and timestamp of to what and when the type was actually set on the uC)
        """
        self.update()
        return (self.__mode,self.__mode_timestamp)

    def data_pins(self):
        """ get the data pins of the interface as they are on the uC
            @return: tuple of data pins and their timestamps of to what and when the pins were actually set on the uC
        """
        self.update()
        return (self.__data_pins[:self.__data_width],self.__data_pins_timestamp[:self.__data_width])
    
    def data_width(self):
        """ get the data width of the interface as it is on the uC
            @return: tuple of data width and its timestamp of to what and when the width was actually set on the uC
        """
        self.update()
        return (self.__data_width,self.__data_width_timestamp)
    
    def req_pin(self):
        """ get the request pin of the interface as it is on the uC
            @return: tuple of request pin and its timestamp of to what and when the pin was actually set on the uC
        """
        self.update()
        return (self.__req_pin,self.__req_pin_timestamp)

    def ack_pin(self): 
        """ get the ack pin of the interface as it is on the uC
            @return: tuple of ack pin and its timestamp of to what and when the pin was actually set on the uC
        """
        self.update()
        return (self.__ack_pin,self.__ack_pin_timestamp)

    def req_delay(self):
        """ get the request delay of the interface as it is on the uC
            @return: tuple of request delay and its timestamp of to what and when the delay was actually set on the uC
        """
        self.update()
        return (self.__req_delay,self.__req_delay_timestamp)

    def data_from_chip(self):
        """ get the data recived from the chip
            will retun 2 lists: one with the word recoded and one with the time when it was recorded, linked by index
            @return: tuple of the of data list and their timestamp list - index matched
        """
        self.update()
        return (self.__data_from_chip, data_from_chip_times)
    
    def data_to_chip(self):
        """ get the data send to the chip when they are actually send off by the uC
            data_to_chip will retun the data send by the uC to the device under test (DUT)

            will retun 2 lists: one with the word send and one with the exact time when it was send, linked by index

            the time might differ slightly from the time you sheduled the send word, 
            as it is the time when it was send out and the uC can only send one word at a time
            
            @return: tuple of the of data list and their timestamp list (of when the uC send them) - index matched
        """
        self.update()
        return (self.__data_to_chip, self.__data_to_chip_times)

    def data_from_chip_and_clear(self):
        """ get the data recived from the chip and clear the buffer
            will retun 2 lists: one with the word recoded and one with the time when it was recorded, linked by index

            @return: tuple of the of data list and their timestamp list - index matched
        """
        self.update()
        data = self.__data_from_chip
        time = self.__data_from_chip_times
        self.__data_from_chip = []
        self.__data_from_chip_times = []
        return (data, time)
    
    def data_to_chip_and_clear(self):
        """ get the data send to the chip when they are actually send off by the uC and clear the buffer
            data_to_chip will retun the data send by the uC to the device under test (DUT)

            will retun 2 lists: one with the word send and one with the exact time when it was send, linked by index

            the time might differ slightly from the time you sheduled the send word, 
            as it is the time when it was send out and the uC can only send one word at a time

            @return: tuple of the of data list and their timestamp list (of when the uC send them) - index matched
        """
        self.update()
        data = self.__data_to_chip
        time = self.__data_to_chip_times
        self.__data_to_chip = []
        self.__data_to_chip_times = []
        return (data, time)
    
    def errors(self):
        """ get the list of errors associated with this interface object
            @return: list of errors
        """
        self.update()
        return self.__errors


    def process_packet(self, packet):
        """ process a packet from the uC, update the status of the interface object and store the data
            @param packet: packet object to be processed
        """
        if packet.header() in self.__header:
            # process any acknolage config packet (config sucessfully set on the uC) 
            if packet.header() == self.__header[0]:
                # interface is active
                if packet.config_header() == ConfigSubHeader.CONF_ACTIVE:
                    self.__status = 2
                    self.__status_timestamp = packet.time()
                    return
                # the mode of the interface
                elif packet.config_header() == ConfigSubHeader.CONF_TYPE:
                    """@todo replace with type"""  
                    if packet.value() == 0:
                        self.__mode = "4Phase_Chigh_Dhigh"
                    elif packet.value() == 1:
                        self.__mode = "4Phase_Clow_Dhigh"
                    elif packet.value() == 10:
                        self.__mode = "2Phase"
                    elif packet.value() == 20:
                        self.__mode = "4Phase_MCP23017"
                    else:
                        logging.error("unknown async type returned")
                    self.__mode_timestamp = packet.time()
                    return
                # the ack pin of the interface
                elif packet.config_header() == ConfigSubHeader.CONF_ACK:
                    self.__ack_pin = packet.value()
                    self.__ack_pin_timestamp = packet.time()
                    return
                # the request pin of the interface
                elif packet.config_header() == ConfigSubHeader.CONF_REQ:
                    self.__req_pin = packet.value()
                    self.__req_pin_timestamp = packet.time()
                    return
                # the data width of the interface
                elif packet.config_header() == ConfigSubHeader.CONF_WIDTH:
                    self.__data_width = packet.value()
                    self.__data_width_timestamp = packet.time()
                    return
                # the data pins of the interface
                elif packet.config_header() < 32:
                    self.__data_pins[packet.config_header()] = packet.value()
                    self.__data_pins_timestamp[packet.config_header()] = packet.time()
                    return
                # the request line delay of the interface
                elif packet.config_header() == ConfigSubHeader.CONF_REQ_DELAY:
                    self.__req_delay = packet.value()
                    self.__req_delay_timestamp = packet.time()
                    return
            # process any data packet (data send from the uC to the chip or recived from the chip)
            elif packet.header() == self.__header[1]:
                if self.__direction == "TO_CHIP":
                    self.__data_to_chip.append(packet.value())
                    self.__data_to_chip_times.append(packet.time())
                    return
                elif self.__direction == "FROM_CHIP":
                    self.__data_from_chip.append(packet.value())
                    self.__data_from_chip_times.append(packet.time())
                    return
        # all other packets are errors or not supported, disable the interface
        self.__errors.append(str(packet))
        self.__status = -1

    def activate(self, req_pin, ack_pin, data_width, data_pins, mode="4Phase_Chigh_Dhigh", req_delay = 0, time = 0):
        """ activate the interface on the uC with the given parameters
            
            Mode: 
            - "4Phase_Chigh_Dhigh" means 4 phase clock with high active clock and high active data
            - "4Phase_Clow_Dhigh" means 4 phase clock with low active clock and high active data
            - "2Phase" means 2 phase clock with high active data
            - "4Phase_MCP23017" means 4 phase clock with high active clock and high active data and MCP23017 port extender, pin IDs are not used in this mode

            @param req_pin: request pin id on the uC
            @param ack_pin: ack pin id on the uC
            @param data_width: data width of the interface
            @param data_pins: list of data pin ids on the uC
            @param mode: mode of the interface - "4Phase_Chigh_Dhigh", "4Phase_Clow_Dhigh", "2Phase", "4Phase_MCP23017"
            @param req_delay: how much the request of the handshake is delayed multiplied by 20us
            @param time: the exec_time when the uc should activate the interface, 0 means as soon as possible
        """
        # only activate if not already activated or pending activation
        if self.__status >= 1:
            logging.warning("ASYNC_to_chip interface "+str(self.__header[0])+" is already activated or waiting activation, doing nothing")
        else:
            # send the configuration to the uC as individual packets
            if mode == "4Phase_Chigh_Dhigh":
                self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader.CONF_TYPE, value = 0, time = time))
                self.__status = 1
            elif mode == "4Phase_Clow_Dhigh":
                self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader.CONF_TYPE, value = 1, time = time))
                self.__status = 1
            elif mode == "2Phase_Chigh_Dhigh":
                logging.warning("pin mode not implmented yet")
                return
            elif mode == "2Phase_Clow_Dhigh":
                logging.warning("pin mode not implmented yet")
                return
            elif mode == "4Phase_MCP23017":
                self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader.CONF_TYPE, value = 20, time = time))
                self.__status = 1
            else:
                logging.error("pin.activate got wrong type "+str(pin_mode)+" only 4Phase_Chigh_Dhigh, 4Phase_Clow_Dhigh, 2Phase_Chigh_Dhigh, 2Phase_Clow_Dhigh are allowed, more modes implemted on request")
                return
            self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader.CONF_ACK, value = ack_pin, time = time))
            self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader.CONF_REQ, value = req_pin, time = time))
            self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader.CONF_WIDTH, value = data_width, time = time))
            for pin in range(data_width):
                self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader(pin), value = data_pins[pin], time = time))
            self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader.CONF_REQ_DELAY, value = req_delay, time = time))
            # after all configuration is send, send activation request
            self.__api.send_packet(ConfigPacket(header = self.__header[0], config_header = ConfigSubHeader.CONF_ACTIVE, time = time))
            # set the status to pending confirmation
            self.__status = 1

    def send(self, word, time = 0):
        """ send a word to the chip
            @param word: word to be send
            @param time: the exec_time when the uc should send the word, 0 means as soon as possible
        """
        self.update()
        if self.__direction == "TO_CHIP":
            # we dont check the status here anymore as the uC will report the error anyway
            self.__api.send_packet(Data32bitPacket(header = self.__header[1], value = word, time = time))

        else:
            logging.error("AER to chip interface "+str(self.__header[1])+" is reading interface - word is not sent.")

    def update(self):
        self.__api.update_state()
