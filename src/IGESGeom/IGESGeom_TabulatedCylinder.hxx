// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGeom_TabulatedCylinder_HeaderFile
#define _IGESGeom_TabulatedCylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt;


class IGESGeom_TabulatedCylinder;
DEFINE_STANDARD_HANDLE(IGESGeom_TabulatedCylinder, IGESData_IGESEntity)

//! defines IGESTabulatedCylinder, Type <122> Form <0>
//! in package IGESGeom
//! A tabulated cylinder is a surface formed by moving a line
//! segment called generatrix parallel to itself along a curve
//! called directrix. The curve may be a line, circular arc,
//! conic arc, parametric spline curve, rational B-spline
//! curve or composite curve.
class IGESGeom_TabulatedCylinder : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGeom_TabulatedCylinder();
  
  //! This method is used to set the fields of the class
  //! TabulatedCylinder
  //! - aDirectrix : Directrix Curve of the tabulated cylinder
  //! - anEnd      : Coordinates of the terminate point of the
  //! generatrix
  //! The start point of the directrix is identical to the start
  //! point of the generatrix
  Standard_EXPORT void Init (const Handle(IGESData_IGESEntity)& aDirectrix, const gp_XYZ& anEnd);
  
  //! returns the directrix curve of the tabulated cylinder
  Standard_EXPORT Handle(IGESData_IGESEntity) Directrix() const;
  
  //! returns end point of generatrix of the tabulated cylinder
  Standard_EXPORT gp_Pnt EndPoint() const;
  
  //! returns end point of generatrix of the tabulated cylinder
  //! after applying Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedEndPoint() const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_TabulatedCylinder,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_IGESEntity) theDirectrix;
  gp_XYZ theEnd;


};







#endif // _IGESGeom_TabulatedCylinder_HeaderFile
