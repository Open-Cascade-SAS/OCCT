// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_SingleParent.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESBasic_SingleParent::IGESBasic_SingleParent ()    {  }


    void  IGESBasic_SingleParent::Init
  (const Standard_Integer nbParentEntities,
   const Handle(IGESData_IGESEntity)& aParentEntity,
   const Handle(IGESData_HArray1OfIGESEntity)& allChildren)
{
  if (!allChildren.IsNull() && allChildren->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESBasic_SingleParent : Init");
  theParentEntity     = aParentEntity;
  theChildren         = allChildren;
  theNbParentEntities = nbParentEntities;
  InitTypeAndForm(402,9);
}


    Standard_Integer  IGESBasic_SingleParent::NbChildren () const
{
  return(theChildren.IsNull() ? 0 : theChildren->Length());
}

    Handle(IGESData_IGESEntity)  IGESBasic_SingleParent::Child
  (const Standard_Integer Index) const
{
  return theChildren->Value(Index);
}

    Standard_Integer  IGESBasic_SingleParent::NbParentEntities () const
{
  return theNbParentEntities;
}
    Handle(IGESData_IGESEntity)  IGESBasic_SingleParent::SingleParent () const
{  return theParentEntity;  }

