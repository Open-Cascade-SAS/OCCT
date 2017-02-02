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

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_View.hxx>
#include <IGESGraph_DrawingSize.hxx>
#include <IGESGraph_DrawingUnits.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_DrawingWithRotation,IGESData_IGESEntity)

IGESDraw_DrawingWithRotation::IGESDraw_DrawingWithRotation ()    {  }


    void IGESDraw_DrawingWithRotation::Init
  (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViews,
   const Handle(TColgp_HArray1OfXY)&               allViewOrigins,
   const Handle(TColStd_HArray1OfReal)&            allOrientationAngles,
   const Handle(IGESData_HArray1OfIGESEntity)&     allAnnotations)
{
  Standard_Integer Len  = allViews->Length();
  if ( allViews->Lower() != 1 ||
      (allViewOrigins->Lower() != 1 || allViewOrigins->Length() != Len) ||
      (allOrientationAngles->Lower() != 1 || allOrientationAngles->Length() != Len) )
    throw Standard_DimensionMismatch("IGESDraw_DrawingWithRotation : Init");
  if (!allAnnotations.IsNull())
    if (allAnnotations->Lower() != 1) throw Standard_DimensionMismatch("IGESDraw_DrawingWithRotation : Init");

  theViews             = allViews; 
  theViewOrigins       = allViewOrigins; 
  theOrientationAngles = allOrientationAngles; 
  theAnnotations       = allAnnotations; 
  InitTypeAndForm(404,1);
}

    Standard_Integer IGESDraw_DrawingWithRotation::NbViews () const
{
  return (theViews->Length());
}

    Handle(IGESData_ViewKindEntity) IGESDraw_DrawingWithRotation::ViewItem
  (const Standard_Integer Index) const
{
  return (theViews->Value(Index));
}

    gp_Pnt2d IGESDraw_DrawingWithRotation::ViewOrigin
  (const Standard_Integer Index) const
{
  return ( gp_Pnt2d (theViewOrigins->Value(Index)) );
}

    Standard_Real IGESDraw_DrawingWithRotation::OrientationAngle
  (const Standard_Integer Index) const
{
  return ( theOrientationAngles->Value(Index) );
}

    Standard_Integer IGESDraw_DrawingWithRotation::NbAnnotations () const
{
  return (theAnnotations.IsNull() ? 0 : theAnnotations->Length() );
}

    Handle(IGESData_IGESEntity) IGESDraw_DrawingWithRotation::Annotation
  (const Standard_Integer Index) const
{
  return ( theAnnotations->Value(Index) );
}

    gp_XY IGESDraw_DrawingWithRotation::ViewToDrawing
  (const Standard_Integer NumView, const gp_XYZ& ViewCoords) const
{
  gp_XY         thisOrigin       = theViewOrigins->Value(NumView);
  Standard_Real XOrigin          = thisOrigin.X();
  Standard_Real YOrigin          = thisOrigin.Y();
  Standard_Real theScaleFactor=0.;

  Handle(IGESData_ViewKindEntity) tempView = theViews->Value(NumView);
  if (tempView->IsKind(STANDARD_TYPE(IGESDraw_View)))
    {
      DeclareAndCast(IGESDraw_View, thisView, tempView);
      theScaleFactor   = thisView->ScaleFactor();
    }
  else if (tempView->IsKind(STANDARD_TYPE(IGESDraw_PerspectiveView)))
    {
      DeclareAndCast(IGESDraw_PerspectiveView, thisView, tempView);
      theScaleFactor   = thisView->ScaleFactor();
    }

  Standard_Real XV               = ViewCoords.X();
  Standard_Real YV               = ViewCoords.Y();

  Standard_Real theta            = theOrientationAngles->Value(NumView);

  Standard_Real XD = 
    XOrigin + theScaleFactor * ( XV * Cos(theta) - YV * Sin(theta) );
  Standard_Real YD = 
    YOrigin + theScaleFactor * ( XV * Sin(theta) + YV * Cos(theta) );

  return ( gp_XY(XD, YD) );
}


    Standard_Boolean  IGESDraw_DrawingWithRotation::DrawingUnit
  (Standard_Real& val) const
{
  val = 0.;
  Handle(Standard_Type) typunit = STANDARD_TYPE(IGESGraph_DrawingUnits);
  if (NbTypedProperties(typunit) != 1) return Standard_False;
  DeclareAndCast(IGESGraph_DrawingUnits,units,TypedProperty(typunit)); 
  if (units.IsNull()) return Standard_False;
  val = units->UnitValue();
  return Standard_True;
}

    Standard_Boolean  IGESDraw_DrawingWithRotation::DrawingSize
  (Standard_Real& X, Standard_Real& Y) const
{
  X = Y = 0.;
  Handle(Standard_Type) typsize = STANDARD_TYPE(IGESGraph_DrawingSize);
  if (NbTypedProperties(typsize) != 1) return Standard_False;
  DeclareAndCast(IGESGraph_DrawingSize,size,TypedProperty(typsize)); 
  if (size.IsNull()) return Standard_False;
  X = size->XSize();  Y = size->YSize();
  return Standard_True;
}
