// File:	StepFEA_SymmetricTensor23dMember.cxx
// Created:	Tue Dec 10 18:13:00 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright:	Open CASCADE 2002

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
