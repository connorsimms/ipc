#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>

#include <atomic>
#include <cerno>
#include <string>
#include <system_error>

struct SharedHeader
{
    alignas(64) std::atomic<std::size_t> write_index;
    std::uint32_t magic;
    std::uint32_t version;
    std::size_t capacity;
    std::size_t element_size;
    alignas(64) std::atomic<std::size_t> read_index;
};

class SHMSegment
{
public:
    void create(std::string name, std::size_t size)
    {
        is_owner_ = true;
        name_ = name;
        total_size_ = size;

        // empty contents if exists
        shm_fd_ = shm_open(name_.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU); 

        if (shm_fd_ == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        if (ftruncate(shm_fd_, total_size_) == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        mapped_ptr_ = mmap(NULL, total_size_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);

        if (mapped_ptr_ == MAP_FAILED)
        {
            mapped_ptr = nullptr;
            throw std::system_error(errno, std::generic_category());
        }
    }

    void attach(std::string name)
    {
        is_owner_ = false;
        name_ = name;

        // throw error if doesn't exist
        shm_fd_ = shm_open(name_.c_str(), O_RDWR, S_IRWXU); 

        if (shm_fd_ == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }

        struct stat file_info;

        if (fstat(shm_fd_, &file_info) == -1)
        {
            throw std::system_error(errno, std::generic_category());
        }
        
        total_size_ = file_info.st_size;

        mapped_ptr_ = mmap(NULL, total_size_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);

        if (mapped_ptr_ == MAP_FAILED)
        {
            mapped_ptr = nullptr;
            throw std::system_error(errno, std::generic_category());
        }
    }

    void* get_address()
    {
        return mapped_ptr_;
    }

    ~SHMSegment()
    {
        if (mapped_ptr_)
        {
            munmap(mapped_ptr_, total_size_);
        }

        if (is_owner_)
        {
            shm_unlink(name_.c_str());
        }

        if (shm_fd_ != -1)
        {
            close(shm_fd_);
        }
    }

private:
    bool is_owner_{ false };                 // unlink responsibility
    std::string name_;              // name identifier
    int shm_fd_{ -1 };              // file descriptor
    std::size_t total_size_;        // size of block in bytes 
    void* mapped_ptr_{ nullptr };   // raw ptr to shm block
}
