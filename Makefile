
INCLUDES = \
    -I. \
    -I/usr/include/SDL


LIBS = \
	-lGL -lGLU -lglut -lSDL -lSDL_mixer -lpng

GPPFLAGS = \
    $(INCLUDES) \


ICCFLAGS = \
    $(INCLUDES) 


CPP = g++ $(GPPFLAGS)
#CPP = g++ $(IPPFLAGS)


.cpp.o:
	$(CPP) -g -c -o $@ $<
.c.o:
	$(CC) -g -c -o $@ $<

OBJS = \
    SoundManager.o \
    main.o \
    Game.o \
    image.o

compileobjs: $(OBJS)

HEADERS = \
    SoundManager.h \
    image.hpp \
    Game.h 

lib_tutorial: $(HEADERS) $(OBJS) Makefile
	ar rvs libsm $(OBJS)
	mv libsm libsm.a

sample_tutorial: $(HEADERS) $(OBJS) Makefile
	$(CPP) -o test $(OBJS) $(LIBS)

clean:
	-rm *.o core
