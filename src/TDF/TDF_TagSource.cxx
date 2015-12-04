// Created on: 1997-08-04
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_TagSource.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_TagSource,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDF_TagSource::GetID () { 

  static Standard_GUID TDF_TagSourceID("2a96b611-ec8b-11d0-bee7-080009dc3333");
  return TDF_TagSourceID;
}



//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDF_TagSource) TDF_TagSource::Set (const TDF_Label& L) {

  Handle(TDF_TagSource) T;
  if (!L.FindAttribute (TDF_TagSource::GetID (), T)) {
    T = new TDF_TagSource ();
    L.AddAttribute (T);
  }
  return T;
}

//=======================================================================
//function : NewChild
//purpose  : 
//=======================================================================

TDF_Label TDF_TagSource::NewChild (const TDF_Label& L) 
{
  Handle(TDF_TagSource) T;
  if (!L.FindAttribute(GetID(),T)) {
    T = new TDF_TagSource();
    L.AddAttribute(T);
  }
  return T->NewChild();
}



//=======================================================================
//function : TDF_TagSource
//purpose  : 
//=======================================================================

TDF_TagSource::TDF_TagSource () : myTag(0) { }



//=======================================================================
//function : NewTag
//purpose  : 
//=======================================================================

Standard_Integer TDF_TagSource::NewTag ()  {

  Backup(); // FID 02/07/98
  return ++myTag;
}


//=======================================================================
//function : NewChild
//purpose  : 
//=======================================================================

TDF_Label TDF_TagSource::NewChild () {
  return Label().FindChild(NewTag(),Standard_True);
}


//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Integer TDF_TagSource::Get() const
{
  return myTag;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TDF_TagSource::Set (const Standard_Integer T) {
  // OCC2932 correction
  if(myTag == T) return;

  Backup (); // FID 02/07/98
  myTag = T;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDF_TagSource::ID() const { return GetID (); }




//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDF_TagSource::NewEmpty () const
{  
  return new TDF_TagSource (); 
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDF_TagSource::Restore(const Handle(TDF_Attribute)& With) 
{
  myTag = Handle(TDF_TagSource)::DownCast (With)->Get ();
}



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDF_TagSource::Paste (const Handle(TDF_Attribute)& Into,
                           const Handle(TDF_RelocationTable)&) const
{
  Handle(TDF_TagSource)::DownCast(Into)->Set (myTag);
}
