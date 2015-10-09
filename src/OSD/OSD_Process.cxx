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


#include <OSD_Environment.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <OSD_Process.hxx>
#include <OSD_WhoAmI.hxx>
#include <Quantity_Date.hxx>
#include <TCollection_AsciiString.hxx>

const OSD_WhoAmI Iam = OSD_WProcess;

#include <errno.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/time.h>
#include <pwd.h>       // For command getpwuid
#include <unistd.h>

OSD_Process::OSD_Process(){
}


void OSD_Process::Spawn (const TCollection_AsciiString& cmd,
			 const Standard_Boolean /*ShowWindow*/)
{
 system(cmd.ToCString());
}


void OSD_Process::TerminalType(TCollection_AsciiString& Name){
TCollection_AsciiString which="TERM";
OSD_Environment term (which,"");

 term.Value();
 which = term.Value();
 Name = term.Name();
}


// Get date of system date

Quantity_Date  OSD_Process::SystemDate(){
Quantity_Date result;
Standard_Integer month=0,day=0,year=0,hh=0,mn=0,ss=0;
struct tm transfert;
struct timeval tval;
struct timezone tzone;
int status;

 status = gettimeofday( &tval, &tzone );
 if (status == -1) myError.SetValue (errno, Iam, "GetSystem");
 else {
  memcpy(&transfert, localtime((time_t *)&tval.tv_sec), sizeof(struct
tm));
  month = transfert.tm_mon + 1;  // Add to January (month #1)
  day   = transfert.tm_mday;
  year  = transfert.tm_year;
  hh    = transfert.tm_hour;
  mn    = transfert.tm_min ;
  ss    = transfert.tm_sec ;
}

 result.SetValues ( month, day, year+1900, hh, mn, ss);
 return (result);
}


Standard_Integer OSD_Process::ProcessId(){
 return (getpid());
}

TCollection_AsciiString OSD_Process::UserName(){
 struct passwd *infos;
 infos = getpwuid(getuid()); 
 TCollection_AsciiString result=infos->pw_name;

 return(result);
}

Standard_Boolean OSD_Process::IsSuperUser (){
  if (getuid()) {
    return Standard_False;
  }
  else {
    return Standard_True;
  }
}


OSD_Path OSD_Process::CurrentDirectory(){
char cwd[MAXPATHLEN+1] ;
OSD_Path result;
TCollection_AsciiString Name;

 if (!getcwd(cwd,MAXPATHLEN+1))
   myError.SetValue (errno, Iam, "Where");
 else {
   Name = cwd;

//   JPT : August,20 1993. This code has been replaced by #ifdef ... #endif
//   position = Name.SearchFromEnd(".");
//   if (position != -1){
//     Ext = Name;
//     Ext.Remove(1,position);
//     Name.Remove( position,Ext.Length()+1);
//   }
//   result.SetValues("","","","","",Name,Ext);
//   End

#if defined(vax) || defined(__vms)
   Standard_Integer iDisk = Name.Search(":");
   if (iDisk){
     TCollection_AsciiString Disk;
     TCollection_AsciiString Directory;
     Disk = Name.SubString(1,iDisk-1);
     Directory = Name.SubString(iDisk+1,Name.Length());
     result.SetValues("","","",Disk,Directory,"","");
   }
#else
   Name += TCollection_AsciiString("/");
   result = OSD_Path(Name);
   //      result.SetValues("","","","",Name,"","");
#endif

 }
return (result);
}


void OSD_Process::SetCurrentDirectory(const OSD_Path& where){
TCollection_AsciiString Name;
int status;

 where.SystemName(Name);

 status = chdir (Name.ToCString());
 if (status == -1) myError.SetValue(errno, Iam, "Move to directory");
}


void OSD_Process::Reset(){
 myError.Reset();
}

Standard_Boolean OSD_Process::Failed()const{
 return( myError.Failed());
}

void OSD_Process::Perror() {
 myError.Perror();
}


Standard_Integer OSD_Process::Error()const{
 return( myError.Error());
}

#else

//------------------------------------------------------------------------
//-------------------  WNT Sources of OSD_Path ---------------------------
//------------------------------------------------------------------------

//it is important to undefine NOUSER and enforce including <windows.h> before
//Standard_Macro.hxx defines it and includes <windows.h> causing compilation errors
#ifdef NOUSER
#undef NOUSER /* we need SW_HIDE from windows.h */
#endif
#include <windows.h>

#ifdef SetCurrentDirectory
# undef SetCurrentDirectory /* undefine SetCurrentDirectory from <winbase.h> to correctly include <OSD_Process.hxx> */
#endif
#include <OSD_Process.hxx>

#include <OSD_Path.hxx>
#include <Quantity_Date.hxx>
#include <Standard_PExtCharacter.hxx>
#include <TCollection_ExtendedString.hxx>

#include <OSD_WNT_1.hxx>
#include <LMCONS.H> /// pour UNLEN  ( see MSDN about GetUserName() )


#pragma warning( disable : 4700 )

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

OSD_Process :: OSD_Process () {

}  // end constructor

void OSD_Process :: Spawn ( const TCollection_AsciiString& cmd ,
			    const Standard_Boolean ShowWindow /* = Standard_True */) {

 STARTUPINFO         si;
 PROCESS_INFORMATION pi;

 ZeroMemory (  &si, sizeof ( STARTUPINFO )  );

 si.cb = sizeof ( STARTUPINFO );
 //============================================
 //---> Added by Stephane Routelous ( stephane.routelous@altavista.net )	[16.03.01]
 //---> Reason : to allow to hide the window
 if ( !ShowWindow )
 {
	 si.dwFlags		= STARTF_USESHOWWINDOW;
	 si.wShowWindow	= SW_HIDE;	
 }
 //<--- End Added by Stephane Routelous ( stephane.routelous@altavista.net )	[16.03.01]
 //============================================

 if (!CreateProcess (
      NULL, (char *)cmd.ToCString (), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi
                    )
 )

  _osd_wnt_set_error ( myError, OSD_WProcess );

 else {
 
  CloseHandle ( pi.hThread );

  WaitForSingleObject ( pi.hProcess, INFINITE );

  CloseHandle ( pi.hProcess );
 
 }  // end else

}  // end OSD_Process :: Spawn

void OSD_Process :: TerminalType ( TCollection_AsciiString& Name ) {

 Name = "WIN32 console";

}  // end OSD_Process :: TerminalType

Quantity_Date OSD_Process :: SystemDate () {

 Quantity_Date retVal;
 SYSTEMTIME    st;

 GetLocalTime ( &st );

 retVal.SetValues (
         st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
        );

 return retVal;

}  // end OSD_Process :: SystemDate

TCollection_AsciiString OSD_Process :: UserName () 
{
	Standard_PCharacter pBuff = new char[UNLEN + 1];
	DWORD                   dwSize = UNLEN + 1;
	TCollection_AsciiString retVal;
	if (  !GetUserName ( pBuff, &dwSize )  )
	{
		_osd_wnt_set_error ( myError, OSD_WProcess );
	}
	else
	{
		TCollection_AsciiString theTmpUserName(pBuff,(int)dwSize -1 );
		retVal = theTmpUserName;
	}
	delete [] pBuff;
	return retVal;
}  // end OSD_Process :: UserName

Standard_Boolean OSD_Process :: IsSuperUser () {

 Standard_Boolean retVal = FALSE;
 PSID             pSIDadmin;
 HANDLE           hProcessToken = INVALID_HANDLE_VALUE;
 PTOKEN_GROUPS    pTKgroups = NULL;

 if (  !OpenProcessToken (
         GetCurrentProcess (),
         TOKEN_QUERY, &hProcessToken
        ) ||
        (  pTKgroups = ( PTOKEN_GROUPS )GetTokenInformationEx (
                                         hProcessToken, TokenGroups
                                        )
        ) == NULL
 )

  _osd_wnt_set_error ( myError, OSD_WProcess );

 else {
 
  pSIDadmin = AdminSid ();

  for ( int i = 0; i < ( int )pTKgroups -> GroupCount; ++i )

   if (  EqualSid ( pTKgroups -> Groups[ i ].Sid, pSIDadmin )  ) {
   
    retVal = TRUE;
    break;
   
   }  // end if
 
 }  // end else

 if ( hProcessToken != INVALID_HANDLE_VALUE ) CloseHandle ( hProcessToken );
 if ( pTKgroups     != NULL                 ) FreeTokenInformation ( pTKgroups );

 return retVal;

}  // end OSD_Process :: IsSuperUser

Standard_Integer OSD_Process :: ProcessId () {

 return ( Standard_Integer )GetCurrentProcessId ();

}  // end OSD_Process :: ProcessId

OSD_Path OSD_Process :: CurrentDirectory () {

  OSD_Path anCurrentDirectory;

  DWORD dwSize = PATHLEN + 1;
  Standard_WideChar* pBuff = new wchar_t[dwSize];

  if ( GetCurrentDirectoryW(dwSize, (wchar_t*)pBuff) > 0 )
  {
    // conversion to UTF-8 is performed inside
    TCollection_AsciiString aPath(TCollection_ExtendedString((Standard_ExtString)pBuff));
    anCurrentDirectory = OSD_Path ( aPath );
  }
  else
    _osd_wnt_set_error ( myError, OSD_WProcess );
 
  delete[] pBuff;
  return anCurrentDirectory;
}  // end OSD_Process :: CurrentDirectory

void OSD_Process :: SetCurrentDirectory ( const OSD_Path& where ) {

 TCollection_AsciiString path;

 where.SystemName ( path );
 TCollection_ExtendedString pathW(path);

 if (   !::SetCurrentDirectoryW ( (const wchar_t*) pathW.ToExtString ()  )   )

  _osd_wnt_set_error ( myError, OSD_WProcess );

}  // end OSD_Process :: SetCurrentDirectory

Standard_Boolean OSD_Process :: Failed () const {

 return myError.Failed ();

}  // end OSD_Process :: Failed

void OSD_Process :: Reset () {

 myError.Reset ();

}  // end OSD_Process :: Reset

void OSD_Process :: Perror () {

 myError.Perror ();

}  // end OSD_Process :: Perror

Standard_Integer OSD_Process :: Error () const {

 return myError.Error ();

}  // end OSD_Process :: Error

#endif
