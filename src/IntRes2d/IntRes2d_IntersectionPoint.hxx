// Created on: 1992-04-03
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntRes2d_IntersectionPoint_HeaderFile
#define _IntRes2d_IntersectionPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
#include <IntRes2d_Transition.hxx>
#include <Standard_Boolean.hxx>


//! Definition of an intersection point between two
//! 2D curves.
class IntRes2d_IntersectionPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT IntRes2d_IntersectionPoint();
  
  //! Creates an IntersectionPoint.
  //! if ReversedFlag is False, the parameter Uc1(resp. Uc2)
  //! and the Transition Trans1 (resp. Trans2) refer to
  //! the first curve (resp. second curve) otherwise Uc1
  //! and Trans1 (resp. Uc2 and Trans2) refer to the
  //! second curve (resp. the first curve).
    IntRes2d_IntersectionPoint(const gp_Pnt2d& P, const Standard_Real Uc1, const Standard_Real Uc2, const IntRes2d_Transition& Trans1, const IntRes2d_Transition& Trans2, const Standard_Boolean ReversedFlag);
  
  //! Sets the values for an existing intersection
  //! point. The meaning of the parameters are the same
  //! as for the Create.
    void SetValues (const gp_Pnt2d& P, const Standard_Real Uc1, const Standard_Real Uc2, const IntRes2d_Transition& Trans1, const IntRes2d_Transition& Trans2, const Standard_Boolean ReversedFlag);
  
  //! Returns the value of the coordinates of the
  //! intersection point in the 2D space.
    const gp_Pnt2d& Value() const;
  
  //! Returns the parameter on the first curve.
    Standard_Real ParamOnFirst() const;
  
  //! Returns the parameter on the second curve.
    Standard_Real ParamOnSecond() const;
  
  //! Returns the transition of the 1st curve compared to
  //! the 2nd one.
    const IntRes2d_Transition& TransitionOfFirst() const;
  
  //! returns the transition of the 2nd curve compared to
  //! the 1st one.
    const IntRes2d_Transition& TransitionOfSecond() const;




protected:





private:



  gp_Pnt2d pt;
  Standard_Real p1;
  Standard_Real p2;
  IntRes2d_Transition trans1;
  IntRes2d_Transition trans2;


};


#include <IntRes2d_IntersectionPoint.lxx>





#endif // _IntRes2d_IntersectionPoint_HeaderFile
