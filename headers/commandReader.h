#ifndef UTOTPARSER_COMMAND_H
#define UTOTPARSER_COMMAND_H

// Copyright (C) 2021 Remy van Elst
//
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <array>
#include <ostream>
#include <string>

#ifdef _WIN32
#include <stdio.h>
#endif

#define BUFFER_SIZE 8


class Command {
public:
    std::string output;

    /**
     * Execute system command and get STDOUT result.
     * Regular system() only gives back exit status, this gives back output as well.
     * @param command system command to execute.
     * @return STDOUT (not stderr) output. of command. Empty if command failed (or has no output).
     * If you want stderr, use shell redirection (2&>1).
     */
    static std::string exec(const std::string &command) {
        std::array<char, BUFFER_SIZE> buffer{};
        std::string result;

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

        FILE *pipe = popen(command.c_str(), "r");
        if (pipe == nullptr)
            throw std::runtime_error("popen() failed!");
        try {
            std::size_t bytesRead;
            while ((bytesRead = std::fread(buffer.data(), sizeof(buffer.at(0)), sizeof(buffer), pipe)) != 0)
                result += std::string(buffer.data(), bytesRead);
        } catch (...) {
            pclose(pipe);
            throw;
        }
        return result;
    }

};

#endif //UTOTPARSER_COMMAND_H