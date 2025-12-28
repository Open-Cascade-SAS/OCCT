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

#include <DsgPrs_LengthPresentation.hxx>

#include <DsgPrs.hxx>
#include <ElCLib.hxx>
#include <gce_MakeLin.hxx>
#include <Geom_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_ExtendedString.hxx>

void DsgPrs_LengthPresentation::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                    const occ::handle<Prs3d_Drawer>&       aDrawer,
                                    const TCollection_ExtendedString&      aText,
                                    const gp_Pnt&                          AttachmentPoint1,
                                    const gp_Pnt&                          AttachmentPoint2,
                                    const gp_Dir&                          aDirection,
                                    const gp_Pnt&                          OffsetPoint)
{
  occ::handle<Prs3d_DimensionAspect> LA = aDrawer->DimensionAspect();
  aPresentation->CurrentGroup()->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Lin L1(AttachmentPoint1, aDirection);
  gp_Lin L2(AttachmentPoint2, aDirection);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1, OffsetPoint), L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2, OffsetPoint), L2);
  gp_Lin L3    = Proj1.IsEqual(Proj2, Precision::Confusion()) ? gp_Lin(Proj1, aDirection)
                                                              : gce_MakeLin(Proj1, Proj2);
  double parmin, parmax, parcur;
  parmin      = ElCLib::Parameter(L3, Proj1);
  parmax      = parmin;
  parcur      = ElCLib::Parameter(L3, Proj2);
  double dist = std::abs(parmin - parcur);
  if (parcur < parmin)
    parmin = parcur;
  if (parcur > parmax)
    parmax = parcur;
  parcur      = ElCLib::Parameter(L3, OffsetPoint);
  gp_Pnt offp = ElCLib::Value(parcur, L3);

  bool outside = false;
  if (parcur < parmin)
  {
    parmin  = parcur;
    outside = true;
  }
  if (parcur > parmax)
  {
    parmax  = parcur;
    outside = true;
  }

  gp_Pnt PointMin = ElCLib::Value(parmin, L3);
  gp_Pnt PointMax = ElCLib::Value(parmax, L3);

  // face processing : 1st group
  occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(6);
  aPrims->AddVertex(PointMin);
  aPrims->AddVertex(PointMax);

  if (dist < (LA->ArrowAspect()->Length() + LA->ArrowAspect()->Length()))
    outside = true;

  gp_Dir arrdir = L3.Direction().Reversed();
  if (outside)
    arrdir.Reverse();

  // arrow 1 : 2nd group
  Prs3d_Arrow::Draw(aPresentation->CurrentGroup(),
                    Proj1,
                    arrdir,
                    LA->ArrowAspect()->Angle(),
                    LA->ArrowAspect()->Length());

  aPresentation->NewGroup();
  aPresentation->CurrentGroup()->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  // arrow 2 : 3rd group
  Prs3d_Arrow::Draw(aPresentation->CurrentGroup(),
                    Proj2,
                    arrdir.Reversed(),
                    LA->ArrowAspect()->Angle(),
                    LA->ArrowAspect()->Length());

  aPresentation->NewGroup();

  // text : 4th group
  Prs3d_Text::Draw(aPresentation->CurrentGroup(), LA->TextAspect(), aText, offp);

  aPresentation->CurrentGroup()->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  // processing of call 1 : 5th group
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(Proj1);

  // processing of call 2 : 6th group
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(Proj2);

  aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);
}

//==================================================================================
// function : Add
// purpose  : Adds presentation of length dimension between two planar faces
//==================================================================================

void DsgPrs_LengthPresentation::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                    const occ::handle<Prs3d_Drawer>&       aDrawer,
                                    const TCollection_ExtendedString&      aText,
                                    const gp_Pnt&                          AttachmentPoint1,
                                    const gp_Pnt&                          AttachmentPoint2,
                                    const gp_Pln&                          PlaneOfFaces,
                                    const gp_Dir&                          aDirection,
                                    const gp_Pnt&                          OffsetPoint,
                                    const DsgPrs_ArrowSide                 ArrowPrs)
{
  occ::handle<Prs3d_DimensionAspect> LA = aDrawer->DimensionAspect();
  aPresentation->CurrentGroup()->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Pnt EndOfArrow1, EndOfArrow2;
  gp_Dir DirOfArrow1;

  DsgPrs::ComputePlanarFacesLengthPresentation(LA->ArrowAspect()->Length(),
                                               LA->ArrowAspect()->Length(),
                                               AttachmentPoint1,
                                               AttachmentPoint2,
                                               aDirection,
                                               OffsetPoint,
                                               PlaneOfFaces,
                                               EndOfArrow1,
                                               EndOfArrow2,
                                               DirOfArrow1);

  // Parameters for length's line
  gp_Lin LengthLine(OffsetPoint, DirOfArrow1);
  double Par1 = ElCLib::Parameter(LengthLine, EndOfArrow1);
  double Par2 = ElCLib::Parameter(LengthLine, EndOfArrow2);
  gp_Pnt FirstPoint, LastPoint;
  if ((Par1 > 0.0 && Par2 > 0.0) || (Par1 < 0.0 && Par2 < 0.0))
  {
    FirstPoint = OffsetPoint;
    LastPoint  = (std::abs(Par1) > std::abs(Par2)) ? EndOfArrow1 : EndOfArrow2;
  }
  else
  {
    FirstPoint = EndOfArrow1;
    LastPoint  = EndOfArrow2;
  }

  // Creating the length's line
  occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(6);

  aPrims->AddVertex(FirstPoint);
  aPrims->AddVertex(LastPoint);

  // Add presentation of arrows
  DsgPrs::ComputeSymbol(aPresentation,
                        LA,
                        EndOfArrow1,
                        EndOfArrow2,
                        DirOfArrow1,
                        DirOfArrow1.Reversed(),
                        ArrowPrs);

  // Drawing the text
  Prs3d_Text::Draw(aPresentation->CurrentGroup(), LA->TextAspect(), aText, OffsetPoint);

  // Line from AttachmentPoint1 to end of Arrow1
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(EndOfArrow1);

  // Line from AttachmentPoint2 to end of Arrow2
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(EndOfArrow2);

  aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);
}

//=========================================================================================
// function : Add
// purpose  : adds presentation of length between two edges, vertex and edge or two vertices
//=========================================================================================

void DsgPrs_LengthPresentation::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                    const occ::handle<Prs3d_Drawer>&       aDrawer,
                                    const TCollection_ExtendedString&      aText,
                                    const gp_Pnt&                          AttachmentPoint1,
                                    const gp_Pnt&                          AttachmentPoint2,
                                    const gp_Dir&                          aDirection,
                                    const gp_Pnt&                          OffsetPoint,
                                    const DsgPrs_ArrowSide                 ArrowPrs)
{
  occ::handle<Prs3d_DimensionAspect> LA = aDrawer->DimensionAspect();
  aPresentation->CurrentGroup()->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Lin L1(AttachmentPoint1, aDirection);
  gp_Lin L2(AttachmentPoint2, aDirection);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1, OffsetPoint), L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2, OffsetPoint), L2);
  gp_Lin L3    = Proj1.IsEqual(Proj2, Precision::Confusion()) ? gp_Lin(Proj1, aDirection)
                                                              : gce_MakeLin(Proj1, Proj2);
  double parmin, parmax, parcur;
  parmin      = ElCLib::Parameter(L3, Proj1);
  parmax      = parmin;
  parcur      = ElCLib::Parameter(L3, Proj2);
  double dist = std::abs(parmin - parcur);
  if (parcur < parmin)
    parmin = parcur;
  if (parcur > parmax)
    parmax = parcur;
  parcur      = ElCLib::Parameter(L3, OffsetPoint);
  gp_Pnt offp = ElCLib::Value(parcur, L3);

  bool outside = false;
  if (parcur < parmin)
  {
    parmin  = parcur;
    outside = true;
  }
  if (parcur > parmax)
  {
    parmax  = parcur;
    outside = true;
  }

  gp_Pnt PointMin = ElCLib::Value(parmin, L3);
  gp_Pnt PointMax = ElCLib::Value(parmax, L3);

  // processing of face
  occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(6);

  aPrims->AddVertex(PointMin);
  aPrims->AddVertex(PointMax);

  if (dist < (LA->ArrowAspect()->Length() + LA->ArrowAspect()->Length()))
    outside = true;

  gp_Dir arrdir = L3.Direction().Reversed();
  if (outside)
    arrdir.Reverse();

  // processing of call  1
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(Proj1);

  // processing of call 2
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(Proj2);

  aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);

  // text
  Prs3d_Text::Draw(aPresentation->CurrentGroup(), LA->TextAspect(), aText, offp);

  // symbols at the extremities of the face
  DsgPrs::ComputeSymbol(aPresentation, LA, Proj1, Proj2, arrdir, arrdir.Reversed(), ArrowPrs);
}

//==================================================================================
// function : Add
// purpose  : Adds presentation of length dimension between two curvilinear faces
//==================================================================================

void DsgPrs_LengthPresentation::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                    const occ::handle<Prs3d_Drawer>&       aDrawer,
                                    const TCollection_ExtendedString&      aText,
                                    const occ::handle<Geom_Surface>&       SecondSurf,
                                    const gp_Pnt&                          AttachmentPoint1,
                                    const gp_Pnt&                          AttachmentPoint2,
                                    const gp_Dir&                          aDirection,
                                    const gp_Pnt&                          OffsetPoint,
                                    const DsgPrs_ArrowSide                 ArrowPrs)
{
  occ::handle<Prs3d_DimensionAspect> LA = aDrawer->DimensionAspect();
  aPresentation->CurrentGroup()->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Pnt                  EndOfArrow2;
  gp_Dir                  DirOfArrow1;
  occ::handle<Geom_Curve> VCurve, UCurve;
  double                  FirstU, deltaU = 0.0e0, FirstV, deltaV = 0.0e0;

  DsgPrs::ComputeCurvilinearFacesLengthPresentation(LA->ArrowAspect()->Length(),
                                                    LA->ArrowAspect()->Length(),
                                                    SecondSurf,
                                                    AttachmentPoint1,
                                                    AttachmentPoint2,
                                                    aDirection,
                                                    EndOfArrow2,
                                                    DirOfArrow1,
                                                    VCurve,
                                                    UCurve,
                                                    FirstU,
                                                    deltaU,
                                                    FirstV,
                                                    deltaV);

  gp_Lin LengthLine(OffsetPoint, DirOfArrow1);
  double Par1 = ElCLib::Parameter(LengthLine, AttachmentPoint1);
  double Par2 = ElCLib::Parameter(LengthLine, EndOfArrow2);
  gp_Pnt FirstPoint, LastPoint;
  if ((Par1 > 0.0 && Par2 > 0.0) || (Par1 < 0.0 && Par2 < 0.0))
  {
    FirstPoint = OffsetPoint;
    LastPoint  = (std::abs(Par1) > std::abs(Par2)) ? AttachmentPoint1 : EndOfArrow2;
  }
  else
  {
    FirstPoint = AttachmentPoint1;
    LastPoint  = EndOfArrow2;
  }

  // Creating the length's line
  occ::handle<Graphic3d_ArrayOfPrimitives> aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(FirstPoint);
  aPrims->AddVertex(LastPoint);
  aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);

  // Add presentation of arrows
  DsgPrs::ComputeSymbol(aPresentation,
                        LA,
                        AttachmentPoint1,
                        EndOfArrow2,
                        DirOfArrow1,
                        DirOfArrow1.Reversed(),
                        ArrowPrs);

  // Drawing the text
  Prs3d_Text::Draw(aPresentation->CurrentGroup(), LA->TextAspect(), aText, OffsetPoint);

  // Two curves from end of Arrow2 to AttachmentPoint2
  double Alpha, delta;
  int    NodeNumber;

  Alpha = std::abs(deltaU);
  if (Alpha > Precision::Angular() && Alpha < Precision::Infinite())
  {
    NodeNumber = std::max(4, int(50. * Alpha / M_PI));
    delta      = deltaU / (double)(NodeNumber - 1);
    aPrims     = new Graphic3d_ArrayOfPolylines(NodeNumber);
    for (int i = 1; i <= NodeNumber; i++, FirstU += delta)
      aPrims->AddVertex(VCurve->Value(FirstU));
    aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);
  }
  Alpha = std::abs(deltaV);
  if (Alpha > Precision::Angular() && Alpha < Precision::Infinite())
  {
    NodeNumber = std::max(4, int(50. * Alpha / M_PI));
    delta      = deltaV / (double)(NodeNumber - 1);
    aPrims     = new Graphic3d_ArrayOfPolylines(NodeNumber);
    for (int i = 1; i <= NodeNumber; i++, FirstV += delta)
      aPrims->AddVertex(UCurve->Value(FirstV));
    aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);
  }
}

//================================
// Function:
// Purpose: Rob 26-mar-96
//=================================

void DsgPrs_LengthPresentation::Add(const occ::handle<Prs3d_Presentation>& aPrs,
                                    const occ::handle<Prs3d_Drawer>&       aDrawer,
                                    const gp_Pnt&                          Pt1,
                                    const gp_Pnt&                          Pt2,
                                    const DsgPrs_ArrowSide                 ArrowPrs)
{
  occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(Pt1);
  aPrims->AddVertex(Pt2);
  aPrs->CurrentGroup()->AddPrimitiveArray(aPrims);

  gp_Vec V;
  switch (ArrowPrs)
  {
    case DsgPrs_AS_LASTAR:
      Prs3d_Arrow::Draw(aPrs->CurrentGroup(),
                        Pt2,
                        gp_Dir(gp_Vec(Pt1, Pt2)),
                        aDrawer->DimensionAspect()->ArrowAspect()->Angle(),
                        aDrawer->DimensionAspect()->ArrowAspect()->Length());
      break;
    case DsgPrs_AS_FIRSTAR:
      Prs3d_Arrow::Draw(aPrs->CurrentGroup(),
                        Pt1,
                        gp_Dir(gp_Vec(Pt2, Pt1)),
                        aDrawer->DimensionAspect()->ArrowAspect()->Angle(),
                        aDrawer->DimensionAspect()->ArrowAspect()->Length());
      break;
    case DsgPrs_AS_BOTHAR:
      V = gp_Vec(Pt1, Pt2);
      Prs3d_Arrow::Draw(aPrs->CurrentGroup(),
                        Pt2,
                        gp_Dir(V),
                        aDrawer->DimensionAspect()->ArrowAspect()->Angle(),
                        aDrawer->DimensionAspect()->ArrowAspect()->Length());
      Prs3d_Arrow::Draw(aPrs->CurrentGroup(),
                        Pt1,
                        gp_Dir(V.Reversed()),
                        aDrawer->DimensionAspect()->ArrowAspect()->Angle(),
                        aDrawer->DimensionAspect()->ArrowAspect()->Length());
      break;
    default:
      break;
  }
}
