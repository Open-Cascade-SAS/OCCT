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


#ifndef WNT

#include <Standard_ConstructionError.hxx>
#include <OSD_Printer.ixx>
#include <OSD_WhoAmI.hxx>

//const OSD_WhoAmI Iam = OSD_WPrinter;

#include <errno.h>

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

#include <OSD_Printer.ixx>

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
