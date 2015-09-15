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


#include <OSD_OSDError.hxx>
#include <OSD_Printer.hxx>
#include <OSD_WhoAmI.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NullObject.hxx>
#include <TCollection_AsciiString.hxx>

#include <errno.h>
//const OSD_WhoAmI Iam = OSD_WPrinter;
// -------------------------------------------------------------
// -------------------------------------------------------------
OSD_Printer::OSD_Printer (const TCollection_AsciiString& Name){
 SetName(Name);
}


// -------------------------------------------------------------
// -------------------------------------------------------------
void OSD_Printer::SetName (const TCollection_AsciiString& Name){

 if (!Name.IsAscii())
  Standard_ConstructionError::Raise("OSD_Printer::SetName");

 myName = Name;
}


// -------------------------------------------------------------
// -------------------------------------------------------------
void OSD_Printer::Name (TCollection_AsciiString &Name)const{
 Name = myName;
}


// -------------------------------------------------------------
// -------------------------------------------------------------
void OSD_Printer::Reset(){
 myError.Reset();
}

// -------------------------------------------------------------
// -------------------------------------------------------------
Standard_Boolean OSD_Printer::Failed()const{
 return( myError.Failed());
}

// -------------------------------------------------------------
// -------------------------------------------------------------
void OSD_Printer::Perror() {
 myError.Perror();
}


// -------------------------------------------------------------
// -------------------------------------------------------------
Standard_Integer OSD_Printer::Error()const{
 return( myError.Error());
}
#else

//------------------------------------------------------------------------
//-------------------  Windows NT sources for OSD_Printer ----------------
//------------------------------------------------------------------------

#include <OSD_OSDError.hxx>
#include <OSD_Printer.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NullObject.hxx>
#include <TCollection_AsciiString.hxx>

OSD_Printer :: OSD_Printer ( const TCollection_AsciiString& Name ) {

 SetName ( Name );

}  // end constructor

void OSD_Printer :: SetName ( const TCollection_AsciiString& Name ) {

 myName = Name;

}  // end OSD_Printer :: SetName

void OSD_Printer :: Name ( TCollection_AsciiString& Name ) const {

 Name = myName;

}  // end OSD_Printer :: Name

Standard_Boolean OSD_Printer :: Failed () const {

 return myError.Failed ();

}  // end OSD_Printer :: Failed

void OSD_Printer :: Reset () {

 myError.Reset ();

}  // end OSD_Printer :: Reset

void OSD_Printer :: Perror () {

 myError.Perror ();

}  // end OSD_Printer :: Perror

Standard_Integer OSD_Printer :: Error () const {

 return myError.Error ();

}  // end OSD_Printer :: Error


#endif
