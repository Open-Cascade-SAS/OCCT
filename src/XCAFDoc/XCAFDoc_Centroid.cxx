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



#include <XCAFDoc_Centroid.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

XCAFDoc_Centroid::XCAFDoc_Centroid()
{
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Centroid::GetID() 
{
  static Standard_GUID CentroidID ("efd212f3-6dfd-11d4-b9c8-0060b0ee281b");
  return CentroidID; 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 Handle(XCAFDoc_Centroid) XCAFDoc_Centroid::Set(const TDF_Label& L,const gp_Pnt& pnt) 
{
  Handle(XCAFDoc_Centroid) A;
  if (!L.FindAttribute (XCAFDoc_Centroid::GetID(), A)) {
    A = new XCAFDoc_Centroid ();
    L.AddAttribute(A);
  }
  A->Set(pnt); 
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 void XCAFDoc_Centroid::Set(const gp_Pnt& pnt) 
{
  Backup();
  myCentroid = pnt;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

 gp_Pnt XCAFDoc_Centroid::Get() const
{
  return myCentroid;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Boolean XCAFDoc_Centroid::Get(const TDF_Label& label,gp_Pnt& pnt) 
{
  Handle(XCAFDoc_Centroid) aCentroid;
  if (!label.FindAttribute(XCAFDoc_Centroid::GetID(), aCentroid))
    return Standard_False;
  
  pnt = aCentroid->Get();
  return Standard_True;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Centroid::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

 void XCAFDoc_Centroid::Restore(const Handle(TDF_Attribute)& With) 
{
  myCentroid = Handle(XCAFDoc_Centroid)::DownCast(With)->Get();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) XCAFDoc_Centroid::NewEmpty() const
{
  return new XCAFDoc_Centroid();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void XCAFDoc_Centroid::Paste(const Handle(TDF_Attribute)& Into,const Handle(TDF_RelocationTable)& /* RT */) const
{
  Handle(XCAFDoc_Centroid)::DownCast(Into)->Set(myCentroid);

}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& XCAFDoc_Centroid::Dump (Standard_OStream& anOS) const
{  
  anOS << "Centroid ( "; 
  anOS << myCentroid.X() << ",";
  anOS << myCentroid.Y() << ",";
  anOS << myCentroid.Z() << ")";
  return anOS;
}
