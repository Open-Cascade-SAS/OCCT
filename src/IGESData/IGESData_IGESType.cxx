#include <IGESData_IGESType.ixx>



   IGESData_IGESType::IGESData_IGESType ()  {  thetype = 0; theform = 0;  }

   IGESData_IGESType::IGESData_IGESType
      (const Standard_Integer atype, const Standard_Integer aform)
      {  thetype = atype; theform = aform;  }

    Standard_Integer IGESData_IGESType::Type () const    {  return thetype;  }

    Standard_Integer IGESData_IGESType::Form () const    {  return theform;  }

    Standard_Boolean IGESData_IGESType::IsEqual (const IGESData_IGESType& other) const
      {  return (thetype == other.Type() && theform == other.Form());  }

    void IGESData_IGESType::Nullify ()      {  thetype = 0; theform = 0;  }
