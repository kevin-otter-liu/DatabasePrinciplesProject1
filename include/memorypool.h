//
// Created by alpha on 9/9/2022.
//

#ifndef DATABASEPROJ_MEMORYPOOL_H
#define DATABASEPROJ_MEMORYPOOL_H

#include "types.h"
#include <cmath>
#include <vector>

struct Address;

class MemoryPool{
public:
    // =============== Methods ================ //

    // Creates a new memory pool with the following parameters:
    // maxPoolSize: Maximum size of the memory pool.
    // blockSize: The fixed size of each block in the pool.
    MemoryPool(std::size_t maxPoolSize, std::size_t blockSize);

    // Allocate a new block from the memory pool. Returns false if error.
    bool allocateBlock(bool record);

    // Allocates a new chunk to the memory pool. Creates a new block if chunk is unable to fit in current free block.
    // Returns a struct with the block's address and the record's offset within the block.
    Address allocate(std::size_t sizeRequired, bool record);

    // Deallocates an existing record and block if block becomes empty. Returns false if error.
    bool deallocate(Address address, std::size_t sizeToDelete);

    // Give a block address, offset and size, returns the data there.
    void *loadFromDisk(Address address, std::size_t size);

    // Save data to the disk given a main memory address.
    Address saveToDisk(void *itemAddress, std::size_t size);

    // Update data in disk if I have already saved it before.
    Address saveToDisk(void *itemAddress, std::size_t size, Address diskAddress);

    // Returns the maximum size of this memory pool.
    std::size_t getMaxPoolSize() const
    {
        return maxPoolSize;
    }

    // Returns the size of a block in memory pool.
    std::size_t getBlockSize() const
    {
        return blockSize;
    };

    // Returns the size used in the current block.
    std::size_t getBlockSizeUsed() const
    {
        return blockSizeUsed;
    };

    // Returns current size used in memory pool (total blocks size).
    std::size_t getSizeUsed() const
    {
        return sizeUsed;
    }

    // Returns actual size of all records stored in memory pool.
    std::size_t getActualSizeUsed() const
    {
        return actualSizeUsed;
    }

    // Returns number of currently allocated blocks.
    int getAllocated() const
    {
        return allocated;
    };
    
    
    // Returns actual size of all records stored in memory pool.
    std::size_t getActualSizeUsedWithoutPadding() const
    {
        // Calculate actual size used without padding
        std::size_t blocksUsedForIndex = allocated - blocksAllocatedForRecords;
        
        // actualRecordSizeWithoutPadding given by blocksAllocatedForRecords * 200B to find total space used, then times 19/20 to simulate if we stored it as 19B instead.
        std::size_t actualRecordSizeWithoutPadding = (blocksAllocatedForRecords * getBlockSize() / 20) * 19;
        std::size_t actualTotalSizeWithoutPadding = actualRecordSizeWithoutPadding + blocksUsedForIndex * getBlockSize();
        return actualTotalSizeWithoutPadding;
    }

    // Returns number of currently allocated blocks.
    int getAllocatedUsedWithoutPadding() const
    {
        // Calculate actual blocks used without padding
        unsigned int blocksUsedForIndex = allocated - blocksAllocatedForRecords;
        
        // blocksAllocatedForRecords * 200 to find total space used, minus 1B saved for each block, divided by 200 again, then take the ceiling.
        std::size_t actualSpaceUsedForRecordsWithoutPadding = ceil(((blocksAllocatedForRecords * getBlockSize()) - blocksAllocatedForRecords) / getBlockSize());
        std::size_t actualBlockUsedWithoutPadding = actualSpaceUsedForRecordsWithoutPadding + blocksUsedForIndex;
        return actualBlockUsedWithoutPadding;
    };
    
    int getBlocksAccessed() const
    {
        return blocksAccessed;
    }

    int resetBlocksAccessed()
    {
        int tempBlocksAccessed = blocksAccessed;
        blocksAccessed = 0;
        return tempBlocksAccessed;
    }
    int getTotalBlocksUsedForRecords() const
    {
        return blocksAllocatedForRecords;
    }

    // Destructor
    ~MemoryPool();

private:
    // =============== Data ================ //

    std::size_t maxPoolSize;    // Maximum size allowed for pool.
    std::size_t blockSize;      // Size of each block in pool in bytes.
    std::size_t sizeUsed;       // Current size used up for storage (total block size).
    std::size_t actualSizeUsed; // Actual size used based on records stored in storage.
    std::size_t blockSizeUsed;  // Size used up within the current block we are pointing to.
    std::size_t blocksAllocatedForRecords; // Total blocks allocated for records

    int allocated;      // Number of currently allocated blocks.
    int blocksAccessed; // Counts number of blocks accessed.

    void *pool;  // Pointer to the memory pool.
    void *block; // Current block pointer we are inserting to.
};

#endif //DATABASEPROJ_MEMORYPOOL_H
