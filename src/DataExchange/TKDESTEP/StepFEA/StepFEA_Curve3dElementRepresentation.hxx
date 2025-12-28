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
#include <StepRepr_RepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepFEA_NodeRepresentation.hxx>
class StepFEA_FeaModel3d;
class StepElement_Curve3dElementDescriptor;
class StepFEA_Curve3dElementProperty;
class StepElement_ElementMaterial;
class TCollection_HAsciiString;
class StepRepr_RepresentationContext;

//! Representation of STEP entity Curve3dElementRepresentation
class StepFEA_Curve3dElementRepresentation : public StepFEA_ElementRepresentation
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_Curve3dElementRepresentation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<TCollection_HAsciiString>& aRepresentation_Name,
    const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>&
                                                       aRepresentation_Items,
    const occ::handle<StepRepr_RepresentationContext>& aRepresentation_ContextOfItems,
    const occ::handle<NCollection_HArray1<occ::handle<StepFEA_NodeRepresentation>>>&
                                                             aElementRepresentation_NodeList,
    const occ::handle<StepFEA_FeaModel3d>&                   aModelRef,
    const occ::handle<StepElement_Curve3dElementDescriptor>& aElementDescriptor,
    const occ::handle<StepFEA_Curve3dElementProperty>&       aProperty,
    const occ::handle<StepElement_ElementMaterial>&          aMaterial);

  //! Returns field ModelRef
  Standard_EXPORT occ::handle<StepFEA_FeaModel3d> ModelRef() const;

  //! Set field ModelRef
  Standard_EXPORT void SetModelRef(const occ::handle<StepFEA_FeaModel3d>& ModelRef);

  //! Returns field ElementDescriptor
  Standard_EXPORT occ::handle<StepElement_Curve3dElementDescriptor> ElementDescriptor() const;

  //! Set field ElementDescriptor
  Standard_EXPORT void SetElementDescriptor(
    const occ::handle<StepElement_Curve3dElementDescriptor>& ElementDescriptor);

  //! Returns field Property
  Standard_EXPORT occ::handle<StepFEA_Curve3dElementProperty> Property() const;

  //! Set field Property
  Standard_EXPORT void SetProperty(const occ::handle<StepFEA_Curve3dElementProperty>& Property);

  //! Returns field Material
  Standard_EXPORT occ::handle<StepElement_ElementMaterial> Material() const;

  //! Set field Material
  Standard_EXPORT void SetMaterial(const occ::handle<StepElement_ElementMaterial>& Material);

  DEFINE_STANDARD_RTTIEXT(StepFEA_Curve3dElementRepresentation, StepFEA_ElementRepresentation)

private:
  occ::handle<StepFEA_FeaModel3d>                   theModelRef;
  occ::handle<StepElement_Curve3dElementDescriptor> theElementDescriptor;
  occ::handle<StepFEA_Curve3dElementProperty>       theProperty;
  occ::handle<StepElement_ElementMaterial>          theMaterial;
};

#endif // _StepFEA_Curve3dElementRepresentation_HeaderFile
