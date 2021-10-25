// Created on: 1991-02-20
// Created by: Jacques GOUSSARD
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

#ifndef _IntAna2d_IntPoint_HeaderFile
#define _IntAna2d_IntPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Boolean.hxx>


//! Geometrical intersection between two 2d elements.
class IntAna2d_IntPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create an intersection point between 2 parametric 2d lines.
  //! X,Y are the coordinate of the point. U1 is the parameter
  //! on the first element, U2 the parameter on the second one.
  Standard_EXPORT IntAna2d_IntPoint(const Standard_Real X, const Standard_Real Y, const Standard_Real U1, const Standard_Real U2);
  
  //! Create an intersection point between a parametric 2d line,
  //! and a line given by an implicit equation (ImplicitCurve).
  //! X,Y are the coordinate of the point. U1 is the parameter
  //! on the parametric element.
  //! Empty constructor. It's necessary to use one of
  //! the SetValue method after this one.
  Standard_EXPORT IntAna2d_IntPoint(const Standard_Real X, const Standard_Real Y, const Standard_Real U1);
  
  Standard_EXPORT IntAna2d_IntPoint();
  
  //! Set the values for a "non-implicit" point.
  Standard_EXPORT virtual void SetValue (const Standard_Real X, const Standard_Real Y, const Standard_Real U1, const Standard_Real U2);
  
  //! Set the values for an "implicit" point.
  Standard_EXPORT virtual void SetValue (const Standard_Real X, const Standard_Real Y, const Standard_Real U1);
  
  //! Returns the geometric point.
    const gp_Pnt2d& Value() const;
  
  //! Returns True if the second curve is implicit.
    Standard_Boolean SecondIsImplicit() const;
  
  //! Returns the parameter on the first element.
    Standard_Real ParamOnFirst() const;
  
  //! Returns the parameter on the second element.
  //! If the second element is an implicit curve, an exception
  //! is raised.
    Standard_Real ParamOnSecond() const;




protected:





private:



  Standard_Real myu1;
  Standard_Real myu2;
  gp_Pnt2d myp;
  Standard_Boolean myimplicit;


};


#include <IntAna2d_IntPoint.lxx>





#endif // _IntAna2d_IntPoint_HeaderFile
