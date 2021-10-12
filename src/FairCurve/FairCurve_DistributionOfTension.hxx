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

#ifndef _FairCurve_DistributionOfTension_HeaderFile
#define _FairCurve_DistributionOfTension_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <FairCurve_BattenLaw.hxx>
#include <FairCurve_DistributionOfEnergy.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>


//! Compute the Tension Distribution
class FairCurve_DistributionOfTension  : public FairCurve_DistributionOfEnergy
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT FairCurve_DistributionOfTension(const Standard_Integer BSplOrder, const Handle(TColStd_HArray1OfReal)& FlatKnots, const Handle(TColgp_HArray1OfPnt2d)& Poles, const Standard_Integer DerivativeOrder, const Standard_Real LengthSliding, const FairCurve_BattenLaw& Law, const Standard_Integer NbValAux = 0, const Standard_Boolean Uniform = Standard_False);
  
  //! change the length sliding
    void SetLengthSliding (const Standard_Real LengthSliding);
  
  //! computes the values <F> of the functions for the
  //! variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const math_Vector& X, math_Vector& F) Standard_OVERRIDE;




protected:





private:



  Standard_Real MyLengthSliding;
  FairCurve_BattenLaw MyLaw;
  Standard_Real MyHeight;


};


#include <FairCurve_DistributionOfTension.lxx>





#endif // _FairCurve_DistributionOfTension_HeaderFile
