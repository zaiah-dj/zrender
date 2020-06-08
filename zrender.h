#include "vendor/zwalker.h"
#include "vendor/zhasher.h"
#include "vendor/util.h"


#ifndef RENDER_H
#define RENDER_H

#define DUMPACTION( NUM ) \
	( NUM == LOOP_START ) ? "LOOP_START" : \
	( NUM == LOOP_END ) ? "LOOP_END" : \
	( NUM == COMPLEX_EXTRACT ) ? "COMPLEX_EXTRACT" : \
	( NUM == SIMPLE_EXTRACT ) ? "SIMPLE_EXTRACT" : \
	( NUM == EACH_KEY ) ? "EACH_KEY" : \
	( NUM == EXECUTE ) ? "EXECUTE" : \
	( NUM == BOOLEAN ) ? "BOOLEAN" : \
	( NUM == RAW ) ? "RAW" : "UNKNOWN" 

enum {
	RAW = 0,
	LOOP_START = 30,
	LOOP_END = 31,
	SIMPLE_EXTRACT = 32,
	COMPLEX_EXTRACT = 33,
	EACH_KEY = 34,
	EXECUTE = 35,
	BOOLEAN = 36,
	BLOCK_START = 100,
	BLOCK_END = 101,
	TERM = -2,
	UNINIT = 127,
};

struct parent { 
	uint8_t *text; 
	int len, pos, childCount; 
}; 

struct dep { 
	struct map **index; 
	int current, childCount; 
};

struct map { 
	int action; 
	int **hashList; 
	int len; 
	int pos; 
	void *ptr; 
	int children; 
	struct map *parent;
};

//static?
void extract_table_value( zKeyval *, uint8_t **, int *, uint8_t *, int );
struct map **table_to_map( zTable *, const uint8_t *, int );
uint8_t *map_to_uint8t( zTable *, struct map **, int * );
uint8_t *table_to_uint8t( zTable *, const uint8_t *, int, int * );

//public?
int zrender_check_balance ( const uint8_t *, int );
void zrender_free_table( struct map **map );
#if 0
int * zrender_copy_int ( int i ) {
uint8_t *zrender_trim ( uint8_t *msg, const char *trim, int len, int *nlen ) {
struct map ** zrender_table_to_map ( void *t, const uint8_t *src, int srclen ) {
#endif

#ifdef DEBUG_H
void zrender_print_table ( struct map ** );
#else
#define zrender_print_table(a)
#endif
#endif




