
#ifndef __XLINK_RINGBUF_H__
#define __XLINK_RINGBUF_H__

#include "xlink_demo.h"


#define min(a, b) (a)<(b)?(a):(b)

typedef struct{
    unsigned char *buffer;
	unsigned int in;
	unsigned int out;
    unsigned int size;
	unsigned char f_lock;
}Kfifo;

extern void Kfifo_Init( Kfifo *fifo, unsigned char *buffer, unsigned int size );
extern void Kfifo_Lock(Kfifo *fifo);
extern void Kfifo_UnLock(Kfifo *fifo);
extern unsigned int __Kfifo_Put( Kfifo *fifo, unsigned char *buffer, unsigned int len );
extern unsigned int __Kfifo_Get( Kfifo *fifo, unsigned char *buffer, unsigned int len );
extern unsigned int Kfifo_Put( Kfifo *fifo, unsigned char *buffer, unsigned int len );
extern unsigned int Kfifo_Get( Kfifo *fifo, unsigned char *buffer, unsigned int len );

extern unsigned int Kfifo_canread_len(Kfifo *fifo);
extern void Kfifo_reset(Kfifo *fifo);
extern void Kfifo_Add_Out(Kfifo *fifo,unsigned int len);

#endif























