#################################################################################

DESTDIR=/usr
PREFIX=/local

#DEBUG	= -g -O0
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I$(DESTDIR)$(PREFIX)/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -std=gnu99 

LDFLAGS	= -L$(DESTDIR)$(PREFIX)/lib
LIBS    = -lwiringPi -lwiringPiDev -lpthread

# May not need to alter anything below this line
###############################################################################

DSRC = cfplightsd.c state.c pins.c sixaxis.c keyboard.c

DOBJ = $(DSRC:.c=.o)

SRC	=	lights.c state.c pins.c sixaxis.c keyboard.c

OBJ	=	$(SRC:.c=.o)

all:	lights cfplightsd

lights:	$(OBJ)
	@echo [Link]
	@$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

cfplightsd: $(DOBJ)
	@echo [Link]
	@$(CC) -o $@ $(DOBJ) $(LDFLAGS) $(LIBS)
	
.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(OBJ) $(DOBJ) lights cfplightsd *~ core tags *.bak

.PHONY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

.PHONY: install
install: lights
	@echo "[Install]"
	@cp lights        $(DESTDIR)$(PREFIX)/bin
	@chown root.root  $(DESTDIR)$(PREFIX)/bin/lights
	@chmod 4755       $(DESTDIR)$(PREFIX)/bin/lights

.PHONY: installd
installd: cfplightsd
	@echo "[Installing Daemon]"
	@cp cfplightsd      $(DESTDIR)$(PREFIX)/sbin
	@chown root.root    $(DESTDIR)$(PREFIX)/sbin/cfplightsd
	@chmod 4755         $(DESTDIR)$(PREFIX)/sbin/cfplightsd
	@cp cfplights.sh /etc/init.d/cfplights
	@chmod +x /etc/init.d/cfplights
	@update-rc.d cfplights defaults

