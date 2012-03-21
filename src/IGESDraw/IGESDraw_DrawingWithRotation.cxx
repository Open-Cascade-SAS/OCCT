// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDraw_DrawingWithRotation.ixx>
#include <IGESDraw_View.hxx>
#include <IGESDraw_PerspectiveView.hxx>

#include <IGESGraph_DrawingUnits.hxx>
#include <IGESGraph_DrawingSize.hxx>

#include <Interface_Macros.hxx>


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
    Standard_DimensionMismatch::Raise
      ("IGESDraw_DrawingWithRotation : Init");
  if (!allAnnotations.IsNull())
    if (allAnnotations->Lower() != 1) Standard_DimensionMismatch::Raise
      ("IGESDraw_DrawingWithRotation : Init");

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
