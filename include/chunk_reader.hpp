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

     * @param size Chunk size
     * @param indices Chunk indices to read
     */
    bool read(std::string filename,
              size_t size,
              const std::vector<size_t>& indices);

    /**
     * Size of chunk
     *
     * @return a size_t describing the size
     */
    size_t chunk_size()
    { 
        return chunk_size_; 
    }

    /**
     * Size of read data
     *
     * @return a size_t describing how much data was read
     */
    size_t data_size()
    {
        return data_size_;
    }
    
    /**
     * Returns a pointer to the read data
     *
     * @return the pointer to the data
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
