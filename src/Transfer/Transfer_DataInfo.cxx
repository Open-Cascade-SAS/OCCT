#include <Transfer_DataInfo.ixx>

    Handle(Standard_Type)  Transfer_DataInfo::Type
  (const Handle(Standard_Transient)& ent)
      {  return ent->DynamicType();  }

    Standard_CString  Transfer_DataInfo::TypeName
  (const Handle(Standard_Transient)& ent)
      {  return ent->DynamicType()->Name();  }
