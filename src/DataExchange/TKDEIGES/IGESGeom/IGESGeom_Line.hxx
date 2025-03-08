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

#ifndef _IGESGeom_Line_HeaderFile
#define _IGESGeom_Line_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt;

class IGESGeom_Line;
DEFINE_STANDARD_HANDLE(IGESGeom_Line, IGESData_IGESEntity)

//! defines IGESLine, Type <110> Form <0>
//! in package IGESGeom
//! A line is a bounded, connected portion of a parent straight
//! line which consists of more than one point. A line is
//! defined by its end points.
//!
//! From IGES-5.3, two other Forms are admitted (same params) :
//! 0 remains for standard limited line (the default)
//! 1 for semi-infinite line (End is just a passing point)
//! 2 for full infinite Line (both Start and End are arbitrary)
class IGESGeom_Line : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_Line();

  //! This method is used to set the fields of the class Line
  //! - aStart : Start point of the line
  //! - anEnd  : End point of the line
  Standard_EXPORT void Init(const gp_XYZ& aStart, const gp_XYZ& anEnd);

  //! Returns the Infinite status i.e. the Form Number : 0 1 2
  Standard_EXPORT Standard_Integer Infinite() const;

  //! Sets the Infinite status
  //! Does nothing if <status> is not 0 1 or 2
  Standard_EXPORT void SetInfinite(const Standard_Integer status);

  //! returns the start point of the line
  Standard_EXPORT gp_Pnt StartPoint() const;

  //! returns the start point of the line after applying Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedStartPoint() const;

  //! returns the end point of the line
  Standard_EXPORT gp_Pnt EndPoint() const;

  //! returns the end point of the line after applying Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedEndPoint() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_Line, IGESData_IGESEntity)

protected:
private:
  gp_XYZ theStart;
  gp_XYZ theEnd;
};

#endif // _IGESGeom_Line_HeaderFile
