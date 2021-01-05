/*Max key searches*/
#define LKV_MAX_SEARCH 10

/*Defines for each key type in a zTable*/
#define LKV_TERM -7

#define LKV_LAST \
	.hash = { LKV_TERM }

#define TEXT_KEY( str ) \
	.key = { LITE_TXT, .v.vchar = str } 

#define TEXT_VALUE( str ) \
	.value = { LITE_TXT, .v.vchar = str }

#define BLOB_KEY( b ) \
	.key = { LITE_BLB, .v.vblob = { sizeof( b ), (unsigned char *)b }}

#define BLOB_VALUE( b ) \
	.value = { LITE_BLB, .v.vblob = { sizeof( b ), (unsigned char *)b }}

#define INT_KEY( b ) \
	.key = { LITE_INT, .v.vint = b } 

#define INT_VALUE( b ) \
	.value = { LITE_INT, .v.vint = b } 

#define FLOAT_VALUE( b ) \
	.value = { LITE_FLT, .v.vfloat = b } 

#define USR_VALUE( b ) \
	.value = { LITE_USR, .v.vusrdata = b } 

#define TABLE_VALUE( b ) \
	.value = { LITE_TBL }

#define NULL_VALUE( ) \
	.value = { LITE_NUL }

#define TRM_VALUE( ) \
	.value = { LITE_TRM }

#define TRM( ) \
	.key = { LITE_TRM }

#define START_TABLEs( str ) \
	.key = { LITE_TXT, .v.vchar = str }, .value = { LITE_TBL }

#define START_TABLEi( num ) \
	.key = { LITE_INT, .v.vint = num }, .value = { LITE_TBL }

#define END_TABLE() \
	.key = { LITE_TRM }



zKeyval SinglezTable[] = {
	{ TEXT_KEY( "ashor" )  , TABLE_VALUE( )         },
		{ INT_KEY( 7043 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog." )  },
		{ INT_KEY( 7002 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog another time." )  },
		{ INT_KEY( 7003 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog for the 3rd time." )  },
		{ INT_KEY( 7004 )    , USR_VALUE ( NULL ) },
		{ INT_KEY( 7008 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog again." )  },
		{ INT_KEY( 7009 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog for the last time." )  },
		{ TRM() },

	{ TEXT_KEY( "artillery" )       , TABLE_VALUE( )         },
		/*Database records look a lot like this*/
		{ INT_KEY( 0 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "val" ), TEXT_VALUE( "MySQL makes me a bother." ) },
			{ TEXT_KEY( "rec" ), TEXT_VALUE( "Glad bacon" ) },
			{ TRM() },
		{ INT_KEY( 1 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "rec" ), TEXT_VALUE( "Choo choo cachoo" ) },
			{ TEXT_KEY( "val" ), TEXT_VALUE( "MySQL makes me ecstatic." ) },
			{ TRM() },
		{ INT_KEY( 2 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "val" ), TEXT_VALUE( "MySQL makes me giddy." ) },
			{ TEXT_KEY( "rec" ), TEXT_VALUE( "Happy, happy, happy is the man!" ) },
			{ TRM() },
		{ INT_KEY( 3 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "val" ), TEXT_VALUE( "MySQL makes me tired." ) },
			{ TEXT_KEY( "rec" ), TEXT_VALUE( "Baby says 'Feed Me'" ) },
			{ TRM() },
		{ TRM() },

	{ LKV_LAST } 
};


int get_count ( zKeyval *kv ) {
	int a = 0;
	while ( *kv->hash != LKV_TERM ) 
		a ++, kv ++;
	return a;	
}


//convert table without actually having to load them
zTable *convert_lkv ( zKeyval *kv ) {
	zTable *t = malloc( sizeof( zTable ) );
	lt_init( t, NULL, get_count( kv ) );

	while ( *kv->hash != LKV_TERM ) {
		if ( kv->key.type == LITE_TXT )
			lt_addtextkey( t, kv->key.v.vchar );
		else if ( kv->key.type == LITE_BLB )
			lt_addblobkey( t, kv->key.v.vblob.blob, kv->key.v.vblob.size );
		else if ( kv->key.type == LITE_INT )
			lt_addintkey( t, kv->key.v.vint );
		else if ( kv->key.type == LITE_TRM ) {
			lt_ascend( t );
			kv ++;
			continue;
		}
		else {
			//Abort immediately b/c this is an error
			fprintf( stderr, "%s: %d - Attempted to add wrong key type!  Bailing!", __FILE__, __LINE__ );
			exit( 0 );	
		}

		if ( kv->value.type == LITE_TXT )
			lt_addtextvalue( t, kv->value.v.vchar );
		else if ( kv->value.type == LITE_BLB )
			lt_addblobvalue( t, kv->value.v.vblob.blob, kv->value.v.vblob.size );
		else if ( kv->value.type == LITE_INT )
			lt_addintvalue( t, kv->value.v.vint );
		else if ( kv->value.type == LITE_FLT )
			lt_addfloatvalue( t, kv->value.v.vfloat );
		else if ( kv->value.type == LITE_USR )
			lt_addudvalue( t, kv->value.v.vusrdata );
		else if ( kv->value.type == LITE_NUL )
			;//lt_ascend has already been called, thus we should never reach this
		else { 
			if ( kv->value.type != LITE_TBL ) {
				//Abort immediately b/c this is an error
				fprintf( stderr, "%s: %d -  Got unknown or invalid key type!  Bailing!", __FILE__, __LINE__ );
				return NULL;
			}
		}

		if ( kv->value.type != LITE_TBL )
			lt_finalize( t );
		else {
			lt_descend( t );
			kv ++;
			continue;
		}
		kv ++;
	}
	lt_lock( t );
	return t;
}
