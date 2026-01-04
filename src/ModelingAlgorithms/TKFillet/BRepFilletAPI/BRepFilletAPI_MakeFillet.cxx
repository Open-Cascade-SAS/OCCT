// Created on: 1994-06-17
// Created by: Modeling
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

#include <BRepFilletAPI_MakeFillet.hxx>
#include <ChFiDS_ErrorStatus.hxx>
#include <ChFiDS_Spine.hxx>
#include <Geom_Surface.hxx>
#include <Law_Linear.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

//=================================================================================================

BRepFilletAPI_MakeFillet::BRepFilletAPI_MakeFillet(const TopoDS_Shape&      S,
                                                   const ChFi3d_FilletShape FShape)
    : myBuilder(S, FShape)
{
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetParams(const double Tang,
                                         const double Tesp,
                                         const double T2d,
                                         const double TApp3d,
                                         const double TolApp2d,
                                         const double Fleche)
{
  myBuilder.SetParams(Tang, Tesp, T2d, TApp3d, TolApp2d, Fleche);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetContinuity(const GeomAbs_Shape InternalContinuity,
                                             const double        AngleTol)
{
  myBuilder.SetContinuity(InternalContinuity, AngleTol);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const TopoDS_Edge& E)
{
  myBuilder.Add(E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const double Radius, const TopoDS_Edge& E)
{
  // myBuilder.Add(Radius,E);
  myBuilder.Add(E);
  int IinC;
  int IC = myBuilder.Contains(E, IinC);
  if (IC)
    SetRadius(Radius, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const double R1, const double R2, const TopoDS_Edge& E)
{
  myBuilder.Add(E);
  int IinC;
  int IC = myBuilder.Contains(E, IinC);
  if (IC)
    SetRadius(R1, R2, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const occ::handle<Law_Function>& L, const TopoDS_Edge& E)
{
  // myBuilder.Add(L,E);
  myBuilder.Add(E);
  int IinC;
  int IC = myBuilder.Contains(E, IinC);
  if (IC)
    SetRadius(L, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Add(const NCollection_Array1<gp_Pnt2d>& UandR, const TopoDS_Edge& E)
{
  myBuilder.Add(E);
  int IinC;
  int IC = myBuilder.Contains(E, IinC);
  if (IC)
    SetRadius(UandR, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const double Radius, const int IC, const int IinC)
{
  gp_XY FirstUandR(0., Radius), LastUandR(1., Radius);
  myBuilder.SetRadius(FirstUandR, IC, IinC);
  myBuilder.SetRadius(LastUandR, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const double R1,
                                         const double R2,
                                         const int    IC,
                                         const int    IinC)
{
  double r1, r2;

  if (std::abs(R1 - R2) < Precision::Confusion())
    r1 = r2 = (R1 + R2) * 0.5;
  else
  {
    r1 = R1;
    r2 = R2;
  }
  gp_XY FirstUandR(0., r1), LastUandR(1., r2);
  myBuilder.SetRadius(FirstUandR, IC, IinC);
  myBuilder.SetRadius(LastUandR, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const occ::handle<Law_Function>& L,
                                         const int                        IC,
                                         const int                        IinC)
{
  myBuilder.SetRadius(L, IC, IinC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const NCollection_Array1<gp_Pnt2d>& UandR,
                                         const int                           IC,
                                         const int                           IinC)
{
  if (UandR.Length() == 1)
    SetRadius(UandR(UandR.Lower()).Y(), IC, IinC);
  else if (UandR.Length() == 2)
    SetRadius(UandR(UandR.Lower()).Y(), UandR(UandR.Upper()).Y(), IC, IinC);
  else
  {
    double Uf = UandR(UandR.Lower()).X();
    double Ul = UandR(UandR.Upper()).X();
    for (int i = UandR.Lower(); i <= UandR.Upper(); i++)
    {
      double Ucur = UandR(i).X();
      Ucur        = (Ucur - Uf) / (Ul - Uf);
      gp_XY newUandR(Ucur, UandR(i).Y());
      myBuilder.SetRadius(newUandR, IC, IinC);
    }
  }
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::IsConstant(const int IC)
{
  return myBuilder.IsConstant(IC);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::Radius(const int IC)
{
  return myBuilder.Radius(IC);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::ResetContour(const int IC)
{
  myBuilder.ResetContour(IC);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::IsConstant(const int IC, const TopoDS_Edge& E)
{
  return myBuilder.IsConstant(IC, E);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::Radius(const int IC, const TopoDS_Edge& E)
{
  return myBuilder.Radius(IC, E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const double Radius, const int IC, const TopoDS_Edge& E)
{
  myBuilder.SetRadius(Radius, IC, E);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::GetBounds(const int IC, const TopoDS_Edge& E, double& F, double& L)
{
  return myBuilder.GetBounds(IC, E, F, L);
}

//=================================================================================================

occ::handle<Law_Function> BRepFilletAPI_MakeFillet::GetLaw(const int IC, const TopoDS_Edge& E)
{
  return myBuilder.GetLaw(IC, E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetLaw(const int                        IC,
                                      const TopoDS_Edge&               E,
                                      const occ::handle<Law_Function>& L)
{
  myBuilder.SetLaw(IC, E, L);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetRadius(const double Radius, const int IC, const TopoDS_Vertex& V)
{
  myBuilder.SetRadius(Radius, IC, V);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::SetFilletShape(const ChFi3d_FilletShape FShape)
{
  myBuilder.SetFilletShape(FShape);
}

//=================================================================================================

ChFi3d_FilletShape BRepFilletAPI_MakeFillet::GetFilletShape() const
{
  return myBuilder.GetFilletShape();
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbContours() const
{
  return myBuilder.NbElements();
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::Contour(const TopoDS_Edge& E) const
{
  return myBuilder.Contains(E);
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbEdges(const int I) const
{
  const occ::handle<ChFiDS_Spine>& Spine = myBuilder.Value(I);
  int                              n     = Spine->NbEdges();
  return n;
}

//=================================================================================================

const TopoDS_Edge& BRepFilletAPI_MakeFillet::Edge(const int I, const int J) const
{
  const occ::handle<ChFiDS_Spine>& Spine = myBuilder.Value(I);
  const TopoDS_Edge&               S     = Spine->Edges(J);
  return S;
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Remove(const TopoDS_Edge& E)
{
  myBuilder.Remove(E);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::Length(const int IC) const
{
  return myBuilder.Length(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::FirstVertex(const int IC) const
{
  return myBuilder.FirstVertex(IC);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::LastVertex(const int IC) const
{
  return myBuilder.LastVertex(IC);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::Abscissa(const int IC, const TopoDS_Vertex& V) const
{
  return myBuilder.Abscissa(IC, V);
}

//=================================================================================================

double BRepFilletAPI_MakeFillet::RelativeAbscissa(const int IC, const TopoDS_Vertex& V) const
{
  return myBuilder.RelativeAbscissa(IC, V);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::ClosedAndTangent(const int IC) const
{
  return myBuilder.ClosedAndTangent(IC);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::Closed(const int IC) const
{
  return myBuilder.Closed(IC);
}

//=================================================================================================

occ::handle<TopOpeBRepBuild_HBuilder> BRepFilletAPI_MakeFillet::Builder() const
{
  return myBuilder.Builder();
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Build(const Message_ProgressRange& /*theRange*/)
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

void BRepFilletAPI_MakeFillet::Reset()
{
  NotDone();
  myBuilder.Reset();
  myMap.Clear();
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbSurfaces() const
{
  return (myBuilder.Builder()->DataStructure())->NbSurfaces();
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeFillet::NewFaces(const int I)
{
  return (*(NCollection_List<TopoDS_Shape>*)&(myBuilder.Builder()->NewFaces(I)));
}

//=================================================================================================

void BRepFilletAPI_MakeFillet::Simulate(const int IC)
{
  myBuilder.Simulate(IC);
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbSurf(const int IC) const
{
  return myBuilder.NbSurf(IC);
}

//=================================================================================================

occ::handle<NCollection_HArray1<ChFiDS_CircSection>> BRepFilletAPI_MakeFillet::Sect(
  const int IC,
  const int IS) const
{
  return myBuilder.Sect(IC, IS);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeFillet::Generated(const TopoDS_Shape& EorV)
{
  return myBuilder.Generated(EorV);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeFillet::Modified(const TopoDS_Shape& F)
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

bool BRepFilletAPI_MakeFillet::IsDeleted(const TopoDS_Shape& F)
{
  return !(myMap.Contains(F) || myBuilder.Builder()->IsSplit(F, TopAbs_OUT)
           || myBuilder.Builder()->IsSplit(F, TopAbs_IN)
           || myBuilder.Builder()->IsSplit(F, TopAbs_ON));
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbFaultyContours() const
{
  return myBuilder.NbFaultyContours();
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::FaultyContour(const int I) const
{
  return myBuilder.FaultyContour(I);
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbComputedSurfaces(const int IC) const
{
  return myBuilder.NbComputedSurfaces(IC);
}

//=================================================================================================

occ::handle<Geom_Surface> BRepFilletAPI_MakeFillet::ComputedSurface(const int IC,
                                                                    const int IS) const
{
  return myBuilder.ComputedSurface(IC, IS);
}

//=================================================================================================

int BRepFilletAPI_MakeFillet::NbFaultyVertices() const
{
  return myBuilder.NbFaultyVertices();
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet::FaultyVertex(const int IV) const
{
  return myBuilder.FaultyVertex(IV);
}

//=================================================================================================

bool BRepFilletAPI_MakeFillet::HasResult() const
{
  return myBuilder.HasResult();
}

//=================================================================================================

TopoDS_Shape BRepFilletAPI_MakeFillet::BadShape() const
{
  return myBuilder.BadShape();
}

//=================================================================================================

ChFiDS_ErrorStatus BRepFilletAPI_MakeFillet::StripeStatus(const int IC) const
{
  return myBuilder.StripeStatus(IC);
}
