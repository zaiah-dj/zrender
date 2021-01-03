# zrender

Drop-in templating for C/C++ using the zhasher hash table library.


## Installation

Like other z-prefixed libraries, there will probably never be an official package for this.   zrender should build on any Unix-based platform with whatever compiler you choose.

zrender requires both <a href="https://github.com/zaiah-dj/zhasher">zhasher</a> and <a href="https://github.com/zaiah-dj/zwalker">zwalker</a> in order to work correctly.  Both libraries ship in the vendor/ folder of this source code. 

If dropping it into your own project, the three files need to be specified somewhere in the build command:
	`gcc -Wall -Werror -std=c99 zhasher.c zwalker.c zrender.c main.c`

where main.c is the file containing your program.


## Usage

### Whirlwind

By default, zrender uses a Mustache-like language for templating.   
So assuming we have a string like the following:

<pre>
char * src = "Hello, {{ zuzah }}!  You are using {{ libname }}.";
</pre>

...and data input following the format:
<pre>
zuzah="T'Pol"
libname="Trellium D"
name="Shazam"
</pre>

We expect to see this in our program's output:

<pre>
Hello, "T'Pol"!  You are using "Trellium D".
</pre>



### Implementation

Our data input in the section above can be found in the file `example/simple.file` within this repository.  
An example parser can be written in whichever language you like, but for the purposes of this tutorial, one is included in example.c
<pre>

</pre> 

Let's assume we have written a function 'read_file_to_table', which contains a parser capable of converting the contents of `simple.file` into a zTable.  
<pre>
zTable * srcdata = malloc( sizeof( zTable ) );
lt_init( zt, NULL, 2048 );
read_file_to_table( zt, "simple.file" );
</pre>


We can now initialize the renderer.
<pre>
//Define placeholders for our rendered data
uint8_t * dest = NULL;
int destlen = 0;

//Initialize the templating engine
zRender * rz = zrender_init();
zrender_set_fetchdata( rz, srcdata );
zrender_set_default_dialect( rz );
dest = zrender_render( rz, src, srclen, &destlen );

//Dump the results
write( 1, dest, destlen );
</pre>

Running this block of code will yield something like the following. (This example can be seen in `example.c`)
<pre>
Hello, "T'Pol"!  You are using "Trellium D".
</pre>

When we're done, we will have to free the resulting buffer, the zRender object, and any source data that will not be automatically deallocated.
<pre>
free( dest );
zrender_destroy( rz );
</pre>



