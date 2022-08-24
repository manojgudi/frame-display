#
# Sheep counter program
#
# Makefile used to build the software
#
# Copyright 2015-2022 Nicolas Mora <mail@babelouest.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the MIT License
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

ULFIUS_REPO=$(ULFIUS_PATH)
CC=gcc
ULFIUS_LOCATION=$(ULFIUS_REPO)/src
ULFIUS_INCLUDE=$(ULFIUS_REPO)/include
EXAMPLE_INCLUDE=$(ULFIUS_REPO)/example_programs/include
STATIC_COMPRESSED_INMEMORY_WEBSITE=$(ULFIUS_REPO)/example_callbacks/static_compressed_inmemory_website
HTTP_COMRESSION=$(ULFIUS_REPO)/example_callbacks/http_compression
CFLAGS+=-c -Wall -I$(ULFIUS_INCLUDE) -I$(EXAMPLE_INCLUDE) -I$(STATIC_COMPRESSED_INMEMORY_WEBSITE) -I$(HTTP_COMRESSION) -D_REENTRANT $(ADDITIONALFLAGS) $(CPPFLAGS)
LIBS=-lc -lz -lpthread -lulfius -lorcania -ljansson -L$(ULFIUS_LOCATION)

ifndef YDERFLAG
LIBS+= -lyder
endif

all: webservice

clean:
	rm -f *.o webservice

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: webservice

../../src/libulfius.so:
	cd $(ULFIUS_LOCATION) && $(MAKE) debug CURLFLAG=1 GNUTLSFLAG=1

static_compressed_inmemory_website_callback.o: $(STATIC_COMPRESSED_INMEMORY_WEBSITE)/static_compressed_inmemory_website_callback.c
	$(CC) $(CFLAGS) $(STATIC_COMPRESSED_INMEMORY_WEBSITE)/static_compressed_inmemory_website_callback.c

http_compression_callback.o: $(HTTP_COMRESSION)/http_compression_callback.c
	$(CC) $(CFLAGS) $(HTTP_COMRESSION)/http_compression_callback.c

#routes.o: routes.c 
#	$(CC) $(CFLAGS) routes.c -DDEBUG -g -O0

webservice.o: webservice.c 
	$(CC) $(CFLAGS) webservice.c -DDEBUG -g -O0

webservice: ../../src/libulfius.so webservice.o static_compressed_inmemory_website_callback.o http_compression_callback.o 
	$(CC) -o webservice webservice.o static_compressed_inmemory_website_callback.o http_compression_callback.o $(LIBS)

test: webservice
	LD_LIBRARY_PATH=$(ULFIUS_LOCATION):${LD_LIBRARY_PATH} ./webservice
