# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

### Fixed

### Changed

### Removed

### Deprecated

## [0.9.3] - 2024-10-29

### Added
- uC errors now print thier doc description, for better understanding.

### Fixed

- If the uC connection is not responding after a second the api will now retry opening the connection

### Changed

 - api will no longer check if an interface is active before writing to it (creates unwanted corner cases) => the uC is responcible for reporting that error!

## [0.9.2] - 2024-02-28

### Added
- uC_api object is now printable
- warning if uc is faster in execution than PC can deliver new timed packages
- headers can now be dynamically selected to print warning or info when they are created by PC or uC

### Fixed
- alignment was missbeaving, when actually miss aligned
- responce improved by reading up to 20 packets per 1 write packet

### Changed
- alignment success responce needs to be triggered by sending allignment word after alignment sucess word, to prevent lockup
- conformed buffer access to functions from core_ring_buffer.h only

### Removed

### Deprecated

## [0.9.1] - 2024-02-18

### Added
- code documentation --
- check of comunication with uC while starting, error on no connection
- warn on api and firmware version missmatch
- sends firmware version on aligning connection
### Fixed
 - trys to recover from misaligned communication if the byte sequence goes out of sync
 - data_i2c package fixed faulty value transmission (firmware) and encoding (api)
 - write on busy serial connection does no longer block the worker thread, eg. from reading
 - typo in packet, variable visibility in thread



## [0.9.0] - 2023-10-13

### Added 
 - all executions are acklogage with timestamp to the PC
 - added support for Teensy 4.0 (MIMXR)
 - added support for Arduino Zero, MRKZero and arduino MRK series (SAMD21), timed release does not work yet (trigger)
 - added software reset for UC
 - added python api level 1 (extablish connection + buffered communication) 
 - added python api level 2 (object representation of uC) next to 0 (packets and headers)
 - output buffer is now contiusly send, with optional legacy mode
 - input buffer can now tell is number of free slots
 - python can now be imported like a package
 - I2C interfaces mow available
 - added level 0 api for mapper (not functional) 
 - documentation website https://olerichter.github.io/uC-chip-interface

### Fixed
 - output buffer now behaves the same as the input buffer
 - readout now stalls data aquisition on full buffer (with warning) instead of going unresponcive

### Changed
 - error packets and normal packets are send via functions
 - SPI now can send 1-4 bytes
 - SPI can now configure mode and speed
 - Pin result is now also indicated by packet header for mapping

### Removed
 - special pin conf ackloage package 

## [0.1.0] - 2023-01-25

### Added 
- all commands can be timed with ~100us presision (modifiable)
- all interfaces are dynamicly set up by API on runtime (no seperate firmare for different chips)
- set pin high/low
- send SPI word 8/32 bits
- send 4 phase HS word 0-32bit
- record pin change with time stamp
- read SPI word 8/32 bits with time stamp
- read 4 phase HS word 0-32bit with time stamp
- primitive packet python API
