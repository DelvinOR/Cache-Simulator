#Cache Simulator

CS211 (Computer Architecture) project to simulatre a working cache software component.

The main data structures that I used in this assignment was a hash table that was an array of a structure of sets that I created. In each set structure, I had an array of structures called blocks and another data structure called a Queue that was used for the fifo and lru replacement policy. By using the prefetcher, I observed an increase in the amount of cache hits and a decrease in the amount of cache misses. The main reason for this is that using prefetching gave the advantage of spatial locality and thus resulting in a higher probability of a cache hit. Additionally, there was an increase in the number of memory reads because prefetching requiers one extra memory read when taking the next block into the cache. I also completed an extra credit (lru).

Grade: 100%(max 100.0)
