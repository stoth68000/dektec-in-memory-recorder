
CPPFLAGS  = -g -O2 -I/usr/local/include -I../LinuxSDK/DTAPI/Include/DTAPI -I../LinuxSDK/DTAPI/Include
CPPFLAGS += -g -O2 -I./Import -fexceptions -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE

all:	DtCollector dtanalyzer

DtCollector:	DtCollector.cpp DtCollector.h
	g++ $(CPPFLAGS) -c -o $(@).o $(@).cpp
	g++ $(CPPFLAGS) DtCollector.o ../LinuxSDK/DTAPI/Lib/GCC4.4/DTAPI64.o -lpthread -ldl -o $(@)

dtanalyzer:	dtanalyzer.c
	gcc --std=c99 -g -Wall $(@).c -o $(@)

clean:
	rm -f DtCollector DtCollector.o dtanalyzer
