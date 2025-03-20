// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepBasic_ProductCategory.hxx>
#include <StepBasic_ProductCategoryRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductCategoryRelationship, Standard_Transient)

//=================================================================================================

StepBasic_ProductCategoryRelationship::StepBasic_ProductCategoryRelationship()
{
  defDescription = Standard_False;
}

//=================================================================================================

void StepBasic_ProductCategoryRelationship::Init(
  const Handle(TCollection_HAsciiString)&  aName,
  const Standard_Boolean                   hasDescription,
  const Handle(TCollection_HAsciiString)&  aDescription,
  const Handle(StepBasic_ProductCategory)& aCategory,
  const Handle(StepBasic_ProductCategory)& aSubCategory)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();

  theCategory = aCategory;

  theSubCategory = aSubCategory;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_ProductCategoryRelationship::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_ProductCategoryRelationship::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_ProductCategoryRelationship::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_ProductCategoryRelationship::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Standard_Boolean StepBasic_ProductCategoryRelationship::HasDescription() const
{
  return defDescription;
}

//=================================================================================================

Handle(StepBasic_ProductCategory) StepBasic_ProductCategoryRelationship::Category() const
{
  return theCategory;
}

//=================================================================================================

void StepBasic_ProductCategoryRelationship::SetCategory(
  const Handle(StepBasic_ProductCategory)& aCategory)
{
  theCategory = aCategory;
}

//=================================================================================================

Handle(StepBasic_ProductCategory) StepBasic_ProductCategoryRelationship::SubCategory() const
{
  return theSubCategory;
}

//=================================================================================================

void StepBasic_ProductCategoryRelationship::SetSubCategory(
  const Handle(StepBasic_ProductCategory)& aSubCategory)
{
  theSubCategory = aSubCategory;
}
