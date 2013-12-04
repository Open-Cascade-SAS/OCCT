// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPDS_BoxBndTree.hxx>
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPDS_BoxBndTreeSelector::BOPDS_BoxBndTreeSelector()
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BOPDS_BoxBndTreeSelector::~BOPDS_BoxBndTreeSelector()
{
}
//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_BoxBndTreeSelector::Reject (const Bnd_Box& aBox) const
{
  return myBox.IsOut(aBox);
}
//=======================================================================
//function : Accept
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_BoxBndTreeSelector::Accept (const Standard_Integer& aIndex)
{
  Standard_Boolean bRet=Standard_False;
  //
  //if (myFence.Add(aIndex)) {
  myIndices.Append(aIndex);
  bRet=!bRet;
  //}
  return bRet;
}
//=======================================================================
//function : SetBox
//purpose  : 
//=======================================================================
  void BOPDS_BoxBndTreeSelector::SetBox(const Bnd_Box& aBox)
{
  myBox=aBox;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
  void BOPDS_BoxBndTreeSelector::Clear()
{
  //myFence.Clear();
  myIndices.Clear();
}
//=======================================================================
//function : Indices
//purpose  : 
//=======================================================================
  const BOPCol_ListOfInteger& BOPDS_BoxBndTreeSelector::Indices() const
{
  return myIndices;
}
