// Created on: 1997-05-27
// Created by: Sergey SOKOLOV
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

#ifndef _PLib_DoubleJacobiPolynomial_HeaderFile
#define _PLib_DoubleJacobiPolynomial_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
class PLib_JacobiPolynomial;



class PLib_DoubleJacobiPolynomial 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT PLib_DoubleJacobiPolynomial();
  
  Standard_EXPORT PLib_DoubleJacobiPolynomial(const Handle(PLib_JacobiPolynomial)& JacPolU, const Handle(PLib_JacobiPolynomial)& JacPolV);
  
  Standard_EXPORT Standard_Real MaxErrorU (const Standard_Integer Dimension, const Standard_Integer DegreeU, const Standard_Integer DegreeV, const Standard_Integer dJacCoeff, const TColStd_Array1OfReal& JacCoeff) const;
  
  Standard_EXPORT Standard_Real MaxErrorV (const Standard_Integer Dimension, const Standard_Integer DegreeU, const Standard_Integer DegreeV, const Standard_Integer dJacCoeff, const TColStd_Array1OfReal& JacCoeff) const;
  
  Standard_EXPORT Standard_Real MaxError (const Standard_Integer Dimension, const Standard_Integer MinDegreeU, const Standard_Integer MaxDegreeU, const Standard_Integer MinDegreeV, const Standard_Integer MaxDegreeV, const Standard_Integer dJacCoeff, const TColStd_Array1OfReal& JacCoeff, const Standard_Real Error) const;
  
  Standard_EXPORT void ReduceDegree (const Standard_Integer Dimension, const Standard_Integer MinDegreeU, const Standard_Integer MaxDegreeU, const Standard_Integer MinDegreeV, const Standard_Integer MaxDegreeV, const Standard_Integer dJacCoeff, const TColStd_Array1OfReal& JacCoeff, const Standard_Real EpmsCut, Standard_Real& MaxError, Standard_Integer& NewDegreeU, Standard_Integer& NewDegreeV) const;
  
  Standard_EXPORT Standard_Real AverageError (const Standard_Integer Dimension, const Standard_Integer DegreeU, const Standard_Integer DegreeV, const Standard_Integer dJacCoeff, const TColStd_Array1OfReal& JacCoeff) const;
  
  Standard_EXPORT void WDoubleJacobiToCoefficients (const Standard_Integer Dimension, const Standard_Integer DegreeU, const Standard_Integer DegreeV, const TColStd_Array1OfReal& JacCoeff, TColStd_Array1OfReal& Coefficients) const;
  
  //! returns myJacPolU;
    Handle(PLib_JacobiPolynomial) U() const;
  
  //! returns myJacPolV;
    Handle(PLib_JacobiPolynomial) V() const;
  
  //! returns myTabMaxU;
    Handle(TColStd_HArray1OfReal) TabMaxU() const;
  
  //! returns myTabMaxV;
    Handle(TColStd_HArray1OfReal) TabMaxV() const;




protected:





private:



  Handle(PLib_JacobiPolynomial) myJacPolU;
  Handle(PLib_JacobiPolynomial) myJacPolV;
  Handle(TColStd_HArray1OfReal) myTabMaxU;
  Handle(TColStd_HArray1OfReal) myTabMaxV;


};


#include <PLib_DoubleJacobiPolynomial.lxx>





#endif // _PLib_DoubleJacobiPolynomial_HeaderFile
