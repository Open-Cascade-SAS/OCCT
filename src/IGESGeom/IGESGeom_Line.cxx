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
#include <IGESGeom_Line.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_Line,IGESData_IGESEntity)

IGESGeom_Line::IGESGeom_Line ()    {  }


    void IGESGeom_Line::Init
  (const gp_XYZ& aStart, const gp_XYZ& anEnd)
{
  theStart = aStart;
  theEnd   = anEnd;
  InitTypeAndForm(110,0);
}

    Standard_Integer  IGESGeom_Line::Infinite () const
      {  return FormNumber();  }

    void  IGESGeom_Line::SetInfinite (const Standard_Integer status)
      {  if (status >= 0 && status <= 2) InitTypeAndForm(110,status);  }


    gp_Pnt IGESGeom_Line::StartPoint () const
{
  gp_Pnt start(theStart);
  return start;
}

    gp_Pnt IGESGeom_Line::TransformedStartPoint () const
{
  gp_XYZ Start = theStart;
  if (HasTransf()) Location().Transforms(Start);
  gp_Pnt transStart(Start);
  return transStart;
}

    gp_Pnt IGESGeom_Line::EndPoint () const
{
  gp_Pnt end(theEnd);
  return end;
}

    gp_Pnt IGESGeom_Line::TransformedEndPoint () const
{
  gp_XYZ End = theEnd;
  if (HasTransf()) Location().Transforms(End);
  gp_Pnt transEnd(End);
  return transEnd;
}
