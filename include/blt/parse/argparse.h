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
            
            struct equals {
                bool operator()(const arg_vector_t& a1, const arg_vector_t& a2) const {
                    // arg cannot have both
                    if (!a1.names.empty()) {
                        // match all pos arg
                        return std::ranges::all_of(a1.names, [&a2](const auto& n) -> bool {
                            if (std::find(a2.names.begin(), a2.names.end(), n) == a2.names.end())
                                return false;
                            return true;
                        });
                    } else {
                        // match any flag (--foo or -f)
                        return std::ranges::all_of(a1.flags, [&a2](const auto& f) -> bool {
                            if (std::find(a2.flags.begin(), a2.flags.end(), f) != a2.flags.end())
                                return true;
                            return false;
                        });
                    }
                }
            };
            
            struct hash {
                size_t operator()(const arg_vector_t& a) const {
                    size_t v = 0;
                    std::hash<std::string> hash;
                    for (const auto& n : a.names) {
                        v >>= 8;
                        v += hash(n);
                    }
                    for (const auto& f : a.flags) {
                        v >>= 8;
                        v += hash(f);
                    }
                    return v;
                }
            };
    };
    
    class arg_nargs_t {
        private:
            friend class argparse;
            static constexpr int UNKNOWN = 0x1;
            static constexpr int ALL = 0x2;
            static constexpr int ALL_REQUIRED = 0x4;
            // 0 means default behaviour (consume 1 if presented otherwise ignore)
            int args = 0;
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
            arg_nargs_t a_nargs = 0;
            std::string a_const{};
            std::string a_default{};
            std::string a_dest{};
            std::string a_help{};
            std::string a_version{};
            std::string a_metavar{};
            bool a_required = false;
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
    
    class argparse {
        public:
            typedef std::variant<std::string, bool, int32_t, std::vector<std::string>> arg_data_t;
        private:
            struct {
                    friend argparse;
                private:
                    std::vector<arg_properties_t> arg_storage;
                public:
                    std::vector<std::pair<std::string, arg_properties_t*>> name_associations;
                    HASHMAP<std::string, arg_properties_t*> flag_associations;
                    HASHSET<arg_vector_t, arg_vector_t::hash, arg_vector_t::equals> required_vars;
            } user_args;
            
            struct arg_results {
                    friend argparse;
                private:
                    HASHSET<arg_vector_t, arg_vector_t::hash, arg_vector_t::equals> found_required;
                public:
                    std::string program_name;
                    HASHMAP <arg_vector_t, arg_data_t, arg_vector_t::hash, arg_vector_t::equals> positional_args;
                    HASHMAP <arg_vector_t, arg_data_t, arg_vector_t::hash, arg_vector_t::equals> flag_args;
            } loaded_args;
        private:
            static std::string filename(const std::string& path);
            static bool validateArgument(const arg_properties_t& args);
            static bool consumeArguments(arg_tokenizer_t& arg_tokenizer, const arg_properties_t& properties, std::vector<std::string>& v);
            void handlePositionalArgument(arg_tokenizer_t& arg_tokenizer, size_t& last_pos);
            void handleFlagArgument(arg_tokenizer_t& arg_tokenizer);
            void processFlag(arg_tokenizer_t& arg_tokenizer, const std::string& flag);
        public:
            argparse() = default;
            
            void addArgument(const arg_properties_t& args);
            
            const arg_results& parse_args(int argc, const char** argv);
            
            void printHelp();
    };
    
}

#endif //BLT_TESTS_ARGPARSE_H
