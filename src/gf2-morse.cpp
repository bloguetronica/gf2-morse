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
#include <map>
#include <string>
#include <unistd.h>
#include "error.h"
#include "gf2device.h"

// Global variables
int EXIT_USERERR = 2;  // Exit status value to indicate a command usage error
int TUNIT = 50000;     // Time unit in us

// Function prototypes
void signalCharCode(GF2Device &device, const std::string &code, int &errcnt, std::string &errstr);
void signalMessage(GF2Device &device, const std::string &message, int &errcnt, std::string &errstr);

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

void signalCharCode(GF2Device &device, const std::string &code, int &errcnt, std::string &errstr)  // Signals character code (adding a trailing inter-character space)
{
    size_t strLength = code.size();
    for (size_t i = 0; i < strLength; ++i) {
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

void signalMessage(GF2Device &device, const std::string &message, int &errcnt, std::string &errstr)  // Signals message
{
    size_t strLength = message.size();
    for (size_t i = 0; i < strLength; ++i) {
        if ((message[i] == '\n' || message[i] == ' ') && i > 0 && message[i - 1] != '\n' && message[i - 1] != ' ') {  // Returns treated as spaces. Extra spaces and returns are to be omitted!
            std::cout << " " << std::flush;
            usleep(4 * TUNIT);  // Word space
        } else {
            char character = message[i];
            if (character >= 'a' && character <= 'z') {  // If lowercase
                character -= 32;  // Convert to uppercase
            }
            std::map<char, std::string> charCodes{
                {'!', "-.-.--"},
                {'"', ".-..-."},
                {'$', "...-..-"},
                {'&', ".-..."},
                {'\'', ".----."},
                {'(', "-.--."},
                {')', "-.--.-"},
                {'+', ".-.-."},
                {',', "--..--"},
                {'-', "-....-"},
                {'.', ".-.-.-"},
                {'/', "-..-."},
                {'0', "-----"},
                {'1', ".----"},
                {'2', "..---"},
                {'3', "...--"},
                {'4', "....-"},
                {'5', "....."},
                {'6', "-...."},
                {'7', "--..."},
                {'8', "---.."},
                {'9', "----."},
                {':', "---..."},
                {';', "-.-.-."},
                {'=', "-...-"},
                {'?', "..--.."},
                {'@', ".--.-."},
                {'A', ".-"},
                {'B', "-..."},
                {'C', "-.-."},
                {'D', "-.."},
                {'E', "."},
                {'F', "..-."},
                {'G', "--."},
                {'H', "...."},
                {'I', ".."},
                {'J', ".---"},
                {'K', "-.-"},
                {'L', ".-.."},
                {'M', "--"},
                {'N', "-."},
                {'O', "---"},
                {'P', ".--."},
                {'Q', "--.-"},
                {'R', ".-."},
                {'S', "..."},
                {'T', "-"},
                {'U', "..-"},
                {'V', "...-"},
                {'W', ".--"},
                {'X', "-..-"},
                {'Y', "-.--"},
                {'Z', "--.."},
                {'_', "..--.-"}
            };
            if (charCodes.count(character) > 0) {  // If character exists
                std::cout << character << std::flush;  // Print character immediately!
                signalCharCode(device, charCodes[character], errcnt, errstr);
            }
        }
        if (errcnt != 0) {  // If one or more errors are detected
            break;  // Break the cycle
        }
    }
    std::cout << "\n";
}
