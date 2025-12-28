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

#ifndef _StepShape_RightAngularWedge_HeaderFile
#define _StepShape_RightAngularWedge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_GeometricRepresentationItem.hxx>
class StepGeom_Axis2Placement3d;
class TCollection_HAsciiString;

class StepShape_RightAngularWedge : public StepGeom_GeometricRepresentationItem
{

public:
  //! Returns a RightAngularWedge
  Standard_EXPORT StepShape_RightAngularWedge();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&  aName,
                            const occ::handle<StepGeom_Axis2Placement3d>& aPosition,
                            const double                      aX,
                            const double                      aY,
                            const double                      aZ,
                            const double                      aLtx);

  Standard_EXPORT void SetPosition(const occ::handle<StepGeom_Axis2Placement3d>& aPosition);

  Standard_EXPORT occ::handle<StepGeom_Axis2Placement3d> Position() const;

  Standard_EXPORT void SetX(const double aX);

  Standard_EXPORT double X() const;

  Standard_EXPORT void SetY(const double aY);

  Standard_EXPORT double Y() const;

  Standard_EXPORT void SetZ(const double aZ);

  Standard_EXPORT double Z() const;

  Standard_EXPORT void SetLtx(const double aLtx);

  Standard_EXPORT double Ltx() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_RightAngularWedge, StepGeom_GeometricRepresentationItem)

private:
  occ::handle<StepGeom_Axis2Placement3d> position;
  double                     x;
  double                     y;
  double                     z;
  double                     ltx;
};

#endif // _StepShape_RightAngularWedge_HeaderFile
