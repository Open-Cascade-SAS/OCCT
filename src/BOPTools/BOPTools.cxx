// Created by: Peter KURNEV
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


#include <BOPCol_ListOfShape.hxx>
#include <BOPTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================
void BOPTools::MapShapes(const TopoDS_Shape& S,
                         BOPCol_MapOfShape& M)
{
  M.Add(S);
  TopoDS_Iterator It(S);
  while (It.More()) {
    MapShapes(It.Value(),M);
    It.Next();
  }
}


//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================
void BOPTools::MapShapes(const TopoDS_Shape& S,
                         BOPCol_IndexedMapOfShape& M)
{
  M.Add(S);
  TopoDS_Iterator It(S);
  while (It.More()) {
    MapShapes(It.Value(),M);
    It.Next();
  }
}

//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================
void BOPTools::MapShapes(const TopoDS_Shape& S,
                         const TopAbs_ShapeEnum T,
                         BOPCol_IndexedMapOfShape& M)
{
  TopExp_Explorer Ex(S,T);
  while (Ex.More()) {
    M.Add(Ex.Current());
    Ex.Next();
  }
}
//=======================================================================
//function : MapShapesAndAncestors
//purpose  : 
//=======================================================================
void BOPTools::MapShapesAndAncestors
  (const TopoDS_Shape& S, 
   const TopAbs_ShapeEnum TS, 
   const TopAbs_ShapeEnum TA, 
   BOPCol_IndexedDataMapOfShapeListOfShape& aMEF)
{
  TopExp_Explorer aExS, aExA;
  //
  // visit ancestors
  aExA.Init(S, TA);
  while (aExA.More()) {
    // visit shapes
    const TopoDS_Shape& aF = aExA.Current();
    //
    aExS.Init(aF, TS);
    while (aExS.More()) {
      const TopoDS_Shape& aE= aExS.Current();
      if (aMEF.Contains(aE)) {
        aMEF.ChangeFromKey(aE).Append(aF);
      }
      else {
        BOPCol_ListOfShape aLS;
        aLS.Append(aF);
        aMEF.Add(aE, aLS);
      }
      aExS.Next();
    }
    aExA.Next();
  }
  //
  // visit shapes not under ancestors
  aExS.Init(S, TS, TA);
  while (aExS.More()) {
    const TopoDS_Shape& aE=aExS.Current();
    BOPCol_ListOfShape aLS;
    aMEF.Add(aE, aLS);
    aExS.Next();
  }
}
