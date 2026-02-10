// Created on: 1996-09-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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
#include <BRep_Tool.hxx>
#include <BSplCLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomConvert.hxx>
#include <gp_Pln.hxx>
#include <LocOpe.hxx>
#include <LocOpe_BuildShape.hxx>
#include <LocOpe_Pipe.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>

static TopAbs_Orientation Orientation(const TopoDS_Shape&, const TopoDS_Shape&);

//=================================================================================================

LocOpe_Pipe::LocOpe_Pipe(const TopoDS_Wire& Spine, const TopoDS_Shape& Profile)
    : myPipe(Spine, Profile)
{

  TopoDS_Shape Result = myPipe.Shape();

  // On enleve les faces generees par les edges de connexite du profile,
  // et on fusionne les plans si possible

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    theEFMap;
  TopExp::MapShapesAndAncestors(Profile, TopAbs_EDGE, TopAbs_FACE, theEFMap);
  TopExp_Explorer                          exp;
  NCollection_List<TopoDS_Shape>           Empty;
  NCollection_List<TopoDS_Shape>::Iterator it;

  NCollection_List<TopoDS_Shape> goodfaces;

  for (int i = 1; i <= theEFMap.Extent(); i++)
  {
    const TopoDS_Edge& edgpr = TopoDS::Edge(theEFMap.FindKey(i));
    myMap.Bind(edgpr, Empty);
    if (theEFMap(i).Extent() >= 2)
    {
      // on ne prend pas les faces generees
    }
    else
    {
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>
        MapFac; // on mappe les plans generes par cet edge
      for (exp.Init(Spine, TopAbs_EDGE); exp.More(); exp.Next())
      {
        const TopoDS_Edge& edgsp  = TopoDS::Edge(exp.Current());
        TopoDS_Face        resfac = myPipe.Face(edgsp, edgpr);
        if (!resfac.IsNull())
        {
          occ::handle<Geom_Surface> P = BRep_Tool::Surface(resfac);
          if (P->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
          {
            P = occ::down_cast<Geom_RectangularTrimmedSurface>(P)->BasisSurface();
          }
          if (P->DynamicType() == STANDARD_TYPE(Geom_Plane))
          {
            MapFac.Add(resfac);
          }
          else
          {
            myMap(edgpr).Append(resfac);
            goodfaces.Append(resfac);
          }
        }
      }

      // Chercher les composantes connexes sur cet ensemble de faces., avec meme
      // support geometrique

      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itm(MapFac);
      if (MapFac.Extent() <= 1)
      { // un seul plan. Rien a faire
        if (MapFac.Extent() == 1)
        {
          myMap(edgpr).Append(itm.Key());
          goodfaces.Append(itm.Key());
        }
        continue;
      }

      while (MapFac.Extent() >= 2)
      {
        itm = NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator(MapFac);
        NCollection_List<TopoDS_Shape> FacFuse;
        TopoDS_Face                    FaceRef = TopoDS::Face(itm.Key());
        FacFuse.Append(FaceRef);
        occ::handle<Geom_Surface> P = BRep_Tool::Surface(FaceRef);
        if (P->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
        {
          P = occ::down_cast<Geom_RectangularTrimmedSurface>(P)->BasisSurface();
        }
        gp_Pln Plref = occ::down_cast<Geom_Plane>(P)->Pln();

        for (itm.Next(); itm.More(); itm.Next())
        {
          P = BRep_Tool::Surface(TopoDS::Face(itm.Key()));
          if (P->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
          {
            P = occ::down_cast<Geom_RectangularTrimmedSurface>(P)->BasisSurface();
          }
          gp_Pln Pl = occ::down_cast<Geom_Plane>(P)->Pln();
          if (Pl.Axis().IsParallel(Plref.Axis(), Precision::Angular())
              && Plref.Contains(Pl.Location(), Precision::Confusion()))
          {
            FacFuse.Append(itm.Key());
          }
        }

        // FacFuse contient des faces de meme support. Il faut en faire
        // des composantes connexes

        while (FacFuse.Extent() >= 2)
        {
          FaceRef = TopoDS::Face(FacFuse.First());
          // Recuperer l'orientation
          TopAbs_Orientation orref = Orientation(FaceRef, Result);
          P                        = BRep_Tool::Surface(FaceRef);
          if (P->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
          {
            P = occ::down_cast<Geom_RectangularTrimmedSurface>(P)->BasisSurface();
          }
          Plref = occ::down_cast<Geom_Plane>(P)->Pln();
          gp_Dir Dirref(Plref.Axis().Direction());
          if ((Plref.Direct() && orref == TopAbs_REVERSED)
              || (!Plref.Direct() && orref == TopAbs_FORWARD))
          {
            Dirref.Reverse();
          }

          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MapEd;
          for (exp.Init(FaceRef.Oriented(TopAbs_FORWARD), TopAbs_EDGE); exp.More(); exp.Next())
          {
            MapEd.Add(exp.Current());
          }

          MapFac.Remove(FaceRef);
          FacFuse.RemoveFirst(); // on enleve FaceRef
          bool FaceToFuse = false;
          bool MoreFound;

          do
          {
            MoreFound = false;
            for (it.Initialize(FacFuse); it.More(); it.Next())
            {
              for (exp.Init(it.Value(), TopAbs_EDGE); exp.More(); exp.Next())
              {
                if (MapEd.Contains(exp.Current()))
                {
                  FaceToFuse = true;
                  MoreFound  = true;
                  break;
                }
              }
              if (exp.More())
              {
                break;
              }
            }
            if (MoreFound)
            {
              const TopoDS_Face&        fac     = TopoDS::Face(it.Value());
              TopAbs_Orientation        orrelat = Orientation(fac, Result);
              occ::handle<Geom_Surface> OtherP  = BRep_Tool::Surface(fac);
              if (OtherP->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
              {
                OtherP = occ::down_cast<Geom_RectangularTrimmedSurface>(OtherP)->BasisSurface();
              }
              gp_Pln Pl = occ::down_cast<Geom_Plane>(OtherP)->Pln();
              gp_Dir Dirpl(Pl.Axis().Direction());
              if ((Pl.Direct() && orrelat == TopAbs_REVERSED)
                  || (!Plref.Direct() && orrelat == TopAbs_FORWARD))
              {
                Dirpl.Reverse();
              }
              if (Dirpl.Dot(Dirref) > 0)
              {
                orrelat = TopAbs_FORWARD;
              }
              else
              {
                orrelat = TopAbs_REVERSED;
              }
              for (exp.Init(fac.Oriented(orrelat), TopAbs_EDGE); exp.More(); exp.Next())
              {
                if (!MapEd.Add(exp.Current()))
                {
                  MapEd.Remove(exp.Current());
                }
              }
              MapFac.Remove(fac);
              FacFuse.Remove(it);
            }
          } while (MoreFound);

          if (FaceToFuse)
          {
            TopoDS_Face  NewFace;
            BRep_Builder B;
            B.MakeFace(NewFace, P, BRep_Tool::Tolerance(FaceRef));
            TopoDS_Wire NewWire;
            B.MakeWire(NewWire);
            for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itm2(MapEd);
                 itm2.More();
                 itm2.Next())
            {
              B.Add(NewWire, itm2.Key());
            }
            exp.Init(FaceRef.Oriented(TopAbs_FORWARD), TopAbs_WIRE);
            NewWire.Orientation(exp.Current().Orientation());
            B.Add(NewFace, NewWire);
            myMap(edgpr).Append(NewFace);
            goodfaces.Append(NewFace);
          }
        }
        if (FacFuse.Extent() == 1)
        {
          MapFac.Remove(FacFuse.First());
          myMap(edgpr).Append(FacFuse.First());
          goodfaces.Append(FacFuse.First());
        }
      }
    }
  }

  for (exp.Init(myPipe.FirstShape(), TopAbs_FACE); exp.More(); exp.Next())
  {
    goodfaces.Append(exp.Current());
  }
  for (exp.Init(myPipe.LastShape(), TopAbs_FACE); exp.More(); exp.Next())
  {
    goodfaces.Append(exp.Current());
  }

  LocOpe_BuildShape BS(goodfaces);
  myRes = BS.Shape();
}

//=================================================================================================

const TopoDS_Shape& LocOpe_Pipe::Shape() const
{
  return myRes;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& LocOpe_Pipe::Shapes(const TopoDS_Shape& S)
{
  TopAbs_ShapeEnum typS = S.ShapeType();
  if (typS != TopAbs_EDGE && typS != TopAbs_VERTEX)
  {
    throw Standard_DomainError();
  }
  //  for (TopExp_Explorer exp(myPipe.Profile(),typS); exp.More(); exp.Next()) {
  TopExp_Explorer exp(myPipe.Profile(), typS);
  for (; exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(S))
    {
      break;
    }
  }
  if (!exp.More())
  {
    throw Standard_NoSuchObject();
  }

  myGShap.Clear();
  if (typS == TopAbs_VERTEX)
  {
    const TopoDS_Vertex& VProfile = TopoDS::Vertex(S);
    for (exp.Init(myPipe.Spine(), TopAbs_EDGE); exp.More(); exp.Next())
    {
      const TopoDS_Edge& edsp  = TopoDS::Edge(exp.Current());
      TopoDS_Edge        resed = myPipe.Edge(edsp, VProfile);
      if (!resed.IsNull())
      {
        myGShap.Append(resed);
      }
    }
    return myGShap;
  }
  // TopAbs_EDGE
  const TopoDS_Edge& EProfile = TopoDS::Edge(S);
  return myMap(EProfile);
}

//=================================================================================================

const NCollection_Sequence<occ::handle<Geom_Curve>>& LocOpe_Pipe::Curves(
  const NCollection_Sequence<gp_Pnt>& Spt)
{

  myCrvs.Clear();
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Map;

  int    i, j, k, Nbpnt = Spt.Length();
  double p1, p2;
  //  gp_Pnt ptbid;

  for (i = 1; i <= Nbpnt; i++)
  {
    gp_Pnt                                        P1     = Spt(i);
    int                                           MaxDeg = 0;
    NCollection_Sequence<occ::handle<Geom_Curve>> seq;
    TopoDS_Wire                                   W = myPipe.PipeLine(P1);

    TopExp_Explorer ex(W, TopAbs_EDGE);
    for (; ex.More(); ex.Next())
    {
      occ::handle<Geom_Curve>        C1 = BRep_Tool::Curve(TopoDS::Edge(ex.Current()), p1, p2);
      occ::handle<Geom_BSplineCurve> C  = GeomConvert::CurveToBSplineCurve(C1);
      if (C.IsNull())
      {
        continue;
      }
      MaxDeg = std::max(MaxDeg, C->Degree());
      P1     = C->Value(p2);
      if (p1 != C->FirstParameter() || p2 != C->LastParameter())
      {
        C->Segment(p1, p2);
      }
      NCollection_Array1<double> Tkn(C->Knots());
      BSplCLib::Reparametrize(seq.Length(), seq.Length() + 1, Tkn);
      C->SetKnots(Tkn);
      seq.Append(C);
    }

    occ::handle<Geom_Curve> newC;
    int                     Nbkn = 0, Nbp = 0;
    int                     Nbcurv = seq.Length();
    if (Nbcurv == 0)
    {
      myCrvs.Append(newC);
      continue;
    }

    occ::handle<Geom_BSplineCurve> Bsp;
    for (j = 1; j <= Nbcurv; j++)
    {
      Bsp = occ::down_cast<Geom_BSplineCurve>(seq(j));
      Bsp->IncreaseDegree(MaxDeg);
      Nbp += Bsp->NbPoles();
      Nbkn += Bsp->NbKnots();
    }
    Nbp -= Nbcurv - 1;
    Nbkn -= Nbcurv - 1;
    NCollection_Array1<double> Tkn(1, Nbkn);
    NCollection_Array1<int>    Tmu(1, Nbkn);
    NCollection_Array1<gp_Pnt> Tpol(1, Nbp);
    int                        Ik = 0, Ip = 0;

    Bsp = occ::down_cast<Geom_BSplineCurve>(seq(1));
    for (k = 1; k <= Bsp->NbPoles(); k++)
    {
      Ip++;
      Tpol(Ip) = Bsp->Pole(k);
    }
    for (k = 1; k <= Bsp->NbKnots(); k++)
    {
      Ik++;
      Tkn(Ik) = Bsp->Knot(k);
      Tmu(Ik) = Bsp->Multiplicity(k);
    }
    Tmu(Ik)--;

    for (j = 2; j <= Nbcurv; j++)
    {
      Bsp = occ::down_cast<Geom_BSplineCurve>(seq(j));
      for (k = 2; k <= Bsp->NbPoles(); k++)
      {
        Ip++;
        Tpol(Ip) = Bsp->Pole(k);
      }
      for (k = 2; k <= Bsp->NbKnots(); k++)
      {
        Ik++;
        Tkn(Ik) = Bsp->Knot(k);
        Tmu(Ik) = Bsp->Multiplicity(k);
      }
      Tmu(Ik)--;
    }
    Tmu(Ik)++;
    newC = new Geom_BSplineCurve(Tpol, Tkn, Tmu, MaxDeg);
    myCrvs.Append(newC);
  }

  return myCrvs;
}

//=======================================================================
// function : Orientation
// purpose  : static, not member
//=======================================================================

static TopAbs_Orientation Orientation(const TopoDS_Shape& Sub, const TopoDS_Shape& S)
{
  TopExp_Explorer exp;
  for (exp.Init(S, Sub.ShapeType()); exp.More(); exp.Next())
  {
    if (exp.Current().IsSame(Sub))
    {
      return exp.Current().Orientation();
    }
  }
  throw Standard_NoSuchObject();
}

//=================================================================================================

occ::handle<Geom_Curve> LocOpe_Pipe::BarycCurve()
{
  int j, k;

  gp_Pnt                       bar(0., 0., 0.);
  NCollection_Sequence<gp_Pnt> spt;
  TopoDS_Shape                 Base = FirstShape();
  LocOpe::SampleEdges(Base, spt);
  for (int jj = 1; jj <= spt.Length(); jj++)
  {
    const gp_Pnt& pvt = spt(jj);
    bar.ChangeCoord() += pvt.XYZ();
  }
  bar.ChangeCoord().Divide(spt.Length());

  double p1, p2;
  //  gp_Pnt ptbid;
  gp_Pnt P1 = bar;

  int                                           MaxDeg = 0;
  NCollection_Sequence<occ::handle<Geom_Curve>> seq;
  TopoDS_Wire                                   W = myPipe.PipeLine(P1);

  TopExp_Explorer ex(W, TopAbs_EDGE);
  for (; ex.More(); ex.Next())
  {
    occ::handle<Geom_Curve>        C1 = BRep_Tool::Curve(TopoDS::Edge(ex.Current()), p1, p2);
    occ::handle<Geom_BSplineCurve> C  = GeomConvert::CurveToBSplineCurve(C1);

    if (C.IsNull())
    {
      continue;
    }
    MaxDeg = std::max(MaxDeg, C->Degree());
    P1     = C->Value(p2);
    if (p1 != C->FirstParameter() || p2 != C->LastParameter())
    {
      C->Segment(p1, p2);
    }
    NCollection_Array1<double> Tkn(C->Knots());
    BSplCLib::Reparametrize(seq.Length(), seq.Length() + 1, Tkn);
    C->SetKnots(Tkn);
    seq.Append(C);
  }
  occ::handle<Geom_Curve> newC;
  int                     Nbkn = 0, Nbp = 0;
  int                     Nbcurv = seq.Length();
  if (Nbcurv == 0)
  {
    myCrvs.Append(newC);
  }
  occ::handle<Geom_BSplineCurve> Bsp;
  for (j = 1; j <= Nbcurv; j++)
  {
    Bsp = occ::down_cast<Geom_BSplineCurve>(seq(j));
    Bsp->IncreaseDegree(MaxDeg);
    Nbp += Bsp->NbPoles();
    Nbkn += Bsp->NbKnots();
  }
  Nbp -= Nbcurv - 1;
  Nbkn -= Nbcurv - 1;
  NCollection_Array1<double> Tkn(1, Nbkn);
  NCollection_Array1<int>    Tmu(1, Nbkn);
  NCollection_Array1<gp_Pnt> Tpol(1, Nbp);
  int                        Ik = 0, Ip = 0;

  Bsp = occ::down_cast<Geom_BSplineCurve>(seq(1));
  for (k = 1; k <= Bsp->NbPoles(); k++)
  {
    Ip++;
    Tpol(Ip) = Bsp->Pole(k);
  }
  for (k = 1; k <= Bsp->NbKnots(); k++)
  {
    Ik++;
    Tkn(Ik) = Bsp->Knot(k);
    Tmu(Ik) = Bsp->Multiplicity(k);
  }
  Tmu(Ik)--;

  for (j = 2; j <= Nbcurv; j++)
  {
    Bsp = occ::down_cast<Geom_BSplineCurve>(seq(j));
    for (k = 2; k <= Bsp->NbPoles(); k++)
    {
      Ip++;
      Tpol(Ip) = Bsp->Pole(k);
    }
    for (k = 2; k <= Bsp->NbKnots(); k++)
    {
      Ik++;
      Tkn(Ik) = Bsp->Knot(k);
      Tmu(Ik) = Bsp->Multiplicity(k);
    }
    Tmu(Ik)--;
  }
  Tmu(Ik)++;
  newC = new Geom_BSplineCurve(Tpol, Tkn, Tmu, MaxDeg);

  return newC;
}
