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

#include <OSD_Host.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_WhoAmI.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NullObject.hxx>
#include <TCollection_AsciiString.hxx>

const OSD_WhoAmI Iam = OSD_WHost;

#include <errno.h>

#include <sys/utsname.h> // For 'uname'
#include <netdb.h>       // This is for 'gethostbyname'
#include <unistd.h>
#include <stdio.h>

#if defined(__osf__) || defined(DECOSF1)
#include <sys/types.h>
#include <sys/sysinfo.h>  // For 'getsysinfo'
#include <sys/socket.h>   // To get ethernet address
#include <sys/ioctl.h>
#include <net/if.h>
extern "C" {
  int gethostname(char* address, int len); 
}
#endif

extern "C" {int sysinfo(int, char *, long);}


// =========================================================================

OSD_Host::OSD_Host(){}

// =========================================================================

TCollection_AsciiString OSD_Host::SystemVersion(){
struct utsname info;
TCollection_AsciiString result;

 uname (&info);
 result  = info.sysname;
 result += " ";
 result += info.release;
 return(result);
}

// =========================================================================

OSD_SysType OSD_Host::SystemId()const{
struct utsname info; 
 
 uname (&info);

 if (!strcmp(info.sysname,"SunOS"))          return (OSD_UnixBSD);
 if (!strcmp(info.sysname,"ULTRIX"))         return (OSD_UnixBSD);
 if (!strcmp(info.sysname,"FreeBSD"))        return (OSD_UnixBSD);
 if (!strncmp(info.sysname,"Linux",5))       return (OSD_LinuxREDHAT);
 if (!strncmp(info.sysname,"IRIX", 4))       return (OSD_UnixSystemV);
 if (!strncmp(info.sysname,"OSF", 3))        return (OSD_OSF);
 if (!strcmp(info.sysname,"AIX"))            return (OSD_Aix);
 if (!strcmp(info.sysname,"UNIX_System_V"))  return (OSD_UnixSystemV);
 if (!strcmp(info.sysname,"VMS_POSIX"))      return (OSD_VMS);
 if (!strcmp(info.sysname,"Darwin"))         return (OSD_MacOs);
 return (OSD_Unknown);
}

// =========================================================================

TCollection_AsciiString OSD_Host::HostName(){
TCollection_AsciiString result;
char value[65];
int status;

status = gethostname(value, 64);
if (status == -1) myError.SetValue(errno, Iam, "Host Name");

 result = value;
 return(result);
}


// =========================================================================


Standard_Integer OSD_Host::AvailableMemory(){
 Standard_Integer result;

#if defined(__osf__) || defined(DECOSF1)
 char buffer[16];
 ////     result = getsysinfo(GSI_PHYSMEM,buffer, 16,0,NULL);
 if (result != -1)
  result *= 1024;
#else
 result = 0;
 //@@ A faire
#endif
 return (result);
}

// =========================================================================

TCollection_AsciiString OSD_Host::InternetAddress(){
 struct hostent internet_address;
 int a,b,c,d;
 char buffer[16];
 TCollection_AsciiString result,host;

 host = HostName();
 memcpy(&internet_address,
        gethostbyname(host.ToCString()),
        sizeof(struct hostent));

 // Gets each bytes into integers
 a = (unsigned char)internet_address.h_addr_list[0][0];
 b = (unsigned char)internet_address.h_addr_list[0][1];
 c = (unsigned char)internet_address.h_addr_list[0][2];
 d = (unsigned char)internet_address.h_addr_list[0][3];
 sprintf(buffer,"%d.%d.%d.%d",a,b,c,d);
 result = buffer;
 return(result);
}

// =========================================================================
OSD_OEMType OSD_Host::MachineType(){
struct utsname info; 
 
 uname (&info);

 if (!strcmp(info.sysname,"SunOS"))         return (OSD_SUN);
 if (!strcmp(info.sysname,"ULTRIX"))        return (OSD_DEC);
 if (!strncmp(info.sysname,"IRIX",4))       return (OSD_SGI);
 if (!strcmp(info.sysname,"HP-UX"))         return (OSD_HP);
 if (!strcmp(info.sysname,"UNIX_System_V")) return (OSD_NEC);
 if (!strcmp(info.sysname,"VMS_POSIX"))     return (OSD_VAX);
 if (!strncmp(info.sysname,"OSF",3))        return (OSD_DEC);
 if (!strncmp(info.sysname,"Linux",5))      return (OSD_LIN);
 if (!strcmp(info.sysname,"FreeBSD"))       return (OSD_LIN);
 if (!strncmp(info.sysname,"AIX",3))        return (OSD_AIX);
 if (!strcmp(info.sysname,"Darwin"))        return (OSD_MAC);
 return (OSD_Unavailable);

}

void OSD_Host::Reset(){
 myError.Reset();
}

Standard_Boolean OSD_Host::Failed()const{
 return( myError.Failed());
}

void OSD_Host::Perror() {
 myError.Perror();
}


Standard_Integer OSD_Host::Error()const{
 return( myError.Error());
}

#else

//------------------------------------------------------------------------
//-------------------  WNT Sources of OSD_Host ---------------------------
//------------------------------------------------------------------------

#include <windows.h>

#include <OSD_Host.hxx>

#if defined(_MSC_VER)
  #pragma comment( lib, "WSOCK32.LIB" )
#endif

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

static BOOL                    fInit = FALSE;
static TCollection_AsciiString hostName;
static TCollection_AsciiString version;
static TCollection_AsciiString interAddr;
static Standard_Integer        memSize;

OSD_Host :: OSD_Host () {
#ifndef OCCT_UWP
 DWORD              nSize;
 char               szHostName[MAX_COMPUTERNAME_LENGTH + 1];
 char*              hostAddr = 0;
 MEMORYSTATUS       ms;
 WSADATA            wd;
 PHOSTENT           phe;
 IN_ADDR            inAddr;
 OSVERSIONINFOW     osVerInfo;

 if ( !fInit ) {

  nSize                         = MAX_COMPUTERNAME_LENGTH + 1;
  ZeroMemory (&osVerInfo, sizeof(OSVERSIONINFOW));
  osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);

  ZeroMemory (&ms, sizeof(ms));
  ZeroMemory (szHostName, sizeof(char) * (MAX_COMPUTERNAME_LENGTH + 1));

  // suppress GetVersionEx() deprecation warning
#if defined(__INTEL_COMPILER)
  #pragma warning(disable : 1478)
#elif defined(_MSC_VER)
  #pragma warning(disable : 4996)
#endif
  if (!GetVersionExW (&osVerInfo))
  {
    _osd_wnt_set_error (myError, OSD_WHost);
  }
  else if (!GetComputerNameA (szHostName, &nSize))
  {
    _osd_wnt_set_error (myError, OSD_WHost);
  }
  else
  {
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus (&ms);
  }  // end else
#ifdef _MSC_VER
  #pragma warning(default : 4996)
#endif

  if (  !Failed ()  ) {
  
    memSize = (Standard_Integer) ms.dwAvailPageFile;

   if (   WSAStartup (  MAKEWORD( 1, 1 ), &wd  )   ) {
   
    _osd_wnt_set_error ( myError, OSD_WHost );
   
   } else if (   (  phe = gethostbyname (szHostName)  ) == NULL   ) {
   
    _osd_wnt_set_error ( myError, OSD_WHost );
   
   } else {

    CopyMemory (  &inAddr, *phe -> h_addr_list, sizeof ( IN_ADDR )  );
    hostAddr = inet_ntoa ( inAddr );

   }  // end else
  
  }  // end if

  if (  !Failed ()  ) {
  
   hostName  = szHostName;
   interAddr = Standard_CString ( hostAddr );
   TCollection_AsciiString aVersion = TCollection_AsciiString("Windows NT Version ") + (int )osVerInfo.dwMajorVersion + "." + (int )osVerInfo.dwMinorVersion;
   if (*osVerInfo.szCSDVersion != L'\0')
   {
     aVersion += TCollection_AsciiString(" ") + TCollection_AsciiString (osVerInfo.szCSDVersion);
   }
   version = aVersion;

   fInit = TRUE;
  
  }  // end if
 
 }  // end if

 if ( fInit )

  myName = hostName;
#endif
}  // end constructor

TCollection_AsciiString OSD_Host :: SystemVersion () {

 return version;

}  // end OSD_Host :: SystemVersion

OSD_SysType OSD_Host :: SystemId () const {

 return OSD_WindowsNT;

}  // end OSD_Host :: SystemId

TCollection_AsciiString OSD_Host :: HostName () {

 return hostName;

}  // end OSD_Host :: HostName

Standard_Integer OSD_Host :: AvailableMemory () {

 return memSize;

}  // end OSD_Host :: AvailableMemory

TCollection_AsciiString OSD_Host :: InternetAddress () {

 return interAddr;

}  // end OSD_Host :: InternetAddress

OSD_OEMType OSD_Host :: MachineType () {

 return OSD_PC;

}  // end OSD_Host :: MachineTYpe

Standard_Boolean OSD_Host :: Failed () const {

 return myError.Failed ();

}  // end OSD_Host :: Failed

void OSD_Host :: Reset () {

 myError.Reset ();

}  // end OSD_Host :: Reset

void OSD_Host :: Perror () {

 myError.Perror ();

}  // end OSD_Host :: Perror

Standard_Integer OSD_Host :: Error () const {

 return myError.Error ();

}  //end OSD_Host :: Error

#endif





