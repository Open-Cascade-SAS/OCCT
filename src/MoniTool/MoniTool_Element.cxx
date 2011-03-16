#include <MoniTool_Element.ixx>


    void  MoniTool_Element::SetHashCode (const Standard_Integer code)
      {  thecode = code;  }

    Standard_Integer  MoniTool_Element::GetHashCode () const
      {  return thecode;  }

    Handle(Standard_Type)  MoniTool_Element::ValueType () const
      {  return DynamicType();  }

    Standard_CString  MoniTool_Element::ValueTypeName () const
      {  return "(finder)";  }


//  ####    ATTRIBUTES    ####


    const MoniTool_AttrList&  MoniTool_Element::ListAttr () const
      {  return theattrib;  }

    MoniTool_AttrList&  MoniTool_Element::ChangeAttr ()
      {  return theattrib;  }
