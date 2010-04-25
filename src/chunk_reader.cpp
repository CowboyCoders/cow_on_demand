/*
Copyright 2010 CowboyCoders. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COWBOYCODERS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COWBOYCODERS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of CowboyCoders.
*/

#include <algorithm>
#include <iostream>
#include <vector>
#include <boost/log/trivial.hpp>

#include "chunk_reader.hpp"

chunk_reader::chunk_reader(size_t max_read)
    : max_read_(max_read),
    chunk_(0),
    chunk_size_(0),
    data_size_(0)
{
}

chunk_reader::~chunk_reader()
{
    dealloc_chunk();
    filestream_.close();
}

bool chunk_reader::read(std::string filename,
          size_t size,
          const std::vector<size_t>& indices) {
    filename_ = filename;
    chunk_size_ = size * indices.size();
    data_size_ = 0;
    size_t chunk_offset = 0;

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
        BOOST_LOG_TRIVIAL(debug) << "chunk_reader: reading index: " << idx;

        size_t pos = size*idx;

        if(pos >= file_size) {
            BOOST_LOG_TRIVIAL(error) << "error: can't read outside of file";
            dealloc_chunk();
            data_size_ = 0;
            return false;
        }

        size_t bytes_to_read = std::min(size, file_size - pos);
        data_size_ += bytes_to_read;

        BOOST_LOG_TRIVIAL(debug) << "chunk_reader: reading size: " << chunk_size_;

        if(data_size_ > max_read_) {
            BOOST_LOG_TRIVIAL(error) << "error: requested " << data_size_
                    << "B, which exceeds the limit "
                    << max_read_ << "B";

            dealloc_chunk();
            data_size_ = 0;

            return false;
        }

        filestream_.seekg(pos, std::ios::beg);
        filestream_.read(chunk_ + chunk_offset, bytes_to_read);

        chunk_offset += bytes_to_read;
    }

    return true;
}

void chunk_reader::realloc_chunk(size_t chunk_size)
{
    chunk_size_ = chunk_size;
    
    if(chunk_) {
        delete [] chunk_;
    }

    chunk_ = new char[chunk_size_];
}

void chunk_reader::dealloc_chunk()
{
    if(chunk_) {
        delete [] chunk_;
        chunk_ = 0;
        chunk_size_ = 0;
    }
}

bool chunk_reader::open_filestream()
{
    if(filestream_.is_open()) {
        filestream_.close();
    }

    filestream_.open(filename_.c_str());
    return filestream_.is_open();
}
