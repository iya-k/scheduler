#
#  Created by Kaba Saran on 03/04/2018.
#  Copyright © 2017 Kaba. All rights reserved.
#
#declaration des variables

CC=gcc

CFLAGS=-Wall -lpthread -I$(IDIR) -g -Werror

IDIR = ./include/
SRCDIR = ./src/

SOURCES = $(SRCDIR)*.c

all: lifo

lifo: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o $@

run: 
	./lifo


clean:
	rm  lifo
