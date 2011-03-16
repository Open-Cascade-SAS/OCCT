#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef WNT

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <Standard_ProgramError.hxx>
#include <OSD_OSDError.hxx>
#include <Standard_NullObject.hxx>
#include <OSD_WhoAmI.hxx>
#include <OSD_FileNode.ixx>

#if!defined(TM_IN_SYS_TIME) && defined(HAVE_TIME_H)
# include <time.h>
#elif HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>  // for "stat"
#endif

#include <stdlib.h>    // For "system"
#include <errno.h>
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdio.h>

#ifdef HAVE_OSFCN_H
# include <osfcn.h>
#endif


const OSD_WhoAmI Iam = OSD_WFileNode;


// Create a file/directory object

OSD_FileNode::OSD_FileNode (){
 myFileChannel = -1;
}



// Create and initialize a file/directory object

OSD_FileNode::OSD_FileNode (const OSD_Path& Name){
 SetPath (Name);
}



// Get values of object

void OSD_FileNode::Path (OSD_Path& Name)const{
 Name = myPath;
}




// Set values of object

void OSD_FileNode::SetPath (const OSD_Path& Name){
 myError.Reset();
 myPath = Name;
}




// Test if specified file/directory exists
 
Standard_Boolean  OSD_FileNode::Exists(){
int status;


// if (myPath.Name().Length()==0)  A directory can have a null name field (ex: root)
//  OSD_OSDError::Raise("OSD_FileNode::Exists : no name was given"); (LD)

// if (Failed()) Perror();

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status = access ( aBuffer.ToCString() , F_OK );
 
 if (status == 0) return (Standard_True);
   else return ( Standard_False );
}




// Physically remove a file/directory

void  OSD_FileNode::Remove(){

// if (myPath.Name().Length()==0) A directory can have a null name field (ex: root)
//  OSD_OSDError::Raise("OSD_FileNode::Remove : no name was given"); (LD)

// if (Failed()) Perror();

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );

 if(access(aBuffer.ToCString(), W_OK))
   {
     myError.SetValue (errno, Iam, "Remove");
     return;
   }

 struct stat  stat_buf;

 if(stat(aBuffer.ToCString(), &stat_buf))
   {
     myError.SetValue (errno, Iam, "Remove");
     return;
   }
  
 if (  S_ISDIR(stat_buf.st_mode))  {
   // DIRECTORY

   if(rmdir(aBuffer.ToCString()))
     {
       myError.SetValue (errno, Iam, "Remove");
       return;
     }
   return; 

 }
 else if  (  S_ISREG(stat_buf.st_mode) || S_ISLNK(stat_buf.st_mode) ||
             S_ISFIFO(stat_buf.st_mode)   )  { 
   
   if (unlink ( aBuffer.ToCString()) == -1) 
     myError.SetValue (errno, Iam, "Remove");
   return;
 }
 myError.SetValue (EINVAL, Iam, "Remove");
 return;
}




// Move a file/directory to another path

void  OSD_FileNode::Move(const OSD_Path& NewPath){
int status;
TCollection_AsciiString thisPath;

// if (myPath.Name().Length()==0)
//  OSD_OSDError::Raise("OSD_FileNode::Move : no name was given");

// if (Failed()) Perror();

 NewPath.SystemName( thisPath );        // Get internal path name
 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status = rename (aBuffer.ToCString(), thisPath.ToCString());

 if (status == -1) myError.SetValue (errno, Iam, "Move");
}





// Copy a file to another path and name


#ifndef WNT

int static copy_file( const char* src, const char* trg )
{
  int err=0;
  errno=0;
  int fds = open( src, O_RDONLY );
  if ( fds <0 )
    return errno;

  int fdo = open( trg, O_WRONLY|O_TRUNC| O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
  if ( fdo <0 )
  {
    err = errno;
    close( fds );
    return err;
  }

  const int BUFSIZE=4096;
  char buf[BUFSIZE];
  int n=0;
  while ( ( n = read ( fds, buf, BUFSIZE )) >0 )
  {
    if ( write ( fdo, buf, n ) != n ) { // writing error
      if ( ! errno )
        errno = ENOSPC;
      break;
    }
  }

  err=errno;
  close( fdo );
  if (!err) err=errno;
  close( fds );
  if (!err) err=errno;
  return err;
}

#endif




void  OSD_FileNode::Copy(const OSD_Path& ToPath)
{
int status;
TCollection_AsciiString second_name;

// if (myPath.Name().Length()==0)   Copy .login would raise !!
//  OSD_OSDError::Raise("OSD_FileNode::Copy : no name was given");
// if (Failed()) Perror();

 ToPath.SystemName (second_name);

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status =  copy_file(aBuffer.ToCString(), second_name.ToCString());
 if (status != 0) myError.SetValue (-1, Iam, "Copy failed") ;// (LD)
#ifdef DEBUG
 printf("Status %d : errno # %d\n",status,errno);
#endif
}





// Get protections of a file/directory

OSD_Protection  OSD_FileNode::Protection(){
OSD_Protection thisProt;
struct stat myStat;
int status;
int s,u,g,w;

// if (myPath.Name().Length()==0)
//  OSD_OSDError::Raise("OSD_FileNode::Protection : no name was given");

// if (Failed()) Perror();

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status = stat(aBuffer.ToCString(), &myStat);
 if (status == -1) myError.SetValue (errno, Iam, "Protection");

 u = g = w = OSD_None;

 if (myStat.st_mode & S_IRUSR)  u |= OSD_R;
 if (myStat.st_mode & S_IWUSR)  u |= OSD_W;
 if (myStat.st_mode & S_IXUSR)  u |= OSD_X;

 if (myStat.st_mode & S_IRGRP)  g |= OSD_R;
 if (myStat.st_mode & S_IWGRP)  g |= OSD_W;
 if (myStat.st_mode & S_IXGRP)  g |= OSD_X;

 if (myStat.st_mode & S_IROTH)  w |= OSD_R;
 if (myStat.st_mode & S_IWOTH)  w |= OSD_W;
 if (myStat.st_mode & S_IXOTH)  w |= OSD_X;

 s = g;
 thisProt.SetValues ((OSD_SingleProtection)s,
                     (OSD_SingleProtection)u,
                     (OSD_SingleProtection)g,
                     (OSD_SingleProtection)w);

 return (thisProt);
}


// Set protections of a file/directory

void  OSD_FileNode::SetProtection(const OSD_Protection& Prot){
int status;

//  if (myPath.Name().Length()==0)
//  OSD_OSDError::Raise("OSD_FileNode::SetProtection : no name was given");

// if (Failed()) Perror();

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status = chmod (aBuffer.ToCString(), (mode_t)Prot.Internal() );
 if (status == -1) myError.SetValue (errno, Iam, "SetProtection");
}



// Returns User Id

Standard_Integer OSD_FileNode::UserId(){
struct stat buffer;

// if (myPath.Name().Length()==0)
//  OSD_OSDError::Raise("OSD_FileNode::UserId : no name was given");

// if (Failed()) Perror();

 /* Get File Informations */

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 stat ( aBuffer.ToCString(), &buffer );

 return ( buffer.st_uid );
}


// Returns Group Id

Standard_Integer OSD_FileNode::GroupId(){
struct stat buffer;

// if (myPath.Name().Length()==0)
//  OSD_OSDError::Raise("OSD_FileNode::GroupId : no name was given");

// if (Failed()) Perror();

 /* Get File Informations */

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 stat ( aBuffer.ToCString(), &buffer );

 return ( buffer.st_gid );
}




// return the date of last access of file/directory

Quantity_Date  OSD_FileNode::CreationMoment(){

 Quantity_Date result;
 struct tm *decode;
 struct stat buffer;

// if (myPath.Name().Length()==0)
//  OSD_OSDError::Raise("OSD_FileNode::CreationMoment : no name was given");

// if (Failed()) Perror();

 /* Get File Informations */

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 if (!stat ( aBuffer.ToCString(), &buffer )) {
   decode = localtime(&buffer.st_ctime);
   result.SetValues ( 
		     decode->tm_mon+1, decode->tm_mday, decode->tm_year+1900,
		     decode->tm_hour, decode->tm_min, decode->tm_sec , 0,0);
 }
 else
   result.SetValues (1, 1, 1979, 0, 0, 0, 0, 0) ;
 return (result);
}

// return Last access of file/directory

Quantity_Date  OSD_FileNode::AccessMoment(){

 Quantity_Date result;
 struct tm *decode;
 struct stat buffer;

// if (myPath.Name().Length()==0)
//  OSD_OSDError::Raise("OSD_FileNode::AccessMoment : no name was given");

// if (Failed()) Perror();

 /* Get File Informations */

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 if (!stat ( aBuffer.ToCString(), &buffer )) {
   decode = localtime(&buffer.st_atime);
   result.SetValues (
		     decode->tm_mon+1, decode->tm_mday, decode->tm_year+1900,
		     decode->tm_hour, decode->tm_min, decode->tm_sec, 0,0 );
 }
 else
   result.SetValues (1, 1, 1979, 0, 0, 0, 0, 0) ;
 return (result);
}


void OSD_FileNode::Reset(){
 myError.Reset();
}

Standard_Boolean OSD_FileNode::Failed()const{
 return( myError.Failed());
}

void OSD_FileNode::Perror() {
 myError.Perror();
}


Standard_Integer OSD_FileNode::Error()const{
 return( myError.Error());
}

#else

//----------------------------------------------------------------------------
//-------------------  WNT Sources of OSD_FileNode ---------------------------
//----------------------------------------------------------------------------

#define STRICT
#include <OSD_FileNode.hxx>
#include <OSD_Protection.hxx>
#include <Quantity_Date.hxx>
#include <Standard_ProgramError.hxx>

#include <OSD_WNT_1.hxx>

#ifndef _INC_TCHAR
# include <tchar.h>
#endif  // _INC_TCHAR

#define TEST_RAISE( arg ) _test_raise (  fName, ( arg )  )
#define RAISE( arg ) Standard_ProgramError :: Raise (  ( arg )  )

PSECURITY_DESCRIPTOR __fastcall _osd_wnt_protection_to_sd ( const OSD_Protection&, BOOL, char* = NULL );
BOOL                 __fastcall _osd_wnt_sd_to_protection (
                                 PSECURITY_DESCRIPTOR pSD, OSD_Protection& prot, BOOL
                                );
Standard_Integer     __fastcall _get_file_type ( Standard_CString, Standard_Integer );

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

static BOOL __fastcall _get_file_time ( Standard_CString, LPSYSTEMTIME, BOOL );
static void __fastcall _test_raise ( TCollection_AsciiString, Standard_CString );

//=======================================================================
//function : OSD_FileNode
//purpose  : Empty Constructor
//=======================================================================

OSD_FileNode::OSD_FileNode () {

 myFileChannel = ( Standard_Integer )INVALID_HANDLE_VALUE;

}  // end constructor ( 1 )

//=======================================================================
//function : OSD_FileNode
//purpose  : Constructor
//=======================================================================

OSD_FileNode::OSD_FileNode ( const OSD_Path& Name ) {

 myFileChannel = ( Standard_Integer )INVALID_HANDLE_VALUE;
 myPath        = Name;

}  // end constructor ( 2 )

//=======================================================================
//function : Path
//purpose  : 
//=======================================================================

void OSD_FileNode::Path ( OSD_Path& Name ) const {

 Name = myPath;

}  // end OSD_FileNode :: Path

//=======================================================================
//function : SetPath
//purpose  : 
//=======================================================================

void OSD_FileNode::SetPath ( const OSD_Path& Name ) {

 myPath = Name;

}  // end OSD_FileNode :: SetPath

//=======================================================================
//function : Exists
//purpose  : 
//=======================================================================

Standard_Boolean OSD_FileNode::Exists () {

 Standard_Boolean        retVal = Standard_False;;
 TCollection_AsciiString fName;

 myPath.SystemName ( fName );

 if (  fName.IsEmpty ()  ) return Standard_False;
 TEST_RAISE(  TEXT( "Exists" )  );

 if (  GetFileAttributes (  fName.ToCString ()  ) == 0xFFFFFFFF  ) {
 
  if (  GetLastError () != ERROR_FILE_NOT_FOUND  )

   _osd_wnt_set_error (  myError, OSD_WFileNode, fName.ToCString ()  );
 
 } else

  retVal = Standard_True;

 return retVal;

}  // end OSD_FileNode :: Exists

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void OSD_FileNode::Remove () {

 TCollection_AsciiString fName;

 myPath.SystemName ( fName );

 TEST_RAISE(  TEXT( "Remove" )  );

 switch (  _get_file_type (  fName.ToCString (),
                             ( Standard_Integer )INVALID_HANDLE_VALUE )  ) {

  case FLAG_FILE:

   if (   !DeleteFile (  fName.ToCString ()  )   )

    _osd_wnt_set_error (  myError, OSD_WFileNode,  fName.ToCString ()  );

  break;

  case FLAG_DIRECTORY:


// LD : Suppression de l'appel a DeleteDirectory pour 
//      ne pas detruire un repertoire no vide.

   if (   !RemoveDirectory (  fName.ToCString ()  )   )

    _osd_wnt_set_error (  myError, OSD_WFileNode, fName.ToCString ()  );

  break;

  default:

   RAISE(  TEXT( "OSD_FileNode :: Remove ():"
                 " invalid file type - neither file nor directory" )  );

 }  // end switch

}  // end OSD_FileNode :: Remove

//=======================================================================
//function : Move
//purpose  : 
//=======================================================================

void OSD_FileNode::Move ( const OSD_Path& NewPath ) {

 TCollection_AsciiString fName;
 TCollection_AsciiString fNameDst;

 myPath.SystemName  ( fName );

 TEST_RAISE(  TEXT( "Move" )  );

 NewPath.SystemName ( fNameDst );

 switch (  _get_file_type ( fName.ToCString (),
                            ( Standard_Integer )INVALID_HANDLE_VALUE )  ) {

  case FLAG_FILE:

   if (!MoveFileEx (fName.ToCString (), fNameDst.ToCString (),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED
                    )
       )
 
     _osd_wnt_set_error ( myError, OSD_WFileNode,
                          fName.ToCString (), fNameDst.ToCString ()  );
  break;

  case FLAG_DIRECTORY:

   if (   !MoveDirectory (
            fName.ToCString (), fNameDst.ToCString ()
           )
   )
 
    _osd_wnt_set_error ( myError, OSD_WFileNode,
                         fName.ToCString (), fNameDst.ToCString ()  );

  break;

  default:

   RAISE(  TEXT( "OSD_FileNode :: Move (): "
                 "invalid file type - neither file nor directory" )  );

 }  // end switch

}  // end OSD_FileNode :: Move

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

void OSD_FileNode::Copy ( const OSD_Path& ToPath ) {

 TCollection_AsciiString fName;
 TCollection_AsciiString fNameDst;

 myPath.SystemName ( fName );

 TEST_RAISE(  TEXT( "Copy" )  );

 ToPath.SystemName ( fNameDst );

 switch (  _get_file_type ( fName.ToCString (),
                            ( Standard_Integer )INVALID_HANDLE_VALUE )  ) {

  case FLAG_FILE:

   if (!CopyFile (fName.ToCString (), fNameDst.ToCString (), FALSE ))
     _osd_wnt_set_error (myError, OSD_WFileNode,
                         fName.ToCString (), fNameDst.ToCString ());
  break;

  case FLAG_DIRECTORY:

   if (   !CopyDirectory (
            fName.ToCString (), fNameDst.ToCString ()
          )
   )
 
    _osd_wnt_set_error (
     myError, OSD_WFileNode, fName.ToCString (), fNameDst.ToCString ()
    );

  break;

  default:

   RAISE(  TEXT( "OSD_FileNode :: Copy ():"
                 " invalid file type - neither file nor directory" )  );

 }  // end switch

}  // end OSD_FileNode :: Copy

//=======================================================================
//function : Protection
//purpose  : 
//=======================================================================

OSD_Protection OSD_FileNode::Protection () {

 OSD_Protection          retVal;
 TCollection_AsciiString fName;
 PSECURITY_DESCRIPTOR    pSD;

 myPath.SystemName ( fName );

 TEST_RAISE(  TEXT( "Protection" )  );

 if (   (  pSD = GetFileSecurityEx (
                  fName.ToCString (), DACL_SECURITY_INFORMATION |
                  OWNER_SECURITY_INFORMATION
                 )
        ) == NULL ||
        !_osd_wnt_sd_to_protection (
          pSD, retVal,
          _get_file_type (  fName.ToCString (), ( Standard_Integer )INVALID_HANDLE_VALUE  ) ==
          FLAG_DIRECTORY
         )
 )
   
   _osd_wnt_set_error ( myError, OSD_WFileNode );

 if ( pSD != NULL )

  FreeFileSecurity ( pSD );

 return retVal;

}  // end OSD_FileNode :: Protection

//=======================================================================
//function : SetProtection
//purpose  : 
//=======================================================================

void OSD_FileNode::SetProtection ( const OSD_Protection& Prot ) {

 TCollection_AsciiString fName;
 PSECURITY_DESCRIPTOR    pSD;

 myPath.SystemName ( fName );

 TEST_RAISE(  TEXT( "SetProtection" )  );

 pSD = _osd_wnt_protection_to_sd (
        Prot,
        _get_file_type ( fName.ToCString (),
                         ( Standard_Integer )INVALID_HANDLE_VALUE  ) ==
        FLAG_DIRECTORY,
        (char *)fName.ToCString ()
       );
 
 if ( pSD == NULL || !SetFileSecurity (
                       fName.ToCString (), DACL_SECURITY_INFORMATION, pSD
                      )
 )
  
  _osd_wnt_set_error (  myError, OSD_WFileNode, fName.ToCString ()  );

 if ( pSD != NULL )

  FreeSD ( pSD );

}  // end OSD_FileNode :: SetProtection

//=======================================================================
//function : AccessMoment
//purpose  : 
//=======================================================================

Quantity_Date OSD_FileNode::AccessMoment () {

 Quantity_Date           retVal;
 SYSTEMTIME              stAccessMoment;
 SYSTEMTIME              stAccessSystemMoment;
 TCollection_AsciiString fName;

 myPath.SystemName ( fName );

 TEST_RAISE(  TEXT( "AccessMoment" )  );

// if (   _get_file_time (  fName.ToCString (), &stAccessMoment, TRUE  )   )
 if (   _get_file_time (  fName.ToCString (), &stAccessSystemMoment, TRUE  ) )
//POP
{
  SYSTEMTIME * aSysTime = &stAccessMoment;
  BOOL aFlag = SystemTimeToTzSpecificLocalTime (NULL ,
                                                &stAccessSystemMoment ,
                                                &stAccessMoment);
  if (aFlag == 0) // AGV: test for success (e.g., unsupported on Win95/98)
    aSysTime = &stAccessSystemMoment;
  retVal.SetValues (aSysTime->wMonth,       aSysTime->wDay,
                    aSysTime->wYear,        aSysTime->wHour,
                    aSysTime->wMinute,      aSysTime->wSecond,
                    aSysTime->wMilliseconds
                    );
}
 else

  _osd_wnt_set_error (  myError, OSD_WFileNode, fName.ToCString ()  );

 return retVal;

}  // end OSD_FileNode :: AccessMoment

//=======================================================================
//function : CreationMoment
//purpose  : 
//=======================================================================

Quantity_Date OSD_FileNode::CreationMoment () {

 Quantity_Date           retVal;
 SYSTEMTIME              stCreationMoment;
 SYSTEMTIME              stCreationSystemMoment;
 TCollection_AsciiString fName;

 myPath.SystemName ( fName );

 TEST_RAISE(  TEXT( "CreationMoment" )  );

// if (   _get_file_time (  fName.ToCString (), &stCreationMoment, FALSE  )   )
 if ( _get_file_time ( fName.ToCString (), &stCreationSystemMoment, TRUE  )   ) 
//POP 
{
  SYSTEMTIME * aSysTime = &stCreationMoment;
  BOOL aFlag = SystemTimeToTzSpecificLocalTime (NULL,
                                                &stCreationSystemMoment ,
                                                &stCreationMoment);
  if (aFlag == 0) // AGV: test for success (e.g., unsupported on Win95/98)
    aSysTime = &stCreationSystemMoment;
  retVal.SetValues (aSysTime->wMonth,       aSysTime->wDay,
                    aSysTime->wYear,        aSysTime->wHour,
                    aSysTime->wMinute,      aSysTime->wSecond,
                    aSysTime->wMilliseconds
                    );
}
 else

  _osd_wnt_set_error (  myError, OSD_WFileNode, fName.ToCString ()  );

 return retVal;

}  // end OSD_FileNode :: CreationMoment

//=======================================================================
//function : UserId
//purpose  : 
//=======================================================================

Standard_Integer OSD_FileNode::UserId () {

 PSID                    pSIDowner = NULL;
 PSID                    retVal;
 BOOL                    fDefaulted;
 TCollection_AsciiString fName;
 PSECURITY_DESCRIPTOR    pSD;

 myPath.SystemName ( fName );

 TEST_RAISE(  TEXT( "UserId" )  );

 if (   (  pSD = GetFileSecurityEx (
                  fName.ToCString (),
                  OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION
                 )
        ) != NULL                                                   &&
        GetSecurityDescriptorOwner ( pSD, &pSIDowner, &fDefaulted ) &&
        pSIDowner != NULL
 )
 
  retVal = CopySidEx ( pSIDowner );

 else

  _osd_wnt_set_error ( myError, OSD_WFileNode );

 if ( pSD != NULL )

  FreeFileSecurity ( pSD );
 
 return ( Standard_Integer )retVal;

}  // end OSD_FileNode :: UserId

//=======================================================================
//function : GroupId
//purpose  : 
//=======================================================================

Standard_Integer OSD_FileNode::GroupId () {

 PGROUP_SID              retVal = NULL;
 TCollection_AsciiString fName;
 PSECURITY_DESCRIPTOR    pSD;

 myPath.SystemName ( fName );

 TEST_RAISE(  TEXT( "GroupId" )  );

 if (   (  pSD = GetFileSecurityEx (
                  fName.ToCString (),
                  OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION
                 )
        ) != NULL
 ) {

  retVal = AllocGroupSid ( pSD );
  FreeFileSecurity ( pSD );

 } else

  _osd_wnt_set_error ( myError, OSD_WFileNode );

 return ( Standard_Integer )retVal;

}  // end OSD_FileNode :: GroupId

//=======================================================================
//function : Failed
//purpose  : 
//=======================================================================

Standard_Boolean OSD_FileNode::Failed () const {

 return myError.Failed ();

}  // end OSD_FileNode :: Failed

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void OSD_FileNode::Reset () {

 myError.Reset ();

}  // end OSD_FileNode :: Reset

//=======================================================================
//function : Perror
//purpose  : 
//=======================================================================

void OSD_FileNode::Perror () {

 myError.Perror ();

}  // end OSD_FileNode :: Perror

//=======================================================================
//function : Error
//purpose  : 
//=======================================================================

Standard_Integer OSD_FileNode::Error () const {

 return myError.Error ();

}  // end OSD_FileNode :: Error

void _osd_wnt_set_error ( OSD_Error& err, OSD_WhoAmI who, ... ) {

 DWORD              errCode;
 Standard_Character buffer[ 2048 ];
 va_list            arg_ptr;

 va_start ( arg_ptr, err );

 errCode = GetLastError ();

 if (  !FormatMessage (
         FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
         0, errCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ),
         buffer, 2048, &arg_ptr
        )
 ) {
 
  sprintf ( buffer, "error code %d", (Standard_Integer)errCode );
  SetLastError ( errCode );

 }  // end if

 err.SetValue ( errCode, who, buffer );

 va_end ( arg_ptr );

}  // end _set_error

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return retVal
#endif

static BOOL __fastcall _get_file_time (
                        Standard_CString fName, LPSYSTEMTIME lpSysTime, BOOL fAccess
                       ) {

 BOOL       retVal = FALSE;
 FILETIME   ftCreationTime;
 FILETIME   ftLastWriteTime;
 LPFILETIME lpftPtr;
 HANDLE     hFile;

 __try {

  if (   (  hFile = CreateFile (
                     fName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
                    )
         ) == INVALID_HANDLE_VALUE
  ) __leave;

  if (  !GetFileTime ( hFile, &ftCreationTime, NULL, &ftLastWriteTime )  ) __leave;

  lpftPtr = fAccess ? &ftLastWriteTime : &ftCreationTime;

  if (  !FileTimeToSystemTime ( lpftPtr, lpSysTime )  ) __leave;

  retVal = TRUE;

 }  // end __try

 __finally {
 
  if ( hFile != INVALID_HANDLE_VALUE )

   CloseHandle ( hFile );
 
 }  // end __finally

#ifdef VAC
leave: ;      // added for VisualAge
#endif

 return retVal;

}  // end _get_file_time

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

static void __fastcall _test_raise ( TCollection_AsciiString fName, Standard_CString str ) {

 Standard_Character buff[ 64 ];

 if (  fName.IsEmpty ()  ) {
 
  _tcscpy (  buff, TEXT( "OSD_FileNode :: " )  );
  _tcscat (  buff, str );
  _tcscat (  buff, TEXT( " (): wrong access" )  );

  Standard_ProgramError :: Raise ( buff );
 
 }  // end if

}  // end _test_raise

#endif
