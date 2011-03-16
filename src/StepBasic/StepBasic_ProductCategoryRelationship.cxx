// File:	StepBasic_ProductCategoryRelationship.cxx
// Created:	Fri Nov 26 16:26:38 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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
