// Created by: Peter KURNEV
// Copyright (c) 2010-2012 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
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
