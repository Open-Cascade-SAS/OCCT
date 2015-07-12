// Created on: 1998-07-29
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _STEPEdit_HeaderFile
#define _STEPEdit_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Interface_Protocol;
class StepData_StepModel;
class IFSelect_Signature;
class IFSelect_SelectSignature;
class STEPEdit_EditContext;
class STEPEdit_EditSDR;


//! Provides tools to exploit and edit a set of STEP data :
//! editors, selections ..
class STEPEdit 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns a Protocol fit for STEP (creates the first time)
  Standard_EXPORT static Handle(Interface_Protocol) Protocol();
  
  //! Returns a new empty StepModel fit for STEP
  //! i.e. with its header determined from Protocol
  Standard_EXPORT static Handle(StepData_StepModel) NewModel();
  
  //! Returns a SignType fit for STEP (creates the first time)
  Standard_EXPORT static Handle(IFSelect_Signature) SignType();
  
  //! Creates a Selection for ShapeDefinitionRepresentation
  //! By default searches among root entities
  Standard_EXPORT static Handle(IFSelect_SelectSignature) NewSelectSDR();
  
  //! Creates a Selection for Placed Items, i.e. MappedItem or
  //! ContextDependentShapeRepresentation, which itself refers to a
  //! RepresentationRelationship with possible subtypes (Shape...
  //! and/or ...WithTransformation)
  //! By default in the whole StepModel
  Standard_EXPORT static Handle(IFSelect_SelectSignature) NewSelectPlacedItem();
  
  //! Creates a Selection for ShapeRepresentation and its sub-types,
  //! plus ContextDependentShapeRepresentation (which is not a
  //! sub-type of ShapeRepresentation)
  //! By default in the whole StepModel
  Standard_EXPORT static Handle(IFSelect_SelectSignature) NewSelectShapeRepr();




protected:





private:




friend class STEPEdit_EditContext;
friend class STEPEdit_EditSDR;

};







#endif // _STEPEdit_HeaderFile
