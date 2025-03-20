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

#ifndef _StepGeom_CartesianTransformationOperator3d_HeaderFile
#define _StepGeom_CartesianTransformationOperator3d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_CartesianTransformationOperator.hxx>
class StepGeom_Direction;
class TCollection_HAsciiString;
class StepGeom_CartesianPoint;

class StepGeom_CartesianTransformationOperator3d;
DEFINE_STANDARD_HANDLE(StepGeom_CartesianTransformationOperator3d,
                       StepGeom_CartesianTransformationOperator)

class StepGeom_CartesianTransformationOperator3d : public StepGeom_CartesianTransformationOperator
{

public:
  //! Returns a CartesianTransformationOperator3d
  Standard_EXPORT StepGeom_CartesianTransformationOperator3d();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Standard_Boolean                  hasAaxis1,
                            const Handle(StepGeom_Direction)&       aAxis1,
                            const Standard_Boolean                  hasAaxis2,
                            const Handle(StepGeom_Direction)&       aAxis2,
                            const Handle(StepGeom_CartesianPoint)&  aLocalOrigin,
                            const Standard_Boolean                  hasAscale,
                            const Standard_Real                     aScale,
                            const Standard_Boolean                  hasAaxis3,
                            const Handle(StepGeom_Direction)&       aAxis3);

  Standard_EXPORT void SetAxis3(const Handle(StepGeom_Direction)& aAxis3);

  Standard_EXPORT void UnSetAxis3();

  Standard_EXPORT Handle(StepGeom_Direction) Axis3() const;

  Standard_EXPORT Standard_Boolean HasAxis3() const;

  DEFINE_STANDARD_RTTIEXT(StepGeom_CartesianTransformationOperator3d,
                          StepGeom_CartesianTransformationOperator)

protected:
private:
  Handle(StepGeom_Direction) axis3;
  Standard_Boolean           hasAxis3;
};

#endif // _StepGeom_CartesianTransformationOperator3d_HeaderFile
