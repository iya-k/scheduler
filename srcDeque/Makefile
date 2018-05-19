#
#  Created by Kaba Saran on 03/04/2018.
#  Copyright © 2017 Kaba. All rights reserved.
#
#declaration des variables

CC=gcc

CFLAGS=-Wall -lpthread -I$(IDIR) -g -O3 #-Werror

IDIR = ../include/
SRCDIR = ./

SOURCES = $(SRCDIR)*.c

all: steal

lifo: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o $@

run: 
	./steal


clean:
	rm  steal
