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

#ifndef _AdvApprox_DichoCutting_HeaderFile
#define _AdvApprox_DichoCutting_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AdvApprox_Cutting.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>



//! if Cutting is necessary in [a,b], we cut at (a+b) / 2.
class AdvApprox_DichoCutting  : public AdvApprox_Cutting
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT AdvApprox_DichoCutting();
  
  Standard_EXPORT virtual Standard_Boolean Value (const Standard_Real a, const Standard_Real b, Standard_Real& cuttingvalue) const Standard_OVERRIDE;




protected:





private:





};







#endif // _AdvApprox_DichoCutting_HeaderFile
