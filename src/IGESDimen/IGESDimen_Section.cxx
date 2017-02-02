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
#include <IGESData_LineFontEntity.hxx>
#include <IGESDimen_Section.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_Section,IGESData_IGESEntity)

IGESDimen_Section::IGESDimen_Section ()    {  }


    void  IGESDimen_Section::Init
  (const Standard_Integer dataType, const Standard_Real aDisp,
   const Handle(TColgp_HArray1OfXY)& dataPoints)
{
  if (dataPoints->Lower() != 1)
    throw Standard_DimensionMismatch("IGESDimen_Section : Init");
  theDatatype      = dataType;
  theZDisplacement = aDisp;
  theDataPoints    = dataPoints;
  InitTypeAndForm(106,FormNumber());
//  FormNumber  precises the type of Hatches  (31-38)
}


    void  IGESDimen_Section::SetFormNumber (const Standard_Integer form)
{
  if (form < 31 || form > 38) throw Standard_OutOfRange("IGESDimen_Section : SetFormNumber");
  InitTypeAndForm(106,form);
}


    Standard_Integer  IGESDimen_Section::Datatype () const 
{
  return theDatatype;
}

    Standard_Integer  IGESDimen_Section::NbPoints () const 
{
  return theDataPoints->Length();
}

    Standard_Real  IGESDimen_Section::ZDisplacement () const 
{
  return theZDisplacement;
}

    gp_Pnt IGESDimen_Section::Point (const Standard_Integer Index) const 
{
  gp_XY tempXY = theDataPoints->Value(Index);
  gp_Pnt point(tempXY.X(), tempXY.Y(), theZDisplacement);
  return point;
}

    gp_Pnt  IGESDimen_Section::TransformedPoint (const Standard_Integer Index) const 
{
  gp_XY point2d = theDataPoints->Value(Index);
  gp_XYZ point(point2d.X(), point2d.Y(), theZDisplacement);
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt(point);
}
