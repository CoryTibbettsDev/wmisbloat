PROG := wmisbloat
VERSION ?= 0.1

RM ?= rm -f
CP ?= cp -f

PREFIX?=/usr/local
DESTDIR?=
BINDIR?=${DESTDIR}${PREFIX}/bin

CFLAGS += -Wall
CPPFLAGS += -DPROG="\"${PROG}\"" -DVERSION="\"${VERSION}\""
# Extra flags for debugging
CFDEBUG = -g -pedantic -Wno-deprecated-declarations -Wunused-parameter -Wlong-long \
		Wsign-conversion -Wconversion -Wimplicit-function-declaration
# Included libraries
LDFLAGS += -lc -lxcb -lxcb-util# -lxcb-randr -lxcb-xinerama

SRC := main.c
OBJ := ${SRC:.c=.o}

.PHONY: all clean install uninstall

all: ${PROG}

.SUFFIXES: .c .o
.c.o:
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

${PROG}: ${OBJ}
	${CC} ${LDFLAGS} ${OBJ} -o ${PROG}

# Cannot launch Xephyr from Makefile because of bug see README.md
test: CFLAGS += ${CFDEBUG}
test: all
	# X in Xephyr must be capitilized
	# Xephyr -screen "1024x650" :1
	# DISPLAY=:1.0 ./wmisbloat
	# DISPLAY=:1.0 xterm
	./xephyr.sh

config:
	${CP} config.def.h config.h

install: all
	install -d ${BINDIR}
	install -m 0755 ${PROG} ${BINDIR}/${PROG}

uninstall:
	${RM} ${BINDIR}/${PROG}

clean:
	${RM} ${PROG} *.o
