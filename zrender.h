/* ------------------------------------------------------- *
zrender.h
=========

Enables zTables (and eventually other data structures in C)
to be used in templating.
 * ------------------------------------------------------- */
#include "zwalker.h"
#include "zhasher.h"

#ifndef ZRENDER_H
#define ZRENDER_H

#define zr_add_item(LIST,ELEMENT,SIZE,LEN) \
 add_item_to_list( (void ***)LIST, ELEMENT, sizeof( SIZE ), LEN )

enum {
	RW = 0,
	SX = 32,
	BL = 33,
	LS = 35,
	LE = 47,
	CX = 46,
	EK = 36,
	EX = 96,
	TM = -2,
	UN = 127,
}; 

struct xdesc;
struct xmap;

struct premap {
	unsigned char *ptr;
	int len;
};

struct xdesc {
	int children;
	int index;
	struct xmap *pxmap;
	struct premap **cxmap;
};

struct xmap {
	unsigned char *ptr;
	short len;
	short type;
	struct xdesc *parent;
};

typedef struct zRender {
	const char *zStart; 
	const char *zEnd;
	int error;
	char errmsg[1024];

	void *userdata;
	struct premap **premap;	
	struct xmap **xmap;
	unsigned char xmapset[128];
} zRender;


zRender * zrender_init();

void zrender_set_fetchdata( zRender *, void * ); 

void zrender_set_boundaries ( zRender *, const char *, const char * );

void zrender_set( zRender *, const char, short );

const char * zrender_strerror( zRender * );

void zrender_set_default_dialect( zRender * );

int zrender_set_marks( zRender *, unsigned char *, unsigned int );

int zrender_convert_marks( zRender *);

unsigned char * zrender_interpret( zRender *, unsigned char **, int * );

void zrender_free( zRender *);


#ifdef DEBUG_H
 #define XMAP_DUMP_LEN 3
 void print_premap ( struct premap ** );
 void print_xmap ( struct xmap ** );
 const char * print_xmap_type ( short );
#else
 #define XMAP_DUMP_LEN
 #define print_xmap(...)
 #define print_premap(...)
 #define print_xmap_type(...)
#endif

#endif
