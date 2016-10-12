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
#include <Standard_Type.hxx>
#include <StepData_Described.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_Simple.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepData_ESDescr,StepData_EDescr)

StepData_ESDescr::StepData_ESDescr  (const Standard_CString name)
:  thenom (name)    { }

void  StepData_ESDescr::SetNbFields (const Standard_Integer nb)
{
  Standard_Integer minb,i, oldnb = NbFields();
  thenames.Clear();
  if (nb == 0)  {  thedescr.Nullify();  return;  }
  Handle(TColStd_HArray1OfTransient) li = new TColStd_HArray1OfTransient(1,nb);
  if (oldnb == 0)  {  thedescr = li;  return;  }
  minb = (oldnb > nb ? nb : oldnb);
  for (i = 1; i <= minb; i ++)  {
    DeclareAndCast(StepData_PDescr,pde,thedescr->Value(i));
    if (!pde.IsNull())  thenames.Bind(pde->Name(),i);
    li->SetValue (i, pde);
  }
  thedescr = li;
}


    void  StepData_ESDescr::SetField
  (const Standard_Integer num, const Standard_CString name,
   const Handle(StepData_PDescr)& descr)
{
  if (num < 1 || num > NbFields()) return;
  Handle(StepData_PDescr) pde = new StepData_PDescr;
  pde->SetFrom (descr);
  pde->SetName (name);
  thedescr->SetValue (num,pde);
  thenames.Bind(name,num);
}

    void  StepData_ESDescr::SetBase (const Handle(StepData_ESDescr)& base)
{
  thebase = base;
//  il faut CUMULER les fields de la base et ses supers
}

    void  StepData_ESDescr::SetSuper (const Handle(StepData_ESDescr)& super)
{
  Handle(StepData_ESDescr) sup = super->Base();
  if (sup.IsNull()) sup = super;
  if (!thebase.IsNull()) thebase->SetSuper (sup);
  else thesuper = sup;
}

    Standard_CString  StepData_ESDescr::TypeName () const
      {  return thenom.ToCString();  }

    const TCollection_AsciiString&  StepData_ESDescr::StepType () const
      {  return thenom;  }

    Handle(StepData_ESDescr)  StepData_ESDescr::Base () const
      {  return thebase;  }

    Handle(StepData_ESDescr)  StepData_ESDescr::Super () const
      {  return thesuper;  }

    Standard_Boolean  StepData_ESDescr::IsSub (const Handle(StepData_ESDescr)& other) const
{
  Handle(StepData_ESDescr) oth = other->Base();
  if (oth.IsNull()) oth = other;
  if (!thebase.IsNull()) return thebase->IsSub (oth);
  Handle(Standard_Transient) t1 = this;
  if (oth == t1)       return Standard_True;
  if (oth == thesuper) return Standard_True;
  else if (thesuper.IsNull()) return Standard_False;
  return thesuper->IsSub (oth);
}


    Standard_Integer  StepData_ESDescr::NbFields () const
      {  return (thedescr.IsNull() ? 0 : thedescr->Length());  }

    Standard_Integer  StepData_ESDescr::Rank (const Standard_CString name) const
{
  Standard_Integer rank;
  if (!thenames.Find(name, rank))
    return 0;
  return rank;
}

    Standard_CString  StepData_ESDescr::Name (const Standard_Integer num) const
{
  if (num < 1) return "";
  if (num > NbFields()) return "";
  DeclareAndCast(StepData_PDescr,pde,thedescr->Value(num));
  return pde->Name();
}

    Handle(StepData_PDescr)  StepData_ESDescr::Field (const Standard_Integer num) const
     {  return GetCasted(StepData_PDescr,thedescr->Value(num));  }

    Handle(StepData_PDescr)  StepData_ESDescr::NamedField
  (const Standard_CString name) const
{
  Handle(StepData_PDescr) pde;
  Standard_Integer rank = Rank(name);
  if (rank > 0) pde = GetCasted(StepData_PDescr,thedescr->Value(rank));
  return pde;
}


    Standard_Boolean  StepData_ESDescr::Matches (const Standard_CString name) const
{
  if (thenom.IsEqual(name)) return Standard_True;
  if (thesuper.IsNull()) return Standard_False;
  return thesuper->Matches (name);
}

    Standard_Boolean  StepData_ESDescr::IsComplex () const
      {  return Standard_False;  }

    Handle(StepData_Described)  StepData_ESDescr::NewEntity () const
{
  Handle(StepData_Simple) ent = new StepData_Simple(this);
  return ent;
}
