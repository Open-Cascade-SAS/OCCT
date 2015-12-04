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

#include <IGESGraph_IntercharacterSpacing.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_IntercharacterSpacing,IGESData_IGESEntity)

IGESGraph_IntercharacterSpacing::IGESGraph_IntercharacterSpacing ()    {  }

    void IGESGraph_IntercharacterSpacing::Init
  (const Standard_Integer nbProps, const Standard_Real anISpace)
{
  theNbPropertyValues = nbProps;
  theISpace           = anISpace;
  InitTypeAndForm(406,18);
}

    Standard_Integer IGESGraph_IntercharacterSpacing::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real IGESGraph_IntercharacterSpacing::ISpace () const
{
  return theISpace;
}
