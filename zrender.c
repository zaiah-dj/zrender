#include "zrender.h"

#ifdef DEBUG_H
static int di = 0;
static char rprintchar[127] = {0};
static char * replace_chars ( char *src, int srclen ) {
	char *srcsrc = src;
	while ( srclen ) {
		( *srcsrc == '\n' || *srcsrc == '\r' ) ? *srcsrc = '|' : 0; 
		srclen--, srcsrc++;
	}
	return src;
}
#endif

static unsigned char *append_to_uint8t ( uint8_t **dest, int *len, uint8_t *src, int srclen ) {
	if ( !( *dest = realloc( *dest, (*len) + srclen ) ) ) {	
		return NULL;
	}

	if ( !memcpy( &(*dest)[ *len ], src, srclen ) ) {
		return NULL;
	}

	(*len) += srclen;
	return *dest;
}


//Add to series
static void * add_item_to_list( void ***list, void *element, int size, int * len ) {
#if 0
	fprintf( stderr, "list => %p, %d, %d\n", *list, (*len), (*len) + 2 );
#endif

	//Reallocate
	if (( (*list) = realloc( (*list), size * ( (*len) + 2 ) )) == NULL ) {
		ZRENDER_PRINTF( "Failed to reallocate block from %d to %d\n", size, size * ((*len) + 2) ); 
		return NULL;
	}

#if 0
	fprintf( stderr, 
		"Successfully reallocated block to size %d\n", size * ((*len) + 2) ); 
	fprintf( stderr, "list => %p, %d, %d, %d\n", *list, (*len), (*len) + 1, size * ((*len) + 2 ) );
#endif

	(*list)[ *len ] = element; 
	(*list)[ (*len) + 1 ] = NULL; 
	(*len) += 1; 
	return list;
}


//Didn't I write something to add to a buffer?
void extract_table_value ( zKeyval *lt, unsigned char **ptr, int *len, unsigned char *t, int tl ) {
	if ( lt->value.type == LITE_TXT ) {
		*len = strlen( lt->value.v.vchar ); 
		*ptr = (unsigned char *)lt->value.v.vchar;
	}

	else if ( lt->value.type == LITE_BLB ) {
		*len = lt->value.v.vblob.size; 
		*ptr = lt->value.v.vblob.blob;
	}
	else if ( lt->value.type == LITE_INT ) {
		*len = snprintf( (char *)t, tl - 1, "%d", lt->value.v.vint );
		*ptr = (unsigned char *)t;
	}
	else {
		//If for some reason we can't convert, just use a blank value.
		*len = 0;
		*ptr = (unsigned char *)"";
	}
}


MAPPER(map_raw_extract) {
	RPRINTF( "\nRAW", ptr, len ); 
	row->ptr = ptr;
	row->len = len;
	row->action = RAW;
}


MAPPER(map_simple_extract) {
	zTable *tt = (zTable *)t;
	int hlen=0, hash = lt_get_long_i( tt, ptr, len ); 
	if ( hash > -1 ) {
		zrender_add_item( &row->hashList, zrender_copy_int( hash ), int *, &hlen );
	}
	row->action = SIMPLE_EXTRACT; 
}


//...
MAPPER(map_loop_start) { 
	int hash = -1;
	int hlen = 0;
	int blen = 0;
	int element_count = 0;
	unsigned char bbuf[ 2048 ] = { 0 };
	zTable *tt = (zTable *)t;
	RPRINTF( "LOOP_START", ptr, len );

	//If a parent should exist, copy the parent's text 
	if ( !( *parent ) ) {
ZRENDER_PRINTF( "no parent\n" );
		//Copy the data
		memcpy( bbuf, ptr, blen = len );

		//Get the hash
		if ( ( hash = lt_get_long_i( tt, bbuf, blen ) ) > -1 ) {
			zrender_add_item( &row->hashList, zrender_copy_int( hash ), int *, &hlen );
			row->children = element_count = lt_counti( tt, hash );
		}
	}
	else {
ZRENDER_PRINTF( "there is a parent\n" );
		//Get a count of the number of elements in the parent.
		int maxCount = 0;
		struct map **cp = &( *parent )[ *plen - 1 ];
ZRENDER_PRINTF( "struct map: %p", cp );
#if 0

		//Save all of the root elements
		for ( int i=0, cCount=0; i < cp->children; i++ ) {
	#if 0
			//All of this is stupid, it should just be:
			snprintf( this, thislen, "%s.%d.%s", (*parent)->text, num, p );
	#else
			char num[ 64 ] = { 0 };
			int numlen = snprintf( num, sizeof( num ) - 1, ".%d.", i );	
			memcpy( bbuf, cp->ptr, cp->len );
			blen = cp->len;
			memcpy( &bbuf[ blen ], num, numlen );
			blen += numlen;
			memcpy( &bbuf[ blen ], ptr, len );
			blen += len;
			hash = lt_get_long_i( tt, bbuf, blen );
			zrender_add_item( &row->hashList, zrender_copy_int( hash ), int *, &hlen ); 
		
			if ( hash > -1 && (cCount = lt_counti( tt, hash )) > maxCount ) {
				maxCount = cCount;	
			}
	#endif
		}
	#if 0
		cp = pp[ pplen - 1 ];

		//
		for ( int i=0, cCount=0; i < cp->childCount; i++ ) {
			char num[ 64 ] = { 0 };
			int numlen = snprintf( num, sizeof( num ) - 1, ".%d.", i );	

			//Copy to static buffer
			memcpy( bbuf, cp->text, cp->len );
			blen = cp->len;
			memcpy( &bbuf[ blen ], num, numlen );
			blen += numlen;
			memcpy( &bbuf[ blen ], p, alen );
			blen += alen;
			hash = lt_get_long_i(t, bbuf, blen );
			zrender_add_item( &row->hashList, zrender_copy_int( hash ), int *, &hashListLen ); 
		
			if ( hash > -1 && (cCount = lt_counti( t, hash )) > maxCount ) {
				maxCount = cCount;	
			}
		}
	#endif
		row->len = eCount = maxCount;
#endif
	}

	//Find the hash
	if ( hlen ) {
		struct map *np = malloc( sizeof( struct map ) );
		if ( !np ) {
			//Free and destroy things
			return;
		}

		//NOTE: len will contain the number of elements to loop
		memset( np, 0, sizeof( struct map ) );
		np->action = 0; 
		np->hashList = NULL; 
		np->children = element_count;
		np->len = blen;
		np->ptr = ptr; 
		zrender_add_item( parent, np, struct parent *, plen );
	}
}


MAPPER(map_complex_extract) {
	RPRINTF( "\nCOMPLEX", ptr, len );
	ZRENDER_PRINTF( "plen: %d\n", *plen );
	if ( *plen ) {
		int c = 0;
		int hlen = 0;
		int pos = 0;
		struct map **w = &( *parent )[ *plen - 1 ]; ZRENDER_PRINTF( "ptr: %p, len: %d\n", w, *plen );
		ZRENDER_PRINTF( "pos: %d, children: %d\n", (*w)->pos, (*w)->children );
 
		while ( (*w)->pos < (*w)->children ) {
#if 1
			//Move to the next block or build a sequence
			if ( c < (*plen - 1) ) {
				w++, c++;
				continue;
			}
			
			//Generate the hash strings
			if ( 1 ) {
			struct map **xx = *parent;
			unsigned char tr[ 2048 ] = { 0 };
			int trlen = 0;
			
			for ( int ii=0; ii < *plen; ii++ ) {
				memcpy( &tr[ trlen ], (*xx)->ptr, (*xx)->len );
				trlen += (*xx)->len;
				trlen += sprintf( (char *)&tr[ trlen ], ".%d.", (*xx)->pos );
				xx++;
			}
			memcpy( &tr[ trlen ], ptr, len );
			trlen += len;

			//TODO: Replace me with zrender_copy_int or a general copy_ macro
			//Check for this hash, save each and dump the list...
			int hh = lt_get_long_i( t, tr, trlen );
			zrender_add_item( &row->hashList, zrender_copy_int( hh ), int *, &hlen ); 
			}

			//Increment the number 
			while ( 1 ) {
				(*w)->pos++;
				//printf( "L%d %d == %d, STOP", c, (*w)->a, (*w)->b );
				if ( c == 0 )
					break;
				else { // ( c > 0 )
					if ( (*w)->pos < (*w)->children ) 
						break;
					else {
						(*w)->pos = 0;
						w--, c--;
					}
				}
			}
#endif
		}
		(*w)->pos = 0;
	}
}


//....
MAPPER(map_loop_end) {
	RPRINTF( "LOOP_END", ptr, len );
	ZRENDER_PRINTF( "Parent: %p  length: %d\n", parent, *plen );
	if ( *parent ) {
		//free( &( *parent )[ *plen ] );
		parent--, (*plen)--;
		ZRENDER_PRINTF( "Parent: %p, length: %d\n", *parent, *plen );
	}
}


//....
EXTRACTOR(extract_raw) {
	ZRENDER_PRINTF( "%-20s, len: %3d\n", "xRAW", (**row)->len );
	append_to_uint8t( dst, dlen, src, len );
}


EXTRACTOR(extract_loop_start) {
	ZRENDER_PRINTF( "%-20s, %d\n", "xLOOP_START", (**row)->len );
	//struct dep *d = (struct dep * )t;
	(*ptr)++;
	(*ptr)->index = &(**row);
	(*ptr)->current = 0;
	(*ptr)->children = (**row)->children;
}


EXTRACTOR(extract_loop_end) {
	ZRENDER_PRINTF( "%-20s\n", "xLOOP_END" );
	(*ptr)->current++;
	ZRENDER_PRINTF( "%d ?= %d\n", (*ptr)->current, (*ptr)->children );
	ZRENDER_PRINTF( "%p ?= %p\n", (**row), (*ptr)->index );

	if ( (*ptr)->current == (*ptr)->children )
		(*ptr)--;
	else {
		*row = (*ptr)->index;
	}
	//ZRENDER_PRINTF( "%p ?= %p\n", (**row), (*ptr)->index );
}


EXTRACTOR(extract_simple_extract) {
	ZRENDER_PRINTF( "%-20s\n", "xSIMPLE_EXTRACT" );
	zTable *tt = (zTable *)t;
	if ( (**row)->hashList ) {
		int hash = **( (**row)->hashList ); 
		if ( hash > -1 ) { 
			zKeyval *lt = lt_retkv( t, hash );
			unsigned char *iptr = NULL, nbuf[ 64 ] = { 0 };
			int itemlen = 0;
			extract_table_value( lt, &iptr, &itemlen, nbuf, sizeof(nbuf) ); 
			append_to_uint8t( dst, dlen, iptr, itemlen );
		}
	}
}


EXTRACTOR(extract_complex_extract) {
	ZRENDER_PRINTF( "%-20s\n", "xCOMPLEX EXTRACT" );
	if ( (**row)->hashList ) {
		//If there is a pointer, it does not move until I get through all three
		ZRENDER_PRINTF( "Getting entry: %d\n", **( (**row)->hashList) );
		int **list = (**row)->hashList;
		int hash = 0;
		//Get the type and length
		if ( ( hash = **list ) > -1 ) {
			zKeyval *lt = lt_retkv( t, hash );
			//NOTE: At this step, nobody should care about types that much...
			unsigned char *iptr = NULL, nbuf[ 64 ] = { 0 };
			int itemlen = 0;
			extract_table_value( lt, &iptr, &itemlen, nbuf, sizeof(nbuf) ); 
			append_to_uint8t( dst, dlen, iptr, itemlen );
		}
		(**row)->hashList++;
	}
}


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

	return zr;
}


//Use the default templating language (mustache)
void zrender_set_default_dialect( zRender *rz ) {
	zrender_set_boundaries( rz, "{{", "}}" );
	zrender_set( rz, '#', map_loop_start, extract_loop_start ); 
	zrender_set( rz, '/', map_loop_end, extract_loop_end ); 
	zrender_set( rz, 0, map_raw_extract, extract_raw ); 
	zrender_set( rz, '.', map_complex_extract, extract_complex_extract ); 
	zrender_set( rz, 1, map_simple_extract, extract_simple_extract ); 
#if 0
	//Simple extracts are anything BUT the other characters (but raw is also 1, so...)
	zrender_set( rz, '!', map_boolean, extract_boolean ); 
	zrender_set( rz, '`', map_execute, extract_execute ); 
#endif
}


//...
struct map * init_map ( int action ) {
	struct map *rp = malloc( sizeof( struct map ) );
	if ( !rp ) {
		//Free and destroy things
		return NULL;
	}

	memset( rp, 0, sizeof( struct map ) );
	rp->action = action; 
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
unsigned char *zrender_trim ( const unsigned char *msg, const char *trim, int len, int *nlen ) {
	//Define stuff
	//unsigned char *m = msg;
	unsigned char *forwards = (unsigned char *)msg;
	unsigned char *backwards = (unsigned char *)&msg[ len - 1 ];
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
	( nlen ) ? *nlen = nl : 0;
	return forwards;
}


//Check that the data is balanced.
int zrender_check_balance ( zRender *rz, const unsigned char *src, int srclen ) {
	zWalker r;
	memset( &r, 0, sizeof( zWalker ) );
	unsigned char check[] = { rz->zStart[0], rz->zEnd[0], '\n' };
	struct pos { int a, b, l, lp; } set[128] = {0}; 
int ls=0, le=0;
	int sl = 0, el = 0, nl = 0, mark = 0;

	//just check that the list is balanced
	while ( memwalk( &r, src, check, srclen, 3 ) ) {
		if ( r.chr == '\n' ) 
			nl++, set[ sl ].lp = r.pos;
		else if ( r.chr == check[0] && *r.ptr == check[0] ) {
			mark = 1, set[ sl ].a = r.pos, set[ sl ].l = nl, sl++;
		}
		else if ( mark == 1 && r.chr == check[1] && *r.ptr == check[1] ) {
			//start list must be marked first, or don't go
			mark = 0, set[ sl ].b = r.pos, set[ sl ].l = nl, el++;
#if 0
			unsigned char * ts = zrender_trim( r.src, " ", r.size - 1, NULL ); 
			( *ts == '#' ) ? ls ++ : ( *ts == '/' ) ? le ++ : 0;
#endif
		}
	}

	if ( sl != el ) {
		//rz->error = ZRENDER_SYNTAX_TERMINATOR;
		for ( int i = 0; i <= sl; i++ ) {
			if ( set[i].b == 0 ) {
				const char fmt[] = "No matching terminator found for sequence at line %d, pos %d\n";
				snprintf( rz->errmsg, 1024, fmt, set[i].l, set[i].a - set[i].lp ); 
				return 0;
			}	
		}
	}

#if 0
	if ( ls != le ) {
		//rz->error = ZRENDER_SYNTAX_LOOP;
		const char fmt[] = "Loop start and end are wrong\n";
		snprintf( rz->errmsg, 1024, fmt );
		return 0;	
	}
#endif

	return 1; 
}


//Convert userdata to an array map
struct map ** zrender_userdata_to_map ( zRender *rz, const unsigned char *src, int srclen ) {
	struct map **rr = NULL, **pr = NULL;
	int rrlen = 0, pplen = 0;
	zWalker r = { 0 };

	//The check map is now dynamically generated
	unsigned char check[] = { rz->zStart[0], rz->zEnd[0] };
	int checklen = 2;//sizeof(check);

	//Allocating a list of characters to elements is easiest.
	while ( memwalk( &r, (unsigned char *)src, check, srclen, checklen ) ) {
		struct zrSet *z = NULL; 
		if ( r.chr == '{' && *r.ptr == '{' ) {
			struct map *rp = init_map( 0 );
			if ( ( z = rz->mapset[ 0 ] ) ) {
				z->mapper( rp, NULL, NULL, r.src, r.size - 1, rz->userdata );
				zrender_add_item( &rr, rp, struct map *, &rrlen );
			}
		}
		else if ( r.chr == '}' && *r.ptr == '}' )	 {
			//Start extraction...
			int alen=0, nlen = 0;	
			unsigned char *p = zrender_trim( r.src, " ", r.size - 1, &nlen );
			struct map *rp = init_map( *p );

			//If no character handler exists, we fallback to 1
			if ( ( z = rz->mapset[ *p ] ) || ( z = rz->mapset[1] ) ) {
				//This should probably return some kind of error...
				//ZRENDER_PRINTF("RUNNING MAPPER on %c\n", rp->action ? ( ( rp->action == 1 ) ? 'S' : rp->action ) : 'R' );
				p = zrender_trim( p, ". #/$`!\t", nlen, &alen );
				z->mapper( rp, &pr, &pplen, p, alen, rz->userdata ); 
				zrender_add_item( &rr, rp, struct map *, &rrlen );
			}
		}
		else if ( *r.src != '{' && *r.src != '}' ) {
		#if 0	
			fprintf( stderr, "else match\n" );
			write( 2, r.src, srclen - r.pos );
			getchar();
		#endif
			struct map *rp = init_map( 0 );
			if ( ( z = rz->mapset[ 0 ] ) ) {
				z->mapper( rp, NULL, NULL, r.src, srclen - r.pos, rz->userdata );
				zrender_add_item( &rr, rp, struct map *, &rrlen );
			}
		}
	}


#if 0
	//copy at the end all sloppy like...
	struct zrSet *z = NULL; 
	struct map *rp = init_map( 0 );
	if ( ( z = rz->mapset[ 0 ] ) ) {
		z->mapper( rp, NULL, NULL, (unsigned char *)&src[ r.pos ], srclen - r.pos, rz->userdata );
		zrender_add_item( &rr, rp, struct map *, &rrlen );
	}
#endif

#if 0
	//Move through each of the rows 
	//Destroy the parent list
	free( pp );
#endif
	return rr;
}



//Merge the values referenced in the map array into an unsigned character block
unsigned char *zrender_map_to_uint8t ( zRender *rz, struct map **xmap, int *newlen ) {
	//...
	unsigned char *block = NULL;
	int blocklen = 0;
	struct ptr mptr[10] = { { 0, 0, 0 } };
	struct ptr *ptr = mptr;
	struct map **map = xmap;

	while ( map && *map ) {
		struct map *rp = *map;
		struct zrSet *z = NULL; 
		//If no character handler exists, we fallback to 1
		if ( ( z = rz->mapset[ (int)rp->action ] ) || ( z = rz->mapset[ 1 ] ) ) { 
			ZRENDER_PRINTF("RUNNING EXTRACTOR on %c\n", rp->action ? ( ( rp->action == 1 ) ? 'S' : rp->action ) : 'R' );
			z->extractor( &map, &block, &blocklen, rp->ptr, rp->len, &ptr, rz->userdata );
		}
		map++;
	}

	//The final step is to assemble everything...
	*newlen = blocklen;
	return block;
}


//Do all the steps to make templating quick and easy.
unsigned char *zrender_render( zRender *rz, const unsigned char *src, int srclen, int *newlen ) {

	//Define things
	struct map **map = NULL;
	unsigned char *buf = NULL;
	int buflen = 0;

	//TODO: Mark the place where the thing is undone
	if ( !zrender_check_balance( rz, src, srclen ) ) {
		ZRENDER_PRINTF( "Syntax at supplied template is wrong..." );
		return NULL;
	}

	//TODO: Rename table_to_map to srcdata_to_map
	//TODO: Be sure to catch errors when mapping (like things aren't there or something)
	if ( !( map = zrender_userdata_to_map( rz, src, srclen ) ) ) {
		return NULL;
	}

	//TODO: Same to catch errors here...
	if ( !( buf = zrender_map_to_uint8t( rz, map, &buflen ) ) ) {
		return NULL;
	}

	*newlen = buflen;
	return buf; 
}


//Destroy the zRender structure
void zrender_free_table( struct map **map ) {
	struct map **top = map;

	while ( *map ) {
		struct map *item = *map;

		//Dump the unchanging elements out...
		ZRENDER_PRINTF( "[%3d] => action: %-16s", di++, DUMPACTION( item->action ) );

		if ( item->action == RAW ) { 
			ZRENDER_PRINTF( "Nothing to free...\n" );
		}
		else if ( item->action == EXECUTE ) {
			ZRENDER_PRINTF( "Freeing pointer to exec content..." );
			free( item->ptr );
		}
		else {
			ZRENDER_PRINTF( "Freeing int lists..." );
			int **ii = item->hashList;
			while ( ii && *ii ) {
				fprintf( stderr, "item->intlist: %p\n", *ii );	
				free( *ii ); 
				ii++;
			}
			free( item->hashList );
			ZRENDER_PRINTF( "\n" );
		}
		free( item );
		map++;
	}

	free( top );
}


void zrender_free( zRender *z ) {

}


const char * zrender_strerror( zRender *z ) {
	return z->errmsg;
}


#ifdef DEBUG_H
//Purely for debugging, see what came out
void zrender_print_table( struct map **map ) {
	while ( *map ) {
		struct map *item = *map;

		//Dump the unchanging elements out...
		ZRENDER_PRINTF( "[%3d] => action: %-16s", di++, DUMPACTION( item->action ) );

		if ( item->action == RAW || item->action == EXECUTE ) {
			unsigned char *p = (unsigned char *)item->ptr;
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
