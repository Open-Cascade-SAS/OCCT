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


#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_TypeMismatch.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_SelectInt.hxx>
#include <StepData_SelectMember.hxx>
#include <StepData_SelectNamed.hxx>
#include <StepData_SelectReal.hxx>
#include <StepData_SelectType.hxx>
#include <StepData_UndefinedEntity.hxx>


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
  else if (ent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity)))
    thevalue = ent;
  else if (!Matches(ent))
    throw Standard_TypeMismatch("StepData : SelectType, SetValue");
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

    Standard_Integer  StepData_SelectType::CaseMem (const Handle(StepData_SelectMember)& /*ent*/) const
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
  if (sm.IsNull()) throw Standard_TypeMismatch("StepData : SelectType, SetInt");
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
      if (!sm->SetName(name)) throw Standard_TypeMismatch("StepData : SelectType, SetInteger");
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
  if (CaseMem (sm) == 0) throw Standard_TypeMismatch("StepData : SelectType, SetInteger");
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
  if (CaseMem (sm) == 0) throw Standard_TypeMismatch("StepData : SelectType, SetBoolean");
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
  if (CaseMem (sm) == 0) throw Standard_TypeMismatch("StepData : SelectType, SetLogical");
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
  if (CaseMem (sm) == 0) throw Standard_TypeMismatch("StepData : SelectType, SetReal");
  thevalue = sm;
}

StepData_SelectType::~StepData_SelectType()
{}
