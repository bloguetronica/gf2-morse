/* GF2 Morse Command - Version 2.0 for Debian Linux
   Copyright (c) 2020-2024 Samuel Lourenço

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
#include <cstdlib>
#include <iostream>

#include <stdbool.h>  // Revise
#include <string.h>  // Revise
#include <unistd.h>  // Revise
#include <libusb-1.0/libusb.h>  // Revise

#include <string>
#include "error.h"
#include "gf2device.h"


// Global variables
int EXIT_USERERR = 2;  // Exit status value to indicate a command usage error
int TUNIT = 50000;  // Time unit in us

// Function prototypes
void signalCharCode(GF2Device &device, char *code, int &errcnt, std::string &errstr);
void signalMessage(GF2Device &device, char *message, int &errcnt, std::string &errstr);

int main(int argc, char **argv)
{
    int err, errlvl = EXIT_SUCCESS;
    if (argc < 2) {  // If the program was called without arguments
        std::cerr << "Error: Missing argument.\nUsage: gf2-morse MESSAGE\n";
        errlvl = EXIT_USERERR;
    } else {
        GF2Device device;
        if (argc < 3) {  // If no serial number was specified
            err = device.open();  // Open a device and get the device handle
        } else {  // Serial number was specified as a second (optional) argument
            err = device.open(argv[2]);  // Open the device having the specified serial number, and get the device handle
        }
        if (err == GF2Device::SUCCESS) {  // Device was successfully opened
            int errcnt = 0;
            std::string errstr;
            if (!device.isWaveGenEnabled(errcnt, errstr) && errcnt == 0) {  // Check if the waveform generator is enabled (errcnt can increment as a consequence of that verification, hence the need for " && errcnt == 0" in order to avoid misleading messages)
                std::cerr << "Error: Waveform generator is stopped and should be running.\nPlease invoke gf2-start and try again.\n";
            } else if (device.isDACEnabled(errcnt, errstr) && errcnt == 0) {  // Check if the DAC internal to the AD9834 waveform generator is enabled (again, the same precaution is needed)
                std::cerr << "Error: Waveform generator DAC is enabled and should be disabled.\nPlease invoke gf2-dacoff and try again.\n";
            } else if (errcnt == 0) {  // If all goes well so far
                std::cout << "Signaling message...\n";
                signalMessage(device, argv[1], errcnt, errstr);
                if (errcnt == 0) {  // Operation successful
                    std::cout << "Message signaled.\n";
                }
            }
            if (errcnt > 0) {  // In case of error
                if (device.disconnected()) {  // If the device disconnected
                    std::cerr << "Error: Device disconnected.\n";
                } else {
                    printErrors(errstr);
                }
                errlvl = EXIT_FAILURE;
            }
            device.close();
        } else {  // Failed to open device
            if (err == GF2Device::ERROR_INIT) {  // Failed to initialize libusb
                std::cerr << "Error: Could not initialize libusb\n";
            } else if (err == GF2Device::ERROR_NOT_FOUND) {  // Failed to find device
                std::cerr << "Error: Could not find device.\n";
            } else if (err == GF2Device::ERROR_BUSY) {  // Failed to claim interface
                std::cerr << "Error: Device is currently unavailable.\n";
            }
            errlvl = EXIT_FAILURE;
        }
    }
    return errlvl;
}

void signalCharCode(GF2Device &device, char *code, int &errcnt, std::string &errstr)  // Signals character code (adding a trailing inter-character space)
{
    size_t len = strlen(code);
    for (size_t i = 0; i < len; ++i) {
        if (code[i] == '.' || code [i] == '-') {  // This condition is only required for sanity purposes
            device.setDACEnabled(true, errcnt, errstr);  // Enable the AD9834 internal DAC
            usleep(TUNIT);  // Corresponds to a "dot"
            if (code[i] == '-') {
                usleep(2 * TUNIT);  // Adds to the previous time interval so it corresponds to a "dash"
            }
            device.setDACEnabled(false, errcnt, errstr);  // Disable the AD9834 internal DAC
            usleep(TUNIT);  // Corresponds to an intra-character space
        }
    }
    usleep(2 * TUNIT);  // Inter-character space
}

void signalMessage(GF2Device &device, char *message, int &errcnt, std::string &errstr)  // Signals message
{
    size_t len = strlen(message);
    for (size_t i = 0; i < len; ++i) {
        switch (message[i]) {  // Note that, at this point, only the characters listed below will be signaled!
            case '\n':
            case ' ':
                if (i > 0 && message[i - 1] != '\n' && message[i - 1] != ' ')  // Extra spaces omitted!
                {
                    std::cout << " " << std::flush;
                    usleep(4 * TUNIT);  // Word space
                }
                break;
            case '!':
                std::cout << "!" << std::flush;
                signalCharCode(device, "-.-.--", errcnt, errstr);
                break;
            case '"':
                std::cout << "\"" << std::flush;
                signalCharCode(device, ".-..-.", errcnt, errstr);
                break;
            case '$':
                std::cout << "$" << std::flush;
                signalCharCode(device, "...-..-", errcnt, errstr);
                break;
            case '&':
                std::cout << "&" << std::flush;
                signalCharCode(device, ".-...", errcnt, errstr);
                break;
            case '\'':
                std::cout << "'" << std::flush;
                signalCharCode(device, ".----.", errcnt, errstr);
                break;
            case '(':
                std::cout << "(" << std::flush;
                signalCharCode(device, "-.--.", errcnt, errstr);
                break;
            case ')':
                std::cout << ")" << std::flush;
                signalCharCode(device, "-.--.-", errcnt, errstr);
                break;
            case '+':
                std::cout << "+" << std::flush;
                signalCharCode(device, ".-.-.", errcnt, errstr);
                break;
            case ',':
                std::cout << "," << std::flush;
                signalCharCode(device, "--..--", errcnt, errstr);
                break;
            case '-':
                std::cout << "-" << std::flush;
                signalCharCode(device, "-....-", errcnt, errstr);
                break;
            case '.':
                std::cout << "." << std::flush;
                signalCharCode(device, ".-.-.-", errcnt, errstr);
                break;
            case '/':
                std::cout << "/" << std::flush;
                signalCharCode(device, "-..-.", errcnt, errstr);
                break;
            case '0':
                std::cout << "0" << std::flush;
                signalCharCode(device, "-----", errcnt, errstr);
                break;
            case '1':
                std::cout << "1" << std::flush;
                signalCharCode(device, ".----", errcnt, errstr);
                break;
            case '2':
                std::cout << "2" << std::flush;
                signalCharCode(device, "..---", errcnt, errstr);
                break;
            case '3':
                std::cout << "3" << std::flush;
                signalCharCode(device, "...--", errcnt, errstr);
                break;
            case '4':
                std::cout << "4" << std::flush;
                signalCharCode(device, "....-", errcnt, errstr);
                break;
            case '5':
                std::cout << "5" << std::flush;
                signalCharCode(device, ".....", errcnt, errstr);
                break;
            case '6':
                std::cout << "6" << std::flush;
                signalCharCode(device, "-....", errcnt, errstr);
                break;
            case '7':
                std::cout << "7" << std::flush;
                signalCharCode(device, "--...", errcnt, errstr);
                break;
            case '8':
                std::cout << "8" << std::flush;
                signalCharCode(device, "---..", errcnt, errstr);
                break;
            case '9':
                std::cout << "9" << std::flush;
                signalCharCode(device, "----.", errcnt, errstr);
                break;
            case ':':
                std::cout << ":" << std::flush;
                signalCharCode(device, "---...", errcnt, errstr);
                break;
            case ';':
                std::cout << ";" << std::flush;
                signalCharCode(device, "-.-.-.", errcnt, errstr);
                break;
            case '=':
                std::cout << "=" << std::flush;
                signalCharCode(device, "-...-", errcnt, errstr);
                break;
            case '?':
                std::cout << "?" << std::flush;
                signalCharCode(device, "..--..", errcnt, errstr);
                break;
            case '@':
                std::cout << "@" << std::flush;
                signalCharCode(device, ".--.-.", errcnt, errstr);
                break;
            case 'A':
            case 'a':
                std::cout << "A" << std::flush;
                signalCharCode(device, ".-", errcnt, errstr);
                break;
            case 'B':
            case 'b':
                std::cout << "B" << std::flush;
                signalCharCode(device, "-...", errcnt, errstr);
                break;
            case 'C':
            case 'c':
                std::cout << "C" << std::flush;
                signalCharCode(device, "-.-.", errcnt, errstr);
                break;
            case 'D':
            case 'd':
                std::cout << "D" << std::flush;
                signalCharCode(device, "-..", errcnt, errstr);
                break;
            case 'E':
            case 'e':
                std::cout << "E" << std::flush;
                signalCharCode(device, ".", errcnt, errstr);
                break;
            case 'F':
            case 'f':
                std::cout << "F" << std::flush;
                signalCharCode(device, "..-.", errcnt, errstr);
                break;
            case 'G':
            case 'g':
                std::cout << "G" << std::flush;
                signalCharCode(device, "--.", errcnt, errstr);
                break;
            case 'H':
            case 'h':
                std::cout << "H" << std::flush;
                signalCharCode(device, "....", errcnt, errstr);
                break;
            case 'I':
            case 'i':
                std::cout << "I" << std::flush;
                signalCharCode(device, "..", errcnt, errstr);
                break;
            case 'J':
            case 'j':
                std::cout << "J" << std::flush;
                signalCharCode(device, ".---", errcnt, errstr);
                break;
            case 'K':
            case 'k':
                std::cout << "K" << std::flush;
                signalCharCode(device, "-.-", errcnt, errstr);
                break;
            case 'L':
            case 'l':
                std::cout << "L" << std::flush;
                signalCharCode(device, ".-..", errcnt, errstr);
                break;
            case 'M':
            case 'm':
                std::cout << "M" << std::flush;
                signalCharCode(device, "--", errcnt, errstr);
                break;
            case 'N':
            case 'n':
                std::cout << "N" << std::flush;
                signalCharCode(device, "-.", errcnt, errstr);
                break;
            case 'O':
            case 'o':
                std::cout << "O" << std::flush;
                signalCharCode(device, "---", errcnt, errstr);
                break;
            case 'P':
            case 'p':
                std::cout << "P" << std::flush;
                signalCharCode(device, ".--.", errcnt, errstr);
                break;
            case 'Q':
            case 'q':
                std::cout << "Q" << std::flush;
                signalCharCode(device, "--.-", errcnt, errstr);
                break;
            case 'R':
            case 'r':
                std::cout << "R" << std::flush;
                signalCharCode(device, ".-.", errcnt, errstr);
                break;
            case 'S':
            case 's':
                std::cout << "S" << std::flush;
                signalCharCode(device, "...", errcnt, errstr);
                break;
            case 'T':
            case 't':
                std::cout << "T" << std::flush;
                signalCharCode(device, "-", errcnt, errstr);
                break;
            case 'U':
            case 'u':
                std::cout << "U" << std::flush;
                signalCharCode(device, "..-", errcnt, errstr);
                break;
            case 'V':
            case 'v':
                std::cout << "V" << std::flush;
                signalCharCode(device, "...-", errcnt, errstr);
                break;
            case 'W':
            case 'w':
                std::cout << "W" << std::flush;
                signalCharCode(device, ".--", errcnt, errstr);
                break;
            case 'X':
            case 'x':
                std::cout << "X" << std::flush;
                signalCharCode(device, "-..-", errcnt, errstr);
                break;
            case 'Y':
            case 'y':
                std::cout << "Y" << std::flush;
                signalCharCode(device, "-.--", errcnt, errstr);
                break;
            case 'Z':
            case 'z':
                std::cout << "Z" << std::flush;
                signalCharCode(device, "--..", errcnt, errstr);
                break;
            case '_':
                std::cout << "_" << std::flush;
                signalCharCode(device, "..--.-", errcnt, errstr);
                break;
        }
        if (errcnt != 0) {  // If one or more errors are detected
            break;  // Break the cycle
        }
    }
    std::cout << "\n";
}
