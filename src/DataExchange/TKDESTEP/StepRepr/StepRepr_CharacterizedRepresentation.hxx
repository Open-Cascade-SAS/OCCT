// Created on: 2016-08-25
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _StepRepr_CharacterizedRepresentation_HeaderFile
#define _StepRepr_CharacterizedRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_Representation.hxx>
class TCollection_HAsciiString;
class StepRepr_RepresentationContext;

class StepRepr_CharacterizedRepresentation : public StepRepr_Representation
{

public:
  Standard_EXPORT StepRepr_CharacterizedRepresentation();

  //! Returns a CharacterizedRepresentation
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& theName,
    const occ::handle<TCollection_HAsciiString>& theDescription,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& theItems,
    const occ::handle<StepRepr_RepresentationContext>& theContextOfItems);

  void SetDescription(const occ::handle<TCollection_HAsciiString>& theDescription)
  {
    myDescription = theDescription;
  }

  occ::handle<TCollection_HAsciiString> Description() const { return myDescription; }

  DEFINE_STANDARD_RTTIEXT(StepRepr_CharacterizedRepresentation, StepRepr_Representation)

private:
  occ::handle<TCollection_HAsciiString> myDescription;
};
#endif // _StepRepr_CharacterizedRepresentation_HeaderFile
