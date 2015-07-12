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


#include <StepData_EnumTool.hxx>
#include <TCollection_AsciiString.hxx>

static TCollection_AsciiString nulstr("");

StepData_EnumTool::StepData_EnumTool
  (const Standard_CString e0, const Standard_CString e1,
   const Standard_CString e2, const Standard_CString e3,
   const Standard_CString e4, const Standard_CString e5,
   const Standard_CString e6, const Standard_CString e7,
   const Standard_CString e8, const Standard_CString e9,
   const Standard_CString e10,const Standard_CString e11,
   const Standard_CString e12,const Standard_CString e13,
   const Standard_CString e14,const Standard_CString e15,
   const Standard_CString e16,const Standard_CString e17,
   const Standard_CString e18,const Standard_CString e19,
   const Standard_CString e20,const Standard_CString e21,
   const Standard_CString e22,const Standard_CString e23,
   const Standard_CString e24,const Standard_CString e25,
   const Standard_CString e26,const Standard_CString e27,
   const Standard_CString e28,const Standard_CString e29,
   const Standard_CString e30,const Standard_CString e31,
   const Standard_CString e32,const Standard_CString e33,
   const Standard_CString e34,const Standard_CString e35,
   const Standard_CString e36,const Standard_CString e37,
   const Standard_CString e38,const Standard_CString e39)
{
  AddDefinition (e0);   AddDefinition (e1);   AddDefinition (e2);
  AddDefinition (e3);   AddDefinition (e4);   AddDefinition (e5);
  AddDefinition (e6);   AddDefinition (e7);   AddDefinition (e8);
  AddDefinition (e9);   AddDefinition (e10);  AddDefinition (e11);
  AddDefinition (e12);  AddDefinition (e13);  AddDefinition (e14);
  AddDefinition (e15);  AddDefinition (e16);  AddDefinition (e17);
  AddDefinition (e18);  AddDefinition (e19);  AddDefinition (e20);
  AddDefinition (e21);  AddDefinition (e22);  AddDefinition (e23);
  AddDefinition (e24);  AddDefinition (e25);  AddDefinition (e26);
  AddDefinition (e27);  AddDefinition (e28);  AddDefinition (e29);
  AddDefinition (e30);  AddDefinition (e31);  AddDefinition (e32);
  AddDefinition (e33);  AddDefinition (e34);  AddDefinition (e35);
  AddDefinition (e36);  AddDefinition (e37);  AddDefinition (e38);
  AddDefinition (e39);
  theinit = thetexts.Length();  theopt = Standard_True;
}

    void  StepData_EnumTool::AddDefinition (const Standard_CString term)
{
  char text[80];
  if (!term) return;
  if (term[0] == '\0') return;
  Standard_Integer n0 = 0, n1 = 0;
  for (; term[n0] != '\0'; n0 ++)  {
    if (term[n0] <= 32) {
      if (n1 == 0) continue;
      if (n1 > 1 || text[0] != '$') {
	if (text[n1-1] != '.')  {  text[n1] = '.';  n1 ++;  }
	text[n1] = '\0';
      }
      thetexts.Append ( TCollection_AsciiString(text) );
      n1 = 0;
    }
    if (n1 == 0 && term[n0] != '.' && term[n0] != '$')  {  text[0] = '.';   n1 ++;  }
    text[n1] = term[n0];  n1 ++;
  }
  if (n1 > 0 || text[0] != '$') {
    if (text[n1-1] != '.')  {  text[n1] = '.';  n1 ++;  }
    text[n1] = '\0';
  }
  if (text[n1-1] != '.')  {  text[n1] = '.';  n1 ++;  }
  text[n1] = '\0';
  thetexts.Append ( TCollection_AsciiString(text) );
}

    Standard_Boolean  StepData_EnumTool::IsSet () const
      {  return (thetexts.Length() > theinit);  }

    Standard_Integer  StepData_EnumTool::MaxValue () const
      {  return thetexts.Length() - 1;  }

    void  StepData_EnumTool::Optional (const Standard_Boolean mode)
      {  theopt = mode;  }

    Standard_Integer  StepData_EnumTool::NullValue () const
      {  return (theopt ? Value("$") : Standard_False);  }

    const TCollection_AsciiString&  StepData_EnumTool::Text
  (const Standard_Integer num) const
{
  if (num < 0 || num >= thetexts.Length()) return nulstr;
  return thetexts.Value (num+1);
}

    Standard_Integer  StepData_EnumTool::Value
  (const Standard_CString txt) const
{
  Standard_Integer nb = thetexts.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thetexts.Value(i).IsEqual(txt)) return i-1;
  }
  return (-1);
}

    Standard_Integer  StepData_EnumTool::Value
  (const TCollection_AsciiString& txt) const
{
  Standard_Integer nb = thetexts.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (thetexts.Value(i).IsEqual(txt)) return i-1;
  }
  return (-1);
}
