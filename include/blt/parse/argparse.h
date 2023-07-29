/*
 * Created by Brett on 28/07/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_ARGPARSE_H
#define BLT_TESTS_ARGPARSE_H

#include <vector>
#include <string>
#include <initializer_list>
#include <optional>
#include <blt/std/hashmap.h>

namespace blt::parser {
    
    enum class arg_action {
        STORE,
        STORE_CONST,
        STORE_TRUE,
        STORE_FALSE,
        APPEND,
        APPEND_CONST,
        COUNT,
        HELP,
        VERSION,
        EXTEND
    };
    
    class arg_t {
        private:
        
        public:
        
    };
    
    class arg_vector {
        private:
            std::vector<std::string> names;
            std::vector<std::string> flags;
            
            void insertAndSort(const std::string& str);
        
        public:
            arg_vector() = default;
            
            arg_vector(const std::vector<std::string>& args);
            
            arg_vector(std::initializer_list<std::string> args);
            
            arg_vector(const std::string& arg);
            
            arg_vector(const char* arg);
            
            arg_vector& operator=(const std::string& arg);
            
            arg_vector& operator=(const char* arg);
            
            arg_vector& operator=(std::initializer_list<std::string>& args);
            
            arg_vector& operator=(std::vector<std::string>& args);
            
            [[nodiscard]] inline std::vector<std::string>& getNames() {
                return names;
            }
            
            [[nodiscard]] inline std::vector<std::string>& getFlags() {
                return flags;
            }
            
            [[nodiscard]] inline const std::vector<std::string>& getNames() const {
                return names;
            }
            
            [[nodiscard]] inline const std::vector<std::string>& getFlags() const {
                return flags;
            }
    };
    
    class arg_nargs {
        private:
            static constexpr int UNKNOWN = 0x1;
            static constexpr int ALL = 0x2;
            static constexpr int ALL_REQUIRED = 0x4;
            int args = 0;
            int flags = 0;
            
            void decode(char c);
        
        public:
            arg_nargs() = default;
            
            arg_nargs(int args): args(args) {}
            
            arg_nargs(char c);
            
            arg_nargs(std::string s);
            
            arg_nargs(const char* s);
            
            arg_nargs& operator=(const std::string& s);
            
            arg_nargs& operator=(const char* s);
            
            arg_nargs& operator=(char c);
            
            arg_nargs& operator=(int args);
    };
    
    struct arg_properties {
        private:
        public:
            arg_vector a_flags;
            arg_action a_action;
            arg_nargs a_nargs;
            std::optional<std::string> a_const;
            std::string a_default;
            std::string a_def;
            std::string a_help;
            std::string a_metavar;
            bool a_required = false;
    };
    
    class arg_tokenizer {
        private:
            static constexpr char FLAG = '-';
            std::vector<std::string> args;
            size_t nextIndex = 0;
            
            inline const std::string& get(size_t i){
                return args[i];
            }
            inline const std::string& next(size_t& i){
                return args[i++];
            }
            inline bool hasNext(size_t i){
                return (size_t)i < args.size();
            }
            inline bool isFlag(size_t i){
                return get(i).starts_with(FLAG);
            }
        public:
            arg_tokenizer() = default;
            
            arg_tokenizer(const char** argv, size_t argc);
            
            inline void forward() {
                nextIndex++;
            }
            
            inline const std::string& get() {
                return get(nextIndex);
            }
            
            inline const std::string& next() {
                return next(nextIndex);
            }
            
            inline bool hasNext() {
                return hasNext(nextIndex);
            }
            
            inline bool isFlag() {
                return isFlag(nextIndex);
            }
    };
    
    struct arg_results {
        private:
        
        public:
        
    };
    
    class argparse {
        private:
            arg_tokenizer tokenizer;
            
            static bool validateArgument(const arg_properties& args);
        public:
            argparse() = default;
            
            void addArgument(const arg_properties& args);
        
    };
    
}

#endif //BLT_TESTS_ARGPARSE_H
