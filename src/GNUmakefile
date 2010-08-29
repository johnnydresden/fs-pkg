bin := ../bin/fs-pkg

bin_objs += main.o
CXXFLAGS += -std=gnu++0x -I..
CXXFLAGS += -O3 -g
CXXFLAGS += -DPATH_CF="\"/etc/fs-pkg.cf\""
LDFLAGS += ../lsd/libsd.a -lev

# ? pkg-config has glib-2.0:cflags ?
CXXFLAGS += $(shell pkg-config --cflags glib-2.0 gthread-2.0)
LDFLAGS += $(shell pkg-config --libs glib-2.0 gthread-2.0)

all: world

world:${bin}

${bin}: ${bin_objs}
	${CXX}  ${LDFLAGS} -o $@ $^ ../lsd/libsd.a

clean:
	${RM} -f ${bin} ${bin_objs}
