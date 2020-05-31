#include "zrender.h"

#define RPRINTF(a,b,blen) \
	memset( rprintchar, 0, sizeof(rprintchar) ); \
	memcpy( rprintchar, b, (blen >= sizeof(rprintchar)) ? sizeof(rprintchar) - 1 : blen ); \
	FPRINTF( "%s > %s(...): '%s'\n", a, __func__, replace_chars( rprintchar, strlen(rprintchar) ) );

static char rprintchar[127] = {0};
	
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

static char * replace_chars ( char *src, int srclen ) {
	char *srcsrc = src;
	while ( srclen ) {
		( *srcsrc == '\n' || *srcsrc == '\r' ) ? *srcsrc = '|' : 0; 
		srclen--, srcsrc++;
	}
	return src;
}

int INSIDE = 0;

struct map * init_map () {
	struct map *rp = malloc( sizeof( struct map ) );
	if ( !rp ) {
		//Free and destroy things
		return NULL;
	}

	rp->action = 0; 
	rp->ptr = NULL; 
	rp->len = 0; 
	rp->hashList = NULL; 
	return rp;
}


//...
struct parent * init_parent() {
	struct parent *np = NULL; 
	if (( np = malloc( sizeof(struct parent) )) == NULL ) {
		return NULL;
	}

	//NOTE: len will contain the number of elements to loop
	np->childCount = 0;
	np->pos = 0;
	np->len = 0;
	np->text = 0;
	return np; 
}



#ifdef DEBUG_H
static int di = 0;
#endif

#if 0
static const int maps[] = {
	['#'] = { LOOP_START, map_start, proc_start } //easier if you just extract at beginning
	['/'] = LOOP_END,
	['.'] = COMPLEX_EXTRACT,
	['$'] = EACH_KEY, 
	['`'] = EXECUTE, //PAIR_EXTRACT
	['!'] = BOOLEAN,
	[254] = RAW,
	[255] = 0
};
#endif

int * copy_int ( int i ) {
	int *h = malloc( sizeof ( int ) );
	memcpy( h, &i, sizeof( int ) );
	return h; 
}

#if 1

void map_raw_extract( struct map *row, struct map **parent, uint8_t *ptr, int len, void *t ) {
	RPRINTF( "RAW", ptr, len ); 
	row->ptr = ptr;
	row->len = len;
	row->action = RAW;
}

void map_simple_extract( struct map *row, struct map **parent, uint8_t *ptr, int len, void *t ) {
	zTable *tt = (zTable *)t;
	int hlen=0, hash = lt_get_long_i( t, ptr, len ); 
	if ( hash > -1 ) {
		add_item( &row->hashList, copy_int( hash ), int *, &hlen );
	}
	row->action = SIMPLE_EXTRACT; 
}


void map_loop_start( struct map *row, struct map **parent, uint8_t *ptr, int len, void *t ) {
	int hash = -1;
	int hlen = 0;
	int blen = 0;
	int eCount = 0;
	uint8_t bbuf[ 2048 ] = { 0 };
	zTable *tt = (zTable *)t;
	RPRINTF( "LOOP_START", ptr, len );

	//If a parent should exist, copy the parent's text 
	if ( !parent ) {
		//Copy the data
		memcpy( bbuf, ptr, len );
		blen += len;

		//Get the hash
		if ( ( hash = lt_get_long_i( tt, bbuf, blen ) ) > -1 ) {
			add_item( &row->hashList, copy_int( hash ), int *, &hlen );
			row->len = lt_counti( tt, hash );
		}
	}
	else {
		//Get a count of the number of elements in the parent.
		int maxCount = 0;
#if 1
		for ( int i=0, cCount=0; i < (*parent)->children; i++ ) {
	#if 0
			//All of this is stupid, it should just be:
			snprintf( this, thislen, "%s.%d.%s", (*parent)->text, num, p );
	#else
			char num[ 64 ] = { 0 };
			int numlen = snprintf( num, sizeof( num ) - 1, ".%d.", i );	
			memcpy( bbuf, (*parent)->ptr, (*parent)->len );
			blen = (*parent)->len;
			memcpy( &bbuf[ blen ], num, numlen );
			blen += numlen;
			memcpy( &bbuf[ blen ], ptr, len );
			blen += len;
			hash = lt_get_long_i( tt, bbuf, blen );
			add_item( &row->hashList, copy_int( hash ), int *, &hlen ); 
		
			if ( hash > -1 && (cCount = lt_counti( tt, hash )) > maxCount ) {
				maxCount = cCount;	
			}
	#endif
		}
	#if 0
		cp = pp[ pplen - 1 ];

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
			add_item( &row->hashList, copy_int( hash ), int *, &hashListLen ); 
		
			if ( hash > -1 && (cCount = lt_counti( t, hash )) > maxCount ) {
				maxCount = cCount;	
			}
		}
	#endif
		row->len = eCount = maxCount;
	}

#if 0
	row->len = eCount;

	//Find the hash
	if ( hashListLen ) {
		struct parent *np = init_parent(); 
		//NOTE: len will contain the number of elements to loop
		np->childCount = eCount;
		np->len = alen;
		np->text = p; 
		add_item( &pp, np, struct parent *, &pplen );
		INSIDE++;
	}
#endif
#endif
}

void map_each_key() {
}

void map_execute() {
}

void map_boolean() {
}

void map_complex_extract( struct map *row, struct map **parent, uint8_t *ptr, int len, void *t ) {
	RPRINTF( "COMPLEX", ptr, len );
#if 0
	if ( pplen ) {
		struct parent **w = pp;
		int c = 0;
		while ( (*w)->pos < (*w)->childCount ) {
			//Move to the next block or build a sequence
			if ( c < (pplen - 1) ) {
				w++, c++;
				continue;
			}
			
			//Generate the hash strings
			if ( 1 ) {
				struct parent **xx = pp;
				uint8_t tr[ 2048 ] = { 0 };
				int trlen = 0;
				
				for ( int ii=0; ii < pplen; ii++ ) {
					memcpy( &tr[ trlen ], (*xx)->text, (*xx)->len );
					trlen += (*xx)->len;
					trlen += sprintf( (char *)&tr[ trlen ], ".%d.", (*xx)->pos );
					xx++;
				}
				memcpy( &tr[ trlen ], p, alen );
				trlen += alen;

				//TODO: Replace me with copy_int or a general copy_ macro
				//Check for this hash, save each and dump the list...
				int hh = lt_get_long_i( t, tr, trlen );
#if 1
				add_item( &rp->hashList, copy_int( hh ), int *, &hashListLen ); 
#else									
				int *h = malloc( sizeof(int) );
				memcpy( h, &hh, sizeof(int) );	
				add_item( &rp->hashList, h, int *, &hashListLen ); 
#endif
				FPRINTF( "string = %s, hash = %3d, ", tr, hh );	
			}

			//Increment the number 
			while ( 1 ) {
				(*w)->pos++;
				//printf( "L%d %d == %d, STOP", c, (*w)->a, (*w)->b );
				if ( c == 0 )
					break;
				else { // ( c > 0 )
					if ( (*w)->pos < (*w)->childCount ) 
						break;
					else {
						(*w)->pos = 0;
						w--, c--;
					}
				}
			}
		}
		(*w)->pos = 0;
	}
#endif
}

void map_loop_end( struct map *row, struct map **parent, uint8_t *ptr, int len, void *t ) {
	RPRINTF( "LOOP_END", ptr, len );
	//rp->hash = lt_get_long_i( t, p, alen );
#if 0
	if ( !INSIDE )
		;
	else if ( pplen == INSIDE ) {
		free( pp[ pplen - 1 ] );
		pplen--;
		INSIDE--;
	}
#endif
	if ( parent ) {
		free( *parent );
		parent--;
	}
}

void extract_raw( struct map *row, uint8_t *ptr, int len ) {
	row->ptr = ptr;
	row->len = len;
	row->action = RAW;
}

void extract_loop_start() {
}
void extract_loop_end() {
}
void extract_complex_extract() {
}
void extract_simple_extract() {
}
void extract_each_key() {
}
void extract_execute() {
}
void extract_boolean() {
}
#endif



//...
//Check that the data is balanced.
int zrender_check_balance ( const uint8_t *src, int srclen ) {

	//This is the syntax to check for...
	const char *start = "{{", *end = "}}";
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
struct map ** zrender_table_to_map ( void *t, const uint8_t *src, int srclen ) {
	int ACTION = 0;
	int BLOCK = 0;
	//int SKIP = 0;
	struct map **rr = NULL ; 
	struct parent **pp = NULL;
	struct map **pr = NULL;
	int rrlen = 0;
	int pplen = 0;
	zWalker r;
	memset( &r, 0, sizeof( zWalker ) );

	//Allocating a list of characters to elements is easiest.
	while ( memwalk( &r, (uint8_t *)src, (uint8_t *)"{}", srclen, 2 ) ) {
		//More than likely, I'll always use a multi-byte delimiter
		if ( r.size == 0 ) {
			//Check if there is a start or end block
			if ( r.chr == '{' ) {
				BLOCK = BLOCK_START;
			}
		}
		else if ( r.chr != '}' ) {
			//We can simply copy if ACTION & BLOCK are 0 
			if ( !ACTION && !BLOCK ) {
				struct map *rp = init_map();
				map_raw_extract( rp, NULL, (uint8_t *)&src[ r.pos ], r.size, t );
				FPRINTF( "ACTION == %s\n", DUMPACTION( rp->action ) );
				add_item( &rr, rp, struct map *, &rrlen );
			}
		}
		else if ( src[ r.pos + r.size + 1 ] == '}' ) {
			//Start extraction...
			BLOCK = BLOCK_END;
			int alen=0, nlen = 0;	
			struct map *rp = init_map();
			uint8_t *p = lt_trim( (uint8_t *)&src[r.pos], " ", r.size, &nlen );
			int action = maps[ *p ];
			FPRINTF( "CHAR: '%c' %d %d\n", *p, maps[ *p ], action );
#if 0
			//Extract the first character
			if ( maps[ *p ] == 0 ) {
				map_simple_extract( rp, NULL, p, nlen, t );
			}
			else {
			}
#endif
				//Advance and reset p b/c we need just the text...
			rp->action = maps[ *p ];
			p = lt_trim( p, ". #/$`!\t", nlen, &alen );
			FPRINTF( "ACTION == %s\n", DUMPACTION( rp->action ) );

#if 0
			//Figure some things out...
			if ( rp->action == LOOP_START ) {
				map_loop_start( rp, pr, p, alen, t );
			}
			else if ( rp->action == LOOP_END ) {
				map_loop_end( rp, pr, p, alen, t );
			}
			else if ( rp->action == COMPLEX_EXTRACT ) {
				map_complex_extract( rp, pr, p, alen, t );
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
	}

	//Destroy the parent list
	free( pp );
	return rr;
}

//Bitmasking will tell me a lot...
struct map **table_to_map ( zTable *t, const uint8_t *src, int srclen ) {
	int ACTION = 0;
	int BLOCK = 0;
	//int SKIP = 0;
	int INSIDE = 0;
	struct map **rr = NULL ; 
	struct parent **pp = NULL;
	int rrlen = 0;
	int pplen = 0;
	zWalker r;
	memset( &r, 0, sizeof( zWalker ) );

	//Allocating a list of characters to elements is easiest.
	while ( memwalk( &r, (uint8_t *)src, (uint8_t *)"{}", srclen, 2 ) ) {
		//More than likely, I'll always use a multi-byte delimiter
		if ( r.size == 0 ) {
			//Check if there is a start or end block
			if ( r.chr == '{' ) {
				BLOCK = BLOCK_START;
			}
		}
		else if ( r.chr == '}' ) {
			if ( src[ r.pos + r.size + 1 ] == '}' ) {
				//Start extraction...
				BLOCK = BLOCK_END;
				int nlen = 0;	
				int hashListLen = 0;
				uint8_t *p = lt_trim( (uint8_t *)&src[r.pos], " ", r.size, &nlen );
				struct map *rp = init_map();

				//Extract the first character
				if ( !maps[ *p ] ) {
					rp->action = SIMPLE_EXTRACT; 
					int hash = lt_get_long_i( t, p, nlen ); 
					if ( hash > -1 ) {
						add_item( &rp->hashList, copy_int( hash ), int *, &hashListLen );
					}
				}
				else {
					//Advance and reset p b/c we need just the text...
					int alen = 0;
					rp->action = maps[ *p ];
					p = lt_trim( p, ". #/$`!\t", nlen, &alen );
					FPRINTF("GOT ACTION %s, and TEXT = ", DUMPACTION(rp->action)); ENCLOSE( p, 0, alen );

					//Figure some things out...
					if ( rp->action == LOOP_START ) {
						FPRINTF( "@LOOP_START - " );	
						int hash = -1;
						int blen = 0;
						int eCount = 0;
						uint8_t bbuf[ 2048 ] = { 0 };
						struct parent *cp = NULL;

						//If a parent should exist, copy the parent's text 
						if ( !INSIDE ) {
							//Copy the data
							memcpy( &bbuf[ blen ], p, alen );
							blen += alen;
							if ( ( hash = lt_get_long_i(t, bbuf, blen) ) > -1 ) {
								add_item( &rp->hashList, copy_int( hash ), int *, &hashListLen );
								eCount = lt_counti( t, hash );
							}
						}
						else {
							//Get a count of the number of elements in the parent.
							int maxCount = 0;
							cp = pp[ pplen - 1 ];

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
								add_item( &rp->hashList, copy_int( hash ), int *, &hashListLen ); 
							
								if ( hash > -1 && (cCount = lt_counti( t, hash )) > maxCount ) {
									maxCount = cCount;	
								}
							}
							eCount = maxCount;
						}

						rp->len = eCount;

						//Find the hash
						if ( hashListLen ) {
							struct parent *np = init_parent(); 
							//NOTE: len will contain the number of elements to loop
							np->childCount = eCount;
							np->len = alen;
							np->text = p; 
							add_item( &pp, np, struct parent *, &pplen );
							INSIDE++;
						}
					}
					else if ( rp->action == LOOP_END ) {
						//If inside is > 1, check for a period, strip it backwards...
						FPRINTF( "@LOOP_END - " );
						//TODO: Check that the hashes match instead of just pplen
						//rp->hash = lt_get_long_i( t, p, alen );
						if ( !INSIDE )
							;
						else if ( pplen == INSIDE ) {
							free( pp[ pplen - 1 ] );
							pplen--;
							INSIDE--;
						}
					}
					else if ( rp->action == COMPLEX_EXTRACT ) {
						FPRINTF( "@COMPLEX_EXTRACT - " );
						if ( pplen ) {
							struct parent **w = pp;
							int c = 0;
							while ( (*w)->pos < (*w)->childCount ) {
								//Move to the next block or build a sequence
								if ( c < (pplen - 1) ) {
									w++, c++;
									continue;
								}
								
								//Generate the hash strings
								if ( 1 ) {
									struct parent **xx = pp;
									uint8_t tr[ 2048 ] = { 0 };
									int trlen = 0;
									
									for ( int ii=0; ii < pplen; ii++ ) {
										memcpy( &tr[ trlen ], (*xx)->text, (*xx)->len );
										trlen += (*xx)->len;
										trlen += sprintf( (char *)&tr[ trlen ], ".%d.", (*xx)->pos );
										xx++;
									}
									memcpy( &tr[ trlen ], p, alen );
									trlen += alen;

									//TODO: Replace me with copy_int or a general copy_ macro
									//Check for this hash, save each and dump the list...
									int hh = lt_get_long_i( t, tr, trlen );
#if 1
									add_item( &rp->hashList, copy_int( hh ), int *, &hashListLen ); 
#else									
									int *h = malloc( sizeof(int) );
									memcpy( h, &hh, sizeof(int) );	
									add_item( &rp->hashList, h, int *, &hashListLen ); 
#endif
									FPRINTF( "string = %s, hash = %3d, ", tr, hh );	
								}

								//Increment the number 
								while ( 1 ) {
									(*w)->pos++;
									//printf( "L%d %d == %d, STOP", c, (*w)->a, (*w)->b );
									if ( c == 0 )
										break;
									else { // ( c > 0 )
										if ( (*w)->pos < (*w)->childCount ) 
											break;
										else {
											(*w)->pos = 0;
											w--, c--;
										}
									}
								}
							}
							(*w)->pos = 0;
						}
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
				}

				add_item( &rr, rp, struct map *, &rrlen );
			}
		}
		else {
			FPRINTF( "@RAW BLOCK COPY" ); 
			//We can simply copy if ACTION & BLOCK are 0 
			if ( !ACTION && !BLOCK ) {
#if 1
				struct map *rp = init_map();
#else
				struct map *rp = malloc( sizeof( struct map ) );
				if ( !rp ) {
					//Teardown and destroy
					return NULL;
				}
#endif	
				//Set defaults
				rp->len = r.size;	
				rp->action = RAW;
				rp->ptr = (uint8_t *)&src[ r.pos ];	
				
				//Save a new record
				add_item( &rr, rp, struct map *, &rrlen );
			}	
		}
	}

	//Destroy the parent list
	free( pp );
	return rr;
}


//Didn't I write something to add to a buffer?
void extract_table_value ( zKeyval *lt, uint8_t **ptr, int *len, uint8_t *t, int tl ) {
	if ( lt->value.type == LITE_TXT ) {
		*len = strlen( lt->value.v.vchar ); 
		*ptr = (uint8_t *)lt->value.v.vchar;
	}
	else if ( lt->value.type == LITE_BLB ) {
		*len = lt->value.v.vblob.size; 
		*ptr = lt->value.v.vblob.blob;
	}
	else if ( lt->value.type == LITE_INT ) {
		*len = snprintf( (char *)t, tl - 1, "%d", lt->value.v.vint );
		*ptr = (uint8_t *)t;
	}
	else {
		//If for some reason we can't convert, just use a blank value.
		*len = 0;
		*ptr = (uint8_t *)"";
	}
}


uint8_t *map_to_uint8t ( zTable *t, struct map **map, int *newlen ) {
	//Start the writes, by using the structure as is
	uint8_t *block = NULL;
	int blockLen = 0;
	struct dep depths[100] = { { 0, 0, 0 } };
	struct dep *d = depths;

	while ( *map ) {
		struct map *item = *map;
		int hash = -1;

		if ( item->action == RAW || item->action == EXECUTE ) {
			FPRINTF( "%-20s, len: %3d", "RAW", item->len );
			append_to_uint8t( &block, &blockLen, item->ptr, item->len );
		}
		else if ( item->action == SIMPLE_EXTRACT ) {
			FPRINTF( "%-20s, len: %3d ", "SIMPLE_EXTRACT", item->len );
			//rip me out
			if ( item->hashList ) {
				//Get the type and length
				if ( ( hash = **item->hashList ) > -1 ) {
					zKeyval *lt = lt_retkv( t, hash );
					//NOTE: At this step, nobody should care about types that much...
					uint8_t *ptr = NULL, nbuf[64] = {0};
					int itemlen = 0;
					extract_table_value( lt, &ptr, &itemlen, nbuf, sizeof(nbuf) );
					append_to_uint8t( &block, &blockLen, (uint8_t *)ptr, itemlen );
				}
				item->hashList++;
			}
			FPRINTF( "\n" );
		}
		else {
			if ( item->action == LOOP_START ) {
				FPRINTF( "%-20s, len: %3d", "LOOP_START", item->len );
				d++;
				d->index = map;
				d->current = 0;
				d->childCount = item->len;
			}
			else if ( item->action == LOOP_END ) {
				FPRINTF( "%-20s, %d =? %d", "LOOP_END", d->current, d->childCount );
				if ( ++d->current == d->childCount )
					d--;
				else {
					map = d->index;
				}
			}
			else if ( item->action == COMPLEX_EXTRACT ) {
				FPRINTF( "%-20s", "COMPLEX_EXTRACT " );
				//rip me out, i am idential to SIMPLE_EXTRACT'S ROUTINE
				if ( item->hashList ) {
					//If there is a pointer, it does not move until I get through all three
					FPRINTF( "List?: %d", **item->hashList );
					int **list = item->hashList;
					//Get the type and length
					if ( ( hash = **list ) > -1 ) {
						zKeyval *lt = lt_retkv( t, hash );
						//NOTE: At this step, nobody should care about types that much...
						uint8_t *ptr = NULL, nbuf[ 64 ] = { 0 };
						int itemlen = 0;
						extract_table_value( lt, &ptr, &itemlen, nbuf, sizeof(nbuf) ); 
						append_to_uint8t( &block, &blockLen, ptr, itemlen );
					}
					list++; //item->hashList++;
				}
			}
		}
		fprintf( stderr, "%d, \n", blockLen );
		map++;
	}

	//The final step is to assemble everything...
	*newlen = blockLen;
	return block;
}



uint8_t *table_to_uint8t ( zTable *t, const uint8_t *src, int srclen, int *newlen ) {

	//Define things
	struct map **map = NULL;
	uint8_t *block = NULL;
	int blocklen = 0;

	//Check against something
	if ( !zrender_check_balance( src, srclen ) ) {
		FPRINTF( "Syntax at supplied template is wrong..." );
		return NULL;
	}

	//Convert T to a map
	if ( !( map = zrender_table_to_map( t, src, srclen ) ) ) {
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
			FPRINTF( " len: %3d, ", item->len ); 
			write( 2, p, item->len );
		}
		else {
		#ifdef DEBUG_H
			fprintf( stderr, " item: " );
			//( item->word ) ? write( 2, item->word, item->wordlen ) : 0;
			fprintf( stderr, "," );
		#endif
			FPRINTF( " len: %3d, list: %p => ", item->len, item->hashList );
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
