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


#include <OSD_File.hxx>
#include <OSD_FileIterator.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <OSD_WhoAmI.hxx>
#include <TCollection_AsciiString.hxx>

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
# ifdef VMS
extern char *vmsify PARAMS ((char *name, int type));
# endif

/* In GNU systems, <dirent.h> defines this macro for us.  */
#ifdef _D_NAMLEN
# undef NAMLEN
# define NAMLEN(d) _D_NAMLEN(d)
#endif

#if (defined (POSIX) || defined (VMS) || defined (WINDOWS32)) && !defined (__GNU_LIBRARY__)
/* Posix does not require that the d_ino field be present, and some
   systems do not provide it. */
# define REAL_DIR_ENTRY(dp) 1
# define FAKE_DIR_ENTRY(dp)
#else
# define REAL_DIR_ENTRY(dp) (dp->d_ino != 0)
# define FAKE_DIR_ENTRY(dp) (dp->d_ino = 1)
#endif /* POSIX */

//const OSD_WhoAmI Iam = OSD_WFileIterator;


OSD_FileIterator::OSD_FileIterator()
: myFlag(false),
  myDescr(0),
  myEntry(0),
  myInit(0)
{
}

OSD_FileIterator::OSD_FileIterator(const OSD_Path& where,
                                   const TCollection_AsciiString& Mask)
: myFlag(false),
  myDescr(0),
  myEntry(0),
  myInit(0)
{
 Initialize(where, Mask) ;
}

// For Windows NT compatibility
void OSD_FileIterator :: Destroy () {}

void OSD_FileIterator::Initialize(const OSD_Path& where,
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

// Is there another file entry ?

Standard_Boolean OSD_FileIterator::More(){
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

// Private :  See if file name matches with a mask (like "*.c")

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

#if 0
    // LD : ancienne version.

#define TRUE  1
#define FALSE 0
#define NO_MASK '*'

static int strcmp_joker(char *fileMask,char *fileName)
{
    char *sauve_fileMask,*sauve_fileName;
    int compare_result;
    int beginning = 1;   // 0 if first character is a joker, otherwise 1

    while (*fileName) {        // Test end of AsciiString

     if (*fileMask == NO_MASK) {
      beginning = 0;

      while(*fileMask == NO_MASK) fileMask++;

      if (*fileMask) {
       while(*fileName && 
             *fileName != *fileMask) 
        fileName++;

       sauve_fileMask = fileMask;  // Save strings
       sauve_fileName = fileName;
      }
      else return(0);   // fileMask ends with a joker

     }
     else { // Compare two characters
      compare_result = *fileMask++ - *fileName++;

      if (compare_result != 0) 
       if (beginning) 
        return (compare_result); /* 1ere chaine pas de joker au debut */
       else {  // Look ahead for same string
        fileMask = sauve_fileMask;
        fileName = ++sauve_fileName;
        while(*fileName && 
              *fileName != *fileMask)
         fileName++;
        sauve_fileName = fileName;
       }

    }    

   }    

   while(*fileMask == NO_MASK) fileMask++;
   return(*fileMask - *fileName);
}
#endif

// Find next file entry in current directory

void OSD_FileIterator::Next(){
int again = 1;
struct stat stat_buf;
 myFlag = false;   // Initialize to nothing found

 do {
    myEntry = readdir((DIR *)myDescr);

    if (!myEntry){   // No file found
     myEntry = NULL;              // Keep pointer clean
     myFlag = Standard_False;   // No more files/directory
     closedir((DIR *)myDescr) ;       // so close directory
     myDescr = NULL;
     again = 0;
    }
    else {
     if (!strcmp(((struct dirent *)myEntry)->d_name,".")) continue;
     if (!strcmp(((struct dirent *)myEntry)->d_name,"..")) continue;

     // Is it a file ?
     const TCollection_AsciiString aFullName = myPlace + "/" + ((struct dirent* )myEntry)->d_name;
     stat(aFullName.ToCString(), &stat_buf);
     if (S_ISREG(stat_buf.st_mode))   // LD : Ensure me it's a regular file
      if (strcmp_joker(myMask.ToCString(), ((struct dirent *)myEntry)->d_name)){
							 // Does it follow mask ?
       myFlag = Standard_True;
       again = 0;
      }
    }

 } while (again);

}

// Get Name of selected file

OSD_File OSD_FileIterator::Values(){
OSD_Path thisvalue;
TCollection_AsciiString Name;
TCollection_AsciiString Ext;
Standard_Integer position;

 if (myEntry) Name = ((struct dirent *)myEntry)->d_name ;

 position = Name.Search(".");

 if (position != -1){
  Ext = Name;
  Ext.Remove(1,position-1);
  Name.Remove( position,Ext.Length());
 }

 thisvalue.SetValues("", "", "", "", "", Name,Ext); 
 TheIterator.SetPath (thisvalue);

 return (TheIterator);
}


void OSD_FileIterator::Reset(){
 myError.Reset();
}

Standard_Boolean OSD_FileIterator::Failed()const{
 return( myError.Failed());
}

void OSD_FileIterator::Perror() {
 myError.Perror();
}


Standard_Integer OSD_FileIterator::Error()const{
 return( myError.Error());
}

#else

//------------------------------------------------------------------------
//-------------------  Windows NT sources for OSD_FileIterator -----------
//------------------------------------------------------------------------

#include <windows.h>


#include <OSD_File.hxx>
#include <OSD_FileIterator.hxx>
#include <OSD_OSDError.hxx>
#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#define _FD (  ( PWIN32_FIND_DATAW )myData  )

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

OSD_FileIterator :: OSD_FileIterator (
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

void OSD_FileIterator :: Destroy () {

 if ( myData != NULL ) HeapFree (  GetProcessHeap (), 0, myData  );

 if (  myHandle != INVALID_HANDLE_VALUE  )

  FindClose (  ( HANDLE )myHandle  );

}  // end  OSD_DirectoryIterator :: Destroy

Standard_Boolean OSD_FileIterator :: More () {

 if (  myHandle == INVALID_HANDLE_VALUE  ) {
 
  TCollection_AsciiString wc = myPlace + "/" + myMask;

  myData = HeapAlloc (
            GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, sizeof ( WIN32_FIND_DATAW )
           );

  // make wchar_t string from UTF-8
  TCollection_ExtendedString wcW(wc);
  myHandle = FindFirstFileExW (wcW.ToWideString(), FindExInfoStandard, (PWIN32_FIND_DATAW)myData, FindExSearchNameMatch, NULL, 0);

  if (  myHandle == INVALID_HANDLE_VALUE  )
  
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

}  // end OSD_FileIterator :: More

void OSD_FileIterator :: Next () {

 if ( ! myFirstCall || ( _FD -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) {
 
  do {
  
   if (   !FindNextFileW (  ( HANDLE )myHandle, _FD  )   ) {
   
    myFlag = Standard_False;

    break;
   
   }  // end if
  
  } while (  ( _FD -> dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )  );
 
 }  // end if
 
 myFirstCall = Standard_False;

}  // end OSD_FileIterator :: Next

OSD_File OSD_FileIterator :: Values () {

 // make UTF-8 string
 TCollection_AsciiString aFileName
   (TCollection_ExtendedString( (Standard_ExtString) _FD -> cFileName) );
 TheIterator.SetPath (   OSD_Path ( aFileName  )   );

 return TheIterator;

}  // end OSD_FileIterator :: Values

Standard_Boolean OSD_FileIterator :: Failed () const {

 return myError.Failed ();

}  // end OSD_FileIterator :: Failed

void OSD_FileIterator :: Reset () {

 myError.Reset ();

}  // end OSD_FileIterator :: Reset

void OSD_FileIterator :: Perror () {

 myError.Perror ();

}  // end OSD_FileIterator :: Perror

Standard_Integer OSD_FileIterator :: Error () const {

 return myError.Error ();

}  // end  OSD_FileIterator :: Error

// For compatibility with UNIX version
OSD_FileIterator::OSD_FileIterator() {}

void OSD_FileIterator::Initialize(
                       const OSD_Path&,
                       const TCollection_AsciiString&){}
 
#endif
