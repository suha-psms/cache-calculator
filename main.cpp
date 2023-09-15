#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "csim.h"

using namespace std;


bool check_numsets(int num_sets) {
   if (num_sets < 1) { // there must be at least one set or else no cache lines in the cache and cache cannot store any data
       cerr << "Error: Number of sets must be a positive power-of-2" << endl;
       return false;
   }
   if (num_sets & (num_sets - 1)) {
       cerr << "Error: Number of sets must be a positive power-of-2" << endl;
       return false;
   }
  
   return true;
}

bool check_num_blocks_per_set(int num_blocks_per_set) {
   if (num_blocks_per_set < 1) { //there must be at least one block per set or else there would be no space in any set to store data in the cache
       cerr << "Error: Number of blocks per set must be a positive power-of-2" << endl;
       return false;
   }
   if (num_blocks_per_set & (num_blocks_per_set - 1)) {
       cerr << "Error: Number of blocks per set must be a positive power-of-2" << endl;
       return false;
   }

   return true;
}


bool check_num_bytes_per_block(int num_bytes_per_block) {
   if (num_bytes_per_block < 4) { //there must be at least 4 bytes per block or else there would be no space in any block to store data in the cache
       cerr << "Error: Number of bytes per block must be a positive power-of-2 and at least 4" << endl;
       return false;
   }
   if (num_bytes_per_block & (num_bytes_per_block - 1)) {
       cerr << "Error: Number of bytes per block must be a positive power-of-2 and at least 4" << endl;
       return false;
   }

   return true;
}

bool check_write_policy_for_cache_miss_during_store(string write_policy_for_cache_miss_during_store) {
   if (write_policy_for_cache_miss_during_store != "write-allocate" && write_policy_for_cache_miss_during_store != "no-write-allocate") {
       cerr << "Error: Write policy for cache miss during a store must be write-allocate or no-write-allocate" << endl;
       return false;
   }
   return true;
}

bool check_write_policy_for_whether_a_store_always_writes_to_memory_immediately(string write_policy_for_whether_a_store_always_writes_to_memory_immediately) {
   if (write_policy_for_whether_a_store_always_writes_to_memory_immediately != "write-back" && write_policy_for_whether_a_store_always_writes_to_memory_immediately != "write-through") {
       cerr << "Error: Write policy for whether a store always writes to memory immediately must be write-back or write-through" << endl;
       return false;
   }

   return true;
}

bool check_block_replacement_policy(string block_replacement_policy) {
   if (block_replacement_policy != "lru" && block_replacement_policy != "fifo") {
       cerr << "Error: Block replacement policy must be lru or fifo" << endl;
       return false;
   }

   return true;
}

int main(int argc, char* argv[]) {
   // Parse command-line arguments
   if (argc != 7) {    // the name of the file
       cerr << "Usage: " << argv[0] << " <num_sets> <num_blocks> <block_size> <write-allocate|no-write-allocate> <write-back|write-through> <lru|fifo>" << endl;
       return 1;
   }

   //Store the number of sets in the cache (a positive power-of-2), stoi converts strings to int
   int num_sets = stoi(argv[1]);

   //Store the number of blocks in each set (a positive power-of-2), stoi converts strings to int
   int num_blocks_per_set = stoi(argv[2]);

   //Store the number of bytes in each block (a positive power-of-2), stoi converts strings to int
   //Must be at least 4
   int num_bytes_per_block = stoi(argv[3]);

   //Store the write-allocate policy (write-allocate or no-write-allocate), string
   string write_policy_for_cache_miss_during_store = argv[4];

   //store the write-through policy (write-back or write-through), string
   string write_policy_for_whether_a_store_always_writes_to_memory_immediately = argv[5];

   //store the block replacement policy (lru or fifo), string
   string block_replacement_policy = argv[6];


   bool valid_input = true;
   //Determine if input is valid
   valid_input = check_numsets(num_sets);
   valid_input = check_num_blocks_per_set(num_blocks_per_set);
   valid_input = check_num_bytes_per_block(num_bytes_per_block);
   valid_input = check_write_policy_for_cache_miss_during_store(write_policy_for_cache_miss_during_store);
   valid_input = check_write_policy_for_whether_a_store_always_writes_to_memory_immediately(write_policy_for_whether_a_store_always_writes_to_memory_immediately);
   valid_input = check_block_replacement_policy(block_replacement_policy);

   //If input is not valid, exit with a non-zero exit code
   if (!valid_input) {
       return 1;
   }

   if (write_policy_for_cache_miss_during_store == "no-write-allocate" && write_policy_for_whether_a_store_always_writes_to_memory_immediately == "write-back") {
       cerr << "Error: Write policy for whether a store always writes to memory immediately must be write-through if write policy for cache miss during a store is no-write-allocate" << endl;
       return 1;
   }

   //Input was valid, continue with the program


 //Make sure there are 7 arguments including executable)
 if (argc != 7) {
   cerr << "Error: invalid number of args" << endl;
   return 1;
 }


 //Create cache_variables struct
 Cache_Variables* cache_var = store_cache_and_program_variables(num_sets, num_blocks_per_set, num_bytes_per_block, write_policy_for_cache_miss_during_store, write_policy_for_whether_a_store_always_writes_to_memory_immediately, block_replacement_policy);


 // Find lengths of index, offset, and tags lengths
 int block = get_actual_length(num_blocks_per_set);
 int indx = get_actual_length(num_sets);
 int offset = get_actual_length(num_bytes_per_block);

  
  // Set up fifo 
  cache_var->fifo = new int*[cache_var->num_sets];
  for (int i = 0; i < cache_var->num_sets; i++) {
    cache_var->fifo[i] = new int[cache_var->num_blocks_per_set];
  }

 // Cache cannot be smaller
 if (indx < 0 ) {
   cerr << "Error: cache index is too small" << endl;
   return 1;
 }
 if (block < 0) {
   cerr << "Error: cache blocks are too small" << endl;
   return 1;
 }
  if (offset < 2) {
   cerr << "Error: cache offset is too small" << endl;
   return 1;
 }
  // Make cache which is a vector of sets that are a vector of blocks
 Cache cache(cache_var->num_sets, vector<Block>(cache_var->num_blocks_per_set, std::make_pair(0, std::make_pair(-1, 0))));
 string file_memory_access;
 stringstream ss;

 while (getline(cin, file_memory_access)) {
   // hit is 1 if hit, 0 if miss
   int hit = access_cache(file_memory_access, cache, cache_var, indx, offset);
   if (hit < 0) {
     //program failed
     return 1;
     cerr << "Error: program failed" << endl;
   }


   int success =  account_for_store_or_load_and_hit_or_miss(cache_var, file_memory_access[0], hit);
  
   if (success != 0) {
     //program failed
     return 1;
     cerr << "Error: input wasn'tt valid" << endl;
   }
 }


   //Output the results
   cout << "Total loads: " << cache_var->load_hits + cache_var->load_misses << endl;
   cout << "Total stores: " << cache_var->store_hits + cache_var->store_misses << endl;
   cout << "Load hits: " << cache_var->load_hits << endl;
   cout << "Load misses: " << cache_var->load_misses << endl;
   cout << "Store hits: " << cache_var->store_hits << endl;
   cout << "Store misses: " << cache_var->store_misses << endl;
   cout << "Total cycles: " << cache_var->cycles << endl;



  //Free memory that was dynamically allocated for fifo
  for (int i = 0; i < cache_var->num_sets; i++) {
    delete[] cache_var->fifo[i];
  }
    delete[] cache_var->fifo;
    delete cache_var;

    //Succcessfully completed the program
   return 0;
   }

