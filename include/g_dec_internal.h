#ifndef G_DEC_INTERNAL_H
#define G_DEC_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>

/** dec net package format
 *
 * ----package header(12 bytes)----
 * command type(4 bytes, unsigned int)
 * reserved(4 bytes, padding by 0)
 * data size(4 bytes, unsigned int)
 *
 * ----data(optional)----
 * data raws...(data size)
 */

#define COM_HEADER_SIZE 12

#define COM_W_REGISTER 0
#define COM_R_REGISTER 1
#define COM_W_BEAT 2
#define COM_R_BEAT 3

#define COM_S_OK 4
#define COM_S_ERROR 5
#define COM_W_OK 6
#define COM_W_ERROR 7
#define COM_R_OK 8
#define COM_R_ERROR 9

#define COM_W_IDLE 10
#define COM_R_IDLE 11

#define COM_W_TASK 12
#define COM_W_TASK_NONE 13

#define COM_S_BUSY 14
 
#define G_OK 0
#define G_ERROR -1


enum _dec_worker_state{
  WORKER_STATE_REGISTER=1000,
  WORKER_STATE_IDLE,
  WORKER_STATE_BUSY
};

#define BEAT_INTERNAL 10


#ifdef __cplusplus
}
#endif

#endif
