/*
 * Created by Brett on 20/07/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/logging.h>

#include <iostream>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <thread>
#include <cstdarg>
#include <iostream>
#include <vector>
#if defined(CXX17_FILESYSTEM) || defined (CXX17_FILESYSTEM_LIBFS)
        #include <filesystem>
#elif defined(CXX11_EXP_FILESYSTEM) || defined (CXX11_EXP_FILESYSTEM_LIBFS)
        #include <experimental/filesystem>
#else
#include <filesystem>
#endif
#include <ios>
#include <fstream>

template <typename K, typename V>
using hashmap = std::unordered_map<K, V>;

namespace blt::logging
{
    /**
     * Used to store fast associations between built in tags and their respective values
     */
    class tag_map
    {
    private:
        tag* tags;
        size_t size;

        [[nodiscard]] static inline size_t hash(const tag& t)
        {
            size_t h = t.tag[1] * 3 - t.tag[0];
            return h - 100;
        }

        // TODO: fix
        void expand()
        {
            auto newSize = size * 2;
            auto newTags = new tag[newSize];
            for (size_t i = 0; i < size; i++)
                newTags[i] = tags[i];
            delete[] tags;
            tags = newTags;
            size = newSize;
        }

    public:
        tag_map(std::initializer_list<tag> initial_tags)
        {
            size_t max = 0;
            for (const auto& t : initial_tags)
                max = std::max(max, hash(t));
            tags = new tag[(size = max + 1)];
            for (const auto& t : initial_tags)
                insert(t);
        }

        tag_map(const tag_map& copy)
        {
            tags = new tag[(size = copy.size)];
            for (size_t i = 0; i < size; i++)
                tags[i] = copy.tags[i];
        }

        void insert(const tag& t)
        {
            auto h = hash(t);
            //if (h > size)
            //    expand();
            if (!tags[h].tag.empty())
                std::cerr << "Tag not empty! " << tags[h].tag << "!!!\n";
            tags[h] = t;
        }

        tag& operator[](const std::string& name) const
        {
            auto h = hash(tag{name, nullptr});
            if (h > size)
                std::cerr << "Tag out of bounds";
            return tags[h];
        }

        ~tag_map()
        {
            delete[] tags;
            tags = nullptr;
        }
    };

    class LogFileWriter
    {
    private:
        std::string m_path;
        std::fstream* output = nullptr;

    public:
        explicit LogFileWriter() = default;

        void writeLine(const std::string& path, const std::string& line)
        {
            if (path != m_path || output == nullptr)
            {
                clear();
                delete output;
                output = new std::fstream(path, std::ios::out | std::ios::app);
                if (!output->good())
                {
                    throw std::runtime_error("Unable to open console filestream!\n");
                }
            }
            if (!output->good())
            {
                std::cerr << "There has been an error in the logging file stream!\n";
                output->clear();
            }
            *output << line;
        }

        void clear()
        {
            if (output != nullptr)
            {
                try
                {
                    output->flush();
                    output->close();
                }
                catch (std::exception& e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
        }

        ~LogFileWriter()
        {
            clear();
            delete(output);
        }
    };

#ifdef WIN32
    #define BLT_NOW() auto t = std::time(nullptr); tm now{}; localtime_s(&now, &t)
#else
#define BLT_NOW() auto t = std::time(nullptr); auto now_ptr = std::localtime(&t); auto& now = *now_ptr
#endif

    //#define BLT_NOW() auto t = std::time(nullptr); tm now; localtime_s(&now, &t); //auto now = std::localtime(&t)
#define BLT_ISO_YEAR(S) auto S = std::to_string(now.tm_year + 1900); \
        S += '-'; \
        S += ensureHasDigits(now.tm_mon+1, 2); \
        S += '-'; \
        S += ensureHasDigits(now.tm_mday, 2);
#define BLT_CUR_TIME(S) auto S = ensureHasDigits(now.tm_hour, 2); \
        S += ':'; \
        S += ensureHasDigits(now.tm_min, 2); \
        S += ':'; \
        S += ensureHasDigits(now.tm_sec, 2);

    static inline std::string ensureHasDigits(int current, int digits)
    {
        std::string asString = std::to_string(current);
        auto length = digits - asString.length();
        if (length <= 0)
            return asString;
        std::string zeros;
        zeros.reserve(length);
        for (unsigned int i = 0; i < length; i++)
        {
            zeros += '0';
        }
        return zeros + asString;
    }

    log_format loggingFormat{};
    hashmap<std::thread::id, std::string> loggingThreadNames;
    hashmap<std::thread::id, hashmap<blt::logging::log_level, std::string>> loggingStreamLines;
    LogFileWriter writer;

    const std::unique_ptr<tag_map> tagMap = std::make_unique<tag_map>(tag_map{
        {
            "YEAR", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                return std::to_string(now.tm_year);
            }
        },
        {
            "MONTH", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                return ensureHasDigits(now.tm_mon + 1, 2);
            }
        },
        {
            "DAY", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                return ensureHasDigits(now.tm_mday, 2);
            }
        },
        {
            "HOUR", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                return ensureHasDigits(now.tm_hour, 2);
            }
        },
        {
            "MINUTE", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                return ensureHasDigits(now.tm_min, 2);
            }
        },
        {
            "SECOND", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                return ensureHasDigits(now.tm_sec, 2);
            }
        },
        {
            "MS", [](const tag_func_param&) -> std::string
            {
                return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now().time_since_epoch()).count()
                );
            }
        },
        {
            "NS", [](const tag_func_param&) -> std::string
            {
                return std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::high_resolution_clock::now().time_since_epoch()).count()
                );
            }
        },
        {
            "ISO_YEAR", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                BLT_ISO_YEAR(returnStr);
                return returnStr;
            }
        },
        {
            "TIME", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                BLT_CUR_TIME(returnStr);
                return returnStr;
            }
        },
        {
            "FULL_TIME", [](const tag_func_param&) -> std::string
            {
                BLT_NOW();
                BLT_ISO_YEAR(ISO);
                BLT_CUR_TIME(TIME);
                ISO += ' ';
                ISO += TIME;
                return ISO;
            }
        },
        {
            "LF", [](const tag_func_param& f) -> std::string
            {
                return loggingFormat.levelColors[(int)f.level];
            }
        },
        {
            "ER", [](const tag_func_param&) -> std::string
            {
                return loggingFormat.levelColors[(int)log_level::ERROR];
            }
        },
        {
            "CNR", [](const tag_func_param& f) -> std::string
            {
                return f.level >= log_level::ERROR ? loggingFormat.levelColors[(int)log_level::ERROR] : "";
            }
        },
        {
            "RC", [](const tag_func_param&) -> std::string
            {
                return "\033[0m";
            }
        },
        {
            "LOG_LEVEL", [](const tag_func_param& f) -> std::string
            {
                return loggingFormat.levelNames[(int)f.level];
            }
        },
        {
            "THREAD_NAME", [](const tag_func_param&) -> std::string
            {
                if (loggingThreadNames.find(std::this_thread::get_id()) == loggingThreadNames.end())
                    return "UNKNOWN";
                return loggingThreadNames[std::this_thread::get_id()];
            }
        },
        {
            "FILE", [](const tag_func_param& f) -> std::string
            {
                return f.file;
            }
        },
        {
            "LINE", [](const tag_func_param& f) -> std::string
            {
                return f.line;
            }
        },
        {
            "RAW_STR", [](const tag_func_param& f) -> std::string
            {
                return f.raw_string;
            }
        },
        {
            "STR", [](const tag_func_param& f) -> std::string
            {
                return f.formatted_string;
            }
        }
    });

    static inline std::vector<std::string> split(std::string s, const std::string& delim)
    {
        size_t pos = 0;
        std::vector<std::string> tokens;
        while ((pos = s.find(delim)) != std::string::npos)
        {
            auto token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + delim.length());
        }
        tokens.push_back(s);
        return tokens;
    }

    inline std::string filename(const std::string& path)
    {
        if (loggingFormat.printFullFileName)
            return path;
        auto paths = split(path, "/");
        auto final = paths[paths.size() - 1];
        if (final == "/")
            return paths[paths.size() - 2];
        return final;
    }

    class string_parser
    {
    private:
        std::string _str;
        size_t _pos;

    public:
        explicit string_parser(std::string str): _str(std::move(str)), _pos(0)
        {
        }

        inline char next()
        {
            return _str[_pos++];
        }

        [[nodiscard]] inline bool has_next() const
        {
            return _pos < _str.size();
        }
    };

    std::string stripANSI(const std::string& str)
    {
        string_parser parser(str);
        std::string out;
        while (parser.has_next())
        {
            char c = parser.next();
            if (c == '\033')
            {
                while (parser.has_next() && parser.next() != 'm');
            }
            else
                out += c;
        }
        return out;
    }

    void applyCFormatting(const std::string& format, std::string& output, std::va_list& args)
    {
        // args must be copied because they will be consumed by the first vsnprintf
        va_list args_copy;
        va_copy(args_copy, args);

        auto buffer_size = std::vsnprintf(nullptr, 0, format.c_str(), args_copy) + 1;
        auto* buffer = new char[static_cast<unsigned long>(buffer_size)];

        vsnprintf(buffer, buffer_size, format.c_str(), args);
        output = std::string(buffer);

        delete[] buffer;

        va_end(args_copy);
    }

    /**
     * Checks if the next character in the parser is a tag opening, if not output the buffer to the out string
     */
    inline bool tagOpening(string_parser& parser, std::string& out)
    {
        char c = ' ';
        if (parser.has_next() && (c = parser.next()) == '{')
            if (parser.has_next() && (c = parser.next()) == '{')
                return true;
            else
                out += c;
        else
            out += c;
        return false;
    }

    void parseString(string_parser& parser, std::string& out, const std::string& userStr, log_level level, const char* file, int line)
    {
        while (parser.has_next())
        {
            char c = parser.next();
            std::string nonTag;
            if (c == '$' && tagOpening(parser, nonTag))
            {
                std::string tag;
                while (parser.has_next())
                {
                    c = parser.next();
                    if (c == '}')
                        break;
                    tag += c;
                }
                c = parser.next();
                if (parser.has_next() && c != '}')
                {
                    std::cerr << "Error processing tag, is not closed with two '}'!\n";
                    break;
                }
                if (loggingFormat.ensureAlignment && tag == "STR")
                {
                    auto currentOutputWidth = out.size();
                    auto& longestWidth = loggingFormat.currentWidth;
                    longestWidth = std::max(longestWidth, currentOutputWidth);
                    // pad with spaces
                    if (currentOutputWidth != longestWidth)
                    {
                        for (size_t i = currentOutputWidth; i < longestWidth; i++)
                            out += ' ';
                    }
                }
                tag_func_param param{
                    level, filename({file}), std::to_string(line), userStr, userStr
                };
                out += (*tagMap)[tag].func(param);
            }
            else
            {
                out += c;
                out += nonTag;
            }
        }
    }

    std::string applyFormatString(const std::string& str, log_level level, const char* file, int line)
    {
        // this can be speedup by preprocessing the string into an easily callable class
        // where all the variables are ready to be substituted in one step
        // and all static information already entered
        string_parser parser(loggingFormat.logOutputFormat);
        std::string out;
        parseString(parser, out, str, level, file, line);

        return out;
    }

    void log_internal(const std::string& format, log_level level, const char* file, int line, std::va_list& args)
    {
        std::string withoutLn = format;
        auto len = withoutLn.length();

        if (len > 0 && withoutLn[len - 1] == '\n')
            withoutLn = withoutLn.substr(0, len - 1);

        std::string out;

        applyCFormatting(withoutLn, out, args);

        if (level == log_level::NONE)
        {
            std::cout << out << std::endl;
            return;
        }

        std::string finalFormattedOutput = applyFormatString(out, level, file, line);

        if (loggingFormat.logToConsole)
            std::cout << finalFormattedOutput;


        if (loggingFormat.logToFile)
        {
            string_parser parser(loggingFormat.logFileName);
            std::string fileName;
            parseString(parser, fileName, withoutLn, level, file, line);

            auto path = loggingFormat.logFilePath;
            if (!path.empty() && path[path.length() - 1] != '/')
                path += '/';

            // if the file has changed (new day in default case) we should reset the rollover count
            if (loggingFormat.lastFile != fileName)
            {
                loggingFormat.currentRollover = 0;
                loggingFormat.lastFile = fileName;
            }

            path += fileName;
            path += '-';
            path += std::to_string(loggingFormat.currentRollover);
            path += ".log";

            if (std::filesystem::exists(path))
            {
                auto fileSize = std::filesystem::file_size(path);

                // will start on next file
                if (fileSize > loggingFormat.logMaxFileSize)
                    loggingFormat.currentRollover++;
            }

            writer.writeLine(path, stripANSI(finalFormattedOutput));
        }
        //std::cout.flush();
    }

    void log_stream_internal(const std::string& str, const logger& logger)
    {
        auto& s = loggingStreamLines[std::this_thread::get_id()][logger.level];
        //        s += str;
        for (char c : str)
        {
            s += c;
            if (c == '\n')
            {
                log(s, logger.level, logger.file, logger.line);
                s = "";
            }
        }
    }

    void setThreadName(const std::string& name)
    {
        loggingThreadNames[std::this_thread::get_id()] = name;
    }

    void setLogFormat(const log_format& format)
    {
        loggingFormat = format;
    }

    void setLogColor(log_level level, const std::string& newFormat)
    {
        loggingFormat.levelColors[(int)level] = newFormat;
    }

    void setLogName(log_level level, const std::string& newFormat)
    {
        loggingFormat.levelNames[(int)level] = newFormat;
    }

    void setLogOutputFormat(const std::string& newFormat)
    {
        loggingFormat.logOutputFormat = newFormat;
    }

    void setLogToFile(bool shouldLogToFile)
    {
        loggingFormat.logToFile = shouldLogToFile;
    }

    void setLogToConsole(bool shouldLogToConsole)
    {
        loggingFormat.logToConsole = shouldLogToConsole;
    }

    void setLogPath(const std::string& path)
    {
        loggingFormat.logFilePath = path;
    }

    void setLogFileName(const std::string& fileName)
    {
        loggingFormat.logFileName = fileName;
    }

    void setMaxFileSize(const size_t fileSize)
    {
        loggingFormat.logMaxFileSize = fileSize;
    }

    void flush()
    {
        std::cerr.flush();
        std::cout.flush();
    }
}
