#ifndef UTOTPARSER_LOGGER_H
#define UTOTPARSER_LOGGER_H

#include <chrono>
#include <utility>

#include "utilities/StringsGetter.hpp"


class Logger {

private:
    // The path in which the logger will write logs.
    std::string logDirPath {};

    // The name of the log file to be written.
    std::string logFileName {};

    // The combination of logDirPath and logFileName.
    std::string logPath {};


    /**
     * Method used to get the current date and time in order to create a timestamp for logs.
     * @return the current date and time.
     */
    static std::string getCurDateTime()
    {
        // Get the current time point.
        auto now = std::chrono::system_clock::now();
        // Convert the time point to a time_t (C-style time).
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        // Convert time_t to a string representation.
        std::string time_str = std::ctime(&current_time);
        return time_str;
    }


public:
    Logger(std::string logDirPath, std::string logFileName) : logDirPath(std::move(logDirPath)), logFileName(std::move(logFileName))
    {
        this->logPath = this->logDirPath + "/" + this->logFileName;
        clearLogs();
        writeLog("Default logger created as: " + logPath);
    }


    /**
     * Method used to clear the log file written by the logger.
     */
    void clearLogs()
    {
        // In order to clean logging files, we open them in truncating mode.
        std::ofstream file(logPath, std::ofstream::out | std::ofstream::trunc);
        file.close();
    }


    /**
     * Method used to write a log.
     * @param log the log that has to be written.
     * @param spacing additional spacing to add before writing the log.
     */
    void writeLog(const std::string &log, uint16_t spacing = 0)
    {
        std::ofstream out;
        out.open(logPath, std::ofstream::out | std::ofstream::app);

        out << std::string(spacing, '\n');
        out << "Log: " << logFileName << " ::: Log written at " << getCurDateTime();
        out << log;

        out.flush();
        out.close();
    }

};


#endif //UTOTPARSER_LOGGER_H
