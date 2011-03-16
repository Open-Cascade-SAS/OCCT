#include <IFSelect_SelectType.ixx>


    IFSelect_SelectType::IFSelect_SelectType ()
      {  thetype = STANDARD_TYPE(Standard_Transient);  }

    IFSelect_SelectType::IFSelect_SelectType (const Handle(Standard_Type)& atype)
      {  thetype = atype;  }

    void  IFSelect_SelectType::SetType (const Handle(Standard_Type)& atype)
      {  thetype = atype;  }

    Handle(Standard_Type)  IFSelect_SelectType::TypeForMatch () const
      {  return thetype;  }

    TCollection_AsciiString  IFSelect_SelectType::ExtractLabel () const 
      {  return TCollection_AsciiString(thetype->Name());  }
