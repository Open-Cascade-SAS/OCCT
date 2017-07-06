// Created on: 1997-03-26
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepRepr_ItemDefinedTransformation_HeaderFile
#define _StepRepr_ItemDefinedTransformation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class TCollection_HAsciiString;
class StepRepr_RepresentationItem;


class StepRepr_ItemDefinedTransformation;
DEFINE_STANDARD_HANDLE(StepRepr_ItemDefinedTransformation, Standard_Transient)

//! Added from StepRepr Rev2 to Rev4
class StepRepr_ItemDefinedTransformation : public Standard_Transient
{

public:

  
  Standard_EXPORT StepRepr_ItemDefinedTransformation();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(TCollection_HAsciiString)& aDescription, const Handle(StepRepr_RepresentationItem)& aTransformItem1, const Handle(StepRepr_RepresentationItem)& aTransformItem2);
  
  Standard_EXPORT void SetName (const Handle(TCollection_HAsciiString)& aName);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Name() const;
  
  Standard_EXPORT void SetDescription (const Handle(TCollection_HAsciiString)& aDescription);
  
  Standard_EXPORT Handle(TCollection_HAsciiString) Description() const;
  
  Standard_EXPORT void SetTransformItem1 (const Handle(StepRepr_RepresentationItem)& aItem);
  
  Standard_EXPORT Handle(StepRepr_RepresentationItem) TransformItem1() const;
  
  Standard_EXPORT void SetTransformItem2 (const Handle(StepRepr_RepresentationItem)& aItem);
  
  Standard_EXPORT Handle(StepRepr_RepresentationItem) TransformItem2() const;




  DEFINE_STANDARD_RTTIEXT(StepRepr_ItemDefinedTransformation,Standard_Transient)

protected:




private:


  Handle(TCollection_HAsciiString) theName;
  Handle(TCollection_HAsciiString) theDescription;
  Handle(StepRepr_RepresentationItem) theTransformItem1;
  Handle(StepRepr_RepresentationItem) theTransformItem2;


};







#endif // _StepRepr_ItemDefinedTransformation_HeaderFile
