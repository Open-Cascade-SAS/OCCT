/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#ifndef WNT

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <errno.h>
#include <stdio.h>

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif

#ifdef HAVE_SYS_SEM_H
# include <sys/sem.h>
#endif

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

/* Modified by Stephan GARNAUD (ARM) 1992 for Matra Datavision */

#if (!defined (__hpux)) && (!defined (HPUX))

#define CLUSTER_NUMBER 4 
#define SIGUSER SIGUSR1-1

static struct sembuf event_flag ;
	 
static void osd_signal_(int); 
int osd_getkey(char *);
#ifdef ANSI
int create_semaphore(char *);
int open_semaphore(char *);
int lock_semaphore(int *,int *);
int free_semaphore(int *,int *);
int reset_semaphore(int *,int *);
int remove_semaphore(int *,char *);
int osd_signl_(int *);
int osd_skill_(int *,int *);
int osd_alarm_(int,void (*)());

#endif

static void
osd_signal_(int i )
{
}


int
create_semaphore(char *name) 
/*=======================================================

	CREATE a semaphore cluster of 32 events

=========================================================*/
{
int status ;



    key_t key;


        key = (key_t) osd_getkey (name);


	status = semget(key,CLUSTER_NUMBER,0750 + IPC_CREAT) ;

	if( status < 0 ) return (0) ;  /* semget failed */
        return status ;
}


int
open_semaphore(char *name)
/*=========================================================

	OPEN a semaphore cluster 

=========================================================================*/
{
int status ;


    key_t key;


        key = (key_t) osd_getkey (name);
	status = semget(key,0,0) ;

	if( status < 0 ) return (0) ;  /* semget failed */
         else   return status;
}



int
lock_semaphore(int *semid, int *event) 
/*==================================================

	WAIT & LOCK the semaphore attached to the cluster

====================================================*/
{
  int status ;

  event_flag.sem_num = *event ;
  event_flag.sem_op  = -1 ;
  event_flag.sem_flg = SEM_UNDO;
  
  do 
#if defined(SEMOP_NO_REFERENCE) && !defined(DECOSF1) 
    status = semop(*semid,event_flag,1) ;
#else 
    status = semop(*semid,&event_flag,1) ;
#endif
  while ( status < 0 && errno != EINTR );
  
  if( status < 0 )
    return (0) ;
  else
    return (1) ;
}


int
free_semaphore(int *semid, int *event) 
/*==================================================

	FREE the semaphore attached to the cluster

====================================================*/
{
  int status ;
  int zero_value = 0;
  
  event_flag.sem_num = *event ;
  event_flag.sem_op  = 1 ;
  event_flag.sem_flg = IPC_NOWAIT ;

#ifdef SEMCTL_NO_REFERENCE
  status = semctl(*semid,*event,GETVAL,zero_value);
#else 
  status = semctl(*semid,*event,GETVAL,&zero_value);
#endif
  if (status !=1) 
#if defined(SEMOP_NO_REFERENCE) && !defined(DECOSF1)
    status = semop(*semid,event_flag,1) ;
#else 
    status = semop(*semid,&event_flag,1) ;
#endif
  else return 0;

   if( status < 0 )
    return (0) ;
  else
    return (1) ;
}



int
reset_semaphore(int *semid, int *event) 
/*===================================================

      RESET semaphore to initial state

======================================================*/
{
 int status;
 int zero_value=0;

#ifdef SEMCTL_NO_REFERENCE
 status = semctl(*semid,*event,SETVAL,zero_value);
#else
 status = semctl(*semid,*event,SETVAL,&zero_value);
#endif
 if (status < 0) return(0);
            else return(1);

}


int
set_semaphore(int *semid, int *event, int value) 
/*=================================================

      SET semaphore counter to a fixed value

====================================================*/
{
 int status;

#ifdef SEMCTL_NO_REFERENCE
 status = semctl((*semid),*event,SETVAL,value);
#else
 status = semctl((*semid),*event,SETVAL,&value);
#endif
 if (status < 0) return(0);
            else return(1);

}


int
get_semaphore(int *semid, int *event, int *value)
/*=================================================
 
      GET semaphore counter value
 
==========================================================================*/
{
 int status;
 
#ifdef SEMCTL_NO_REFERENCE
 status = semctl((*semid),*event,GETVAL,value);
#else
 status = semctl((*semid),*event,GETVAL,&value);
#endif

 if (status < 0) return(0);
            else return(1);
 
}



int
remove_semaphore(int *semid, char *name) 
/*======================================================

          CLOSE semaphore attached to cluster

=============================================================================*/
{
 int status;

/*  status = semctl(*semid,IPC_RMID,NULL);*/
  status = semctl(*semid,IPC_RMID,0);
  if (status < 0) return 0;
             else return 1;

}


int
osd_signl_(int *event) 
/*====================================================

	ARM the SIGNAL event (Must be 1 or 2 )
	And WAIT after it was KILLED

=======================================================*/
{
	signal(SIGUSER + *event ,  osd_signal_) ;
	pause() ;
	return (1) ;
}

int
osd_skill_(int *pid, int *event) 
/*==================================================

	SEND a SIGNAL to the PROCESS ID specified

 ====================================================*/
{
       if( kill( *pid , SIGUSER  + *event ) < 0 ) return (0) ;
       return (1) ;
}

int
osd_alarm_(int sec,RETSIGTYPE (*_alarm)())
/*====================================================

	ARM the SIGNAL event (Must be 1 or 2 )
	And WAIT after it was KILLED
       
==========================================================================*/
{
  signal(SIGALRM, _alarm) ;
  alarm(sec) ;
  return (1) ;
}

#endif /* __hpux  */

#endif /* WNT  */
