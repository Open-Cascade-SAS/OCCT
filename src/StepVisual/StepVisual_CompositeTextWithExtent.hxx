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

#ifndef _StepVisual_CompositeTextWithExtent_HeaderFile
#define _StepVisual_CompositeTextWithExtent_HeaderFile

#include <Standard.hxx>

#include <StepVisual_CompositeText.hxx>
#include <StepVisual_HArray1OfTextOrCharacter.hxx>
class StepVisual_PlanarExtent;
class TCollection_HAsciiString;


class StepVisual_CompositeTextWithExtent;
DEFINE_STANDARD_HANDLE(StepVisual_CompositeTextWithExtent, StepVisual_CompositeText)


class StepVisual_CompositeTextWithExtent : public StepVisual_CompositeText
{

public:

  
  //! Returns a CompositeTextWithExtent
  Standard_EXPORT StepVisual_CompositeTextWithExtent();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText, const Handle(StepVisual_PlanarExtent)& aExtent);
  
  Standard_EXPORT void SetExtent (const Handle(StepVisual_PlanarExtent)& aExtent);
  
  Standard_EXPORT Handle(StepVisual_PlanarExtent) Extent() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_CompositeTextWithExtent,StepVisual_CompositeText)

protected:




private:


  Handle(StepVisual_PlanarExtent) extent;


};







#endif // _StepVisual_CompositeTextWithExtent_HeaderFile
