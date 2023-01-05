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
    class random {
        private:
            std::random_device rd; // obtain a random number from hardware
            std::mt19937 gen;
            dist<T>* distribution = nullptr;
        public:
            /**
             * Construct the random number generator.
             * @param min min value possible to generate. (default: 0)
             * @param max max value possible to generate. (default: 1)
             * @param seed seed to use in generating random values. (default: 0)
             */
            explicit random(T min = (T)0, T max = (T)1, long seed = 0): gen(std::mt19937(seed)){
                distribution = new dist(min, max);
            }
            /**
             * Note the min/max are inclusive and defaults to a **uniform** distribution.
             * @return random number between the defined min/max or the default of [0,1].
             */
            T get(){
                return (*distribution)(gen);
            }
            ~random(){
                delete distribution;
            }
    };
}

#endif //BLT_RANDOM_H
