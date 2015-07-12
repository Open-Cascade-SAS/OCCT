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

#ifndef _StepVisual_FillAreaStyleColour_HeaderFile
#define _StepVisual_FillAreaStyleColour_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <MMgt_TShared.hxx>
class TCollection_HAsciiString;
class StepVisual_Colour;


class StepVisual_FillAreaStyleColour;
DEFINE_STANDARD_HANDLE(StepVisual_FillAreaStyleColour, MMgt_TShared)


class StepVisual_FillAreaStyleColour : public MMgt_TShared
{

public:

  
  //! Returns a FillAreaStyleColour
  Standard_EXPORT StepVisual_FillAreaStyleColour();
  
  Standard_EXPORT virtual void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepVisual_Colour)& aFillColour);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetFillColour (const Handle(StepVisual_Colour)& aFillColour);
  
  Standard_EXPORT Handle(StepVisual_Colour) FillColour() const;




  DEFINE_STANDARD_RTTI(StepVisual_FillAreaStyleColour,MMgt_TShared)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  Handle(StepVisual_Colour) fillColour;


};







#endif // _StepVisual_FillAreaStyleColour_HeaderFile
