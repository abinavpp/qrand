.SUFFIXES:
# disables implicit rules

TGT=qrand

LD=gcc
LDFLAGS+=-lfl

CC=gcc
override CFLAGS+=-g
CPPFLAGS+=-MMD

LEX=flex
LEXFLAGS+=

SRC=$(wildcard *.c)
SRC_LEX=$(wildcard *.l)
OBJ=$(SRC:%.c=%.o)
OBJ+=lex.yy.o
DEP=$(SRC:%.c=%.d)

.PHONY: all 
all : $(TGT)

lex.yy.c : $(SRC_LEX)
	$(LEX) $(LEXFLAGS) $^

lex.yy.o : lex.yy.c

%.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

$(TGT) : $(OBJ)
	$(LD) $(LDFLAGS) $(CFLAGS) $^ -o $@

.PHONY: clean
clean : 
	rm -f $(DEP) $(OBJ) $(TGT) lex.yy.*

-include $(DEP)
