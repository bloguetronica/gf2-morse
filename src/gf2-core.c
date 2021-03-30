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


// Includes
#include <stdio.h>
#include <stdlib.h>
#include "gf2-core.h"

// Defines
#define TR_TIMEOUT 100  // Transfer timeout in milliseconds

// Global variables
int err_level;

void clear_registers(libusb_device_handle *devhandle)  // Clears all frequency and phase registers on the AD9834 waveform generator (channel 0 must be enabled)
{
    unsigned char write_command_buf[20] = {
        0x00, 0x00,              // Reserved
        0x01,                    // Write command
        0x00,                    // Reserved
        0x0C, 0x00, 0x00, 0x00,  // Twelve bytes to write
        0x40, 0x00, 0x40, 0x00,  // FREQ0 register set to zero
        0x80, 0x00, 0x80, 0x00,  // FREQ1 register set to zero
        0xC0, 0x00,              // PHASE0 register set to zero
        0xE0, 0x00               // PHASE1 register set to zero
    };
    int bytes_written;
    if (libusb_bulk_transfer(devhandle, 0x01, write_command_buf, sizeof(write_command_buf), &bytes_written, TR_TIMEOUT) != 0)
    {
        fprintf(stderr, "Error: Failed bulk OUT transfer to endpoint 1 (address 0x01).\n");
        err_level = EXIT_FAILURE;
    }
}

void configure_spi_mode(libusb_device_handle *devhandle, uint8_t channel, bool cpol, bool cpha)  // Configures the given SPI channel in respect to its clock polarity and phase
{
    unsigned char control_buf_out[2] = {
        channel,                          // Selected channel
        0x20 * cpha + 0x10 * cpol + 0x08  // Control word (specified polarity and phase, push-pull mode, 12MHz)
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x31, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x31).\n");
        err_level = EXIT_FAILURE;
    }
}

void disable_cs(libusb_device_handle *devhandle, uint8_t channel)  // Disables the chip select corresponding to the target channel
{
    unsigned char control_buf_out[2] = {
        channel,  // Selected channel
        0x00      // Corresponding chip select disabled
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x25, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x25).\n");
        err_level = EXIT_FAILURE;
    }
}

void disable_spi_delays(libusb_device_handle *devhandle, uint8_t channel)  // Disables all SPI delays for a given channel
{
    unsigned char control_buf_out[8] = {
        channel,     // Selected channel
        0x00,        // All SPI delays disabled, no CS toggle
        0x00, 0x00,  // Inter-byte,
        0x00, 0x00,  // post-assert and
        0x00, 0x00   // pre-deassert delays all set to 0us
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x33, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x33).\n");
        err_level = EXIT_FAILURE;
    }
}

bool get_gpio2(libusb_device_handle *devhandle)  // Gets the current value of the GPIO.2 pin on the CP2130
{
    unsigned char control_buf_in[2];
    if (libusb_control_transfer(devhandle, 0xC0, 0x20, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), TR_TIMEOUT) != sizeof(control_buf_in))
    {
        fprintf(stderr, "Error: Failed control transfer (0xC0, 0x20).\n");
        err_level = EXIT_FAILURE;
    }
    return ((0x20 & control_buf_in[1]) != 0x00);  // Returns one if bit 5 of byte 1, which corresponds to the GPIO.2 pin, is not set to zero
}

bool get_gpio3(libusb_device_handle *devhandle)  // Gets the current value of the GPIO.3 pin on the CP2130
{
    unsigned char control_buf_in[2];
    if (libusb_control_transfer(devhandle, 0xC0, 0x20, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), TR_TIMEOUT) != sizeof(control_buf_in))
    {
        fprintf(stderr, "Error: Failed control transfer (0xC0, 0x20).\n");
        err_level = EXIT_FAILURE;
    }
    return ((0x40 & control_buf_in[1]) != 0x00);  // Returns one if bit 6 of byte 1, which corresponds to the GPIO.3 pin, is not set to zero
}

bool get_gpio4(libusb_device_handle *devhandle)  // Gets the current value of the GPIO.4 pin on the CP2130
{
    unsigned char control_buf_in[2];
    if (libusb_control_transfer(devhandle, 0xC0, 0x20, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), TR_TIMEOUT) != sizeof(control_buf_in))
    {
        fprintf(stderr, "Error: Failed control transfer (0xC0, 0x20).\n");
        err_level = EXIT_FAILURE;
    }
    return ((0x80 & control_buf_in[1]) != 0x00);  // Returns one if bit 7 of byte 1, which corresponds to the GPIO.4 pin, is not set to zero
}

bool get_gpio5(libusb_device_handle *devhandle)  // Gets the current value of the GPIO.5 pin on the CP2130
{
    unsigned char control_buf_in[2];
    if (libusb_control_transfer(devhandle, 0xC0, 0x20, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), TR_TIMEOUT) != sizeof(control_buf_in))
    {
        fprintf(stderr, "Error: Failed control transfer (0xC0, 0x20).\n");
        err_level = EXIT_FAILURE;
    }
    return ((0x01 & control_buf_in[0]) != 0x00);  // Returns one if bit 0 of byte 0, which corresponds to the GPIO.5 pin, is not set to zero
}

bool get_gpio6(libusb_device_handle *devhandle)  // Gets the current value of the GPIO.6 pin on the CP2130
{
    unsigned char control_buf_in[2];
    if (libusb_control_transfer(devhandle, 0xC0, 0x20, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), TR_TIMEOUT) != sizeof(control_buf_in))
    {
        fprintf(stderr, "Error: Failed control transfer (0xC0, 0x20).\n");
        err_level = EXIT_FAILURE;
    }
    return ((0x04 & control_buf_in[0]) != 0x00);  // Returns one if bit 2 of byte 0, which corresponds to the GPIO.6 pin, is not set to zero
}

bool is_otp_locked(libusb_device_handle *devhandle)  // Checks if the OTP ROM of the CP2130 is locked
{
    unsigned char control_buf_in[2];
    if (libusb_control_transfer(devhandle, 0xC0, 0x6E, 0x0000, 0x0000, control_buf_in, sizeof(control_buf_in), TR_TIMEOUT) != sizeof(control_buf_in))
    {
        fprintf(stderr, "Error: Failed control transfer (0xC0, 0x6E).\n");
        err_level = EXIT_FAILURE;
    }
    return (control_buf_in[0] == 0x00 && control_buf_in[1] == 0x00);  // Returns one if both lock bytes are set to zero, that is, the OPT ROM is locked
}

void lock_otp(libusb_device_handle *devhandle)  // Locks the OTP ROM on the CP2130
{
    unsigned char control_buf_out[2] = {
        0x00, 0x00  // Values to be written into the lock bytes, so that both are set to zero
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x6F, 0xA5F1, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x6F).\n");
        err_level = EXIT_FAILURE;
    }
}

void reset(libusb_device_handle *devhandle)  // Issues a reset to the CP2130, which in effect resets the entire device
{
    if (libusb_control_transfer(devhandle, 0x40, 0x10, 0x0000, 0x0000, NULL, 0, TR_TIMEOUT) != 0)
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x10).\n");
        err_level = EXIT_FAILURE;
    }
}

void select_cs(libusb_device_handle *devhandle, uint8_t channel)  // Enables the chip select of the target channel, disabling any others
{
    unsigned char control_buf_out[2] = {
        channel,  // Selected channel
        0x02      // Only the corresponding chip select is enabled, all the others are disabled
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x25, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x25).\n");
        err_level = EXIT_FAILURE;
    }
}

void setup(libusb_device_handle *devhandle)  // Sets up the AD9834 waveform generator
{
    unsigned char write_command_buf[10] = {
        0x00, 0x00,              // Reserved
        0x01,                    // Write command
        0x00,                    // Reserved
        0x02, 0x00, 0x00, 0x00,  // Two bytes to write
        0x22, 0x00               // B28 = 1, PIN/SW = 1
    };
    int bytes_written;
    if (libusb_bulk_transfer(devhandle, 0x01, write_command_buf, sizeof(write_command_buf), &bytes_written, TR_TIMEOUT) != 0)
    {
        fprintf(stderr, "Error: Failed bulk OUT transfer to endpoint 1 (address 0x01).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_amplitude(libusb_device_handle *devhandle, uint16_t value)  // Sets the register on the AD5310 DAC to a given value, in order to set the amplitude (channel 1 must be enabled)
{
    unsigned char write_command_buf[10] = {
        0x00, 0x00,                    // Reserved
        0x01,                          // Write command
        0x00,                          // Reserved
        0x02, 0x00, 0x00, 0x00,        // Two bytes to write
        0x0F & (uint8_t)(value >> 6),  // Amplitude set according to the given value
        (uint8_t)(value << 2)
    };
    int bytes_written;
    if (libusb_bulk_transfer(devhandle, 0x01, write_command_buf, sizeof(write_command_buf), &bytes_written, TR_TIMEOUT) != 0)
    {
        fprintf(stderr, "Error: Failed bulk OUT transfer to endpoint 1 (address 0x01).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_frequency(libusb_device_handle *devhandle, uint32_t value, bool sel)  // Sets either FREQ0 or FREQ1 registers on the AD9834 waveform generator to the given value
{
    unsigned char write_command_buf[12] = {
        0x00, 0x00,                                            // Reserved
        0x01,                                                  // Write command
        0x00,                                                  // Reserved
        0x04, 0x00, 0x00, 0x00,                                // Four bytes to write
        (sel ? 0x80 : 0x40) + (0x3F & (uint8_t)(value >> 8)),  // FREQ0 or FREQ1 register set to the given value, according to the boolean variable "sel"
        (uint8_t)value,
        (sel ? 0x80 : 0x40) + (0x3F & (uint8_t)(value >> 22)),
        (uint8_t)(value >> 14)
    };
    int bytes_written;
    if (libusb_bulk_transfer(devhandle, 0x01, write_command_buf, sizeof(write_command_buf), &bytes_written, TR_TIMEOUT) != 0)
    {
        fprintf(stderr, "Error: Failed bulk OUT transfer to endpoint 1 (address 0x01).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_gpio2(libusb_device_handle *devhandle, bool value)  // Sets the GPIO.2 pin on the CP2130 to a given value
{
    unsigned char control_buf_out[4] = {
        0x00, 0xFF * value,  // Set the value of GPIO.2 to the intended value
        0x00, 0x20           // Set the mask so that only GPIO.2 is changed
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x21, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x21).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_gpio3(libusb_device_handle *devhandle, bool value)  // Sets the GPIO.3 pin on the CP2130 to a given value
{
    unsigned char control_buf_out[4] = {
        0x00, 0xFF * value,  // Set the value of GPIO.3 to the intended value
        0x00, 0x40           // Set the mask so that only GPIO.3 is changed
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x21, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x21).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_gpio4(libusb_device_handle *devhandle, bool value)  // Sets the GPIO.4 pin on the CP2130 to a given value
{
    unsigned char control_buf_out[4] = {
        0x00, 0xFF * value,  // Set the value of GPIO.4 to the intended value
        0x00, 0x80           // Set the mask so that only GPIO.4 is changed
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x21, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x21).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_gpio5(libusb_device_handle *devhandle, bool value)  // Sets the GPIO.5 pin on the CP2130 to a given value
{
    unsigned char control_buf_out[4] = {
        0xFF * value, 0x00,  // Set the value of GPIO.5 to the intended value
        0x01, 0x00           // Set the mask so that only GPIO.5 is changed
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x21, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x21).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_gpio6(libusb_device_handle *devhandle, bool value)  // Sets the GPIO.6 pin on the CP2130 to a given value
{
    unsigned char control_buf_out[4] = {
        0xFF * value, 0x00,  // Set the value of GPIO.6 to the intended value
        0x04, 0x00           // Set the mask so that only GPIO.6 is changed
    };
    if (libusb_control_transfer(devhandle, 0x40, 0x21, 0x0000, 0x0000, control_buf_out, sizeof(control_buf_out), TR_TIMEOUT) != sizeof(control_buf_out))
    {
        fprintf(stderr, "Error: Failed control transfer (0x40, 0x21).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_phase(libusb_device_handle *devhandle, uint16_t value, bool sel)  // Sets either PHASE0 or PHASE1 registers on the AD9834 waveform generator to the given value
{
    unsigned char write_command_buf[10] = {
        0x00, 0x00,                                            // Reserved
        0x01,                                                  // Write command
        0x00,                                                  // Reserved
        0x02, 0x00, 0x00, 0x00,                                // Two bytes to write
        (sel ? 0xE0 : 0xC0) + (0x0F & (uint8_t)(value >> 8)),  // PHASE0 or PHASE1 register set to the given value, according to the boolean variable "sel"
        (uint8_t)value
    };
    int bytes_written;
    if (libusb_bulk_transfer(devhandle, 0x01, write_command_buf, sizeof(write_command_buf), &bytes_written, TR_TIMEOUT) != 0)
    {
        fprintf(stderr, "Error: Failed bulk OUT transfer to endpoint 1 (address 0x01).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_sine_wave(libusb_device_handle *devhandle)  // Configures the control register on the AD9834 waveform generator to generate a sine wave (channel 0 must be enabled)
{
    unsigned char write_command_buf[10] = {
        0x00, 0x00,              // Reserved
        0x01,                    // Write command
        0x00,                    // Reserved
        0x02, 0x00, 0x00, 0x00,  // Two bytes to write
        0x22, 0x00               // Sinusoidal waveform, B28 = 1, PIN/SW = 1
    };
    int bytes_written;
    if (libusb_bulk_transfer(devhandle, 0x01, write_command_buf, sizeof(write_command_buf), &bytes_written, TR_TIMEOUT) != 0)
    {
        fprintf(stderr, "Error: Failed bulk OUT transfer to endpoint 1 (address 0x01).\n");
        err_level = EXIT_FAILURE;
    }
}

void set_triangle_wave(libusb_device_handle *devhandle)  // Configures the control register on the AD9834 waveform generator in order to generate a triangle wave (channel 0 must be enabled)
{
    unsigned char write_command_buf[10] = {
        0x00, 0x00,              // Reserved
        0x01,                    // Write command
        0x00,                    // Reserved
        0x02, 0x00, 0x00, 0x00,  // Two bytes to write
        0x22, 0x02               // Triangular waveform, B28 = 1, PIN/SW = 1
    };
    int bytes_written;
    if (libusb_bulk_transfer(devhandle, 0x01, write_command_buf, sizeof(write_command_buf), &bytes_written, TR_TIMEOUT) != 0)
    {
        fprintf(stderr, "Error: Failed bulk OUT transfer to endpoint 1 (address 0x01).\n");
        err_level = EXIT_FAILURE;
    }
}
