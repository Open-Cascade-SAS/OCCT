// Created on: 2000-09-08
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <XCAFDoc_Volume.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

XCAFDoc_Volume::XCAFDoc_Volume()
{
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Volume::GetID() 
{
  static Standard_GUID VolumeID ("efd212f1-6dfd-11d4-b9c8-0060b0ee281b");
  return VolumeID;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Volume::ID() const
{
  return GetID();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_Volume) XCAFDoc_Volume::Set (const TDF_Label& L,const Standard_Real V) 
{
  Handle(XCAFDoc_Volume) A;
  if (!L.FindAttribute(XCAFDoc_Volume::GetID(), A)) {
    A = new XCAFDoc_Volume;
    L.AddAttribute(A);
  }
  A->Set(V); 
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void XCAFDoc_Volume::Set (const Standard_Real V) 
{
  Backup();
  myValue = V;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Real XCAFDoc_Volume::Get() const { return myValue; }

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_Volume::Get(const TDF_Label& label,Standard_Real& vol) 
{
  Handle(XCAFDoc_Volume) aVolume;
  if (!label.FindAttribute(XCAFDoc_Volume::GetID(), aVolume))
    return Standard_False;
  
  vol = aVolume->Get();
  return Standard_True;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_Volume::NewEmpty () const
{  
  return new XCAFDoc_Volume(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_Volume::Restore(const Handle(TDF_Attribute)& With) 
{
  Handle(XCAFDoc_Volume) R = Handle(XCAFDoc_Volume)::DownCast (With);
  myValue = R->Get();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_Volume::Paste (const Handle(TDF_Attribute)& Into,
			   const Handle(TDF_RelocationTable)& /* RT */) const
{ 
  Handle(XCAFDoc_Volume) R = Handle(XCAFDoc_Volume)::DownCast (Into);
  R->Set(myValue);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& XCAFDoc_Volume::Dump (Standard_OStream& anOS) const
{  
  anOS << "Volume "; 
  anOS << Get();
  return anOS;
}
