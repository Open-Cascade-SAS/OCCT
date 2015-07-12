// Created on: 1996-01-26
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _FairCurve_BattenLaw_HeaderFile
#define _FairCurve_BattenLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <math_Function.hxx>
#include <Standard_Boolean.hxx>


//! This class compute the Heigth of an batten
class FairCurve_BattenLaw  : public math_Function
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructor of linear batten with
  //! Heigth : the Heigth at the middle point
  //! Slope  : the geometric slope of the batten
  //! Sliding : Active Length of the batten without extension
  Standard_EXPORT FairCurve_BattenLaw(const Standard_Real Heigth, const Standard_Real Slope, const Standard_Real Sliding);
  
  //! Change the value of sliding
    void SetSliding (const Standard_Real Sliding);
  
  //! Change the value of Heigth at the middle point.
    void SetHeigth (const Standard_Real Heigth);
  
  //! Change the value of the geometric slope.
    void SetSlope (const Standard_Real Slope);
  
  //! computes the value of  the heigth for the parameter T
  //! on  the neutral fibber
    virtual Standard_Boolean Value (const Standard_Real T, Standard_Real& THeigth) Standard_OVERRIDE;




protected:





private:



  Standard_Real MiddleHeigth;
  Standard_Real GeometricSlope;
  Standard_Real LengthSliding;


};


#include <FairCurve_BattenLaw.lxx>





#endif // _FairCurve_BattenLaw_HeaderFile
