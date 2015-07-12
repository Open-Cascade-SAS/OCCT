// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_SymmetricTensor22d_HeaderFile
#define _StepFEA_SymmetricTensor22d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepData_SelectType.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
class Standard_Transient;


//! Representation of STEP SELECT type SymmetricTensor22d
class StepFEA_SymmetricTensor22d  : public StepData_SelectType
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT StepFEA_SymmetricTensor22d();
  
  //! Recognizes a kind of SymmetricTensor22d select type
  //! 1 -> HArray1OfReal from TColStd
  //! 0 else
  Standard_EXPORT Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const;
  
  //! Returns Value as AnisotropicSymmetricTensor22d (or Null if another type)
  Standard_EXPORT Handle(TColStd_HArray1OfReal) AnisotropicSymmetricTensor22d() const;




protected:





private:





};







#endif // _StepFEA_SymmetricTensor22d_HeaderFile
