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


#include <OSD_Disk.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <OSD_WhoAmI.hxx>

const OSD_WhoAmI Iam = OSD_WDisk;

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ANDROID__)
  #include <sys/vfs.h>
  #define statvfs  statfs
  #define fstatvfs fstatfs
#else
  #include <sys/statvfs.h>
#endif

#ifdef __cplusplus
}
#endif

#include <errno.h>

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

struct statvfs buffer;

  if ( statvfs(DiskName.ToCString(),&buffer) == 0 ){
    int BSize512 = buffer.f_frsize / 512 ;
    return buffer.f_blocks * BSize512 ;
  }
  else {
    myError.SetValue(errno, Iam, "OSD_Disk: statvfs failed.");
    return 0;
  }
}

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

#include <OSD_Disk.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <Standard_ProgramError.hxx>
#include <NCollection_String.hxx>
#include <TCollection_ExtendedString.hxx>

#include <windows.h>

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

static void __fastcall _osd_wnt_set_disk_name ( TCollection_AsciiString&, const OSD_Path& );

OSD_Disk :: OSD_Disk () {
 DWORD aBuffLen = GetCurrentDirectoryW(0, NULL);
 wchar_t* aBuff = new wchar_t[size_t(aBuffLen) + 1];
 GetCurrentDirectoryW(aBuffLen, aBuff);
 aBuff[aBuffLen - 1] = (aBuff[aBuffLen - 2] == L'\\') ? L'\0' : L'\\';
 aBuff[aBuffLen] = L'\0';
 if (aBuffLen > 3 && aBuff[0] != L'\\')
 {
   aBuff[3] = L'\0';
   DiskName = TCollection_AsciiString (aBuff);
   delete[] aBuff;
 }
 else
 {
   DiskName = "";
 }
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

  TCollection_ExtendedString DiskNameW(DiskName);
  if (!GetDiskFreeSpaceExW (DiskNameW.ToWideString(),
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
  TCollection_ExtendedString DiskNameW(DiskName);
  if (!GetDiskFreeSpaceExW (DiskNameW.ToWideString(),
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

void OSD_Disk :: SetDiskQuota ( const Standard_Integer /*QuotaSize*/ ) {

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
         dir.Value ( 1 ) == '|'     &&
         dir.Value ( 2 ) == '|'
  ) {
  
   dir.SetValue (  1, '\\');
   dir.SetValue (  2, '\\');
   
   for ( i = 3, k = 0; i <= j; ++i )

    if (  dir.Value ( i ) == '|') {
    
     if ( k == 0 ) {

      dir.SetValue (  i, '\\');
      ++k;
      continue; 
     
     }  // end if

     dir.SetValue (  i, '\\');
     break;
     
    }  /* end if */

    if ( k == 0 )
    {
     if (  path.Name ().UsefullLength () == 0 && path.Extension ().UsefullLength () == 0  )
     
      goto badPath;

     else {

      dir += '\\';
      dir += path.Name ();
      dir += path.Extension ();
      
     }  // end else    
    }

    if (   dir.Value (  dir.UsefullLength ()  ) != '\\') dir += '\\';

    result = dir;
  
  } else {
badPath:  
   throw Standard_ProgramError ( "OSD_Disk: bad disk name" );

  }  // end else
 
 } else result += '/';

}  // end _osd_set_disk_name

#endif
