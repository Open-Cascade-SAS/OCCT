/*
 Copyright (c) 1998-1999 Matra Datavision
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and / or modify it
 under the terms of the GNU Lesser General Public version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#if !defined( WNT ) && !defined(__hpux) && !defined( HPUX)
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined(HAVE_MALLOC_H)
#include <malloc.h>
#endif

/* Modified by Stephan GARNAUD (ARM) 1992 for Matra Datavision */


static int status ;

/* #ifdef ANSI */
int osd_getkey(char *) ;
int create_sharedmemory (int **,char *,int);
int open_sharedmemory   (int **,char *,int);
int remove_sharedmemory (int * ,char *    );
/* #endif */


int create_sharedmemory(int **section, char *section_name, int section_size) 
/*===============================

	CREATE a mapping memory section

        returns 0 if failed
        otherwise successfull : the shared memory id 

===============================================================*/
{

        key_t key;
        key = (key_t) osd_getkey (section_name);

	*section = NULL ;
	status = shmget(	key,section_size,0750 + IPC_CREAT) ;

	if( status < 0 ) return 0 ;  /* shmget failed */
	else {
	      *section = (int*)shmat(status,NULL,0) ; /* shmat failed */

	      if( *section == (int*)-1 ) {
		     *section = (int*)malloc(section_size) ;
		     return 0 ;
		}
	}
	return status;
}


int open_sharedmemory(int **section, char *section_name, int section_size) 
/*=============================

	OPEN a mapping memory section
        We suppose that the shared memory segment is already
        created(allocated)
        Returns Value:
             0 : If failed
        otherwise successfull 
================================================================*/
{

        key_t key;
        key = (key_t) osd_getkey (section_name);

	*section = NULL ;
        /* Test if shared memory identified by "k" exists ? */
	status = shmget( key,0,0) ;

	if( status < 0 ) return 0;  /* shmget failed */

        /* Try to attach the shared memory to the current process */
        *section = (int*)shmat(status,NULL,0) ;

	if( *section == (int*)-1 ) return 0; /* shmat failed */

	return status ;
}


int remove_sharedmemory(int *shmid, char *section_name) 
/*===========================================

	CLOSE a mapping memory section

========================================================*/
{
	status = shmctl(*shmid ,IPC_RMID,NULL) ;
        if (status < 0) return 0;
                   else return 1;
}
#else

// to avoid compiler warning on empty file
#include "string.h"

#endif
