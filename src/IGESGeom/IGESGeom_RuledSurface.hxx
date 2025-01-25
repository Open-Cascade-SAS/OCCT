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

#ifndef _IGESGeom_RuledSurface_HeaderFile
#define _IGESGeom_RuledSurface_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>

class IGESGeom_RuledSurface;
DEFINE_STANDARD_HANDLE(IGESGeom_RuledSurface, IGESData_IGESEntity)

//! defines IGESRuledSurface, Type <118> Form <0-1>
//! in package IGESGeom
//! A ruled surface is formed by moving a line connecting points
//! of equal relative arc length or equal relative parametric
//! value on two parametric curves from a start point to a
//! terminate point on the curves. The parametric curves may be
//! points, lines, circles, conics, rational B-splines,
//! parametric splines or any parametric curve defined in
//! the IGES specification.
class IGESGeom_RuledSurface : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_RuledSurface();

  //! This method is used to set the fields of the class
  //! RuledSurface
  //! - aCurve       : First parametric curve
  //! - anotherCurve : Second parametric curve
  //! - aDirFlag     : Direction Flag
  //! 0 = Join first to first, last to last
  //! 1 = Join first to last, last to first
  //! - aDevFlag     : Developable Surface Flag
  //! 1 = Developable
  //! 0 = Possibly not
  Standard_EXPORT void Init(const Handle(IGESData_IGESEntity)& aCurve,
                            const Handle(IGESData_IGESEntity)& anotherCurve,
                            const Standard_Integer             aDirFlag,
                            const Standard_Integer             aDevFlag);

  //! Sets <me> to be Ruled by Parameter (Form 1) if <mode> is
  //! True, or Ruled by Length (Form 0) else
  Standard_EXPORT void SetRuledByParameter(const Standard_Boolean mode);

  //! Returns True if Form is 1
  Standard_EXPORT Standard_Boolean IsRuledByParameter() const;

  //! returns the first curve
  Standard_EXPORT Handle(IGESData_IGESEntity) FirstCurve() const;

  //! returns the second curve
  Standard_EXPORT Handle(IGESData_IGESEntity) SecondCurve() const;

  //! return the sense of direction
  //! 0 = Join first to first, last to last
  //! 1 = Join first to last, last to first
  Standard_EXPORT Standard_Integer DirectionFlag() const;

  //! returns True if developable else False
  Standard_EXPORT Standard_Boolean IsDevelopable() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_RuledSurface, IGESData_IGESEntity)

protected:
private:
  Handle(IGESData_IGESEntity) theCurve1;
  Handle(IGESData_IGESEntity) theCurve2;
  Standard_Integer            theDirFlag;
  Standard_Integer            theDevFlag;
};

#endif // _IGESGeom_RuledSurface_HeaderFile
