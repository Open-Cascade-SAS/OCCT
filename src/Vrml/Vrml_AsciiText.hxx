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

#include <TColStd_HArray1OfAsciiString.hxx>
#include <Vrml_AsciiTextJustification.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

class Vrml_AsciiText;
DEFINE_STANDARD_HANDLE(Vrml_AsciiText, Standard_Transient)

//! defines a AsciiText node of VRML specifying geometry shapes.
//! This  node  represents  strings  of  text  characters  from  ASCII  coded
//! character  set. All subsequent strings advance y by -( size * spacing).
//! The justification field determines the placement of the strings in the x
//! dimension. LEFT (the default) places the left edge of each string at x=0.
//! CENTER places the center of each string at x=0. RIGHT places the right edge
//! of each string at x=0. Text is rendered from left to right, top to
//! bottom in the font set by FontStyle.
//! The  default  value  for  the  wigth  field  indicates  the  natural  width
//! should  be  used  for  that  string.
class Vrml_AsciiText : public Standard_Transient
{

public:
  Standard_EXPORT Vrml_AsciiText();

  Standard_EXPORT Vrml_AsciiText(const Handle(TColStd_HArray1OfAsciiString)& aString,
                                 const Standard_Real                         aSpacing,
                                 const Vrml_AsciiTextJustification           aJustification,
                                 const Standard_Real                         aWidth);

  Standard_EXPORT void SetString(const Handle(TColStd_HArray1OfAsciiString)& aString);

  Standard_EXPORT Handle(TColStd_HArray1OfAsciiString) String() const;

  Standard_EXPORT void SetSpacing(const Standard_Real aSpacing);

  Standard_EXPORT Standard_Real Spacing() const;

  Standard_EXPORT void SetJustification(const Vrml_AsciiTextJustification aJustification);

  Standard_EXPORT Vrml_AsciiTextJustification Justification() const;

  Standard_EXPORT void SetWidth(const Standard_Real aWidth);

  Standard_EXPORT Standard_Real Width() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

  DEFINE_STANDARD_RTTIEXT(Vrml_AsciiText, Standard_Transient)

protected:
private:
  Handle(TColStd_HArray1OfAsciiString) myString;
  Standard_Real                        mySpacing;
  Vrml_AsciiTextJustification          myJustification;
  Standard_Real                        myWidth;
};

#endif // _Vrml_AsciiText_HeaderFile
