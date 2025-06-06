
#pragma once
#include <iostream>
#include <string>
#include <format>
#include <fstream>
#include <sstream>
#include <mutex>
#include <ctime>

namespace hkp {
    class Logger {
    public:
        enum Mode {
            DEBUG,
            RELEASE
        };

        enum Level {
            INFO,
            WARNING,
            ERROR,
            CRITICAL
        };

    private:
        Mode _mode;
        std::ofstream _file;
        std::mutex _mutex;

    public:
        Logger(Mode mode) : _mode(mode) {
            if (_mode == Mode::RELEASE) {
                _file.open("log.txt", std::ios::out | std::ios::app);
                if (!_file.is_open()) {
                    std::cerr << "Unable to open log file!" << std::endl;
                }
            }
        }

        ~Logger() {
            if (_file.is_open()) {
                _file.close();
            }
        }

        void log(Level level, const std::string& msg) {
            std::lock_guard<std::mutex> lock(_mutex);

            std::time_t t = std::time(nullptr);
            char mbstr[18];
            std::strftime(mbstr, sizeof(mbstr), "%d %T  %Y", std::localtime(&t));

            std::ostringstream oss;

            if (_mode == Mode::DEBUG) {
                switch (level) {
                    case Level::INFO:
                        oss << std::format("[{}] INFO - {}\n", mbstr, msg);
                    break;
                    case Level::WARNING:
                        oss << std::format("\x1b[33m[{}] WARNING - {}\n", mbstr, msg);
                    break;
                    case Level::ERROR:
                        oss << std::format("\x1b[31m[{}] ERROR - {}\n", mbstr, msg);
                    break;
                    case Level::CRITICAL:
                        oss << std::format("\n\x1b[40;31m[{}] CRITICAL - {}\n", mbstr, msg);
                    break;
                }
                std::cout << oss.str();

            } else if (_mode == Mode::RELEASE && _file.is_open()) {
                switch (level) {
                    case Level::INFO:
                        oss << std::format("[{}] INFO - {}\n", mbstr, msg);
                    break;
                    case Level::WARNING:
                        oss << std::format("[{}] WARNING - {}\n", mbstr, msg);
                    break;
                    case Level::ERROR:
                        oss << std::format("    [{}] ERROR - {}\n", mbstr, msg);
                    break;
                    case Level::CRITICAL:
                        oss << std::format("\n[{}] CRITICAL - {}\n", mbstr, msg);
                    break;
                }
                _file << oss.str();

            }

        }

    };
}
