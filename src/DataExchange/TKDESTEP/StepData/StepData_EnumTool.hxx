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

#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
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
  Standard_EXPORT StepData_EnumTool(const char* const e0  = "",
                                    const char* const e1  = "",
                                    const char* const e2  = "",
                                    const char* const e3  = "",
                                    const char* const e4  = "",
                                    const char* const e5  = "",
                                    const char* const e6  = "",
                                    const char* const e7  = "",
                                    const char* const e8  = "",
                                    const char* const e9  = "",
                                    const char* const e10 = "",
                                    const char* const e11 = "",
                                    const char* const e12 = "",
                                    const char* const e13 = "",
                                    const char* const e14 = "",
                                    const char* const e15 = "",
                                    const char* const e16 = "",
                                    const char* const e17 = "",
                                    const char* const e18 = "",
                                    const char* const e19 = "",
                                    const char* const e20 = "",
                                    const char* const e21 = "",
                                    const char* const e22 = "",
                                    const char* const e23 = "",
                                    const char* const e24 = "",
                                    const char* const e25 = "",
                                    const char* const e26 = "",
                                    const char* const e27 = "",
                                    const char* const e28 = "",
                                    const char* const e29 = "",
                                    const char* const e30 = "",
                                    const char* const e31 = "",
                                    const char* const e32 = "",
                                    const char* const e33 = "",
                                    const char* const e34 = "",
                                    const char* const e35 = "",
                                    const char* const e36 = "",
                                    const char* const e37 = "",
                                    const char* const e38 = "",
                                    const char* const e39 = "");

  //! Processes a definition, splits it according blanks if any
  //! empty definitions are ignored
  //! A null definition can be input by given "$" :the corresponding
  //! position is attached to "null/undefined" value (as one
  //! particular item of the enumeration list)
  //! See also IsSet
  Standard_EXPORT void AddDefinition(const char* const term);

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
  Standard_EXPORT bool IsSet() const;

  //! Returns the maximum integer for a suitable value
  //! Remark : while values begin at zero, MaxValue is the count of
  //! recorded values minus one
  Standard_EXPORT int MaxValue() const;

  //! Sets or Unsets the EnumTool to accept undefined value (for
  //! optional field). Ignored if no null value is defined (by "$")
  //! Can be changed during execution (to read each field),
  //! Default is True (if a null value is defined)
  Standard_EXPORT void Optional(const bool mode);

  //! Returns the value attached to "null/undefined value"
  //! If none is specified or if Optional has been set to False,
  //! returns -1
  //! Null Value has been specified by definition "$"
  Standard_EXPORT int NullValue() const;

  //! Returns the text which corresponds to a given numeric value
  //! It is limited by dots
  //! If num is out of range, returns an empty string
  Standard_EXPORT const TCollection_AsciiString& Text(const int num) const;

  //! Returns the numeric value found for a text
  //! The text must be in capitals and limited by dots
  //! A non-suitable text gives a negative value to be returned
  Standard_EXPORT int Value(const char* const txt) const;

  //! Same as above but works on an AsciiString
  Standard_EXPORT int Value(const TCollection_AsciiString& txt) const;

private:
  NCollection_Sequence<TCollection_AsciiString> thetexts;
  int                                           theinit;
  bool                                          theopt;
};

#endif // _StepData_EnumTool_HeaderFile
