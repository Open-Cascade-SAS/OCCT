// Created on: 1995-10-25
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StepData_EnumTool_HeaderFile
#define _StepData_EnumTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_SequenceOfAsciiString.hxx>
#include <Standard_CString.hxx>
class TCollection_AsciiString;

//! This class gives a way of conversion between the value of an
//! enumeration and its representation in STEP
//! An enumeration corresponds to an integer with reserved values,
//! which begin to 0
//! In STEP, it is represented by a name in capital letter and
//! limited by two dots, e.g. .UNKNOWN.
//!
//! EnumTool works with integers, it is just required to cast
//! between an integer and an enumeration of required type.
//!
//! Its definition is intended to allow static creation in once,
//! without having to recreate once for each use.
//!
//! It is possible to define subclasses on it, which directly give
//! the good list of definition texts, and accepts a enumeration
//! of the good type instead of an integer
class StepData_EnumTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an EnumTool with definitions given by e0 .. e<max>
  //! Each definition string can bring one term, or several
  //! separated by blanks. Each term corresponds to one value of the
  //! enumeration, if dots are not presents they are added
  //!
  //! Such a static constructor allows to build a static description
  //! as : static StepData_EnumTool myenumtool("e0","e1"...);
  //! then use it without having to initialise it
  //!
  //! A null definition can be input by given "$" :the corresponding
  //! position is attached to "null/undefined" value (as one
  //! particular item of the enumeration list)
  Standard_EXPORT StepData_EnumTool(const Standard_CString e0  = "",
                                    const Standard_CString e1  = "",
                                    const Standard_CString e2  = "",
                                    const Standard_CString e3  = "",
                                    const Standard_CString e4  = "",
                                    const Standard_CString e5  = "",
                                    const Standard_CString e6  = "",
                                    const Standard_CString e7  = "",
                                    const Standard_CString e8  = "",
                                    const Standard_CString e9  = "",
                                    const Standard_CString e10 = "",
                                    const Standard_CString e11 = "",
                                    const Standard_CString e12 = "",
                                    const Standard_CString e13 = "",
                                    const Standard_CString e14 = "",
                                    const Standard_CString e15 = "",
                                    const Standard_CString e16 = "",
                                    const Standard_CString e17 = "",
                                    const Standard_CString e18 = "",
                                    const Standard_CString e19 = "",
                                    const Standard_CString e20 = "",
                                    const Standard_CString e21 = "",
                                    const Standard_CString e22 = "",
                                    const Standard_CString e23 = "",
                                    const Standard_CString e24 = "",
                                    const Standard_CString e25 = "",
                                    const Standard_CString e26 = "",
                                    const Standard_CString e27 = "",
                                    const Standard_CString e28 = "",
                                    const Standard_CString e29 = "",
                                    const Standard_CString e30 = "",
                                    const Standard_CString e31 = "",
                                    const Standard_CString e32 = "",
                                    const Standard_CString e33 = "",
                                    const Standard_CString e34 = "",
                                    const Standard_CString e35 = "",
                                    const Standard_CString e36 = "",
                                    const Standard_CString e37 = "",
                                    const Standard_CString e38 = "",
                                    const Standard_CString e39 = "");

  //! Processes a definition, splits it according blanks if any
  //! empty definitions are ignored
  //! A null definition can be input by given "$" :the corresponding
  //! position is attached to "null/undefined" value (as one
  //! particular item of the enumeration list)
  //! See also IsSet
  Standard_EXPORT void AddDefinition(const Standard_CString term);

  //! Returns True if at least one definition has been entered after
  //! creation time (i.e. by AddDefinition only)
  //!
  //! This allows to build a static description by a first pass :
  //! static StepData_EnumTool myenumtool("e0" ...);
  //! ...
  //! if (!myenumtool.IsSet()) {             for further inits
  //! myenumtool.AddDefinition("e21");
  //! ...
  //! }
  Standard_EXPORT Standard_Boolean IsSet() const;

  //! Returns the maximum integer for a suitable value
  //! Remark : while values begin at zero, MaxValue is the count of
  //! recorded values minus one
  Standard_EXPORT Standard_Integer MaxValue() const;

  //! Sets or Unsets the EnumTool to accept undefined value (for
  //! optional field). Ignored if no null value is defined (by "$")
  //! Can be changed during execution (to read each field),
  //! Default is True (if a null value is defined)
  Standard_EXPORT void Optional(const Standard_Boolean mode);

  //! Returns the value attached to "null/undefined value"
  //! If none is specified or if Optional has been set to False,
  //! returns -1
  //! Null Value has been specified by definition "$"
  Standard_EXPORT Standard_Integer NullValue() const;

  //! Returns the text which corresponds to a given numeric value
  //! It is limited by dots
  //! If num is out of range, returns an empty string
  Standard_EXPORT const TCollection_AsciiString& Text(const Standard_Integer num) const;

  //! Returns the numeric value found for a text
  //! The text must be in capitals and limited by dots
  //! A non-suitable text gives a negative value to be returned
  Standard_EXPORT Standard_Integer Value(const Standard_CString txt) const;

  //! Same as above but works on an AsciiString
  Standard_EXPORT Standard_Integer Value(const TCollection_AsciiString& txt) const;

protected:
private:
  TColStd_SequenceOfAsciiString thetexts;
  Standard_Integer              theinit;
  Standard_Boolean              theopt;
};

#endif // _StepData_EnumTool_HeaderFile
