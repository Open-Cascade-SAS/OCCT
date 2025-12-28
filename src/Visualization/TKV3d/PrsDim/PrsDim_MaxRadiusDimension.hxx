// Created on: 1998-01-22
// Created by: Sergey ZARITCHNY
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

#ifndef _PrsDim_MaxRadiusDimension_HeaderFile
#define _PrsDim_MaxRadiusDimension_HeaderFile

#include <PrsDim_EllipseRadiusDimension.hxx>
#include <DsgPrs_ArrowSide.hxx>

//! Ellipse Max radius dimension of a Shape which can be Edge
//! or Face (planar or cylindrical(surface of extrusion or
//! surface of offset))
class PrsDim_MaxRadiusDimension : public PrsDim_EllipseRadiusDimension
{
  DEFINE_STANDARD_RTTIEXT(PrsDim_MaxRadiusDimension, PrsDim_EllipseRadiusDimension)
public:
  //! Max Ellipse radius dimension
  //! Shape can be edge, planar face or cylindrical face
  Standard_EXPORT PrsDim_MaxRadiusDimension(const TopoDS_Shape&               aShape,
                                            const double                      aVal,
                                            const TCollection_ExtendedString& aText);

  //! Max Ellipse radius dimension with position
  //! Shape can be edge, planar face or cylindrical face
  Standard_EXPORT PrsDim_MaxRadiusDimension(const TopoDS_Shape&               aShape,
                                            const double                      aVal,
                                            const TCollection_ExtendedString& aText,
                                            const gp_Pnt&                     aPosition,
                                            const DsgPrs_ArrowSide            aSymbolPrs,
                                            const double                      anArrowSize = 0.0);

private:
  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;

  Standard_EXPORT void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                                const int theMode) override;

  Standard_EXPORT void ComputeEllipse(const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT void ComputeArcOfEllipse(const occ::handle<Prs3d_Presentation>& aPresentation);

private:
  gp_Pnt myApexP;
  gp_Pnt myApexN;
  gp_Pnt myEndOfArrow;
};

#endif // _PrsDim_MaxRadiusDimension_HeaderFile
