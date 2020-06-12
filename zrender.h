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

#if 0
//Doing this makes sense...
zRender *rz = zrender_init();

//This is a lot longer than I would like, but it kind of works
zrender_setstart( rz, "{{" ); 
zrender_setend( rz, "}}" ); 
zrender_setraw( rz, map_ls, extract_ls );
zrender_setsrcdata( rz, t );

//These all set something
zrender_set( rz, '#', ZRENDER_LOOP_START, map_ls, extract_ls );
zrender_set( rz, '/', ZRENDER_LOOP_END, map_le, extract_le );
zrender_set( rz, ' ', ZRENDER_SIMPLE_EXTRACT, map_se, extract_se );
zrender_set( rz, '.', ZRENDER_COMPLEX_EXTRACT, map_ce, extract_ce );
zrender_set( rz, '$', ZRENDER_EACH_KEY, map_ek, extract_ek );
zrender_set( rz, '`', ZRENDER_EXECUTE, map_ex, extract_ex );
zrender_set( rz, '!', ZRENDER_BOOLEAN, map_bl, extract_bl );

//... do templating

//Destroy
zrender_free( rz );
#endif

#if 0 
//Most templating looks like this
uint8_t *buf = NULL, *src = 
#include "myfile_with_template_data"
;
zrender_oneshot( rz, src, sizeof(src), &buf, &buflen );
#endif

#if 0
//Another way
zRender rz = {
	.start = "{{",
	.end = "}}",
	.raw = { 0, map_raw, extract_raw },
	.loopStart = { '#', map_ls, extract_ls },
	.loopEnd = { '/', map_ls, extract_ls },
	.simpleExtract = { '#', map_ls, extract_ls },
	.complexExtract = { '.', map_ls, extract_ls },
	.eachKey = { '$', map_ls, extract_ls },
	.execute = { '`', map_ls, extract_ls },
	.boolean = { '!', map_ls, extract_ls },
	.userdata = t
};

#endif

#if 0
//Yet another way to do it
zRender *rz = zrender_init();
zrender_setboundaries( rz, "{{", "}}" ); 
zrender_setuserdata( rz, t );
zrender_setraw( rz, map_ls, extract_ls );

//These all set something
zrender_set( rz, '#', map_ls, extract_ls );
zrender_set( rz, '/', map_le, extract_le );
zrender_set( rz, ' ', map_se, extract_se );
zrender_set( rz, '.', map_ce, extract_ce );
zrender_set( rz, '$', map_ek, extract_ek );
zrender_set( rz, '`', map_ex, extract_ex );
zrender_set( rz, '!', map_bl, extract_bl );

//... do templating

//Destroy
zrender_free( rz );
#endif

//static?
void extract_table_value( zKeyval *, uint8_t **, int *, uint8_t *, int );
struct map **table_to_map( void *, const uint8_t *, int );
uint8_t *map_to_uint8t( void *, struct map **, int * );
uint8_t *table_to_uint8t( zRender *, const uint8_t *, int, int * );
//uint8_t *table_to_uint8t( zTable *, const uint8_t *, int, int * );

//public?
int zrender_check_balance ( zRender *, const uint8_t *, int );
void zrender_free_table( struct map **map );
int * zrender_copy_int ( int i ) ;
uint8_t *zrender_trim ( uint8_t *msg, const char *trim, int len, int *nlen ) ;
//struct map ** zrender_table_to_map ( void *t, const uint8_t *src, int srclen ) ;

zRender * zrender_init();
void zrender_set_boundaries ( zRender *, const char *, const char *);
void zrender_set_fetchdata( zRender *, void * );
void zrender_set( zRender *, const char, Mapper, Extractor );

#ifdef DEBUG_H
void zrender_print_table ( struct map ** );
#else
#define zrender_print_table(a)
#endif
#endif




