#include "chunk_reader.hpp"

#include <iostream>

ChunkReader::ChunkReader(size_t max_read)
    : max_read_(max_read),
      chunk_(0),
      chunk_size_(0)
{
}

ChunkReader::~ChunkReader()
{
    dealloc_chunk();
    filestream_.close();
}

bool ChunkReader::read(std::string filename,
                       size_t size,
                       size_t index,
                       size_t count)
{
    filename_ = filename;
    if(!open_filestream()) {
        std::cerr << "error: can't open file '"
                  << filename_ << "' for reading"
                  << std::endl;

        dealloc_chunk();
        return false;
    }

    filestream_.seekg(0, std::ios::end);
    size_t file_size = filestream_.tellg();
    
    size_t pos = size * index;
    if(pos >= file_size) {
        std::cerr << "error: can't read outside of file"
                  << std::endl;

        dealloc_chunk();
        return false;
    }

    size_t chunk_size =
        file_size-pos < size*count ? file_size-pos : size*count;

    if(chunk_size > max_read_) {
        std::cerr << "error: requested " << chunk_size
                  << "B, which exceeds the limit " 
                  << max_read_ << "B" << std::endl;
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
