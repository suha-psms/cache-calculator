#include "csim.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

using namespace std;


Cache_Variables* store_cache_and_program_variables(int sets, int blocks, int block_size, string param_write_policy_for_cache_miss_during_store, string param_write_policy_for_whether_a_store_always_writes_to_memory_immediately, string param_block_replacement_policy) {
  Cache_Variables* cache_var = new Cache_Variables();
  
  cache_var->load_hits = 0;
  cache_var->load_misses = 0;
  cache_var->store_hits = 0;
  cache_var->store_misses = 0;
  cache_var->cycles = 0;

  cache_var->num_sets = sets;
  cache_var->num_blocks_per_set = blocks;
  cache_var->bytes_per_block = block_size;

  cache_var->write_policy_for_cache_miss_during_store = param_write_policy_for_cache_miss_during_store;
  cache_var->write_policy_for_whether_a_store_always_writes_to_memory_immediately = param_write_policy_for_whether_a_store_always_writes_to_memory_immediately;
  cache_var->block_replacement_policy = param_block_replacement_policy;

  return cache_var;
}


//Convert hex address into unsigned number
unsigned int convert_hex_string_into_unsigned(string hex_address) {
  return stoul(hex_address, nullptr, 16);
}


//Determine the length by getting the exponent of the 2 base exponent
int get_actual_length(int num_blocks_sets_or_bytes) {
    int counter = 0;

    if (num_blocks_sets_or_bytes < 0) {
      cerr << "Error: number is negative" << endl;
        return -1;
    }

    while (num_blocks_sets_or_bytes > 1) {
      //if num is odd / not a power of 2, return -1
        if (num_blocks_sets_or_bytes % 2 == 1) {
          cerr << "Error: number is not a power of 2" << endl;
            return -1;
        }
        num_blocks_sets_or_bytes = num_blocks_sets_or_bytes >> 1;
        counter++;
    }
    return counter;
}

//finds block in set
int locate_block(unsigned int tag, vector<Block> &set) {
  
  for (int i = 0; i < (int) set.size(); i++) {
    //if tag is found in cache
    if (set[i].first == tag) {
      if (set[i].second.first != -1) {
      return i;
      }
    }
  }
  return -1; 
}


// put in tag to empty block
void load_tag_into_empty_block(int ind, unsigned int tag, vector<Block> &cache, Cache_Variables* cache_var) {
  //loads given tag onto empty block in the set
  cache[ind].first = tag;
  cache[ind].second.first = 0; // valid bit
  cache[ind].second.second = cache_var->cycles; //lru timestamp
}

//store a block in the specified set
int store_block_for_given_set(unsigned int tag, vector<Block> &set, Cache_Variables* cache_var) {
  //loads given tag onto empty block in the set
  for (int i = 0; i < (int) set.size(); i++) {
    if (set[i].second.first == -1) {
      load_tag_into_empty_block(i, tag, set, cache_var);
      return i;
    }
  }
  return -1; 
}

// Determine the block with the lowest LRU timestamp
int find_block_with_LRU( unsigned int ind, unsigned int tag, Cache &set, Cache_Variables* cache_var, unsigned int &cycles, int &lru_block) { 
  // find block using LRU
  for (int i = 0; i < (int) set[ind].size(); i++) {
    // if block is empty, load tag into it
    if(cycles > set[ind][i].second.second){
      cycles = set[ind][i].second.second;
      lru_block = i;
    }
  }
  return lru_block;
}



//Load block into cache and update cycles
void load_fail(int index_of_block, unsigned int ind, unsigned int tag, Cache &cache, Cache_Variables* cache_var) {
  //if no empty block is found in set and replacement policy is lru 
  if(index_of_block == -1 && cache_var->block_replacement_policy == "lru"){
    
    //LRU cycle
    unsigned int lru_cycles = cache_var->cycles;
    int lru_bi = -1;
    
    //find block with LRU
    lru_bi = find_block_with_LRU(ind, tag, cache, cache_var, lru_cycles, lru_bi);
    
    // if block is dirty, write back to memory
    if (cache[ind][lru_bi].second.first == 1){
     cache_var->cycles = cache_var->cycles +  (100 * (cache_var->bytes_per_block >> 2));
    }
    //load new block into cache
    cache[ind][lru_bi].second.first = -1;

    index_of_block = store_block_for_given_set(tag, cache[ind], cache_var); // load again- now with empty block
  
  }

 //FIFO cycle
  else if (index_of_block == -1 && cache_var->block_replacement_policy == "fifo") { //must be "fifo"
      int fifo_two = -1;
      int fifo_cycles = cache_var->cycles;

      //locate fifo block index
      for (int i = 0; i < cache_var->num_blocks_per_set; i++){
        if (fifo_cycles > cache_var->fifo[ind][i]){
          fifo_cycles = cache_var->fifo[ind][i];
          fifo_two = i;
        }
      }

      if (cache[ind][fifo_two].second.first == 1){
          //Dirty
          cache_var->cycles = cache_var->cycles + (100 * (cache_var->bytes_per_block >> 2));
      }

      cache[ind][fifo_two].second.first = -1;
      index_of_block = store_block_for_given_set(tag, cache[ind], cache_var);
  }

  //fifo timestamp
  cache_var->fifo[ind][index_of_block] = cache_var->cycles;
  cache_var->cycles += 1;
  
}

//load block into cache and update cycles
int read(unsigned int ind, unsigned int tag, Cache &cache, Cache_Variables* cache_var) {
  int block_ind = locate_block(tag, cache[ind]); //find block in set

  // if block is found in cache
  if (block_ind != -1) {
    // change timestamp
    cache[ind][block_ind].second.second = cache_var->cycles;
    cache_var->cycles++;
    return 1;
  }
  
  //store block in cache
  cache_var->cycles = cache_var->cycles + (100 * (cache_var->bytes_per_block >> 2));
  block_ind = store_block_for_given_set(tag, cache[ind], cache_var); //try loading into empty block
  load_fail(block_ind, ind, tag, cache, cache_var); 

  return 0;
}

//update cycles for store
void update_cycles(unsigned int ind, unsigned int tag, Cache &cache, Cache_Variables* cache_var, int ind_of_block) { 
  // if block is found in cache
  cache[ind][ind_of_block].second.second = cache_var->cycles;
  if (cache_var->write_policy_for_whether_a_store_always_writes_to_memory_immediately == "write-through") {
    cache_var->cycles = cache_var->cycles + 101;
  }
  else { //write-back
    // set dirty bit
    cache[ind][ind_of_block].second.first = 1;
    cache_var->cycles++; 
  }
}

//store a block in the specified set for write-allocate and write through
void write_allocate_through(unsigned int ind, unsigned int tag, Cache &cache, Cache_Variables* cache_var, int ind_of_block) { 

  if (cache_var->write_policy_for_cache_miss_during_store == "write-allocate") {
    read(ind, tag, cache, cache_var);
    if (cache_var->write_policy_for_whether_a_store_always_writes_to_memory_immediately == "write-through") {
      cache_var->cycles = cache_var->cycles + 100;
    }
    else {
      // set dirty bit
      ind_of_block= locate_block(tag, cache[ind]);
      cache[ind][ind_of_block].second.first = 1; 
    } 
  }
  else { 
    // add 100 cycles for write-through
    if (cache_var->write_policy_for_whether_a_store_always_writes_to_memory_immediately == "write-through") {
      cache_var->cycles = cache_var->cycles + 100;
    }
  }
}

//  locates block in set with given tag and writes to the cache
int write(unsigned int ind, unsigned int tag, Cache &cache, Cache_Variables* cache_var) {
  int block_index = locate_block(tag, cache[ind]); //find block in set
  int num; 
  
  if (block_index != -1) { //if block is found in cache
    update_cycles(ind, tag, cache, cache_var, block_index); 
    return 1;
  }

  //else block was not found in the cache
  write_allocate_through(ind, tag, cache, cache_var, block_index); 
  return 0;
}

//returns the index of the block with the given tag
unsigned int determine_index_mask(int ind) {
  // create index mask
  unsigned int index_mask = 0;
  for (int i = 0; i < ind; ++i) {
   index_mask <<= 1;
    index_mask++;
  }
  return index_mask;
}

//Write or read based off of input
int execute_store_load(char store_load, unsigned int off_index, unsigned int tag, Cache &cache, Cache_Variables* cache_var) {
  // execute store or load
  if (store_load == 's') {
    return write(off_index, tag, cache, cache_var);
  }
  else if (store_load == 'l') {
    return read(off_index, tag, cache, cache_var);
  }
  else {
    cout << "Error: invalid store_load" << endl;
    return -1;
  }
}

//Creates tag and index from address and calls store or load based on input 
int access_cache(string access, Cache &cache, Cache_Variables* cache_var, int index, int offset) {
    
    stringstream ss(access);
    
    //s or l
    char store_load;
    ss >> store_load;
    
    
    unsigned int hex_address = 0;
    string address;
    ss >> address;
    
    //convert address to hex
    hex_address = convert_hex_string_into_unsigned(address);

    int ignore_third_var;
    ss >> ignore_third_var; // Ignore third variable

    // Get index using offset
    unsigned int index_mask = determine_index_mask(index);
    unsigned int off_index = (hex_address >> offset) & index_mask; // set ID/index
    unsigned int tag = hex_address >> (offset + index); //tag
    
    return execute_store_load(store_load, off_index, tag, cache, cache_var);
}

 //Update the cache variables
 int account_for_store_or_load_and_hit_or_miss(Cache_Variables *cache_var, char s_or_l, int hit) {
  int curr_store_hits = cache_var->store_hits;
  int curr_store_misses = cache_var->store_misses;
  int curr_load_hits = cache_var->load_hits;
  int curr_load_misses = cache_var->load_misses;

   if (s_or_l == 's' && hit) {
    curr_store_hits+= 1;
    cache_var->store_hits = curr_store_hits;
   } else if (s_or_l == 's' && !hit) {
     curr_store_misses+= 1;
     cache_var->store_misses = curr_store_misses;
   } else if (s_or_l == 'l' && hit) {
    curr_load_hits+= 1;
     cache_var->load_hits = curr_load_hits;
   } else if (s_or_l == 'l' && !hit) {
     curr_load_misses+= 1;
     cache_var->load_misses+= 1;
   } else {
     cerr << "Error: input not valid" << endl;
     return 1;
 }
 return 0;
 }
