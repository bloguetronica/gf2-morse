/* GF2 Morse Command - Version 1.0 for Debian Linux
   Copyright (c) 2020 Samuel Lourenço

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation, either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <https://www.gnu.org/licenses/>.


   Please feel free to contact me via e-mail: samuel.fmlourenco@gmail.com */


// Includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include "gf2-core.h"
#include "libusb-extra.h"

// Defines
#define EXIT_USERERR 2  // Exit status value to indicate a command usage error
#define UNIT 50000  // Time unit in us

// Function prototypes
void signal_char_code(libusb_device_handle *devhandle, char *code);
void signal_message(libusb_device_handle *devhandle, char *message);

int main(int argc, char **argv)
{
    err_level = EXIT_SUCCESS;  // Note that this variable is declared externally!
    if (argc < 2)  // If the program was called without arguments
    {
        fprintf(stderr, "Error: Missing argument.\nUsage: gf2-morse MESSAGE\n");
        err_level = EXIT_USERERR;
    }
    else
    {
        libusb_context *context;
        if (libusb_init(&context) != 0)  // Initialize libusb. In case of failure
        {
            fprintf(stderr, "Error: Could not initialize libusb.\n");
            err_level = EXIT_FAILURE;
        }
        else  // If libusb is initialized
        {
            libusb_device_handle *devhandle;
            if (argc < 3)  // If the program was called without arguments
                devhandle = libusb_open_device_with_vid_pid(context, 0x10C4, 0x8BF1);  // Open a device and get the device handle
            else  // Serial number was specified as argument
                devhandle = libusb_open_device_with_vid_pid_serial(context, 0x10C4, 0x8BF1, (unsigned char *)argv[2]);  // Open the device having the specified serial number, and get the device handle
            if (devhandle == NULL)  // If the previous operation fails to get a device handle
            {
                fprintf(stderr, "Error: Could not find device.\n");
                err_level = EXIT_FAILURE;
            }
            else  // If the device is successfully opened and a handle obtained
            {
                bool kernel_attached = false;
                if (libusb_kernel_driver_active(devhandle, 0) != 0)  // If a kernel driver is active on the interface
                {
                    libusb_detach_kernel_driver(devhandle, 0);  // Detach the kernel driver
                    kernel_attached = true;  // Flag that the kernel driver was attached
                }
                if (libusb_claim_interface(devhandle, 0) != 0)  // Claim the interface. In case of failure
                {
                    fprintf(stderr, "Error: Device is currently unavailable.\n");
                    err_level = EXIT_FAILURE;
                }
                else  // If the interface is successfully claimed
                {
                    if (get_gpio2(devhandle) && err_level == EXIT_SUCCESS)  // Check if GPIO.2/RESET is high (err_level can change to 1 as a consequence of that verification, hence the need for "&& err_level ==  EXIT_SUCCESS" in order to avoid misleading messages)
                            fprintf(stderr, "Error: Waveform generator is stopped and should be running.\nPlease invoke gf2-start and try again.\n");
                    else if (!get_gpio3(devhandle) && err_level == EXIT_SUCCESS)  // Check if GPIO.3/SLEEP is low (again, the same precaution is needed)
                            fprintf(stderr, "Error: Waveform generator DAC is enabled and should be disabled.\nPlease invoke gf2-dacoff and try again.\n");
                    else if (err_level == EXIT_SUCCESS)  // If all goes well so far
                    {
                        printf("Signaling message...\n");
                        signal_message(devhandle, argv[1]);
                        if (err_level == EXIT_SUCCESS)  // If all goes well
                            printf("\nMessage signaled.\n");
                    }
                    libusb_release_interface(devhandle, 0);  // Release the interface
                }
                if (kernel_attached)  // If a kernel driver was attached to the interface before
                    libusb_attach_kernel_driver(devhandle, 0);  // Reattach the kernel driver
                libusb_close(devhandle);  // Close the device
            }
            libusb_exit(context);  // Deinitialize libusb
        }
    }
    return err_level;
}

void signal_char_code(libusb_device_handle *devhandle, char *code)  // Signals character code (adding a trailing inter-character space)
{
    size_t len = strlen(code);
    for (size_t i = 0; i < len; ++i)
    {
        if (code[i] == '.' || code [i] == '-')  // This condition is only required for sanity purposes
        {
            set_gpio3(devhandle, false);  // Set GPIO.3 to a logical low in order to enable the AD9834 internal DAC
            usleep(UNIT);  // Corresponds to a "dot"
            if (code[i] == '-')
                usleep(2 * UNIT);  // Adds to the previous time interval so it corresponds to a "dash"
            set_gpio3(devhandle, true);  // Set GPIO.3 to a logical high in order to disable the AD9834 internal DAC
            usleep(UNIT);  // Corresponds to an intra-character space
        }
    }
    usleep(2 * UNIT);  // Inter-character space
}

void signal_message(libusb_device_handle *devhandle, char *message)  // Signals message
{
    size_t len = strlen(message);
    for (size_t i = 0; i < len; ++i)
    {
        switch (message[i])  // Note that, at this point, only the characters listed below will be signaled!
        {
            case '\n':
            case ' ':
                if (i > 0 && message[i - 1] != '\n' && message[i - 1] != ' ')  // Extra spaces omitted!
                {
                    printf(" ");
                    fflush(stdout);  // Force character output
                    usleep(4 * UNIT);  // Word space
                }
                break;
            case '!':
                printf("!");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-.-.--");
                break;
            case '"':
                printf("\"");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".-..-.");
                break;
            case '$':
                printf("$");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "...-..-");
                break;
            case '&':
                printf("&");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".-...");
                break;
            case '\'':
                printf("'");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".----.");
                break;
            case '(':
                printf("(");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-.--.");
                break;
            case ')':
                printf(")");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-.--.-");
                break;
            case '+':
                printf("+");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".-.-.");
                break;
            case ',':
                printf(",");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "--..--");
                break;
            case '-':
                printf("-");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-....-");
                break;
            case '.':
                printf(".");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".-.-.-");
                break;
            case '/':
                printf("/");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-..-.");
                break;
            case '0':
                printf("0");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-----");
                break;
            case '1':
                printf("1");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".----");
                break;
            case '2':
                printf("2");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "..---");
                break;
            case '3':
                printf("3");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "...--");
                break;
            case '4':
                printf("4");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "....-");
                break;
            case '5':
                printf("5");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".....");
                break;
            case '6':
                printf("6");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-....");
                break;
            case '7':
                printf("7");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "--...");
                break;
            case '8':
                printf("8");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "---..");
                break;
            case '9':
                printf("9");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "----.");
                break;
            case ':':
                printf(":");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "---...");
                break;
            case ';':
                printf(";");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-.-.-.");
                break;
            case '=':
                printf("=");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-...-");
                break;
            case '?':
                printf("?");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "..--..");
                break;
            case '@':
                printf("@");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".--.-.");
                break;
            case 'A':
            case 'a':
                printf("A");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".-");
                break;
            case 'B':
            case 'b':
                printf("B");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-...");
                break;
            case 'C':
            case 'c':
                printf("C");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-.-.");
                break;
            case 'D':
            case 'd':
                printf("D");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-..");
                break;
            case 'E':
            case 'e':
                printf("E");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".");
                break;
            case 'F':
            case 'f':
                printf("F");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "..-.");
                break;
            case 'G':
            case 'g':
                printf("G");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "--.");
                break;
            case 'H':
            case 'h':
                printf("H");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "....");
                break;
            case 'I':
            case 'i':
                printf("I");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "..");
                break;
            case 'J':
            case 'j':
                printf("J");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".---");
                break;
            case 'K':
            case 'k':
                printf("K");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-.-");
                break;
            case 'L':
            case 'l':
                printf("L");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".-..");
                break;
            case 'M':
            case 'm':
                printf("M");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "--");
                break;
            case 'N':
            case 'n':
                printf("N");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-.");
                break;
            case 'O':
            case 'o':
                printf("O");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "---");
                break;
            case 'P':
            case 'p':
                printf("P");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".--.");
                break;
            case 'Q':
            case 'q':
                printf("Q");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "--.-");
                break;
            case 'R':
            case 'r':
                printf("R");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".-.");
                break;
            case 'S':
            case 's':
                printf("S");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "...");
                break;
            case 'T':
            case 't':
                printf("T");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-");
                break;
            case 'U':
            case 'u':
                printf("U");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "..-");
                break;
            case 'V':
            case 'v':
                printf("V");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "...-");
                break;
            case 'W':
            case 'w':
                printf("W");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, ".--");
                break;
            case 'X':
            case 'x':
                printf("X");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-..-");
                break;
            case 'Y':
            case 'y':
                printf("Y");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "-.--");
                break;
            case 'Z':
            case 'z':
                printf("Z");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "--..");
                break;
            case '_':
                printf("_");
                fflush(stdout);  // Force character output
                signal_char_code(devhandle, "..--.-");
                break;
        }
        if (err_level != EXIT_SUCCESS)  // If one or more errors are detected
            break;  // Break the cycle
    }
}
