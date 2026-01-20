// Created on: 1994-06-01
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESSelect_FloatFormat_HeaderFile
#define _IGESSelect_FloatFormat_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <IGESSelect_FileModifier.hxx>
#include <Standard_Integer.hxx>
class IFSelect_ContextWrite;
class IGESData_IGESWriter;

//! This class gives control out format for floatting values :
//! ZeroSuppress or no, Main Format, Format in Range (for values
//! around 1.), as IGESWriter allows to manage it.
//! Formats are given under C-printf form
class IGESSelect_FloatFormat : public IGESSelect_FileModifier
{

public:
  //! Creates a new FloatFormat, with standard options :
  //! ZeroSuppress, Main Format = %E,
  //! Format between 0.001 and 1000. = %f
  Standard_EXPORT IGESSelect_FloatFormat();

  //! Sets FloatFormat to default value (see Create) but if <digits>
  //! is given positive, it commands Formats (main and range) to
  //! ensure <digits> significant digits to be displayed
  Standard_EXPORT void SetDefault(const int digits = 0);

  //! Sets ZeroSuppress mode to a new value
  Standard_EXPORT void SetZeroSuppress(const bool mode);

  //! Sets Main Format to a new value
  //! Remark : SetFormat, SetZeroSuppress and SetFormatForRange are
  //! independent
  Standard_EXPORT void SetFormat(const char* const format = "%E");

  //! Sets Format for Range to a new value with its range of
  //! application.
  //! To cancel it, give format as "" (empty string)
  //! Remark that if the condition (0. < Rmin < Rmax) is not
  //! verified, this secondary format will be ignored.
  //! Moreover, this secondary format is intended to be used in a
  //! range around 1.
  Standard_EXPORT void SetFormatForRange(const char* const format = "%f",
                                         const double    Rmin   = 0.1,
                                         const double    Rmax   = 1000.0);

  //! Returns all recorded parameters :
  //! zerosup  : ZeroSuppress status
  //! mainform : Main Format (which applies out of the range, or
  //! for every real if no range is set)
  //! hasrange : True if a FormatInRange is set, False else
  //! (following parameters do not apply if it is False)
  //! forminrange : Secondary Format (it applies inside the range)
  //! rangemin, rangemax : the range in which the secondary format
  //! applies
  Standard_EXPORT void Format(bool&        zerosup,
                              TCollection_AsciiString& mainform,
                              bool&        hasrange,
                              TCollection_AsciiString& forminrange,
                              double&           rangemin,
                              double&           rangemax) const;

  //! Sets the Floatting Formats of IGESWriter to the recorded
  //! parameters
  Standard_EXPORT void Perform(IFSelect_ContextWrite& ctx,
                               IGESData_IGESWriter&   writer) const override;

  //! Returns specific Label : for instance,
  //! "Float Format [ZeroSuppress] %E [, in range R1-R2 %f]"
  Standard_EXPORT TCollection_AsciiString Label() const override;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_FloatFormat, IGESSelect_FileModifier)

private:
  bool        thezerosup;
  TCollection_AsciiString themainform;
  TCollection_AsciiString theformrange;
  double           therangemin;
  double           therangemax;
};

#endif // _IGESSelect_FloatFormat_HeaderFile
