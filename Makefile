#CC = colorgcc
GPP = g++
top_srcdir = /home/ssridhar/git/webclient


# -Wno-deprecatd-declarations is for 
CFLAGS+= -g -Wall -Wformat-nonliteral -Wformat-security \
         -Wno-deprecated-declarations \
         -Iinclude \
         #-DSINGLE_ROW_SET \

CPPFLAGS+= -std=c++11 

LDFLAGS +=    -lpthread \
	      -lz \
	      #-Wl \
	      

web_client_c_sources = \

web_client_cpp_sources = \
	src/Log.cpp \
        src/QueueFactory.cpp \
        src/StateFactory.cpp \
        src/Job.cpp \
        src/JobFactory.cpp \
        src/MutexFactory.cpp \
        src/SchedulerFactory.cpp \
        src/ThreadFactory.cpp \
        src/web_client_main.cpp \
	src/stdin_cb.cpp \
	src/one_second_timer.cpp \
        

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
