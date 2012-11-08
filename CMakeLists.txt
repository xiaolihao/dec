CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(dec)

ADD_DEFINITIONS("-g -Wall")
SET(CMAKE_INSTALL_PREFIX ".")
INCLUDE_DIRECTORIES(include
			minizip/include
			/usr/local/include
			/opt/local/include
			/opt/local/include/glib-2.0
			/opt/local/lib/glib-2.0/include)
LINK_DIRECTORIES(${_dec_SOURCE_DIR}/lib
			/usr/lib
			/usr/local/lib
			/opt/local/lib)

SET(dec_src_list	
	src/g_dec_serv.c
	src/g_dec_util.c
	src/g_dec_worker.c
	src/g_dec_reducer.c)

set(zip_src_list
	minizip/src/ioapi.c
	minizip/src/zip.c
	minizip/src/unzip.c
	minizip/src/mztools.c)

ADD_LIBRARY(zip ${zip_src_list})

ADD_LIBRARY(dec ${dec_src_list})
TARGET_LINK_LIBRARIES(dec  zip z event glib-2.0.0)

ADD_EXECUTABLE(_dec src/_dec.c)
TARGET_LINK_LIBRARIES(_dec dec z event glib-2.0.0)
