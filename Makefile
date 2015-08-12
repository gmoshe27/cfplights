#################################################################################

DESTDIR=/usr
PREFIX=/local

#DEBUG	= -g -O0
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I$(DESTDIR)$(PREFIX)/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -std=c99

LDFLAGS	= -L$(DESTDIR)$(PREFIX)/lib
LIBS    = -lwiringPi -lwiringPiDev -lpthread -lm

# May not need to alter anything below this line
###############################################################################

SRC	=	lights.c state.c pins.c

OBJ	=	$(SRC:.c=.o)

all:		lights

lights:	$(OBJ)
	@echo [Link]
	@$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)
	
.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(OBJ) lights *~ core tags *.bak

.PHONY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

.PHONY: install
install: lights
	@echo "[Install]"
	@chown root.root lights
	@chmod 4755 lights
#    @cp gpio        $(DESTDIR)$(PREFIX)/bin
#    @chown root.root    $(DESTDIR)$(PREFIX)/bin/gpio
#    @chmod 4755     $(DESTDIR)$(PREFIX)/bin/gpio
#    @mkdir -p       $(DESTDIR)$(PREFIX)/man/man1
#    @cp gpio.1      $(DESTDIR)$(PREFIX)/man/man1

