#include <StepVisual_MarkerMember.ixx>
#include <StepData_EnumTool.hxx>
#include <TCollection_AsciiString.hxx>

static StepData_EnumTool tool
  (".DOT.",".X.",".PLUS.",".ASTERISK.",".RING.",".SQUARE.",".TRIANGLE.");

    StepVisual_MarkerMember::StepVisual_MarkerMember ()   {  }

    Standard_Boolean  StepVisual_MarkerMember::HasName () const
      {  return Standard_True;  }

    Standard_CString  StepVisual_MarkerMember::Name    () const
      {  return "MARKER_TYPE";  }

    Standard_Boolean  StepVisual_MarkerMember::SetName (const Standard_CString name)
      {  return Standard_True;  }

    Standard_CString  StepVisual_MarkerMember::EnumText () const
      {  return tool.Text(Int()).ToCString();  }

    void  StepVisual_MarkerMember::SetEnumText
  (const Standard_Integer val, const Standard_CString text)
{
  Standard_Integer vl = tool.Value (text);
  if (vl >= 0) SetInt (vl);
}

    void  StepVisual_MarkerMember::SetValue (const StepVisual_MarkerType val)
      {  SetInt ( Standard_Integer (val) );  }

    StepVisual_MarkerType  StepVisual_MarkerMember::Value () const
      {  return StepVisual_MarkerType (Int());  }
