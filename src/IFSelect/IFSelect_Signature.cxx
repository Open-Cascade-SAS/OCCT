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

#include <IFSelect_Signature.ixx>
#include <stdio.h>


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
  lnt = strlen(val) - ln;
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
