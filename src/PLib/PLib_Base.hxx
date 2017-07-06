// Created on: 1997-10-22
// Created by: Philippe MANGIN / Sergey SOKOLOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _PLib_Base_HeaderFile
#define _PLib_Base_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>


class PLib_Base;
DEFINE_STANDARD_HANDLE(PLib_Base, Standard_Transient)

//! To work with different polynomial's Bases
class PLib_Base : public Standard_Transient
{

public:

  

  //! Convert the polynomial P(t) in the canonical base.
  Standard_EXPORT virtual void ToCoefficients (const Standard_Integer Dimension, const Standard_Integer Degree, const TColStd_Array1OfReal& CoeffinBase, TColStd_Array1OfReal& Coefficients) const = 0;
  
  //! Compute the values of the basis functions in u
  Standard_EXPORT virtual void D0 (const Standard_Real U, TColStd_Array1OfReal& BasisValue) = 0;
  
  //! Compute the values and the derivatives values of
  //! the basis functions in u
  Standard_EXPORT virtual void D1 (const Standard_Real U, TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1) = 0;
  
  //! Compute the values and the derivatives values of
  //! the basis functions in u
  Standard_EXPORT virtual void D2 (const Standard_Real U, TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1, TColStd_Array1OfReal& BasisD2) = 0;
  
  //! Compute the values and the derivatives values of
  //! the basis functions in u
  Standard_EXPORT virtual void D3 (const Standard_Real U, TColStd_Array1OfReal& BasisValue, TColStd_Array1OfReal& BasisD1, TColStd_Array1OfReal& BasisD2, TColStd_Array1OfReal& BasisD3) = 0;
  
  //! returns WorkDegree
  Standard_EXPORT virtual Standard_Integer WorkDegree() const = 0;
  

  //! Compute NewDegree <= MaxDegree so that MaxError is lower
  //! than Tol.
  //! MaxError can be greater than Tol if it is not possible
  //! to find a NewDegree <= MaxDegree.
  //! In this case NewDegree = MaxDegree
  Standard_EXPORT virtual void ReduceDegree (const Standard_Integer Dimension, const Standard_Integer MaxDegree, const Standard_Real Tol, Standard_Real& BaseCoeff, Standard_Integer& NewDegree, Standard_Real& MaxError) const = 0;




  DEFINE_STANDARD_RTTIEXT(PLib_Base,Standard_Transient)

protected:




private:




};







#endif // _PLib_Base_HeaderFile
