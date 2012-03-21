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

#ifndef WNT

#include <Standard_NullObject.hxx>
#include <Standard_ConstructionError.hxx>
#include <OSD_Host.ixx>
#include <OSD_WhoAmI.hxx>

const OSD_WhoAmI Iam = OSD_WHost;

#include <errno.h>

#ifdef HAVE_SYS_UTSNAME_H
# include <sys/utsname.h> // For 'uname'
#endif

#ifdef HAVE_NETDB_H
# include <netdb.h>       // This is for 'gethostbyname'
#endif

#include <stdio.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

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

#ifdef HAVE_SYSENT_H
# include <sysent.h>      // for 'gethostname'
#endif

#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#ifdef HAVE_SYS_UNISTD_H
# include <sys/unistd.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_SYS_SYSTEMINFO_H
# include <sys/systeminfo.h>
#endif

extern "C" {int sysinfo(int, char *, long);}


// =========================================================================

OSD_Host::OSD_Host(){}

// =========================================================================

TCollection_AsciiString OSD_Host::SystemVersion(){
struct utsname info;
TCollection_AsciiString result;
#ifdef HAVE_SYS_SYSTEMINFO_H
char buf[100];
#endif

 uname (&info);
 result  = info.sysname;
 result += " ";
 result += info.release;
#ifdef HAVE_SYS_SYSTEMINFO_H
 result += " ";
 sysinfo(SI_ARCHITECTURE,buf,99);
 result += buf;
 result += " ";
 sysinfo(SI_HW_PROVIDER,buf,99);
 result += buf;
#endif
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

#define STRICT
#include <windows.h>

#include <OSD_Host.hxx>

#pragma comment( lib, "WSOCK32.LIB" )

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

static BOOL                    fInit = FALSE;
static TCollection_AsciiString hostName;
static TCollection_AsciiString version;
static TCollection_AsciiString interAddr;
static Standard_Integer        memSize;

OSD_Host :: OSD_Host () {

 DWORD              nSize;
 Standard_Character szHostName[ MAX_COMPUTERNAME_LENGTH + 1 ];
 char*              hostAddr;
 MEMORYSTATUS       ms;
 WSADATA            wd;
 PHOSTENT           phe;
 IN_ADDR            inAddr;
 OSVERSIONINFO      osVerInfo;

 if ( !fInit ) {

  nSize                         = MAX_COMPUTERNAME_LENGTH + 1;
  osVerInfo.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );

  ZeroMemory (  szHostName, sizeof ( hostName )  );

  if (  !GetVersionEx ( &osVerInfo )  ) {

   _osd_wnt_set_error ( myError, OSD_WHost );

  } else if (  !GetComputerName ( szHostName, &nSize )  ) {
  
   _osd_wnt_set_error ( myError, OSD_WHost );
  
  } else {
 
   ms.dwLength = sizeof ( MEMORYSTATUS );
   GlobalMemoryStatus ( &ms );

  }  // end else

  if (  !Failed ()  ) {
  
   memSize = ms.dwAvailPageFile;

   if (   WSAStartup (  MAKEWORD( 1, 1 ), &wd  )   ) {
   
    _osd_wnt_set_error ( myError, OSD_WHost );
   
   } else if (   (  phe = gethostbyname ( szHostName )  ) == NULL   ) {
   
    _osd_wnt_set_error ( myError, OSD_WHost );
   
   } else {

    CopyMemory (  &inAddr, *phe -> h_addr_list, sizeof ( IN_ADDR )  );
    hostAddr = inet_ntoa ( inAddr );

   }  // end else
  
  }  // end if

  if (  !Failed ()  ) {
  
   hostName  = szHostName;
   interAddr = Standard_CString ( hostAddr );
   wsprintf (
    osVerInfo.szCSDVersion, TEXT( "Windows NT Version %d.%d" ),
    osVerInfo.dwMajorVersion, osVerInfo.dwMinorVersion
   );
   version = osVerInfo.szCSDVersion;

   fInit = TRUE;
  
  }  // end if
 
 }  // end if

 if ( fInit )

  myName = hostName;

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





