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


#include <OSD_Directory.hxx>
#include <OSD_DirectoryIterator.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <OSD_WhoAmI.hxx>
#include <TCollection_AsciiString.hxx>

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

OSD_DirectoryIterator::OSD_DirectoryIterator() 
: myFlag(false),
  myDescr(0),
  myEntry(0),
  myInit(0)
{
}

OSD_DirectoryIterator::OSD_DirectoryIterator(const OSD_Path& where,
                                             const TCollection_AsciiString& Mask)
: myFlag(false),
  myDescr(0),
  myEntry(0),
  myInit(0)
{
 Initialize(where, Mask) ;
}

// For Windows NT compatibility -----------
void OSD_DirectoryIterator :: Destroy () {}
//-----------------------------------------

void OSD_DirectoryIterator::Initialize(const OSD_Path& where,
                                   const TCollection_AsciiString& Mask){

 myFlag = Standard_False;
 where.SystemName(myPlace);
 if (myPlace.Length()==0) myPlace = ".";
 myMask = Mask;
 if (myDescr) {
   closedir((DIR *)myDescr) ;
   myDescr = NULL ;
 }
 myInit = 1 ;
}

// Is there another directory entry ?

Standard_Boolean OSD_DirectoryIterator::More(){
 if (myInit) {
   myInit = 0 ;
   myDescr = (Standard_Address) opendir(myPlace.ToCString()); 
   if (myDescr) {            // LD : Si repertoire inaccessible retourner False
     myFlag = Standard_True;
     myInit = 0 ;
     Next();          // Now find first entry
   }
 }
 return myFlag;
}

// Private :  See if directory name matches with a mask (like "*.c")
// LD : reecrit (original dans OSD_FileIterator.cxx)


static int strcmp_joker(const char *Mask,const char *Name)
{
  const char *p, *s ;

  for(p = Mask,s = Name ; *p && *p != '*' ; p++,s++)
    if (*p != *s) return 0 ;
  if (!*p) return !(*s) ;
  while (*p == '*') p++ ;
  if (!*p) return 1 ;
  for (;*s; s++)
    if (strcmp_joker(p,s)) return 1 ;
  return 0 ;
}

// Find next directory entry in current directory

void OSD_DirectoryIterator::Next(){
int again = 1;
struct stat stat_buf;
 myFlag = false;   // Initialize to nothing found

 do{
    myEntry = readdir((DIR *)myDescr);

    if (!myEntry){   // No file found
     myEntry = NULL;              // Keep pointer clean
     myFlag = Standard_False;   // No more files/directory
     closedir((DIR *)myDescr) ;       // so close directory
     myDescr = NULL;
     again = 0;
    }
    else {
//     if (!strcmp(entry->d_name,".")) continue;     LD : on prend ces 
//     if (!strcmp(entry->d_name,"..")) continue;         2 directories.

     // Is it a directory ?
     const TCollection_AsciiString aFullName = myPlace + "/" + ((struct dirent* )myEntry)->d_name;
     stat(aFullName.ToCString(), &stat_buf);
     if (S_ISDIR(stat_buf.st_mode))   // Ensure me it's not a file
      if (strcmp_joker(myMask.ToCString(), ((struct dirent *)myEntry)->d_name)){
							 // Does it follow mask ?
       myFlag = Standard_True;
       again = 0;
     }
    }

 } while (again);

}


// Get Name of selected directory

OSD_Directory OSD_DirectoryIterator::Values(){
OSD_Path thisvalue;
TCollection_AsciiString Name;
TCollection_AsciiString Ext;
Standard_Integer position;

 if (myEntry) Name = ((struct dirent *)myEntry)->d_name ;

 position = Name.Search(".");

 if (position != -1){
  Ext = Name.Split(position - 1) ;   // Debug LD
//  Ext.Remove(1,position);
//  Name.Remove( position,Ext.Length()+1);
 }

 thisvalue.SetValues("", "", "", "", "", Name,Ext);
 TheIterator.SetPath (thisvalue);

 return (TheIterator);
}


void OSD_DirectoryIterator::Reset(){
 myError.Reset();
}

Standard_Boolean OSD_DirectoryIterator::Failed()const{
 return( myError.Failed());
}

void OSD_DirectoryIterator::Perror() {
 myError.Perror();
}


Standard_Integer OSD_DirectoryIterator::Error()const{
 return( myError.Error());
}

#else

//------------------------------------------------------------------------
//-------------------  Windows NT sources for OSD_DirectoryIterator ------
//------------------------------------------------------------------------


#include <windows.h>


#include <OSD_Directory.hxx>
#include <OSD_DirectoryIterator.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#define _FD (  ( PWIN32_FIND_DATAW )myData  )

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

OSD_DirectoryIterator :: OSD_DirectoryIterator (
                          const OSD_Path&                where,
                          const TCollection_AsciiString& Mask
                         ) {

 myFlag   = Standard_False;
 myHandle = INVALID_HANDLE_VALUE;

 where.SystemName ( myPlace );

 if (  myPlace.Length () == 0  ) myPlace = ".";

 myMask = Mask;
 myData = NULL;

}  // end constructor

void OSD_DirectoryIterator :: Destroy () {

 if ( myData != NULL ) HeapFree (  GetProcessHeap (), 0, myData  );

 if (  myHandle != INVALID_HANDLE_VALUE  )

  FindClose (  ( HANDLE )myHandle  );

}  // end  OSD_DirectoryIterator :: Destroy

Standard_Boolean OSD_DirectoryIterator :: More () {

 if (  myHandle == INVALID_HANDLE_VALUE  ) {
 
  TCollection_AsciiString wc = myPlace + "/" + myMask;

  myData = HeapAlloc (
            GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, sizeof ( WIN32_FIND_DATAW )
           );

  // make wchar_t string from UTF-8
  TCollection_ExtendedString wcW(wc);
  myHandle = FindFirstFileExW (wcW.ToWideString(), FindExInfoStandard, (PWIN32_FIND_DATAW)myData, FindExSearchNameMatch, NULL, 0);

  if ( myHandle == INVALID_HANDLE_VALUE )

    _osd_wnt_set_error ( myError, OSD_WDirectoryIterator );
  
  else {
  
   myFlag      = Standard_True;
   myFirstCall = Standard_True;

   Next ();

  }  // end else
  
 } else if ( !myFlag ) {
 
  FindClose (  ( HANDLE )myHandle  );
  myHandle = INVALID_HANDLE_VALUE;
 
 }  // end if

 return myFlag;

}  // end OSD_DirectoryIterator :: More

void OSD_DirectoryIterator :: Next () {

 if ( ! myFirstCall || ! ( _FD -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) {
 
  do {
  
   if (   !FindNextFileW (  ( HANDLE )myHandle, _FD  )   ) {
   
    myFlag = Standard_False;

    break;
   
   }  // end if
  
  } while (  !( _FD -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )  );
 
 }  // end if
 
 myFirstCall = Standard_False;

}  // end  OSD_DirectoryIterator :: Next

OSD_Directory OSD_DirectoryIterator :: Values () {

 // make UTF-8 string
 TCollection_AsciiString aFileName
   (TCollection_ExtendedString( (Standard_ExtString) _FD -> cFileName) );
 TheIterator.SetPath (   OSD_Path ( aFileName )   );

 return TheIterator;

}  // end  OSD_DirectoryIterator :: Values

Standard_Boolean OSD_DirectoryIterator :: Failed () const {

 return myError.Failed ();

}  // end OSD_DirectoryIterator :: Failed

void OSD_DirectoryIterator :: Reset () {

 myError.Reset ();

}  // end OSD_DirectoryIterator :: Reset

void OSD_DirectoryIterator :: Perror () {

 myError.Perror ();

}  // end OSD_DirectoryIterator :: Perror

Standard_Integer OSD_DirectoryIterator :: Error () const {

 return myError.Error ();

}  // end OSD_DirectoryIterator :: Error

// For compatibility with UNIX version
OSD_DirectoryIterator::OSD_DirectoryIterator() {}

void OSD_DirectoryIterator::Initialize(
                             const OSD_Path&,
                             const TCollection_AsciiString&){}

#endif
