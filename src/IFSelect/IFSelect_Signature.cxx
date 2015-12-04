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
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_Signature,Interface_SignType)

// unused 
//static Standard_CString nulsign = "";
static char intval[20];


    IFSelect_Signature::IFSelect_Signature (const Standard_CString name)
    : thename (name)     {  thecasi[0] = thecasi[1] = thecasi[2] = 0;  }

    void  IFSelect_Signature::SetIntCase
  (const Standard_Boolean hasmin, const Standard_Integer valmin,
   const Standard_Boolean hasmax, const Standard_Integer valmax)
{
  thecasi[0] = 1;
  if (hasmin) {  thecasi[0] += 2;  thecasi[1] = valmin;  }
  if (hasmax) {  thecasi[0] += 4;  thecasi[2] = valmax;  }
}

    Standard_Boolean  IFSelect_Signature::IsIntCase
  (Standard_Boolean& hasmin, Standard_Integer& valmin,
   Standard_Boolean& hasmax, Standard_Integer& valmax) const
{
  hasmin = hasmax = Standard_False;
  valmin = valmax = 0;
  if (!thecasi[0]) return Standard_False;
  if (thecasi[0] & 2)  {  hasmin = Standard_True;  valmin = thecasi[1];  }
  if (thecasi[0] & 4)  {  hasmax = Standard_True;  valmax = thecasi[2];  }
  return Standard_True;
}

    void  IFSelect_Signature::AddCase (const Standard_CString acase)
{
  if (thecasl.IsNull()) thecasl = new TColStd_HSequenceOfAsciiString();
  TCollection_AsciiString scase(acase);
  thecasl->Append(scase);
}

  Handle(TColStd_HSequenceOfAsciiString) IFSelect_Signature::CaseList () const
      {  return thecasl;  }


    Standard_CString IFSelect_Signature::Name () const
      { return thename.ToCString();  }

    TCollection_AsciiString  IFSelect_Signature::Label () const
{
  TCollection_AsciiString label("Signature : ");
  label.AssignCat(thename);
  return label;
}


    Standard_Boolean  IFSelect_Signature::Matches
  (const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model,
   const TCollection_AsciiString& text, const Standard_Boolean exact) const

{  return IFSelect_Signature::MatchValue ( Value(ent,model) , text, exact);  }


    Standard_Boolean  IFSelect_Signature::MatchValue
  (const Standard_CString val,
   const TCollection_AsciiString& text, const Standard_Boolean exact)
{
  if (exact) return text.IsEqual (val);
  // NB: no regexp
  char cardeb = text.Value(1);
  Standard_Integer ln,lnt,i,j;
  ln  = text.Length();
  lnt = (Standard_Integer)(strlen(val) - ln);
  for (i = 0; i <= lnt; i ++) {
    if (val[i] == cardeb) {
//    un candidat
      Standard_Boolean res = Standard_True;
      for (j = 1; j < ln; j ++) {
        if (val[i+j] != text.Value(j+1))
          {  res = Standard_False;  break;  }
      }
      if (res) return res;
    }
  }
  return Standard_False;
}


    Standard_CString  IFSelect_Signature::IntValue
  (const Standard_Integer val)
{
  switch (val) {
    case 0 : return "0";
    case 1 : return "1";
    case 2 : return "2";
    case 3 : return "3";
    case 4 : return "4";
    case 5 : return "5";
    case 6 : return "6";
    case 7 : return "7";
    case 8 : return "8";
    case 9 : return "9";
    default : break;
  }
  sprintf (intval,"%d",val);
  return intval;
}
