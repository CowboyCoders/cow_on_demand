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

#ifndef ___CHUNK_READER_H___
#define ___CHUNK_READER_H___

#include <fstream>
#include <string>

class chunk_reader {
public:
   /**
    * Creates and initalizes a new chunk_reader.
    * @param max_read Maximum size to return from a single request.
    */
    chunk_reader(size_t max_read);

    ~chunk_reader();

   /**
    * Read a not necessarily sequential list of chunks based on the
    * supplied indices.
    * @param filename The file to read from.
    * @param size Chunk size.
    * @param indices Chunk indices to read.
    */
    bool read(std::string filename,
              size_t size,
              const std::vector<size_t>& indices);

   /**
    * This function returns the size of a chunk.
    * @return A size_t describing the size of a chunk.
    */
    size_t chunk_size()
    { 
        return chunk_size_; 
    }

   /**
    * This function returns how much data was read.
    * @return A size_t describing how much data was read.
    */
    size_t data_size()
    {
        return data_size_;
    }
    
   /**
    * Returns a pointer to the read data.
    * @return The pointer to the data.
    */
    const char* chunk() { 
        return chunk_; 
    }

private:
    void realloc_chunk(size_t chunk_size);
    void dealloc_chunk();
    
    bool open_filestream();

    std::ifstream filestream_;
    std::string filename_;
    size_t max_read_;
    char* chunk_;
    size_t chunk_size_;
    size_t data_size_;
};

#endif // ___CHUNK_READER_H___
