// Created on: 1993-11-18
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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



#include <ChFiDS_StripeMap.ixx>

//=======================================================================
//function : ChFiDS_StripeMap
//purpose  : 
//=======================================================================

ChFiDS_StripeMap::ChFiDS_StripeMap() 
{
}



//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void  ChFiDS_StripeMap::Add(const TopoDS_Vertex& V, 
			       const Handle(ChFiDS_Stripe)& F)
{
  Standard_Integer Index = mymap.FindIndex(V);
  if (Index==0) {
    ChFiDS_ListOfStripe Empty;
    Index = mymap.Add(V,Empty);
  }
  mymap(Index).Append(F);
}



//=======================================================================
//function : FindFromKey
//purpose  : 
//=======================================================================

const ChFiDS_ListOfStripe&  ChFiDS_StripeMap::FindFromKey(
						  const TopoDS_Vertex& V)const 
{
  return mymap.FindFromKey(V);
}



//=======================================================================
//function : FindFromIndex
//purpose  : 
//=======================================================================

const ChFiDS_ListOfStripe&  ChFiDS_StripeMap::
                               FindFromIndex(const Standard_Integer I)const 
{
  return mymap.FindFromIndex(I);
}

void ChFiDS_StripeMap::Clear()
{
  mymap.Clear();
}
