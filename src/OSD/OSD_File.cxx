// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

//------------------------------------------------------------------------
//                    UNIX Part
//------------------------------------------------------------------------


#ifndef WNT

#include <Standard_ProgramError.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_WhoAmI.hxx>
#include <OSD_FromWhere.hxx>
#include <OSD_File.ixx>

#include <Standard_PCharacter.hxx>

const OSD_WhoAmI Iam = OSD_WFile;

#if defined (sun) || defined(SOLARIS)
#define POSIX
#else             
#define SYSV
#endif

#include <errno.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#define NEWLINE '\10';

// ---------------------------------------------------------------------
// Create an empty file object
// ---------------------------------------------------------------------

OSD_File::OSD_File():OSD_FileNode() {
 ImperativeFlag = Standard_False;
 myLock         = OSD_NoLock;
 myIO           = 0;
 myMode         = OSD_ReadWrite;
 myFileChannel  = -1;
 myFILE         = (Standard_Address) NULL;
}

// ---------------------------------------------------------------------
// Create and initialize a file object
// ---------------------------------------------------------------------

OSD_File::OSD_File(const OSD_Path& Name):OSD_FileNode(Name){

 ImperativeFlag = Standard_False;
 myLock         = OSD_NoLock;
 myIO           = 0;
 myMode         = OSD_ReadWrite;
 myFileChannel  = -1;
 myFILE         = (Standard_Address) NULL;
}


// ---------------------------------------------------------------------
// Build a file if it doesn't exist or create again if it already exists
// ---------------------------------------------------------------------

void OSD_File::Build(const OSD_OpenMode Mode,
                     const OSD_Protection& Protect){
 
 Standard_Integer internal_prot;
 Standard_Integer internal_mode = O_CREAT | O_TRUNC ;
 TCollection_AsciiString aBuffer;

 if (myPath.Name().Length()==0)
  Standard_ProgramError::Raise("OSD_File::Build : no name was given");

 if (myFileChannel != -1)
  Standard_ProgramError::Raise("OSD_File::Build : file is already open");


 myMode = Mode;

 internal_prot = Protect.Internal();

 char CMode[5];

 switch (Mode){
  case OSD_ReadOnly:
   internal_mode |= O_RDONLY;
   strcpy (CMode,"r");
   break;
  case OSD_WriteOnly:
   internal_mode |= O_WRONLY;
   strcpy (CMode,"w");
   break;
  case OSD_ReadWrite:
   internal_mode |= O_RDWR;
   strcpy (CMode,"rw");
   break;
 }

 myPath.SystemName( aBuffer ); 
 myFileChannel = open (aBuffer.ToCString(),(int) internal_mode,(int) internal_prot);
 if (myFileChannel >= 0) { 
   myFILE = fdopen (myFileChannel,(const char*) CMode);
 }
 else
 /* Handle OPEN errors */

   myError.SetValue (errno, Iam, "Open");

}



// ---------------------------------------------------------------------
// Append to an existing file
// ---------------------------------------------------------------------

void  OSD_File::Append(const OSD_OpenMode Mode,
                       const OSD_Protection& Protect){

 Standard_Integer internal_prot;
 Standard_Integer internal_mode = O_APPEND;;
 TCollection_AsciiString aBuffer;

 if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) {
    Standard_ProgramError::Raise("OSD_File::Append : it is a directory");
 } 
 
 if (myPath.Name().Length()==0)
  Standard_ProgramError::Raise("OSD_File::Append : no name was given");

 if (myFileChannel != -1)
  Standard_ProgramError::Raise("OSD_File::Append : file is already open");

 internal_prot = Protect.Internal();


 myMode = Mode;
 char CMode[5];

 switch (Mode){
  case OSD_ReadOnly:
   internal_mode |= O_RDONLY;
   strcpy (CMode,"r");
   break;
  case OSD_WriteOnly:
   internal_mode |= O_WRONLY;
   strcpy (CMode,"a");
   break;
  case OSD_ReadWrite:
   internal_mode |= O_RDWR;
   strcpy (CMode,"a");
   break;
 }

 // If file doesn't exist, creates it.

 if (!Exists()) internal_mode |= O_CREAT;

 myPath.SystemName ( aBuffer );
 myFileChannel = open (aBuffer.ToCString(),(int) internal_mode,(int) internal_prot);
 if (myFileChannel >= 0)
   myFILE = fdopen (myFileChannel,(const char*) CMode);
 else
 /* Handle OPEN errors */

   myError.SetValue (errno, Iam, "Open");
}

// ---------------------------------------------------------------------
// Open a file
// ---------------------------------------------------------------------

void  OSD_File::Open(const OSD_OpenMode Mode,
                     const OSD_Protection& Protect){

 Standard_Integer internal_prot;
 Standard_Integer internal_mode = 0;
 TCollection_AsciiString aBuffer;

  if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
    myError.SetValue (1, Iam, "Could not be open : it is a directory");
  }

 if (myPath.Name().Length()==0)
  Standard_ProgramError::Raise("OSD_File::Open : no name was given");

 if (myFileChannel != -1)
  Standard_ProgramError::Raise("OSD_File::Open : file is already open");

 internal_prot = Protect.Internal();


 myMode = Mode;
 char CMode[5];

 switch (Mode){
  case OSD_ReadOnly:
   internal_mode |= O_RDONLY;
   strcpy (CMode,"r");
   break;
  case OSD_WriteOnly:
   internal_mode |= O_WRONLY;
   strcpy (CMode,"w");
   break;
  case OSD_ReadWrite:
   internal_mode |= O_RDWR;
   strcpy (CMode,"rw");
   break;
 }

 myPath.SystemName ( aBuffer );
 myFileChannel = open (aBuffer.ToCString(),(int) internal_mode,(int) internal_prot);
 if (myFileChannel >= 0)
   myFILE = fdopen (myFileChannel,(const char*) CMode);
 else
 /* Handle OPEN errors */

   myError.SetValue (errno, Iam, "Open");
}



// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
OSD_File OSD_File::BuildTemporary(){

#if defined(vax) || defined(__vms) || defined(VAXVMS)
 FILE *fic;
 OSD_File result;
 int dummy;

 fic = tmpfile();
 dummy = open("dummy", O_RDWR | O_CREAT);  // Open a summy file
 result.myFileChannel = dummy - 1;         // This is file channel of "fic" +1
 close(dummy);                             // Close dummy file
 unlink("dummy");                          // Removes dummy file

#else 
 OSD_File result;
 char *name = tmpnam((char*) 0) ;


 TCollection_AsciiString aName ( name ) ;
 OSD_Path aPath( aName ) ;

 result.SetPath( aPath ) ;

 result.myFILE  = fopen( name, "w+" ) ;

 result.myFileChannel = fileno( (FILE*)result.myFILE );

#endif

 result.myMode = OSD_ReadWrite;

 return (result);
}
 


// ---------------------------------------------------------------------
// Read content of a file
// ---------------------------------------------------------------------
 
void  OSD_File::Read(TCollection_AsciiString& Buffer, 
                     const Standard_Integer Nbyte){
 Standard_PCharacter readbuf;
 int status;

  if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
    Standard_ProgramError::Raise("OSD_File::Read : it is a directory");
  }

 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::Read : file is not open");

 if (Failed()) Perror();

 if (myMode == OSD_WriteOnly)
  Standard_ProgramError::Raise("OSD_File::Read : file is Write only");

 if (Nbyte <= 0)
  Standard_ProgramError::Raise("OSD_File::Read : Nbyte is null");

 TCollection_AsciiString transfert(Nbyte,' ');
 readbuf = (Standard_PCharacter)transfert.ToCString();  

 status = read( (int)myFileChannel, readbuf, Nbyte);
 //
 Buffer = transfert;  // Copy transfert to Buffer

 if (status == -1) myError.SetValue (errno, Iam, "Read");
 else
 if ( status < Nbyte ) myIO = EOF;
}


// ---------------------------------------------------------------------
// Read a line from a file
// ---------------------------------------------------------------------

void OSD_File::ReadLine(TCollection_AsciiString& Buffer, 
			const Standard_Integer Nbyte,
			Standard_Integer& NByteRead)
{
  Standard_PCharacter readbuf, abuffer;
  //
  if (OSD_File::KindOfFile() == OSD_DIRECTORY ) { 
    Standard_ProgramError::Raise("OSD_File::Read : it is a directory");
  }
  if (myFileChannel == -1){
    Standard_ProgramError::Raise("OSD_File::ReadLine : file is not open");
  }
  if (Failed()) {
    Perror();
  }
  if (myMode == OSD_WriteOnly) {
    Standard_ProgramError::Raise("OSD_File::ReadLine : file is Write only");
  }
  if (Nbyte <= 0){
    Standard_ProgramError::Raise("OSD_File::ReadLine : Nbyte is null");
  }
  //
  TCollection_AsciiString transfert(Nbyte,' ');
  readbuf = (Standard_PCharacter)transfert.ToCString();
  //
  abuffer = fgets(readbuf, Nbyte, (FILE *) myFILE);
  //
  if (abuffer == NULL) {
    if (!feof((FILE *) myFILE)) {
      myError.SetValue (errno, Iam, "ReadLine");
      return;
    }
    else {
      myIO = EOF;
      Buffer.Clear();
      NByteRead = 0 ;
    }
  }
  else   {
    NByteRead = strlen(abuffer);
    Buffer.SetValue(1,abuffer);  // Copy transfert to Buffer
    Buffer.Trunc(NByteRead);
  }
}
// -------------------------------------------------------------------------- 
// OSD::KindOfFile Retourne le type de fichier.
// -------------------------------------------------------------------------- 
OSD_KindFile OSD_File::KindOfFile ( ) const{
int status;
struct stat buffer;
TCollection_AsciiString FullName;
OSD_Path aPath;

Path(aPath);
aPath.SystemName (FullName);
status = stat (FullName.ToCString()  , &buffer );
if ( status == 0) {
  if       (  S_ISDIR(buffer.st_mode)  )  { return OSD_DIRECTORY ; }
  else if  (  S_ISREG(buffer.st_mode)  )  { return OSD_FILE      ; }
  else if  (  S_ISLNK(buffer.st_mode)  )  { return OSD_LINK      ; }
  else if  (  S_ISSOCK(buffer.st_mode) )  { return OSD_SOCKET    ; }
  else                                    { return OSD_UNKNOWN   ; }
}
return OSD_UNKNOWN   ; 

}
// -------------------------------------------------------------------------- 
// Read content of a file
// -------------------------------------------------------------------------- 
void  OSD_File::Read(      Standard_Address&  Buffer, 
                     const Standard_Integer   Nbyte,
                           Standard_Integer&  Readbyte)
{
  int status;
  
  Readbyte = 0;
  if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
    Standard_ProgramError::Raise("OSD_File::Read : it is a directory");
  }
 
  if (myFileChannel == -1)
    Standard_ProgramError::Raise("OSD_File::Read : file is not open");
  
  if (Failed()) Perror();
  
  if (myMode == OSD_WriteOnly)
    Standard_ProgramError::Raise("OSD_File::Read : file is Write only");
  
  if (Nbyte <= 0)
    Standard_ProgramError::Raise("OSD_File::Read : Nbyte is null");

  if (Buffer == NULL)
    Standard_ProgramError::Raise("OSD_File::Read : Buffer is null");
  
  status = read( (int)myFileChannel, (char*) Buffer, Nbyte);
  
  if (status == -1) myError.SetValue (errno, Iam, "Read");
  else{
    if ( status < Nbyte ) myIO = EOF;
    Readbyte = (Standard_Integer) status;
  }
}

// Write content of a file

void  OSD_File::Write(const TCollection_AsciiString &Buffer, 
                      const Standard_Integer Nbyte){

 Standard_CString writebuf;
 int status;

 if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
   Standard_ProgramError::Raise("OSD_File::Write : it is a directory");
 }
 
 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::Write : file is not open");

 if (Failed()) Perror();

 if (myMode == OSD_ReadOnly)
  Standard_ProgramError::Raise("OSD_File::Write : file is Read only");

 if (Nbyte <= 0)
  Standard_ProgramError::Raise("OSD_File::Write : Nbyte is null");

 writebuf = Buffer.ToCString();

 status = write( (int)myFileChannel, writebuf, Nbyte);

 if ( status == -1) myError.SetValue (errno, Iam, "Write");
 else
 if ( status < Nbyte ) myIO = EOF;
}


void  OSD_File::Write(const Standard_Address   Buffer, 
                      const Standard_Integer   Nbyte)
{

  int status;
  
  if (myFileChannel == -1)
    Standard_ProgramError::Raise("OSD_File::Write : file is not open");
  
  if (Failed()) Perror();
  
  if (myMode == OSD_ReadOnly)
    Standard_ProgramError::Raise("OSD_File::Write : file is Read only");
  
  if (Nbyte <= 0)
    Standard_ProgramError::Raise("OSD_File::Write : Nbyte is null");
  
  status = write( (int)myFileChannel, (const char *)Buffer, Nbyte);
  
  if ( status == -1) myError.SetValue (errno, Iam, "Write");
  else
    if ( status < Nbyte ) myIO = EOF;
}





// Move file pointer to a specified position

void  OSD_File::Seek(const Standard_Integer Offset, 
                     const OSD_FromWhere Whence){
 int status,where=0;

 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::Seek : file is not open");

 if (Failed()) Perror();

 switch (Whence){
  case OSD_FromBeginning :
    where = SEEK_SET;
    break;
  case OSD_FromHere:
    where = SEEK_CUR;
    break;
  case OSD_FromEnd:
    where = SEEK_END;
    break;
  default :
   myError.SetValue (EINVAL, Iam, "Seek");
 }
 status = (int) lseek( (int)myFileChannel, Offset, where );

 if (status == -1) myError.SetValue (errno, Iam, "Seek");
}






// Close a file

void  OSD_File::Close(){
 int status;

 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::Close : file is not open");

 if (Failed()) Perror();

 status = close ( (int) myFileChannel );

 if (status == -1) myError.SetValue (errno, Iam, "Close");
 myFileChannel = -1;
 if ( myFILE != NULL ) {
   status = fclose ( (FILE*) myFILE );
   myFILE = (Standard_Address) NULL;
 }
 myIO = 0;
}




// -------------------------------------------------------------------------- 
// Test if at end of file
// -------------------------------------------------------------------------- 

Standard_Boolean OSD_File::IsAtEnd(){
 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::IsAtEnd : file is not open");

 if (myIO == EOF) return (Standard_True);
 return (Standard_False);
}



/*void  OSD_File::Link(const TCollection_AsciiString& ToFile){
 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::Link : file is not open");

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 link ( aBuffer.ToCString(), ToFile.ToCString() );

}*/



void  OSD_File::SetLock(const OSD_LockType Lock){
int status;

 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::SetLock : file is not open");


#ifdef POSIX
 int lock;
 struct stat buf;

 switch (Lock){
  case OSD_ExclusiveLock :
  case OSD_WriteLock     : lock = F_LOCK;
                           break;
  case OSD_ReadLock      : return;
                           break;
  default : myError.SetValue (EINVAL, Iam, "SetLock");
            return;
 }

 if (fstat((int)myFileChannel, &buf) == -1) {
  myError.SetValue (errno, Iam, "SetLock");
  return;
 }

 status = lockf(myFileChannel, lock, buf.st_size);
 if (status == -1) myError.SetValue (errno, Iam, "SetLock");
 else myLock = Lock;

#else
#ifdef SYSV
 struct stat  buf;
 struct flock key;

 switch (Lock){
  case OSD_ExclusiveLock :
  case OSD_WriteLock     : key.l_type = F_WRLCK;
                           break;
  case OSD_ReadLock      : key.l_type = F_RDLCK;
                           break;
  case OSD_NoLock : return;
 // default : myError.SetValue (EINVAL, Iam, "SetLock");
 }

 key.l_whence = 0;
 key.l_start = 0;
 key.l_len = 0;

 status = fcntl( (int) myFileChannel ,F_SETLKW,(long) &key);
 if (status == -1) myError.SetValue (errno, Iam, "SetLock");
 else myLock = Lock;

 if (Lock == OSD_ExclusiveLock){
  fstat( (int) myFileChannel ,&buf);
  TCollection_AsciiString aBuffer;
  myPath.SystemName ( aBuffer );
  chmod( aBuffer.ToCString() ,buf.st_mode | S_ISGID);
  ImperativeFlag = Standard_True;
 }

#else   /* BSD */
 int lock;

 switch (Lock){
  case OSD_ExclusiveLock :
  case OSD_WriteLock     : lock = F_WRLCK;
                           break;
  case OSD_ReadLock      : lock = F_RDLCK;
                           break;
  default : myError.SetValue (EINVAL, Iam, "SetLock");
 }

 status = flock((int)myFileChannel,lock);
 if (status == -1) myError.SetValue (errno, Iam, "SetLock");
 else myLock = Lock;
#endif // SysV
#endif // POSIX
}




// Remove a lock from a file

void  OSD_File::UnLock(){
int status;

 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::UnLock : file is not open");

#ifdef POSIX
 struct stat buf;

 if (fstat(myFileChannel, &buf) == -1) {
  myError.SetValue(errno, Iam, "UnsetLock");
  return;
 }
 
 status = lockf(myFileChannel,F_ULOCK, buf.st_size);
  if (status == -1) myError.SetValue (errno, Iam, "SetLock");

#else
#ifdef SYSV
 struct stat  buf;
 struct flock key;

 if (ImperativeFlag){
  fstat((int)myFileChannel,&buf);
  TCollection_AsciiString aBuffer;
  myPath.SystemName ( aBuffer );
  chmod(aBuffer.ToCString(),buf.st_mode & ~S_ISGID);
  ImperativeFlag = Standard_False;
 }
 key.l_type = F_UNLCK;
 status = fcntl((int)myFileChannel,F_SETLK,(long) &key);
 if (status == -1) myError.SetValue (errno, Iam, "UnSetLock");
#else

 status = flock((int)myFileChannel,LOCK_UN);
 if (status == -1) myError.SetValue (errno, Iam, "UnSetLock");
#endif
#endif // POSIX
 else myLock = OSD_NoLock;
}





// Return lock of a file

OSD_LockType  OSD_File::GetLock(){
 if (myFileChannel == -1)
  Standard_ProgramError::Raise("OSD_File::GetLock : file is not open");

 return(myLock);
}




// -------------------------------------------------------------------------- 
// Return size of a file
// -------------------------------------------------------------------------- 

Standard_Integer  OSD_File::Size(){
 struct stat buffer;
 int status;

 if (myPath.Name().Length()==0)
  Standard_ProgramError::Raise("OSD_File::Size : empty file name");

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status = stat( aBuffer.ToCString() ,&buffer );
 if (status == -1) {
  myError.SetValue (errno, Iam, "Size");
  return (-1);
 }

 return ( buffer.st_size );
}


// -------------------------------------------------------------------------- 
// Print contains of a file
// -------------------------------------------------------------------------- 

void OSD_File::Print (const OSD_Printer &WhichPrinter ){
char buffer[255];
TCollection_AsciiString PrinterName;

 if (myPath.Name().Length()==0)
   Standard_ProgramError::Raise("OSD_File::Print : empty file name");

 WhichPrinter.Name(PrinterName);

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );

 if (PrinterName.Length()==0)
   sprintf(buffer,"lp %s",aBuffer.ToCString());
 else
   sprintf(buffer,"lpr -P%s %s",PrinterName.ToCString(),aBuffer.ToCString());

 system(buffer);
}


// -------------------------------------------------------------------------- 
// Test if a file is open
// -------------------------------------------------------------------------- 

Standard_Boolean OSD_File::IsOpen()const{

 if (myPath.Name().Length()==0)
   Standard_ProgramError::Raise("OSD_File::IsOpen : empty file name");

 return (myFileChannel != -1);
}


Standard_Boolean OSD_File::IsLocked(){

 if (myPath.Name().Length()==0)
   Standard_ProgramError::Raise("OSD_File::IsLocked : empty file name");

 return(myLock != OSD_NoLock); 
}

Standard_Boolean OSD_File::IsReadable()
{
  TCollection_AsciiString FileName ;

  myPath.SystemName(FileName) ;

  if (access(FileName.ToCString(),F_OK|R_OK))
    return Standard_False;
  else
    return Standard_True;
}

Standard_Boolean OSD_File::IsWriteable()
{
  TCollection_AsciiString FileName ;

  myPath.SystemName(FileName) ;

  if (access(FileName.ToCString(),F_OK|R_OK|W_OK))
    return Standard_False;
  else
    return Standard_True;
}

Standard_Boolean OSD_File::IsExecutable()
{
  TCollection_AsciiString FileName ;

  myPath.SystemName(FileName) ;

  if (access(FileName.ToCString(),F_OK|X_OK))
    return Standard_False;
  else
    return Standard_True;
}

#else //WNT

//------------------------------------------------------------------------
//-------------------  Windows NT sources for OSD_File -------------------
//------------------------------------------------------------------------

#include <windows.h>

#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <OSD_Printer.hxx>
#include <Standard_ProgramError.hxx>

#include <OSD_WNT_1.hxx>

#include <stdio.h>
#include <io.h>
#include <Standard_PCharacter.hxx>

#ifndef _INC_TCHAR
# include <tchar.h>
#endif  // _INC_TCHAR

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define VAC
#define _int64 int
#endif

#pragma comment( lib, "WSOCK32.LIB"  )
#pragma comment( lib, "WINSPOOL.LIB" )

#define ACE_HEADER_SIZE (  sizeof ( ACCESS_ALLOWED_ACE ) - sizeof ( DWORD )  )

#define RAISE( arg ) Standard_ProgramError :: Raise (  ( arg )  )
#define TEST_RAISE( arg ) _test_raise (  myFileChannel, ( arg )  )

#define OPEN_NEW    0
#define OPEN_OLD    1
#define OPEN_APPEND 2

void                            _osd_wnt_set_error        ( OSD_Error&, OSD_WhoAmI, ... );
PSECURITY_DESCRIPTOR __fastcall _osd_wnt_protection_to_sd ( const OSD_Protection&, BOOL, char* = NULL );
BOOL                 __fastcall _osd_wnt_sd_to_protection (
                                 PSECURITY_DESCRIPTOR, OSD_Protection&, BOOL
                                );
BOOL                 __fastcall _osd_print (const Standard_PCharacter, Standard_CString );

static void      __fastcall _test_raise ( Standard_Integer, Standard_CString );
static DWORDLONG __fastcall _get_line   ( Standard_PCharacter&, DWORD );
static int       __fastcall _get_buffer ( HANDLE, Standard_PCharacter&, DWORD, BOOL, BOOL );
static DWORD     __fastcall _get_access_mask ( OSD_SingleProtection );
static DWORD     __fastcall _get_dir_access_mask ( OSD_SingleProtection prt );
static HANDLE    __fastcall _open_file  ( Standard_CString, OSD_OpenMode, DWORD, LPBOOL = NULL );

static OSD_SingleProtection __fastcall _get_protection     ( DWORD );
static OSD_SingleProtection __fastcall _get_protection_dir ( DWORD );

typedef OSD_SingleProtection ( __fastcall *GET_PROT_FUNC ) ( DWORD );

Standard_Integer __fastcall _get_file_type ( Standard_CString, Standard_Integer );

// ---------------------------------------------------------------------
// Create an empty file object
// ---------------------------------------------------------------------

OSD_File :: OSD_File () {

 ImperativeFlag = Standard_False;
 myLock         = OSD_NoLock;
 myIO           = 0;

}  // end constructor ( 1 )

// ---------------------------------------------------------------------
// Create and initialize a file object
// ---------------------------------------------------------------------

OSD_File :: OSD_File ( const OSD_Path& Name ) : OSD_FileNode ( Name ) {

 ImperativeFlag = Standard_False;
 myLock         = OSD_NoLock;
 myIO           = 0;
 myPath         = Name;

}  // end constructor ( 2 )

// ---------------------------------------------------------------------
// Build a file if it doesn't exist or create again if it already exists
// ---------------------------------------------------------------------

void OSD_File :: Build (
                  const OSD_OpenMode Mode, const OSD_Protection& Protect
                 ) {

 TCollection_AsciiString fName;

 if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
   Standard_ProgramError::Raise("OSD_File::Read : it is a directory");
 }
                                        
 if (  ( HANDLE )myFileChannel != INVALID_HANDLE_VALUE  )

  RAISE(  TEXT( "OSD_File :: Build (): incorrect call - file already opened" )  );

 myMode = Mode;
 myPath.SystemName ( fName );

 if (  fName.IsEmpty ()  )

  RAISE(  TEXT( "OSD_File :: Build (): incorrent call - no filename given" )  );

 myFileChannel = ( Standard_Integer )_open_file ( fName.ToCString (), Mode, OPEN_NEW );

 if (  ( HANDLE )myFileChannel == INVALID_HANDLE_VALUE  )

  _osd_wnt_set_error ( myError, OSD_WFile );

 else {

  SetProtection ( Protect );
  myIO |= FLAG_FILE;

 }  // end else

}  // end OSD_File :: Build



// ---------------------------------------------------------------------
// Open a file
// ---------------------------------------------------------------------

void OSD_File :: Open (
                  const OSD_OpenMode Mode, const OSD_Protection& Protect
                 ) {

 TCollection_AsciiString fName;


 if (  ( HANDLE )myFileChannel != INVALID_HANDLE_VALUE  )

  RAISE(  TEXT( "OSD_File :: Open (): incorrect call - file already opened" )  );

 myMode = Mode;
 myPath.SystemName ( fName );

 if (  fName.IsEmpty ()  )

  RAISE(  TEXT( "OSD_File :: Open (): incorrent call - no filename given" )  );

 myFileChannel = ( Standard_Integer )_open_file ( fName.ToCString (), Mode, OPEN_OLD );

 if (  ( HANDLE )myFileChannel == INVALID_HANDLE_VALUE  ) {

   _osd_wnt_set_error ( myError, OSD_WFile );
 }
 else
   {
     myIO |= _get_file_type (  fName.ToCString (), myFileChannel  );
   }
}  // end OSD_File :: Open

// ---------------------------------------------------------------------
// Append to an existing file
// ---------------------------------------------------------------------

void OSD_File :: Append (
                  const OSD_OpenMode Mode, const OSD_Protection& Protect
                 ) {

 BOOL                    fNew = FALSE;
 TCollection_AsciiString fName;

 if (  ( HANDLE )myFileChannel != INVALID_HANDLE_VALUE  )

  RAISE(  TEXT( "OSD_File :: Append (): incorrect call - file already opened" )  );

 myMode = Mode;
 myPath.SystemName ( fName );

 if (  fName.IsEmpty ()  )

  RAISE(  TEXT( "OSD_File :: Append (): incorrent call - no filename given" )  );

 myFileChannel = ( Standard_Integer )_open_file ( fName.ToCString (), Mode, OPEN_APPEND, &fNew );

 if (  ( HANDLE )myFileChannel == INVALID_HANDLE_VALUE  )

  _osd_wnt_set_error ( myError, OSD_WFile );

 else {

  if ( !fNew ) {

   myIO |= _get_file_type (  fName.ToCString (), myFileChannel  );
   Seek ( 0, OSD_FromEnd );

  } else {
  
   SetProtection ( Protect );
   myIO |= FLAG_FILE;
  
  }  // end else

 }  // end else;

}  // end OSD_File :: Append

// ---------------------------------------------------------------------
// Read content of a file
// ---------------------------------------------------------------------
 
void OSD_File :: Read (
                  TCollection_AsciiString& Buffer, const Standard_Integer Nbyte
                 ) {

 if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
   cout << " OSD_File::Read : it is a directory " << endl;
   return ;
//   Standard_ProgramError::Raise("OSD_File::Read : it is a directory");
 }
                                        
 Standard_Integer NbyteRead;
 Standard_Address buff;

 TEST_RAISE(  TEXT( "Read" )  );
     
 buff = ( Standard_Address )new Standard_Character[ Nbyte + 1 ];

 Read ( buff, Nbyte, NbyteRead );

 (  ( Standard_PCharacter )buff  )[ NbyteRead ] = 0;

 if ( NbyteRead != 0 )

  Buffer = ( Standard_PCharacter )buff;

 else

  Buffer.Clear ();

 delete [] buff;

}  // end OSD_File :: Read

#define PRO13471

// ---------------------------------------------------------------------
// Read a line from a file
// ---------------------------------------------------------------------

#ifdef PRO13471

// Modified so that we have <nl> at end of line if we have read <nl> or <cr>
// in the file.
// by LD 17 dec 98 for B4.4

void OSD_File :: ReadLine (
                  TCollection_AsciiString& Buffer,
                  const Standard_Integer NByte, Standard_Integer& NbyteRead
                 ) {

 DWORDLONG          status;
 DWORD              dwBytesRead;
 DWORD              dwDummy;
 Standard_Character peekChar;
 Standard_PCharacter ppeekChar;
 Standard_PCharacter cBuffer;
 Standard_CString   eos;
 DWORD              dwSeekPos;

 if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
   Standard_ProgramError::Raise("OSD_File::Read : it is a directory");
 }
                                        
 TEST_RAISE(  TEXT( "ReadLine" )  );              

 if (  myIO & FLAG_PIPE && !( myIO & FLAG_READ_PIPE )  )

  RAISE(  TEXT( "OSD_File :: ReadLine (): attempt to read from write only pipe" )  );

                                        // +----> leave space for end-of-string
                                        // |       plus <CR><LF> sequence      
                                        // |

 ppeekChar=&peekChar;
 cBuffer = new Standard_Character[ NByte + 3 ];

 if ( myIO & FLAG_FILE ) {
 
  if (  !ReadFile (
          ( HANDLE )myFileChannel, cBuffer, ( DWORD )NByte, &dwBytesRead, NULL
         )
  ) {  // an error occured

   _osd_wnt_set_error ( myError, OSD_WFile );   
   Buffer.Clear ();
   NbyteRead = 0;
   
  } else if ( dwBytesRead == 0 ) {  // end-of-file reached
   
   Buffer.Clear ();
   NbyteRead = 0;
   myIO |= FLAG_EOF;
   
  } else {
   myIO &= ~FLAG_EOF ;  // if the file increased since last read (LD)
   status = _get_line ( cBuffer, dwBytesRead );

   dwSeekPos = LODWORD( status );
   eos       = ( Standard_CString )HIDWORD( status );
#ifdef VAC
   if ( (__int64) status == (__int64) -1 ) {  // last character in the buffer is <CR> -
#else
   if ( status == 0xFFFFFFFFFFFFFFFF ) {  // last character in the buffer is <CR> -
                                          // peek next character to see if it is a <LF>
#endif
    if (  !ReadFile (
            ( HANDLE )myFileChannel, ppeekChar, 1, &dwDummy, NULL
           )
    ) {
    
     _osd_wnt_set_error ( myError, OSD_WFile );

    } else if ( dwDummy != 0 ) {  // end-of-file reached ?

     if ( peekChar != '\n' )  // if we did not get a <CR><LF> sequence
    
     // adjust file position

      SetFilePointer (  ( HANDLE )myFileChannel, -1, NULL, FILE_CURRENT  );

    } else
     myIO |= FLAG_EOF;

    NbyteRead = dwBytesRead;

   } else {

    if ( dwSeekPos != 0 )
     SetFilePointer (  ( HANDLE )myFileChannel, ( LONG )dwSeekPos, NULL, FILE_CURRENT  );

    NbyteRead = ( Standard_Integer )( eos - cBuffer );

   }

  }  // end else
   
 } else if ( myIO & FLAG_SOCKET || myIO & FLAG_PIPE || myIO & FLAG_NAMED_PIPE ) {

  dwBytesRead = ( DWORD )_get_buffer (
                          ( HANDLE )myFileChannel, cBuffer, ( DWORD )NByte,
                          TRUE, myIO & FLAG_SOCKET
                         );

  if (  ( int )dwBytesRead == -1  ) { // an error occured

   _osd_wnt_set_error ( myError, OSD_WFile );
   Buffer.Clear ();
   NbyteRead = 0;

  } else if ( dwBytesRead == 0 )  { // connection closed - set end-of-file flag

   Buffer.Clear ();
   NbyteRead = 0;
   myIO |= FLAG_EOF;

  } else {

   status = _get_line ( cBuffer, dwBytesRead );

   dwSeekPos = LODWORD( status );
   eos       = ( Standard_CString )HIDWORD( status );

#ifdef VAC
   if ( (__int64) status == (__int64) -1 ) {  // last character in the buffer is <CR> -
#else  
   if ( status == 0xFFFFFFFFFFFFFFFF ) {  // last character in the buffer is <CR> -    
                                          // peek next character to see if it is a <LF>
#endif

    NbyteRead = dwBytesRead; // (LD) always fits this case.

    dwDummy = _get_buffer (
               ( HANDLE )myFileChannel, ppeekChar, 1, TRUE, myIO & FLAG_SOCKET
              );
    if (  ( int )dwDummy == -1  ) {  // an error occured
   
     _osd_wnt_set_error ( myError, OSD_WFile );

    } else if ( dwDummy != 0 ) {  // connection closed ?

     if ( peekChar == '\n' )  // we got a <CR><LF> sequence

       dwBytesRead++ ;  // (LD) we have to jump <LF>
    } else

     myIO |= FLAG_EOF;

   } else {

    if ( dwSeekPos != 0 )
     dwBytesRead = dwBytesRead + dwSeekPos;

    NbyteRead  = ( Standard_Integer )( eos - cBuffer );

   }

   // Don't rewrite datas in cBuffer.

   Standard_PCharacter cDummyBuffer = new Standard_Character[ NByte + 3 ];

   _get_buffer (  // remove pending input
    ( HANDLE )myFileChannel, cDummyBuffer, dwBytesRead, FALSE, myIO & FLAG_SOCKET
   );
   delete [] cDummyBuffer ;

  }  // end else
   
 } else

  RAISE(  TEXT( "OSD_File :: ReadLine (): incorrect call - file is a directory" )  );

 if (  !Failed () && !IsAtEnd ()  )

  Buffer = cBuffer;

 delete [] (Standard_PCharacter)cBuffer;

}  // end OSD_File :: ReadLine

#else  // PRO13471

void OSD_File :: ReadLine (
                  TCollection_AsciiString& Buffer,
                  const Standard_Integer NByte, Standard_Integer& NbyteRead
                 ) {

 DWORDLONG          status;
 DWORD              dwBytesRead;
 DWORD              dwDummy;
 Standard_Character peekChar;
 Standard_CString   cBuffer;
 Standard_CString   eos;
 DWORD              dwSeekPos;
                                        
 if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
   Standard_ProgramError::Raise("OSD_File::Read : it is a directory");
 }

                                        
 TEST_RAISE(  TEXT( "ReadLine" )  );              

 if (  myIO & FLAG_PIPE && !( myIO & FLAG_READ_PIPE )  )

  RAISE(  TEXT( "OSD_File :: ReadLine (): attempt to read from write only pipe" )  );

                                        // +----> leave space for end-of-string
                                        // |       plus <CR><LF> sequence      
                                        // |
 cBuffer = new Standard_Character[ NByte + 3 ];

 if ( myIO & FLAG_FILE ) {
 
  if (  !ReadFile (
          ( HANDLE )myFileChannel, cBuffer, ( DWORD )NByte, &dwBytesRead, NULL
         )
  ) {  // an error occured

   _osd_wnt_set_error ( myError, OSD_WFile );   
   Buffer.Clear ();
   NbyteRead = 0;
   
  } else if ( dwBytesRead == 0 ) {  // end-of-file reached
   
   Buffer.Clear ();
   NbyteRead = 0;
   myIO |= FLAG_EOF;
   
  } else {

   status = _get_line ( cBuffer, dwBytesRead );

   dwSeekPos = LODWORD( status );
   eos       = ( Standard_CString )HIDWORD( status );
#ifdef VAC
   if ( (__int64) status == (__int64) -1 ) {  // last character in the buffer is <CR> -
#else
   if ( status == 0xFFFFFFFFFFFFFFFF ) {  // last character in the buffer is <CR> -
                                          // peek next character to see if it is a <LF>
#endif
    if (  !ReadFile (
            ( HANDLE )myFileChannel, &peekChar, 1, &dwDummy, NULL
           )
    ) {
    
     _osd_wnt_set_error ( myError, OSD_WFile );
     NbyteRead = dwBytesRead;

    } else if ( dwDummy != 0 ) {  // end-of-file reached ?

     if ( peekChar == '\n' )  // we got a <CR><LF> sequence
    
      cBuffer[ --dwBytesRead ] = 0;
    
     else  // adjust file position

      SetFilePointer (  ( HANDLE )myFileChannel, -1, NULL, FILE_CURRENT  );

    } else
         
     myIO |= FLAG_EOF;

    NbyteRead = dwBytesRead;

   } else if ( dwSeekPos != 0 ) {
   
    SetFilePointer (  ( HANDLE )myFileChannel, ( LONG )dwSeekPos, NULL, FILE_CURRENT  );
    NbyteRead = ( Standard_Integer )( eos - cBuffer );

   } else

    NbyteRead = eos - cBuffer;

  }  // end else
   
 } else if ( myIO & FLAG_SOCKET || myIO & FLAG_PIPE || myIO & FLAG_NAMED_PIPE ) {

  dwBytesRead = ( DWORD )_get_buffer (
                          ( HANDLE )myFileChannel, cBuffer, ( DWORD )NByte,
                          TRUE, myIO & FLAG_SOCKET
                         );

  if (  ( int )dwBytesRead == -1  )  // an error occured

   _osd_wnt_set_error ( myError, OSD_WFile );

  else if ( dwBytesRead == 0 )  { // connection closed - set end-of-file flag

   Buffer.Clear ();
   NbyteRead = 0;
   myIO |= FLAG_EOF;

  } else {

   status = _get_line ( cBuffer, dwBytesRead );

   dwSeekPos = LODWORD( status );
   eos       = ( Standard_CString )HIDWORD( status );

#ifdef VAC
   if ( (__int64) status == (__int64) -1 ) {  // last character in the buffer is <CR> -
#else  
   if ( status == 0xFFFFFFFFFFFFFFFF ) {  // last character in the buffer is <CR> -    
                                          // peek next character to see if it is a <LF>
#endif
    dwDummy = _get_buffer (
               ( HANDLE )myFileChannel, &peekChar, 1, TRUE, myIO & FLAG_SOCKET
              );
    eos     = cBuffer + dwBytesRead;

    if (  ( int )dwDummy == -1  ) {  // an error occured
   
     _osd_wnt_set_error ( myError, OSD_WFile );
     NbyteRead = dwBytesRead;

    } else if ( dwDummy != 0 ) {  // connection closed ?

     if ( peekChar == '\n' )  // we got a <CR><LF> sequence

      cBuffer[ dwBytesRead - 1 ] = 0, eos = cBuffer + ( dwBytesRead++ - 1 );

    } else

     myIO |= FLAG_EOF;

    NbyteRead = dwBytesRead;

   } else if ( dwSeekPos != 0 ) {
   
    dwBytesRead = dwBytesRead + dwSeekPos;
    NbyteRead  = ( Standard_Integer )( eos - cBuffer );

   } else

    NbyteRead = ( Standard_Integer )( eos - cBuffer );

   _get_buffer (  // remove pending input
    ( HANDLE )myFileChannel, cBuffer, dwBytesRead, FALSE, myIO & FLAG_SOCKET
   );
   *eos = 0;

  }  // end else
   
 } else

  RAISE(  TEXT( "OSD_File :: ReadLine (): incorrect call - file is a directory" )  );

 if (  !Failed () && !IsAtEnd ()  )

  Buffer = cBuffer;

 delete [] cBuffer;

}  // end OSD_File :: ReadLine

#endif // PRO13471

// -------------------------------------------------------------------------- 
// Read content of a file
// -------------------------------------------------------------------------- 

void OSD_File :: Read (
                  Standard_Address& Buffer,
                  const Standard_Integer Nbyte, Standard_Integer& Readbyte
                 ) {

 DWORD dwBytesRead;

 if ( OSD_File::KindOfFile ( ) == OSD_DIRECTORY ) { 
   Standard_ProgramError::Raise("OSD_File::Read : it is a directory");
 }
                                        
 TEST_RAISE(  TEXT( "Read" )  );

 if (  myIO & FLAG_PIPE && !( myIO & FLAG_READ_PIPE )  )

  RAISE(  TEXT( "OSD_File :: Read (): attempt to read from write only pipe" )  );

 if (  !ReadFile (
         ( HANDLE )myFileChannel, Buffer, ( DWORD )Nbyte, &dwBytesRead, NULL
        )
 ) {
 
  _osd_wnt_set_error ( myError, OSD_WFile );
  dwBytesRead = 0;
 
 } else if ( dwBytesRead == 0 )

  myIO |= FLAG_EOF;

 else

  myIO &= ~FLAG_EOF;

 Readbyte = ( Standard_Integer )dwBytesRead;

}  // end OSD_File :: Read

void OSD_File :: Write (
                  const TCollection_AsciiString& Buffer,
                  const Standard_Integer Nbyte
                 ) {

 Write (  ( Standard_Address )Buffer.ToCString (), Nbyte  );

}  // end OSD_File :: Write

// -------------------------------------------------------------------------- 
// Write content of a file
// -------------------------------------------------------------------------- 

void OSD_File :: Write (
                  const Standard_Address Buffer,
                  const Standard_Integer Nbyte
                 ) {

 DWORD dwBytesWritten;

 TEST_RAISE(  TEXT( "Write" )  );

 if ( myIO & FLAG_PIPE && myIO & FLAG_READ_PIPE )

  RAISE(  TEXT( "OSD_File :: Write (): attempt to write to read only pipe" )  );

 if (  !WriteFile (
         ( HANDLE )myFileChannel, Buffer, ( DWORD )Nbyte, &dwBytesWritten, NULL
        ) || dwBytesWritten != ( DWORD )Nbyte
 )

  _osd_wnt_set_error ( myError, OSD_WFile );

}  // end OSD_File :: Write

void OSD_File :: Seek (
                  const Standard_Integer Offset, const OSD_FromWhere Whence
                 ) {

 DWORD dwMoveMethod;

 TEST_RAISE(  TEXT( "Seek" )  );

 if ( myIO & FLAG_FILE || myIO & FLAG_DIRECTORY ) {

  switch ( Whence ) {
  
   case OSD_FromBeginning:

    dwMoveMethod = FILE_BEGIN;

   break;

   case OSD_FromHere:

    dwMoveMethod = FILE_CURRENT;

   break;

   case OSD_FromEnd:

    dwMoveMethod = FILE_END;

   break;

   default:

    RAISE(  TEXT( "OSD_File :: Seek (): invalid parameter" )  );
  
  }  // end switch

  if (  SetFilePointer (
         ( HANDLE )myFileChannel, ( LONG )Offset, NULL, dwMoveMethod
        ) == 0xFFFFFFFF
  )

   _osd_wnt_set_error ( myError, OSD_WFile );
  
 }  // end if

 myIO &= ~FLAG_EOF;

}  // end OSD_File :: Seek

// -------------------------------------------------------------------------- 
// Close a file
// -------------------------------------------------------------------------- 

void OSD_File :: Close () {

 TEST_RAISE(  TEXT( "Close" )  );

 CloseHandle (  ( HANDLE )myFileChannel  );

 myFileChannel = ( Standard_Integer )INVALID_HANDLE_VALUE;
 myIO          = 0;

}  // end OSD_File :: Close

// -------------------------------------------------------------------------- 
// Test if at end of file
// -------------------------------------------------------------------------- 

Standard_Boolean OSD_File :: IsAtEnd () {

 TEST_RAISE(  TEXT( "IsAtEnd" )  );

 if (myIO & FLAG_EOF)
   return Standard_True ;
 return Standard_False ;

}  // end OSD_File :: IsAtEnd

OSD_KindFile OSD_File :: KindOfFile () const {

 OSD_KindFile     retVal;
 Standard_Integer flags;

 if (  ( HANDLE )myFileChannel == INVALID_HANDLE_VALUE  ) {

  TCollection_AsciiString fName;

  myPath.SystemName ( fName );

  if (  fName.IsEmpty ()  )

   RAISE(  TEXT( "OSD_File :: KindOfFile (): incorrent call - no filename given" )  );

  flags = _get_file_type (
            fName.ToCString (), ( Standard_Integer )INVALID_HANDLE_VALUE
           );

 } else

  flags = myIO;

 switch ( flags & FLAG_TYPE ) {
 
  case FLAG_FILE:

   retVal = OSD_FILE;

  break;

  case FLAG_DIRECTORY:

   retVal = OSD_DIRECTORY;

  break;

  case FLAG_SOCKET:

   retVal = OSD_SOCKET;

  break;

  default:

   retVal = OSD_UNKNOWN;
 
 }  // end switch

 return retVal;

}  // end OSD_File :: KindOfFile
#define PRO13792
#ifndef PRO13792

OSD_File OSD_File :: BuildTemporary () {

 OSD_Protection          prt;
 OSD_File                retVal;

#ifdef VAC
 char tmpbuf [MAX_PATH];
 if (GetTempPath (MAX_PATH, tmpbuf) == 0)
 {
    perror ("ERROR in GetTempPath");
    exit (10);
 }
 char tmpbuf2 [MAX_PATH];
 if (GetTempFileName (tmpbuf, NULL, 0, tmpbuf2) == 0)
 {
    perror ("ERROR in GetTempFileName");
    exit (10);
 }
 TCollection_AsciiString fileName (  tmpbuf2  );
#else
 TCollection_AsciiString fileName (  _ttmpnam ( NULL )  );
#endif

 OSD_Path                filePath ( fileName );

 retVal.SetPath ( filePath );
 retVal.Build   ( OSD_ReadWrite, prt );

 return retVal;

}  // end OSD_File :: BuildTemporary

#else   // PRO13792

//-------------------------------------------------debutpri???980424

typedef struct _osd_wnt_key {

                HKEY   hKey;
                LPTSTR keyPath;

               } OSD_WNT_KEY;


OSD_File OSD_File :: BuildTemporary () {

 OSD_Protection prt;
 OSD_File       retVal;
 HKEY           hKey;
 TCHAR          tmpPath[ MAX_PATH ];
 BOOL           fOK = FALSE;
 OSD_WNT_KEY    regKey[ 2 ] = {
 
                 { HKEY_LOCAL_MACHINE,
                   TEXT( "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" )
                 },
                 { HKEY_USERS,
                   TEXT( ".DEFAULT\\Environment" )
                 }
 
                };
 
 for ( int i = 0; i < 2; ++i ) {

  if (  RegOpenKeyEx (
         regKey[ i ].hKey, regKey[ i ].keyPath, 0, KEY_QUERY_VALUE, &hKey
       ) == ERROR_SUCCESS
  ) {
  
   DWORD dwType;
   DWORD dwSize = 0;

   if (  RegQueryValueEx (
          hKey, TEXT( "TEMP" ), NULL, &dwType, NULL, &dwSize
         ) == ERROR_SUCCESS
   ) {
  
    LPTSTR kVal = ( LPTSTR )HeapAlloc (
                             GetProcessHeap (), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS,
                             dwSize + sizeof ( TCHAR )
                            );

     RegQueryValueEx (  hKey, TEXT( "TEMP" ), NULL, &dwType, ( LPBYTE )kVal, &dwSize  );

     if ( dwType == REG_EXPAND_SZ )
    
      ExpandEnvironmentStrings ( kVal, tmpPath, MAX_PATH );

     else

      lstrcpy ( tmpPath, kVal );

    HeapFree (  GetProcessHeap (), 0, ( LPVOID )kVal  );
    fOK = TRUE;

   }  // end if

   RegCloseKey ( hKey );

  }  // end if

  if ( fOK ) break;

 }  // end for

 if ( !fOK ) lstrcpy (  tmpPath, TEXT( "./" )  );
 
 GetTempFileName ( tmpPath, "CSF", 0, tmpPath );

 retVal.SetPath (  OSD_Path ( tmpPath )  );
 retVal.Build   (  OSD_ReadWrite, prt    );

 return retVal;

}  // end OSD_File :: BuildTemporary

#endif // PRO13792

//-------------------------------------------------finpri???980424

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return
#endif

void OSD_File :: SetLock ( const OSD_LockType Lock ) {

 DWORD      dwFlags;
 OVERLAPPED ovlp;

 TEST_RAISE(  TEXT( "SetLock" )  );
 
 ZeroMemory (  &ovlp, sizeof ( OVERLAPPED )  );

 __try {

  if (  ( myLock = Lock ) == OSD_NoLock  ) {

   UnLock ();
   __leave;

  } else if ( myLock == OSD_ReadLock || myLock == OSD_ExclusiveLock ) {

   dwFlags = LOCKFILE_EXCLUSIVE_LOCK;

  } else

   dwFlags = 0;

  if (  !LockFileEx (
          ( HANDLE )myFileChannel, dwFlags, 0, Size (), 0, &ovlp
         )
  ) {

   _osd_wnt_set_error ( myError, OSD_WFile );
   __leave;

  }  // end if

  ImperativeFlag = Standard_True;

 }  // end __try

 __finally {}

#ifdef VAC
leave: ;         // added for VisualAge
#endif
}  // end OSD_File :: SetLock

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

void OSD_File :: UnLock () {

 TEST_RAISE(  TEXT( "Unlock" )  );

 if ( ImperativeFlag ) {
 
  if (  !UnlockFile (
          ( HANDLE )myFileChannel, 0, 0, Size (), 0
         )
  )
   
   _osd_wnt_set_error ( myError, OSD_WFile );

  ImperativeFlag = Standard_False;
 
 }  // end if

}  // end OSD_File :: UnLock

OSD_LockType OSD_File :: GetLock () {

 return myLock;

}  // end OSD_File :: GetLock

Standard_Boolean OSD_File :: IsLocked () {

 TEST_RAISE(  TEXT( "IsLocked" )  );

 return ImperativeFlag;

}  // end OSD_File :: IsLocked


// -------------------------------------------------------------------------- 
// Return size of a file
// -------------------------------------------------------------------------- 

Standard_Integer OSD_File :: Size () {

 Standard_Integer retVal;

 TEST_RAISE(  TEXT( "Size" )  );

 retVal = ( Standard_Integer )GetFileSize (
                               ( HANDLE )myFileChannel, NULL
                              );

 if (  retVal == ( Standard_Integer )0xFFFFFFFF  )

  _osd_wnt_set_error ( myError, OSD_WFile );

 return retVal;

}  // end OSD_File :: Size

// -------------------------------------------------------------------------- 
// Print contains of a file
// -------------------------------------------------------------------------- 

void OSD_File :: Print ( const OSD_Printer& WhichPrinter ) {

 if (  ( HANDLE )myFileChannel != INVALID_HANDLE_VALUE  )

  RAISE(  TEXT( "OSD_File :: Print (): incorrect call - file opened" )  );

 TCollection_AsciiString pName, fName;

 WhichPrinter.Name ( pName );
 myPath.SystemName ( fName );

 if (   !_osd_print ( (Standard_PCharacter)pName.ToCString (), fName.ToCString ()  )   )

  _osd_wnt_set_error ( myError, OSD_WFile );

}  // end OSD_File :: Print

// -------------------------------------------------------------------------- 
// Test if a file is open
// -------------------------------------------------------------------------- 

Standard_Boolean OSD_File :: IsOpen () const {

 return ( HANDLE )myFileChannel != INVALID_HANDLE_VALUE;

}  // end OSD_File :: IsOpen

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return retVal
#endif

PSECURITY_DESCRIPTOR __fastcall _osd_wnt_protection_to_sd (
                                 const OSD_Protection& prot, BOOL fDir, char* fName
                                ) {

 int                  i, j;
 BOOL                 fOK      = FALSE;
 PACL                 pACL     = NULL;
 HANDLE               hProcess = NULL;
 PSID                 pSIDowner;
 PSID                 pSIDadmin;
 PSID                 pSIDworld;
 PSID                 pSIDtemp;
 DWORD                dwAccessAdmin;
 DWORD                dwAccessGroup;
 DWORD                dwAccessOwner;
 DWORD                dwAccessWorld;
 DWORD                dwAccessAdminDir;
 DWORD                dwAccessGroupDir;
 DWORD                dwAccessOwnerDir;
 DWORD                dwAccessWorldDir;
 DWORD                dwACLsize       = sizeof ( ACL );
 DWORD                dwIndex         = 0;
 PTOKEN_OWNER         pTkOwner        = NULL;
 PTOKEN_GROUPS        pTkGroups       = NULL;
 PTOKEN_PRIMARY_GROUP pTkPrimaryGroup = NULL;
 PSECURITY_DESCRIPTOR retVal;
 PSECURITY_DESCRIPTOR pfSD = NULL;
 BOOL                 fDummy;
 PFILE_ACE            pFileACE;

 __try {

  j = fDir ? 1 : 0;

  if (  !OpenProcessToken (
          GetCurrentProcess (), TOKEN_QUERY, &hProcess
         )
  ) __leave;

  if (   (  pTkGroups = ( PTOKEN_GROUPS )GetTokenInformationEx (
                                          hProcess, TokenGroups
                                         )
         ) == NULL
  ) __leave; 
  
  if (   (  pTkOwner = ( PTOKEN_OWNER )GetTokenInformationEx (
                                        hProcess, TokenOwner
                                       )
         ) == NULL
  ) __leave;

  if (   (  pTkPrimaryGroup = ( PTOKEN_PRIMARY_GROUP )GetTokenInformationEx (
                                                       hProcess, TokenPrimaryGroup
                                                      )
         ) == NULL
  ) __leave;


retry:
  if ( fName == NULL )

   pSIDowner = pTkOwner -> Owner;

  else {

   pfSD = GetFileSecurityEx ( fName, OWNER_SECURITY_INFORMATION );

   if (  pfSD == NULL || !GetSecurityDescriptorOwner ( pfSD, &pSIDowner, &fDummy )  ) {

    fName = NULL;
    goto retry;

   }  // end if

  }  // end else

  pSIDadmin = AdminSid ();
  pSIDworld = WorldSid ();

  dwAccessAdmin = _get_access_mask (  prot.System ()  );
  dwAccessGroup = _get_access_mask (  prot.Group  ()  );
  dwAccessOwner = _get_access_mask (  prot.User   ()  );
  dwAccessWorld = _get_access_mask (  prot.World  ()  );

  dwAccessAdminDir = _get_dir_access_mask (  prot.System ()  );
  dwAccessGroupDir = _get_dir_access_mask (  prot.Group  ()  );
  dwAccessOwnerDir = _get_dir_access_mask (  prot.User   ()  );
  dwAccessWorldDir = _get_dir_access_mask (  prot.World  ()  );

  if (  dwAccessGroup != 0  ) {
                                             
   for ( i = 0; i < ( int )pTkGroups -> GroupCount; ++i ) {

    pSIDtemp = pTkGroups -> Groups[ i ].Sid;

    if (  !NtPredefinedSid ( pSIDtemp                                  ) &&
          !EqualSid        ( pSIDtemp, pSIDworld                       ) &&
          !EqualSid        ( pSIDtemp, pTkPrimaryGroup -> PrimaryGroup ) &&
           IsValidSid      ( pSIDtemp                                  )
    )

     dwACLsize += ( (  GetLengthSid ( pSIDtemp ) + ACE_HEADER_SIZE  ) << j );

   }  // end for
  
  }  // end if

  dwACLsize += (  ( ( GetLengthSid ( pSIDowner ) + ACE_HEADER_SIZE ) << j ) +
                  ( ( GetLengthSid ( pSIDadmin ) + ACE_HEADER_SIZE ) << j ) +
                  ( ( GetLengthSid ( pSIDworld ) + ACE_HEADER_SIZE ) << j )
               );

  if (   (  pACL = CreateAcl ( dwACLsize )  ) == NULL   ) __leave;

  if ( dwAccessAdmin != 0 )

   if (   (  pFileACE = ( PFILE_ACE )AllocAccessAllowedAce (
                                      dwAccessAdmin, 0,
                                      pSIDadmin
                                     )
           ) != NULL
   ) {

    AddAce ( pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE -> header.AceSize );

    if ( fDir ) {

     pFileACE -> dwMask = dwAccessAdminDir;
     pFileACE -> header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
     AddAce ( pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE -> header.AceSize );

    }  // end if

    FreeAce ( pFileACE );

   }  // end if

  if ( dwAccessOwner != 0 )

   if (   (  pFileACE = ( PFILE_ACE )AllocAccessAllowedAce (
                                      dwAccessOwner, 0, pSIDowner
                                     )
          ) != NULL
   ) {

    AddAce ( pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE -> header.AceSize );

    if ( fDir ) {

     pFileACE -> dwMask = dwAccessOwnerDir;
     pFileACE -> header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
     AddAce ( pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE -> header.AceSize );
   
    }  // end if

    FreeAce ( pFileACE );

   }  // end if

  if ( dwAccessWorld != 0 )

   if (   (  pFileACE = ( PFILE_ACE )AllocAccessAllowedAce (
                                      dwAccessWorld, 0, pSIDworld
                                     )
          ) != NULL
   ) {

    AddAce ( pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE -> header.AceSize );

    if ( fDir ) {

     pFileACE -> header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
     AddAce ( pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE -> header.AceSize );
   
    }  // end if

    FreeAce ( pFileACE );

   }  // end if

  if ( dwAccessGroup != 0 ) {

   for ( i = 0; i < ( int )pTkGroups -> GroupCount; ++i ) {

    pSIDtemp = pTkGroups -> Groups[ i ].Sid;

    if (  !NtPredefinedSid ( pSIDtemp                                  ) &&
          !EqualSid        ( pSIDtemp, pSIDworld                       ) &&
          !EqualSid        ( pSIDtemp, pTkPrimaryGroup -> PrimaryGroup ) &&
           IsValidSid      ( pSIDtemp                                  )
    ) {

     if ( dwAccessGroup != 0 )

      if (   (  pFileACE = ( PFILE_ACE )AllocAccessAllowedAce (
                                         dwAccessGroup, 0, pSIDtemp
                                        )
             ) != NULL
      ) {

       AddAce ( pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE -> header.AceSize );

      if ( fDir ) {

       pFileACE -> header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE | INHERIT_ONLY_ACE;
       AddAce ( pACL, ACL_REVISION, dwIndex++, pFileACE, pFileACE -> header.AceSize );
   
      }  // end if

      FreeAce ( pFileACE );

     }  // end if

    }  // end if

   }  // end for

  }  // end if

  if (   (  retVal = AllocSD ()  ) == NULL   ) __leave;

  if (  !SetSecurityDescriptorDacl ( retVal, TRUE, pACL, TRUE )  ) __leave;

  fOK = TRUE;

 }  // end __try

 __finally {
 
  if ( !fOK ) {

   if ( retVal != NULL )
       
    FreeSD ( retVal );

   else if ( pACL != NULL )

    FreeAcl ( pACL );

   retVal = NULL;
  
  }  // end if

  if ( hProcess        != NULL ) CloseHandle          ( hProcess        );
  if ( pTkOwner        != NULL ) FreeTokenInformation ( pTkOwner        );
  if ( pTkGroups       != NULL ) FreeTokenInformation ( pTkGroups       );
  if ( pTkPrimaryGroup != NULL ) FreeTokenInformation ( pTkPrimaryGroup );
  if ( pfSD            != NULL ) FreeFileSecurity     ( pfSD            );
 
 }  // end __finally

#ifdef VAC
leave: ;     // added for VisualAge
#endif

 return retVal;
 
}  // end _osd_wnt_protection_to_sd */

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

static void __fastcall _test_raise ( Standard_Integer hFile, Standard_CString str ) {

 Standard_Character buff[ 64 ];

 if (  ( HANDLE )hFile == INVALID_HANDLE_VALUE  ) {
 
  _tcscpy (  buff, TEXT( "OSD_File :: " )  );
  _tcscat (  buff, str );
  _tcscat (  buff, TEXT( " (): wrong access" )  );

  Standard_ProgramError :: Raise ( buff );
 
 }  // end if

}  // end _test_raise

#ifdef PRO13471

// Modified so that we have <nl> at end of line if we have read <nl> or <cr>
// by LD 17 dec 98 for B4.4

static DWORDLONG __fastcall _get_line ( Standard_PCharacter& buffer, DWORD dwBuffSize ) {

 DWORDLONG        retVal;
 Standard_PCharacter ptr;

 buffer[ dwBuffSize ] = 0;
 ptr                  = buffer;

 while ( *ptr != 0 ) {
 
  if (  *ptr == TEXT( '\n' )  ) {
  
   ptr++ ;   // jump newline char.
   *ptr = 0 ;
   retVal = ptr - buffer - dwBuffSize;
   retVal &= 0x0000000FFFFFFFF;// import 32-bit to 64-bit
#ifdef VAC
   retVal = (DWORDLONG) ( (unsigned __int64) retVal | (((unsigned __int64) ptr) << 32) );
#else
   retVal |= (   (  ( DWORDLONG )( DWORD )ptr  ) << 32   );
#endif   
   return retVal;
  
  } else if (  *ptr == TEXT( '\r' ) && ptr[ 1 ] == TEXT( '\n' )  ) {
  
   *(ptr++) = '\n' ; // Substitue carriage return by newline.
   *ptr = 0 ;
   retVal = ptr + 1 - buffer - dwBuffSize;
   retVal &= 0x0000000FFFFFFFF;// import 32-bit to 64-bit
#ifdef VAC
   retVal = (DWORDLONG) ( (unsigned __int64) retVal | (((unsigned __int64) ptr) << 32) );
#else
   retVal |= (   (  ( DWORDLONG )( DWORD )ptr  ) << 32   );
#endif
   return retVal;
  
  } else if (  *ptr == TEXT( '\r' ) && ptr[ 1 ] == 0  ) {
    *ptr = '\n' ; // Substitue carriage return by newline

#ifdef VAC  
    return (DWORDLONG) (__int64) (-1);
#else
    return 0xFFFFFFFFFFFFFFFF;
#endif
  }
  ++ptr;
  
 }  // end while

#ifdef VAC
 retVal  = (DWORDLONG) ( ( (unsigned __int64) ((DWORD) buffer + dwBuffSize) ) << 32 );
 retVal = (DWORDLONG) ( (unsigned __int64) retVal & (((unsigned __int64) 0xFFFFFFFF) << 32) );
#else
 retVal  = (   (  ( DWORDLONG )( ( DWORD )buffer + dwBuffSize )  ) << 32   );
 retVal &= 0xFFFFFFFF00000000;
#endif

 return retVal;

}  // end _get_line

#else // PRO13471

static DWORDLONG __fastcall _get_line ( Standard_CString buffer, DWORD dwBuffSize ) {

 DWORDLONG        retVal;
 Standard_CString ptr;

 buffer[ dwBuffSize ] = 0;
 ptr                  = buffer;

 while ( *ptr != 0 ) {
 
  if (  *ptr == TEXT( '\n' )  ) {
  
   retVal = buffer - ptr;
#ifdef VAC
   buffer[ (__int64) -( LONGLONG )retVal ] = 0;
#else
   buffer[ -( LONGLONG )retVal ] = 0;
#endif
   retVal = ptr + 1 - buffer - dwBuffSize;
#ifdef VAC
   retVal = (DWORDLONG) ( (unsigned __int64) retVal | (((unsigned __int64) ptr) << 32) );
#else
   retVal |= (   (  ( DWORDLONG )( DWORD )ptr  ) << 32   );
#endif   
   return retVal;
  
  } else if (  *ptr == TEXT( '\r' ) && ptr[ 1 ] == TEXT( '\n' )  ) {
  
   retVal = buffer - ptr;   
#ifdef VAC
   buffer[ (__int64) -( LONGLONG )retVal ] = 0;
#else
   buffer[ -( LONGLONG )retVal ] = 0;
#endif
   retVal = ptr + 2 - buffer - dwBuffSize;
#ifdef VAC
   retVal = (DWORDLONG) ( (unsigned __int64) retVal | (((unsigned __int64) ptr) << 32) );
#else
   retVal |= (   (  ( DWORDLONG )( DWORD )ptr  ) << 32   );
#endif
   return retVal;
  
  } else if (  *ptr == TEXT( '\r' ) && ptr[ 1 ] == 0  )

#ifdef VAC  
   return (DWORDLONG) (__int64) (-1);
#else
   return 0xFFFFFFFFFFFFFFFF;
#endif

  ++ptr;
  
 }  // end while

#ifdef VAC
 retVal  = (DWORDLONG) ( ( (unsigned __int64) ((DWORD) buffer + dwBuffSize) ) << 32 );
 retVal = (DWORDLONG) ( (unsigned __int64) retVal & (((unsigned __int64) 0xFFFFFFFF) << 32) );
#else
 retVal  = (   (  ( DWORDLONG )( ( DWORD )buffer + dwBuffSize )  ) << 32   );
 retVal &= 0xFFFFFFFF00000000;
#endif

 return retVal;

}  // end _get_line

#endif // PRO13471

static int __fastcall _get_buffer (
                        HANDLE hChannel,
                        Standard_PCharacter& buffer, 
				   DWORD dwSize,
                        BOOL fPeek, BOOL fSocket
                       ) {

 int   retVal = -1;
 int   flags;
 DWORD dwDummy;
 DWORD dwBytesRead;

 if ( fSocket ) {
 
  flags = fPeek ? MSG_PEEK : 0;

  retVal = recv (  ( SOCKET )hChannel, buffer, ( int )dwSize, flags  );

  if ( retVal == SOCKET_ERROR ) retVal = -1;

 } else {
 
  if ( fPeek ) {
   
   if (  !PeekNamedPipe (
           hChannel, buffer, dwSize, &dwBytesRead, &dwDummy, &dwDummy
          ) && GetLastError () != ERROR_BROKEN_PIPE
   )

    retVal = -1;

   else

    retVal = ( int )dwBytesRead;
   
  } else {

   if (  !ReadFile ( hChannel, buffer, dwSize, &dwBytesRead, NULL )  )
       
    retVal = -1;

   else

    retVal = ( int )dwBytesRead;
   
  }  // end else
 
 }  // end else

 return retVal;

}  // end _get_buffer


static DWORD __fastcall _get_access_mask ( OSD_SingleProtection prt ) {

 DWORD retVal;

 switch ( prt ) {
 
  case OSD_None:

   retVal = 0;

  break;

  case OSD_R:

   retVal = FILE_GENERIC_READ;

  break;

  case OSD_W:

   retVal = FILE_GENERIC_WRITE;

  break;

  case OSD_RW:

   retVal = FILE_GENERIC_READ | FILE_GENERIC_WRITE;

  break;

  case OSD_X:

   retVal = FILE_GENERIC_EXECUTE;

  break;

  case OSD_RX:

   retVal = FILE_GENERIC_READ | FILE_GENERIC_EXECUTE;

  break;

  case OSD_WX:

   retVal = FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE;

  break;

  case OSD_RWX:

   retVal = FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE;

  break;

  case OSD_D:

   retVal = DELETE;

  break;

  case OSD_RD:

   retVal = FILE_GENERIC_READ | DELETE;

  break;

  case OSD_WD:

   retVal = FILE_GENERIC_WRITE | DELETE;

  break;

  case OSD_RWD:

   retVal = FILE_GENERIC_READ | FILE_GENERIC_WRITE | DELETE;

  break;

  case OSD_XD:

   retVal = FILE_GENERIC_EXECUTE | DELETE;

  break;

  case OSD_RXD:

   retVal = FILE_GENERIC_READ | FILE_GENERIC_EXECUTE | DELETE;

  break;

  case OSD_WXD:

   retVal = FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE;

  break;

  case OSD_RWXD:

   retVal = FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE;

  break;

  default:

   RAISE(  TEXT( "_get_access_mask (): incorrect parameter" )  );
 
 }  // end switch

 return retVal;

}  // end _get_access_mask

static DWORD __fastcall _get_dir_access_mask ( OSD_SingleProtection prt ) {

 DWORD retVal;

 switch ( prt ) {
 
  case OSD_None:

   retVal = 0;

  break;

  case OSD_R:

   retVal = GENERIC_READ;

  break;

  case OSD_W:

   retVal = GENERIC_WRITE;

  break;

  case OSD_RW:

   retVal = GENERIC_READ | GENERIC_WRITE;

  break;

  case OSD_X:

   retVal = GENERIC_EXECUTE;

  break;

  case OSD_RX:

   retVal = GENERIC_READ | GENERIC_EXECUTE;

  break;

  case OSD_WX:

   retVal = GENERIC_WRITE | GENERIC_EXECUTE;

  break;

  case OSD_RWX:

   retVal = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE;

  break;

  case OSD_D:

   retVal = DELETE;

  break;

  case OSD_RD:

   retVal = GENERIC_READ | DELETE;

  break;

  case OSD_WD:

   retVal = GENERIC_WRITE | DELETE;

  break;

  case OSD_RWD:

   retVal = GENERIC_READ | GENERIC_WRITE | DELETE;

  break;

  case OSD_XD:

   retVal = GENERIC_EXECUTE | DELETE;

  break;

  case OSD_RXD:

   retVal = GENERIC_READ | GENERIC_EXECUTE | DELETE;

  break;

  case OSD_WXD:

   retVal = GENERIC_WRITE | GENERIC_EXECUTE | DELETE;

  break;

  case OSD_RWXD:

   retVal = GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | DELETE;

  break;

  default:

   RAISE(  TEXT( "_get_dir_access_mask (): incorrect parameter" )  );
 
 }  // end switch

 return retVal;

}  // end _get_dir_access_mask

static HANDLE __fastcall _open_file (
                          Standard_CString fName,
                          OSD_OpenMode oMode,
                          DWORD dwOptions, LPBOOL fNew
                         ) {

 HANDLE retVal = INVALID_HANDLE_VALUE;
 DWORD  dwDesiredAccess;
 DWORD  dwCreationDistribution;

 switch ( oMode ) {
  
  case OSD_ReadOnly:

   dwDesiredAccess = GENERIC_READ;

  break;

  case OSD_WriteOnly:

   dwDesiredAccess = GENERIC_WRITE;

  break;

  case OSD_ReadWrite:

   dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;

  break;

  default:

   RAISE(  TEXT( "_open_file (): incorrect parameter" )  );
  
 }  // end switch

 dwCreationDistribution = ( dwOptions != OPEN_NEW ) ? OPEN_EXISTING : CREATE_ALWAYS;

 retVal = CreateFile (
           fName, dwDesiredAccess,
           FILE_SHARE_READ | FILE_SHARE_WRITE,
           NULL, dwCreationDistribution, FILE_ATTRIBUTE_NORMAL, NULL
          );

 if ( retVal          == INVALID_HANDLE_VALUE &&
      dwOptions       == OPEN_APPEND          &&
      GetLastError () == ERROR_FILE_NOT_FOUND
 ) {

 
  dwCreationDistribution = CREATE_ALWAYS;
  
  retVal = CreateFile (
            fName, dwDesiredAccess,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, dwCreationDistribution, FILE_ATTRIBUTE_NORMAL, NULL
           );

  *fNew = TRUE;

 }  // end if

 return retVal;

}  // end _open_file

Standard_Integer __fastcall _get_file_type (
                             Standard_CString fName, Standard_Integer fileHandle
                            ) {

 Standard_Integer retVal;
 DWORD            dwType;
 int              fileType;

 fileType = fileHandle == ( Standard_Integer )INVALID_HANDLE_VALUE ?
                          FILE_TYPE_DISK :
                          GetFileType (  ( HANDLE )fileHandle  );

 switch ( fileType ) {
 
  case FILE_TYPE_UNKNOWN:

   retVal = FLAG_SOCKET;

  break;

  case FILE_TYPE_DISK:

   if (   (  dwType = GetFileAttributes ( fName )  ) != 0xFFFFFFFF  )

    retVal =  dwType & FILE_ATTRIBUTE_DIRECTORY ? FLAG_DIRECTORY : FLAG_FILE;

   else

    retVal = 0x80000000;

  break;

  case FILE_TYPE_CHAR:

   retVal = FLAG_DEVICE;

  break;

  case FILE_TYPE_PIPE:

   retVal = FLAG_PIPE;

  break;
 
 }  // end switch

 return retVal;

}  // end _get_file_type

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return retVal
#endif

BOOL __fastcall _osd_wnt_sd_to_protection (
                 PSECURITY_DESCRIPTOR pSD, OSD_Protection& prot, BOOL fDir
                ) {

 int           i;
 BOOL          fPresent;
 BOOL          fDefaulted;
 PACL          pACL;
 PSID          pSIDowner;
 PSID          pSIDadmin;
 PSID          pSIDworld;
 LPVOID        pACE;
 DWORD         dwAccessOwner = 0;
 DWORD         dwAccessGroup = 0;
 DWORD         dwAccessAdmin = 0;
 DWORD         dwAccessWorld = 0;
 BOOL          retVal = FALSE;
 GET_PROT_FUNC _get_prot_func = fDir ? &_get_protection_dir : &_get_protection;

 __try {

  if (  !GetSecurityDescriptorOwner ( pSD, &pSIDowner, &fDefaulted )  ) __leave;

  if (  !GetSecurityDescriptorDacl ( pSD, &fPresent, &pACL, &fDefaulted ) ||
        !fPresent
  ) __leave;

  if ( pSIDowner == NULL || pACL == NULL ) {
  
   SetLastError ( ERROR_NO_SECURITY_ON_OBJECT );
   __leave;
  
  }  // end if
 
  pSIDadmin = AdminSid ();
  pSIDworld = WorldSid ();

  for ( i = 0; i < ( int )pACL -> AceCount; ++i ) {
  
   if (  GetAce ( pACL, i, &pACE )  ) {
   
    if (   EqualSid (  pSIDowner, GET_SID( pACE )  )   )

     dwAccessOwner = (  ( PACE_HEADER )pACE  ) -> AceType == ACCESS_DENIED_ACE_TYPE ?
                     0 : *GET_MSK( pACE );

    else if (   EqualSid (  pSIDadmin, GET_SID( pACE )  )   )

     dwAccessAdmin = (  ( PACE_HEADER )pACE  ) -> AceType == ACCESS_DENIED_ACE_TYPE ?
                     0 : *GET_MSK( pACE );

    else if (   EqualSid (  pSIDworld, GET_SID( pACE )  )   )

     dwAccessWorld = (  ( PACE_HEADER )pACE  ) -> AceType == ACCESS_DENIED_ACE_TYPE ?
                     0 : *GET_MSK( pACE );

    else

     dwAccessGroup = (  ( PACE_HEADER )pACE  ) -> AceType == ACCESS_DENIED_ACE_TYPE ?
                     0 : *GET_MSK( pACE );
   
   }  // end if
  
  }  // end for

  prot.SetValues (
        ( *_get_prot_func ) ( dwAccessAdmin ),
        ( *_get_prot_func ) ( dwAccessOwner ),
        ( *_get_prot_func ) ( dwAccessGroup ),
        ( *_get_prot_func ) ( dwAccessWorld )
       );

  retVal = TRUE;
  
 }  // end __try

 __finally {}
       
#ifdef VAC
leave: ;      // added for VisualAge
#endif

 return retVal;

}  // end _osd_wnt_sd_to_protection

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

static OSD_SingleProtection __fastcall _get_protection ( DWORD mask ) {

 OSD_SingleProtection retVal;

 switch ( mask ) {
 
  case FILE_GENERIC_READ:

   retVal = OSD_R;

  break;

  case FILE_GENERIC_WRITE:

   retVal = OSD_W;

  break;

  case FILE_GENERIC_READ | FILE_GENERIC_WRITE:

   retVal = OSD_RW;

  break;

  case FILE_GENERIC_EXECUTE:

   retVal = OSD_X;

  break;

  case FILE_GENERIC_READ | FILE_GENERIC_EXECUTE:

   retVal = OSD_RX;

  break;

  case FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE:

   retVal = OSD_WX;

  break;

  case FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE:

   retVal = OSD_RWX;

  break;

  case DELETE:

   retVal = OSD_D;

  break;

  case FILE_GENERIC_READ | DELETE:

   retVal = OSD_RD;

  break;

  case FILE_GENERIC_WRITE | DELETE:

   retVal = OSD_WD;

  break;

  case FILE_GENERIC_READ | FILE_GENERIC_WRITE | DELETE:

   retVal = OSD_RWD;

  break;

  case FILE_GENERIC_EXECUTE | DELETE:

   retVal = OSD_XD;

  break;

  case FILE_GENERIC_READ | FILE_GENERIC_EXECUTE | DELETE:

   retVal = OSD_RXD;

  break;

  case FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE:

   retVal = OSD_WXD;

  break;

  case FILE_ALL_ACCESS:
  case FILE_GENERIC_READ | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE | DELETE:

   retVal = OSD_RWXD;

  break;

  case 0:
  default:

   retVal = OSD_None;
 
 }  // end switch

 return retVal;

}  // end _get_protection

static OSD_SingleProtection __fastcall _get_protection_dir ( DWORD mask ) {

 OSD_SingleProtection retVal;

 switch ( mask ) {
 
  case GENERIC_READ:

   retVal = OSD_R;

  break;

  case GENERIC_WRITE:

   retVal = OSD_W;

  break;

  case GENERIC_READ | GENERIC_WRITE:

   retVal = OSD_RW;

  break;

  case GENERIC_EXECUTE:

   retVal = OSD_X;

  break;

  case GENERIC_READ | GENERIC_EXECUTE:

   retVal = OSD_RX;

  break;

  case GENERIC_WRITE | GENERIC_EXECUTE:

   retVal = OSD_WX;

  break;

  case GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE:

   retVal = OSD_RWX;

  break;

  case DELETE:

   retVal = OSD_D;

  break;

  case GENERIC_READ | DELETE:

   retVal = OSD_RD;

  break;

  case GENERIC_WRITE | DELETE:

   retVal = OSD_WD;

  break;

  case GENERIC_READ | GENERIC_WRITE | DELETE:

   retVal = OSD_RWD;

  break;

  case GENERIC_EXECUTE | DELETE:

   retVal = OSD_XD;

  break;

  case GENERIC_READ | GENERIC_EXECUTE | DELETE:

   retVal = OSD_RXD;

  break;

  case GENERIC_WRITE | GENERIC_EXECUTE | DELETE:

   retVal = OSD_WXD;

  break;

  case FILE_ALL_ACCESS:
  case GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | DELETE:

   retVal = OSD_RWXD;

  break;

  case 0:
  default:

   retVal = OSD_None;
 
 }  // end switch

 return retVal;

}  // end _get_protection_dir

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#define __try
#define __finally
#define __leave return fOK
#endif

BOOL __fastcall _osd_print (const Standard_PCharacter pName, Standard_CString fName ) {

 BOOL   fOK, fJob;                
 HANDLE hPrinter;
 BYTE   jobInfo[ MAX_PATH + sizeof ( DWORD ) ];
 DWORD  dwNeeded, dwCode;

 fOK = fJob = FALSE;

 __try {
 
  if (  !OpenPrinter ( Standard_PCharacter(pName), &hPrinter, NULL )  ) {
  
   hPrinter = NULL;
   __leave;
  
  }  // end if

  if (   !AddJob (
           hPrinter, 1, jobInfo, MAX_PATH + sizeof ( DWORD ), &dwNeeded
          )
  ) __leave;

  fJob = TRUE;

  if (  !CopyFile (
          fName, (  ( ADDJOB_INFO_1* )jobInfo  ) -> Path, FALSE
         )
  ) __leave;

  if (  !ScheduleJob (
          hPrinter, (  ( ADDJOB_INFO_1* )jobInfo  ) -> JobId
         )
  ) __leave;
  
  fOK = TRUE;
 
 }  // end __try

 __finally {
 
  if ( !fOK ) {
  
   BYTE  info[ 1024 ];
   DWORD dwBytesNeeded;

   dwCode = GetLastError ();

   if ( fJob && hPrinter != NULL ) {

    GetJob (
     hPrinter, (  ( ADDJOB_INFO_1* )jobInfo  ) -> JobId, 1, 
     info, 1024, &dwBytesNeeded
    );

    if ( fJob ) SetJob (
                 hPrinter,
                 (  ( ADDJOB_INFO_1* )jobInfo  ) -> JobId,
                 1, info, JOB_CONTROL_CANCEL
                );

   }  // end if

  }  // end if

  if ( hPrinter != NULL ) ClosePrinter ( hPrinter );
 
 }  // end __finally

#ifdef VAC
leave: ;       // added for VisualAge
#endif

 if ( !fOK ) SetLastError ( dwCode );

 return fOK;
                
}  // end _osd_print

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

Standard_Boolean OSD_File::IsReadable()
{
  TCollection_AsciiString FileName ;
  HANDLE Channel ;

  myPath.SystemName(FileName) ;
  Channel = _open_file(FileName.ToCString(), OSD_ReadOnly, OPEN_OLD) ;
  if (Channel == INVALID_HANDLE_VALUE)
    return Standard_False ;
  else {
    CloseHandle (Channel) ;
    return Standard_True ;
  }
}


Standard_Boolean OSD_File::IsWriteable()
{
  TCollection_AsciiString FileName ;
  HANDLE Channel ;

  myPath.SystemName(FileName) ;
  Channel = _open_file(FileName.ToCString(), OSD_ReadWrite, OPEN_OLD) ;
  if (Channel == INVALID_HANDLE_VALUE)
    return Standard_False ;
  else {
    CloseHandle (Channel) ;
    return Standard_True ;
  }
}

Standard_Boolean OSD_File::IsExecutable()
{
  return IsReadable() ;

//  if (_access(FileName.ToCString(),0))
}

#endif // WNT


// ---------------------------------------------------------------------
// Read lines in a file while it is increasing.
// Each line is terminated with a <nl>.
// ---------------------------------------------------------------------

#include <OSD.hxx>

Standard_Boolean OSD_File::ReadLastLine(TCollection_AsciiString& aLine,const Standard_Integer aDelay,const Standard_Integer aNbTries)
{
  static Standard_Integer MaxLength = 1000 ;
  Standard_Integer Len ;
  Standard_Integer Count = aNbTries ;

  if (Count <= 0)
      return Standard_False ;
  while(1) {
    ReadLine(aLine, MaxLength, Len) ;
    if (!aLine.IsEmpty()) 
      return Standard_True ;
    if (!--Count)
      return Standard_False ;
    OSD::SecSleep(aDelay) ;
  }
}


Standard_Boolean OSD_File::Edit()
{
  cout << "Function OSD_File::Edit() not yet implemented." << endl;
  return Standard_False ;
}




