#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip> 
#include <sstream> 
#include <fstream> 


#define LOG_TO_FILE "simulation.log" 

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};


inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "[DEBUG]";
        case LogLevel::INFO:    return "[INFO] "; 
        case LogLevel::WARNING: return "[WARN] ";
        case LogLevel::ERROR:   return "[ERROR]";
        default:                return "[UNKNW]";
    }
}

constexpr LogLevel MIN_LOG_LEVEL_CONSOLE = LogLevel::WARNING;
#ifdef LOG_TO_FILE
constexpr LogLevel MIN_LOG_LEVEL_FILE = LogLevel::INFO;
extern std::ofstream log_file_stream;
#endif


class Logger {
public:
    static void init() {
    #ifdef LOG_TO_FILE
        log_file_stream.open(LOG_TO_FILE, std::ios::app);
        if (!log_file_stream.is_open()) {
            std::cerr << "Failed to open log file: " << LOG_TO_FILE << std::endl;
        } else {
            log(LogLevel::INFO, "Logger initialized. Logging to file: " LOG_TO_FILE);
        }
    #endif
        log(LogLevel::INFO, "Logger initialized for console output.");
    }

    static void shutdown() {
    #ifdef LOG_TO_FILE
        if (log_file_stream.is_open()) {
            log(LogLevel::INFO, "Logger shutting down.");
            log_file_stream.close();
        }
    #endif
    }

    template<typename... Args>
    static void log(LogLevel level, const Args&... args) {
        std::ostringstream oss;
        (oss << ... << args); 
        std::string message = oss.str();

        if (level >= MIN_LOG_LEVEL_CONSOLE) {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            #ifdef _MSC_VER 
            std::tm timeinfo_tm;
            localtime_s(&timeinfo_tm, &in_time_t);
            std::cout << std::put_time(&timeinfo_tm, "%Y-%m-%d %H:%M:%S") << " "
                      << logLevelToString(level) << " " << message << std::endl;
            #else
            std::cout << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << " " << logLevelToString(level) << " " << message << std::endl;
            #endif
        }

    #ifdef LOG_TO_FILE
        if (log_file_stream.is_open() && level >= MIN_LOG_LEVEL_FILE) {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            #ifdef _MSC_VER
            std::tm timeinfo_tm_file;
            localtime_s(&timeinfo_tm_file, &in_time_t);
            log_file_stream << std::put_time(&timeinfo_tm_file, "%Y-%m-%d %H:%M:%S") << " " << logLevelToString(level) << " " << message << std::endl;
            #else
            log_file_stream << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << " " << logLevelToString(level) << " " << message << std::endl;
            #endif
        }
    #endif
    }

    template<typename... Args> static void debug(const Args&... args) { log(LogLevel::DEBUG, args...); }
    template<typename... Args> static void info(const Args&... args) { log(LogLevel::INFO, args...); }
    template<typename... Args> static void warn(const Args&... args) { log(LogLevel::WARNING, args...); }
    template<typename... Args> static void error(const Args&... args) { log(LogLevel::ERROR, args...); }
};