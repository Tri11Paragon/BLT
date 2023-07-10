/*
 * Created by Brett on 29/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/filesystem.h>
#include <cstring>

int blt::fs::fstream_block_reader::read(char* buffer, size_t bytes) {
    if (readIndex == 0)
        m_stream.read(m_buffer, (long) m_bufferSize);
    if (readIndex + bytes >= m_bufferSize) {
        // copy out all the data from the current buffer
        auto bytesLeft = m_bufferSize - readIndex;
        memcpy(buffer, m_buffer + readIndex, bytesLeft);
        readIndex = 0;
        // now to prevent large scale reading in small blocks, we should just read the entire thing into the buffer.
        m_stream.read(buffer + bytesLeft, (long) (bytes - bytesLeft));
    } else {
        // but in the case that the size of the data read is small, we should read in blocks and copy from that buffer
        // that should be quicker since file operations are slow.
        std::memcpy(buffer, m_buffer + readIndex, bytes);
        readIndex += bytes;
    }
    return 0;
}

int blt::fs::fstream_block_writer::write(char* buffer, size_t bytes) {
    if (writeIndex + bytes >= m_bufferSize) {
        // in an attempt to stay efficient we write out the old buffer and the new buffer
        // since there is a good chance there is more than a buffer's worth of data being written
        // otherwise the buffer is almost full and can be written anyway. (this might be bad for performance especially if the FS wants round numbers)
        m_stream.write(m_buffer, (long) writeIndex);
        writeIndex = 0;
        m_stream.write(buffer, (long) bytes);
    } else {
        std::memcpy(m_buffer + writeIndex, buffer, bytes);
        writeIndex += bytes;
    }
    return 0;
}

void blt::fs::fstream_block_writer::flush() {
    m_stream.write(m_buffer, (long) writeIndex);
    writeIndex = 0;
}
