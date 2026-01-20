// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESGraph_LineFontDefTemplate_HeaderFile
#define _IGESGraph_LineFontDefTemplate_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_LineFontEntity.hxx>
class IGESBasic_SubfigureDef;

//! defines IGESLineFontDefTemplate, Type <304> Form <1>
//! in package IGESGraph
//!
//! Line Font can be defined as a repetition of Template figure
//! that is displayed at regularly spaced locations along a
//! planer anchoring curve. The anchoring curve itself has
//! no visual purpose.
class IGESGraph_LineFontDefTemplate : public IGESData_LineFontEntity
{

public:
  Standard_EXPORT IGESGraph_LineFontDefTemplate();

  //! This method is used to set the fields of the class
  //! LineFontDefTemplate
  //! - anOrientation : Orientation of Template figure on
  //! anchoring curve
  //! - aTemplate     : SubfigureDef entity used as Template figure
  //! - aDistance     : Distance between the neighbouring Template
  //! figures
  //! - aScale        : Scale factor applied to the Template figure
  Standard_EXPORT void Init(const int                anOrientation,
                            const occ::handle<IGESBasic_SubfigureDef>& aTemplate,
                            const double                   aDistance,
                            const double                   aScale);

  //! if return value = 0, Each Template display is oriented by aligning
  //! the axis of the SubfigureDef with the axis of
  //! the definition space of the anchoring curve.
  //! = 1, Each Template display is oriented by aligning
  //! X-axis of the SubfigureDef with the tangent
  //! vector of the anchoring curve at the point of
  //! incidence of the curve and the origin of
  //! subfigure.
  //! Similarly Z-axis is aligned.
  Standard_EXPORT int Orientation() const;

  //! returns SubfigureDef as the Entity used as Template figure.
  Standard_EXPORT occ::handle<IGESBasic_SubfigureDef> TemplateEntity() const;

  //! returns the Distance between any two Template figures on the
  //! anchoring curve.
  Standard_EXPORT double Distance() const;

  //! returns the Scaling factor applied to SubfigureDef to form
  //! Template figure.
  Standard_EXPORT double Scale() const;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_LineFontDefTemplate, IGESData_LineFontEntity)

private:
  int               theOrientation;
  occ::handle<IGESBasic_SubfigureDef> theTemplateEntity;
  double                  theDistance;
  double                  theScale;
};

#endif // _IGESGraph_LineFontDefTemplate_HeaderFile
