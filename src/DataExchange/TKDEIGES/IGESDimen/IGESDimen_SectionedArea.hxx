// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( SIVA )
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

#ifndef _IGESDimen_SectionedArea_HeaderFile
#define _IGESDimen_SectionedArea_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class gp_Pnt;

//! defines IGES Sectioned Area, Type <230> Form <0>,
//! in package IGESDimen
//! A sectioned area is a portion of a design which is to be
//! filled with a pattern of lines. Ordinarily, this entity
//! is used to reveal or expose shape or material characteri-
//! stics defined by other entities. It consists of a pointer
//! to an exterior definition curve, a specification of the
//! pattern of lines, the coordinates of a point on a pattern
//! line, the distance between the pattern lines, the angle
//! between the pattern lines and the X-axis of definition
//! space, and the specification of any enclosed definition
//! curves (commonly known as islands).
class IGESDimen_SectionedArea : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDimen_SectionedArea();

  Standard_EXPORT void Init(
    const occ::handle<IGESData_IGESEntity>&                                   aCurve,
    const int                                                                 aPattern,
    const gp_XYZ&                                                             aPoint,
    const double                                                              aDistance,
    const double                                                              anAngle,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& someIslands);

  //! Sets the cross hatches to be inverted or not,
  //! according value of <mode> (corresponds to FormNumber)
  Standard_EXPORT void SetInverted(const bool mode);

  //! Returns True if cross hatches as Inverted, else they are
  //! Standard (Inverted : Form=1, Standard : Form=0)
  Standard_EXPORT bool IsInverted() const;

  //! returns the exterior definition curve
  Standard_EXPORT occ::handle<IGESData_IGESEntity> ExteriorCurve() const;

  //! returns fill pattern code
  Standard_EXPORT int Pattern() const;

  //! returns point thru which line should pass
  Standard_EXPORT gp_Pnt PassingPoint() const;

  //! returns point thru which line should pass after Transformation
  Standard_EXPORT gp_Pnt TransformedPassingPoint() const;

  //! returns the Z depth
  Standard_EXPORT double ZDepth() const;

  //! returns the normal distance between lines
  Standard_EXPORT double Distance() const;

  //! returns the angle of lines with XT axis
  Standard_EXPORT double Angle() const;

  //! returns the number of island curves
  Standard_EXPORT int NbIslands() const;

  //! returns the interior definition curves, returns Null Handle
  //! exception raised if Index <= 0 or Index > NbIslands()
  Standard_EXPORT occ::handle<IGESData_IGESEntity> IslandCurve(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESDimen_SectionedArea, IGESData_IGESEntity)

private:
  occ::handle<IGESData_IGESEntity>                                   theExteriorCurve;
  int                                                                thePattern;
  gp_XYZ                                                             thePassingPoint;
  double                                                             theDistance;
  double                                                             theAngle;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>> theIslandCurves;
};

#endif // _IGESDimen_SectionedArea_HeaderFile
