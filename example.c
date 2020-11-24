/* 
example.c 

Summary
-------
For README 

Usage
-----
Compile me with gcc -Wall -Werror zrender.c vendor/{zwalker,zhasher}.c example.c 
*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "vendor/zhasher.h"
#include "vendor/zwalker.h"
#include "zrender.h"
#define EXAMPLE "example/"

//Die
void die( char *msg ) {
	fprintf( stderr, "%s\n", msg );
	exit( 1 );
}


char *loadexample ( const char *file ) {
	//open a buffer
	int fd;
	const int sz = 8196;
	char *buf = malloc( sz );
	memset( buf, 0, sz );
	
	//read in our example file
	if ( ( fd = open( file, O_RDONLY ) ) == -1 ) {
		fprintf( stderr, "Problem opening: %s", file );
		exit( 1 );
	}

	if ( read( fd, buf,  sz - 1 ) == -1 ) {
		fprintf( stderr, "Problem reading entirety of file: %s", file );
		exit( 1 );
	}

	//Don't really care if we couldn't close for this simple test...
	close( fd );
	return buf;
}


//Do simple templating
void simple_rendering() {
	//load our test file
	char *buf = loadexample( EXAMPLE "simple.file" );

	//create the table (TODO: This should be a one-line thing)
	zTable *zt = malloc( sizeof( zTable ) );
	lt_init( zt, NULL, 512 );

	//populate the table	
	zWalker zw = {0};
	while ( strwalk( &zw, buf, "=\n" ) ) {
		char i[ 1024 ] = { 0 };
		memcpy( i, &buf[ zw.pos ], zw.size );
		if ( !zw.size )
			break;
		else if ( zw.chr == '=' )
			lt_addtextkey( zt, i );
		else {
			lt_addtextvalue( zt, i );
			lt_finalize( zt );
		}
	} 
	
	//dump the ztable
	lt_lock( zt );
	//lt_dump( zt );
	free( buf );
	
	//render according to string template
	int dl = 0;
	char * src = "Hello, {{ zuzah }}!  You are using {{ libname }}.";
	zRender * rz = zrender_init();
	zrender_set_default_dialect( rz );
	zrender_set_fetchdata( rz, zt );
	unsigned char * dest = zrender_render( rz, (unsigned char *)src, strlen(src), &dl );
	fprintf( stderr, "%s\n", src );	
	write( 2, dest, dl );
}




//Do templating for a CSV file
void csv_rendering() {
	//load our test file
	char *buf = loadexample( EXAMPLE "courselist.csv" );

	//create the table (TODO: This should be a one-line thing)
	zTable *zt = malloc( sizeof( zTable ) );
	lt_init( zt, NULL, 2048 );

	//get the headers first
	zWalker zw = {0};
	char headers[ 3 ][ 128 ] = { 0 };
	int hi = 0;
	while ( strwalk( &zw, buf, ",\n" ) ) {
		memcpy( headers[ hi ], &buf[ zw.pos ], zw.size );
		hi++;
		if ( zw.chr == '\n' ) {
			break;
		}
	}

	//when looping through tables, zRender assumes that there is a root node to start from
	lt_addtextkey( zt, "root" );
	lt_descend( zt );

	//continue populating the table	
	int rowcount = 0;
	while ( strwalk( &zw, buf, ",\n" ) ) {
		char i[ 1024 ] = { 0 };
		memcpy( i, &buf[ zw.pos ], zw.size );
		if ( ( hi = ( hi == 3 ) ? 0 : hi ) == 0 ) {
			if ( !zw.size ) {
				break;
			}
			lt_addintkey( zt, rowcount ); 
			lt_descend( zt  );
			rowcount++;
		}

		lt_addtextkey( zt, headers[ hi ] );
		lt_addtextvalue( zt, i );
		lt_finalize( zt );
		if ( ++hi == 3 ) {
			lt_ascend( zt );
		}
	}

	//jump back to the root level and "freeze" the table
	lt_ascend( zt );
	lt_finalize( zt );
	lt_lock( zt );
	
	//dump the ztable
	//lt_dump( zt );

	//try a render
	const char src[] =
"<html>\n\
	<table>\n\
		{{ #root }}\n\
		<tr>\n\
			<td>{{ .course_name }}</td>\n\
			<td>{{ .course_num }}</td>\n\
			<td>{{ .course_description }}</td>\n\
		</tr>\n\
		{{ /root }}\n\
	</table>\n\
</html>\n\
	";
	
	int dl = 0;
	zRender * rz = zrender_init();
	zrender_set_default_dialect( rz );
	zrender_set_fetchdata( rz, zt );
	unsigned char * dest = zrender_render( rz, (unsigned char *)src, strlen(src), &dl );
	fprintf( stderr, "%s\n", src );
	write( 2, dest, dl );
}


typedef enum {
	NO_ACTION = 0
, SIMPLE_ACTION
, COMPLEX_ACTION
, CSV_ACTION
} Action;


int main ( int argc, char *argv[] ) {
	if ( argc < 2 ) {
		fprintf( stderr, "./example usage:\n" );
		fprintf( stderr, "-s, --simple       Test out simple templating.\n" );
		fprintf( stderr, "-c, --csv          Test out templating with CSV as source.\n" );
		return 1;
	}

	//Loop through CSV files
	argv++;
	Action action = NO_ACTION;
	while ( *argv ) {
		if ( !strcmp( "-s", *argv ) || !strcmp( "--simple", *argv ) )
			action = SIMPLE_ACTION;
		else if ( !strcmp( "-c", *argv ) || !strcmp( "--csv", *argv ) ) 
			action = CSV_ACTION;
		else {
			fprintf( stderr, "Unrecognized option: %s\n", *argv );
			return 1;
		}
		argv++;
	}

	//Convert our datatypes to tables
	if ( action == SIMPLE_ACTION ) 
		simple_rendering();
	else if ( action == CSV_ACTION ) {
		csv_rendering();
	}	

	return 0;
}
