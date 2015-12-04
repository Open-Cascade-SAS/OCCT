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

#include <IGESBasic_SubfigureDef.hxx>
#include <IGESGraph_LineFontDefTemplate.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_LineFontDefTemplate,IGESData_LineFontEntity)

IGESGraph_LineFontDefTemplate::IGESGraph_LineFontDefTemplate ()    {  }


// This class inherits from IGESData_LineFontEntity

    void IGESGraph_LineFontDefTemplate::Init
  (const Standard_Integer                anOrientation,
   const Handle(IGESBasic_SubfigureDef)& aTemplate,
   const Standard_Real                   aDistance,
   const Standard_Real                   aScale)
{
  theOrientation    = anOrientation;
  theTemplateEntity = aTemplate;
  theDistance       = aDistance;
  theScale          = aScale;
  InitTypeAndForm(304,1);
}

    Standard_Integer IGESGraph_LineFontDefTemplate::Orientation () const
{
  return theOrientation;
}

    Handle(IGESBasic_SubfigureDef) IGESGraph_LineFontDefTemplate::TemplateEntity
  () const
{
  return theTemplateEntity;
}

    Standard_Real IGESGraph_LineFontDefTemplate::Distance () const
{
  return theDistance;
}

    Standard_Real IGESGraph_LineFontDefTemplate::Scale () const
{
  return theScale;
}
