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

#if !defined( WNT ) && !defined(__hpux)

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <errno.h>

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

/* #ifdef ANSI */
int create_sharedmemory (int **,char *,int);
int open_sharedmemory   (int **,char *,int);
int remove_sharedmemory (int * ,char *    );
/* #endif */

/* This is a multi-server & multi-client asynchronous mailbox management */


/* The UNIX mail box is based on shared memory 
/
/
/ The messages are send via shared memory .
/ AST like functions (VMS) are simulated with a signal handler using SIGUSR1.
/
/ To manage multiple mail boxes , an internal use shared memory is used
/ to communicate index among table of mail box informations .
/
/
/ Primitives to manage mail boxes :
/  osd_crmbox     Create a mail box
/  osd_opmbox     Open a mail box
/  osd_clmbox     Close a mail box
/  osd_wrmbox     Write into a mail box
/
/  user function needed to receive messages :
/   user_function (int *box_id, char *box_name, char *message_address, int message_length)
/
/
/
/ In following explanations, "special shared memory" designates an internal 
/ data area implemented with shared memory used to send index of mail box 
/ informations table for the signal handler.
/
/ Algorithms :
/
/  To create a mail box      - Find a free entry in mail box infos table
/                            - If first mail box, create special shared memory
/                            - Create a shared memory for messages
/                            - Put PID of creator into shared memory
/                            - Install signal handler for SIGUSR1
/
/
/  To open a mailbox         - Find a free entry in mail box infos table
/                            - Attach shared memory to process
/                            - Get PID of creator from shared memory
/
/
/
/  To close a mail box       - Remove shared memory
/                            - Free entry in mail box infos table
/                            - If last mail box, remove special shared memory
/
/
/  To write in a mail box    - Write message into shared memory using 
/                              following protocol :
/                                 Length Message
/
/                            - Send signal SIGUSR1 to server process
/
/
/  To receive message        - Get mail box identification from special shared
/                              memory.
/                            - Get message with protocol
/                            - Get all informations for user function
/                            - Call user function
/                            - Arm again the signal handler
*/



#define MAX_BOX    256          /* Maximum number of mail boxes */

/* Protocol to communicate PID of server to clients */

#define BEGIN_PROTOCOL 0xAABB
#define END_PROTOCOL   0xCCDD

#define SIZEOFNAME     64      /* length of mailbox name */


/* Mail boxes informations table */

static struct {

  int channel;                  /* Id of shared memory (IPC)   */
  int  size;                    /* Size of data area           */
  int (* user_func) ();         /* User function               */
  char name[SIZEOFNAME];        /* Name of mail box   VMS only */
  int *address;                /* Address of data area        */

} id_table[MAX_BOX +1];         /* Allows up to MAX_BOX mail boxes */
/*  char *address;   */             /* Address of data area        */

static int first_mail_box = 1;  /* Flag to initialize mail boxes */
static int pid;                 /* Pid of server or client       */
static int nb_mail = 0;
static int max_mail = MAX_BOX;

/* For special shared memory only */

/*static char *shared_infos;*/      /* Address of shared memory     */
static int *shared_infos;      /* Address of shared memory     */
static int   shared_shmid;      /* Id (IPC) of shared memory    */




static struct {
  int code1;      /* Beginning of protocol */
  int pid;        /* PID of server         */
  int code2;      /* End of protocol       */
 } protocol;










/*====== Private : ======================================================

  find an entry in the mail_box identification table

  -----------------------------------------------------------------------*/

static int
alloc_entry()
{
 int i;

 /* If first use, initialize id_table */

 if (first_mail_box) {
  memset(id_table,0,sizeof(id_table));
  first_mail_box = 0;

  /* Allocate special shared memory named "XptY"  why not ?! */

  if (! create_sharedmemory(&shared_infos, "XptY", sizeof( id_table ) ))
     max_mail = 1;
  else {
     shared_shmid = open_sharedmemory (&shared_infos, "XptY", sizeof (id_table) );
     if (shared_shmid == 0) max_mail = 1;
    }
 }


 i = 1;
 while ( id_table[i].address != NULL && i < MAX_BOX)
  i++;

 if (i == MAX_BOX-1) return -1;   /* Too many mail boxes opened */
   else return (i);
}




/*========= Private : ===================================================

 Remove special shared memory (internal use) when no more mail boxes

 -----------------------------------------------------------------------*/

static void
keep_clean()
{
 remove_sharedmemory (&shared_shmid,"Xpty");
 first_mail_box = 1;
}





/*========= Private : ===================================================

 Set specific error for Mail boxes
 This allow more detailed error decoding

 -----------------------------------------------------------------------*/

static void
set_err(int number)
{
 errno = number + 256;   /* Set specific error for Mail boxes */
}




/*====== Private : ======================================================

  Put PID of handler process into shared memory
  using a special protocol  AABB pid CCDD

----------------------------------------------------------------------*/


void
put_pid(int boxid)
{
 protocol.code1 = BEGIN_PROTOCOL;
 protocol.pid   = getpid();
 protocol.code2 = END_PROTOCOL;
 memcpy (id_table[boxid].address, &protocol, sizeof(protocol));
}

int
get_pid(int boxid)
{
 memcpy(&protocol, id_table[boxid].address, sizeof(protocol));
 if (protocol.code1 != BEGIN_PROTOCOL) return(-1);
 if (protocol.pid   <= 2) return(-2);
 if (protocol.code2 != END_PROTOCOL) return(-3);

 pid = protocol.pid;

 return(0);
}




/*====== Private : ======================================================

  Mail box handler

 This simulate a VMS AST function :
   Asynchronous function

----------------------------------------------------------------------*/

void
handler(int sig)
{         
 int boxid;
 int length;
/* char * address;*/
 int * address;
 char boxname [65];

 memcpy (boxname, shared_infos, SIZEOFNAME); /* Get name of mailbox */

 for (boxid=1; boxid <= MAX_BOX; boxid++)
     if (strcmp(boxname,id_table[boxid].name) == 0) break;

 if (boxid > MAX_BOX) return ; /* ****** What could we do ? ***** */
 
 address = id_table[boxid].address;

 address += sizeof(protocol);            /* Pass the protocol     JPT */

 memcpy(&length, address, sizeof(int));  /* Restore length of message */

 address += sizeof(int);                 /* Adjust pointer to the message */

 /* Call user function */

 /* Don't forget to give again PID 
    of handler process in case of multi-clients */


/* call user-function */
 
(*id_table[boxid].user_func) (&boxid,
                                id_table[boxid].name,
                                address,
                                length
                               );

 /* Re-arm handler */

  signal (SIGUSR1, handler); 


/* don't forget to give again PID of handler process in case of multi-clients
 */ 
 put_pid(boxid); 

}



/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
/
/                          P U B L I C       functions
/
/@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/



/*====================== Create a mail box =========================

  Returns 0 if failed
          mail box identification if succeeded

  ================================================================*/

int
create_mailbox(char *box_name, int box_size,
               int (* async_func) (int *box_id, char *box_name,
               char *message_address, int message_length) )
{
/* int status;*/
 int index;
 int shmid;
 

 /* Test function security */

 if (async_func == NULL) {
  set_err (EFAULT);
  return (0);  /* Verifies function exists */
 }

 if (box_size == 0){
  set_err (EINVAL);
  return (0);
 }

 if (box_name == NULL) {
  set_err (EFAULT);
  return (0);
 }

 index = alloc_entry();               /* Find an entry for id_table */
 if (index == -1) {
  set_err(EMFILE);
  keep_clean();
  return(0);
 }

 if (max_mail == 1 && index > 0) {  /* If only one mail box authorized */
  set_err (EMFILE);
  return(0);
 }

 /* Create shared memory for the process */

 shmid = create_sharedmemory ( &id_table[index].address, box_name, box_size);
 if (shmid == 0) return (0);


 put_pid (index);    /* Put pid of server into shared memory  */

 id_table[index].channel = shmid;              /* Keep id of shared memory */
 id_table[index].size = box_size;              /* Keep size of mail box */
 strncpy(id_table[index].name,box_name,SIZEOFNAME);    /* Keep name of mail box */
 id_table[index].user_func = async_func;       /* Keep user function */


 /* Install asynchronous function : AST function */

  signal (SIGUSR1,  handler); 

 nb_mail++;          
 return (index);
}



/*====================== Open a mail box =======================

  Returns 0 if failed
          mail box identification if succeeded

  ================================================================*/

int
open_mailbox(char * box_name, int box_size)
{
 int status;
 int index;    /* Index for mail box informations access */

 /* Test function security */

 if (box_size == 0){
  set_err (EINVAL);
  return (0);
 }

 if (box_name == NULL) {
  set_err (EFAULT);
  return (0);
 }

 index = alloc_entry();               /* Find an entry for id_table */
 if (index == -1) {
  set_err(EMFILE);
  if (nb_mail == 0) keep_clean();
  return(0);
 }

 id_table[index].size = box_size;              /* Keep size of mail box */
 strncpy(id_table[index].name,box_name,SIZEOFNAME);    /* Keep name of mail box */

 /* Attach shared memory to the process */

 status = open_sharedmemory ( (int **)&id_table[index].address, box_name,
                       box_size);

 if (status !=0)

 if (get_pid (index) < 0){  /* Get pid from shared memory  */
   set_err(ESRCH);
   return (0);
 }
 

 id_table[index].channel = status;

 if (status != 0) {
  return (index);    
 } 
 else {               /* Error */
  id_table[index].address = NULL;    /* ensure pointer is empty */
  keep_clean();
  return(0);
 }
}





/*====================== Close a mail box =======================*/

int
remove_mailbox(int *boxid, char *box_name)
{
 if (boxid == 0){
  set_err(EINVAL);
  return (0);
 }

 if (box_name == NULL) {
  set_err(EFAULT);
  return (0);
 }


/*  (*boxid)--; JPT */

 nb_mail--;

 /* If last mail box removed, remove special shared memory */

 if (nb_mail == 0) keep_clean(); 

 remove_sharedmemory (&id_table[*boxid].channel, box_name );  /* Close shared memory */
 id_table[*boxid].address = NULL;

 return (1);
}



/*====================== Write into a mail box =======================*/

int
write_mailbox(int *boxid, char *box_name, char *message, int length)
{
 int status;
/* char * address;*/         /* Used for protocol  :  length   message */
 int * address;         /* Used for protocol  :  length   message */
 int  interm_length;     /* Used to copy length into shared memory */

 if (*boxid == 0){
  set_err(EBADF);
  return (0);
 }

 if (message == NULL) {
  set_err(EFAULT);
  return (0);
 }

 /* (*boxid)--; JPT */

 address = id_table[*boxid].address;

 address += sizeof(protocol); /* After the protocol JPT */

 interm_length = length; /* Use an intermediate variable for memcpy transfert */

 memcpy(address, &interm_length, sizeof(int));  /* Put length of message */
 address += sizeof(int);                        /* Adjust address for message */

 memcpy(address, message, length+1); /* Put message */

 memcpy(shared_infos, id_table[*boxid].name, SIZEOFNAME ); /* Give index in table infos */

 status = kill (pid, SIGUSR1);   /* Send signal to server */
 
  if (status == 0) return (1);
  else {
   set_err(errno);
   return (0);
  }
}
#else

// to avoid compiler warning on empty file
#include "string.h"

#endif
