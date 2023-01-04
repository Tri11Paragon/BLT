/*
 * Created by Brett on 04/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_RANDOM_H
#define BLT_RANDOM_H

#include <random>

namespace BLT {
    /**
     * Creates a container class for generating random number distributions
     * @tparam T numeric type
     * @tparam dist std::uniform_real_distribution or std::uniform_int_distribution
     */
    template<typename T, template<typename = T> typename dist = std::uniform_real_distribution>
    class Random {
        private:
            std::random_device rd; // obtain a random number from hardware
            std::mt19937 gen;
            dist<T>* distribution = nullptr;
        public:
            explicit Random(T min = (T)0, T max = (T)1, long seed = 0): gen(std::mt19937(seed)){
                distribution = new dist(min, max);
            }
            T get(){
                return (*distribution)(gen);
            }
            ~Random(){
                delete distribution;
            }
    };
}

#endif //BLT_RANDOM_H
