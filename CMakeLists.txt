CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(dec)

ADD_DEFINITIONS("-g -Wall")
SET(CMAKE_INSTALL_PREFIX ".")
INCLUDE_DIRECTORIES(include
			/usr/local/include
			/opt/local/include
			/opt/local/include/glib-2.0
			/opt/local/lib/glib-2.0/include)
LINK_DIRECTORIES(${_dec_SOURCE_DIR}/lib
			/usr/lib
			/usr/local/lib
			/opt/local/lib)

SET(src_list	
	src/g_dec_serv.c
	src/g_dec_util.c
	src/g_dec_worker.c)
ADD_LIBRARY(dec ${src_list})
TARGET_LINK_LIBRARIES(dec  event glib-2.0.0)

ADD_EXECUTABLE(_serv src/_serv.c)
TARGET_LINK_LIBRARIES(_serv dec event glib-2.0.0)

ADD_EXECUTABLE(_worker src/_worker.c)
TARGET_LINK_LIBRARIES(_worker dec event glib-2.0.0)

