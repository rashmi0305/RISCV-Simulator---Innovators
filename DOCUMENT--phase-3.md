
# PHASE 3 DOCUMENT
-----
## What we have Successfully Implemented
- In this phase of development, we have extended the functionality of our simulator to incorporate cache.
The implementation includes features such as cache size, block size, associativity, cache replacement policies (LRU, LFU, Random), and access
latency of the cache.
- Memory access within the simulator now first searches for the address in the cache. If the data is found in the cache (cache hit), it is accessed
directly fromthe cache.If not (cache miss), the data is fetched from the main memory.cache parameters including cache size, block size, associativity,
and access latency through input files 
### Cache Replacement Policies
 The simulator supports three cache replacement policies:
 #### LRU (Least Recently Used):
- Replaces the least recently used cache block when the cache is full.We have implemented this using linked list
Implementation:
- When a memory block is accessed or inserted into the cache, it is added to the head of the linked list, indicating that it is the most recently used block.
- If the cache is full and a new block needs to be added, the least recently used block at the tail of the linked list is removed to make space.
- Nodes are moved within the linked list to reflect their usage patterns. The most frequently accessed blocks remain towards the head, while the least accessed
ones are towards the tail.
#### LFU (Least Frequently Used): 
- Replaces the least frequently used cache block when the cache is full.
- We are implementing  LFU as it is generally better suited for workloads with predictable and stable access patterns, where certain data items are accessed frequently
over time.
Implementation:
- Each block maintains a frequency or priority counter indicating its usage frequency.
- Upon a hit, the accessed block's frequency counter is increased.
- Upon a miss, the block with the lowest frequency (LFU) is chosen for replacement.
- Upon a miss, the identified block (LFU) is replaced with the new data.
#### Random
Implementation:
- A random number is generated within the range [lowerLimit, upperLimit] using the rand() function.
- Once the random index is generated, the cache block at that index in the specified set is updated with the new tag and marked as valid.
- This effectively replaces the randomly selected block with the new block that needs to be inserted into the cache.
- Replaces a randomly selected cache block when the cache is full.

---
- The input also includes the main memory access time, which is used to calculate the overall access latency for cache misses.The implemented cache acts as
a shared cache, meaning that both  cores of the simulator access the same cache.
- An instruction fetch is considered a memory access. If the instruction is found in the cache, it is fetched from the cache. Otherwise, 
it is fetched from the main memory by taking extra cycles.We have added instructions to memory from 3000 index.


##  What we have not Implemented

In the extension of our simulator to incorporate cache, we have not implemented variable latency inside th epipeline.However stall count for memory instructions (loads and stores) and the instruction fetch (IF) stage have been added correctly. 
As we are not able to hold the instruction until latency becomes zero in pipeline the instruction is getting flushed and empty
instruction is getting passed to next stage instead of correct instruction because of this we were not able to implement latency.

