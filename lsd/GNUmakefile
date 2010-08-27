lib := libsd
lib_suffix := .a
lsd_objs += conf.o
lsd_objs += daemon.o
lsd_objs += log.o
lsd_objs += match.o
lsd_objs += module.o
lsd_objs += signal.o
lsd_objs += stringhelper.o
lsd_objs += time.o
CXXFLAGS += -std=gnu++0x -I../
CXXFLAGS += -DBUILDING_LIBSD
CXXFLAGS += -O3 -g
CXXFLAGS += -DPLUGINDIR="\"/usr/share/appworx/modules/\""


# ? pkg-config has glib-2.0:cflags ?
CXXFLAGS += $(shell pkg-config --cflags glib-2.0)
LDFLAGS += $(shell pkg-config --libs glib-2.0)

#? pkgmgr has {os.path.includes}/ev.h => ev ?>
ev_inc += $(foreach ev,this,-I$(dirname $this))
#? filesystem has ev.h => ev ?>

all: ${lib}${lib_suffix}

libsd.a: ${lsd_objs}
	${AR} rc $@ $^
	ranlib $@

clean:
	${RM} -f ${lsd_objs}
