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

BINDIR = bin
SRCDIR = src
OBJDIR = obj

VPATH := $(SRCDIR)

DSRC = cfplightsd.c state.c pins.c sixaxis.c keyboard.c
DOBJ = $(DSRC:%.c=$(OBJDIR)/%.o)

SRC	=	lights.c state.c pins.c sixaxis.c keyboard.c
OBJ	=	$(SRC:%.c=$(OBJDIR)/%.o)

all:	lights cfplightsd sixpair

lights:	$(OBJ)
	@echo [Link]
	@mkdir -p $(BINDIR)
	@$(CC) -o $(BINDIR)/$@ $(OBJ) $(LDFLAGS) $(LIBS)

cfplightsd: $(DOBJ)
	@echo [Link]
	@mkdir -p $(BINDIR)
	@$(CC) -o $(BINDIR)/$@ $(DOBJ) $(LDFLAGS) $(LIBS)

.PHONY: sixpair
sixpair: 
	@echo [Compile Sixpair]
	@$(CC) -o $(BINDIR)/sixpair sixpair/sixpair.c -lusb
	
obj/%.o:src/%.c
	@echo [Compile] $<
	@mkdir -p $(OBJDIR)
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(OBJDIR)/*.o $(BINDIR)/* *~ core tags *.bak

#.PHONY:	tags
#tags:	$(SRC)
#	@echo [ctags]
#	@ctags $(SRC)

.PHONY: install
install: cfplightsd
	@echo "[Installing Daemon]"
	@cp $(BINDIR)/cfplightsd      $(DESTDIR)$(PREFIX)/sbin
	@chown root.root    $(DESTDIR)$(PREFIX)/sbin/cfplightsd
	@chmod 4755         $(DESTDIR)$(PREFIX)/sbin/cfplightsd
	@cp cfplights.sh /etc/init.d/cfplights
	@chmod +x /etc/init.d/cfplights
	@update-rc.d cfplights defaults

