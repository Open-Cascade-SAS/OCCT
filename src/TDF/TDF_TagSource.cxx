// Created on: 1997-08-04
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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



#include <TDF_TagSource.ixx>
#include <Standard_GUID.hxx>


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

void TDF_TagSource::Paste (
  const Handle(TDF_Attribute)&       Into,
  const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDF_TagSource)::DownCast(Into)->Set (myTag);
}
