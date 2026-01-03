// Created on: 1997-10-22
// Created by: Jean Yves LEBEY
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

#include <Standard_Type.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_Marker.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_Marker, Standard_Transient)

// #include <TopExp.hxx>
// #include <NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>.hxx>
TopOpeBRepDS_Marker::TopOpeBRepDS_Marker()
{
  Reset();
}

void TopOpeBRepDS_Marker::Reset()
{
  myhe.Nullify();
  myne = 0;
}

void TopOpeBRepDS_Marker::Set(const int ie, const bool b)
{
  Allocate(ie);
  if (ie < 1 || ie > myne)
    return;
  myhe->SetValue(ie, b);
}

void TopOpeBRepDS_Marker::Set(const bool b, const int na, void* const aa)
{
  char** a = (char**)aa;
  //  int ia,ie;
  int ia;
  if (!na)
    myhe->Init(b);
  else
    for (ia = 0; ia < na; ia++)
      Set(atoi(a[ia]), b);
}

bool TopOpeBRepDS_Marker::GetI(const int ie) const
{
  if (myhe.IsNull())
    return false;
  if (ie < 1 || ie > myne)
    return false;
  return myhe->Value(ie);
}

void TopOpeBRepDS_Marker::Allocate(const int n)
{
  bool all  = (n > myne);
  int  nall = n;
  if (all)
  {
    if (myne == 0)
      nall = 1000;
    myhe = new NCollection_HArray1<bool>(0, nall);
    myhe->Init(false);
  }
  if (nall)
    myne = nall;
}
