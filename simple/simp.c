// Try this whole thing way differently
#include "../zwalker.h"
#include "../zhasher.h"
#include "../zrender.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "util.c"
#include "table.c"

#define XMAP_DUMP_LEN 3

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
}; //Can I name this?


#if 0
struct premap {
	unsigned char *ptr;
	int len;
};
#endif


struct xparent {
	unsigned char *ptr;
	int children;
	int *index;
	struct map *parent;
};


struct xmap {
	unsigned char *ptr;
	short len;
	short type;
	struct parent *parent;
};


typedef struct xRender {
	const char *zStart;
	const char *zEnd;
	int error;
	char *errmsg;

	void *userdata;
	struct premap **premap;
	struct xmap **map;
	struct zrSet *exec[128];
} xRender;


const char *xmap_types[] = {
	"RAW"
, "SIMPLE EXTRACT"
, "BOOLEAN"
, "LOOP START"
, "LOOP END"
, "COMPLEX EXTRACT"
, "EACH KEY"
, "EXECUTE"
, "TERMINATOR"
, "UNINITIALIZED"
};


const char * print_xmap_type ( short type ) {
	const char *a;
	( type == RW ) ? a = xmap_types[ 0 ] : 0;
	( type == SX ) ? a = xmap_types[ 1 ] : 0;
	( type == BL ) ? a = xmap_types[ 2 ] : 0;
	( type == LS ) ? a = xmap_types[ 3 ] : 0;
	( type == LE ) ? a = xmap_types[ 4 ] : 0;
	( type == CX ) ? a = xmap_types[ 5 ] : 0;
	( type == EK ) ? a = xmap_types[ 6 ] : 0;
	( type == EX ) ? a = xmap_types[ 7 ] : 0;
	( type == TM ) ? a = xmap_types[ 8 ] : 0;
	( type == UN ) ? a = xmap_types[ 9 ] : 0;
	return a;
};



//Dump premaps
void print_premap ( struct premap **p ) {
	while ( p && *p ) {
		fprintf( stderr, "%p => %3d, ", *p, (*p)->len );
		write( 2, (*p)->ptr, (*p)->len < XMAP_DUMP_LEN ? (*p)->len : XMAP_DUMP_LEN );
		write( 2, "...\n", 4 );
		p++;
	}
}

//Dump maps 
void print_xmap ( struct xmap **p ) {
	while ( p && *p ) {
		fprintf( stderr, "%p => ", *p );
		fprintf( stderr, "%6s, ", print_xmap_type( (*p)->type ) );
		fprintf( stderr, "%3d, ", (*p)->len );
		fprintf( stderr, "%p, ", (*p)->parent );
		write( 2, (*p)->ptr, (*p)->len < XMAP_DUMP_LEN ? (*p)->len : XMAP_DUMP_LEN );
		write( 2, "...\n", 4 );
		p++;
	}
}

//Initialize a premap
struct premap * init_premap() {
	struct premap * p = malloc ( sizeof( struct premap ) );
	return ( !p ) ? NULL : memset( p, 0, sizeof( struct premap ) ); 
}


//Initialize a new xmap
struct xmap * init_xmap() {
	struct xmap * p = malloc ( sizeof( struct xmap ) );
	return ( !p ) ? NULL : memset( p, 0, sizeof( struct xmap ) ); 
}


//Destroy the premap
void free_premap( struct premap **premap ) {
	struct premap **p = premap;
	while ( p && *p ) {
		free( *p );
		p++;
	}	
	free( premap );
}


//Destroy the map
void free_xmap( struct xmap **map ) {
	struct xmap **p = map;
	while ( p && *p ) {
		free( *p );
		p++;
	}
	free( map );
}


//mapping / marking
int zrender_set_marks( zRender *rz, unsigned char *src, unsigned int srclen ) {
	zWalker r = {0};
	unsigned char check[] = { rz->zStart[0], rz->zEnd[0], '\n' };
	struct pos { int a, b, l, lp; } set[128] = {0}; 
	int ls=0, le=0;
	int sl = 0, el = 0, nl = 0;

	//Early marking list goes here
	struct premap **premap = NULL;
	int pmlen = 0;

	//just check that the list is balanced
	while ( memwalk( &r, src, check, srclen, sizeof(check) ) ) {
		struct premap *p = init_premap(); 

		// [[ TRACK NEWLINES FOR THE PURPOSE OF SYNTAX CHECK ]]
		if ( r.chr == '\n' ) {
			nl++, set[ sl ].lp = r.pos;
			p->ptr = r.src;	
			p->len = r.size;
		}

		// [[ RAW LOOPS ]]
		else if ( r.chr == check[0] && *r.ptr == check[0] ) {
			set[ sl ].a = r.pos, set[ sl ].l = nl, sl++;
			p->ptr = r.src;	
			p->len = r.size - 1;
		}

		// [[ INNER LOOPS ]]
		else if ( r.chr == check[1] && *r.ptr == check[1] ) {
			set[ el ].b = r.pos;
			#if 1
			//Do another check for the end sequence before a start sequence
			if ( el > sl ) {
				//rz->error = ZRENDER_SYNTAX_LOOP;
				//TODO: Add exact positioning later
				const char fmt[] = "Loop end sequence detected at line %d, but no loop start found\n";
				snprintf( rz->errmsg, 1024, fmt, nl ); 
				return 0;	
			}
			#endif
			unsigned char * ts = zrender_trim( r.src, " ", r.size - 1, NULL ); 
			if ( *ts == '#' )
				ls++;
			else if ( *ts == '/' ) {
				//TODO: Add exact positioning later
				if ( ++le > ls ) {
					//rz->error = ZRENDER_SYNTAX_LOOP;
					const char fmt[] = "Loop end detected at line %d, but no loop start found\n";
					snprintf( rz->errmsg, 1024, fmt, nl ); 
					return 0;	
				}
			}
			p->ptr = r.src - 1 ;
			p->len = r.size + 1;
			el++;
		}

		//[[ ANYTHING ELSE ]]
		else if ( *r.src != check[0] && *r.src != check[1] ) {  
			p->ptr = r.src;
			p->len = ( *r.ptr == check[0] ) ? r.size - 1 : r.size; 
		}

		if ( p->len )
			zrender_add_item( &premap, p, struct premap *, &pmlen );
		else {
			free( p );
		}
	}

	rz->premap = premap;
	return 1;
}


//expanding to map
int zrender_convert_marks( zRender *rz ) {
	print_premap( rz->premap );	
	struct premap **pmap = rz->premap;
	struct xmap **xmap = NULL;
	int xmaplen = 0;

	while ( pmap && *pmap ) {
		struct xmap *xp = init_xmap();
		struct premap *pp = *pmap;

		//Raw write first
		if ( *pp->ptr != '{' ) {
			xp->type = RW;
			xp->len = pp->len;
			xp->ptr = pp->ptr;
			zrender_add_item( &xmap, xp, struct xmap *, &xmaplen );
		}

		//Handle other cases here				
		else {

		} 	
		pmap++;
	}

	print_xmap( xmap );
	return 1;
}


//rendering...
int zrender_interpret( zRender *rz ) {
	return 0;
}


int main (int argc, char *argv[]) {
	
	const char testsrc[] = "../tests/src/TABLE_SINGLE";
	unsigned char *dest, *src = read_file( testsrc );
	int srclen = strlen( (char *)src ); 
	
	zTable *t = convert_lkv( SinglezTable ); 
	zRender *rz = zrender_init();
	zrender_set_default_dialect( rz );
	zrender_set_fetchdata( rz, t );
	lt_dump( t );

	if ( !zrender_set_marks( rz, src, srclen ) ) {
		fprintf( stderr, "set_marks failed\n" ); return 1;
	}

	if ( !zrender_convert_marks( rz ) ) {
		fprintf( stderr, "convert_marks failed\n" ); return 1;
	}

	if ( !zrender_interpret( rz ) ) {
		fprintf( stderr, "interpret failed\n" ); return 1;
	}
	
	return 0;
}
