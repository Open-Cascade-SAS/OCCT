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

#ifndef _StepVisual_CompositeText_HeaderFile
#define _StepVisual_CompositeText_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepVisual_HArray1OfTextOrCharacter.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepVisual_TextOrCharacter;


class StepVisual_CompositeText;
DEFINE_STANDARD_HANDLE(StepVisual_CompositeText, StepGeom_GeometricRepresentationItem)


class StepVisual_CompositeText : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Returns a CompositeText
  Standard_EXPORT StepVisual_CompositeText();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText);
  
  Standard_EXPORT void SetCollectedText (const Handle(StepVisual_HArray1OfTextOrCharacter)& aCollectedText);
  
  Standard_EXPORT Handle(StepVisual_HArray1OfTextOrCharacter) CollectedText() const;
  
  Standard_EXPORT StepVisual_TextOrCharacter CollectedTextValue (const Standard_Integer num) const;
  
  Standard_EXPORT Standard_Integer NbCollectedText() const;




  DEFINE_STANDARD_RTTIEXT(StepVisual_CompositeText,StepGeom_GeometricRepresentationItem)

protected:




private:


  Handle(StepVisual_HArray1OfTextOrCharacter) collectedText;


};







#endif // _StepVisual_CompositeText_HeaderFile
