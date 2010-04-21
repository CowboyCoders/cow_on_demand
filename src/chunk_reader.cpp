#include <algorithm>
#include <iostream>
#include <vector>
#include <boost/log/trivial.hpp>

#include "chunk_reader.hpp"

ChunkReader::ChunkReader(size_t max_read)
    : max_read_(max_read),
    chunk_(0),
    chunk_size_(0),
    data_size_(0)
{
}

ChunkReader::~ChunkReader()
{
    dealloc_chunk();
    filestream_.close();
}

/**
 * Reads a list of chunks from file, based on indices and size.
 */
bool ChunkReader::read(std::string filename,
          size_t size,
          const std::vector<size_t>& indices) {
    filename_ = filename;
    chunk_size_ = size * indices.size();
    data_size_ = 0;
    size_t read = 0;

    if(!open_filestream()) {
        BOOST_LOG_TRIVIAL(error)  << "error: can't open file '"
                << filename_ << "' for reading";

        dealloc_chunk();
        return false;
    }

    filestream_.seekg(0, std::ios::end);
    size_t file_size = filestream_.tellg();

    realloc_chunk(chunk_size_);

    for(std::vector<size_t>::const_iterator it = indices.begin(); it < indices.end(); it++) {
        size_t idx = *it;
        BOOST_LOG_TRIVIAL(debug) << idx;

        size_t pos = size*idx;

        if(pos >= file_size) {
            BOOST_LOG_TRIVIAL(error) << "error: can't read outside of file";
            dealloc_chunk();
            data_size_ = 0;
            return false;
        }

        data_size_ += std::min(size, file_size - pos);

        BOOST_LOG_TRIVIAL(debug) << "chunk_size_: " << chunk_size_;

        if(data_size_ > max_read_) {
            BOOST_LOG_TRIVIAL(error) << "error: requested " << data_size_
                    << "B, which exceeds the limit "
                    << max_read_ << "B";

            dealloc_chunk();
            data_size_ = 0;

            return false;
        }

        filestream_.seekg(pos, std::ios::beg);
        filestream_.read(chunk_ + read, size);

        read += std::min(size, file_size - pos);
    }

    return true;
}

/**
 * Reads a range of chunks.
 * @deprecated
 */
bool ChunkReader::read(std::string filename,
                       size_t size,
                       size_t index,
                       size_t count)
{
    filename_ = filename;

    if(!open_filestream()) {
        BOOST_LOG_TRIVIAL(error)  << "error: can't open file '"
                << filename_ << "' for reading";

        dealloc_chunk();
        return false;
    }

    filestream_.seekg(0, std::ios::end);
    size_t file_size = filestream_.tellg();
    
    size_t pos = size * index;
    if(pos >= file_size) {
        BOOST_LOG_TRIVIAL(error) << "error: can't read outside of file";

        dealloc_chunk();
        return false;
    }

    size_t chunk_size =
            file_size-pos < size*count ? file_size-pos : size*count;

    if(chunk_size > max_read_) {
        BOOST_LOG_TRIVIAL(error) << "error: requested " << chunk_size
                << "B, which exceeds the limit "
                << max_read_ << "B";

        dealloc_chunk();
        return false;
    }
    
    realloc_chunk(chunk_size);
    
    filestream_.seekg(pos, std::ios::beg);
    filestream_.read(chunk_, chunk_size);
    
    return true;
}

void ChunkReader::realloc_chunk(size_t chunk_size)
{
    chunk_size_ = chunk_size;
    
    if(chunk_) {
        delete [] chunk_;
    }

    chunk_ = new char[chunk_size_];
}

void ChunkReader::dealloc_chunk()
{
    if(chunk_) {
        delete [] chunk_;
        chunk_ = 0;
        chunk_size_ = 0;
    }
}

bool ChunkReader::open_filestream()
{
    if(filestream_.is_open()) {
        filestream_.close();
    }

    filestream_.open(filename_.c_str());
    return filestream_.is_open();
}
