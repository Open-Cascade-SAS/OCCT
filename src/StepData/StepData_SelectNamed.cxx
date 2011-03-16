#include <StepData_SelectNamed.ixx>

//  Definitions reprises de Field :
#define KindInteger 1
#define KindBoolean 2
#define KindLogical 3
#define KindEnum    4
#define KindReal    5
#define KindString  6

    StepData_SelectNamed::StepData_SelectNamed  ()
    {  theval.Clear();  thename.Clear();  }


    Standard_Boolean  StepData_SelectNamed::HasName () const
      {  return (thename.Length() > 0);  }

    Standard_CString  StepData_SelectNamed::Name () const
      {  return thename.ToCString();  }

    Standard_Boolean  StepData_SelectNamed::SetName (const Standard_CString name)
      {  thename.Clear();  thename.AssignCat (name);  return Standard_True;  }

    const StepData_Field&  StepData_SelectNamed::Field () const
      {  return theval;  }

    StepData_Field&  StepData_SelectNamed::CField ()
      {  return theval;  }


    Standard_Integer  StepData_SelectNamed::Kind () const
      {  return theval.Kind();  }

    void  StepData_SelectNamed::SetKind  (const Standard_Integer kind)
      {  theval.Clear (kind);  }

    Standard_Integer  StepData_SelectNamed::Int  () const
      {  return theval.Int();  }

    void  StepData_SelectNamed::SetInt (const Standard_Integer val)
      {  theval.SetInt (val);  }

    Standard_Real  StepData_SelectNamed::Real () const
      {  return theval.Real();  }

    void  StepData_SelectNamed::SetReal (const Standard_Real val)
      {  theval.SetReal (val);  }

    Standard_CString  StepData_SelectNamed::String  () const
      {  return theval.String();  }

    void  StepData_SelectNamed::SetString (const Standard_CString val)
      {  theval.SetString (val);  }
