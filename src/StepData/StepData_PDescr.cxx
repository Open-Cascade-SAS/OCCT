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


#include <Interface_Check.hxx>
#include <Standard_Type.hxx>
#include <StepData_EDescr.hxx>
#include <StepData_Field.hxx>
#include <StepData_PDescr.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_PDescr,Standard_Transient)

#define KindInteger 1
#define KindBoolean 2
#define KindLogical 3
#define KindEnum    4
#define KindReal    5
#define KindString  6
#define KindEntity  7

StepData_PDescr::StepData_PDescr  ()
    : thesel (0), thekind (0), thearit (0),
      theopt (Standard_False), theder (Standard_False), thefnum (0)
      {  }

    void  StepData_PDescr::SetName (const Standard_CString name)
      {  thename.Clear();  thename.AssignCat (name);  }

    Standard_CString  StepData_PDescr::Name () const
      {  return thename.ToCString();  }

    Standard_Integer  StepData_PDescr::Kind () const
      {  return thekind;  }

    void  StepData_PDescr::SetSelect ()
      {  thesel = 4;  }

    void  StepData_PDescr::AddMember (const Handle(StepData_PDescr)& member)
{
  if (member.IsNull()) return;
  if (thenext.IsNull()) thenext = member;
  else thenext->AddMember (member);
  if (thesel == 3) return;
  if (thekind < KindEntity && thenext->Kind() >= KindEntity) thesel = 3;
  else if (thekind <  KindEntity && (thesel == 2 || thesel == 4)) thesel = 3;
  else if (thekind >= KindEntity && (thesel == 1 || thesel == 4)) thesel = 2;
}

    void  StepData_PDescr::SetMemberName (const Standard_CString memname)
      {  thesnam.Clear();  thesnam.AssignCat (memname);  }

    void  StepData_PDescr::SetInteger ()
      {  thekind = KindInteger;  }

    void  StepData_PDescr::SetReal ()
      {  thekind = KindReal;  }

    void  StepData_PDescr::SetString ()
      {  thekind = KindString;  }

    void  StepData_PDescr::SetBoolean ()
      {  thekind = KindBoolean;  }

    void  StepData_PDescr::SetLogical ()
      {  thekind = KindLogical;  }

    void  StepData_PDescr::SetEnum ()
      {  thekind = KindEnum;  }

    void  StepData_PDescr::AddEnumDef (const Standard_CString enumdef)
      {  theenum.AddDefinition (enumdef);  }

    void  StepData_PDescr::SetType  (const Handle(Standard_Type)& atype)
      {  thekind = KindEntity;  thetype = atype;  thednam.Clear();  }

    void  StepData_PDescr::SetDescr (const Standard_CString dscnam)
      {  thekind = KindEntity;  thetype.Nullify();
	 thednam.Clear();  thednam.AssignCat(dscnam);  }

    void  StepData_PDescr::AddArity (const Standard_Integer arity)
      {  thearit += arity;  }

    void  StepData_PDescr::SetArity (const Standard_Integer arity)
      {  thearit = arity;  }


    void  StepData_PDescr::SetFrom  (const Handle(StepData_PDescr)& other)
{
  if (other.IsNull()) return;
  thekind = other->Kind();
  Standard_Integer i, maxenum = other->EnumMax ();
  for (i = 0; i <= maxenum; i ++)    AddEnumDef (other->EnumText(i));
//  ne sont pas reprises : les SELECT
  thetype = other->Type();
  thearit = other->Arity();
  thefrom = other;
  theopt  = other->IsOptional ();
  theder  = other->IsDerived  ();
  thefnam.Clear();  thefnam.AssignCat (other->FieldName());
  thefnum = other->FieldRank ();
}

    void  StepData_PDescr::SetOptional (const Standard_Boolean opt)
      {  theopt = opt;  }

    void  StepData_PDescr::SetDerived  (const Standard_Boolean der)
      {  theder = der;  }

    void  StepData_PDescr::SetField
  (const Standard_CString name, const Standard_Integer rank)
{
  thefnam.Clear();  thefnam.AssignCat (name);
  thefnum = rank;
}

//    ######  INTERRO  ######

    Standard_Boolean  StepData_PDescr::IsSelect  () const
{
  if (!thefrom.IsNull()) return thefrom->IsSelect();
  return (thesel > 0);
}

    Handle(StepData_PDescr)  StepData_PDescr::Member (const Standard_CString name) const
{
  if (!thefrom.IsNull()) return thefrom->Member (name);
  Handle(StepData_PDescr) descr;
  if (thesnam.IsEqual (name)) return this;
  if (thenext.IsNull()) return descr;  // null
  return thenext->Member (name);
}

    Standard_Boolean  StepData_PDescr::IsInteger () const
      {  return (thekind == KindInteger);  }

    Standard_Boolean  StepData_PDescr::IsReal    () const
      {  return (thekind == KindReal);     }

    Standard_Boolean  StepData_PDescr::IsString  () const
      {  return (thekind == KindString);   }

    Standard_Boolean  StepData_PDescr::IsBoolean () const
      {  return (thekind == KindBoolean || thekind == KindLogical);  }

    Standard_Boolean  StepData_PDescr::IsLogical () const
      {  return (thekind == KindLogical);  }

    Standard_Boolean  StepData_PDescr::IsEnum    () const
      {  return (thekind == KindEnum);     }

    Standard_Integer  StepData_PDescr::EnumMax   () const
      {  return theenum.MaxValue();  }

    Standard_Integer  StepData_PDescr::EnumValue (const Standard_CString name) const
      {  return theenum.Value (name);  }

    Standard_CString  StepData_PDescr::EnumText  (const Standard_Integer val)  const
      {  return theenum.Text  (val).ToCString();   }

    Standard_Boolean  StepData_PDescr::IsEntity  () const
      {  return (thekind == KindEntity);  }

    Standard_Boolean  StepData_PDescr::IsType    (const Handle(Standard_Type)& atype) const
{
  if (atype.IsNull()) return Standard_False;
  if (!thetype.IsNull()) {
    if (atype->SubType(thetype)) return Standard_True;
  }
  if (!thenext.IsNull()) return thenext->IsType(atype);
  if (!thefrom.IsNull()) return thefrom->IsType(atype);
  return Standard_False;
}

    Handle(Standard_Type)  StepData_PDescr::Type () const
      {  return thetype;  }

    Standard_Boolean  StepData_PDescr::IsDescr
  (const Handle(StepData_EDescr)& descr) const
{
  if (descr.IsNull()) return Standard_False;
  if (thednam.Length() > 0) {
    if (descr->Matches (thednam.ToCString())) return Standard_True;
  }
  if (!thenext.IsNull()) return thenext->IsDescr (descr);
  if (!thefrom.IsNull()) return thefrom->IsDescr (descr);
  return Standard_False;
}

    Standard_CString  StepData_PDescr::DescrName () const
      {  return thednam.ToCString();  }


    Standard_Integer  StepData_PDescr::Arity () const
      {  return thearit;  }

    Handle(StepData_PDescr)  StepData_PDescr::Simple () const
{
  if (thearit == 0) return this;
  if (thefrom.IsNull()) return this;
  return thefrom;
}

    Standard_Boolean  StepData_PDescr::IsOptional () const
      {  return theopt;  }

    Standard_Boolean  StepData_PDescr::IsDerived  () const
      {  return theder;  }

    Standard_Boolean  StepData_PDescr::IsField    () const
      {  return (thefnum > 0);  }

    Standard_CString  StepData_PDescr::FieldName  () const
      {  return thefnam.ToCString ();  }

    Standard_Integer  StepData_PDescr::FieldRank  () const
      {  return thefnum;  }


void StepData_PDescr::Check(const StepData_Field& /*afild*/,
                            Handle(Interface_Check)& /*ach*/) const
{
//  pour l instant ...
}
