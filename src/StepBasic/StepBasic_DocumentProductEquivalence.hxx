// Created on: 2003-01-28
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepBasic_DocumentProductEquivalence_HeaderFile
#define _StepBasic_DocumentProductEquivalence_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_DocumentProductAssociation.hxx>


class StepBasic_DocumentProductEquivalence;
DEFINE_STANDARD_HANDLE(StepBasic_DocumentProductEquivalence, StepBasic_DocumentProductAssociation)

//! Representation of STEP entity DocumentProductEquivalence
class StepBasic_DocumentProductEquivalence : public StepBasic_DocumentProductAssociation
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepBasic_DocumentProductEquivalence();




  DEFINE_STANDARD_RTTIEXT(StepBasic_DocumentProductEquivalence,StepBasic_DocumentProductAssociation)

protected:




private:




};







#endif // _StepBasic_DocumentProductEquivalence_HeaderFile
