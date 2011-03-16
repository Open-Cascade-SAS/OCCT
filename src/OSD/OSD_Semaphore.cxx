#ifndef WNT

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Standard_ProgramError.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ConstructionError.hxx>
#include <OSD_Semaphore.ixx>
#include <OSD_WhoAmI.hxx>


const OSD_WhoAmI Iam = OSD_WSemaphore;

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <errno.h>
#include <stdio.h>

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_IPC_H
# include <sys/ipc.h>
#endif

#ifdef HAVE_SYS_SEM_H
# include <sys/sem.h>
#endif

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#ifdef SEMUN_DEFINED
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
  int val;                    /* value for SETVAL */
  struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
  unsigned short int *array;  /* array for GETALL, SETALL */
# ifdef SEMUN_BUF_DEFINED     /* union semun contains four members not three */
  struct seminfo *__buf;      /* buffer for IPC_INFO */
# endif
};
#endif


extern "C" int osd_getkey(char *);

// Remark :   ifdef DS3 means  use of DecStation under ULTRIX

#define CLUSTER_NUMBER 4 

static struct sembuf event_flag ;
	 
OSD_Semaphore::OSD_Semaphore(){
 mySemId = -1;
 event_flag.sem_num = 0;
 event_flag.sem_op  = 0;
 event_flag.sem_flg = 0;
}

// ===============================================================
OSD_Semaphore::OSD_Semaphore(const TCollection_AsciiString& Name) 

// CREATE a semaphore cluster of 32 events =======================
// ===============================================================


{
 mySemId = -1;
 event_flag.sem_num = 0;
 event_flag.sem_op  = 0;
 event_flag.sem_flg = 0;

 if (!Name.IsAscii())
  Standard_ConstructionError::Raise("OSD_Semaphore::OSD_Semaphore : name");

 myName = Name;
 
 Standard_PCharacter pStr;
 //
 pStr=(Standard_PCharacter)Name.ToCString();
 myKey = osd_getkey(pStr);
}




// ===============================================================
void OSD_Semaphore::Build()
// Set semaphore into system =====================================
// ===============================================================
{
 mySemId = semget((key_t) myKey,CLUSTER_NUMBER,0750 + IPC_CREAT) ;
 
 if (mySemId == -1) myError.SetValue(errno, Iam, "OSD_Semaphore::Build");
}



// ===============================================================
void OSD_Semaphore::Open(const TCollection_AsciiString& Name) 
//     OPEN a semaphore cluster ==================================
// ===============================================================

{

 if (!Name.IsAscii())
  Standard_ConstructionError::Raise("OSD_Semaphore::Open");

 myName = Name;
 Standard_PCharacter pStr;
 //
 pStr=(Standard_PCharacter)Name.ToCString();
 myKey = osd_getkey(pStr);

 mySemId = semget((key_t) myKey,0,0) ;

 if (mySemId == -1) myError.SetValue(errno, Iam, "OSD_Semaphore::Open");
}



// ===============================================================
void OSD_Semaphore::Lock() 
//	WAIT & LOCK the semaphore attached to the cluster ========
// ===============================================================


{
 int status ;
 int event=0;

 if (myError.Failed()) myError.Perror();

 if (mySemId == -1)
   Standard_ProgramError::Raise("OSD_Semaphore::Lock : semaphore not created");

 event_flag.sem_num = event ;
 event_flag.sem_op  = -1 ;
 event_flag.sem_flg = SEM_UNDO;

 status = semop((int)mySemId ,&event_flag,1) ;

 if (status == -1) myError.SetValue(errno, Iam, "Lock semaphore");
}



// ===============================================================
void OSD_Semaphore::Free() 
//	FREE the semaphore attached to the cluster ===============
// ===============================================================

{
int status ;

 if (myError.Failed()) myError.Perror();

 if (mySemId == -1)
   Standard_ProgramError::Raise("OSD_Semaphore::Free : semaphore not created");

 event_flag.sem_num = 0 ;
 event_flag.sem_op  = 1 ;
 event_flag.sem_flg = IPC_NOWAIT ;

 status=semop((int)mySemId ,&event_flag,1) ;

 if (status == -1) myError.SetValue(errno, Iam, "OSD_Semaphore::Free");
}



// ===============================================================
void OSD_Semaphore::Restore()
//      RESET semaphore to initial state =========================
// ===============================================================


{
 int status;
 int event=0;
 union semun zero_value;

 if (myError.Failed()) myError.Perror();

 if (mySemId == -1)
   Standard_ProgramError::Raise("OSD_Semaphore::Restore : semaphore not created");

 zero_value.val = 0;

 status = semctl((int)mySemId ,event,SETVAL,zero_value);

 if (status == -1) myError.SetValue(errno, Iam, "OSD_Semaphore::Restore semaphore");
}




// ===============================================================
void OSD_Semaphore::SetCounter(const Standard_Integer Value)
//       Set internal Semaphore counter to desired Value =========
// ===============================================================


{
 int status;
 int event=0;
 union semun param_value;

 if (myError.Failed()) myError.Perror();

 if (mySemId == -1)
   Standard_ProgramError::Raise("OSD_Semaphore::SetCounter : semaphore not created");

 param_value.val = (int)Value;

 status = semctl((int)mySemId ,event,SETVAL,param_value);
 
 if (status == -1) myError.SetValue(errno, Iam, "OSD_Semaphore::SetCounter semaphore");
}



// ===============================================================
Standard_Integer OSD_Semaphore::GetCounter()
//     Get value of semaphore counter ============================
// ===============================================================

{
 int status;
 int event=0;
 int Value; 
 union semun param_value;

 if (myError.Failed()) myError.Perror();

 if (mySemId == -1)
   Standard_ProgramError::Raise("OSD_Semaphore::GetCounter : semaphore not created");

 param_value.array = (ushort *)&Value; 
 status = semctl((int)mySemId ,event,GETVAL,param_value);
  
 if (status == -1)
   myError.SetValue(errno, Iam, "OSD_Semaphore::GetCounter semaphore"); 
 return (Value);
} 



// ===============================================================
void OSD_Semaphore::Delete() 
//          CLOSE semaphore attached to cluster ==================
// ===============================================================

{
 int status;
 union semun param_value;

 if (myError.Failed()) myError.Perror();

 if (mySemId == -1)
   Standard_ProgramError::Raise("OSD_Semaphore::Delete : semaphore not created");

  param_value.array = NULL;
  status = semctl((int)mySemId ,IPC_RMID, 0, param_value);

 if (status == -1) 
   myError.SetValue(errno, Iam, "OSD_Semaphore::Delete semaphore");
 mySemId = -1;
}



// ===============================================================
void OSD_Semaphore::Reset(){
// ===============================================================
 myError.Reset();
}

// ===============================================================
Standard_Boolean OSD_Semaphore::Failed()const{
// ===============================================================
 return( myError.Failed());
}

// ===============================================================
void OSD_Semaphore::Perror() {
// ===============================================================
 myError.Perror();
}


// ===============================================================
Standard_Integer OSD_Semaphore::Error()const{
// ===============================================================
 return( myError.Error());
}
#endif
