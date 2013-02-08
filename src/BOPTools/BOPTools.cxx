// Created by: Peter KURNEV
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


#include <BOPTools.ixx>
#include <TopExp_Explorer.hxx>
#include <BOPCol_ListOfShape.hxx>

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
  void BOPTools::MapShapesAndAncestors(const TopoDS_Shape& S, 
                                       const TopAbs_ShapeEnum TS, 
                                       const TopAbs_ShapeEnum TA, 
                                       BOPCol_IndexedDataMapOfShapeListOfShape& M)
{
  BOPCol_ListOfShape empty;
  
  // visit ancestors
  TopExp_Explorer exa(S,TA);
  while (exa.More()) {
    // visit shapes
    const TopoDS_Shape& anc = exa.Current();
    TopExp_Explorer exs(anc,TS);
    while (exs.More()) {
      Standard_Integer index = M.FindIndex(exs.Current());
      if (index == 0) index = M.Add(exs.Current(),empty);
      M(index).Append(anc);
      exs.Next();
    }
    exa.Next();
  }
  
  // visit shapes not under ancestors
  TopExp_Explorer ex(S,TS,TA);
  while (ex.More()) {
    Standard_Integer index = M.FindIndex(ex.Current());
    if (index == 0) index = M.Add(ex.Current(),empty);
    ex.Next();
  }
}
