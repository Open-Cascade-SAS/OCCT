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

#ifndef _StepElement_Volume3dElementDescriptor_HeaderFile
#define _StepElement_Volume3dElementDescriptor_HeaderFile

#include <Standard.hxx>

#include <StepElement_HArray1OfVolumeElementPurposeMember.hxx>
#include <StepElement_Volume3dElementShape.hxx>
#include <StepElement_ElementDescriptor.hxx>
#include <StepElement_ElementOrder.hxx>
class TCollection_HAsciiString;


class StepElement_Volume3dElementDescriptor;
DEFINE_STANDARD_HANDLE(StepElement_Volume3dElementDescriptor, StepElement_ElementDescriptor)

//! Representation of STEP entity Volume3dElementDescriptor
class StepElement_Volume3dElementDescriptor : public StepElement_ElementDescriptor
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepElement_Volume3dElementDescriptor();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const StepElement_ElementOrder aElementDescriptor_TopologyOrder, const Handle(TCollection_HAsciiString)& aElementDescriptor_Description, const Handle(StepElement_HArray1OfVolumeElementPurposeMember)& aPurpose, const StepElement_Volume3dElementShape aShape);
  
  //! Returns field Purpose
  Standard_EXPORT Handle(StepElement_HArray1OfVolumeElementPurposeMember) Purpose() const;
  
  //! Set field Purpose
  Standard_EXPORT void SetPurpose (const Handle(StepElement_HArray1OfVolumeElementPurposeMember)& Purpose);
  
  //! Returns field Shape
  Standard_EXPORT StepElement_Volume3dElementShape Shape() const;
  
  //! Set field Shape
  Standard_EXPORT void SetShape (const StepElement_Volume3dElementShape Shape);




  DEFINE_STANDARD_RTTIEXT(StepElement_Volume3dElementDescriptor,StepElement_ElementDescriptor)

protected:




private:


  Handle(StepElement_HArray1OfVolumeElementPurposeMember) thePurpose;
  StepElement_Volume3dElementShape theShape;


};







#endif // _StepElement_Volume3dElementDescriptor_HeaderFile
