#include "zwalker.h"
#include "zhasher.h"

#ifndef RENDER_H
#define RENDER_H

#define zrender_add_item(LIST,ELEMENT,SIZE,LEN) \
 add_item_to_list( (void ***)LIST, ELEMENT, sizeof( SIZE ), LEN )

#ifndef DEBUG_H
 #define RPRINTF(a,b,blen)

 #define ZRENDER_PRINTF(...)

#else
 #define ZRENDER_PRINTF(...) \
	fprintf( stderr, "DEBUG: %s[%d]: ", __FILE__, __LINE__ ) && \
	fprintf( stderr, __VA_ARGS__ )

 #define RPRINTF(a,b,blen) \
	memset( rprintchar, 0, sizeof(rprintchar) ); \
	memcpy( rprintchar, b, (blen >= sizeof(rprintchar)) ? sizeof(rprintchar) - 1 : blen ); \
	ZRENDER_PRINTF( "%s > %s(...): '%s'\n", a, __func__, replace_chars( rprintchar, strlen(rprintchar) ) );

#endif

#define EXTRACTOR(name) \
	void name ( struct map ***row, uint8_t **dst, int *dlen, uint8_t *src, int len, struct ptr **ptr, void *t )

#define MAPPER(name) \
	void name ( struct map *row, struct map ***parent, int *plen, unsigned char *ptr, int len, void *t )

enum {
	RAW = 0,
	SIMPLE_EXTRACT = 32,
	BOOLEAN = 33,
	LOOP_START = 35,
	LOOP_END = 47,
	COMPLEX_EXTRACT = 46,
	EACH_KEY = 36,
	EXECUTE = 96,
	TERM = -2,
	UNINIT = 127,
};

struct map { 
	char action; 
	int **hashList, **olist;
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

//call it arg for now
struct arg {
	struct map **index; //?
	struct map **parent; //?
	int *plen;  //length of parent array (not sure why we need it yet)
	int *dlen;  //length written to destination
	int current; //Current index of whatever we may be dealing with
	int children; //How many children of whatever we may be dealing with
	unsigned char *dest; //Destination block
	void *userdata;	//Userdata of whatever we may be dealing with
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
	struct map **map;	
	struct zrSet *mapset[128]; //takes up more space
	int error;
	char errmsg[1024];
} zRender;

void extract_table_value( zKeyval *, uint8_t **, int *, uint8_t *, int );

uint8_t *zrender_render ( zRender *, const uint8_t *, int, int * );

int zrender_check_syntax ( zRender *, const uint8_t *, int );

void zrender_free_map( struct map **map );

int * zrender_copy_int ( int i ) ;

uint8_t *zrender_trim ( const uint8_t *msg, const char *trim, int len, int *nlen ) ;

zRender * zrender_init();

void zrender_set_boundaries ( zRender *, const char *, const char *);

void zrender_set_fetchdata( zRender *, void * );

void zrender_set( zRender *, const char, Mapper, Extractor );

void zrender_set_default_dialect( zRender *rz ) ;

unsigned char *zrender_map_to_uint8t ( zRender *, int * );

struct map ** zrender_userdata_to_map ( zRender *, const unsigned char *, int );

void zrender_free( zRender * );

const char * zrender_strerror( zRender * );

#ifdef DEBUG_H

 void zrender_print_map( zRender * );

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
 #define zrender_print_map(a)
 #define DUMPACTION( NUM )

#endif

#endif
