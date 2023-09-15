#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

using std::pair;
using std::vector;
using std::string;

//block 
typedef pair<unsigned int, pair<int, unsigned int>> Block;
//Cache is a vector of sets (not given a name), which are vectors of blocks 
typedef vector< std::vector<Block> > Cache;

struct Cache_Variables {
  int** fifo;
  int num_sets;
  int num_blocks_per_set;
  int bytes_per_block;
  string write_policy_for_cache_miss_during_store;
  string write_policy_for_whether_a_store_always_writes_to_memory_immediately;
  string block_replacement_policy;

  int load_hits;
  int load_misses;
  int store_hits;
  int store_misses;
  int cycles;
};


Cache_Variables* store_cache_and_program_variables(int sets, int blocks, int block_size, string param_write_policy_for_cache_miss_during_store, string param_write_policy_for_whether_a_store_always_writes_to_memory_immediately, string param_block_replacement_policy);

unsigned int convert_hex_string_into_unsigned(string addr);

unsigned int determine_index_mask(int index);

int execute_store_load(char store_load, unsigned int off_index, unsigned int tag, Cache &cache, Cache_Variables* cache_var);

int get_actual_length(int num_blocks_sets_or_bytes);

int access_cache(string access, Cache &cache, Cache_Variables* cache_var, int index, int offset);

int locate_block(unsigned int tag, vector<Block> &cache_set);

int store_block_for_given_set(unsigned int tag, vector<Block> &set, Cache_Variables* cache_var);

int write(unsigned int index, unsigned int tag, Cache &cache, Cache_Variables* cache_var);

int read(unsigned int index, unsigned int tag, Cache &cache, Cache_Variables* cache_var);

int account_for_store_or_load_and_hit_or_miss(Cache_Variables *cache_var, char s_or_l, int hit);

void load_fail(int block_index, unsigned int index, unsigned int tag, Cache &cache, Cache_Variables* cache_var);

int find_block_with_LRU( unsigned int index, unsigned int tag, Cache &cache, Cache_Variables* cache_var, unsigned int &lru_cycles, int &lru_bi); 

void update_cycles( unsigned int index, unsigned int tag, Cache &cache, Cache_Variables* cache_var, int block_index);
#endif