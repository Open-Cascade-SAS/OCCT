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

#ifndef _IGESGeom_Plane_HeaderFile
#define _IGESGeom_Plane_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt;

class IGESGeom_Plane;
DEFINE_STANDARD_HANDLE(IGESGeom_Plane, IGESData_IGESEntity)

//! defines IGESPlane, Type <108> Form <-1,0,1>
//! in package IGESGeom
//! A plane entity can be used to represent unbounded plane,
//! as well as bounded portion of a plane. In either of the
//! above cases the plane is defined within definition space
//! by means of coefficients A, B, C, D where at least one of
//! A, B, C is non-zero and  A * XT + B * YT + C * ZT = D
class IGESGeom_Plane : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_Plane();

  Standard_EXPORT void Init(const Standard_Real                A,
                            const Standard_Real                B,
                            const Standard_Real                C,
                            const Standard_Real                D,
                            const Handle(IGESData_IGESEntity)& aCurve,
                            const gp_XYZ&                      attach,
                            const Standard_Real                aSize);

  //! Changes FormNumber (indicates the Type of Bound :
  //! 0 no Bound, 1 (External) Bound, -1 Hole)
  //! Remark that Init keeps this Value and must be consistent :
  //! aCurve Null if FormNumber = 0, Non-Null else
  //! Error if not in ranges [0-1] or [10-12]
  Standard_EXPORT void SetFormNumber(const Standard_Integer form);

  Standard_EXPORT void Equation(Standard_Real& A,
                                Standard_Real& B,
                                Standard_Real& C,
                                Standard_Real& D) const;

  Standard_EXPORT void TransformedEquation(Standard_Real& A,
                                           Standard_Real& B,
                                           Standard_Real& C,
                                           Standard_Real& D) const;

  //! returns True if there exists a bounding curve
  Standard_EXPORT Standard_Boolean HasBoundingCurve() const;

  //! returns True if bounding curve exists and bounded portion is negative
  Standard_EXPORT Standard_Boolean HasBoundingCurveHole() const;

  //! returns Optional Bounding Curve, can be positive (normal clipping)
  //! or negative (hole) according to Form Number
  Standard_EXPORT Handle(IGESData_IGESEntity) BoundingCurve() const;

  //! returns True if SymbolSize() > 0, False if SymbolSize() = 0
  Standard_EXPORT Standard_Boolean HasSymbolAttach() const;

  //! returns (X, Y, Z) if symbol exists else returns (0, 0, 0)
  Standard_EXPORT gp_Pnt SymbolAttach() const;

  //! returns (X, Y, Z) if symbol exists after applying Transf. Matrix
  //! else returns (0, 0, 0)
  Standard_EXPORT gp_Pnt TransformedSymbolAttach() const;

  //! Size of optional display symbol
  Standard_EXPORT Standard_Real SymbolSize() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_Plane, IGESData_IGESEntity)

protected:
private:
  Standard_Real               theA;
  Standard_Real               theB;
  Standard_Real               theC;
  Standard_Real               theD;
  Handle(IGESData_IGESEntity) theCurve;
  gp_XYZ                      theAttach;
  Standard_Real               theSize;
};

#endif // _IGESGeom_Plane_HeaderFile
