/* GF2 core functions - Version 1.0
   Copyright (c) 2018-2019 Samuel Lourenço

   This library is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library.  If not, see <https://www.gnu.org/licenses/>.


   Please feel free to contact me via e-mail: samuel.fmlourenco@gmail.com */


#ifndef GF2_CORE_H_
#define GF2_CORE_H_

// Includes
#include <stdbool.h>
#include <libusb-1.0/libusb.h>

// Defines
#define CPOL0 false   // Boolean corresponding to CPOL = 0, applicable to configure_spi_mode()
#define CPOL1 true    // Boolean corresponding to CPOL = 1, applicable to configure_spi_mode()
#define CPHA0 false   // Boolean corresponding to CPHA = 0, applicable to configure_spi_mode()
#define CPHA1 true    // Boolean corresponding to CPHA = 1, applicable to configure_spi_mode()
#define FREQ0 false   // Boolean corresponding to the FREQ0 register, applicable to set_frequency()
#define FREQ1 true    // Boolean corresponding to the FREQ1 register, applicable to set_frequency()
#define PHASE0 false  // Boolean corresponding to the PHASE0 register, applicable to set_phase()
#define PHASE1 true   // Boolean corresponding to the PHASE1 register, applicable to set_phase()

// Global external variables
extern int err_level;

// Function prototypes
void clear_registers(libusb_device_handle *devhandle);
void configure_spi_mode(libusb_device_handle *devhandle, uint8_t channel, bool cpol, bool cpha);
void disable_cs(libusb_device_handle *devhandle, uint8_t channel);
void disable_spi_delays(libusb_device_handle *devhandle, uint8_t channel);
bool get_gpio2(libusb_device_handle *devhandle);
bool get_gpio3(libusb_device_handle *devhandle);
bool get_gpio4(libusb_device_handle *devhandle);
bool get_gpio5(libusb_device_handle *devhandle);
bool get_gpio6(libusb_device_handle *devhandle);
bool is_otp_locked(libusb_device_handle *devhandle);
void lock_otp(libusb_device_handle *devhandle);
void reset(libusb_device_handle *devhandle);
void select_cs(libusb_device_handle *devhandle, uint8_t channel);
void setup(libusb_device_handle *devhandle);
void set_amplitude(libusb_device_handle *devhandle, uint16_t value);
void set_frequency(libusb_device_handle *devhandle, uint32_t value, bool sel);
void set_gpio2(libusb_device_handle *devhandle, bool value);
void set_gpio3(libusb_device_handle *devhandle, bool value);
void set_gpio4(libusb_device_handle *devhandle, bool value);
void set_gpio5(libusb_device_handle *devhandle, bool value);
void set_gpio6(libusb_device_handle *devhandle, bool value);
void set_phase(libusb_device_handle *devhandle, uint16_t value, bool sel);
void set_sine_wave(libusb_device_handle *devhandle);
void set_triangle_wave(libusb_device_handle *devhandle);

#endif
