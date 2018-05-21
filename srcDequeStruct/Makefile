#
#  Created by Kaba Saran on 03/04/2018.
#  Copyright © 2018 Kaba. All rights reserved.
#
#declaration des variables

CC=gcc

CFLAGS=-Wall -lpthread -I$(IDIR) -g -O3 #-Werror

IDIR = ../include/
SRCDIR = ./

SOURCES = $(SRCDIR)*.c

all: steal

steal: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) -o $@

run: 
	./steal


clean:
	rm  steal
