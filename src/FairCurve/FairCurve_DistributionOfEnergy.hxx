// Created on: 1996-01-22
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

#ifndef _FairCurve_DistributionOfEnergy_HeaderFile
#define _FairCurve_DistributionOfEnergy_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <math_FunctionSet.hxx>


//! Abstract class to use the Energy of an FairCurve
class FairCurve_DistributionOfEnergy  : public math_FunctionSet
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns the number of variables of the function.
  Standard_EXPORT virtual Standard_Integer NbVariables() const Standard_OVERRIDE;
  
  //! returns the number of equations of the function.
  Standard_EXPORT virtual Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  Standard_EXPORT void SetDerivativeOrder (const Standard_Integer DerivativeOrder);




protected:

  
  Standard_EXPORT FairCurve_DistributionOfEnergy(const Standard_Integer BSplOrder, const Handle(TColStd_HArray1OfReal)& FlatKnots, const Handle(TColgp_HArray1OfPnt2d)& Poles, const Standard_Integer DerivativeOrder, const Standard_Integer NbValAux = 0);


  Standard_Integer MyBSplOrder;
  Handle(TColStd_HArray1OfReal) MyFlatKnots;
  Handle(TColgp_HArray1OfPnt2d) MyPoles;
  Standard_Integer MyDerivativeOrder;
  Standard_Integer MyNbVar;
  Standard_Integer MyNbEqua;
  Standard_Integer MyNbValAux;


private:





};







#endif // _FairCurve_DistributionOfEnergy_HeaderFile
