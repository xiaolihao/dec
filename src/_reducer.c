#include <stdio.h>
#include <stdlib.h>

#include "dec.h"

int main(){
  DEC_REDUCER reducer=NULL;
  char *app[3]={"app1", "app2", "app3"};
  reducer = g_dec_reducer_init("127.0.0.1",
			       "9000",
			       app,
			       3,
			       "127.0.0.2",
			       "9001",
			       10);
  if(reducer){
    g_dec_reducer_start(reducer);
  }
  return 0;
}
