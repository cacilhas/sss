SHARED= -shared
ARCH= x86_64

CC= clang -c
LD= clang

RM= rm -f
INSTALL= install -m 0755
RMDIR= rm -rf
MD= mkdir -p

LUA_PREFIX= /usr/local
LUA_SUFFIX= 5.1
LUA_CPATH= $(LUA_PREFIX)/lib/lua/$(LUA_SUFFIX)
LUA_INCLUDE= $(LUA_PREFIX)/include
LUA_LIB= $(LUA_PREFIX)/lib

TARGET= dist/lsss.so
OBJECTS= build/wrap.o build/sss.o
PATH= /bin:/usr/bin:/usr/local/bin

CFLAGS= -Wall -I./src -I$(LUA_INCLUDE) -g
LIBS= -lm -L$(LUA_LIB) -llua -ldl `libgcrypt-config --libs`

#-----------------------------------------------------------------------
all: $(TARGET)


.PHONY: clean
clean:
	$(RMDIR) build


.PHONY: mrproper
mrproper: clean
	$(RMDIR) dist


.PHONY: install
install: $(TARGET)
	$(INSTALL) $< $(LUA_CPATH)


$(TARGET): $(OBJECTS)
	@$(MD) dist
	$(LD) $(SHARED) $? -arch $(ARCH) $(LIBS) -o $@


build/%.o: src/%.c
	@$(MD) build
	$(CC) $< $(CFLAGS) -o $@
