// Created on: 1994-12-09
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepLib.hxx>
#include <BRepTools_NurbsConvertModification.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>

//#include <gp.hxx>
//=======================================================================
//function : BRepBuilderAPI_NurbsConvert
//purpose  : 
//=======================================================================
BRepBuilderAPI_NurbsConvert::BRepBuilderAPI_NurbsConvert () 
     
{
  myModification = new BRepTools_NurbsConvertModification();
}

//=======================================================================
//function : BRepBuilderAPI_NurbsConvert
//purpose  : 
//=======================================================================

BRepBuilderAPI_NurbsConvert::BRepBuilderAPI_NurbsConvert (const TopoDS_Shape& S,
                                                          const Standard_Boolean Copy) 
     
{
  myModification = new BRepTools_NurbsConvertModification();
  Perform(S,Copy);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_NurbsConvert::Perform(const TopoDS_Shape& S,
                                          const Standard_Boolean /*Copy*/)
{
  Handle(BRepTools_NurbsConvertModification) theModif = 
    Handle(BRepTools_NurbsConvertModification)::DownCast(myModification);
  DoModif(S,myModification);
  CorrectVertexTol();
}


//=======================================================================
//function : CorrectVertexTol
//purpose  : 
//=======================================================================

void BRepBuilderAPI_NurbsConvert::CorrectVertexTol()
{
  TopTools_MapOfShape anInitVertices;
  TopExp_Explorer anExp(myInitialShape, TopAbs_VERTEX);
  for(; anExp.More(); anExp.Next())
  {
    anInitVertices.Add(anExp.Current());
  }
  //
  Handle(BRepTools_NurbsConvertModification) aModif = 
    Handle(BRepTools_NurbsConvertModification)::DownCast(myModification);

  BRep_Builder aBB;
  myVtxToReplace.Clear();
  TopTools_ListIteratorOfListOfShape anEIter(aModif->GetUpdatedEdges());
  for(; anEIter.More(); anEIter.Next())
  {
    const TopoDS_Shape& anE = anEIter.Value();
    //
    Standard_Real anETol = BRep_Tool::Tolerance(TopoDS::Edge(anE));
    TopoDS_Iterator anIter(anE);
    for(; anIter.More(); anIter.Next())
    {
      const TopoDS_Vertex& aVtx = TopoDS::Vertex(anIter.Value());
      if(anInitVertices.Contains(aVtx))
      {
        if(myVtxToReplace.IsBound(aVtx))
        {
          aBB.UpdateVertex(TopoDS::Vertex(myVtxToReplace(aVtx)), anETol + Epsilon(anETol));
        }
        else
        {
          Standard_Real aVTol = BRep_Tool::Tolerance(aVtx);
          if(aVTol < anETol)
          {
            TopoDS_Vertex aNewVtx;
            gp_Pnt aVPnt = BRep_Tool::Pnt(aVtx);
            aBB.MakeVertex(aNewVtx, aVPnt,anETol + Epsilon(anETol));
            aNewVtx.Orientation(aVtx.Orientation());
            myVtxToReplace.Bind(aVtx, aNewVtx);
          }
        }
      }
      else
      {
        aBB.UpdateVertex(aVtx, anETol + Epsilon(anETol));
      }
    }
  }
  //
  if(myVtxToReplace.IsEmpty())
  {
    return;
  }
  //
  mySubs.Clear();
  TopTools_DataMapIteratorOfDataMapOfShapeShape anIter(myVtxToReplace);
  for(; anIter.More(); anIter.Next())
  {
    mySubs.Replace(anIter.Key(), anIter.Value());
  }
  mySubs.Apply( myShape );
  myShape = mySubs.Value(myShape);
  //
}

//=======================================================================
//function : ModifiedShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepBuilderAPI_NurbsConvert::ModifiedShape
                                           (const TopoDS_Shape& S) const
{
 if(S.ShapeType() == TopAbs_VERTEX)
 {
   if(myVtxToReplace.IsBound(S))
   {
     return myVtxToReplace(S);
   }
 }
 if(myVtxToReplace.IsEmpty())
 {
  return myModifier.ModifiedShape(S);
 }
 else
 {
   const TopoDS_Shape& aNS = myModifier.ModifiedShape(S);
   return mySubs.Value(aNS);
 }
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_NurbsConvert::Modified
                                                  (const TopoDS_Shape& F)
{
  myGenerated.Clear();
  if(F.ShapeType() == TopAbs_VERTEX)
  {
    if(myVtxToReplace.IsBound(F))
    {
      myGenerated.Append(myVtxToReplace(F));
    }
    else
    {
      myGenerated.Append(myModifier.ModifiedShape(F));
    }
  }
  else
  {
    if(myVtxToReplace.IsEmpty())
    {
      myGenerated.Append(myModifier.ModifiedShape(F));
    }
    else
    {
      const TopoDS_Shape& aNS = myModifier.ModifiedShape(F);
      myGenerated.Append(mySubs.Value(aNS));
    }
  }
  return myGenerated;
}
