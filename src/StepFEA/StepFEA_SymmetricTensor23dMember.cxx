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

#include <Standard_Type.hxx>
#include <StepFEA_SymmetricTensor23dMember.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_SymmetricTensor23dMember,StepData_SelectArrReal)

static Standard_CString IST = "ISOTROPIC_SYMMETRIC_TENSOR2_3D";
static Standard_CString OST = "ORTHOTROPIC_SYMMETRIC_TENSOR2_3D";
static Standard_CString AST = "ANISOTROPIC_SYMMETRIC_TENSOR2_3D";

//=======================================================================
//function : StepFEA_SymmetricTensor23dMember
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor23dMember::StepFEA_SymmetricTensor23dMember () : mycase(0) 
{
}

//=======================================================================
//function : HasName
//purpose  : 
//=======================================================================

Standard_Boolean StepFEA_SymmetricTensor23dMember::HasName() const
{
 return mycase >0;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString StepFEA_SymmetricTensor23dMember::Name() const
{
  switch(mycase)  {
    case 1  : return IST;
    case 2  : return OST;
    case 3  : return AST;
    default : break;
  }
  return "";
}

//=======================================================================
//function : CompareNames
//purpose  : 
//=======================================================================

static Standard_Integer CompareNames(const Standard_CString name)
{
  Standard_Integer thecase = 0;
  if (!name || name[0] == '\0') thecase = 0;
   else if(!strcmp (name,IST)) thecase = 1;
   else if(!strcmp (name,OST)) thecase = 2;
   else if(!strcmp (name,AST)) thecase = 3;
  return thecase;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

Standard_Boolean StepFEA_SymmetricTensor23dMember::SetName(const Standard_CString name) 
{
  mycase = CompareNames(name);
  return (mycase >0);
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean StepFEA_SymmetricTensor23dMember::Matches(const Standard_CString name) const
{
  Standard_Integer thecase = CompareNames(name);
  return (mycase==thecase);
}
