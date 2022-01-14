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

#ifndef _IGESGeom_CurveOnSurface_HeaderFile
#define _IGESGeom_CurveOnSurface_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>


class IGESGeom_CurveOnSurface;
DEFINE_STANDARD_HANDLE(IGESGeom_CurveOnSurface, IGESData_IGESEntity)

//! defines IGESCurveOnSurface, Type <142> Form <0>
//! in package IGESGeom
//! A curve on a parametric surface entity associates a given
//! curve with a surface and identifies the curve as lying on
//! the surface.
class IGESGeom_CurveOnSurface : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESGeom_CurveOnSurface();
  
  //! This method is used to set the fields of the class
  //! CurveOnSurface
  //! - aMode       : Way the curve on the surface has been created
  //! - aSurface    : Surface on which the curve lies
  //! - aCurveUV    : Curve S (UV)
  //! - aCurve3D    : Curve C (3D)
  //! - aPreference : 0 = Unspecified
  //! 1 = S o B is preferred
  //! 2 = C is preferred
  //! 3 = C and S o B are equally preferred
  Standard_EXPORT void Init (const Standard_Integer aMode, const Handle(IGESData_IGESEntity)& aSurface, const Handle(IGESData_IGESEntity)& aCurveUV, const Handle(IGESData_IGESEntity)& aCurve3D, const Standard_Integer aPreference);
  
  //! returns the mode in which the curve is created on the surface
  //! 0 = Unspecified
  //! 1 = Projection of a given curve on the surface
  //! 2 = Intersection of two surfaces
  //! 3 = Isoparametric curve, i.e:- either a `u` parametric
  //! or a `v` parametric curve
  Standard_EXPORT Standard_Integer CreationMode() const;
  
  //! returns the surface on which the curve lies
  Standard_EXPORT Handle(IGESData_IGESEntity) Surface() const;
  
  //! returns curve S
  Standard_EXPORT Handle(IGESData_IGESEntity) CurveUV() const;
  
  //! returns curve C
  Standard_EXPORT Handle(IGESData_IGESEntity) Curve3D() const;
  
  //! returns preference mode
  //! 0 = Unspecified
  //! 1 = S o B is preferred
  //! 2 = C is preferred
  //! 3 = C and S o B are equally preferred
  Standard_EXPORT Standard_Integer PreferenceMode() const;




  DEFINE_STANDARD_RTTIEXT(IGESGeom_CurveOnSurface,IGESData_IGESEntity)

protected:




private:


  Standard_Integer theCreationMode;
  Handle(IGESData_IGESEntity) theSurface;
  Handle(IGESData_IGESEntity) theCurveUV;
  Handle(IGESData_IGESEntity) theCurve3D;
  Standard_Integer thePreferenceMode;


};







#endif // _IGESGeom_CurveOnSurface_HeaderFile
