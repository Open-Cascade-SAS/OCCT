#include <StepGeom_TrimmingMember.ixx>

    StepGeom_TrimmingMember::StepGeom_TrimmingMember  ()    {  }

    Standard_Boolean  StepGeom_TrimmingMember::HasName () const
      {  return Standard_True;  }

    Standard_CString  StepGeom_TrimmingMember::Name () const
      {  return "PARAMETER_VALUE";  }

    Standard_Boolean  StepGeom_TrimmingMember::SetName (const Standard_CString name)
      {  return Standard_True;  }
