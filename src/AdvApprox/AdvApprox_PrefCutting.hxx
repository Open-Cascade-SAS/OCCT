// Created on: 1996-04-05
// Created by: Joelle CHAUVET
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

#ifndef _AdvApprox_PrefCutting_HeaderFile
#define _AdvApprox_PrefCutting_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <AdvApprox_Cutting.hxx>
#include <Standard_Boolean.hxx>


//! inherits class Cutting; contains a list of preferential points (di)i
//! if Cutting is necessary in [a,b], we cut at the di nearest from (a+b)/2.
class AdvApprox_PrefCutting  : public AdvApprox_Cutting
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT AdvApprox_PrefCutting(const TColStd_Array1OfReal& CutPnts);
  
  Standard_EXPORT virtual Standard_Boolean Value (const Standard_Real a, const Standard_Real b, Standard_Real& cuttingvalue) const Standard_OVERRIDE;




protected:





private:



  TColStd_Array1OfReal myPntOfCutting;


};







#endif // _AdvApprox_PrefCutting_HeaderFile
