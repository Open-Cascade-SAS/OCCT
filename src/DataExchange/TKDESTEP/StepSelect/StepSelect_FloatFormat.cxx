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
#include <Standard_Type.hxx>
#include <StepData_StepWriter.hxx>
#include <StepSelect_FloatFormat.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(StepSelect_FloatFormat, StepSelect_FileModifier)

StepSelect_FloatFormat::StepSelect_FloatFormat()
    : thezerosup(true),
      themainform("%E"),
      theformrange("%f"),
      therangemin(0.1),
      therangemax(1000.)
{
}

void StepSelect_FloatFormat::SetDefault(const int digits)
{
  themainform.Clear();
  theformrange.Clear();
  if (digits <= 0)
  {
    themainform.AssignCat("%E");
    theformrange.AssignCat("%f");
  }
  else
  {
    char format[20];
    char pourcent = '%';
    char point    = '.';
    Sprintf(format, "%c%d%c%dE", pourcent, digits + 2, point, digits);
    themainform.AssignCat(format);
    Sprintf(format, "%c%d%c%df", pourcent, digits + 2, point, digits);
    theformrange.AssignCat(format);
  }
  therangemin = 0.1;
  therangemax = 1000.;
  thezerosup  = true;
}

void StepSelect_FloatFormat::SetZeroSuppress(const bool mode)
{
  thezerosup = mode;
}

void StepSelect_FloatFormat::SetFormat(const char* const format)
{
  themainform.Clear();
  themainform.AssignCat(format);
}

void StepSelect_FloatFormat::SetFormatForRange(const char* const form,
                                               const double      R1,
                                               const double      R2)
{
  theformrange.Clear();
  theformrange.AssignCat(form);
  therangemin = R1;
  therangemax = R2;
}

void StepSelect_FloatFormat::Format(bool&                    zerosup,
                                    TCollection_AsciiString& mainform,
                                    bool&                    hasrange,
                                    TCollection_AsciiString& formrange,
                                    double&                  rangemin,
                                    double&                  rangemax) const
{
  zerosup   = thezerosup;
  mainform  = themainform;
  hasrange  = (theformrange.Length() > 0);
  formrange = theformrange;
  rangemin  = therangemin;
  rangemax  = therangemax;
}

void StepSelect_FloatFormat::Perform(IFSelect_ContextWrite& /*ctx*/,
                                     StepData_StepWriter& writer) const
{
  writer.FloatWriter().SetFormat(themainform.ToCString());
  writer.FloatWriter().SetZeroSuppress(thezerosup);
  if (theformrange.Length() > 0)
    writer.FloatWriter().SetFormatForRange(theformrange.ToCString(), therangemin, therangemax);
}

TCollection_AsciiString StepSelect_FloatFormat::Label() const
{
  TCollection_AsciiString lab("Float Format ");
  if (thezerosup)
    lab.AssignCat(" ZeroSuppress");
  lab.AssignCat(themainform);
  if (theformrange.Length() > 0)
  {
    char mess[30];
    Sprintf(mess, ", in range %f %f %s", therangemin, therangemax, theformrange.ToCString());
    lab.AssignCat(mess);
  }
  return lab;
}
