#include <StepBasic_SizeMember.ixx>

    StepBasic_SizeMember::StepBasic_SizeMember  ()    {  }

    Standard_Boolean  StepBasic_SizeMember::HasName () const
      {  return Standard_True;  }

    Standard_CString  StepBasic_SizeMember::Name () const
      {  return "POSITIVE_LENGTH_MEASURE";  }

    Standard_Boolean  StepBasic_SizeMember::SetName (const Standard_CString name)
      {  return Standard_True;  }
