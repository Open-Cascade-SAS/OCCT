// Created on: 1998-09-03
// Created by: Yves FRICAUD
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



#include <TopOpeBRepDS_Association.ixx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>


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


