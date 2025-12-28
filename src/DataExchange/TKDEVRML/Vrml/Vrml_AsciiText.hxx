// Created on: 1996-12-24
// Created by: Alexander BRIVIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Vrml_AsciiText_HeaderFile
#define _Vrml_AsciiText_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Vrml_AsciiTextJustification.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

//! defines a AsciiText node of VRML specifying geometry shapes.
//! This node represents strings of text characters from ASCII coded
//! character set. All subsequent strings advance y by -( size * spacing).
//! The justification field determines the placement of the strings in the x
//! dimension. LEFT (the default) places the left edge of each string at x=0.
//! CENTER places the center of each string at x=0. RIGHT places the right edge
//! of each string at x=0. Text is rendered from left to right, top to
//! bottom in the font set by FontStyle.
//! The default value for the wigth field indicates the natural width
//! should be used for that string.
class Vrml_AsciiText : public Standard_Transient
{

public:
  Standard_EXPORT Vrml_AsciiText();

  Standard_EXPORT Vrml_AsciiText(
    const occ::handle<NCollection_HArray1<TCollection_AsciiString>>& aString,
    const double                                                     aSpacing,
    const Vrml_AsciiTextJustification                                aJustification,
    const double                                                     aWidth);

  Standard_EXPORT void SetString(
    const occ::handle<NCollection_HArray1<TCollection_AsciiString>>& aString);

  Standard_EXPORT occ::handle<NCollection_HArray1<TCollection_AsciiString>> String() const;

  Standard_EXPORT void SetSpacing(const double aSpacing);

  Standard_EXPORT double Spacing() const;

  Standard_EXPORT void SetJustification(const Vrml_AsciiTextJustification aJustification);

  Standard_EXPORT Vrml_AsciiTextJustification Justification() const;

  Standard_EXPORT void SetWidth(const double aWidth);

  Standard_EXPORT double Width() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

  DEFINE_STANDARD_RTTIEXT(Vrml_AsciiText, Standard_Transient)

private:
  occ::handle<NCollection_HArray1<TCollection_AsciiString>> myString;
  double                                                    mySpacing;
  Vrml_AsciiTextJustification                               myJustification;
  double                                                    myWidth;
};

#endif // _Vrml_AsciiText_HeaderFile
