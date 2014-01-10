// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <BOPCol_Box2DBndTree.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPCol_Box2DBndTreeSelector::BOPCol_Box2DBndTreeSelector()
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPCol_Box2DBndTreeSelector::~BOPCol_Box2DBndTreeSelector()
{
}
//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================
Standard_Boolean BOPCol_Box2DBndTreeSelector::Reject
  (const Bnd_Box2d& aBox2D) const
{
  return myBox2D.IsOut(aBox2D);
}
//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================
Standard_Boolean BOPCol_Box2DBndTreeSelector::Accept 
  (const Standard_Integer& aIndex)
{
  Standard_Boolean bRet=Standard_False;
  //
  myIndices.Append(aIndex);
  bRet=!bRet;
  //}
  return bRet;
}
//=======================================================================
//function : SetBox
//purpose  : 
//=======================================================================
void BOPCol_Box2DBndTreeSelector::SetBox(const Bnd_Box2d& aBox2D)
{
  myBox2D=aBox2D;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void BOPCol_Box2DBndTreeSelector::Clear()
{
  myIndices.Clear();
}
//=======================================================================
//function : Indices
//purpose  : 
//=======================================================================
const BOPCol_ListOfInteger& BOPCol_Box2DBndTreeSelector::Indices() const
{
  return myIndices;
}
