// Created on: 2001-12-28
// Created by: Andrey BETENEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_EdgeBasedWireframeModel_HeaderFile
#define _StepShape_EdgeBasedWireframeModel_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_HArray1OfConnectedEdgeSet.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
class TCollection_HAsciiString;


class StepShape_EdgeBasedWireframeModel;
DEFINE_STANDARD_HANDLE(StepShape_EdgeBasedWireframeModel, StepGeom_GeometricRepresentationItem)

//! Representation of STEP entity EdgeBasedWireframeModel
class StepShape_EdgeBasedWireframeModel : public StepGeom_GeometricRepresentationItem
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepShape_EdgeBasedWireframeModel();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aRepresentationItem_Name, const Handle(StepShape_HArray1OfConnectedEdgeSet)& aEbwmBoundary);
  
  //! Returns field EbwmBoundary
  Standard_EXPORT Handle(StepShape_HArray1OfConnectedEdgeSet) EbwmBoundary() const;
  
  //! Set field EbwmBoundary
  Standard_EXPORT void SetEbwmBoundary (const Handle(StepShape_HArray1OfConnectedEdgeSet)& EbwmBoundary);




  DEFINE_STANDARD_RTTIEXT(StepShape_EdgeBasedWireframeModel,StepGeom_GeometricRepresentationItem)

protected:




private:


  Handle(StepShape_HArray1OfConnectedEdgeSet) theEbwmBoundary;


};







#endif // _StepShape_EdgeBasedWireframeModel_HeaderFile
