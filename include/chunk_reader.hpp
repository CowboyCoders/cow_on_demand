#ifndef ___CHUNK_READER_H___
#define ___CHUNK_READER_H___

#include <fstream>
#include <string>

class ChunkReader {
public:
    ChunkReader(size_t max_read);

    ~ChunkReader();

    bool read(std::string filename,
              size_t size,
              size_t index,
              size_t count);

    bool read(std::string filename,
              size_t size,
              const std::vector<size_t>& indices);

    size_t chunk_size()
    { 
        return chunk_size_; 
    }
    
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
};

#endif // ___CHUNK_READER_H___
