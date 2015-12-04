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


#include <IFSelect_Signature.hxx>
#include <IFSelect_SignMultiple.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SignMultiple,IFSelect_Signature)

static TCollection_AsciiString theval;    // temporaire pour construire Value

    IFSelect_SignMultiple::IFSelect_SignMultiple (const Standard_CString name)
    : IFSelect_Signature (name)    {  }

    void  IFSelect_SignMultiple::Add
  (const Handle(IFSelect_Signature)& subsign,
   const Standard_Integer tabul, const Standard_Boolean maxi)
{
  if (subsign.IsNull()) return;
  thesubs.Append (subsign);
  thetabs.Append (maxi ? -tabul : tabul);
}

    Standard_CString  IFSelect_SignMultiple::Value
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  theval.Clear();
  Standard_Integer i, nb = thesubs.Length();
  for (i = 1; i <= nb; i ++) {
    Standard_Integer tabul = thetabs.Value(i);
    Standard_Boolean maxi  = (tabul < 0);
    if (maxi) tabul = -tabul;
    Handle(IFSelect_Signature) sign = Handle(IFSelect_Signature)::DownCast(thesubs.Value(i));
    Standard_CString val = sign->Value (ent,model);
    TCollection_AsciiString str(val);
    Standard_Integer sl = str.Length();
    str.LeftJustify (tabul,' ');
    if (sl > tabul && maxi) {
      str.Remove (sl+1,tabul-sl);
      str.SetValue (sl,'.');
    }
    str.AssignCat("   ");
    theval.AssignCat (str);
  }
  return theval.ToCString();
}


    Standard_Boolean  IFSelect_SignMultiple::Matches
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model,
   const TCollection_AsciiString& text, const Standard_Boolean exact) const
{
  if (exact) return IFSelect_Signature::Matches (ent,model,text,exact);
  Standard_Integer i, nb = thesubs.Length();
  for (i = 1; i <= nb; i ++) {
    Handle(IFSelect_Signature) sign =
      Handle(IFSelect_Signature)::DownCast(thesubs.Value(i));
    if (sign->Matches (ent,model,text,exact)) return Standard_True;
  }
  return Standard_False;
}
