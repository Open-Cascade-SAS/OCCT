// Created on: 1995-06-22
// Created by: Flore Lantheaume
// Copyright (c) 1995-1999 Matra Datavision
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

#include <BRepFilletAPI_MakeChamfer.hxx>
#include <ChFiDS_Spine.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

//=================================================================================================

BRepFilletAPI_MakeChamfer::BRepFilletAPI_MakeChamfer(const TopoDS_Shape& S)
    : myBuilder(S)
{
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Add(const TopoDS_Edge& E)
{
  myBuilder.Add(E);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Add(const Standard_Real Dis, const TopoDS_Edge& E)
{
  myBuilder.Add(Dis, E);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::SetDist(const Standard_Real    Dis,
                                        const Standard_Integer IC,
                                        const TopoDS_Face&     F)
{
  myBuilder.SetDist(Dis, IC, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::GetDist(const Standard_Integer IC, Standard_Real& Dis) const
{
  myBuilder.GetDist(IC, Dis);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Add(const Standard_Real Dis1,
                                    const Standard_Real Dis2,
                                    const TopoDS_Edge&  E,
                                    const TopoDS_Face&  F)
{
  myBuilder.Add(Dis1, Dis2, E, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::SetDists(const Standard_Real    Dis1,
                                         const Standard_Real    Dis2,
                                         const Standard_Integer IC,
                                         const TopoDS_Face&     F)
{
  myBuilder.SetDists(Dis1, Dis2, IC, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Dists(const Standard_Integer IC,
                                      Standard_Real&         Dis1,
                                      Standard_Real&         Dis2) const
{
  Standard_Real temp1, temp2;
  myBuilder.Dists(IC, temp1, temp2);
  Dis1 = temp1;
  Dis2 = temp2;
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::AddDA(const Standard_Real Dis,
                                      const Standard_Real Angle,
                                      const TopoDS_Edge&  E,
                                      const TopoDS_Face&  F)
{
  myBuilder.AddDA(Dis, Angle, E, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::SetDistAngle(const Standard_Real    Dis,
                                             const Standard_Real    Angle,
                                             const Standard_Integer IC,
                                             const TopoDS_Face&     F)
{
  myBuilder.SetDistAngle(Dis, Angle, IC, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::GetDistAngle(const Standard_Integer IC,
                                             Standard_Real&         Dis,
                                             Standard_Real&         Angle) const
{
  myBuilder.GetDistAngle(IC, Dis, Angle);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::SetMode(const ChFiDS_ChamfMode theMode)
{
  myBuilder.SetMode(theMode);
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::IsSymetric(const Standard_Integer IC) const
{
  ChFiDS_ChamfMethod ChamfMeth = myBuilder.IsChamfer(IC);
  Standard_Boolean   ret       = Standard_False;

  if (ChamfMeth == ChFiDS_Sym)
    ret = Standard_True;

  return ret;
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::IsTwoDistances(const Standard_Integer IC) const
{
  ChFiDS_ChamfMethod ChamfMeth = myBuilder.IsChamfer(IC);
  Standard_Boolean   ret       = Standard_False;

  if (ChamfMeth == ChFiDS_TwoDist)
    ret = Standard_True;

  return ret;
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::IsDistanceAngle(const Standard_Integer IC) const
{
  ChFiDS_ChamfMethod ChamfMeth = myBuilder.IsChamfer(IC);
  Standard_Boolean   ret       = Standard_False;

  if (ChamfMeth == ChFiDS_DistAngle)
    ret = Standard_True;

  return ret;
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::ResetContour(const Standard_Integer IC)
{
  myBuilder.ResetContour(IC);
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeChamfer::NbContours() const
{
  return myBuilder.NbElements();
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeChamfer::Contour(const TopoDS_Edge& E) const
{
  return myBuilder.Contains(E);
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeChamfer::NbEdges(const Standard_Integer I) const

{
  const Handle(ChFiDS_Spine)& Spine = myBuilder.Value(I);
  return (Spine->NbEdges());
}

//=================================================================================================

const TopoDS_Edge& BRepFilletAPI_MakeChamfer::Edge(const Standard_Integer I,
                                                   const Standard_Integer J) const
{
  const Handle(ChFiDS_Spine)& Spine = myBuilder.Value(I);
  const TopoDS_Edge&          E     = Spine->Edges(J);
  return E;
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Remove(const TopoDS_Edge& E)
{
  myBuilder.Remove(E);
}

//=================================================================================================

Standard_Real BRepFilletAPI_MakeChamfer::Length(const Standard_Integer IC) const
{
  return myBuilder.Length(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeChamfer::FirstVertex(const Standard_Integer IC) const
{
  return myBuilder.FirstVertex(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeChamfer::LastVertex(const Standard_Integer IC) const
{
  return myBuilder.LastVertex(IC);
}

//=================================================================================================

Standard_Real BRepFilletAPI_MakeChamfer::Abscissa(const Standard_Integer IC,
                                                  const TopoDS_Vertex&   V) const
{
  return myBuilder.Abscissa(IC, V);
}

//=================================================================================================

Standard_Real BRepFilletAPI_MakeChamfer::RelativeAbscissa(const Standard_Integer IC,
                                                          const TopoDS_Vertex&   V) const
{
  return myBuilder.RelativeAbscissa(IC, V);
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::ClosedAndTangent(const Standard_Integer IC) const
{
  return myBuilder.ClosedAndTangent(IC);
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::Closed(const Standard_Integer IC) const
{
  return myBuilder.Closed(IC);
}

//=================================================================================================

Handle(TopOpeBRepBuild_HBuilder) BRepFilletAPI_MakeChamfer::Builder() const
{
  return myBuilder.Builder();
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Build(const Message_ProgressRange& /*theRange*/)
{
  myBuilder.Compute();
  if (myBuilder.IsDone())
  {
    Done();
    myShape = myBuilder.Shape();

    // creation of the Map.
    TopExp_Explorer ex;
    for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next())
    {
      myMap.Add(ex.Current());
    }
  }
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Reset()
{
  NotDone();
  myBuilder.Reset();
  myMap.Clear();
}

//=================================================================================================

const TopTools_ListOfShape& BRepFilletAPI_MakeChamfer::Generated(const TopoDS_Shape& EorV)
{
  return myBuilder.Generated(EorV);
}

//=================================================================================================

const TopTools_ListOfShape& BRepFilletAPI_MakeChamfer::Modified(const TopoDS_Shape& F)
{
  myGenerated.Clear();

  if (myBuilder.Builder()->IsSplit(F, TopAbs_OUT))
  {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_OUT));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_IN))
  {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_IN));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_ON))
  {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_ON));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  return myGenerated;
}

//=================================================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::IsDeleted(const TopoDS_Shape& F)
{
  if (myMap.Contains(F) || myBuilder.Builder()->IsSplit(F, TopAbs_OUT)
      || myBuilder.Builder()->IsSplit(F, TopAbs_IN) || myBuilder.Builder()->IsSplit(F, TopAbs_ON))
    return Standard_False;

  return Standard_True;
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Simulate(const Standard_Integer IC)
{
  myBuilder.Simulate(IC);
}

//=================================================================================================

Standard_Integer BRepFilletAPI_MakeChamfer::NbSurf(const Standard_Integer IC) const
{
  return myBuilder.NbSurf(IC);
}

//=================================================================================================

Handle(ChFiDS_SecHArray1) BRepFilletAPI_MakeChamfer::Sect(const Standard_Integer IC,
                                                          const Standard_Integer IS) const
{
  return myBuilder.Sect(IC, IS);
}
