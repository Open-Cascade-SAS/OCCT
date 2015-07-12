// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
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

#ifndef _BRepBlend_PointOnRst_HeaderFile
#define _BRepBlend_PointOnRst_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntSurf_Transition.hxx>
#include <Standard_Real.hxx>
class Adaptor2d_HCurve2d;
class Standard_DomainError;
class IntSurf_Transition;


//! Definition of an intersection point between a line
//! and a restriction on a surface.
//! Such a point is contains geometrical informations (see
//! the Value method) and logical informations.
class BRepBlend_PointOnRst 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT BRepBlend_PointOnRst();
  
  //! Creates the PointOnRst on the arc A, at parameter Param,
  //! with the transition TLine on the walking line, and
  //! TArc on the arc A.
  Standard_EXPORT BRepBlend_PointOnRst(const Handle(Adaptor2d_HCurve2d)& A, const Standard_Real Param, const IntSurf_Transition& TLine, const IntSurf_Transition& TArc);
  
  //! Sets the values of a point which is on the arc
  //! A, at parameter Param.
  Standard_EXPORT void SetArc (const Handle(Adaptor2d_HCurve2d)& A, const Standard_Real Param, const IntSurf_Transition& TLine, const IntSurf_Transition& TArc);
  
  //! Returns the arc of restriction containing the
  //! vertex.
    const Handle(Adaptor2d_HCurve2d)& Arc() const;
  
  //! Returns the transition of the point on the
  //! line on surface.
    const IntSurf_Transition& TransitionOnLine() const;
  
  //! Returns the transition of the point on the arc
  //! returned by Arc().
    const IntSurf_Transition& TransitionOnArc() const;
  
  //! Returns the parameter of the point on the
  //! arc returned by the method Arc().
    Standard_Real ParameterOnArc() const;




protected:





private:



  Handle(Adaptor2d_HCurve2d) arc;
  IntSurf_Transition traline;
  IntSurf_Transition traarc;
  Standard_Real prm;


};


#include <BRepBlend_PointOnRst.lxx>





#endif // _BRepBlend_PointOnRst_HeaderFile
