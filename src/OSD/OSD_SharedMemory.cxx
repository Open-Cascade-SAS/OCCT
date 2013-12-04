// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef WNT
#if (!defined (__hpux )) && (!defined (HPUX))

#include <Standard_ProgramError.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ConstructionError.hxx>
#include <OSD_SharedMemory.ixx>
#include <OSD_WhoAmI.hxx>

#include <errno.h>

const OSD_WhoAmI Iam = OSD_WSharedMemory;


extern "C" int create_sharedmemory(int **,char *,int);
extern "C" int open_sharedmemory(int **,char *,int);
extern "C" int remove_sharedmemory(int *, char *);


OSD_SharedMemory::OSD_SharedMemory(){
 myId = -1;
}


// ======================================================================
OSD_SharedMemory::OSD_SharedMemory(const TCollection_AsciiString& Name,
                                   const Standard_Integer Size) 
// 	CREATE a mapping memory object ==================================
// ======================================================================


{
 myId = -1;
 
 if (!Name.IsAscii())
  Standard_ConstructionError::Raise("OSD_SharedMemory::OSD_SharedMemory: Name");

 myName = Name;

 if (Size <= 0)
  Standard_ProgramError::Raise("OSD_SharedMemory::OSD_SharedMemory : invalid size");

 mySize = Size;
}






// ======================================================================
void OSD_SharedMemory::Build()
// ======================================================================
{
  Standard_PCharacter pStr;
  //
  pStr=(Standard_PCharacter)myName.ToCString();
  myId = create_sharedmemory((int **)&myAddress, pStr, (int)mySize);
  
  if (myId == 0)
    myError.SetValue (errno, Iam, "OSD_SharedMemory::Build");
}




// ======================================================================
void OSD_SharedMemory::Open(const TCollection_AsciiString& Name,
                            const Standard_Integer Size)
// ======================================================================
//	OPEN a mapping memory section
//      We suppose that the shared memory segment is already
//      created(allocated)

{
  if (!Name.IsAscii()) {
    Standard_ConstructionError::Raise("OSD_SharedMemory::Open : Name");
  }
  myName = Name;

  if (Size <= 0) {
    Standard_ProgramError::Raise("OSD_SharedMemory::Open : invalid size");
  }
  mySize = Size;
  //
  Standard_PCharacter pStr;
  //
  pStr=(Standard_PCharacter)myName.ToCString();
  myId = open_sharedmemory((int **)&myAddress, pStr, (int)mySize);
  //
  if (myId == 0) {
    myError.SetValue (errno, Iam, "OSD_SharedMemory::Open");
  }
}

// ======================================================================
void OSD_SharedMemory::Delete() 
// ======================================================================
//	CLOSE a mapping memory section

{
  if (myError.Failed()) {
    myError.Perror();
  }
  if (myId == -1) {
    Standard_ProgramError::Raise("OSD_SharedMemory::Delete : shared memory not opened/created");
  }
  //
  Standard_PCharacter pStr;
  //
  pStr=(Standard_PCharacter)myName.ToCString();
  if (remove_sharedmemory((int *)&myId, pStr) == 0) {
    myError.SetValue(errno, Iam, "OSD_SharedMemory::Delete");
  }
}


// ======================================================================
Standard_Address OSD_SharedMemory::GiveAddress()const{
// ======================================================================
 if (myAddress == NULL)
  Standard_NullObject::Raise("OSD_SharedMemory::Address : shared memory not opened/created");
 return(myAddress);
}



// ======================================================================
void OSD_SharedMemory::Reset(){
// ======================================================================
 myError.Reset();
}

// ======================================================================
Standard_Boolean OSD_SharedMemory::Failed()const{
// ======================================================================
 return( myError.Failed());
}

// ======================================================================
void OSD_SharedMemory::Perror() {
// ======================================================================
 myError.Perror();
}


// ======================================================================
Standard_Integer OSD_SharedMemory::Error()const{
// ======================================================================
 return( myError.Error());
}

#endif
#else

//------------------------------------------------------------------------
//-------------------  Windows NT sources for OSD_SharedMemory- ----------
//------------------------------------------------------------------------

#define STRICT
#include <windows.h>

#include <OSD_SharedMemory.ixx>

void _osd_wnt_set_error ( OSD_Error&, OSD_WhoAmI, ... );

OSD_SharedMemory :: OSD_SharedMemory () {

 myId = 0;

}  // end constructor ( 1 )

OSD_SharedMemory :: OSD_SharedMemory (
                     const TCollection_AsciiString& Name,
                     const Standard_Integer         size
                    ) {

 myName = Name;

 if ( size <= 0 )

  Standard_ConstructionError :: Raise (
                                 "OSD_SharedMemory :: OSD_SharedMemory : invalid size"
                                );

 mySize = size;

}  // end constructor ( 2 )

void OSD_SharedMemory :: Build () {

 HANDLE hFileMapping = CreateFileMapping (
                        ( HANDLE )0xFFFFFFFF, NULL, PAGE_READWRITE, 0, mySize,
                        myName.ToCString ()
                       );

 if (  hFileMapping == NULL || GetLastError () == ERROR_ALREADY_EXISTS  )
 
  _osd_wnt_set_error ( myError, OSD_WSharedMemory ); 
 
 else {
 
  myAddress = MapViewOfFile ( hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0 );

  if ( myAddress == NULL )

   _osd_wnt_set_error ( myError, OSD_WSharedMemory );

  else

   myId = ( Standard_Integer )hFileMapping;

 }  // end else

}  // end OSD_SharedMemory :: Build

void OSD_SharedMemory :: Open (
                          const TCollection_AsciiString& Name,
                          const Standard_Integer         size
                         ) {

 myName = Name;

 if ( size <= 0 )

  Standard_ProgramError :: Raise ( "OSD_SharedMemory :: Open : invalid size" );

 mySize = size;

 HANDLE hFileMapping = OpenFileMapping (
                        FILE_MAP_ALL_ACCESS, FALSE, myName.ToCString ()
                       );

 if ( hFileMapping == NULL )

  _osd_wnt_set_error ( myError, OSD_WSharedMemory );

 else {
 
  myAddress = MapViewOfFile ( hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, mySize );

  if ( myAddress == NULL )
 
   _osd_wnt_set_error ( myError, OSD_WSharedMemory );

  else

   myId = ( Standard_Integer )hFileMapping;

  CloseHandle ( hFileMapping );

 }  // end else

}  // end OSD_SharedMemory :: Open

void OSD_SharedMemory :: Delete () {

 if (  Failed ()  ) Perror ();

 if ( myId == 0 )

  Standard_ProgramError :: Raise (
                            "OSD_SharedMemory :: Delete : shared memory not opened/created"
                           );

 UnmapViewOfFile ( myAddress );
 CloseHandle (  ( HANDLE )myId  );

}  // end OSD_SharedMemory :: Delete

Standard_Address OSD_SharedMemory :: GiveAddress () const {

 if ( myAddress == NULL )

  Standard_NullObject :: Raise (
                          "OSD_SharedMemory :: Address : shared memory not opened/created"
                         );

 return myAddress;

}  // end OSD_SharedMemory :: GiveAddress

Standard_Boolean OSD_SharedMemory :: Failed () const {

 return myError.Failed ();

}  // end OSD_SharedMemory :: Failed

void OSD_SharedMemory :: Reset () {

 myError.Reset ();

}  // end OSD_SharedMemory :: Reset

void OSD_SharedMemory :: Perror () {

 myError.Perror ();

}  // end OSD_SharedMemory :: Perror

Standard_Integer OSD_SharedMemory :: Error () const{

 return myError.Error ();

}  // end OSD_SharedMemory :: Error

#endif
