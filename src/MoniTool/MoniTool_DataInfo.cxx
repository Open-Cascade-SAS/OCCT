#include <MoniTool_DataInfo.ixx>

    Handle(Standard_Type)  MoniTool_DataInfo::Type
  (const Handle(Standard_Transient)& ent)
      {  return ent->DynamicType();  }

    Standard_CString  MoniTool_DataInfo::TypeName
  (const Handle(Standard_Transient)& ent)
      {  return ent->DynamicType()->Name();  }
