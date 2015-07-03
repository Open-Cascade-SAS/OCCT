// Created on: 1991-12-02
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _AppDef_MultiPointConstraint_HeaderFile
#define _AppDef_MultiPointConstraint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppParCurves_MultiPoint.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TColgp_HArray1OfVec2d.hxx>

class Standard_OutOfRange;
class Standard_ConstructionError;
class Standard_DimensionError;
class gp_Vec;
class gp_Vec2d;

//! Describes a MultiPointConstraint used in a
//! Multiline. MultiPointConstraints are composed
//! of several two or three-dimensional points.
//! The purpose is to define the corresponding
//! points that share a common constraint in order
//! to compute the approximation of several lines in parallel.
//! Notes:
//! -   The order of points of a MultiPointConstraints is very important.
//! Users must give 3D points first, and then 2D points.
//! -   The constraints for the points included in a
//! MultiPointConstraint are always identical for
//! all points, including the parameter.
//! -   If a MultiPointConstraint is a "tangency"
//! point, the point is also a "passing" point.
class AppDef_MultiPointConstraint  : public AppParCurves_MultiPoint
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates an undefined MultiPointConstraint.
  Standard_EXPORT AppDef_MultiPointConstraint();
  
  //! constructs a set of Points used to approximate a Multiline.
  //! These Points can be of 2 or 3 dimensions.
  //! Points will be initialized with SetPoint and SetPoint2d.
  Standard_EXPORT AppDef_MultiPointConstraint(const Standard_Integer NbPoints, const Standard_Integer NbPoints2d);
  
  //! creates a MultiPoint only composed of 3D points.
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt& tabP);
  
  //! creates a MultiPoint only composed of 2D points.
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt2d& tabP);
  
  //! constructs a set of Points used to approximate a Multiline.
  //! These Points can be of 2 or 3 dimensions.
  //! Points will be initialized with SetPoint and SetPoint2d.
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt& tabP, const TColgp_Array1OfPnt2d& tabP2d);
  
  //! creates a MultiPointConstraint with a constraint of
  //! Curvature.
  //! An exception is raised if
  //! (length of <tabP> + length of <tabP2d> ) is different
  //! from (length of <tabVec> + length of <tabVec2d> ) or
  //! from (length of <tabCur> + length of <tabCur2d> )
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt& tabP, const TColgp_Array1OfPnt2d& tabP2d, const TColgp_Array1OfVec& tabVec, const TColgp_Array1OfVec2d& tabVec2d, const TColgp_Array1OfVec& tabCur, const TColgp_Array1OfVec2d& tabCur2d);
  
  //! creates a MultiPointConstraint with a constraint of
  //! Tangency.
  //! An exception is raised if
  //! (length of <tabP> + length of <tabP2d> ) is different
  //! from (length of <tabVec> + length of <tabVec2d> )
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt& tabP, const TColgp_Array1OfPnt2d& tabP2d, const TColgp_Array1OfVec& tabVec, const TColgp_Array1OfVec2d& tabVec2d);
  
  //! creates a MultiPointConstraint only composed of 3d points
  //! with constraints of curvature.
  //! An exception is raised if the length of tabP is different
  //! from the length of tabVec or from tabCur.
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt& tabP, const TColgp_Array1OfVec& tabVec, const TColgp_Array1OfVec& tabCur);
  
  //! creates a MultiPointConstraint only composed of 3d points
  //! with constraints of tangency.
  //! An exception is raised if the length of tabP is different
  //! from the length of tabVec.
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt& tabP, const TColgp_Array1OfVec& tabVec);
  
  //! creates a MultiPointConstraint only composed of 2d points
  //! with constraints of tangency.
  //! An exception is raised if the length of tabP is different
  //! from the length of tabVec2d.
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt2d& tabP2d, const TColgp_Array1OfVec2d& tabVec2d);
  
  //! creates a MultiPointConstraint only composed of 2d points
  //! with constraints of curvature.
  //! An exception is raised if the length of tabP is different
  //! from the length of tabVec2d or from tabCur2d.
  Standard_EXPORT AppDef_MultiPointConstraint(const TColgp_Array1OfPnt2d& tabP2d, const TColgp_Array1OfVec2d& tabVec2d, const TColgp_Array1OfVec2d& tabCur2d);
  
  //! sets the value of the tangency of the point of range
  //! Index.
  //! An exception is raised if Index <0 or if Index > number
  //! of 3d points.
  //! An exception is raised if Tang has an incorrect number of
  //! dimensions.
  Standard_EXPORT void SetTang (const Standard_Integer Index, const gp_Vec& Tang);
  
  //! returns the tangency value of the point of range Index.
  //! An exception is raised if Index < 0 or if Index > number
  //! of 3d points.
  Standard_EXPORT gp_Vec Tang (const Standard_Integer Index) const;
  
  //! sets the value of the tangency of the point of range
  //! Index.
  //! An exception is raised if Index <number of 3d points or if
  //! Index > total number of Points
  //! An exception is raised if Tang has an incorrect number of
  //! dimensions.
  Standard_EXPORT void SetTang2d (const Standard_Integer Index, const gp_Vec2d& Tang2d);
  
  //! returns the tangency value of the point of range Index.
  //! An exception is raised if Index < number  of 3d points or
  //! if Index > total number of points.
  Standard_EXPORT gp_Vec2d Tang2d (const Standard_Integer Index) const;
  
  //! Vec sets the value of the normal vector at the
  //! point of index Index. The norm of the normal
  //! vector at the point of position Index is set to the normal curvature.
  //! An exception is raised if Index <0 or if Index > number
  //! of 3d points.
  //! An exception is raised if Curv has an incorrect number of
  //! dimensions.
  Standard_EXPORT void SetCurv (const Standard_Integer Index, const gp_Vec& Curv);
  
  //! returns the normal vector at the point of range Index.
  //! An exception is raised if Index < 0 or if Index > number
  //! of 3d points.
  Standard_EXPORT gp_Vec Curv (const Standard_Integer Index) const;
  
  //! Vec sets the value of the normal vector at the
  //! point of index Index. The norm of the normal
  //! vector at the point of position Index is set to the normal curvature.
  //! An exception is raised if Index <0 or if Index > number
  //! of 3d points.
  //! An exception is raised if Curv has an incorrect number of
  //! dimensions.
  Standard_EXPORT void SetCurv2d (const Standard_Integer Index, const gp_Vec2d& Curv2d);
  
  //! returns the normal vector at the point of range Index.
  //! An exception is raised if Index < 0 or if Index > number
  //! of 3d points.
  Standard_EXPORT gp_Vec2d Curv2d (const Standard_Integer Index) const;
  
  //! returns True if the MultiPoint has a tangency value.
  Standard_EXPORT Standard_Boolean IsTangencyPoint() const;
  
  //! returns True if the MultiPoint has a curvature value.
  Standard_EXPORT Standard_Boolean IsCurvaturePoint() const;
  
  //! Prints on the stream o information on the current
  //! state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT virtual void Dump (Standard_OStream& o) const Standard_OVERRIDE;

private:
  Handle(TColgp_HArray1OfVec) tabTang;
  Handle(TColgp_HArray1OfVec) tabCurv;
  Handle(TColgp_HArray1OfVec2d) tabTang2d;
  Handle(TColgp_HArray1OfVec2d) tabCurv2d;
};

#endif // _AppDef_MultiPointConstraint_HeaderFile
