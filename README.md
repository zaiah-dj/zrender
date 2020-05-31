# zrender

Drop-in templating for C/C++


## Installation

Like other z-prefixed libraries, there will probably never be an official package for this.   zrender should build on any Unix-based platform with whatever compiler you choose.

zrender does require <a href="https://github.com/zaiah-dj/zwalker">zwalker</a> in order to work correctly.  It ships in the vendor/ folder of this source code. 

If dropping it into your own project, you can use:
	gcc -Wall -Werror -std=c99 -c zwalker.c
	gcc -Wall -Werror -std=c99 zwalker.o zrender.c main.c


## Usage

By default, zrender uses a Mustache-like language for templating.   If this is fine, templating can be done via
<pre>
uint8_t * dest = NULL;
zrender_set_cmp( cmp );
zrender_one_shot( dest, &destlen, data, src, srclen );
</pre>

The templating process can be broken down into a few distinct steps.  


