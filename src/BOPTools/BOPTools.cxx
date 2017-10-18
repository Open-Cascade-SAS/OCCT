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
   BOPCol_IndexedDataMapOfShapeListOfShape& Map)
{
  // visit ancestors
  TopExp_Explorer aExA(S, TA);
  for (; aExA.More(); aExA.Next())
  {
    // visit shapes
    const TopoDS_Shape& anAnc = aExA.Current();
    //
    TopExp_Explorer aExS(anAnc, TS);
    for (; aExS.More(); aExS.Next())
    {
      const TopoDS_Shape& aSS = aExS.Current();
      BOPCol_ListOfShape* pLA = Map.ChangeSeek(aSS);
      if (!pLA)
        pLA = &Map(Map.Add(aSS, BOPCol_ListOfShape()));
      pLA->Append(anAnc);
    }
  }

  // visit shapes not under ancestors
  TopExp_Explorer aExS(S, TS, TA);
  for (; aExS.More(); aExS.Next())
    Map.Add(aExS.Current(), BOPCol_ListOfShape());
}
