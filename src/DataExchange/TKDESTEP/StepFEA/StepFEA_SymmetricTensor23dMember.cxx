// Created on: 2002-12-10
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <StepFEA_SymmetricTensor23dMember.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_SymmetricTensor23dMember, StepData_SelectArrReal)

static const char* IST = "ISOTROPIC_SYMMETRIC_TENSOR2_3D";
static const char* OST = "ORTHOTROPIC_SYMMETRIC_TENSOR2_3D";
static const char* AST = "ANISOTROPIC_SYMMETRIC_TENSOR2_3D";

//=================================================================================================

StepFEA_SymmetricTensor23dMember::StepFEA_SymmetricTensor23dMember()
    : mycase(0)
{
}

//=================================================================================================

bool StepFEA_SymmetricTensor23dMember::HasName() const
{
  return mycase > 0;
}

//=================================================================================================

const char* StepFEA_SymmetricTensor23dMember::Name() const
{
  switch (mycase)
  {
    case 1:
      return IST;
    case 2:
      return OST;
    case 3:
      return AST;
    default:
      break;
  }
  return "";
}

//=================================================================================================

static int CompareNames(const char* const name)
{
  int thecase = 0;
  if (!name || name[0] == '\0')
    thecase = 0;
  else if (!strcmp(name, IST))
    thecase = 1;
  else if (!strcmp(name, OST))
    thecase = 2;
  else if (!strcmp(name, AST))
    thecase = 3;
  return thecase;
}

//=================================================================================================

bool StepFEA_SymmetricTensor23dMember::SetName(const char* const name)
{
  mycase = CompareNames(name);
  return (mycase > 0);
}

//=================================================================================================

bool StepFEA_SymmetricTensor23dMember::Matches(const char* const name) const
{
  int thecase = CompareNames(name);
  return (mycase == thecase);
}
