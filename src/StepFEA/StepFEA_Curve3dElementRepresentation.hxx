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

#ifndef _StepFEA_Curve3dElementRepresentation_HeaderFile
#define _StepFEA_Curve3dElementRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepFEA_ElementRepresentation.hxx>
#include <StepRepr_HArray1OfRepresentationItem.hxx>
#include <StepFEA_HArray1OfNodeRepresentation.hxx>
class StepFEA_FeaModel3d;
class StepElement_Curve3dElementDescriptor;
class StepFEA_Curve3dElementProperty;
class StepElement_ElementMaterial;
class TCollection_HAsciiString;
class StepRepr_RepresentationContext;


class StepFEA_Curve3dElementRepresentation;
DEFINE_STANDARD_HANDLE(StepFEA_Curve3dElementRepresentation, StepFEA_ElementRepresentation)

//! Representation of STEP entity Curve3dElementRepresentation
class StepFEA_Curve3dElementRepresentation : public StepFEA_ElementRepresentation
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_Curve3dElementRepresentation();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aRepresentation_Name, const Handle(StepRepr_HArray1OfRepresentationItem)& aRepresentation_Items, const Handle(StepRepr_RepresentationContext)& aRepresentation_ContextOfItems, const Handle(StepFEA_HArray1OfNodeRepresentation)& aElementRepresentation_NodeList, const Handle(StepFEA_FeaModel3d)& aModelRef, const Handle(StepElement_Curve3dElementDescriptor)& aElementDescriptor, const Handle(StepFEA_Curve3dElementProperty)& aProperty, const Handle(StepElement_ElementMaterial)& aMaterial);
  
  //! Returns field ModelRef
  Standard_EXPORT Handle(StepFEA_FeaModel3d) ModelRef() const;
  
  //! Set field ModelRef
  Standard_EXPORT void SetModelRef (const Handle(StepFEA_FeaModel3d)& ModelRef);
  
  //! Returns field ElementDescriptor
  Standard_EXPORT Handle(StepElement_Curve3dElementDescriptor) ElementDescriptor() const;
  
  //! Set field ElementDescriptor
  Standard_EXPORT void SetElementDescriptor (const Handle(StepElement_Curve3dElementDescriptor)& ElementDescriptor);
  
  //! Returns field Property
  Standard_EXPORT Handle(StepFEA_Curve3dElementProperty) Property() const;
  
  //! Set field Property
  Standard_EXPORT void SetProperty (const Handle(StepFEA_Curve3dElementProperty)& Property);
  
  //! Returns field Material
  Standard_EXPORT Handle(StepElement_ElementMaterial) Material() const;
  
  //! Set field Material
  Standard_EXPORT void SetMaterial (const Handle(StepElement_ElementMaterial)& Material);




  DEFINE_STANDARD_RTTIEXT(StepFEA_Curve3dElementRepresentation,StepFEA_ElementRepresentation)

protected:




private:


  Handle(StepFEA_FeaModel3d) theModelRef;
  Handle(StepElement_Curve3dElementDescriptor) theElementDescriptor;
  Handle(StepFEA_Curve3dElementProperty) theProperty;
  Handle(StepElement_ElementMaterial) theMaterial;


};







#endif // _StepFEA_Curve3dElementRepresentation_HeaderFile
