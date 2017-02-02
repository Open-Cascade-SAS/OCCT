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
#include <IGESDimen_CenterLine.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_CenterLine,IGESData_IGESEntity)

IGESDimen_CenterLine::IGESDimen_CenterLine ()    {  }


    void  IGESDimen_CenterLine::Init
  (const Standard_Integer aDataType,
   const Standard_Real aZdisp,
   const Handle(TColgp_HArray1OfXY)& dataPnts)
{
  if (dataPnts->Lower() != 1)
    throw Standard_DimensionMismatch("IGESDimen_CenterLine : Init");
  theDatatype      = aDataType;
  theZDisplacement = aZdisp;
  theDataPoints    = dataPnts;
  InitTypeAndForm(106,FormNumber());
// FormNumber :  20:points, 21:centres cercles
}

    void  IGESDimen_CenterLine::SetCrossHair (const Standard_Boolean mode)
{
  InitTypeAndForm(106, (mode ? 20 : 21));
}


    Standard_Integer  IGESDimen_CenterLine::Datatype () const 
{
  return theDatatype;
}

    Standard_Integer  IGESDimen_CenterLine::NbPoints () const 
{
  return theDataPoints->Length();
}

    Standard_Real  IGESDimen_CenterLine::ZDisplacement () const 
{
  return theZDisplacement;
}

    gp_Pnt  IGESDimen_CenterLine::Point (const Standard_Integer Index) const 
{
  gp_XY tempXY = theDataPoints->Value(Index);
  gp_Pnt point(tempXY.X(), tempXY.Y(), theZDisplacement);
  return point;
}

    gp_Pnt  IGESDimen_CenterLine::TransformedPoint
  (const Standard_Integer Index) const 
{
  gp_XY point2d = (theDataPoints->Value(Index));
  gp_XYZ point(point2d.X(), point2d.Y(), theZDisplacement);
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt(point);
}

    Standard_Boolean  IGESDimen_CenterLine::IsCrossHair () const 
{
  return(FormNumber() == 20);
}
