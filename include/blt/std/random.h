/*
 * Created by Brett on 04/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_RANDOM_H
#define BLT_RANDOM_H

#include <random>

namespace blt::random {
    
    static inline uint32_t PCG_Hash(uint32_t input) {
        uint32_t state = input * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^
        word;
    }

    static inline float randomFloat(uint32_t& seed){
        seed = PCG_Hash(seed);
        return (float)seed / (float)std::numeric_limits<uint32_t>::max();
    }

    /**
     * @return random float without changing seed
     */
    static inline float randomFloat_c(uint32_t seed){
        return randomFloat(seed);
    }

    /**
     * @param seed seed for random
     * @param min inclusive min
     * @param max exclusive max
     * @return random int between min (inclusive) and max (exclusive)
     */
    static inline int randomInt(uint32_t& seed, int min = 0, int max = 1){
        return (int)((randomFloat(seed) * (float)(max - min)) + (float)min);
    }

    static inline int randomInt_c(uint32_t seed, int min = 0, int max = 1){
        return randomInt(seed, min, max);
    }

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
            explicit random(T min = (T) 0, T max = (T) 1, long seed = 0): gen(std::mt19937(seed)) {
                distribution = new dist(min, max);
            }
            
            /**
             * Note the min/max are inclusive and defaults to a **uniform** distribution.
             * @return random number between the defined min/max or the default of [0,1].
             */
            T get() {
                return (*distribution)(gen);
            }
            
            ~random() {
                delete distribution;
            }
    };
    
    template<typename T>
    class simplex_noise {
        private:
        
        public:
            simplex_noise() {
            
            }
    };
    
}

#endif //BLT_RANDOM_H
