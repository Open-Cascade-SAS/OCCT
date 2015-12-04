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


#include <IFSelect_ContextWrite.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESSelect_FloatFormat.hxx>
#include <Interface_FloatWriter.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_FloatFormat,IGESSelect_FileModifier)

IGESSelect_FloatFormat::IGESSelect_FloatFormat ()
    : thezerosup (Standard_True) , themainform ("%E") ,
      theformrange ("%f") , therangemin (0.1) , therangemax (1000.)
      {  }

    void  IGESSelect_FloatFormat::SetDefault (const Standard_Integer digits)
{
  themainform.Clear();
  theformrange.Clear();
  if (digits <= 0) {
    themainform.AssignCat  ("%E");
    theformrange.AssignCat ("%f");
  } else {
    char format[20];
    char pourcent = '%'; char point = '.';
    Sprintf(format,  "%c%d%c%dE",pourcent,digits+2,point,digits);
    themainform.AssignCat  (format);
    Sprintf(format,  "%c%d%c%df",pourcent,digits+2,point,digits);
    theformrange.AssignCat (format);
  }
  therangemin = 0.1; therangemax = 1000.;
  thezerosup = Standard_True;
}

    void  IGESSelect_FloatFormat::SetZeroSuppress (const Standard_Boolean mode)
      {  thezerosup = mode;  }

    void  IGESSelect_FloatFormat::SetFormat (const Standard_CString format)
      {  themainform.Clear();  themainform.AssignCat(format);  }


    void  IGESSelect_FloatFormat::SetFormatForRange
  (const Standard_CString form, const Standard_Real R1, const Standard_Real R2)
{
  theformrange.Clear();  theformrange.AssignCat(form);
  therangemin = R1;  therangemax = R2;
}

    void  IGESSelect_FloatFormat::Format
  (Standard_Boolean& zerosup,  TCollection_AsciiString& mainform,
   Standard_Boolean& hasrange, TCollection_AsciiString& formrange,
   Standard_Real& rangemin,    Standard_Real& rangemax) const
{
  zerosup   = thezerosup;
  mainform  = themainform;
  hasrange  = (theformrange.Length() > 0);
  formrange = theformrange;
  rangemin  = therangemin;
  rangemax  = therangemax;
}


    void  IGESSelect_FloatFormat::Perform
  (IFSelect_ContextWrite& /*ctx*/,
   IGESData_IGESWriter& writer) const
{
  writer.FloatWriter().SetFormat (themainform.ToCString());
  writer.FloatWriter().SetZeroSuppress (thezerosup);
  if (theformrange.Length() > 0) writer.FloatWriter().SetFormatForRange
    (theformrange.ToCString(), therangemin, therangemax);
}

    TCollection_AsciiString  IGESSelect_FloatFormat::Label () const
{
  TCollection_AsciiString lab("Float Format ");
  if (thezerosup) lab.AssignCat(" ZeroSup ");
  lab.AssignCat (themainform);
  if (theformrange.Length() > 0) {
    char mess[30];
//    Sprintf(mess,", in range %f %f %s",
//	    therangemin,therangemax,theformrange.ToCString());
//    lab.AssignCat(mess);
//    ... FloatFormat a droit aussi a un beau format pour son propre compte ...
    lab.AssignCat (", in range ");
    Standard_Integer convlen = Interface_FloatWriter::Convert
      (therangemin,mess,Standard_True,therangemin/2.,therangemax*2.,"%f","%f");
    mess[convlen] = ' ';  mess[convlen+1] = '\0';
    lab.AssignCat(mess);
    convlen = Interface_FloatWriter::Convert
      (therangemax,mess,Standard_True,therangemin/2.,therangemax*2.,"%f","%f");
    mess[convlen] = ':';  mess[convlen+1] = '\0';
    lab.AssignCat(mess);
    lab.AssignCat(theformrange.ToCString());
  }
  return lab;
}
