// Created on: 1995-03-01
// Created by: Arnaud BOUZY
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

#include <DsgPrs_RadiusPresentation.hxx>

#include <DsgPrs.hxx>
#include <ElCLib.hxx>
#include <gce_MakeDir.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_ExtendedString.hxx>

static bool DsgPrs_InDomain(const double fpar, const double lpar, const double para)
{
  if (fpar >= 0.)
    return ((para >= fpar) && (para <= lpar));
  if (para >= (fpar + 2. * M_PI))
    return true;
  if (para <= lpar)
    return true;
  return false;
}

//=================================================================================================

void DsgPrs_RadiusPresentation::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                    const occ::handle<Prs3d_Drawer>&       aDrawer,
                                    const TCollection_ExtendedString&      aText,
                                    const gp_Pnt&                          AttachmentPoint,
                                    const gp_Circ&                         aCircle,
                                    const double                           firstparam,
                                    const double                           lastparam,
                                    const bool                             drawFromCenter,
                                    const bool                             reverseArrow)
{
  double fpara = firstparam;
  double lpara = lastparam;
  while (lpara > 2. * M_PI)
  {
    fpara -= 2. * M_PI;
    lpara -= 2. * M_PI;
  }
  occ::handle<Prs3d_DimensionAspect> LA = aDrawer->DimensionAspect();
  aPresentation->CurrentGroup()->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  double parat     = ElCLib::Parameter(aCircle, AttachmentPoint);
  gp_Pnt attpoint  = AttachmentPoint;
  bool   otherside = false;
  if (!DsgPrs_InDomain(fpara, lpara, parat))
  {
    double otherpar = parat + M_PI;
    if (otherpar > 2. * M_PI)
      otherpar -= 2. * M_PI;
    if (DsgPrs_InDomain(fpara, lpara, otherpar))
    {
      parat     = otherpar;
      otherside = true;
    }
    else
    {
      const double ecartpar = std::min(std::abs(fpara - parat), std::abs(lpara - parat));
      const double ecartoth = std::min(std::abs(fpara - otherpar), std::abs(lpara - otherpar));
      if (ecartpar <= ecartoth)
      {
        parat = (parat < fpara) ? fpara : lpara;
      }
      else
      {
        otherside = true;
        parat     = (otherpar < fpara) ? fpara : lpara;
      }
      gp_Pnt       ptdir = ElCLib::Value(parat, aCircle);
      gp_Lin       lsup(aCircle.Location(), gp_Dir(ptdir.XYZ() - aCircle.Location().XYZ()));
      const double parpos = ElCLib::Parameter(lsup, AttachmentPoint);
      attpoint            = ElCLib::Value(parpos, lsup);
    }
  }
  gp_Pnt ptoncirc = ElCLib::Value(parat, aCircle);
  gp_Lin L(aCircle.Location(), gp_Dir(attpoint.XYZ() - aCircle.Location().XYZ()));
  gp_Pnt firstpoint  = attpoint;
  gp_Pnt drawtopoint = ptoncirc;
  if (drawFromCenter && !otherside)
  {
    const double uatt = ElCLib::Parameter(L, attpoint);
    const double uptc = ElCLib::Parameter(L, ptoncirc);
    if (std::abs(uatt) > std::abs(uptc))
      drawtopoint = aCircle.Location();
    else
      firstpoint = aCircle.Location();
  }

  occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(firstpoint);
  aPrims->AddVertex(drawtopoint);
  aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);

  gp_Dir arrdir = L.Direction();
  if (reverseArrow)
    arrdir.Reverse();

  // fleche
  Prs3d_Arrow::Draw(aPresentation->CurrentGroup(),
                    ptoncirc,
                    arrdir,
                    LA->ArrowAspect()->Angle(),
                    LA->ArrowAspect()->Length());

  // texte
  Prs3d_Text::Draw(aPresentation->CurrentGroup(), LA->TextAspect(), aText, attpoint);
}

//=======================================================================
// function : DsgPrs_RadiusPresentation::Add
// purpose  : SZY 20-february-98
//         : adds radius representation according drawFromCenter value
//=======================================================================

void DsgPrs_RadiusPresentation::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                    const occ::handle<Prs3d_Drawer>&       aDrawer,
                                    const TCollection_ExtendedString&      aText,
                                    const gp_Pnt&                          AttachmentPoint,
                                    const gp_Pnt&                          Center,
                                    const gp_Pnt&                          EndOfArrow,
                                    const DsgPrs_ArrowSide                 ArrowPrs,
                                    const bool                             drawFromCenter,
                                    const bool                             reverseArrow)
{
  occ::handle<Prs3d_DimensionAspect> LA = aDrawer->DimensionAspect();
  aPresentation->CurrentGroup()->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Pnt LineOrigin, LineEnd;
  DsgPrs::ComputeRadiusLine(Center,
                            EndOfArrow,
                            AttachmentPoint,
                            drawFromCenter,
                            LineOrigin,
                            LineEnd);

  occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(LineOrigin);
  aPrims->AddVertex(LineEnd);
  aPresentation->CurrentGroup()->AddPrimitiveArray(aPrims);

  // text
  Prs3d_Text::Draw(aPresentation->CurrentGroup(), LA->TextAspect(), aText, AttachmentPoint);

  gp_Dir ArrowDir = gce_MakeDir(LineOrigin, LineEnd);
  if (reverseArrow)
    ArrowDir.Reverse();
  DsgPrs::ComputeSymbol(aPresentation,
                        LA,
                        Center,
                        EndOfArrow,
                        ArrowDir.Reversed(),
                        ArrowDir,
                        ArrowPrs,
                        drawFromCenter);
}
