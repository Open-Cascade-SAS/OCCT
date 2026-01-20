// Created on: 1995-08-04
// Created by: Modelistation
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

#include <Adaptor3d_IsoCurve.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_List.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <NCollection_List.hxx>
#include <StdPrs_Curve.hxx>
#include <StdPrs_WFSurface.hxx>

static void FindLimits(const occ::handle<Adaptor3d_Surface>& surf,
                       const double              aLimit,
                       double&                   UFirst,
                       double&                   ULast,
                       double&                   VFirst,
                       double&                   VLast)
{
  UFirst = surf->FirstUParameter();
  ULast  = surf->LastUParameter();
  VFirst = surf->FirstVParameter();
  VLast  = surf->LastVParameter();

  bool UfirstInf = Precision::IsNegativeInfinite(UFirst);
  bool UlastInf  = Precision::IsPositiveInfinite(ULast);
  bool VfirstInf = Precision::IsNegativeInfinite(VFirst);
  bool VlastInf  = Precision::IsPositiveInfinite(VLast);

  if (UfirstInf || UlastInf)
  {
    gp_Pnt        P1, P2;
    double v;
    if (VfirstInf && VlastInf)
      v = 0;
    else if (VfirstInf)
      v = VLast;
    else if (VlastInf)
      v = VFirst;
    else
      v = (VFirst + VLast) / 2;

    double delta = aLimit * 2;

    if (UfirstInf && UlastInf)
    {
      do
      {
        delta /= 2;
        UFirst = -delta;
        ULast  = delta;
        surf->D0(UFirst, v, P1);
        surf->D0(ULast, v, P2);
      } while (P1.Distance(P2) > aLimit);
    }
    else if (UfirstInf)
    {
      surf->D0(ULast, v, P2);
      do
      {
        delta /= 2;
        UFirst = ULast - delta;
        surf->D0(UFirst, v, P1);
      } while (P1.Distance(P2) > aLimit);
    }
    else if (UlastInf)
    {
      surf->D0(UFirst, v, P1);
      do
      {
        delta /= 2;
        ULast = UFirst + delta;
        surf->D0(ULast, v, P2);
      } while (P1.Distance(P2) > aLimit);
    }
  }

  if (VfirstInf || VlastInf)
  {
    gp_Pnt        P1, P2;
    double u = (UFirst + ULast) / 2;

    double delta = aLimit * 2;

    if (VfirstInf && VlastInf)
    {
      do
      {
        delta /= 2;
        VFirst = -delta;
        VLast  = delta;
        surf->D0(u, VFirst, P1);
        surf->D0(u, VLast, P2);
      } while (P1.Distance(P2) > aLimit);
    }
    else if (VfirstInf)
    {
      surf->D0(u, VLast, P2);
      do
      {
        delta /= 2;
        VFirst = VLast - delta;
        surf->D0(u, VFirst, P1);
      } while (P1.Distance(P2) > aLimit);
    }
    else if (VlastInf)
    {
      surf->D0(u, VFirst, P1);
      do
      {
        delta /= 2;
        VLast = VFirst + delta;
        surf->D0(u, VLast, P2);
      } while (P1.Distance(P2) > aLimit);
    }
  }
}

//=================================================================================================

void StdPrs_WFSurface::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                           const occ::handle<Adaptor3d_Surface>&  aSurface,
                           const occ::handle<Prs3d_Drawer>&       aDrawer)
{

  double U1, U2, V1, V2;
  double MaxP = aDrawer->MaximalParameterValue();

  FindLimits(aSurface, MaxP, U1, U2, V1, V2);

  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> freeCurves;
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> UIsoCurves;
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> VIsoCurves;

  bool UClosed = aSurface->IsUClosed();
  bool VClosed = aSurface->IsVClosed();

  Adaptor3d_IsoCurve anIso;
  anIso.Load(aSurface);

  // Trace des frontieres.
  // *********************
  //
  if (!(UClosed && VClosed))
  {
    aPresentation->CurrentGroup()->SetPrimitivesAspect(aDrawer->FreeBoundaryAspect()->Aspect());
    if (!UClosed)
    {
      anIso.Load(GeomAbs_IsoU, U1, V1, V2);
      occ::handle<NCollection_HSequence<gp_Pnt>> aPntsU1 = new NCollection_HSequence<gp_Pnt>;
      StdPrs_Curve::Add(aPresentation, anIso, aDrawer, aPntsU1->ChangeSequence(), false);
      freeCurves.Append(aPntsU1);

      anIso.Load(GeomAbs_IsoU, U2, V1, V2);
      occ::handle<NCollection_HSequence<gp_Pnt>> aPntsU2 = new NCollection_HSequence<gp_Pnt>;
      StdPrs_Curve::Add(aPresentation, anIso, aDrawer, aPntsU2->ChangeSequence(), false);
      freeCurves.Append(aPntsU2);
    }
    if (!VClosed)
    {
      anIso.Load(GeomAbs_IsoV, V1, U1, U2);
      occ::handle<NCollection_HSequence<gp_Pnt>> aPntsV1 = new NCollection_HSequence<gp_Pnt>;
      StdPrs_Curve::Add(aPresentation, anIso, aDrawer, aPntsV1->ChangeSequence(), false);
      freeCurves.Append(aPntsV1);

      anIso.Load(GeomAbs_IsoV, V2, U1, U2);
      occ::handle<NCollection_HSequence<gp_Pnt>> aPntsV2 = new NCollection_HSequence<gp_Pnt>;
      StdPrs_Curve::Add(aPresentation, anIso, aDrawer, aPntsV2->ChangeSequence(), false);
      freeCurves.Append(aPntsV2);
    }
  }
  //
  // Trace des isoparametriques.
  // ***************************
  //
  int fin = aDrawer->UIsoAspect()->Number();
  if (fin != 0)
  {
    aPresentation->CurrentGroup()->SetPrimitivesAspect(aDrawer->UIsoAspect()->Aspect());

    double du = UClosed ? (U2 - U1) / fin : (U2 - U1) / (1 + fin);
    for (int i = 1; i <= fin; i++)
    {
      anIso.Load(GeomAbs_IsoU, U1 + du * i, V1, V2);
      occ::handle<NCollection_HSequence<gp_Pnt>> Pnts = new NCollection_HSequence<gp_Pnt>;
      StdPrs_Curve::Add(aPresentation, anIso, aDrawer, Pnts->ChangeSequence(), false);
      UIsoCurves.Append(Pnts);
    }
  }
  fin = aDrawer->VIsoAspect()->Number();
  if (fin != 0)
  {
    aPresentation->CurrentGroup()->SetPrimitivesAspect(aDrawer->VIsoAspect()->Aspect());

    double dv = VClosed ? (V2 - V1) / fin : (V2 - V1) / (1 + fin);
    for (int i = 1; i <= fin; i++)
    {
      anIso.Load(GeomAbs_IsoV, V1 + dv * i, U1, U2);
      occ::handle<NCollection_HSequence<gp_Pnt>> Pnts = new NCollection_HSequence<gp_Pnt>;
      StdPrs_Curve::Add(aPresentation, anIso, aDrawer, Pnts->ChangeSequence(), false);
      VIsoCurves.Append(Pnts);
    }
  }

  int nbVertices = 0, nbBounds = 0;
  // Draw surface via primitive array
  if (UIsoCurves.Size() > 0)
  {
    nbBounds = UIsoCurves.Size();
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>::Iterator It;
    for (It.Init(UIsoCurves); It.More(); It.Next())
      nbVertices += It.Value()->Length();
    occ::handle<Graphic3d_ArrayOfPolylines> UIsoArray =
      new Graphic3d_ArrayOfPolylines(nbVertices, nbBounds);
    for (It.Init(UIsoCurves); It.More(); It.Next())
    {
      const occ::handle<NCollection_HSequence<gp_Pnt>>& Pnts = It.Value();
      UIsoArray->AddBound(Pnts->Length());
      for (int i = 1; i <= Pnts->Length(); i++)
        UIsoArray->AddVertex(Pnts->Value(i));
    }
    occ::handle<Graphic3d_Group> TheGroup = aPresentation->NewGroup();
    TheGroup->SetPrimitivesAspect(aDrawer->UIsoAspect()->Aspect());
    TheGroup->AddPrimitiveArray(UIsoArray);
  }

  if (VIsoCurves.Size() > 0)
  {
    nbBounds = VIsoCurves.Size();
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>::Iterator It;
    for (It.Init(VIsoCurves); It.More(); It.Next())
      nbVertices += It.Value()->Length();
    occ::handle<Graphic3d_ArrayOfPolylines> VIsoArray =
      new Graphic3d_ArrayOfPolylines(nbVertices, nbBounds);
    for (It.Init(VIsoCurves); It.More(); It.Next())
    {
      const occ::handle<NCollection_HSequence<gp_Pnt>>& Pnts = It.Value();
      VIsoArray->AddBound(Pnts->Length());
      for (int i = 1; i <= Pnts->Length(); i++)
        VIsoArray->AddVertex(Pnts->Value(i));
    }
    occ::handle<Graphic3d_Group> TheGroup = aPresentation->NewGroup();
    TheGroup->SetPrimitivesAspect(aDrawer->VIsoAspect()->Aspect());
    TheGroup->AddPrimitiveArray(VIsoArray);
  }
  if (freeCurves.Size() > 0)
  {
    nbBounds = freeCurves.Size();
    NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>::Iterator It;
    for (It.Init(freeCurves); It.More(); It.Next())
      nbVertices += It.Value()->Length();
    occ::handle<Graphic3d_ArrayOfPolylines> freeArray =
      new Graphic3d_ArrayOfPolylines(nbVertices, nbBounds);
    for (It.Init(freeCurves); It.More(); It.Next())
    {
      const occ::handle<NCollection_HSequence<gp_Pnt>>& Pnts = It.Value();
      freeArray->AddBound(Pnts->Length());
      for (int i = 1; i <= Pnts->Length(); i++)
        freeArray->AddVertex(Pnts->Value(i));
    }
    occ::handle<Graphic3d_Group> TheGroup = aPresentation->NewGroup();
    TheGroup->SetPrimitivesAspect(aDrawer->FreeBoundaryAspect()->Aspect());
    TheGroup->AddPrimitiveArray(freeArray);
  }
}
