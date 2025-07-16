/*
    This file is part of the Firmware project to interface with small Async or Neuromorphic chips
    Copyright (C) 2023 Ole Richter - University of Groningen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef UC_BOARDS_H
#define UC_BOARDS_H

enum firmware_version : uint8_t {
  VERSION_MAJOR = 0U,
  VERSION_MINOR = 9U,
  VERSION_PATCH = 2U,
};

#if defined(ARDUINO_TEENSY41) 

#define INPUT_BUFFER_SIZE 4096
#define OUTPUT_BUFFER_SIZE 4096

#define NUMBER_OF_DIGITAL_PINS 55

#define I2C_NUMBER_OF_INTERFACES 3
enum I2C_PORTS: uint8_t {
  I2C_SCL_PORT = 19U,
  I2C_SDA_PORT = 18U,
  I2C_SDA1_PORT = 17U,
  I2C_SCL1_PORT = 16U,
  I2C_SDA2_PORT = 25U,
  I2C_SCL2_PORT = 24U,
};
#define SPI_NUMBER_OF_INTERFACES 3
enum SPI_PORTS: uint8_t {
  SPI_SCK_PORT = 13U,
  SPI_COPI_PORT = 11U,
  SPI_CIPO_PORT = 12U,
  SPI_SCK1_PORT = 27U,
  SPI_COPI1_PORT = 26U,
  SPI_CIPO1_PORT = 1U,
  SPI_SCK2_PORT = 45U,
  SPI_COPI2_PORT = 43U,
  SPI_CIPO2_PORT = 42U,
};

#elif defined(ARDUINO_TEENSY40)

#define INPUT_BUFFER_SIZE 4096
#define OUTPUT_BUFFER_SIZE 4096

#define NUMBER_OF_DIGITAL_PINS 40

#define I2C_NUMBER_OF_INTERFACES 3
enum I2C_PORTS: uint8_t {
  I2C_SCL_PORT = 19U,
  I2C_SDA_PORT = 18U,
  I2C_SDA1_PORT = 17U,
  I2C_SCL1_PORT = 16U,
  I2C_SDA2_PORT = 25U,
  I2C_SCL2_PORT = 24U,
};
#define SPI_NUMBER_OF_INTERFACES 3
enum SPI_PORTS: uint8_t {
  SPI_SCK_PORT = 13U,
  SPI_COPI_PORT = 11U,
  SPI_CIPO_PORT = 12U,
  SPI_SCK1_PORT = 27U,
  SPI_COPI1_PORT = 26U,
  SPI_CIPO1_PORT = 1U,
  SPI_SCK2_PORT = 45U,
  SPI_COPI2_PORT = 43U,
  SPI_CIPO2_PORT = 42U,
};

#elif defined(USE_ARDUINO_MKR_PIN_LAYOUT)

#define INPUT_BUFFER_SIZE 512
#define OUTPUT_BUFFER_SIZE 512

#define NUMBER_OF_DIGITAL_PINS 22

#define I2C_NUMBER_OF_INTERFACES 1
enum I2C_PORTS: uint8_t {
  I2C_SCL_PORT = 12U,
  I2C_SDA_PORT = 11U,
};
#define SPI_NUMBER_OF_INTERFACES 1
enum SPI_PORTS: uint8_t {
  SPI_SCK_PORT = 9U,
  SPI_COPI_PORT = 8U,
  SPI_CIPO_PORT = 10U,
};

#else // defined(ARDUINO_SAMD_ZERO)

#define INPUT_BUFFER_SIZE 512
#define OUTPUT_BUFFER_SIZE 512

#define NUMBER_OF_DIGITAL_PINS 22

#define I2C_NUMBER_OF_INTERFACES 1
enum I2C_PORTS: uint8_t {
  I2C_SCL_PORT = 21U,
  I2C_SDA_PORT = 20U,
};
#define SPI_NUMBER_OF_INTERFACES 1
enum SPI_PORTS: uint8_t {
  SPI_SCK_PORT = 13U,
  SPI_COPI_PORT = 11U,
  SPI_CIPO_PORT = 12U,
};
#endif

#endif