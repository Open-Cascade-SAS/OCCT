//--------------------------------------------------------------------
//
//  File Name : IGESDraw_Drawing.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDraw_Drawing.ixx>
#include <IGESDraw_View.hxx>
#include <IGESDraw_PerspectiveView.hxx>

#include <IGESGraph_DrawingUnits.hxx>
#include <IGESGraph_DrawingSize.hxx>

#include <Interface_Macros.hxx>


    IGESDraw_Drawing::IGESDraw_Drawing ()    {  }


    void IGESDraw_Drawing::Init
  (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViews,
   const Handle(TColgp_HArray1OfXY)&               allViewOrigins,
   const Handle(IGESData_HArray1OfIGESEntity)&     allAnnotations)
{
  if (!allViews.IsNull()) {
    Standard_Integer Len  = allViews->Length();
    Standard_Boolean Flag = ( allViewOrigins->Length() == Len );
    if (!Flag || allViews->Lower() != 1 || allViewOrigins->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDraw_Drawing : Init");
  }
  if (!allAnnotations.IsNull())
    if (allAnnotations->Lower() != 1) Standard_DimensionMismatch::Raise
      ("IGESDraw_Drawing : Init");

  theViews         = allViews; 
  theViewOrigins   = allViewOrigins; 
  theAnnotations   = allAnnotations;
  InitTypeAndForm(404,0);
}

    Standard_Integer IGESDraw_Drawing::NbViews () const
{
  return (theViews.IsNull() ? 0 : theViews->Length());
}

    Handle(IGESData_ViewKindEntity) IGESDraw_Drawing::ViewItem
  (const Standard_Integer ViewIndex) const
{
  return theViews->Value(ViewIndex);
}

    gp_Pnt2d IGESDraw_Drawing::ViewOrigin
  (const Standard_Integer TViewIndex) const
{
  return (gp_Pnt2d (theViewOrigins->Value(TViewIndex)) );
}

    Standard_Integer IGESDraw_Drawing::NbAnnotations () const
{
  return (theAnnotations.IsNull() ? 0 : theAnnotations->Length() );
}

    Handle(IGESData_IGESEntity) IGESDraw_Drawing::Annotation
  (const Standard_Integer AnnotationIndex) const
{
  return ( theAnnotations->Value(AnnotationIndex) );
}

    gp_XY IGESDraw_Drawing::ViewToDrawing
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

  Standard_Real XD = XOrigin + (theScaleFactor * XV);
  Standard_Real YD = YOrigin + (theScaleFactor * YV);

  return ( gp_XY(XD, YD) );
}


    Standard_Boolean  IGESDraw_Drawing::DrawingUnit (Standard_Real& val) const
{
  val = 0.;
  Handle(Standard_Type) typunit = STANDARD_TYPE(IGESGraph_DrawingUnits);
  if (NbTypedProperties(typunit) != 1) return Standard_False;
  DeclareAndCast(IGESGraph_DrawingUnits,units,TypedProperty(typunit)); 
  if (units.IsNull()) return Standard_False;
  val = units->UnitValue();
  return Standard_True;
}

    Standard_Boolean  IGESDraw_Drawing::DrawingSize
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
