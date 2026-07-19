// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepVisual_ColourRgb_HeaderFile
#define _StepVisual_ColourRgb_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <StepVisual_ColourSpecification.hxx>
class TCollection_HAsciiString;

class StepVisual_ColourRgb : public StepVisual_ColourSpecification
{

public:
  //! Returns a ColourRgb
  Standard_EXPORT StepVisual_ColourRgb();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const double                                 aRed,
                            const double                                 aGreen,
                            const double                                 aBlue);

  Standard_EXPORT void SetRed(const double aRed);

  Standard_EXPORT double Red() const;

  Standard_EXPORT void SetGreen(const double aGreen);

  Standard_EXPORT double Green() const;

  Standard_EXPORT void SetBlue(const double aBlue);

  Standard_EXPORT double Blue() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_ColourRgb, StepVisual_ColourSpecification)

private:
  double red;
  double green;
  double blue;
};

#endif // _StepVisual_ColourRgb_HeaderFile
