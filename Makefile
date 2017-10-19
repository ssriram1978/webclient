#CC = colorgcc
GPP = g++
top_srcdir = /home/ssridhar/git/webclient


# -Wno-deprecatd-declarations is for 
CFLAGS+= -g -Wall -Wformat-nonliteral -Wformat-security \
         -Wno-deprecated-declarations \
         -I${top_srcdir}/include \
         #-DSINGLE_ROW_SET \

CPPFLAGS+= -std=c++11 

LDFLAGS +=    -Wl,-rpath-link \
	      -lpthread \
	      -lrt \
              -lz \

web_client_c_sources = \

web_client_cpp_sources = \
        src/web_client_main.cpp \
        src/QueueFactory.cpp \
        src/StateFactory.cpp \
        src/Job.cpp \
        src/JobFactory.cpp \
        src/SchedulerFactory.cpp \


web_client_c_objects=$(web_client_c_sources:%.c=%.o)


web_client_cpp_objects=$(web_client_cpp_sources:%.cpp=%.o)

web_client_objects=$(web_client_c_objects) $(web_client_cpp_objects)

APPS = WEBCLIENT
all: $(APPS)
#

WEBCLIENT: $(web_client_objects)
	$(GPP) $(CFLAGS) $(CPPFLAGS)  -fPIC  $^ -o $@  $(LDFLAGS) \
#
%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

%.o: %.cpp
	$(GPP) $(CFLAGS) $(CPPFLAGS) -fPIC -c $< -o $@

install:
	#cp WEBCLIENT 

clean:
	rm -f $(APPS)
	rm -f *.o
	find . -type f -name '*.o' -delete

clobber:
	rm -rf ./build

distclean:
	rm -rf ./build
	find . -type f -name '*.o' -delete

dist: WEBCLIENT
	pkg/stageandpkg.sh WEBCLIENT $(VERSION) --clean
