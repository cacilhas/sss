SHARED= -shared
ARCH= x86_64

ifeq ($(shell uname), Darwin)
	CC= clang -c
	LD= clang
else
	CC= gcc -c
	LD= gcc
endif

RM= rm -f
AR= ar -r
INSTALL= install -m 0755
CP= install -m 0644
RMDIR= rm -rf
MD= mkdir -p

LUA_PREFIX= /usr/local
LUA_SUFFIX= 5.1
LUA_CPATH= $(LUA_PREFIX)/lib/lua/$(LUA_SUFFIX)
LUA_INCLUDE= $(LUA_PREFIX)/include
LUA_LIB= $(LUA_PREFIX)/lib

OBJECTS= build/wrap.o build/sss.o
PATH= /bin:/usr/bin:/usr/local/bin

CFLAGS= -Wall -I./src -I$(LUA_INCLUDE) -g
LIBS= -lm -L$(LUA_LIB) -llua -ldl

#-----------------------------------------------------------------------
all: dist/lsss.so dist/libluasss.a


.PHONY: clean
clean:
	$(RMDIR) build


.PHONY: mrproper
mrproper: clean
	$(RMDIR) dist


.PHONY: install
install: install_so install_a


.PHONY: install_so
install_so: dist/lsss.so
	$(INSTALL) $< $(LUA_CPATH)


.PHONY: install_a
install_a: dist/libluasss.a src/sss.h
	$(CP) $< $(LUA_LIB)
	$(CP) src/sss.h $(LUA_INCLUDE)


.PHONY: uninstall
uninstall:
	$(RM) $(LUA_CPATH)/lsss.so
	$(RM) $(LUA_LIB)/libluasss.a
	$(RM) $(LUA_INCLUDE)/sss.h


dist/%.so: $(OBJECTS)
	@$(MD) dist
	$(LD) $(SHARED) $? -arch $(ARCH) $(LIBS) -o $@


dist/%.a: $(OBJECTS)
	@$(MD) dist
	$(AR) $@ $?


build/%.o: src/%.c
	@$(MD) build
	$(CC) $< $(CFLAGS) -o $@
