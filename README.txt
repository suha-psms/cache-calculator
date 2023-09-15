Eric Alvarado: ealvara6@jhu.edu 
Suha Patel: spate186@jhu.edu

We worked on implementing the cache an equal amount. While one person was coding, the other would talk it through and vice versa. There were a few functions that we implemented separately, but overall we communicated and shared our progress an equal amount. We worked on the README after implementing fifo together. 


Cache Results Report:

To find the best cache, we used the gcc.trace memory trace to determine which configuration has the best overall effectiveness. We changed the standard input variables (sets, blocks, bytes in each block, and the policies) to see where the number of cycles was smallest. The effectiveness of the cache depends on the number of cycles since there would be more load/stores hits than load/store misses. We did not test no-write-allocate and write-back. 

First, we changed the write and replacement policy parameters: 

Write and replacement policy tests:
./csim 512 8 32 no-write-allocate write-through lru < gcc.trace
./csim 512 8 32 no-write-allocate write-through fifo < gcc.trace
./csim 512 8 32 write-allocate write-through lru < gcc.trace
./csim 512 8 32 write-allocate write-through fifo < gcc.trace
./csim 512 8 32 write-allocate write-back fifo < gcc.trace

Write and replacement polic test results:
./csim 512 8 32 no-write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314395
Load misses: 3802
Store hits: 169731
Store misses: 27755
Total cycles: 23278128

./csim 512 8 32 no-write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314359
Load misses: 3838
Store hits: 169701
Store misses: 27785
Total cycles: 23306898

./csim 512 8 32 write-allocate write-through lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316595
Load misses: 1602
Store hits: 192939
Store misses: 4547
Total cycles: 25183483

./csim 512 8 32 write-allocate write-through fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316488
Load misses: 1709
Store hits: 192913
Store misses: 4573
Total cycles: 25289883

./csim 512 8 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316595
Load misses: 1602
Store hits: 192939
Store misses: 4547
Total cycles: 6634083

./csim 512 8 32 write-allocate write-back fifo < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316488
Load misses: 1709
Store hits: 192913
Store misses: 4573
Total cycles: 6806883

After observing the results of each test, we came to the conclusion that write-allocate and write-back with the replacement policy of lru was optimal for the best cache configuration. Thus, in our next few tests, we kept these variables constant.

In our next tests, we changed size of the set, the size of the block, as well as the size of the bytes in each block to test the differnet types of cache: direct-mapped, set associative, and fully associative. 


Direct-mapped caches tests:
./csim 64 1 64 write-allocate write-back lru < gcc.trace
./csim 32 1 64 write-allocate write-back lru < gcc.trace 
./csim 16 1 64 write-allocate write-back lru < gcc.trace

N-way set associative caches tests:
./csim 16 1024 64 write-allocate write-back lru < gcc.trace
./csim 8 512 64 write-allocate write-back lru < gcc.trace
./csim 4 256 64 write-allocate write-back lru < gcc.trace

Fully associative caches tests: 
./csim 1 64 32 write-allocate write-back lru < gcc.trace
./csim 1 128 32 write-allocate write-back lru < gcc.trace
./csim 1 256 32 write-allocate write-back lru < gcc.trace

Cache test results:
./csim 64 1 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 297445
Load misses: 20752
Store hits: 191437
Store misses: 6049
Total cycles: 61326883

./csim 32 1 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 272992
Load misses: 45205
Store hits: 185743
Store misses: 11743
Total cycles: 123750883

./csim 16 1 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 249446
Load misses: 68751
Store hits: 179410
Store misses: 18076
Total cycles: 188582883

./csim 16 1024 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 5902883

./csim 8 512 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 317204
Load misses: 993
Store hits: 195112
Store misses: 2374
Total cycles: 5902883

./csim 4 256 64 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316994
Load misses: 1203
Store hits: 195069
Store misses: 2417
Total cycles: 9669283

./csim 1 64 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 308097
Load misses: 10100
Store hits: 191177
Store misses: 6309
Total cycles: 20150883

./csim 1 128 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314052
Load misses: 4145
Store hits: 192202
Store misses: 5284
Total cycles: 13010083

./csim 1 256 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315140
Load misses: 3057
Store hits: 192496
Store misses: 4990
Total cycles: 11499683


After observing the results of these tests, we concluded that the direct-mapped cache had the greatest number of cycles, meaning it had the least efficient cache configuration. On the other hand, the fully associative cache had the least total number of cycles, meaning it was more efficient than the direct-mapped cache and the set associative cache. Thus, in our next few tests, the policies (wriet-allocate, write-back, lru) as well as the type of cache will remain constant.

Lastly, we will change the number of blocks (the second parameter) to find the best configuration of the cache.

./csim 1 128 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314052
Load misses: 4145
Store hits: 192202
Store misses: 5284
Total cycles: 13010083

./csim 1 256 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315140
Load misses: 3057
Store hits: 192496
Store misses: 4990
Total cycles: 11499683

./csim 1 512 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 315889
Load misses: 2308
Store hits: 192698
Store misses: 4788
Total cycles: 10344483

./csim 1 1024 32 write-allocate write-back lru < gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 316200
Load misses: 1997
Store hits: 192867
Store misses: 4619
Total cycles: 9386883

In conclusion, fully associative caches using "write-allocate write-back lru" are most efficient. The block size and cycles amounts are inverses of each other in terms of efficiency.  
However, from an engineering perspective it is impractical to have such large caches and block sizes to decrease the amount of cycles.