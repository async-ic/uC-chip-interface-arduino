
#    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
#    Copyright (C) 2023 Ole Richter - University of Groningen
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
import threading
import serial
import serial.tools.list_ports
from .packet import *
from .header import *
from time import sleep
from .interface_pin import Interface_PIN
from .interface_i2c import Interface_I2C
from .interface_spi import Interface_SPI
from .interface_async import Interface_Async
from queue import Queue

class FIRMWARE_VERSION(enum.IntEnum):
    """FIRMWARE_VERSION specifies the version of the uC firmware that this API is compatible with
    major and minor version need to match, patch version of the API can be lower than the (newer) firmware 
    """
    FIRMWARE_VERSION_MAJOR = 0
    FIRMWARE_VERSION_MINOR = 9
    FIRMWARE_VERSION_PATCH = 2

class uC_api:
    """ 
    the class uC_api exposes the full interface to the uC as an object, 
    all the interface like i2c, spi, pin and async can be found as exposed variables

    Interfaces need to be activated before use, and the activation is ackloaged by the uC
    you might need to time.sleep(0.01) after the activation for the python object to update register the state change

    if you want to record data you need to use function start_experiment() to be abel to see the data send back
    the maximum time of each experiment is 72min, please call stop experiment before that to start agian

    after you are done call close_connection to sever the serial connection to the uC, 
    the recorded data in the python object remains and can be processed after
    """
    def __init__(self, serial_port_path, api_level=2):
        """__init__ creates the uC interface object and establishes the connection to the uC on the given port

        :param serial_port_path: the path of your system to the serial port, eg. on linux it might be /dev/ttyAMC0 or higher, on mac /dev/tty.usbmodem<XXXXX> on windows <COM port>
        :type serial_port_path: string 
        :param api_level: level 1 is that the api only espablishes the connection to the uC and the "infinite" write and read buffers, you need to construct the instruction packages your self, 
        level 2 it wraps the full representation of the uC interfaces in objects that are made availible as variables on this object, defaults to 2
        :type api_level: int, optional
        """
        
        #print(f"Initializing for {serial_port_path}, API: {api_level}")
        self.__experiment_state = []
        self.__experiment_state_timestamp = []
        self.__read_buffer = Queue()
        self.__write_buffer_timed = Queue()
        self.__write_buffer = Queue()
        self.__last_timed_packet = 0
        self.__api_level = api_level
        self.__name = "MCU_" + str(serial_port_path)
        
        self.__connection = None
        self.__serial_port_path = serial_port_path
        
        if self.__api_level == 2:
            # create all the interface objects
            self.errors = []
            self.spi = [Interface_SPI(self,0), Interface_SPI(self,1), Interface_SPI(self,2)]
            self.i2c = [Interface_I2C(self,0), Interface_I2C(self,1), Interface_I2C(self,2)]
            self.pin = []
            for pin_id in range(55):
                self.pin.append(Interface_PIN(self,pin_id))
            self.async_to_chip = []
            for async_id in range(8):
                self.async_to_chip.append(Interface_Async(self,async_id,"TO_CHIP"))
            self.async_from_chip = []
            for async_id in range(8):
                self.async_from_chip.append(Interface_Async(self,async_id,"FROM_CHIP"))
                
        # setup the thread function that handles the communication
        self.__communication_thread = threading.Thread(target=self.__thread_function)

        #start the thread
        self.__communication_thread.start()
        
        # wait for 10 seconds for the uC to align
        self.__connection = False
        

    def update_state(self):
        """update_state This method processes all availible messages from the uC and updates the internal representaion
        in detail it distributes the recorded packages to the coresponding interfaces for processing.

        level 2 only
        """
        # process all availible messages from the uC one by one
        while not self.__read_buffer.empty():
            # get the next package
            packet_to_process = self.__read_buffer.get()
            # choose the interface to process the package
            if isinstance(packet_to_process, ErrorPacket):
                header_for_sorting = packet_to_process.original_header()
            else:
                header_for_sorting = packet_to_process.header()
        
            no_match = True
            # high level experiment control packet
            if header_for_sorting == Data32bitHeader.IN_SET_TIME:
                self.__experiment_state.append(packet_to_process.value())
                self.__experiment_state_timestamp.append(packet_to_process.time())
                no_match = False
                continue
            # iterate though all interfaces and check if they signal they are responcible for that header
            for interface in self.async_to_chip + self.async_from_chip + self.spi + self.i2c:
                if header_for_sorting in interface.header():
                    interface.process_packet(packet_to_process)
                    self.__read_buffer.task_done()
                    no_match = False
                    break
            # pins use all the same header, so we assing the package to the pin object with the same id
            if header_for_sorting == self.pin[0].header()[0]:
                    self.pin[packet_to_process.value()].process_packet(packet_to_process)
                    self.__read_buffer.task_done()
                    no_match = False
                    continue
            elif header_for_sorting in self.pin[0].header():
                    self.pin[packet_to_process.pin_id()].process_packet(packet_to_process)
                    self.__read_buffer.task_done()
                    no_match = False
                    continue
            # if no interface is responcible for the package, we add it to the error package list
            if no_match:
                self.errors.append(str(packet_to_process))
                self.__read_buffer.task_done()

    def __str__(self):
        self.update_state()
        return "UC" + \
            "\nExperiment state: " + str(self.__experiment_state) + \
            "\nExperiment state timestamp: " + str(self.__experiment_state_timestamp) + \
            "\nlast timed packet: " + str(self.__last_timed_packet) + \
            "\nfree input queue spots on uC: " + str(free_input_queue_spots_on_uc ) + \
            "\napilevel: " + str(self.__api_level) + \
            "\nERRORS: "+str(self.errors) + "\n"            

    def start_experiment(self):
        """start_experiment This will reset the uC clock, enable that data is collected and that timed instructions are executed by the uC
        latest after 72min stop_experiment has to be called, after which a new experiment can be programmed and the function can be called again.
        """
        
        # Print status
        print(f"Starting experiment on {self.__name}")
        
        packet_to_send = Data32bitPacket(header=Data32bitHeader.IN_SET_TIME,value=1)
        self.send_packet(packet_to_send)
        

    def stop_experiment(self, time = 0):
        """stop_experiment this will stop recording and flush all not jet excecuted timed instructions

        :param time: the time in us after start_experiment when this function should be called, defaults to 0 (execute instantly)
        :type time: int, optional
        """
        
        # Print status
        print(f"Stopping experiment on {self.__name}")
        
        packet_to_send = Data32bitPacket(header=Data32bitHeader.IN_SET_TIME,value=0,time=time)
        self.send_packet(packet_to_send)
        

    def experiment_state(self):
        """experiment_state returns the state history

        1 or larger: experiment is running
        0 experiment stopped
        -1 uC reset

        :return: 2 lists: first the state second the uC timesteps in us of the state change
        :rtype: ([int],[int])
        """
        return (self.__experiment_state, self.__experiment_state_timestamp)

    def send_packet(self, packet_to_send):
        """send_packet send a packet to the uC via the "infinite" buffer
        needs a package object see package.py

        :param packet_to_send: the package to be send
        :type packet_to_send: Packet, or any subclass
        """
        # reset the time reference for the timed instructions
        if packet_to_send.header() == Data32bitHeader.IN_SET_TIME:
            self.__last_timed_packet = packet_to_send.value()
        # put the packet in the buffer depending if it s instant or timed
        if packet_to_send.time() == 0:
            self.__write_buffer.put(packet_to_send)
        else:
            self.__write_buffer_timed.put(packet_to_send)
            # check if the timed instructions are sorted in time
            if packet_to_send.time() < self.__last_timed_packet:
                logging.warning("the instructions are not sorted in time - execution order will be inconsistent")

    def read_packet(self):
        """read_packet returns one package from the uC via the "infinte" buffer

        if there is no packet availible it blocks and waits until a packet becomes availible

        :return: one package from the uC
        :rtype: Packet, or any subclass
        """
        if self.__api_level == 1:
            read_packet = self.__read_buffer.get()
            self.__read_buffer.task_done()
            return read_packet
        else:
            logging.error("reading raw packets is only availible in API level 1")

    def has_packet(self):
        """has_packet checks if a packet is availible for reading from the buffer

        :return: true if packet is avilible, false if not
        :rtype: boolean
        """
        if self.__api_level == 1:
            return (self.__read_buffer.empty() == False)
        else:
            logging.error("reading raw packets is only availible in API level 1")

    #def print_all_errors(self):

    def close_connection(self):
        """close_connection closes the serial connection to the uC and blocks until this is done
        resets the uC too
        """
        # place close connection packet in the write buffer, so the worker thread closes the connection and stop itself
        self.__write_buffer.put(Data32bitPacket(Data32bitHeader.UC_CLOSE_CONNECTION))
        # add reset to the experiment state history
        self.__experiment_state.append(-1)
        self.__experiment_state_timestamp.append(-1)
        # wait for the worker thread to close the connection
        self.__communication_thread.join()

    def reset(self):
        """reset uC and hope the serial connection survives
        """
        # place reset packet in the write buffer, so the worker thread sends it
        self.__write_buffer.put(Data32bitPacket(Data32bitHeader.IN_RESET))
        # add reset to the experiment state history
        self.__experiment_state.append(-1)
        self.__experiment_state_timestamp.append(-1)

    def __check_first_connection(self,connection):
        """__check_first_connection checks if the uC is responding and prints the firmware version
        if the firmware version does not match the API version it will print a warning
        """
        connection_state = False
        logging.info("send: opening connection - aligning commuication")
        # write 9 bytes to the uC to align the communication
        connection.write(ALIGN_BYTEARRAY)
        # wait for 10 seconds for the uC to align -> 1 second
        for i in range(4): # was 40 with sleep (0.25)
            # check if the uC has send a packet
            if connection.in_waiting >= 9:
                byte_packet = connection.read(size = 9)
                # do reading alingnment
                byte_packet = byte_packet.lstrip(b'\xff')
                sleep(0.05)
                # if the packet is not complete, try to recover by reading the rest of the packet
                if len(byte_packet) < 9:
                    if len(byte_packet) == 0:
                        # no packet (was only alignment bytes), jump to next iteration
                        continue
                    elif connection.in_waiting < 9-len(byte_packet):
                        # something went wrong, try again
                        logging.error("partial packet received, but not enough bytes send by uC, trying to recover by realigning")
                        connection.write(ALIGN_BYTEARRAY)
                        continue
                    # complete partial packet
                    else:
                        byte_packet = bytearray(byte_packet).extend(bytearray(connection.read(size = 9-len(byte_packet))))
                # convert the byte packet to a packet object
                read_packet = Packet.from_bytearray(byte_packet)
                # check if the packet is the expected Success packet
                if read_packet.header() == ErrorHeader.OUT_ALIGN_SUCCESS_VERSION:
                    logging.info("uC is ready - firmware version: "+str(read_packet.original_header())+"."+str(read_packet.original_sub_header())+"."+str(read_packet.value()))
                    # check if the firmware version matches the API version
                    if read_packet.original_header() != FIRMWARE_VERSION.FIRMWARE_VERSION_MAJOR or read_packet.original_sub_header() != FIRMWARE_VERSION.FIRMWARE_VERSION_MINOR or read_packet.value() < FIRMWARE_VERSION.FIRMWARE_VERSION_PATCH:
                        logging.warning("uC firmware version does not match the API version: \nfirmware version: "+str(read_packet.original_header())+"."+str(read_packet.original_sub_header())+"."+str(read_packet.value())+" \nAPI version: "+str(int(FIRMWARE_VERSION.FIRMWARE_VERSION_MAJOR))+"."+str(int(FIRMWARE_VERSION.FIRMWARE_VERSION_MINOR))+"."+str(int(FIRMWARE_VERSION.FIRMWARE_VERSION_PATCH)))
                    # connection is established
                    connection = True
                    connection_state = True
                    return True
                else:
                    logging.warning("unknown packet received, while connecting to uC for the first time: "+str(read_packet))
            sleep(0.25)
        # connection failed after 40 tries/10sec --> changed to 4 times, 1 second
        if connection_state == False:
            #logging.error("uC is not responding for 10 sec, wrong port?, no permission?")
            logging.error("uC is not responding to first connection request")
            return False


    def __thread_function(self):
        """__thread_function internal function managing the actual async communication with the uC in the background
        """
        idle_write_pc = False
        idle_write_uc = 0
        idle_read = False
        last_sent_time = 0
        last_free_spots = 0
        packet_send = 0
        exec_running = 0
        free_input_queue_spots_on_uc  = -1
        request_free_input_queue_spots = False
        
        # Serial connection helper variables, to retry connecting if somehow there is already a connection live
        attempt = 1
        max_attempts = 5
        connected = False
        port_error = False   
        
        # List all available serial ports
        ports = serial.tools.list_ports.comports()
        
        # Check if the ports are busy
        for port in ports:   
            if self.__serial_port_path == port.device: 
                if "USB Serial Device" in port.description:    
                    logging.info(f"{self.__serial_port_path} is listed.")    
                else:     
                    logging.info(f"{self.__serial_port_path} is busy.")
                    port_error = True         
            
        logging.info(f"Opening serial {self.__serial_port_path}, API: {self.__api_level}")
        for attempt in range(max_attempts + 1):                    
            if not connected: # no connection has been established yet                 
                if not port_error:  # port is busy or not connected
                    self.__connection = serial.Serial(self.__serial_port_path, 115200, timeout= None, write_timeout=0) #its USB so the speed setting gets ignored and it runes at max speed
                
                # init communication by forcing the uC to align
                if not self.__check_first_connection(self.__connection):
                    #print(f"Try[{attempt + 1}] Port: {self.__connection.port}, Was not the first connection, closing and retrying")
                    logging.warning(f"Try[{attempt + 1}] Port: {self.__connection.port}, Was not the first connection, closing and retrying")
                    self.__connection.close()     
                    #return
                else:
                    port_error = False
                    connected = True
            else:
                break
        
        # throw an error message if it failed to connected after x amount of times
        if attempt >= max_attempts:
            logging.error(f"ERROR: Tried {attempt + 1} times but failed to connect to {self.__serial_port_path}")

        # only start the communication if connected and no port errors        
        if connected and not port_error:           
            logging.info(f"Connected to {self.__serial_port_path}!")
            # start communication
            while True:
                # check if there is something to send
                if not self.__write_buffer_timed.empty() or not self.__write_buffer.empty():
                    # set loop slowdown condition flags to false
                    idle_write_pc = False
                    # first write the instant packets
                    if not self.__write_buffer.empty():
                        data_packet = self.__write_buffer.get()
                        # check and close the connection if requested by API
                        if data_packet.header() == Data32bitHeader.UC_CLOSE_CONNECTION:
                            self.__connection.write(Data32bitPacket(Data32bitHeader.IN_RESET).to_bytearray())
                            self.__connection.close()
                            return
                        # else send the packet
                        self.__connection.write(data_packet.to_bytearray())
                        logging.debug("send instant: "+str(data_packet))
                        self.__write_buffer.task_done()
                    # then write the timed packets
                    else:
                        # check if there is space in the uC input queue
                        if free_input_queue_spots_on_uc  > 0 :
                            # send the packet and decrease the free input queue spots reference in the API
                            data_packet = self.__write_buffer_timed.get()
                            free_input_queue_spots_on_uc  -= 1
                            self.__connection.write(data_packet.to_bytearray())
                            last_sent_time = data_packet.time()
                            packet_send += 1
                            logging.debug("send timed: "+str(data_packet))
                            self.__write_buffer_timed.task_done()
                        else:
                            # request the free input queue spots from the uC, 
                            # first request is send instantly, then every 200th loop run through
                            # to not overload the uC with requests, uC will also report the free input
                            # queue spots when it frees up space and the queue was full before
                            idle_write_uc += 1
                            if idle_write_uc%200 == 1:
                                if request_free_input_queue_spots == False:
                                    request_free_input_queue_spots = True
                                    packet_to_send = Data32bitPacket(Data32bitHeader.IN_FREE_INSTRUCTION_SPOTS)
                                    self.__connection.write(packet_to_send.to_bytearray())
                                    logging.debug("send request: "+str(packet_to_send))
                else:
                    # set write loop slowdown condition flag
                    idle_write_pc = True

                # check if there is a package to read from the serial connection, 
                # read 20 packages under the assumption that the pc is faster and
                # the uC will send more packages 
                # stops if there is nothing to read
                for i in range(20):
                    if self.__connection.in_waiting >= 9:
                        idle_read = False
                        byte_packet = self.__connection.read(size = 9)
                        # remove alignment bytes
                        byte_packet = byte_packet.lstrip(b'\xff')
                        # check if the packet is complete
                        if len(byte_packet) < 9:
                            if len(byte_packet) != 0:
                                #initiate alignment for incompleate packets
                                logging.warning("outgoing uC alignment needed, shifted by "+str(len(byte_packet))+ " bytes")
                                # partial packet not recoverable
                                if self.__connection.in_waiting < 9-len(byte_packet):
                                    logging.error("partial packet received, but not enough bytes send by uC, trying to recover by realigning")
                                    self.__connection.write(ALIGN_BYTEARRAY)
                                    continue
                                # to complete partial packet
                                byte_packet = bytearray(byte_packet).extend(bytearray(self.__connection.read(size = 9-len(byte_packet))))

                            else:
                                logging.debug("alignment sucesss - no incoming alignment error")
                                # no packet, jump to next iteration
                                continue
                        # is now aligned
                        try:
                            # convert the byte packet to a packet object
                            read_packet = Packet.from_bytearray(byte_packet)
                        except:
                            # packet was malformed, force alignment sequence
                            logging.error("packet is malformed, maybe misaligned, trying to recover by realigning")
                            self.__connection.write(ALIGN_BYTEARRAY)
                            continue
                        # packet is complete and valid
                        logging.debug("read: "+str(read_packet))
                        # catch the special case of the uC reporting free input queue spots
                        if read_packet.header() is Data32bitHeader.OUT_FREE_INSTRUCTION_SPOTS:
                            # save the free input queue spots in the API
                            if read_packet.time() > last_sent_time and exec_running > 0:
                                logging.warning("Timing exec squewed, increase buffer size in firmware, last sent time: "+\
                                    str(last_sent_time)+" uC time: "+str(read_packet.time())+\
                                        "\npackets send: "+str(packet_send)+" for free spots: "+str(last_free_spots))    
                            else:
                                logging.debug("uC reports "+str(read_packet.value())+" free input queue spots at time "+\
                                    str(read_packet.time())+"\nwaiting on PC: "+str(self.__write_buffer_timed.qsize())+\
                                        " with time starting from: "+str(last_sent_time)+\
                                        "\npackets send: "+str(packet_send)+" for free spots: "+str(last_free_spots))
                            last_free_spots = read_packet.value()
                            packet_send = 0
                            # set one less then availible, because of bug the pc will send to much
                            free_input_queue_spots_on_uc  = read_packet.value()
                            if free_input_queue_spots_on_uc == 0:
                                idle_write_uc = 2
                            else:
                                idle_write_uc = 0
                            request_free_input_queue_spots = False
                        # catch the special case of the uC reporting an malformed packet from the API
                        elif read_packet.header() is ErrorHeader.OUT_ERROR_UNKNOWN_INSTRUCTION or read_packet.header() is ErrorHeader.OUT_ERROR_UNKNOWN_CONFIGURATION:
                            logging.error("uC is reporting that it cant understand a send packet, either API and firmware are a different version or communication is not aligned, trying to recover by realigning")
                            self.__connection.write(ALIGN_BYTEARRAY)
                        # keep track of the experiment state, so we know when to issue a warning for execution time squew
                        elif read_packet.header() == Data32bitHeader.IN_SET_TIME:
                            exec_running = read_packet.value()
                            logging.info("Experiment state changed to: "+str(exec_running))
                            self.__read_buffer.put(read_packet)
                        # normal packet, send to the read buffer for further processing by the main thread
                        else:
                            self.__read_buffer.put(read_packet)
                    else:
                        # set read loop slowdown condition flag, as there is nothing to read
                        idle_read = True
                        break
                
                # slow down the loop if there is nothing to do
                if idle_read and (idle_write_pc or idle_write_uc > 0):
                    sleep(0.000003)
                    idle_read = False
                    idle_write_pc = False

            
                    
