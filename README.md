# zrender

Drop-in templating for C/C++ using the zhasher hash table library.


## Installation

Like other z-prefixed libraries, there will probably never be an official package for this.   zrender should build on any Unix-based platform with whatever compiler you choose.

zrender requires both <a href="https://github.com/zaiah-dj/zhasher">zhasher</a> and <a href="https://github.com/zaiah-dj/zwalker">zwalker</a> in order to work correctly.  Both libraries ship in the vendor/ folder of this source code. 

If dropping it into your own project, the three files need to be specified somewhere in the build command:
	gcc -Wall -Werror -std=c99 zhasher.c zwalker.c zrender.c main.c


## Usage

Let's assume we have a string like the following:

<pre>
char * src = "Hello, {{ zuzah }}!  You are using {{ libname }}.";
</pre>


Let's also assume we've created a zTable with the key 'zuzah' and 'libname' somewhere.
<pre>
zTable srcdata;
</pre>


By default, zrender uses a Mustache-like language for templating. 
<pre>
//Define placeholders for our rendered data
uint8_t * dest = NULL;
int destlen = 0;

//Initialize the templating engine
zRender * rz = zrender_init();
zrender_set_default_dialect( rz );
dest = zrender_render( rz, src, srclen, &destlen );

//Dump the results
write( 1, dest, destlen );
</pre>

When we're done, we will have to free the resulting buffer, the zRender object, and any source data that will not be automatically deallocated.
<pre>
free( dest );
zrender_destroy( rz );
</pre>

zrender does not only support just one language.

The templating process can be broken down into a few distinct steps.  


