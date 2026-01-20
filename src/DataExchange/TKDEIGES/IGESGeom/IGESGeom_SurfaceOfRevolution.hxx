// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
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

#ifndef _IGESGeom_SurfaceOfRevolution_HeaderFile
#define _IGESGeom_SurfaceOfRevolution_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
class IGESGeom_Line;

//! defines IGESSurfaceOfRevolution, Type <120> Form <0>
//! in package IGESGeom
//! A surface of revolution is defined by an axis of rotation
//! a generatrix, and start and terminate rotation angles. The
//! surface is created by rotating the generatrix about the axis
//! of rotation through the start and terminate rotation angles.
class IGESGeom_SurfaceOfRevolution : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_SurfaceOfRevolution();

  //! This method is used to set the fields of the class Line
  //! - anAxis      : Axis of revolution
  //! - aGeneratrix : The curve which is revolved about the axis
  //! - aStartAngle : Start angle of the surface of revolution
  //! - anEndAngle  : End angle of the surface of revolution
  Standard_EXPORT void Init(const occ::handle<IGESGeom_Line>&       anAxis,
                            const occ::handle<IGESData_IGESEntity>& aGeneratrix,
                            const double                aStartAngle,
                            const double                anEndAngle);

  //! returns the axis of revolution
  Standard_EXPORT occ::handle<IGESGeom_Line> AxisOfRevolution() const;

  //! returns the curve which is revolved about the axis
  Standard_EXPORT occ::handle<IGESData_IGESEntity> Generatrix() const;

  //! returns start angle of revolution
  Standard_EXPORT double StartAngle() const;

  //! returns end angle of revolution
  Standard_EXPORT double EndAngle() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_SurfaceOfRevolution, IGESData_IGESEntity)

private:
  occ::handle<IGESGeom_Line>       theLine;
  occ::handle<IGESData_IGESEntity> theGeneratrix;
  double               theStartAngle;
  double               theEndAngle;
};

#endif // _IGESGeom_SurfaceOfRevolution_HeaderFile
