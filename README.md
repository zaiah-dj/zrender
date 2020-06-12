# zrender

Drop-in templating for C/C++


## Installation

Like other z-prefixed libraries, there will probably never be an official package for this.   zrender should build on any Unix-based platform with whatever compiler you choose.

zrender does require <a href="https://github.com/zaiah-dj/zwalker">zwalker</a> in order to work correctly.  It ships in the vendor/ folder of this source code. 

If dropping it into your own project, you can use:
	gcc -Wall -Werror -std=c99 -c zwalker.c
	gcc -Wall -Werror -std=c99 zwalker.o zrender.c main.c


## Usage

Templating for compiled languages can get incredibly complicated.
Let's assume we have something like the following:

<pre>
typedef struct keyval { char * zuzah, * libname; } KeyVal;
KeyVal k = { .zuzah = "Lucious", .libname = "zrender" }; 
char * src = "Hello, {{ zuzah }}!  You are using {{ libname }}.";
</pre>


By default, zrender uses a Mustache-like language for templating.
<pre>
zRender * rz = zrender_init();
uint8_t * dest = NULL;
int destlen = 0;
zrender_use_default( rz );
zrender_one_shot( rz, dest, &destlen, data, src, srclen );
</pre>

zrender does not only support just one language.

The templating process can be broken down into a few distinct steps.  


