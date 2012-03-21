// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepData_SelectType.ixx>
#include <Standard_TypeMismatch.hxx>
#include <StepData_SelectNamed.hxx>
#include <StepData_SelectReal.hxx>
#include <StepData_SelectInt.hxx>
#include <Interface_Macros.hxx>



Standard_Boolean  StepData_SelectType::Matches
  (const Handle(Standard_Transient)& ent) const
{
  if (CaseNum(ent) > 0) return Standard_True;
  DeclareAndCast(StepData_SelectMember,sm,ent);
  if (sm.IsNull()) return Standard_False;
  if (CaseMem(sm)  > 0) return Standard_True;
  return Standard_False;
}

    void  StepData_SelectType::SetValue (const Handle(Standard_Transient)& ent)
{
  if (ent.IsNull())  thevalue.Nullify();
  else if (!Matches(ent))
    Standard_TypeMismatch::Raise ("StepData : SelectType, SetValue");
  else thevalue = ent;
}

    void  StepData_SelectType::Nullify ()
      {  thevalue.Nullify();  }

    const Handle(Standard_Transient)&  StepData_SelectType::Value () const
      {  return thevalue;  }

    Standard_Boolean  StepData_SelectType::IsNull () const
      {  return thevalue.IsNull();  }

    Handle(Standard_Type) StepData_SelectType::Type () const
{
  if (thevalue.IsNull()) return STANDARD_TYPE(Standard_Transient);
  return thevalue->DynamicType();
}

    Standard_Integer  StepData_SelectType::CaseNumber () const
      {  if (thevalue.IsNull()) return 0;  return CaseNum(thevalue);  }


//  **********   Types Immediats   ***********

    Handle(StepData_PDescr)  StepData_SelectType::Description () const
      {  Handle(StepData_PDescr) nuldescr;  return nuldescr;  }

    Handle(StepData_SelectMember)   StepData_SelectType::NewMember () const
      {  Handle(StepData_SelectMember) nulmem;  return nulmem;  }

    Standard_Integer  StepData_SelectType::CaseMem (const Handle(StepData_SelectMember)& ent) const
      {  return 0;  }

    Standard_Integer  StepData_SelectType::CaseMember () const
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (sm.IsNull()) return 0;
  return CaseMem (sm);
}

    Handle(StepData_SelectMember)  StepData_SelectType::Member () const
      {  return GetCasted(StepData_SelectMember,thevalue);  }

    Standard_CString  StepData_SelectType::SelectName () const
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (sm.IsNull()) return "";
  return sm->Name();
}

    Standard_Integer  StepData_SelectType::Int () const
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (sm.IsNull()) return 0;
  return sm->Int();
}

    void  StepData_SelectType::SetInt (const Standard_Integer val)
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (sm.IsNull()) Standard_TypeMismatch::Raise ("StepData : SelectType, SetInt");
  sm->SetInt (val);
}

//  **********   Types Immediats : Differents Cas  ***********

static Handle(StepData_SelectMember) SelectVal
  (const Handle(Standard_Transient)& thevalue, const Standard_CString name,
   const int mode)
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (!sm.IsNull()) {
    if (name && name[0] != '\0')
      if (!sm->SetName(name)) Standard_TypeMismatch::Raise ("StepData : SelectType, SetInteger");
  }
  else if (name && name[0] != '\0') {
    Handle(StepData_SelectNamed) sn = new StepData_SelectNamed;
    sn->SetName (name);
    sm = sn;
  } else {
    if (mode == 0) sm = new StepData_SelectInt;
    if (mode == 1) sm = new StepData_SelectReal;
  }
  return sm;
}


    Standard_Integer  StepData_SelectType::Integer () const
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (sm.IsNull()) return 0;
  return sm->Integer();
}

    void  StepData_SelectType::SetInteger
  (const Standard_Integer val, const Standard_CString name)
{
  Handle(StepData_SelectMember) sm = SelectVal (thevalue,name,0);
  sm->SetInteger (val);
  if (CaseMem (sm) == 0) Standard_TypeMismatch::Raise ("StepData : SelectType, SetInteger");
  thevalue = sm;
}

    Standard_Boolean  StepData_SelectType::Boolean () const
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (sm.IsNull()) return Standard_False;
  return sm->Boolean();
}

    void  StepData_SelectType::SetBoolean
  (const Standard_Boolean val, const Standard_CString name)
{
  Handle(StepData_SelectMember) sm = SelectVal (thevalue,name,0);
  sm->SetBoolean (val);
  if (CaseMem (sm) == 0) Standard_TypeMismatch::Raise ("StepData : SelectType, SetBoolean");
  thevalue = sm;
}

    StepData_Logical  StepData_SelectType::Logical () const
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (sm.IsNull()) return StepData_LUnknown;
  return sm->Logical();
}

    void  StepData_SelectType::SetLogical
  (const StepData_Logical val, const Standard_CString name)
{
  Handle(StepData_SelectMember) sm = SelectVal (thevalue,name,0);
  sm->SetLogical (val);
  if (CaseMem (sm) == 0) Standard_TypeMismatch::Raise ("StepData : SelectType, SetLogical");
  thevalue = sm;
}

Standard_Real  StepData_SelectType::Real () const
{
  DeclareAndCast(StepData_SelectMember,sm,thevalue);
  if (sm.IsNull()) return 0.0;
  return sm->Real();
}

    void  StepData_SelectType::SetReal
  (const Standard_Real val, const Standard_CString name)
{
  Handle(StepData_SelectMember) sm = SelectVal (thevalue,name,1);
  sm->SetReal (val);
  if (CaseMem (sm) == 0) Standard_TypeMismatch::Raise ("StepData : SelectType, SetReal");
  thevalue = sm;
}

void StepData_SelectType::Destroy(){}
