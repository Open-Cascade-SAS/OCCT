#include <StepFEA_SymmetricTensor43dMember.ixx>
#include <TCollection_HAsciiString.hxx>

static Standard_CString AST = "ANISOTROPIC_SYMMETRIC_TENSOR4_3D";
static Standard_CString IST = "FEA_ISOTROPIC_SYMMETRIC_TENSOR4_3D";
static Standard_CString IOST = "FEA_ISO_ORTHOTROPIC_SYMMETRIC_TENSOR4_3D";
static Standard_CString TIST = "FEA_TRANSVERSE_ISOTROPIC_SYMMETRIC_TENSOR4_3D";
static Standard_CString CNOST = "FEA_COLUMN_NORMALISED_ORTHOTROPIC_SYMMETRIC_TENSOR4_3D";
static Standard_CString CNMST = "FEA_COLUMN_NORMALISED_MONOCLINIC_SYMMETRIC_TENSOR4_3D";


//=======================================================================
//function : StepFEA_SymmetricTensor43dMember
//purpose  : 
//=======================================================================

StepFEA_SymmetricTensor43dMember::StepFEA_SymmetricTensor43dMember () : mycase(0) 
{
}


//=======================================================================
//function : HasName
//purpose  : 
//=======================================================================

Standard_Boolean StepFEA_SymmetricTensor43dMember::HasName() const
{
  return mycase>0;
}


//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Standard_CString StepFEA_SymmetricTensor43dMember::Name() const
{
  switch(mycase)  {
    case 1  : return AST;
    case 2  : return IST;
    case 3  : return IOST;
    case 4  : return TIST;
    case 5  : return CNOST;
    case 6  : return CNMST;
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
  Standard_Integer thecase =0;
  if (!name || name[0] == '\0') thecase = 0;
  else if(!strcmp (name,AST)) thecase = 1;
  else if(!strcmp (name,IST)) thecase = 2;
  else if(!strcmp (name,IOST)) thecase = 3;
  else if(!strcmp (name,TIST)) thecase = 4;
  else if(!strcmp (name,CNOST)) thecase = 5;
  else if(!strcmp (name,CNMST)) thecase = 6;
  return thecase;
}


//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

Standard_Boolean StepFEA_SymmetricTensor43dMember::SetName(const Standard_CString name) 
{
  mycase = CompareNames(name);
  return (mycase >0);
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean StepFEA_SymmetricTensor43dMember::Matches(const Standard_CString name) const
{
  Standard_Integer thecase = CompareNames(name);
  return (mycase==thecase);
}
