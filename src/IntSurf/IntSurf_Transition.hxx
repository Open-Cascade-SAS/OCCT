// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
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

#ifndef _IntSurf_Transition_HeaderFile
#define _IntSurf_Transition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <IntSurf_Situation.hxx>


//! Definition of the transition at the intersection
//! between an intersection line and a restriction curve
//! on a surface.
class IntSurf_Transition 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor. Creates an UNDECIDED transition.
  Standard_EXPORT IntSurf_Transition();
  
  //! Create a IN or OUT transition
  Standard_EXPORT IntSurf_Transition(const Standard_Boolean Tangent, const IntSurf_TypeTrans Type);
  
  //! Create a TOUCH transition.
  Standard_EXPORT IntSurf_Transition(const Standard_Boolean Tangent, const IntSurf_Situation Situ, const Standard_Boolean Oppos);
  
  //! Set the values of an IN or OUT transition.
    void SetValue (const Standard_Boolean Tangent, const IntSurf_TypeTrans Type);
  
  //! Set the values of a TOUCH transition.
    void SetValue (const Standard_Boolean Tangent, const IntSurf_Situation Situ, const Standard_Boolean Oppos);
  
  //! Set the values of an UNDECIDED transition.
    void SetValue();
  
  //! Returns the type of Transition (in/out/touch/undecided)
  //! for the arc given by value. This the transition of
  //! the intersection line compared to the Arc of restriction,
  //! i-e when the function returns INSIDE for example, it
  //! means that the intersection line goes inside the
  //! part of plane limited by the arc of restriction.
    IntSurf_TypeTrans TransitionType() const;
  
  //! Returns TRUE if the point is tangent to the arc
  //! given by Value.
  //! An exception is raised if TransitionType returns UNDECIDED.
    Standard_Boolean IsTangent() const;
  
  //! Returns a significant value if TransitionType returns
  //! TOUCH. In this case, the function returns :
  //! INSIDE when the intersection line remains inside the Arc,
  //! OUTSIDE when it remains outside the Arc,
  //! UNKNOWN when the calsulus cannot give results.
  //! If TransitionType returns IN, or OUT, or UNDECIDED, a
  //! exception is raised.
    IntSurf_Situation Situation() const;
  
  //! returns a significant value if TransitionType returns
  //! TOUCH.
  //! In this case, the function returns true when
  //! the 2 curves locally define two different parts of the
  //! space.
  //! If TransitionType returns IN or OUT or UNDECIDED, an
  //! exception is raised.
    Standard_Boolean IsOpposite() const;




protected:





private:



  Standard_Boolean tangent;
  IntSurf_TypeTrans typetra;
  IntSurf_Situation situat;
  Standard_Boolean oppos;


};


#include <IntSurf_Transition.lxx>





#endif // _IntSurf_Transition_HeaderFile
