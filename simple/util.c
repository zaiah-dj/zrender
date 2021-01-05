//util.c

//read file and path
unsigned char *read_file ( const char *path ) {
	unsigned char *buf = NULL;
	struct stat sb = {0};
	int fd = 0;

	if ( stat( path, &sb ) == -1 ) goto die;	
	if ( !( buf = malloc( sb.st_size + 1 ) ) || !memset( buf, 0, sb.st_size + 1 ) ) goto die;	
	if ( ( fd = open( path, O_RDONLY ) ) == -1 ) goto die;	
	if ( ( read( fd, buf, sb.st_size ) ) == -1 ) goto die; 

	close( fd );
	return buf;

die:
	fprintf( stderr, "attempt to load: %s, e: %s\n", path, strerror( errno ) );
	exit( 0 );
	return NULL;
}


//Utility to add to a series of items
void * add_item_to_list( void ***list, void *element, int size, int * len ) {
	//Reallocate
	if (( (*list) = realloc( (*list), size * ( (*len) + 2 ) )) == NULL ) {
		ZRENDER_PRINTF( "Failed to reallocate block from %d to %d\n", size, size * ((*len) + 2) ); 
		return NULL;
	}

	(*list)[ *len ] = element; 
	(*list)[ (*len) + 1 ] = NULL; 
	(*len) += 1; 
	return list;
}
