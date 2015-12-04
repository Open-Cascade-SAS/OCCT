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

#include <IGESGraph_DrawingSize.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_DrawingSize,IGESData_IGESEntity)

IGESGraph_DrawingSize::IGESGraph_DrawingSize ()    {  }


    void IGESGraph_DrawingSize::Init
  (const Standard_Integer nbProps, const Standard_Real aXSize, 
   const Standard_Real    aYSize)
{
  theNbPropertyValues = nbProps;
  theXSize            = aXSize;
  theYSize            = aYSize;
  InitTypeAndForm(406,16);
}


    Standard_Integer IGESGraph_DrawingSize::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real IGESGraph_DrawingSize::XSize () const
{
  return theXSize;
}

    Standard_Real IGESGraph_DrawingSize::YSize () const
{
  return theYSize;
}
