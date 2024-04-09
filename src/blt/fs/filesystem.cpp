/*
 *  <Short Description>
 *  Copyright (C) 2024  Brett Terpstra
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
#include <blt/fs/filesystem.h>
#include <cstring>
#include <blt/std/logging.h>

namespace blt::fs
{
    
    fstream_block_reader::fstream_block_reader(std::fstream& stream, size_t bufferSize):
            block_reader(bufferSize), m_stream(stream), m_buffer(new char[bufferSize])
    {
        if (!m_stream.good() || m_stream.fail())
            BLT_WARN("Provided std::fstream is not good! Clearing!");
        m_stream.clear();
    }
    
    int fstream_block_reader::read(char* buffer, size_t bytes)
    {
        if (readIndex == 0)
            m_stream.read(m_buffer, (long) m_bufferSize);
        if (readIndex + bytes >= m_bufferSize)
        {
            // copy out all the data from the current buffer
            auto bytesLeft = m_bufferSize - readIndex;
            memcpy(buffer, m_buffer + readIndex, bytesLeft);
            readIndex = 0;
            // now to prevent large scale reading in small blocks, we should just read the entire thing into the buffer.
            m_stream.read(buffer + bytesLeft, (long) (bytes - bytesLeft));
        } else
        {
            // but in the case that the size of the data read is small, we should read in blocks and copy from that buffer
            // that should be quicker since file operations are slow.
            std::memcpy(buffer, m_buffer + readIndex, bytes);
            readIndex += bytes;
        }
        return 0;
    }
    
    int fstream_block_writer::write(char* buffer, size_t bytes)
    {
        if (writeIndex + bytes >= m_bufferSize)
        {
            // in an attempt to stay efficient we write out the old buffer and the new buffer
            // since there is a good chance there is more than a buffer's worth of data being written
            // otherwise the buffer is almost full and can be written anyway. (this might be bad for performance especially if the FS wants round numbers)
            m_stream.write(m_buffer, (long) writeIndex);
            writeIndex = 0;
            m_stream.write(buffer, (long) bytes);
        } else
        {
            std::memcpy(m_buffer + writeIndex, buffer, bytes);
            writeIndex += bytes;
        }
        return 0;
    }
    
    void fstream_block_writer::flush_internal()
    {
        m_stream.write(m_buffer, (long) writeIndex);
        writeIndex = 0;
    }
    
}