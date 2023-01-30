/*
 * Created by Brett on 29/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_FILESYSTEM_H
#define BLT_FILESYSTEM_H

#include <fstream>
#include <ios>

namespace blt::fs {
    
    /**
     * Creates an encapsulation of a T array which will be automatically deleted when this object goes out of scope.
     * This is a simple buffer meant to be used only inside of a function and not moved around, with a few minor exceptions.
     * @tparam T type that is stored in buffer eg char
     */
    template<typename T>
    struct scoped_buffer {
        T* buffer;
        unsigned long size;
        
        explicit scoped_buffer(unsigned long size): size(size) {
            buffer = new T[size];
        }
        
        scoped_buffer(scoped_buffer& copy) = delete;
        
        scoped_buffer(scoped_buffer&& move) = delete;
        
        scoped_buffer operator=(scoped_buffer& copyAssignment) = delete;
        
        scoped_buffer operator=(scoped_buffer&& moveAssignment) = delete;
        
        inline T& operator[](unsigned long index) const {
            return buffer[index];
        }
        
        ~scoped_buffer() {
            delete[] buffer;
        }
    };
    
    /**
     * A simple interface which provides a way of reading the next block of data from a resource.
     * The interface provides a single function "read" which will read a specified number of bytes into the buffer.
     * The implementation for this could be fstreams, zlib deflate, or any method filesystem access.
     * Reading of a large number of bytes ( > block size) is guaranteed to not significantly increase the read time and will likely result in a
     * direct passthrough to the underlying system. Small reads will be buffered, hence the name "block" reader.
     */
    class block_reader {
        protected:
            // 32768 block size seems the fastest on my system
            unsigned long m_bufferSize;
        public:
            explicit block_reader(unsigned long bufferSize): m_bufferSize(bufferSize) {}
            
            /**
             * Reads bytes from the internal filesystem implementation
             * @param buffer buffer to copy the read bytes into
             * @param bytes number of bytes to read
             * @return status code. non-zero return codes indicates a failure has occurred.
             */
            virtual int read(char* buffer, size_t bytes) = 0;
    };
    
    /**
     * A buffered block writer without a definite backend implementation. Exactly the same as a block_reader but for writing to the filesystem.
     */
    class block_writer {
        protected:
            unsigned long m_bufferSize;
        public:
            explicit block_writer(unsigned long bufferSize): m_bufferSize(bufferSize) {}
            
            /**
             * Writes the bytes to the filesystem backend implementation
             * @param buffer bytes to write
             * @param bytes number of bytes to write
             * @return non-zero code if failure
             */
            virtual int write(char* buffer, size_t bytes) = 0;
            
            /**
             * Ensures that the internal buffer is written to the filesystem.
             */
            virtual void flush() = 0;
    };
    
    /**
     * fstream implementation of the block reader.
     */
    class fstream_block_reader : public block_reader {
        private:
            std::fstream& m_stream;
            char* m_buffer;
            size_t readIndex = 0;
        public:
            explicit fstream_block_reader(std::fstream& stream, size_t bufferSize):
                    m_stream(stream), m_buffer(new char[bufferSize]), block_reader(bufferSize) {}
            
            explicit fstream_block_reader(fstream_block_reader& copy) = delete;
            
            explicit fstream_block_reader(fstream_block_reader&& move) = delete;
            
            fstream_block_reader& operator=(const fstream_block_reader& copy) = delete;
            
            fstream_block_reader& operator=(const fstream_block_reader&& move) = delete;
            
            int read(char* buffer, size_t bytes) override;
            
            ~fstream_block_reader() {
                delete[] m_buffer;
            }
    };
    
    class fstream_block_writer : public block_writer {
        private:
            std::fstream& m_stream;
            char* m_buffer;
            size_t writeIndex = 0;
        public:
            explicit fstream_block_writer(std::fstream& stream, size_t bufferSize):
                    m_stream(stream), m_buffer(new char[bufferSize]), block_writer(bufferSize) {}
            
            explicit fstream_block_writer(fstream_block_writer& copy) = delete;
            
            explicit fstream_block_writer(fstream_block_writer&& move) = delete;
            
            fstream_block_writer& operator=(const fstream_block_writer& copy) = delete;
            
            fstream_block_writer& operator=(const fstream_block_writer&& move) = delete;
            
            int write(char* buffer, size_t bytes) override;
            void flush() override;
            
            ~fstream_block_writer() {
                delete[] m_buffer;
            }
    };
    
    
}

#endif //BLT_FILESYSTEM_H
