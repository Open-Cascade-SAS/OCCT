// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepGeom_CartesianTransformationOperator_HeaderFile
#define _StepGeom_CartesianTransformationOperator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_GeometricRepresentationItem.hxx>
class StepGeom_Direction;
class StepGeom_CartesianPoint;
class TCollection_HAsciiString;

class StepGeom_CartesianTransformationOperator : public StepGeom_GeometricRepresentationItem
{

public:
  //! Returns a CartesianTransformationOperator
  Standard_EXPORT StepGeom_CartesianTransformationOperator();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const bool                                   hasAaxis1,
                            const occ::handle<StepGeom_Direction>&       aAxis1,
                            const bool                                   hasAaxis2,
                            const occ::handle<StepGeom_Direction>&       aAxis2,
                            const occ::handle<StepGeom_CartesianPoint>&  aLocalOrigin,
                            const bool                                   hasAscale,
                            const double                                 aScale);

  Standard_EXPORT void SetAxis1(const occ::handle<StepGeom_Direction>& aAxis1);

  Standard_EXPORT void UnSetAxis1();

  Standard_EXPORT occ::handle<StepGeom_Direction> Axis1() const;

  Standard_EXPORT bool HasAxis1() const;

  Standard_EXPORT void SetAxis2(const occ::handle<StepGeom_Direction>& aAxis2);

  Standard_EXPORT void UnSetAxis2();

  Standard_EXPORT occ::handle<StepGeom_Direction> Axis2() const;

  Standard_EXPORT bool HasAxis2() const;

  Standard_EXPORT void SetLocalOrigin(const occ::handle<StepGeom_CartesianPoint>& aLocalOrigin);

  Standard_EXPORT occ::handle<StepGeom_CartesianPoint> LocalOrigin() const;

  Standard_EXPORT void SetScale(const double aScale);

  Standard_EXPORT void UnSetScale();

  Standard_EXPORT double Scale() const;

  Standard_EXPORT bool HasScale() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_CartesianTransformationOperator,
                          StepGeom_GeometricRepresentationItem)

private:
  occ::handle<StepGeom_Direction>      axis1;
  occ::handle<StepGeom_Direction>      axis2;
  occ::handle<StepGeom_CartesianPoint> localOrigin;
  double                               scale;
  bool                                 hasAxis1;
  bool                                 hasAxis2;
  bool                                 hasScale;
};

#endif // _StepGeom_CartesianTransformationOperator_HeaderFile
