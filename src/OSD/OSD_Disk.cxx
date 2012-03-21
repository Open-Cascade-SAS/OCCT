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

#include <OSD_Disk.ixx>
#include <OSD_WhoAmI.hxx>

const OSD_WhoAmI Iam = OSD_WDisk;

#ifdef __cplusplus
extern "C" {
#endif

  //#include <sys/types.h>

#if defined(HAVE_STATVFS) && defined(HAVE_SYS_STATVFS_H)
# include <sys/statvfs.h>
#elif defined(HAVE_STATFS) && defined(HAVE_SYS_VFS_H)
  // Note: we want to use the "vfs" method on HP, not elsewhere
# include <sys/vfs.h>
#elif defined(HAVE_STATFS)
# include <sys/param.h>
# include <sys/mount.h>
#endif

#ifdef __cplusplus
}
#endif                                                       

#include <errno.h>

#if defined(HAVE_STATVFS)
extern "C" {int statvfs(const char *, struct statvfs *); }
#endif

OSD_Disk::OSD_Disk() : myQuotaSize(0) {}


OSD_Disk::OSD_Disk(const OSD_Path& name){
 DiskName = name.Disk();
 myQuotaSize = 0;
}

OSD_Disk::OSD_Disk(const Standard_CString name)
{
 DiskName = name;
 myQuotaSize = 0;
}

void OSD_Disk::SetName(const OSD_Path& name){
 DiskName = name.Disk();
}


OSD_Path OSD_Disk::Name()const{
 OSD_Path result;
 result.SetDisk(DiskName);
 return(result);
}

Standard_Integer OSD_Disk::DiskSize(){

#if !defined(HAVE_STATVFS)

struct statfs buffer;
  if ( statfs (DiskName.ToCString(),&buffer) == 0 ){
    long BSize512 = buffer.f_bsize / 512 ;
    return buffer.f_blocks * BSize512 ;
  }
  else {
    myError.SetValue(errno, Iam, "OSD_Disk: statfs failed.");

#else

struct statvfs buffer;

  if ( statvfs(DiskName.ToCString(),&buffer) == 0 ){
    int BSize512 = buffer.f_frsize / 512 ;
    return buffer.f_blocks * BSize512 ;
  }
  else {
    myError.SetValue(errno, Iam, "OSD_Disk: statvfs failed.");

#endif
    return 0;
  }
}

#if !defined(HAVE_STATVFS)
Standard_Integer OSD_Disk::DiskFree(){

struct statfs buffer;
  if ( statfs (DiskName.ToCString(),&buffer) == 0 ){
    long BSize512 = buffer.f_bsize / 512 ;
    return buffer.f_bavail * BSize512 ;
  }
  else {
    myError.SetValue(errno, Iam, "OSD_Disk: statfs failed.");
    return 0;
  }
}

#else

Standard_Integer OSD_Disk::DiskFree(){

struct statvfs buffer;
  if ( statvfs (DiskName.ToCString(),&buffer) == 0 ){
    int BSize512 = buffer.f_frsize / 512 ;
    return buffer.f_bavail * BSize512 ;
  }
  else {
    myError.SetValue(errno, Iam, "OSD_Disk: statvfs failed.");
    return 0;
  }
}

#endif

Standard_Integer OSD_Disk::DiskQuota(){
//@@@ A faire
return 0;
}


void OSD_Disk::SetDiskQuota(const Standard_Integer ){
// int status;
// struct dqblk quota_info;
#ifdef ULTRIX
// status = quota(Q_SETDLIM,<< User Id>>,&quota_info);
#else
// status = quotactl(Q_SETQLIM,"",<< User Id >>,&quota_info);
#endif
//@@@ A terminer
}


void OSD_Disk::SetQuotaOff(){
//int status;
#ifdef ULTRIX
// status = setquota("","");
#else
// status = quotactl(Q_QUOTAOFF,"",0,NULL);
#endif
//@@@ A faire
}

void OSD_Disk::SetQuotaOn(){
//TCollection_AsciiString quota_file="????";
//int status;
#ifdef ULTRIX
// status = setquota("",quota_file);
#else
// status = quotactl(Q_QUOTAON,"",0,quota_file);
#endif
//@@@ A faire
}


void OSD_Disk::Reset(){
 myError.Reset();
}

Standard_Boolean OSD_Disk::Failed()const{
 return( myError.Failed());
}

void OSD_Disk::Perror() {
 myError.Perror();
}


Standard_Integer OSD_Disk::Error()const{
 return( myError.Error());
}

#else

//-------------------------------------------------------------------------------
//---------------------------- Windows NT System --------------------------------
//-------------------------------------------------------------------------------

#define STRICT
#include <OSD_Disk.ixx>
#include <Standard_ProgramError.hxx>

#include <windows.h>

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

static void __fastcall _osd_wnt_set_disk_name ( TCollection_AsciiString&, const OSD_Path& );

OSD_Disk :: OSD_Disk () {

 TCHAR cwd[ MAX_PATH ];

 GetCurrentDirectory ( MAX_PATH, cwd );

 cwd[ 3 ] = TEXT( '\x00' );
 DiskName = cwd;

}  // end constructor ( 1 )

OSD_Disk :: OSD_Disk ( const OSD_Path& Name ) {

 _osd_wnt_set_disk_name ( DiskName, Name );

}  // end constructor ( 2 )

OSD_Disk :: OSD_Disk ( const Standard_CString PathName ) {

 OSD_Path path ( PathName );

 _osd_wnt_set_disk_name ( DiskName, path );

}  // end constructor ( 3 )

OSD_Path OSD_Disk :: Name () const {

 return DiskName;

}  // end OSD_Disk :: Name

void OSD_Disk :: SetName ( const OSD_Path& Name ) {

 DiskName = Name.Disk ();

}  // end OSD_Disk :: SetName

Standard_Integer OSD_Disk :: DiskSize () {

 Standard_Integer retVal = 0;


//  DWORD            dwSpC;
//  DWORD            dwBpS;
//  DWORD            dwFC;
//  DWORD            dwC;

//  if (   !GetDiskFreeSpace (  DiskName.ToCString (), &dwSpC, &dwBpS, &dwFC, &dwC  )   )

  ULARGE_INTEGER lpFreeBytesAvailableToCaller; // receives the number of bytes on
                                                // disk available to the caller
  ULARGE_INTEGER lpTotalNumberOfBytes;    // receives the number of bytes on disk
  ULARGE_INTEGER lpTotalNumberOfFreeBytes;// receives the free bytes on disk


  if (!GetDiskFreeSpaceEx (DiskName.ToCString (),
			   &lpFreeBytesAvailableToCaller,
			   &lpTotalNumberOfBytes,
			   &lpTotalNumberOfFreeBytes))
    
    _osd_wnt_set_error ( myError, OSD_WDisk );
  
  else {
    
    ULONGLONG  aSize = lpTotalNumberOfBytes.QuadPart /512;
   
    retVal = ( Standard_Integer ) aSize; // may be an overflow
    
    // retVal = ( Standard_Integer )( dwSpC * dwBpS * dwFC );
  }


 return retVal;

}  // end OSD_Disk :: DiskSize

Standard_Integer OSD_Disk :: DiskFree () {

 Standard_Integer retVal = -1;


//  DWORD            dwSpC;
//  DWORD            dwBpS;
//  DWORD            dwFC;
//  DWORD            dwC;

  ULARGE_INTEGER lpFreeBytesAvailableToCaller; // receives the number of bytes on
                                                // disk available to the caller
  ULARGE_INTEGER lpTotalNumberOfBytes;    // receives the number of bytes on disk
  ULARGE_INTEGER lpTotalNumberOfFreeBytes;// receives the free bytes on disk

  // if (   !GetDiskFreeSpace (  DiskName.ToCString (), &dwSpC, &dwBpS, &dwFC, &dwC  )   )

  if (!GetDiskFreeSpaceEx (DiskName.ToCString (),
			   &lpFreeBytesAvailableToCaller,
			   &lpTotalNumberOfBytes,
			   &lpTotalNumberOfFreeBytes))
    
    _osd_wnt_set_error ( myError, OSD_WDisk );
  
  else {
    
    ULONGLONG  aSize = lpFreeBytesAvailableToCaller.QuadPart /512;
    
    retVal = ( Standard_Integer ) aSize; // may be an overflow

    //  retVal = ( Standard_Integer )( dwSpC * dwBpS * dwFC );
  }

 return retVal;

}  // end OSD_Disk :: DiskFree

Standard_Integer OSD_Disk :: DiskQuota () {

 return DiskSize ();

}  // end OSD_Disk :: DiskQuota

void OSD_Disk :: SetDiskQuota ( const Standard_Integer QuotaSize ) {

 SetLastError (  ( DWORD )STG_E_UNIMPLEMENTEDFUNCTION  );

 _osd_wnt_set_error ( myError, OSD_WDisk );

}  // end OSD_Disk :: SetDiskQuota

void OSD_Disk :: SetQuotaOn () {

 SetLastError (  ( DWORD )STG_E_UNIMPLEMENTEDFUNCTION  );

 _osd_wnt_set_error ( myError, OSD_WDisk );

}  // end OSD_Disk :: SetQuotaOn

void OSD_Disk :: SetQuotaOff () {

 SetLastError (  ( DWORD )STG_E_UNIMPLEMENTEDFUNCTION  );

 _osd_wnt_set_error ( myError, OSD_WDisk );

}  // end OSD_Disk :: SetQuotaOff

Standard_Boolean OSD_Disk :: Failed () const {

 return myError.Failed ();

}  // end OSD_Disk :: Failed

void OSD_Disk :: Reset () {

 myError.Reset ();

}  // end OSD_Disk :: Reset

void OSD_Disk :: Perror () {

 myError.Perror ();

}  // end OSD_Disk :: Perror

Standard_Integer OSD_Disk :: Error () const {

 return myError.Error ();

}  // end OSD_Disk :: Error

static void __fastcall _osd_wnt_set_disk_name ( TCollection_AsciiString& result, const OSD_Path& path ) {

 TCollection_AsciiString dir;

 result = path.Disk ();

 if (  result.UsefullLength () == 0  ) {
 
  int i, j, k;

  dir = path.Trek ();
  
  if (   (  j = dir.UsefullLength ()  ) > 2 &&
         dir.Value ( 1 ) == TEXT( '|' )     &&
         dir.Value ( 2 ) == TEXT( '|' )
  ) {
  
   dir.SetValue (  1, TEXT( '\\' )  );
   dir.SetValue (  2, TEXT( '\\' )  );
   
   for ( i = 3, k = 0; i <= j; ++i )

    if (  dir.Value ( i ) == TEXT( '|' )  ) {
    
     if ( k == 0 ) {

      dir.SetValue (  i, TEXT( "\\" )  );     
      ++k;
      continue; 
     
     }  // end if

     dir.SetValue (  i, TEXT( "\\" )  );
     break;
     
    }  /* end if */

    if ( k == 0 )

     if (  path.Name ().UsefullLength () == 0 && path.Extension ().UsefullLength () == 0  )
     
      goto badPath;

     else {

      dir += TEXT( "\\" );
      dir += path.Name ();
      dir += path.Extension ();
      
     }  // end else    

    if (   dir.Value (  dir.UsefullLength ()  ) != TEXT( '\\' )   ) dir += TEXT( "\\" );

    result = dir;
  
  } else {
badPath:  
   Standard_ProgramError :: Raise ( "OSD_Disk: bad disk name" );

  }  // end else
 
 } else result += TEXT( "/" );

}  // end _osd_set_disk_name

#endif
