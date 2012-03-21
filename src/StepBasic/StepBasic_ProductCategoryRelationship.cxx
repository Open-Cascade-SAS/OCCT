// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepBasic_ProductCategoryRelationship.ixx>

//=======================================================================
//function : StepBasic_ProductCategoryRelationship
//purpose  : 
//=======================================================================

StepBasic_ProductCategoryRelationship::StepBasic_ProductCategoryRelationship ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ProductCategoryRelationship::Init (const Handle(TCollection_HAsciiString) &aName,
                                                  const Standard_Boolean hasDescription,
                                                  const Handle(TCollection_HAsciiString) &aDescription,
                                                  const Handle(StepBasic_ProductCategory) &aCategory,
                                                  const Handle(StepBasic_ProductCategory) &aSubCategory)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theCategory = aCategory;

  theSubCategory = aSubCategory;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ProductCategoryRelationship::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_ProductCategoryRelationship::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ProductCategoryRelationship::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_ProductCategoryRelationship::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_ProductCategoryRelationship::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : Category
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductCategory) StepBasic_ProductCategoryRelationship::Category () const
{
  return theCategory;
}

//=======================================================================
//function : SetCategory
//purpose  : 
//=======================================================================

void StepBasic_ProductCategoryRelationship::SetCategory (const Handle(StepBasic_ProductCategory) &aCategory)
{
  theCategory = aCategory;
}

//=======================================================================
//function : SubCategory
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductCategory) StepBasic_ProductCategoryRelationship::SubCategory () const
{
  return theSubCategory;
}

//=======================================================================
//function : SetSubCategory
//purpose  : 
//=======================================================================

void StepBasic_ProductCategoryRelationship::SetSubCategory (const Handle(StepBasic_ProductCategory) &aSubCategory)
{
  theSubCategory = aSubCategory;
}
