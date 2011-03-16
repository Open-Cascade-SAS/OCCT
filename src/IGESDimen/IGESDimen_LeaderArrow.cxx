//--------------------------------------------------------------------
//
//  File Name : IGESDimen_LeaderArrow.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_LeaderArrow.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <gp_GTrsf.hxx>


    IGESDimen_LeaderArrow::IGESDimen_LeaderArrow ()    {  }

    void  IGESDimen_LeaderArrow::Init
  (const Standard_Real height, const Standard_Real width,
   const Standard_Real depth,  const gp_XY&        position,
   const Handle(TColgp_HArray1OfXY)& segments)
{
  if (segments->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDimen_LeaderArrow : Init");
  theArrowHeadHeight = height;
  theArrowHeadWidth  = width;
  theZDepth          = depth ;
  theArrowHead       = position;
  theSegmentTails    = segments;
  InitTypeAndForm(214,FormNumber());
// FormNumber precises the Type of the Arrow (1-12)
}

    void  IGESDimen_LeaderArrow::SetFormNumber (const Standard_Integer form)
{
  if (form < 1 || form > 12) Standard_OutOfRange::Raise
    ("IGESDimen_LeaderArrow : SetFormNumber");
  InitTypeAndForm(214,form);
}

    Standard_Integer  IGESDimen_LeaderArrow::NbSegments () const 
{
  return theSegmentTails->Length();
}

    Standard_Real  IGESDimen_LeaderArrow::ArrowHeadHeight () const 
{
  return theArrowHeadHeight;
}

    Standard_Real  IGESDimen_LeaderArrow::ArrowHeadWidth () const 
{
  return theArrowHeadWidth;
}

    Standard_Real  IGESDimen_LeaderArrow::ZDepth () const 
{
  return theZDepth;
}

    gp_Pnt2d  IGESDimen_LeaderArrow::ArrowHead () const 
{
  gp_Pnt2d AHPnt2d(theArrowHead);
  return AHPnt2d;
}

    gp_Pnt  IGESDimen_LeaderArrow::TransformedArrowHead () const 
{
  gp_XYZ point(theArrowHead.X(), theArrowHead.Y(), ZDepth());
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt(point);
}

    gp_Pnt2d  IGESDimen_LeaderArrow::SegmentTail
  (const Standard_Integer Index) const 
{
  gp_Pnt2d STPnt2d(theSegmentTails->Value(Index));
  return STPnt2d;
}

    gp_Pnt  IGESDimen_LeaderArrow::TransformedSegmentTail
  (const Standard_Integer Index) const 
{
  gp_XY point2d = theSegmentTails->Value(Index);
  gp_XYZ point(point2d.X(), point2d.Y(), ZDepth());
  if (HasTransf()) Location().Transforms(point);
  return gp_Pnt(point);
}
