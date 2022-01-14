// Created on: 2005-12-19
// Created by: Julia GERASIMOVA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _math_ComputeGaussPointsAndWeights_HeaderFile
#define _math_ComputeGaussPointsAndWeights_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>



class math_ComputeGaussPointsAndWeights 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT math_ComputeGaussPointsAndWeights(const Standard_Integer Number);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT math_Vector Points() const;
  
  Standard_EXPORT math_Vector Weights() const;




protected:





private:



  Handle(TColStd_HArray1OfReal) myPoints;
  Handle(TColStd_HArray1OfReal) myWeights;
  Standard_Boolean myIsDone;


};







#endif // _math_ComputeGaussPointsAndWeights_HeaderFile
