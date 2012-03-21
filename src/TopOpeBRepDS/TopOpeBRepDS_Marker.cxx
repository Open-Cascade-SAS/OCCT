// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
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


#include <TopOpeBRepDS_Marker.ixx>
#include <TopOpeBRepDS_define.hxx>
//#include <TopExp.hxx>
//#include <TopTools_IndexedMapOfShape.hxx>

TopOpeBRepDS_Marker::TopOpeBRepDS_Marker()
{
  Reset();
}

void TopOpeBRepDS_Marker::Reset()
{
  myhe.Nullify();
  myne = 0;
}

void TopOpeBRepDS_Marker::Set(const Standard_Integer ie, const Standard_Boolean b)
{
  Allocate(ie);
  if (!(ie>=1 && ie<=myne)) return;
  myhe->SetValue(ie,b); 
}

void TopOpeBRepDS_Marker::Set(const Standard_Boolean b, const Standard_Integer na, const Standard_Address aa)
{
  char ** a = (char**)aa;
//  Standard_Integer ia,ie;
  Standard_Integer ia;
  if (!na) myhe->Init(b);
  else for (ia=0; ia<na; ia++) Set(atoi(a[ia]),b);
}

Standard_Boolean TopOpeBRepDS_Marker::GetI(const Standard_Integer ie) const
{
  if (myhe.IsNull()) return Standard_False;
  if (!(ie>=1 && ie<=myne)) return Standard_False;
  return myhe->Value(ie);
}

void TopOpeBRepDS_Marker::Allocate(const Standard_Integer n)
{
  Standard_Boolean all = (n > myne);
  Standard_Integer nall = n;
  if (all) {
    if (myne == 0) nall = 1000;
    myhe = new TColStd_HArray1OfBoolean(0,nall);
    myhe->Init(Standard_False);
  }
  if (nall) myne = nall;
}
