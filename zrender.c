#include "zrender.h"

/*
TODO: NOTES: Whatnot:

- map data structure should only be handled or worried about here
- perhaps all loops should mark their children

- using comparators instead of a full blown function for extraction / mapping might help


TODO
get the last block from whatever body

*/

#define ZIPPY

#ifdef DEBUG_H
static int di = 0;
#endif

int INSIDE = 0;

	
static const int maps[] = {
	[0  ] = RAW,
	['#'] = LOOP_START,
	['/'] = LOOP_END,
	['.'] = COMPLEX_EXTRACT,
	['$'] = EACH_KEY, 
	['`'] = EXECUTE, //PAIR_EXTRACT
	['!'] = BOOLEAN,
	//[254] = SIMPLE_EXTRACT,
	[255] = 0
};



int * zrender_copy_int ( int i ) {
	int *h = malloc( sizeof ( int ) );
	memcpy( h, &i, sizeof( int ) );
	return h; 
}


//Initialize the object
zRender * zrender_init() {
	zRender *zr = malloc( sizeof( zRender ) );
	if ( !zr ) {
		return NULL;
	}

	if ( !memset( zr, 0, sizeof( zRender ) ) ) {
		return NULL;
	}

#if 0
#ifdef ZIPPY
	const int zrset_size = sizeof( struct zrSet ) * 127; 
	if ( !( zr->mapset = malloc( zrset_size ) ) ) {
		return NULL;
	}

	if ( !memset( zr->mapset, 0, zrset_size ) ) {
		return NULL;
	}

#endif
#endif
	return zr;
}



//...
struct map * init_map () {
	struct map *rp = malloc( sizeof( struct map ) );
	if ( !rp ) {
		//Free and destroy things
		return NULL;
	}

	memset( rp, 0, sizeof( struct map ) );
	rp->action = 0; 
	rp->ptr = NULL; 
	rp->len = 0; 
	rp->hashList = NULL; 
	return rp;
}


//Set start and optional end boundaries of whatever language is chosen.
void zrender_set_boundaries ( zRender *rz, const char *s, const char *end ) {
	if ( s ) {
		rz->zStart = strdup( s );	
	}
	if ( end ) {
		rz->zEnd = strdup( end );
	}
}


//Set the source for fetching data
void zrender_set_fetchdata( zRender *rz, void *t ) { 
	rz->userdata = t;	
}


//Set each character for replacement (we assume that it's just one)
void zrender_set( zRender *rz, const char map, Mapper mp, Extractor xp ) {
	struct zrSet *record = malloc( sizeof( struct zrSet ) );
	record->mapper = mp;
	record->extractor = xp;
	rz->mapset[ (int)map ] = record;
}


//Trim an unsigned character block 
uint8_t *zrender_trim ( uint8_t *msg, const char *trim, int len, int *nlen ) {
	//Define stuff
	//uint8_t *m = msg;
	uint8_t *forwards = msg;
	uint8_t *backwards = &msg[ len - 1 ];
	int nl = len;
	int tl = strlen( trim );
	while ( nl ) {
		int dobreak = 1;
		if ( memchr( trim, *forwards, tl ) )
			forwards++, nl--, dobreak = 0;
		if ( memchr( trim, *backwards, tl ) )
			backwards--, nl--, dobreak = 0;
		if ( dobreak ) {
			break;
		}	
	}
	*nlen = nl;
	return forwards;
}


//Check that the data is balanced.
int zrender_check_balance ( zRender *rz, const uint8_t *src, int srclen ) {

	//This is the syntax to check for...
	zWalker r;
	memset( &r, 0, sizeof( zWalker ) );
	//Check these counts at the end...	
	int startList = 0, endList = 0;

	//just check that the list is balanced
	while ( memwalk( &r, src, (uint8_t *)"{}", srclen, 2 ) ) {
		if ( r.size == 0 ) {
			if ( r.chr == '{' ) {
				startList ++;
			}	
			else if ( r.chr == '}' ) {
				endList ++;
			}	
		}	
	}

	FPRINTF( "%s: %d ?= %d\n", __func__, startList, endList );
	return ( startList == endList );
}


//Bitmasking will tell me a lot...
struct map ** zrender_userdata_to_map ( zRender *rz, const uint8_t *src, int srclen ) {
	struct map **rr = NULL ; 
	struct parent **pp = NULL;
	struct map **pr = NULL;
	int rrlen = 0;
	int pplen = 0;
	zWalker r;
	memset( &r, 0, sizeof( zWalker ) );

	//The check map is now dynamically generated
	uint8_t check[] = { rz->zStart[0], rz->zEnd[0] };
	int checklen = 2;//sizeof(check);

	//Allocating a list of characters to elements is easiest.
	while ( memwalk( &r, (uint8_t *)src, check, srclen, checklen ) ) {
		//More than likely, I'll always use a multi-byte delimiter
		//FPRINTF( "MOTION == %s\n", DUMPACTION( rp->action ) );
		struct zrSet *z = NULL; 
		if ( r.size == 0 && r.chr == '{' ) {
		}
		else if ( r.chr != '}' ) {
			//We can simply copy if ACTION & BLOCK are 0 
			struct map *rp = init_map();
			rp->action = 0;  
			if ( ( z = rz->mapset[ 0 ] ) ) {
				z->mapper( rp, NULL, NULL, (uint8_t *)&src[ r.pos ], r.size, rz->userdata );
				add_item( &rr, rp, struct map *, &rrlen );
			}
		}
#if 1
		else if ( src[ r.pos + r.size + 1 ] == '}' ) {
			//Start extraction...
			int alen=0, nlen = 0;	
			struct map *rp = init_map();
			uint8_t *p = zrender_trim( (uint8_t *)&src[ r.pos ], " ", r.size, &nlen );
			rp->action = *p;  
#if 1
			if ( ( z = rz->mapset[ *p ] ) ) {
FPRINTF( "z->extractor %d, %c = %p\n", (int)rp->action, rp->action, z );
				//This should probably return some kind of error...
				p = zrender_trim( p, ". #/$`!\t", nlen, &alen );
				z->mapper( rp, &pr, &pplen, p, alen, rz->userdata ); 
				add_item( &rr, rp, struct map *, &rrlen );
			}
#else
			//Figure some things out...
			if ( rp->action == LOOP_START ) {
				map_loop_start( rp, &pr, &pplen, p, alen, t );
			}
			else if ( rp->action == LOOP_END ) {
				map_loop_end( rp, &pr, &pplen, p, alen, t );
			}
			else if ( rp->action == COMPLEX_EXTRACT ) {
				map_complex_extract( rp, &pr, &pplen, p, alen, t );
			}
			else if ( rp->action == EACH_KEY ) {
				FPRINTF( "@EACH_KEY :: Nothing yet...\n" );
			}
			else if ( rp->action == EXECUTE ) {
				FPRINTF( "@EXECUTE :: Nothing yet...\n" );
			}
			else if ( rp->action == BOOLEAN ) {
				FPRINTF( "@BOOLEAN :: Nothing yet...\n" );
			}
			else {
				map_simple_extract( rp, NULL, p, nlen, t );
			}
			add_item( &rr, rp, struct map *, &rrlen );
#endif
		}
#endif
	}

#if 0
	//Move through each of the rows 
	zrender_print_table( rr );
	//Destroy the parent list
	free( pp );
#endif
	return rr;
}



//...
uint8_t *zrender_map_to_uint8t ( zRender *rz, struct map **xmap, int *newlen ) {
	//...
	uint8_t *block = NULL;
	int blocklen = 0;
	//struct dep depths[10] = { { 0, 0, 0 } };
	//struct dep *d = depths;
	struct ptr *ptr, mptr[10] = { { 0, 0, 0 } };
	ptr = mptr;

	//...
	struct map **map = xmap;

	while ( *map ) {
		struct map *rp = *map;
		struct zrSet *z = NULL; 
		if ( ( z = rz->mapset[ (int)rp->action ] ) ) {
			z->extractor( &map, &block, &blocklen, rp->ptr, rp->len, &ptr, rz->userdata );
		}
		map++;
	}

	//The final step is to assemble everything...
FPRINTF( "BLOCK LENGTH: %d\n", blocklen );
write( 2, block, blocklen );
	*newlen = blocklen;
	return block;
}


#if 1
//uint8_t *table_to_uint8t ( zTable *t, const uint8_t *src, int srclen, int *newlen ) {
uint8_t *table_to_uint8t ( zRender *rz, const uint8_t *src, int srclen, int *newlen ) {

	//Define things
	struct map **map = NULL;
	uint8_t *block = NULL;
	int blocklen = 0;

	//TODO: Mark the place where the thing is undone
	if ( !zrender_check_balance( rz, src, srclen ) ) {
		FPRINTF( "Syntax at supplied template is wrong..." );
		return NULL;
	}

	//TODO: Rename table_to_map to srcdata_to_map
	//TODO: Be sure to catch errors when mapping (like things aren't there or something)
	if ( !( map = zrender_userdata_to_map( rz, src, srclen ) ) ) {
		return NULL;
	}

	//TODO: Same to catch errors here...
	if ( !( block = zrender_map_to_uint8t( rz, map, &blocklen ) ) ) {
		return NULL;
	}

#if 0
	//Convert T to a map
	if ( !( map = table_to_map( t, src, srclen ) ) ) {
		return NULL;
	}

#if 1
	//See the map 
	zrender_print_table( map );

	//Do the map	
	if ( !( block = map_to_uint8t( t, map, &blocklen ) ) ) {
		return NULL;
	}
#endif

	//Free the map
	//destroy_render_table( map );
	*newlen = blocklen;
#endif

	return block; 
}
#endif


//Destroy the map...
void zrender_free_table( struct map **map ) {
	struct map **top = map;

	while ( *map ) {
		struct map *item = *map;

		//Dump the unchanging elements out...
		FPRINTF( "[%3d] => action: %-16s", di++, DUMPACTION( item->action ) );

		if ( item->action == RAW ) { 
			FPRINTF( "Nothing to free...\n" );
		}
		else if ( item->action == EXECUTE ) {
			FPRINTF( "Freeing pointer to exec content..." );
			free( item->ptr );
		}
		else {
			FPRINTF( "Freeing int lists..." );
			int **ii = item->hashList;
			while ( ii && *ii ) {
				fprintf( stderr, "item->intlist: %p\n", *ii );	
				free( *ii ); 
				ii++;
			}
			free( item->hashList );
			FPRINTF( "\n" );
		}
		free( item );
		map++;
	}

	free( top );
}


#ifdef DEBUG_H
//Purely for debugging, see what came out
void zrender_print_table( struct map **map ) {
	while ( *map ) {
		struct map *item = *map;

		//Dump the unchanging elements out...
		FPRINTF( "[%3d] => action: %-16s", di++, DUMPACTION( item->action ) );

		if ( item->action == RAW || item->action == EXECUTE ) {
			uint8_t *p = (uint8_t *)item->ptr;
			fprintf( stderr, " len: %3d, ", item->len ); 
			write( 2, p, item->len );
			fprintf( stderr, "\n" );
		}
		else {
		#ifdef DEBUG_H
			fprintf( stderr, " item: " );
			//( item->word ) ? write( 2, item->word, item->wordlen ) : 0;
			fprintf( stderr, "," );
		#endif
			fprintf( stderr, " len: %3d, list: %p => ", item->len, item->hashList );
			int **ii = item->hashList;
			if ( !ii ) {
				fprintf( stderr , "NULL" );
			}
			else {
				int d=0;
				while ( *ii ) {
					fprintf( stderr, "%c%d", d++ ? ',' : ' ',  **ii );
					ii++;
				}
			}
			fprintf( stderr, "\n" );
		}
		map++;
	}
}
#endif
