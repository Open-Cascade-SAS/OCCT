// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepElement_AnalysisItemWithinRepresentation_HeaderFile
#define _StepElement_AnalysisItemWithinRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepRepr_RepresentationItem;
class StepRepr_Representation;


class StepElement_AnalysisItemWithinRepresentation;
DEFINE_STANDARD_HANDLE(StepElement_AnalysisItemWithinRepresentation, Standard_Transient)

//! Representation of STEP entity AnalysisItemWithinRepresentation
class StepElement_AnalysisItemWithinRepresentation : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepElement_AnalysisItemWithinRepresentation();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepRepr_RepresentationItem)& aItem, const Handle(StepRepr_Representation)& aRep);
  
  //! Returns field Name
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  //! Set field Name
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& Name);
  
  //! Returns field Description
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  //! Set field Description
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& Description);
  
  //! Returns field Item
  Standard_EXPORT Handle(StepRepr_RepresentationItem) Item() const;
  
  //! Set field Item
  Standard_EXPORT void SetItem (const Handle(StepRepr_RepresentationItem)& Item);
  
  //! Returns field Rep
  Standard_EXPORT Handle(StepRepr_Representation) Rep() const;
  
  //! Set field Rep
  Standard_EXPORT void SetRep (const Handle(StepRepr_Representation)& Rep);




  DEFINE_STANDARD_RTTIEXT(StepElement_AnalysisItemWithinRepresentation,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theName;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(StepRepr_RepresentationItem) theItem;
  Handle(StepRepr_Representation) theRep;


};







#endif // _StepElement_AnalysisItemWithinRepresentation_HeaderFile
