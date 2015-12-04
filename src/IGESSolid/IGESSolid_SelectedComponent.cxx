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

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESSolid_BooleanTree.hxx>
#include <IGESSolid_SelectedComponent.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESSolid_SelectedComponent,IGESData_IGESEntity)

IGESSolid_SelectedComponent::IGESSolid_SelectedComponent ()    {  }


    void  IGESSolid_SelectedComponent::Init
  (const Handle(IGESSolid_BooleanTree)& anEntity, const gp_XYZ& SelectPnt)
{
  theEntity      = anEntity;
  theSelectPoint = SelectPnt;
  InitTypeAndForm(182,0);
}

    Handle(IGESSolid_BooleanTree)  IGESSolid_SelectedComponent::Component () const
{
  return theEntity;
}

    gp_Pnt  IGESSolid_SelectedComponent::SelectPoint () const
{
  return gp_Pnt(theSelectPoint);
}

    gp_Pnt  IGESSolid_SelectedComponent::TransformedSelectPoint () const
{
  if (!HasTransf()) return gp_Pnt(theSelectPoint);
  else
    {
      gp_XYZ tmp = theSelectPoint;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}
