#include "zrender.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define SPACE_TEST "sister_cities"

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
#if 0
struct block { const char *model, *view; } files[] = {
	{ TESTDIR "empty.lua", TESTDIR "empty.tpl" },
	{ TESTDIR "simple.lua", TESTDIR "simple.tpl" },
	{ TESTDIR "castigan.lua", TESTDIR "castigan.tpl" },
	{ TESTDIR "multi.lua", TESTDIR "multi.tpl" },
	{ NULL }
};
#else
/*
All of these are defined seperately, becuase many will be re-used for the tests.
*/
zKeyval NozTable[] = {
	{ TEXT_KEY( "zxy" ), TEXT_VALUE( "def" ) },
	{ TEXT_KEY( "def" ), INT_VALUE( 342 ) },
	{ TEXT_KEY( "ghi" ), INT_VALUE( 245 ) },
	{  INT_KEY( 553   ), INT_VALUE( 455 ) },
	{ TEXT_KEY( "jkl" ), INT_VALUE( 981 ) },
	{  INT_KEY( 8234  ), INT_VALUE( 477 ) },
	{  INT_KEY( 11    ), INT_VALUE( 343 ) },
	{  INT_KEY( 32    ),  INT_VALUE( 3222 ) },
	{ TEXT_KEY( "abc" ), TEXT_VALUE( "Large angry deer are following me." ) },
	{ LKV_LAST } 
};


#if 0
//
zKeyval ST[] = {
	{ TEXT_KEY( "ashor" )  , TABLE_VALUE( )         },
		{ INT_KEY( 7043 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog." )  },
		{ INT_KEY( 7002 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog another time." )  },
		{ TRM() },
	{ LKV_LAST } 
};


zKeyval MT[] = {
	{ INT_KEY( 1 )    , TEXT_VALUE( "Wash the dog." )  },
	{ TEXT_KEY( "ashor" )  , TABLE_VALUE( )         },
		{ INT_KEY( 7043 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog." )  },
		{ INT_KEY( 7002 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog another time." )  },
		{ TRM() },
	{ TEXT_KEY( "artillery" )       , TABLE_VALUE( )         },
		/*Database records look a lot like this*/
		{ INT_KEY( 0 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "val" ), BLOB_VALUE( "MySQL makes me tired." ) },
			{ TEXT_KEY( "rec" ), TEXT_VALUE( "Choo choo cachoo." ) },
			{ TRM() },
		{ TRM() },
	{ TEXT_KEY( "michael" )       , TABLE_VALUE( )         },
		{ TEXT_KEY( "val" ), BLOB_VALUE( "MySQL makes me giddy." ) },
		{ TEXT_KEY( "rec" ), TEXT_VALUE( "Choo choo cachoo." ) },
		{ TRM() },
	{ TEXT_KEY( "jackson" )       , TABLE_VALUE( )         },
		{ TEXT_KEY( "val" ), BLOB_VALUE( "MySQL makes me ecstatic." ) },
		{ TEXT_KEY( "rec" ), TEXT_VALUE( "Choo choo cachoo." ) },
		{ TRM() },
	{ LKV_LAST } 
};
#endif


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



zKeyval DoublezTableAlpha[] = {
	{ TEXT_KEY( "cities" )       , TABLE_VALUE( )         },
		/*Database records look a lot like this*/
		{ INT_KEY( 0 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "San Francisco" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "CA" ) },
			{ TEXT_KEY( "desc" ), BLOB_VALUE( "There are so many things to see and do in this wonderful town.  Like talk to a billionaire startup founder or super-educated University of Berkeley professors." ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					//Pay attention to this, I'd like to embed unsigned char data here (I think Lua can handle this)
					{ TEXT_KEY( "claim_to_fame" ), TEXT_VALUE( "The Real Silicon Valley" ) },
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 870887 ) },
					{ TRM() },
				{ TRM() },
			{ TRM() },

		{ INT_KEY( 1 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "New York" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "NY" ) },
			{ TEXT_KEY( "desc" ), BLOB_VALUE( "New York City is one of the most well-known destinations on earth and home to over 8 million residents." ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					{ TEXT_KEY( "claim_to_fame" ), TEXT_VALUE( "The Greatest City on Earth" ) },
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 8750000 ) },
					{ TRM() },
				{ TRM() },
			{ TRM() },

		{ INT_KEY( 2 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "Raleigh" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "NC" ) },
			{ TEXT_KEY( "desc" ), BLOB_VALUE( "Otherwise known as the Oak City, around 600,000 residents call Raleigh home." ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					{ TEXT_KEY( "claim_to_fame" ), TEXT_VALUE( "Silicon Valley of the South" ) },
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 350001 ) },
					{ TRM() },
				{ TRM() },
			{ TRM() },
		{ TRM() },
	{ LKV_LAST } 
};



zKeyval DoublezTableNumeric[] = {
	{ TEXT_KEY( "cities" )       , TABLE_VALUE( )         },
		/*Database records look a lot like this*/
		{ INT_KEY( 0 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "San Francisco, CA" ) },
			{ TEXT_KEY( "desc" ), TEXT_VALUE( "It reeks of weed and opportunity. You know you want it..." ) },
			{ TEXT_KEY( "population" ), INT_VALUE( 332420 ) },
			{ TEXT_KEY( SPACE_TEST ), TABLE_VALUE( )         },
				{ INT_KEY( 0 ), TEXT_VALUE( "Sydney, Austrailia" ) },
				{ INT_KEY( 1 ), TEXT_VALUE( "Beijing, China" ) },
				{ INT_KEY( 2 ), TEXT_VALUE( "Perth, Australia" ) },
				{ INT_KEY( 3 ), TEXT_VALUE( "Johannesburg, South Africa" ) },
				{ TRM() },
			{ TRM() },

		{ INT_KEY( 1 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "Durham, NC" ) },
			{ TEXT_KEY( "population" ), INT_VALUE( 33242 ) },
			{ TEXT_KEY( SPACE_TEST ), TABLE_VALUE( )         },
				{ INT_KEY( 0 ), TEXT_VALUE( "Arusha, Tanzania" ) },
				{ INT_KEY( 1 ), TEXT_VALUE( "Durham, United Kingdom" ) },
				{ INT_KEY( 2 ), TEXT_VALUE( "Kostroma, Russia" ) },
				{ INT_KEY( 3 ), TEXT_VALUE( "Toyama, Japan" ) },
				{ INT_KEY( 4 ), TEXT_VALUE( "Zhuzhou, Hunan Province, China" ) },
				{ TRM() },
			{ TRM() },

		{ INT_KEY( 2 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "Tampa, FL" ) },
			{ TEXT_KEY( "desc" ), TEXT_VALUE( "It's home..." ) },
			{ TEXT_KEY( "population" ), INT_VALUE( 777000 ) },
			{ TEXT_KEY( SPACE_TEST ), TABLE_VALUE( )         },
				{ INT_KEY( 0 ), TEXT_VALUE( "Agrigento, Sicily" ) },
				{ INT_KEY( 1 ), TEXT_VALUE( "Ashdod South, Isreal" ) },
				{ INT_KEY( 2 ), TEXT_VALUE( "Barranquilla, Colombia" ) },
				{ INT_KEY( 3 ), TEXT_VALUE( "Boca del Rio, Veracruz" ) },
				{ TRM() },
			{ TRM() },
		{ TRM() },

	{ TEXT_KEY( "ashor" )  , TABLE_VALUE( )         },
		{ INT_KEY( 7043 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog." )  },
		{ INT_KEY( 7002 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog another time." )  },
		{ INT_KEY( 7003 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog for the 3rd time." )  },
		{ INT_KEY( 7004 )    , USR_VALUE ( NULL ) },
		{ INT_KEY( 7008 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog again." )  },
		{ INT_KEY( 7009 )    , TEXT_VALUE( "The quick brown fox jumps over the lazy dog for the last time." )  },
		{ TRM() },

	{ LKV_LAST } 
};



zKeyval MultiLevelzTable[] = {
	{ TEXT_KEY( "cities" )       , TABLE_VALUE( )         },
		/*Database records look a lot like this*/
		{ INT_KEY( 0 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "San Francisco" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "CA" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 870887 ) },
					{ TEXT_KEY( "landmarks" ), TABLE_VALUE( )         },
						{ INT_KEY( 0 )       , TABLE_VALUE( )         },
							{ TEXT_KEY( "name" ),    TEXT_VALUE( "Golden Gate	Bridge" ) },
							{ TEXT_KEY( "long" ),  TEXT_VALUE( "37.820106557807"  ) },
							{ TEXT_KEY( "lat" ),  TEXT_VALUE( "-122.47828728867952" ) },
							{ TRM() },
						{ INT_KEY( 1 )       , TABLE_VALUE( )         },
							{ TEXT_KEY( "name" ),    TEXT_VALUE( "Alcatraz Island" ) },
							{ TEXT_KEY( "long" ),  TEXT_VALUE( "37.8272536122546"  ) },
							{ TEXT_KEY( "lat" ),  TEXT_VALUE( "-122.42296734450015" ) },
							{ TRM() },
						{ TRM() },
					{ TRM() },
				{ TRM() },
			{ TRM() },

		{ INT_KEY( 1 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "New York" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "NY" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 19750000 ) },
					{ TEXT_KEY( "landmarks" ), TABLE_VALUE( )         },
						{ INT_KEY( 0 )       , TABLE_VALUE( )         },
							{ TEXT_KEY( "name" ),    TEXT_VALUE( "Statue of Liberty National Monument" ) },
							{ TEXT_KEY( "long" ),    TEXT_VALUE( "40.68941208336438" ) },
							{ TEXT_KEY( "lat" ),    TEXT_VALUE( "-74.04453258859414" ) },
							{ TRM() },
						{ INT_KEY( 1 )       , TABLE_VALUE( )         },
							{ TEXT_KEY( "name" ),    TEXT_VALUE( "Museum of Modern Art" ) },
							{ TEXT_KEY( "long" ),    TEXT_VALUE( "40.761595206982896"  ) },
							{ TEXT_KEY( "lat" ),    TEXT_VALUE( "-73.97758941557696" ) },
							{ TRM() },
						{ INT_KEY( 2 )       , TABLE_VALUE( )         },
							{ TEXT_KEY( "name" ),    TEXT_VALUE( "Brooklyn Bridge" ) },
							{ TEXT_KEY( "long" ),    TEXT_VALUE( "40.70655756655535" ) },
							{ TEXT_KEY( "lat" ),    TEXT_VALUE( "-73.996724576758" ) },
							{ TRM() },
						{ INT_KEY( 3 )       , TABLE_VALUE( )         },
							{ TEXT_KEY( "name" ),    TEXT_VALUE( "" ) },
							{ TEXT_KEY( "long" ),    TEXT_VALUE( "40.74863555058537"  ) },
							{ TEXT_KEY( "lat" ),    TEXT_VALUE( "-73.98563221557743" ) },
							{ TRM() },
						{ TRM() },
					{ TRM() },
				{ TRM() },
			{ TRM() },

		{ INT_KEY( 2 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "Raleigh" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "NC" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 870887 ) },
					{ TEXT_KEY( "landmarks" ), TABLE_VALUE( )         },
						{ INT_KEY( 0 )       , TABLE_VALUE( )         },
							{ TEXT_KEY( "name" ),  TEXT_VALUE( "North Carolina Museum of Art" ) },
							{ TEXT_KEY( "long" ),  TEXT_VALUE( "35.8103090163288" ) },
							{ TEXT_KEY( "lat" ),  TEXT_VALUE( "-78.70240260185079" ) },
							{ TRM() },
						{ TRM() },
					{ TRM() },
				{ TRM() },
			{ TRM() },
		{ TRM() },
	{ LKV_LAST } 
};


#if 0
zKeyval MultiLevelKeyValTable[] = {
	{ TEXT_KEY( "cities" )       , TABLE_VALUE( )         },
		/*Database records look a lot like this*/
		{ INT_KEY( 0 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "San Francisco" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "CA" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					//Pay attention to this, I'd like to embed unsigned char data here (I think Lua can handle this)
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 870887 ) },
					{ TEXT_KEY( "demographics" ), TABLE_VALUE( )         },
					{ INT_KEY( 0 )       , TABLE_VALUE( )         },
						{ TEXT_KEY( "Black" ),    INT_VALUE( 5 ) },
						{ TEXT_KEY( "White" ),    INT_VALUE( 40 ) },
						{ TEXT_KEY( "Asian" ),    INT_VALUE( 35 ) },
						{ TEXT_KEY( "Hispanic" ), INT_VALUE( 15 ) },
						{ TEXT_KEY( "Other" ),    INT_VALUE( 20 ) },
						{ TRM() },
					{ TRM() },
				{ TRM() },
			{ TRM() },
		{ TRM() },

		{ INT_KEY( 1 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "New York" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "NY" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 19750000 ) },
					{ TEXT_KEY( "demographics" ), TABLE_VALUE( )         },
					{ INT_KEY( 0 )       , TABLE_VALUE( )         },
						{ TEXT_KEY( "Black" ),    INT_VALUE( 17 ) },
						{ TEXT_KEY( "White" ),    INT_VALUE( 55 ) },
						{ TEXT_KEY( "Asian" ),    INT_VALUE( 8 ) },
						{ TEXT_KEY( "Hispanic" ), INT_VALUE( 19 ) },
						{ TEXT_KEY( "Other" ),    INT_VALUE( 13 ) },
						{ TRM() },
					{ TRM() },
				{ TRM() },
			{ TRM() },
		{ TRM() },

		{ INT_KEY( 2 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "Raleigh" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "NC" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ INT_KEY( 0 )       , TABLE_VALUE( )         },
					{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
					{ TEXT_KEY( "population" ), INT_VALUE( 870887 ) },
					{ TEXT_KEY( "demographics" ), TABLE_VALUE( )         },
					{ INT_KEY( 0 )       , TABLE_VALUE( )         },
						{ TEXT_KEY( "Black" ),    INT_VALUE( 17 ) },
						{ TEXT_KEY( "White" ),    INT_VALUE( 55 ) },
						{ TEXT_KEY( "Asian" ),    INT_VALUE( 8 ) },
						{ TEXT_KEY( "Hispanic" ), INT_VALUE( 19 ) },
						{ TEXT_KEY( "Other" ),    INT_VALUE( 13 ) },
						{ TRM() },
					{ TRM() },
				{ TRM() },
			{ TRM() },
		{ TRM() },
		{ TRM() },
	{ LKV_LAST } 
};
#endif


#if 0
zKeyval MultiLevelzTableExtreme[] = {
	{ TEXT_KEY( "cities" )       , TABLE_VALUE( )         },
		/*Database records look a lot like this*/
		{ INT_KEY( 0 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "San Francisco" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "CA" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				//Pay attention to this, I'd like to embed unsigned char data here (I think Lua can handle this)
				{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
				{ TEXT_KEY( "population" ), INT_VALUE( 870887 ) },
				{ TEXT_KEY( "demographics" ), TABLE_VALUE( )         },
#if 0
					{ TEXT_KEY( "Black" ),    INT_VALUE( 5.5 ) },
					{ TEXT_KEY( "White" ),    INT_VALUE( 40.5 ) },
					{ TEXT_KEY( "Asian" ),    INT_VALUE( 35.4 ) },
					{ TEXT_KEY( "Hispanic" ), INT_VALUE( 15.2 ) },
#else
					{ TEXT_KEY( "Black" ),    INT_VALUE( 5 ) },
					{ TEXT_KEY( "White" ),    INT_VALUE( 40 ) },
					{ TEXT_KEY( "Asian" ),    INT_VALUE( 35 ) },
					{ TEXT_KEY( "Hispanic" ), INT_VALUE( 15 ) },
#endif
					{ TEXT_KEY( "Other" ),    INT_VALUE( 20 ) },
					{ TRM() },
				{ TEXT_KEY( "sisterCities" ), TABLE_VALUE( )         },
					{ INT_KEY( 0 ), TEXT_VALUE( "Sydney, Austrailia" ) },
					{ INT_KEY( 1 ), TEXT_VALUE( "Beijing, China" ) },
					{ INT_KEY( 2 ), TEXT_VALUE( "Perth, Australia" ) },
					{ INT_KEY( 3 ), TEXT_VALUE( "Johannesburg, South Africa" ) },
					{ TRM() },
				{ TRM() },
			{ TRM() },

		{ INT_KEY( 1 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "New York" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "NY" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
				{ TEXT_KEY( "population" ), INT_VALUE( 19750000 ) },
				{ TEXT_KEY( "demographics" ), TABLE_VALUE( )         },
#if 0
					{ TEXT_KEY( "Black" ),    INT_VALUE( 17.7 ) },
					{ TEXT_KEY( "White" ),    INT_VALUE( 55.8 ) },
					{ TEXT_KEY( "Asian" ),    INT_VALUE( 8.9 ) },
#else
					{ TEXT_KEY( "Black" ),    INT_VALUE( 17 ) },
					{ TEXT_KEY( "White" ),    INT_VALUE( 55 ) },
					{ TEXT_KEY( "Asian" ),    INT_VALUE( 8 ) },
#endif
					{ TEXT_KEY( "Hispanic" ), INT_VALUE( 19 ) },
					{ TEXT_KEY( "Other" ),    INT_VALUE( 13 ) },
					{ TRM() },
				{ TEXT_KEY( "sisterCities" ), TABLE_VALUE( )         },
					{ INT_KEY( 0 ), TEXT_VALUE( "Arusha, Tanzania" ) },
					{ INT_KEY( 1 ), TEXT_VALUE( "Durham, United Kingdom" ) },
					{ INT_KEY( 2 ), TEXT_VALUE( "Kostroma, Russia" ) },
					{ INT_KEY( 3 ), TEXT_VALUE( "Toyama, Japan" ) },
					{ INT_KEY( 4 ), TEXT_VALUE( "Zhuzhou, Hunan Province, China" ) },
					{ TRM() },
				{ TRM() },
			{ TRM() },

		{ INT_KEY( 2 )       , TABLE_VALUE( )         },
			{ TEXT_KEY( "city" ), BLOB_VALUE( "Raleigh" ) },
			{ TEXT_KEY( "parent_state" ), TEXT_VALUE( "NC" ) },
			{ TEXT_KEY( "metadata" ), TABLE_VALUE( )         },
				{ TEXT_KEY( "skyline" ), BLOB_VALUE( "CA" ) },
				{ TEXT_KEY( "population" ), INT_VALUE( 870887 ) },
				{ TEXT_KEY( "demographics" ), TABLE_VALUE( )         },
#if 0
					{ TEXT_KEY( "Black" ),    INT_VALUE( 28.4 ) },
					{ TEXT_KEY( "White" ),    INT_VALUE( 57.74 ) },
					{ TEXT_KEY( "Asian" ),    INT_VALUE( 4.69 ) },
					{ TEXT_KEY( "Hispanic" ), INT_VALUE( 11.81 ) },
#else
					{ TEXT_KEY( "Black" ),    INT_VALUE( 28 ) },
					{ TEXT_KEY( "White" ),    INT_VALUE( 57 ) },
					{ TEXT_KEY( "Asian" ),    INT_VALUE( 4 ) },
					{ TEXT_KEY( "Hispanic" ), INT_VALUE( 11 ) },
#endif
					{ TEXT_KEY( "Other" ),    INT_VALUE( 9 ) },
					{ TRM() },
				{ TEXT_KEY( "sisterCities" ), TABLE_VALUE( )         },
					{ INT_KEY( 0 ), TEXT_VALUE( "Agrigento, Sicily" ) },
					{ INT_KEY( 1 ), TEXT_VALUE( "Ashdod South, Isreal" ) },
					{ TRM() },
				{ TRM() },
			{ TRM() },
		{ TRM() },
	{ LKV_LAST } 
};
#endif
#endif


int get_count ( zKeyval *kv ) {
	int a = 0;
	while ( *kv->hash != LKV_TERM ) 
		a ++, kv ++;
	return a;	
}


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


#define TEST(k,d) \
	{ #k, k }



struct Test {
	zKeyval *kvset;
	const char *name, *desc;
	const char *src, *cmp;
	unsigned char *dest;
	int dlen;
};


struct Test tests[] = {
	//Expected failure should also be listed here
#if 0
	{ NozTable, "NO_MATCHES", "No matches found anywhere." },
	{ NozTable, "TABLE_NONE_REALWORLD", "Template values with no tables and <style> tag at the top." },
	{ NozTable, "TABLE_NONE", "Template values with no tables." },
	{ SinglezTable, "TABLE_SINGLE", "one level table" },
	{ DoublezTableAlpha, "TABLE_DOUBLE", "two level table | key value test" },
#endif
	{ MultiLevelzTable, "TABLE_TRIPLE", "three level table | key value test" },
#if 0
	{ NozTable, "TABLE_NONE_FAIL", "Template values with no tables and a bad input source." },
	{ NozTable, "TABLE_NONE_REALWORLD", "Template values with no tables and <style> tag at the top." },
	//{ NozTable, "TABLE_NONE_RWFAIL", "Template values with no tables, <style> tag at the top and bad input." },
	{	SinglezTable, "TABLE_SINGLE_FAIL", "one level table with syntax failure" },
#if 0
	{	DoublezTableNumeric, "TABLE_TWO_LEVEL", "two level table | key value test" },
	{	MultiLevelzTable, "TABLE_KV", "key and value" },
#endif
#endif
	{ NULL }
};



//Utility to read file and path
static char *read_file ( const char *name, const char *path  ) {
	char *buf = NULL, bb[2048] = {0};
	struct stat sb = {0};
	int fd = 0;
	sprintf( bb, "%s/%s", path, name );

	if ( stat( bb, &sb ) == -1 ) goto die;	
	if ( !( buf = malloc( sb.st_size + 1 ) ) || !memset( buf, 0, sb.st_size + 1 ) ) goto die;	
	if ( ( fd = open( bb, O_RDONLY ) ) == -1 ) goto die;	
	if ( ( read( fd, buf, sb.st_size ) ) == -1 ) goto die; 

	close( fd );
	return buf;

die:
	fprintf( stderr, "attempt to load: %s, e: %s\n", bb, strerror( errno ) );
	exit( 0 );
	return NULL;
}


//Utility to write out unsigned data
static void write_unsigned ( unsigned char *msg, int msglen ) {
	write( 2, "<<<<<<<<<<<<<<<<<<<<\n", 21 );
	write( 2, msg, msglen );
	write( 2, ">>>>>>>>>>>>>>>>>>>>\n", 21 );
}



int main (int argc, char *argv[]) {
#if 0
	//Need to add options

	-v, --verbose    Dump the src and cmp
	-c, --compact    Leave everything in one line (default)
		(e.g. $TEST_NAME = SUCCESS (maybe add time) )
#endif

	//....
	struct Test *t = tests;

	while ( t->kvset ) {
		zTable *tt = convert_lkv( t->kvset );
#if 1
		zRender *rz = zrender_init();
		zrender_set_default_dialect( rz );
		zrender_set_fetchdata( rz, tt );

		//Load both t files
		t->src = read_file( t->name, "tests/src" ); 
		t->cmp = read_file( t->name, "tests/cmp" );
		write_unsigned( (unsigned char *)t->src, strlen( t->src ) );

	#if 0
		//This performs a one-shot templating function 
		if ( !( r = zrender_render( rz, (unsigned char *)t->src, strlen(t->src), &rlen ) ) ) {
			fprintf(stderr, "Error rendering template at item: %s\n", t->name );
			goto destroy_zr;
		}
	#else
		//These ought to be seperated out
		if ( !zrender_set_marks( rz, (unsigned char *)t->src, strlen( t->src ) ) ) {
			fprintf( stderr, "set_marks failed\n" ); return 1;
		}

		if ( !zrender_convert_marks( rz ) ) {
			fprintf( stderr, "convert_marks failed\n" ); return 1;
		}

		if ( !zrender_interpret( rz, &t->dest, &t->dlen ) ) {
			fprintf( stderr, "interpret failed\n" ); return 1;
		}
	#endif

		//Dump the message
		write_unsigned( t->dest, t->dlen );

	#if 0
		if ( t->cmp ) {
			fprintf( stderr, "%s\n", 
				memcmp( t->dest, t->cmp, t->dlen ) ? "FAILED" : "SUCCESS" );
		}
	#endif

		//Destroy everything...
		free( t->dest );
		zrender_free( rz );
		lt_free( tt );
		free( tt );
		free( (void *)t->src );
		( t->cmp ) ? free( (void *)t->cmp ) : 0;
#endif
		//lt_dump( tt );lt_free( tt ); free( tt );
		t++;
	}
	return 0;
}
