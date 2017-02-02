// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_SingleParent.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_SingleParent,IGESData_SingleParentEntity)

IGESBasic_SingleParent::IGESBasic_SingleParent ()    {  }


    void  IGESBasic_SingleParent::Init
  (const Standard_Integer nbParentEntities,
   const Handle(IGESData_IGESEntity)& aParentEntity,
   const Handle(IGESData_HArray1OfIGESEntity)& allChildren)
{
  if (!allChildren.IsNull() && allChildren->Lower() != 1)
    throw Standard_DimensionMismatch("IGESBasic_SingleParent : Init");
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

