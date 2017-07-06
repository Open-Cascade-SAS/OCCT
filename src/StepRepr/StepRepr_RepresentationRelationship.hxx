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

#ifndef _StepRepr_RepresentationRelationship_HeaderFile
#define _StepRepr_RepresentationRelationship_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepRepr_Representation;


class StepRepr_RepresentationRelationship;
DEFINE_STANDARD_HANDLE(StepRepr_RepresentationRelationship, Standard_Transient)


class StepRepr_RepresentationRelationship : public Standard_Transient
{

public:

  
  //! Returns a RepresentationRelationship
  Standard_EXPORT StepRepr_RepresentationRelationship();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepRepr_Representation)& aRep1, const Handle(StepRepr_Representation)& aRep2);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& aDescription);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  Standard_EXPORT void SetRep1 (const Handle(StepRepr_Representation)& aRep1);
  
  Standard_EXPORT Handle(StepRepr_Representation) Rep1() const;
  
  Standard_EXPORT void SetRep2 (const Handle(StepRepr_Representation)& aRep2);
  
  Standard_EXPORT Handle(StepRepr_Representation) Rep2() const;




  DEFINE_STANDARD_RTTIEXT(StepRepr_RepresentationRelationship,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) name;
  Handle(TCollection_HAsciiString) description;
  Handle(StepRepr_Representation) rep1;
  Handle(StepRepr_Representation) rep2;


};







#endif // _StepRepr_RepresentationRelationship_HeaderFile
