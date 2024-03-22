/* Error handling functions - Version 1.0.0
   Copyright (c) 2022 Samuel Lourenço

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
#include <iostream>
#include "error.h"

// Prints errors in separated lines (each error should be terminated with a newline character)
void printErrors(const std::string &errstr)
{
    size_t lnend, lnstart = 0;
    while ((lnend = errstr.find('\n', lnstart)) != std::string::npos) {
        std::cerr << "Error: " << errstr.substr(lnstart, lnend - lnstart + 1);  // This includes the newline character
        lnstart = lnend + 1;
    }
}
