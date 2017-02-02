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

#include <IGESData_IGESEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDraw_ViewsVisible.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_ViewsVisible,IGESData_ViewKindEntity)

IGESDraw_ViewsVisible::IGESDraw_ViewsVisible ()    {  }


// This class inherits from IGESData_ViewKindEntity

    void IGESDraw_ViewsVisible::Init
  (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViewEntities,
   const Handle(IGESData_HArray1OfIGESEntity)&     allDisplayEntity)
{
  if (!allDisplayEntity.IsNull())
    if (allDisplayEntity->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDraw_ViewsVisible : Init");
  if (!allViewEntities.IsNull())
    if (allViewEntities->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDraw_ViewsVisible : Init");
  theViewEntities  = allViewEntities;
  theDisplayEntity = allDisplayEntity;
  InitTypeAndForm(402,3);
}

    void IGESDraw_ViewsVisible::InitImplied
  (const Handle(IGESData_HArray1OfIGESEntity)&     allDisplayEntity)
{
  if (!allDisplayEntity.IsNull())
    if (allDisplayEntity->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDraw_ViewsVisible : InitImplied");
  theDisplayEntity = allDisplayEntity;
}

    Standard_Boolean IGESDraw_ViewsVisible::IsSingle () const
{
  return Standard_False;
  // Is Redefined to return FALSE
}

    Standard_Integer IGESDraw_ViewsVisible::NbViews () const
{
  return (theViewEntities.IsNull() ? 0 : theViewEntities->Length());
}

    Standard_Integer IGESDraw_ViewsVisible::NbDisplayedEntities () const
{
  return (theDisplayEntity.IsNull() ? 0 : theDisplayEntity->Length());
  // return 0 if HArray1 theDisplayEntity is NULL Handle
}

    Handle(IGESData_ViewKindEntity) IGESDraw_ViewsVisible::ViewItem
  (const Standard_Integer Index) const
{
  return theViewEntities->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

    Handle(IGESData_IGESEntity) IGESDraw_ViewsVisible::DisplayedEntity
  (const Standard_Integer Index) const
{
  return theDisplayEntity->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
  // if (theDisplayEntity.IsNull()) then NoSuchObject Exception is raised
}
