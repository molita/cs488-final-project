
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
    image.o \
    Unit.o \
		glm.o \
		glmimg.o \
		glmimg_devil.o \
		glmimg_jpg.o \
		glmimg_png.o \
		glmimg_sdl.o \
		glmimg_sim.o \
		glm_util.o
		
compileobjs: $(OBJS)

HEADERS = \
    SoundManager.h \
    image.hpp \
    Game.hpp \
    Unit.hpp \
		glm.h \
		glmint.h

lib_tutorial: $(HEADERS) $(OBJS) Makefile
	ar rvs libsm $(OBJS)
	mv libsm libsm.a

sample_tutorial: $(HEADERS) $(OBJS) Makefile
	$(CPP) -o test $(OBJS) $(LIBS)

clean:
	-rm *.o core
