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
#include <variant>
#include <blt/std/logging.h>

namespace blt::parser {
    
    enum class arg_action_t {
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
    
    enum class arg_result_t {
        BOOL,
        VALUE,
        VECTOR
    };
    
    class arg_vector_t {
        private:
            std::vector<std::string> names;
            std::vector<std::string> flags;
            bool isFlags = false;
            
            void insertAndSort(const std::string& str);
        public:
            arg_vector_t() = default;
            
            arg_vector_t(const std::vector<std::string>& args);
            
            arg_vector_t(std::initializer_list<std::string> args);
            
            arg_vector_t(const std::string& arg);
            
            arg_vector_t(const char* arg);
            
            arg_vector_t& operator=(const std::string& arg);
            
            arg_vector_t& operator=(const char* arg);
            
            arg_vector_t& operator=(std::initializer_list<std::string>& args);
            
            arg_vector_t& operator=(std::vector<std::string>& args);
            
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
            
            /**
             * @return true if contains flags
             */
            [[nodiscard]] inline bool isFlag() const {
                return isFlags;
            }
            
            [[nodiscard]] inline bool contains(std::string_view string) const {
                return std::any_of(flags.begin(), flags.end(), [&string](const auto& flag) -> bool {
                    return flag == string;
                }) || std::any_of(names.begin(), names.end(), [&string](const auto& name) -> bool {
                    return name == string;
                });
            }
    };
    
    class arg_nargs_t {
        private:
            friend class arg_parse;
            static constexpr int UNKNOWN = 0x1;
            static constexpr int ALL = 0x2;
            static constexpr int ALL_REQUIRED = 0x4;
            // 0 means ignore
            int args = 1;
            // 0 indicates args is used
            int flags = 0;
            
            void decode(char c);
        
        public:
            arg_nargs_t() = default;
            
            arg_nargs_t(int args): args(args) {}
            
            arg_nargs_t(char c);
            
            arg_nargs_t(std::string s);
            
            arg_nargs_t(const char* s);
            
            arg_nargs_t& operator=(const std::string& s);
            
            arg_nargs_t& operator=(const char* s);
            
            arg_nargs_t& operator=(char c);
            
            arg_nargs_t& operator=(int args);
    };
    
    struct arg_properties_t {
        private:
        public:
            arg_vector_t a_flags;
            arg_action_t a_action = arg_action_t::STORE;
            arg_nargs_t a_nargs = 1;
            std::string a_const{};
            std::string a_default{};
            std::string a_dest{};
            std::string a_help{};
            std::string a_version{};
            std::string a_metavar{};
            bool a_required = false;
    };
    
    class arg_builder {
            arg_properties_t properties;
        public:
            arg_builder(const arg_vector_t& flags): properties(flags) {}
            
            inline arg_properties_t build() {
                return properties;
            }
            
            inline arg_builder& setAction(arg_action_t action){
                properties.a_action = action;
                return *this;
            }
            
            inline arg_builder& setNArgs(const arg_nargs_t& nargs){
                properties.a_nargs = nargs;
                return *this;
            }
            
            inline arg_builder& setConst(const std::string& a_const){
                properties.a_const = a_const;
                return *this;
            }
            
            inline arg_builder& setDefault(const std::string& def){
                properties.a_default = def;
                return *this;
            }
            
            inline arg_builder& setDest(const std::string& dest){
                properties.a_dest = dest;
                return *this;
            }
            
            inline arg_builder& setHelp(const std::string& help){
                properties.a_help = help;
                return *this;
            }
            
            inline arg_builder& setVersion(const std::string& version){
                properties.a_version = version;
                return *this;
            }
            
            inline arg_builder& setMetavar(const std::string& metavar){
                properties.a_metavar = metavar;
                return *this;
            }
            
            inline arg_builder& setRequired(){
                properties.a_required = true;
                return *this;
            }
            
    };
    
    class arg_tokenizer_t {
        private:
            static constexpr char FLAG = '-';
            std::vector<std::string> args;
            size_t nextIndex = 0;
            
            inline const std::string& get(size_t i) {
                return args[i];
            }
            
            inline bool hasNext(size_t i) {
                return (size_t) i < args.size();
            }
        
        public:
            arg_tokenizer_t() = default;
            
            arg_tokenizer_t(size_t argc, const char** argv);
            
            inline void forward() {
                nextIndex++;
            }
            
            inline const std::string& get() {
                return get(nextIndex);
            }
            
            inline const std::string& next() {
                return get(nextIndex++);
            }
            
            inline bool hasNext() {
                return hasNext(nextIndex);
            }
            
            inline bool isFlag(size_t i) {
                return get(i).starts_with(FLAG);
            }
            
            inline bool isFlag() {
                return isFlag(nextIndex);
            }
    };
    
    class arg_parse {
        public:
            typedef std::variant<std::string, bool, int32_t, std::vector<std::string>> arg_data_t;
        private:
            struct {
                    friend arg_parse;
                private:
                    std::vector<arg_properties_t> arg_storage;
                public:
                    std::vector<std::pair<std::string, arg_properties_t*>> name_associations;
                    HASHMAP<std::string, arg_properties_t*> flag_associations;
                    HASHSET<std::string> required_vars;
            } user_args;
            
            struct arg_results {
                    friend arg_parse;
                private:
                    HASHSET<std::string> found_required;
                    std::vector<std::string> unrecognized_args;
                public:
                    std::string program_name;
                    HASHMAP <std::string, arg_data_t> positional_args;
                    HASHMAP <std::string, arg_data_t> flag_args;
            } loaded_args;
        private:
            static std::string filename(const std::string& path);
            static bool validateArgument(const arg_properties_t& args);
            static bool consumeArguments(arg_tokenizer_t& arg_tokenizer, const arg_properties_t& properties, std::vector<std::string>& v_out);
            void handlePositionalArgument(arg_tokenizer_t& arg_tokenizer, size_t& last_pos);
            void handleFlagArgument(arg_tokenizer_t& arg_tokenizer);
            void processFlag(arg_tokenizer_t& arg_tokenizer, const std::string& flag);
        public:
            arg_parse() {
                //addArgument(arg_builder({"--help", "-h"}).setAction(arg_action_t::HELP).setHelp("Show this help menu").build());
            };
            
            void addArgument(const arg_properties_t& args);
            
            const arg_results& parse_args(int argc, const char** argv);
            
            void printHelp();
    };
    
    std::string to_string(const blt::parser::arg_parse::arg_data_t& v);
    
}

#endif //BLT_TESTS_ARGPARSE_H
