// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <StepFEA_SymmetricTensor23dMember.ixx>
#include <TCollection_HAsciiString.hxx>

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
