#include "xlink_ringbuf.h"


void Kfifo_Init( Kfifo *fifo, unsigned char *buffer, unsigned int size )
{
	if(fifo == NULL){
		return;
	}
	if(buffer == NULL){
		return;
	}
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = 0;
    fifo->out = 0;
    fifo->f_lock = 0;
}


void Kfifo_Lock(Kfifo *fifo){
	int i = 0;
	while(fifo->f_lock) {
		i++;
		if(i >= 50000)break; //prevent dead loop
	}
	fifo->f_lock = 1;
}

void Kfifo_UnLock(Kfifo *fifo){
	fifo->f_lock = 0;
}

unsigned int __Kfifo_Put( Kfifo *fifo, unsigned char *buffer, unsigned int len )
{
    unsigned int l;

    len = min( len, ( fifo->size + fifo->out - fifo->in ) );
    /*
    * Ensure that we sample the fifo->out index -before- we
    * start putting bytes into the kfifo.
    */
    /* first put the data starting from fifo->in to buffer end */
    l = min( len, ( fifo->size - ( fifo->in & ( fifo->size - 1 ) ) ) );

    memcpy( fifo->buffer + ( fifo->in & ( fifo->size - 1 ) ), buffer, l );
    /* then put the rest (if any) at the beginning of the buffer */
    memcpy( fifo->buffer, buffer + l, len - l );
    /*
    * Ensure that we add the bytes to the kfifo -before-
    * we update the fifo->in index.
    */
    fifo->in += len;

    return len;
}





unsigned int __Kfifo_Get( Kfifo *fifo, unsigned char *buffer, unsigned int len )
{
    unsigned int l;

    len = min(len, ( fifo->in - fifo->out));
	  
    /*
     * Ensure that we sample the fifo->in index -before- we
     * start removing bytes from the kfifo.
     */

    /* first get the data from fifo->out until the end of the buffer */
    l = min( len, ( fifo->size - ( fifo->out & ( fifo->size - 1 ) ) ));
    
	memcpy( buffer, fifo->buffer + ( fifo->out & ( fifo->size - 1 ) ), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy( buffer + l, fifo->buffer, len - l );
    /*
     * Ensure that we remove the bytes from the kfifo -before-
     * we update the fifo->out index.
     */
//    fifo->out += len;  //add this code into Kfifo_Add_Out() func
    return len;
}


unsigned int Kfifo_canread_len(Kfifo *fifo)
{
	return (fifo->in - fifo->out);
}


void Kfifo_reset(Kfifo *fifo)
{
	 fifo->in = fifo->out = 0;
}


unsigned int Kfifo_Put( Kfifo *fifo, unsigned char *buffer, unsigned int len ){
    
	  unsigned int ret;
	  
	  ret = __Kfifo_Put(fifo,buffer,len);
	 
	  return ret;
}


/*
 *when fifo->in == fifo->out indicate the the buf
 *have no data,so can reset the fifo->in == fifo->out
 *to 0
 * */
unsigned int Kfifo_Get(Kfifo *fifo, unsigned char *buffer, unsigned int len){
    
	  unsigned int ret;
	   
	  ret = __Kfifo_Get(fifo,buffer,len);

//	  if(fifo->in == fifo->out){
//		  fifo->in = fifo->out = 0;
//	  }
	  return ret;
}


/*******************************************************
 * move the position when have readed out data,
 * use with Kfifo_Get()
 * ****************************************************/
void Kfifo_Add_Out(Kfifo *fifo,unsigned int len)
{
    fifo->out += len;
    /*
     *when fifo->in == fifo->out indicate the the buf
     *have no data,so can reset the fifo->in == fifo->out
     *to 0
     * */
//    if(fifo->in == fifo->out){
//    	Kfifo_reset(fifo);
//    }
}



















