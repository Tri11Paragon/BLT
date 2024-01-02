/*
 *  <Short Description>
 *  Copyright (C) 2023  Brett Terpstra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BLT_THREAD_H
#define BLT_THREAD_H

#include <thread>
#include <functional>
#include <string>
#include <vector>
#include <queue>
#include <variant>
#include <atomic>
#include <mutex>
#include <chrono>
#include <optional>

namespace blt
{
    /**
     * @tparam queue should we use a queue or execute the same function over and over?
     */
    template<bool queue = false>
    class thread_pool
    {
        private:
            typedef std::function<void()> thread_function;
            volatile std::atomic_bool should_stop = false;
            volatile std::atomic_uint64_t stopped = 0;
            std::uint64_t number_of_threads = 0;
            std::vector<std::thread*> threads;
            std::variant<std::queue<thread_function>, thread_function> func_queue;
            std::mutex queue_mutex;
        public:
            explicit thread_pool(std::uint64_t number_of_threads = 8, std::optional<thread_function> default_function = {})
            {
                if (default_function.has_value())
                    func_queue = default_function.value();
                this->number_of_threads = number_of_threads;
                for (std::uint64_t i = 0; i < number_of_threads; i++)
                {
                    threads.push_back(new std::thread([this]() {
                        while (!should_stop)
                        {
                            if constexpr (queue)
                            {
                                // should be safe right?
                                std::unique_lock lock(queue_mutex);
                                lock.lock();
                                auto& func_q = std::get<std::queue<thread_function>>(func_queue);
                                if (func_q.empty())
                                {
                                    lock.unlock();
                                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                                    continue;
                                }
                                auto func = func_q.front();
                                func_q.pop();
                                lock.unlock();
                                func();
                            } else
                            {
                                {
                                    std::scoped_lock lock(queue_mutex);
                                    if (std::holds_alternative<std::queue<thread_function>>(func_queue))
                                    {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(16));
                                        BLT_WARN("Running non queue variant with a queue inside!");
                                        break;
                                    }
                                }
                                auto& func = std::get<thread_function>(func_queue);
                                func();
                            }
                        }
                        stopped++;
                    }));
                }
            }
            
            inline void execute(const thread_function& func)
            {
                std::scoped_lock lock(queue_mutex);
                if constexpr (queue)
                {
                    auto& v = std::get<std::queue<thread_function>>(func_queue);
                    v.push(func);
                } else
                {
                    func_queue = func;
                }
            }
            
            [[nodiscard]] inline bool complete() const {
                return stopped == number_of_threads;
            }
            
            inline void stop()
            {
                should_stop = true;
            }
            
            ~thread_pool()
            {
                should_stop = true;
                for (auto* t : threads)
                {
                    if (t->joinable())
                        t->join();
                    delete t;
                }
            }
    };
}

#endif //BLT_THREAD_H
