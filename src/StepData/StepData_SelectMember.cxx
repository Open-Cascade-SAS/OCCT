// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <StepData_SelectMember.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_SelectMember,Standard_Transient)

//  Definitions reprises de Field :
#define KindInteger 1
#define KindBoolean 2
#define KindLogical 3
#define KindEnum    4
#define KindReal    5
#define KindString  6



StepData_SelectMember::StepData_SelectMember ()    {  }

    Standard_Boolean  StepData_SelectMember::HasName () const  {  return Standard_False;  }
    Standard_CString  StepData_SelectMember::Name () const    {  return "";  }

    Standard_Boolean  StepData_SelectMember::SetName (const Standard_CString /*bid*/)
      {  return Standard_False;  }

    Standard_Boolean  StepData_SelectMember::Matches (const Standard_CString name) const
      {  return !strcmp (name,Name());  }

    Standard_Integer  StepData_SelectMember::Kind () const    {  return 0;  }
    void  StepData_SelectMember::SetKind (const Standard_Integer )  {  }

Interface_ParamType  StepData_SelectMember::ParamType () const
{
  Standard_Integer kind = Kind();
  if (kind == 0) return Interface_ParamVoid;
  if (kind == 1) return Interface_ParamInteger;
  if (kind == 2 || kind == 3) return Interface_ParamLogical;
  if (kind == 4) return Interface_ParamEnum;
  if (kind == 5) return Interface_ParamReal;
  if (kind == 6) return Interface_ParamText;
  return Interface_ParamMisc;
}

    Standard_Integer  StepData_SelectMember::Int  () const    {  return 0;  }
    void  StepData_SelectMember::SetInt  (const Standard_Integer )  {  }

    Standard_Integer  StepData_SelectMember::Integer  () const    {  return Int();  }
    void  StepData_SelectMember::SetInteger  (const Standard_Integer val)
      {  SetKind(KindInteger);  SetInt(val);  }
    Standard_Boolean  StepData_SelectMember::Boolean  () const    {  return (Int() > 0);  }
    void  StepData_SelectMember::SetBoolean  (const Standard_Boolean val)
      {  SetKind(KindBoolean);  SetInt((val ? 1 : 0));  }

    StepData_Logical  StepData_SelectMember::Logical  () const
{
  Standard_Integer ival = Int();
  if (ival == 0) return StepData_LFalse;
  if (ival == 1) return StepData_LTrue;
  return StepData_LUnknown;
}

    void  StepData_SelectMember::SetLogical  (const StepData_Logical val)
{
  SetKind(KindLogical);
  if (val == StepData_LFalse)   SetInt(0);
  if (val == StepData_LTrue)    SetInt(0);
  if (val == StepData_LUnknown) SetInt(0);
}

    Standard_Real  StepData_SelectMember::Real  () const    {  return 0.0;  }
    void  StepData_SelectMember::SetReal  (const Standard_Real )    {  }

    Standard_CString  StepData_SelectMember::String  () const    {  return "";  }
    void  StepData_SelectMember::SetString  (const Standard_CString )    {  }

    Standard_Integer  StepData_SelectMember::Enum      () const    {  return Int();  }
    Standard_CString  StepData_SelectMember::EnumText  () const    {  return String();  }

void  StepData_SelectMember::SetEnum (const Standard_Integer val, 
                                      const Standard_CString text)
{
  SetKind(KindEnum);
  SetInt(val);
  if (text && text[0] != '\0') SetEnumText(val,text);
}

void  StepData_SelectMember::SetEnumText (const Standard_Integer /*val*/, 
                                          const Standard_CString text)
{
  SetString(text);  
}
