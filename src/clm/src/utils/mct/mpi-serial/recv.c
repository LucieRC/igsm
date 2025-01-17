
#include "mpiP.h"



/* 
 * RECEIVING
 *
 */



static int clmmpi_match_send(void *r, void *tag)
{
  return( *((int *)tag) == CLMMPI_ANY_TAG ||
	  *((int *)tag) == ((Req *)r)->tag );
}



/*
 *
 */



FORT_NAME( clmmpi_irecv , CLMMPI_IRECV )(void *buf, int *count, int *datatype,
				   int *source, int *tag, int *comm,
				   int *request, int *ierror)
{

  *ierror=CLMMPI_Irecv(buf,*count,*datatype,*source,*tag,
		    *comm, (void *)request);

}



int CLMMPI_Irecv(void *buf, int count, CLMMPI_Datatype datatype,
	      int source, int tag, CLMMPI_Comm comm, CLMMPI_Request *request)

{
  pListitem match;
  Comm *mycomm;
  Req *rreq, *sreq;

  mycomm=clmmpi_handle_to_ptr(comm);         /* mycomm=(Comm *)comm; */

#ifdef INFO
  fflush(stdout);
  fprintf(stderr,"MPI_Irecv: Comm=%d  tag=%d  count=%d type=%d\n",
	 mycomm->num,tag,count,datatype);
#endif


  if (source!=0 && source!=CLMMPI_ANY_SOURCE)
    {
      fprintf(stderr,"MPI_Irecv: bad source %d\n",source);
      abort();
    }

  clmmpi_alloc_handle(request,(void **)&rreq);


  if ( match=AP_list_search_func(mycomm->sendlist,clmmpi_match_send,&tag) )
    {
      sreq=(Req *)AP_listitem_data(match);
      AP_list_delete_item(mycomm->sendlist,match);

      memcpy(buf,sreq->buf,count * datatype);
      rreq->complete=1;
      rreq->tag=sreq->tag;                   /* in case tag was CLMMPI_ANY_TAG */

      sreq->complete=1;

#ifdef DEBUG
      printf("Completion(recv) value=%d tag=%d\n",
	     *((int *)buf),rreq->tag);
#endif

      return(CLMMPI_SUCCESS);
    }

  rreq->buf=buf;
  rreq->tag=tag;
  rreq->complete=0;
  rreq->listitem=AP_list_append(mycomm->recvlist,rreq);

#ifdef INFO
  print_list(mycomm->recvlist,"recvlist for comm ",mycomm->num);
#endif

  return(CLMMPI_SUCCESS);
}


/*********/


FORT_NAME( clmmpi_recv , CLMMPI_RECV )(void *buf, int *count, int *datatype,
				 int *source, int *tag, int *comm,
				 int *status, int *ierror)
{
  *ierror=CLMMPI_Recv(buf,*count,*datatype,*source,*tag,*comm,
		   (CLMMPI_Status *)status);
}



int CLMMPI_Recv(void *buf, int count, CLMMPI_Datatype datatype, int source,
             int tag, CLMMPI_Comm comm, CLMMPI_Status *status)
{
  CLMMPI_Request request;

#ifdef INFO
  fflush(stdout);
  fprintf(stderr,"MPI_Recv: ");
#endif


  CLMMPI_Irecv(buf,count,datatype,source,tag,comm,&request);
  CLMMPI_Wait(&request,status);


  return(CLMMPI_SUCCESS);
}



#ifdef INFO

int print_item(void *item, void *data)
{
  fprintf(stderr,"%d  ", ((Req *)item)->tag);
  return(0);
}


int print_list(pList list, char *msg, int num)
{
  fflush(stdout);
  fprintf(stderr,"%s %d: ",msg,num);

  AP_list_apply(list,print_item,NULL);

  fprintf(stderr,"\n");
  return(0);
}


#endif
