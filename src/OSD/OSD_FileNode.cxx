// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _WIN32

//----------------------------------------------------------------------------
//------------------- Linux Sources of OSD_FileNode --------------------------
//----------------------------------------------------------------------------

#include <OSD_FileNode.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <OSD_WhoAmI.hxx>
#include <Quantity_Date.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ProgramError.hxx>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
// For "system"
const OSD_WhoAmI Iam = OSD_WFileNode;


// Create a file/directory object

OSD_FileNode::OSD_FileNode ()
{
}

// Create and initialize a file/directory object

OSD_FileNode::OSD_FileNode (const OSD_Path& Name)
{
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
//  throw OSD_OSDError("OSD_FileNode::Exists : no name was given"); (LD)

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
//  throw OSD_OSDError("OSD_FileNode::Remove : no name was given"); (LD)

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
//  throw OSD_OSDError("OSD_FileNode::Move : no name was given");

// if (Failed()) Perror();

 NewPath.SystemName( thisPath );        // Get internal path name
 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status = rename (aBuffer.ToCString(), thisPath.ToCString());

 if (status == -1) myError.SetValue (errno, Iam, "Move");
}

// Copy a file to another path and name
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

void  OSD_FileNode::Copy(const OSD_Path& ToPath)
{
int status;
TCollection_AsciiString second_name;

// if (myPath.Name().Length()==0)   Copy .login would raise !!
//  throw OSD_OSDError("OSD_FileNode::Copy : no name was given");
// if (Failed()) Perror();

 ToPath.SystemName (second_name);

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status =  copy_file(aBuffer.ToCString(), second_name.ToCString());
 if (status != 0) myError.SetValue (-1, Iam, "Copy failed") ;// (LD)
#ifdef OCCT_DEBUG
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
//  throw OSD_OSDError("OSD_FileNode::Protection : no name was given");

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
//  throw OSD_OSDError("OSD_FileNode::SetProtection : no name was given");

// if (Failed()) Perror();

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 status = chmod (aBuffer.ToCString(), (mode_t)Prot.Internal() );
 if (status == -1) myError.SetValue (errno, Iam, "SetProtection");
}

// return the date of last access of file/directory

Quantity_Date  OSD_FileNode::CreationMoment(){

 Quantity_Date result;
 struct tm *decode;
 struct stat buffer;

// if (myPath.Name().Length()==0)
//  throw OSD_OSDError("OSD_FileNode::CreationMoment : no name was given");

// if (Failed()) Perror();

 /* Get File Informations */

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 if (!stat ( aBuffer.ToCString(), &buffer )) {
   time_t aTime = (time_t)buffer.st_ctime;
   decode = localtime (&aTime);
   result.SetValues (decode->tm_mon+1, decode->tm_mday, decode->tm_year+1900,
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
//  throw OSD_OSDError("OSD_FileNode::AccessMoment : no name was given");

// if (Failed()) Perror();

 /* Get File Informations */

 TCollection_AsciiString aBuffer;
 myPath.SystemName ( aBuffer );
 if (!stat ( aBuffer.ToCString(), &buffer )) {
   time_t aTime = (time_t)buffer.st_ctime;
   decode = localtime (&aTime);
   result.SetValues (decode->tm_mon+1, decode->tm_mday, decode->tm_year+1900,
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

#else /* _WIN32 */

//----------------------------------------------------------------------------
//-------------------  WNT Sources of OSD_FileNode ---------------------------
//----------------------------------------------------------------------------

#ifdef NONLS
#undef NONLS
#endif
#include <windows.h>

#include <OSD_FileNode.hxx>
#include <OSD_Protection.hxx>
#include <Quantity_Date.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_ExtendedString.hxx>

#include <OSD_WNT.hxx>

#ifndef _INC_TCHAR
# include <tchar.h>
#endif  // _INC_TCHAR

#include <Strsafe.h>

#define TEST_RAISE( arg ) _test_raise (  fName, ( arg )  )
#define RAISE( arg ) throw Standard_ProgramError (  ( arg )  )

#ifndef OCCT_UWP
// None of the existing security APIs are supported in a UWP applications
PSECURITY_DESCRIPTOR __fastcall _osd_wnt_protection_to_sd ( const OSD_Protection&, BOOL, const wchar_t* );
BOOL                 __fastcall _osd_wnt_sd_to_protection (
                                 PSECURITY_DESCRIPTOR pSD, OSD_Protection& prot, BOOL
                                );
#endif
Standard_Integer     __fastcall _get_file_type ( Standard_CString, HANDLE );

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

static BOOL __fastcall _get_file_time (const wchar_t*, LPSYSTEMTIME, BOOL );
static void __fastcall _test_raise ( TCollection_AsciiString, Standard_CString );

//=======================================================================
//function : OSD_FileNode
//purpose  : Empty Constructor
//=======================================================================

OSD_FileNode::OSD_FileNode () 
{
}

//=======================================================================
//function : OSD_FileNode
//purpose  : Constructor
//=======================================================================

OSD_FileNode::OSD_FileNode ( const OSD_Path& Name )
{
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

Standard_Boolean OSD_FileNode::Exists ()
{
  myError.Reset();

 Standard_Boolean        retVal = Standard_False;;
 TCollection_AsciiString fName;

 myPath.SystemName ( fName );

 if (  fName.IsEmpty ()  ) return Standard_False;
 TEST_RAISE(  "Exists"  );

 // make wide character string from UTF-8
 TCollection_ExtendedString fNameW(fName);

 WIN32_FILE_ATTRIBUTE_DATA aFileInfo;

 if (!GetFileAttributesExW (fNameW.ToWideString(), GetFileExInfoStandard, &aFileInfo))
 {
  if (GetLastError() != ERROR_FILE_NOT_FOUND)
  {
    _osd_wnt_set_error (myError, OSD_WFileNode, fNameW.ToWideString());
  }
 }
 else
 {
  retVal = Standard_True;
 }

 return retVal;

}  // end OSD_FileNode :: Exists

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void OSD_FileNode::Remove () {

 TCollection_AsciiString fName;

 myPath.SystemName ( fName );
 TCollection_ExtendedString fNameW(fName);

 TEST_RAISE(  "Remove"  );

 switch (_get_file_type (fName.ToCString(), INVALID_HANDLE_VALUE)) {

  case FLAG_FILE:

   if (!DeleteFileW (fNameW.ToWideString()))
     _osd_wnt_set_error (  myError, OSD_WFileNode, fNameW.ToWideString());
  break;

  case FLAG_DIRECTORY:


// LD : Suppression de l'appel a DeleteDirectory pour 
//      ne pas detruire un repertoire no vide.

   if (!RemoveDirectoryW (fNameW.ToWideString()))
     _osd_wnt_set_error (myError, OSD_WFileNode, fNameW.ToWideString());
  break;

  default:
   RAISE("OSD_FileNode :: Remove (): invalid file type - neither file nor directory");
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
 TCollection_ExtendedString fNameW(fName);

 TEST_RAISE(  "Move"  );

 NewPath.SystemName ( fNameDst );
 TCollection_ExtendedString fNameDstW(fNameDst);

 switch (_get_file_type (fName.ToCString (), INVALID_HANDLE_VALUE)) {

  case FLAG_FILE:

   if (!MoveFileExW (fNameW.ToWideString (),
                     fNameDstW.ToWideString (),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED))
     _osd_wnt_set_error(myError, OSD_WFileNode, fNameW.ToWideString(), fNameDstW.ToWideString());
  break;

  case FLAG_DIRECTORY:

   if (!MoveDirectory (fNameW.ToWideString(), fNameDstW.ToWideString()))
   _osd_wnt_set_error(myError, OSD_WFileNode, fNameW.ToWideString(), fNameDstW.ToWideString());
  break;

  default:
   RAISE("OSD_FileNode :: Move (): invalid file type - neither file nor directory");
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
 TCollection_ExtendedString fNameW(fName);

 TEST_RAISE(  "Copy"  );

 ToPath.SystemName ( fNameDst );
 TCollection_ExtendedString fNameDstW(fNameDst);

 switch (_get_file_type (fName.ToCString(), INVALID_HANDLE_VALUE)) {

  case FLAG_FILE:
#ifndef OCCT_UWP
    if (!CopyFileW (fNameW.ToWideString(), fNameDstW.ToWideString(), FALSE))
#else
   if (CopyFile2 (fNameW.ToWideString(), fNameDstW.ToWideString(), FALSE) != S_OK)
#endif
   _osd_wnt_set_error (myError, OSD_WFileNode, fNameW.ToWideString(), fNameDstW.ToWideString());
  break;

  case FLAG_DIRECTORY:

   if (!CopyDirectory (fNameW.ToWideString(), fNameDstW.ToWideString()))
   _osd_wnt_set_error (myError, OSD_WFileNode, fNameW.ToWideString(), fNameDstW.ToWideString());

  break;

  default:
   RAISE("OSD_FileNode :: Copy (): invalid file type - neither file nor directory");

 }  // end switch

}  // end OSD_FileNode :: Copy

// None of the existing security APIs are supported in a UWP applications
#ifndef OCCT_UWP

//=======================================================================
//function : Protection
//purpose  : 
//=======================================================================

OSD_Protection OSD_FileNode::Protection () {

 OSD_Protection          retVal;
 TCollection_AsciiString fName;
 PSECURITY_DESCRIPTOR    pSD;

 myPath.SystemName ( fName );
 TCollection_ExtendedString fNameW(fName);

 TEST_RAISE(  "Protection"  );

 if ((pSD = GetFileSecurityEx (fNameW.ToWideString(), DACL_SECURITY_INFORMATION |OWNER_SECURITY_INFORMATION)) == NULL
  || !_osd_wnt_sd_to_protection (
          pSD, retVal,
          _get_file_type (fName.ToCString(), INVALID_HANDLE_VALUE) == FLAG_DIRECTORY)
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
 TCollection_ExtendedString fNameW(fName);

 TEST_RAISE(  "SetProtection"  );

 pSD = _osd_wnt_protection_to_sd (Prot,
        _get_file_type (fName.ToCString(), INVALID_HANDLE_VALUE) == FLAG_DIRECTORY,
        fNameW.ToWideString());
 
 if (pSD == NULL || !SetFileSecurityW (fNameW.ToWideString(), DACL_SECURITY_INFORMATION, pSD))
  _osd_wnt_set_error (myError, OSD_WFileNode, fNameW.ToWideString());

 if ( pSD != NULL )

  FreeSD ( pSD );

}  // end OSD_FileNode :: SetProtection

#else /* UWP stub */

#include <io.h>

//=======================================================================
//function : Protection
//purpose  : 
//=======================================================================

OSD_Protection OSD_FileNode::Protection ()
{
 TCollection_AsciiString fName;

 myPath.SystemName ( fName );
 TCollection_ExtendedString fNameW(fName);

 OSD_SingleProtection aProt = OSD_None;
 if (_waccess_s (fNameW.ToWideString(), 6))
   aProt = OSD_RW;
 else if (_waccess_s (fNameW.ToWideString(), 2))
   aProt = OSD_W;
 else if (_waccess_s (fNameW.ToWideString(), 4))
   aProt = OSD_R;

 // assume full access for system and none for everybody
 OSD_Protection retVal (OSD_RWXD, aProt, aProt, OSD_None);
 return retVal;
}  // end OSD_FileNode :: Protection

//=======================================================================
//function : SetProtection
//purpose  : 
//=======================================================================

void OSD_FileNode::SetProtection ( const OSD_Protection& /*Prot*/ )
{
}  // end OSD_FileNode :: SetProtection

#endif

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
 TCollection_ExtendedString fNameW(fName);

 TEST_RAISE(  "AccessMoment"  );

 if (_get_file_time (fNameW.ToWideString(), &stAccessSystemMoment, TRUE))
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
 {
  _osd_wnt_set_error (myError, OSD_WFileNode, fNameW.ToWideString());
 }

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
 TCollection_ExtendedString fNameW(fName);

 TEST_RAISE(  "CreationMoment"  );

 if (_get_file_time (fNameW.ToWideString(), &stCreationSystemMoment, TRUE))
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
 {
  _osd_wnt_set_error (myError, OSD_WFileNode, fNameW.ToWideString());
 }

 return retVal;

}  // end OSD_FileNode :: CreationMoment

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

 wchar_t buffer[2048];

 va_list            arg_ptr;

 va_start ( arg_ptr, who);

 errCode = GetLastError ();

 if (  !FormatMessageW (
         FORMAT_MESSAGE_FROM_SYSTEM,
         0, errCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ),
         buffer, 2048, &arg_ptr
        )
 ) {
  StringCchPrintfW(buffer, _countof(buffer), L"error code %d", (Standard_Integer)errCode);

  SetLastError ( errCode );

 }  // end if

 char aBufferA[2048];
 WideCharToMultiByte(CP_UTF8, 0, buffer, -1, aBufferA, sizeof(aBufferA), NULL, NULL);
 err.SetValue(errCode, who, aBufferA);

 va_end ( arg_ptr );

}  // end _set_error

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#ifdef __try /* is defined on MinGw as either "try" or "if (true)" */
#undef __try
#endif
#define __try
#define __finally
#define __leave return retVal
#endif

static BOOL __fastcall _get_file_time (const wchar_t* fName, LPSYSTEMTIME lpSysTime, BOOL fAccess)
{
 BOOL       retVal = FALSE;
 FILETIME   ftCreationTime;
 FILETIME   ftLastWriteTime;
 LPFILETIME lpftPtr;
 HANDLE     hFile = INVALID_HANDLE_VALUE;

 __try {
#ifndef OCCT_UWP
   if ((hFile = CreateFileW (fName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)
     ) == INVALID_HANDLE_VALUE
     )
#else
   CREATEFILE2_EXTENDED_PARAMETERS pCreateExParams = {};
   pCreateExParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
   pCreateExParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
   pCreateExParams.lpSecurityAttributes = NULL;
   pCreateExParams.hTemplateFile = NULL;
   if ((hFile = CreateFile2 (fName, NULL, NULL, OPEN_EXISTING, &pCreateExParams)) == INVALID_HANDLE_VALUE)
#endif
    __leave;

  if (  !GetFileTime ( hFile, &ftCreationTime, NULL, &ftLastWriteTime )  ) __leave;

  lpftPtr = fAccess ? &ftLastWriteTime : &ftCreationTime;

  if (  !FileTimeToSystemTime ( lpftPtr, lpSysTime )  ) __leave;

  retVal = TRUE;

 }  // end __try

 __finally {
 
  if ( hFile != INVALID_HANDLE_VALUE )

   CloseHandle ( hFile );
 
 }  // end __finally

 return retVal;

}  // end _get_file_time

#if defined(__CYGWIN32__) || defined(__MINGW32__)
#undef __try
#undef __finally
#undef __leave
#endif

static void __fastcall _test_raise ( TCollection_AsciiString fName, Standard_CString str ) {
 if (  fName.IsEmpty ()  ) {
   TCollection_AsciiString buff = "OSD_FileNode :: ";
   buff += str;
   buff += " (): wrong access";

   throw Standard_ProgramError(buff.ToCString());
 }  // end if

}  // end _test_raise

#endif /* _WIN32 */
