// Created on: 1998-12-16
// Created by: Roman LYGIN
// Copyright (c) 1998-1999 Matra Datavision
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
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Trsf2d.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_IGESBoundary.hxx>
#include <IGESToBRep_TopoCurve.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESToBRep_IGESBoundary, Standard_Transient)

//=================================================================================================

IGESToBRep_IGESBoundary::IGESToBRep_IGESBoundary() {}

//=================================================================================================

IGESToBRep_IGESBoundary::IGESToBRep_IGESBoundary(const IGESToBRep_CurveAndSurface& CS)
    : myCS(CS)
{
}

//=================================================================================================

void IGESToBRep_IGESBoundary::Init(const IGESToBRep_CurveAndSurface&  CS,
                                   const occ::handle<IGESData_IGESEntity>& entity,
                                   const TopoDS_Face&                 face,
                                   const gp_Trsf2d&                   trans,
                                   const double                uFact,
                                   const int             filepreference)
{
  myCS             = CS;
  myentity         = entity;
  myface           = face;
  mytrsf           = trans;
  myuFact          = uFact;
  myfilepreference = filepreference;
}

//=================================================================================================

bool IGESToBRep_IGESBoundary::Transfer(
  bool&                           okCurve,
  bool&                           okCurve3d,
  bool&                           okCurve2d,
  const occ::handle<IGESData_IGESEntity>&          curve3d,
  const bool                      toreverse3d,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& curves2d,
  const int                      number)
{
  occ::handle<ShapeExtend_WireData> scurve3d, lsewd; // temporary objects
  return Transfer(okCurve,
                  okCurve3d,
                  okCurve2d,
                  curve3d,
                  scurve3d,
                  false,
                  toreverse3d,
                  curves2d,
                  false,
                  number,
                  lsewd);
}

//=================================================================================================

bool IGESToBRep_IGESBoundary::Transfer(
  bool&                           okCurve,
  bool&                           okCurve3d,
  bool&                           okCurve2d,
  const occ::handle<ShapeExtend_WireData>&         curve3d,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& curves2d,
  const bool                      toreverse2d,
  const int                      number,
  occ::handle<ShapeExtend_WireData>&               lsewd)
{
  occ::handle<IGESData_IGESEntity> icurve3d; // temporary object
  return Transfer(okCurve,
                  okCurve3d,
                  okCurve2d,
                  icurve3d,
                  curve3d,
                  true,
                  false,
                  curves2d,
                  toreverse2d,
                  number,
                  lsewd);
}

//=================================================================================================

void IGESToBRep_IGESBoundary::Check(const bool,
                                    const bool,
                                    const bool,
                                    const bool)
{
  // Implemented in IGESControl_IGESBoundary, subject to refactoring
}

//=================================================================================================

bool IGESToBRep_IGESBoundary::Transfer(
  bool&,
  bool&,
  bool&,
  const occ::handle<IGESData_IGESEntity>&          icurve3d,
  const occ::handle<ShapeExtend_WireData>&         scurve3d,
  const bool                      usescurve,
  const bool                      toreverse3d,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& curves2d,
  const bool                      toreverse2d,
  const int                      number,
  occ::handle<ShapeExtend_WireData>&               Gsewd)
{
  Gsewd = new ShapeExtend_WireData; // local translation (for mysewd)
  // clang-format off
  occ::handle<ShapeExtend_WireData> Gsewd3d = new ShapeExtend_WireData;//local translation (for mysewd3d)
  occ::handle<ShapeExtend_WireData> Gsewd2d = new ShapeExtend_WireData;//local translation (for mysewd2d)
  // clang-format on

  bool GTranslate3d = true, GTranslate2d = true,
                   Preferred3d = true, Preferred2d = true;

  int                     len3d = 0, len2d = 0;
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> seq3d, seq2d;
  if (usescurve)
    len3d = scurve3d->NbEdges();
  else
  {
    IGESToBRep::IGESCurveToSequenceOfIGESCurve(icurve3d, seq3d);
    len3d = seq3d->Length();
  }
  if (!curves2d.IsNull())
  {
    for (int i = 1; i <= curves2d->Length(); i++)
      IGESToBRep::IGESCurveToSequenceOfIGESCurve(curves2d->Value(i), seq2d);
    len2d = seq2d->Length();
  }

  if (len3d == 0)
    GTranslate3d = false;
  else if (len2d == 0)
    GTranslate2d = false;

  if (GTranslate3d && GTranslate2d)
  {
    // Setting preference in the case of inconsistency between 3D and 2D
    if (myfilepreference == 2)
      Preferred3d = false;
    else if (myfilepreference == 3)
      Preferred2d = false;
    else
      Preferred3d = false;
  }
  if (GTranslate3d && GTranslate2d && len3d != len2d)
  {
    GTranslate3d = Preferred3d;
    GTranslate2d = Preferred2d;
  }

  IGESToBRep_TopoCurve TC(myCS);

  if (GTranslate3d && !GTranslate2d)
  {
    if (usescurve)
      Gsewd->Add(scurve3d->Wire());
    else
    {
      TopoDS_Shape Sh = TC.TransferTopoCurve(icurve3d);
      if (!Sh.IsNull())
      {
        Gsewd3d->Add(Sh);
        if (toreverse3d)
        {
          ReverseCurves3d(Gsewd3d);
          Gsewd->Add(Gsewd3d->Wire());
        }
        else
          Gsewd->Add(
            Sh); // Gsewd = Gsewd3d is impossible to avoid sharing of sewd (UK1.igs entity 7)
      }
    }
  }
  else if (!GTranslate3d && GTranslate2d)
  {
    for (int i = curves2d->Lower(); i <= curves2d->Upper(); i++)
    {
      TopoDS_Shape Sh = TC.Transfer2dTopoCurve(curves2d->Value(i), myface, mytrsf, myuFact);
      if (!Sh.IsNull())
        Gsewd2d->Add(Sh);
    }
    if (toreverse2d)
      ReverseCurves2d(Gsewd2d, myface);
    Gsewd->Add(Gsewd2d->Wire());
  }
  else if (GTranslate3d && GTranslate2d)
  {
    // Translate both curves 3D and 2D
    // Suppose that i-th segment in 2D curve corresponds to i-th segment in 3D curve
    for (int i = 1; i <= len3d; i++)
    {
      bool LTranslate3d = true, LTranslate2d = true;

      occ::handle<ShapeExtend_WireData> Lsewd3d = new ShapeExtend_WireData;
      TC.SetBadCase(false); //: 27
      if (usescurve)
        Lsewd3d->Add(scurve3d->Edge(i));
      else
      {
        TopoDS_Shape shape3d =
          TC.TransferTopoCurve(occ::down_cast<IGESData_IGESEntity>(seq3d->Value(i)));
        if (!shape3d.IsNull())
        {
          Lsewd3d->Add(shape3d);
          if (toreverse3d)
            ReverseCurves3d(Lsewd3d);
        }
        else
          LTranslate3d = false;
      }
      Gsewd3d->Add(Lsewd3d->Wire());

      occ::handle<ShapeExtend_WireData> Lsewd2d = new ShapeExtend_WireData;
      TopoDS_Shape                 shape2d =
        TC.Transfer2dTopoCurve(occ::down_cast<IGESData_IGESEntity>(seq2d->Value(i)),
                               myface,
                               mytrsf,
                               myuFact);
      if (!shape2d.IsNull())
      {
        Lsewd2d->Add(shape2d);
        if (toreverse2d)
          ReverseCurves2d(Lsewd2d, myface);
        Gsewd2d->Add(Lsewd2d->Wire());
      }
      else
        LTranslate2d = false;

      if (LTranslate3d && LTranslate2d && Lsewd3d->NbEdges() != Lsewd2d->NbEdges())
      {
        LTranslate3d = Preferred3d;
        LTranslate2d = Preferred2d;
      }
      occ::handle<ShapeExtend_WireData> Lsewd; // Lsewd3d or Lsewd2d or Lsewd3d+pcurve
      if (LTranslate3d && !LTranslate2d)
        Lsewd = Lsewd3d;
      else if (!LTranslate3d && LTranslate2d)
        Lsewd = Lsewd2d;
      else
      {
        Lsewd = Lsewd3d;
        // copying pcurve to edge with 3D curve
        for (int iedge = 1; iedge <= Lsewd3d->NbEdges(); iedge++)
        {
          TopoDS_Edge edge3d = Lsewd3d->Edge(iedge), edge2d = Lsewd2d->Edge(iedge);
          if (!IGESToBRep::TransferPCurve(edge2d, edge3d, myface))
            continue;
        }
      }
      Gsewd->Add(Lsewd->Wire());
    }
  }

  if (number > 1)
  {
    mysewd->Add(Gsewd->Wire());
    mysewd3d->Add(Gsewd3d->Wire());
    mysewd2d->Add(Gsewd2d->Wire());
  }
  else
  {
    mysewd   = Gsewd;
    mysewd3d = Gsewd3d;
    mysewd2d = Gsewd2d;
  }
  return true;
}

//=======================================================================
// function : ReverseCurves3d
// purpose  : Reverses 3D curves of the edges in the wire and reverses
//           the order of edges in the wire.
//           Orientation of each edge is not changed
//=======================================================================

void IGESToBRep_IGESBoundary::ReverseCurves3d(const occ::handle<ShapeExtend_WireData>& sewd)
{
  sewd->Reverse();
  BRep_Builder B;
  TopoDS_Wire  W;
  B.MakeWire(W);
  for (int i = 1; i <= sewd->NbEdges(); i++)
  {
    TopoDS_Edge        oldedge = sewd->Edge(i), newedge;
    TopLoc_Location    L;
    double      p1, p2;
    occ::handle<Geom_Curve> curve = BRep_Tool::Curve(oldedge, L, p1, p2);
    if (curve->IsPeriodic()) // #21
      ShapeBuild_Edge().MakeEdge(newedge,
                                 curve->Reversed(),
                                 L,
                                 curve->ReversedParameter(p2),
                                 curve->ReversedParameter(p1));
    else
      ShapeBuild_Edge().MakeEdge(
        newedge,
        curve->Reversed(),
        L,
        std::max(curve->ReversedParameter(curve->LastParameter()), curve->ReversedParameter(p2)),
        std::min(curve->ReversedParameter(curve->FirstParameter()), curve->ReversedParameter(p1)));
    newedge.Orientation(TopAbs::Reverse(oldedge.Orientation()));
    // sewd->Set (newedge, i);
    B.Add(W, newedge);
  }
  occ::handle<ShapeFix_Wire> sfw = new ShapeFix_Wire();
  sfw->Load(W);
  sfw->FixConnected();
  sewd->Init(sfw->Wire());
}

//=======================================================================
// function : ReverseCurves2d
// purpose  : Reverses pcurves of the edges in the wire and reverses
//           the order of edges in the wire.
//           Orientation of each edge is also changed
//=======================================================================

void IGESToBRep_IGESBoundary::ReverseCurves2d(const occ::handle<ShapeExtend_WireData>& sewd,
                                              const TopoDS_Face&                  face)
{
  sewd->Reverse(face);
  for (int i = 1; i <= sewd->NbEdges(); i++)
  {
    TopoDS_Edge          oldedge = sewd->Edge(i), newedge;
    double        p1, p2;
    occ::handle<Geom2d_Curve> curve = BRep_Tool::CurveOnSurface(oldedge, face, p1, p2);

    // skl 24.04.2002 for OCC314
    if (curve->IsPeriodic())
      ShapeBuild_Edge().MakeEdge(newedge,
                                 curve->Reversed(),
                                 face,
                                 curve->ReversedParameter(p2),
                                 curve->ReversedParameter(p1));
    else
      ShapeBuild_Edge().MakeEdge(
        newedge,
        curve->Reversed(),
        face,
        std::max(curve->FirstParameter(),
                 curve->ReversedParameter(p2)), // BUC50001 entity 936 2DForced
        std::min(curve->LastParameter(), curve->ReversedParameter(p1)));
    newedge.Orientation(oldedge.Orientation());
    sewd->Set(newedge, i);
  }
}
