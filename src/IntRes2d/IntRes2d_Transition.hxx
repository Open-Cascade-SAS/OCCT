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

#ifndef _IntRes2d_Transition_HeaderFile
#define _IntRes2d_Transition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IntRes2d_Position.hxx>
#include <IntRes2d_TypeTrans.hxx>
#include <IntRes2d_Situation.hxx>


//! Definition of    the  type  of   transition    near an
//! intersection point between  two curves. The transition
//! is either a "true transition", which means that one of
//! the curves goes inside or outside  the area defined by
//! the other curve  near  the intersection, or  a  "touch
//! transition" which means that the  first curve does not
//! cross  the  other one,  or an  "undecided" transition,
//! which means that the curves are superposed.
class IntRes2d_Transition 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT IntRes2d_Transition();
  
  //! Creates an IN or OUT transition.
    IntRes2d_Transition(const Standard_Boolean Tangent, const IntRes2d_Position Pos, const IntRes2d_TypeTrans Type);
  
  //! Creates a TOUCH transition.
    IntRes2d_Transition(const Standard_Boolean Tangent, const IntRes2d_Position Pos, const IntRes2d_Situation Situ, const Standard_Boolean Oppos);
  
  //! Creates an UNDECIDED transition.
    IntRes2d_Transition(const IntRes2d_Position Pos);
  
  //! Sets the values of an IN or OUT transition.
    void SetValue (const Standard_Boolean Tangent, const IntRes2d_Position Pos, const IntRes2d_TypeTrans Type);
  
  //! Sets the values of a TOUCH transition.
    void SetValue (const Standard_Boolean Tangent, const IntRes2d_Position Pos, const IntRes2d_Situation Situ, const Standard_Boolean Oppos);
  
  //! Sets the values of an UNDECIDED transition.
    void SetValue (const IntRes2d_Position Pos);
  
  //! Sets the value of the position.
    void SetPosition (const IntRes2d_Position Pos);
  
  //! Indicates if the  intersection is at the beginning
  //! (IntRes2d_Head),  at the end (IntRes2d_End), or in
  //! the middle (IntRes2d_Middle) of the curve.
    IntRes2d_Position PositionOnCurve() const;
  
  //! Returns the type of transition at the intersection.
  //! It may be IN or OUT or TOUCH, or UNDECIDED if the
  //! two first derivatives are not enough to give
  //! the tangent to one of the two curves.
    IntRes2d_TypeTrans TransitionType() const;
  
  //! Returns TRUE when the 2 curves are tangent at the
  //! intersection point.
  //! Theexception DomainError is raised if the type of
  //! transition is UNDECIDED.
    Standard_Boolean IsTangent() const;
  
  //! returns a significant value if TransitionType returns
  //! TOUCH. In this case, the function returns :
  //! INSIDE when the curve remains inside the other one,
  //! OUTSIDE when it remains outside the other one,
  //! UNKNOWN when the calculus, based on the second derivatives
  //! cannot give the result.
  //! If TransitionType returns IN or OUT or UNDECIDED, the
  //! exception DomainError is raised.
    IntRes2d_Situation Situation() const;
  
  //! returns a  significant value   if   TransitionType
  //! returns TOUCH. In this  case, the function returns
  //! true   when  the  2   curves   locally define  two
  //! different  parts of the  space.  If TransitionType
  //! returns  IN or   OUT or UNDECIDED,  the  exception
  //! DomainError is raised.
    Standard_Boolean IsOpposite() const;




protected:





private:



  Standard_Boolean tangent;
  IntRes2d_Position posit;
  IntRes2d_TypeTrans typetra;
  IntRes2d_Situation situat;
  Standard_Boolean oppos;


};


#include <IntRes2d_Transition.lxx>





#endif // _IntRes2d_Transition_HeaderFile
