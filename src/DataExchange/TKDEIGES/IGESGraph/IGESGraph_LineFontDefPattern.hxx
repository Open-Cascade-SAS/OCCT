// Created on: 1993-01-11
// Created by: CKY / Contract Toubro-Larsen ( Niraj RANGWALA )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESGraph_LineFontDefPattern_HeaderFile
#define _IGESGraph_LineFontDefPattern_HeaderFile

#include <Standard.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;

//! defines IGESLineFontDefPattern, Type <304> Form <2>
//! in package IGESGraph
//!
//! Line Font may be defined by repetition of a basic pattern
//! of visible-blank(or, on-off) segments superimposed on
//! a line or a curve. The line or curve is then displayed
//! according to the basic pattern.
class IGESGraph_LineFontDefPattern : public IGESData_LineFontEntity
{

public:
  Standard_EXPORT IGESGraph_LineFontDefPattern();

  //! This method is used to set the fields of the class
  //! LineFontDefPattern
  //! - allSegLength : Containing lengths of respective segments
  //! - aPattern     : HAsciiString indicating visible-blank segments
  Standard_EXPORT void Init(const occ::handle<NCollection_HArray1<double>>& allSegLength,
                            const occ::handle<TCollection_HAsciiString>&    aPattern);

  //! returns the number of segments in the visible-blank pattern
  Standard_EXPORT int NbSegments() const;

  //! returns the Length of Index'th segment of the basic pattern
  //! raises exception if Index <= 0 or Index > NbSegments
  Standard_EXPORT double Length(const int Index) const;

  //! returns the string indicating which segments of the basic
  //! pattern are visible and which are blanked.
  //! e.g:
  //! theNbSegments = 5 and if Bit Pattern = 10110, which means that
  //! segments 2, 3 and 5 are visible, whereas segments 1 and 4 are
  //! blank. The method returns "2H16" as the HAsciiString.
  //! Note: The bits are right justified. (16h = 10110)
  Standard_EXPORT occ::handle<TCollection_HAsciiString> DisplayPattern() const;

  //! The Display Pattern is decrypted to
  //! return True if the Index'th basic pattern is Visible,
  //! False otherwise.
  //! If Index > NbSegments or Index <= 0 then return value is
  //! False.
  Standard_EXPORT bool IsVisible(const int Index) const;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_LineFontDefPattern, IGESData_LineFontEntity)

private:
  occ::handle<NCollection_HArray1<double>> theSegmentLengths;
  occ::handle<TCollection_HAsciiString>    theDisplayPattern;
};

#endif // _IGESGraph_LineFontDefPattern_HeaderFile
