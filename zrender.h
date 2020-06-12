#include "vendor/zwalker.h"
#include "vendor/zhasher.h"
#include "vendor/util.h"


#ifndef RENDER_H
#define RENDER_H

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

#if 0
struct parent { 
	uint8_t *text; 
	int len, pos, childCount; 
}; 
#endif

struct map { 
	char action; 
	int **hashList; 
	int len; 
	int pos; 
	void *ptr; 
	int children; 
	struct map *parent;
};

//You can alternately just get rid of this by allocating enough space for the new things
struct ptr {
	struct map **index;
	int current;
	int children;	
};

typedef void (*Mapper)
	( struct map *, struct map ***, int *, uint8_t *, int, void * );

typedef void (*Extractor)
	( struct map ***, uint8_t **, int *, uint8_t *, int, struct ptr **, void * );

struct zrSet {
	Mapper mapper;
	Extractor extractor;
};

typedef struct zRender {
	const char *zStart, *zEnd;
	void *userdata;
	int zrlen;
	struct zrSet *mapset[128]; //takes up more space
} zRender;

//static?
void extract_table_value( zKeyval *, uint8_t **, int *, uint8_t *, int );
struct map **table_to_map( void *, const uint8_t *, int );
uint8_t *map_to_uint8t( void *, struct map **, int * );
uint8_t *table_to_uint8t( zRender *, const uint8_t *, int, int * );

//public?
int zrender_check_balance ( zRender *, const uint8_t *, int );
void zrender_free_table( struct map **map );
int * zrender_copy_int ( int i ) ;
uint8_t *zrender_trim ( uint8_t *msg, const char *trim, int len, int *nlen ) ;

zRender * zrender_init();
void zrender_set_boundaries ( zRender *, const char *, const char *);
void zrender_set_fetchdata( zRender *, void * );
void zrender_set( zRender *, const char, Mapper, Extractor );

#ifdef DEBUG_H
void zrender_print_table ( struct map ** );

#define DUMPACTION( NUM ) \
	( NUM == LOOP_START ) ? "LOOP_START" : \
	( NUM == LOOP_END ) ? "LOOP_END" : \
	( NUM == COMPLEX_EXTRACT ) ? "COMPLEX_EXTRACT" : \
	( NUM == SIMPLE_EXTRACT ) ? "SIMPLE_EXTRACT" : \
	( NUM == EACH_KEY ) ? "EACH_KEY" : \
	( NUM == EXECUTE ) ? "EXECUTE" : \
	( NUM == BOOLEAN ) ? "BOOLEAN" : \
	( NUM == RAW ) ? "RAW" : "UNKNOWN" 
#else
#define zrender_print_table(a)
#endif

#endif




