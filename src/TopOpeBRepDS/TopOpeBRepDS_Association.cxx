// Created on: 1998-09-03
// Created by: Yves FRICAUD
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


#include <Standard_Type.hxx>
#include <TopOpeBRepDS_Association.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_Association,Standard_Transient)

//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================
static Standard_Boolean Contains (const TopOpeBRepDS_ListOfInterference& LI,
				  const Handle(TopOpeBRepDS_Interference)& I)
{
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
    if (I->HasSameGeometry(it.Value())) return 1;
  }
  return 0;
}


//=======================================================================
//function : TopOpeBRepDS_Association
//purpose  : 
//=======================================================================

TopOpeBRepDS_Association::TopOpeBRepDS_Association()
{
}


//=======================================================================
//function : Associate
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Association::Associate(const Handle(TopOpeBRepDS_Interference)& I,
					 const Handle(TopOpeBRepDS_Interference)& K) 
{
  if (!myMap.IsBound(I)) {
    TopOpeBRepDS_ListOfInterference empty;
    myMap.Bind(I,empty);
    myMap(I).Append(K);
  }
  else if (!Contains(myMap(I),K)) {
    myMap(I).Append(K);
  }
  if (!myMap.IsBound(K)) {
    TopOpeBRepDS_ListOfInterference empty;
    myMap.Bind(K,empty);
    myMap(K).Append(I);
  }
  else if (!Contains(myMap(K),I)) {
    myMap(K).Append(I);
  }
}


//=======================================================================
//function : Associate
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Association::Associate(const Handle(TopOpeBRepDS_Interference)& I,
					 const TopOpeBRepDS_ListOfInterference& LI) 
{  
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(LI); it.More(); it.Next()) {
    Associate(I,it.Value());
  }
}

//=======================================================================
//function : HasAssociation
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Association::HasAssociation(const Handle(TopOpeBRepDS_Interference)& I) const
{
  return myMap.IsBound(I);
}


//=======================================================================
//function : Associated
//purpose  : 
//=======================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_Association::Associated
(const Handle(TopOpeBRepDS_Interference)& I)
{
  if (myMap.IsBound(I)) {
    return myMap.ChangeFind(I);
  }
  static TopOpeBRepDS_ListOfInterference empty;
  return empty;
}


//=======================================================================
//function : AreAssociated
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Association::AreAssociated(const Handle(TopOpeBRepDS_Interference)& I,
							 const Handle(TopOpeBRepDS_Interference)& K) const
{
  return (myMap.IsBound(I) && Contains(myMap(I),K));
}


