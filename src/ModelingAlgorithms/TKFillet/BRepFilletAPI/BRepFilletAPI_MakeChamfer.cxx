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

void BRepFilletAPI_MakeChamfer::Add(const double Dis, const TopoDS_Edge& E)
{
  myBuilder.Add(Dis, E);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::SetDist(const double Dis, const int IC, const TopoDS_Face& F)
{
  myBuilder.SetDist(Dis, IC, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::GetDist(const int IC, double& Dis) const
{
  myBuilder.GetDist(IC, Dis);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Add(const double       Dis1,
                                    const double       Dis2,
                                    const TopoDS_Edge& E,
                                    const TopoDS_Face& F)
{
  myBuilder.Add(Dis1, Dis2, E, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::SetDists(const double       Dis1,
                                         const double       Dis2,
                                         const int          IC,
                                         const TopoDS_Face& F)
{
  myBuilder.SetDists(Dis1, Dis2, IC, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Dists(const int IC, double& Dis1, double& Dis2) const
{
  double temp1, temp2;
  myBuilder.Dists(IC, temp1, temp2);
  Dis1 = temp1;
  Dis2 = temp2;
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::AddDA(const double       Dis,
                                      const double       Angle,
                                      const TopoDS_Edge& E,
                                      const TopoDS_Face& F)
{
  myBuilder.AddDA(Dis, Angle, E, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::SetDistAngle(const double       Dis,
                                             const double       Angle,
                                             const int          IC,
                                             const TopoDS_Face& F)
{
  myBuilder.SetDistAngle(Dis, Angle, IC, F);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::GetDistAngle(const int IC, double& Dis, double& Angle) const
{
  myBuilder.GetDistAngle(IC, Dis, Angle);
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::SetMode(const ChFiDS_ChamfMode theMode)
{
  myBuilder.SetMode(theMode);
}

//=================================================================================================

bool BRepFilletAPI_MakeChamfer::IsSymetric(const int IC) const
{
  ChFiDS_ChamfMethod ChamfMeth = myBuilder.IsChamfer(IC);
  bool               ret       = false;

  if (ChamfMeth == ChFiDS_Sym)
    ret = true;

  return ret;
}

//=================================================================================================

bool BRepFilletAPI_MakeChamfer::IsTwoDistances(const int IC) const
{
  ChFiDS_ChamfMethod ChamfMeth = myBuilder.IsChamfer(IC);
  bool               ret       = false;

  if (ChamfMeth == ChFiDS_TwoDist)
    ret = true;

  return ret;
}

//=================================================================================================

bool BRepFilletAPI_MakeChamfer::IsDistanceAngle(const int IC) const
{
  ChFiDS_ChamfMethod ChamfMeth = myBuilder.IsChamfer(IC);
  bool               ret       = false;

  if (ChamfMeth == ChFiDS_DistAngle)
    ret = true;

  return ret;
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::ResetContour(const int IC)
{
  myBuilder.ResetContour(IC);
}

//=================================================================================================

int BRepFilletAPI_MakeChamfer::NbContours() const
{
  return myBuilder.NbElements();
}

//=================================================================================================

int BRepFilletAPI_MakeChamfer::Contour(const TopoDS_Edge& E) const
{
  return myBuilder.Contains(E);
}

//=================================================================================================

int BRepFilletAPI_MakeChamfer::NbEdges(const int I) const

{
  const occ::handle<ChFiDS_Spine>& Spine = myBuilder.Value(I);
  return (Spine->NbEdges());
}

//=================================================================================================

const TopoDS_Edge& BRepFilletAPI_MakeChamfer::Edge(const int I, const int J) const
{
  const occ::handle<ChFiDS_Spine>& Spine = myBuilder.Value(I);
  const TopoDS_Edge&               E     = Spine->Edges(J);
  return E;
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Remove(const TopoDS_Edge& E)
{
  myBuilder.Remove(E);
}

//=================================================================================================

double BRepFilletAPI_MakeChamfer::Length(const int IC) const
{
  return myBuilder.Length(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeChamfer::FirstVertex(const int IC) const
{
  return myBuilder.FirstVertex(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeChamfer::LastVertex(const int IC) const
{
  return myBuilder.LastVertex(IC);
}

//=================================================================================================

double BRepFilletAPI_MakeChamfer::Abscissa(const int IC, const TopoDS_Vertex& V) const
{
  return myBuilder.Abscissa(IC, V);
}

//=================================================================================================

double BRepFilletAPI_MakeChamfer::RelativeAbscissa(const int IC, const TopoDS_Vertex& V) const
{
  return myBuilder.RelativeAbscissa(IC, V);
}

//=================================================================================================

bool BRepFilletAPI_MakeChamfer::ClosedAndTangent(const int IC) const
{
  return myBuilder.ClosedAndTangent(IC);
}

//=================================================================================================

bool BRepFilletAPI_MakeChamfer::Closed(const int IC) const
{
  return myBuilder.Closed(IC);
}

//=================================================================================================

occ::handle<TopOpeBRepBuild_HBuilder> BRepFilletAPI_MakeChamfer::Builder() const
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

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeChamfer::Generated(const TopoDS_Shape& EorV)
{
  return myBuilder.Generated(EorV);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeChamfer::Modified(const TopoDS_Shape& F)
{
  myGenerated.Clear();

  if (myBuilder.Builder()->IsSplit(F, TopAbs_OUT))
  {
    NCollection_List<TopoDS_Shape>::Iterator It(myBuilder.Builder()->Splits(F, TopAbs_OUT));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_IN))
  {
    NCollection_List<TopoDS_Shape>::Iterator It(myBuilder.Builder()->Splits(F, TopAbs_IN));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_ON))
  {
    NCollection_List<TopoDS_Shape>::Iterator It(myBuilder.Builder()->Splits(F, TopAbs_ON));
    for (; It.More(); It.Next())
    {
      myGenerated.Append(It.Value());
    }
  }
  return myGenerated;
}

//=================================================================================================

bool BRepFilletAPI_MakeChamfer::IsDeleted(const TopoDS_Shape& F)
{
  if (myMap.Contains(F) || myBuilder.Builder()->IsSplit(F, TopAbs_OUT)
      || myBuilder.Builder()->IsSplit(F, TopAbs_IN) || myBuilder.Builder()->IsSplit(F, TopAbs_ON))
    return false;

  return true;
}

//=================================================================================================

void BRepFilletAPI_MakeChamfer::Simulate(const int IC)
{
  myBuilder.Simulate(IC);
}

//=================================================================================================

int BRepFilletAPI_MakeChamfer::NbSurf(const int IC) const
{
  return myBuilder.NbSurf(IC);
}

//=================================================================================================

occ::handle<NCollection_HArray1<ChFiDS_CircSection>> BRepFilletAPI_MakeChamfer::Sect(
  const int IC,
  const int IS) const
{
  return myBuilder.Sect(IC, IS);
}
