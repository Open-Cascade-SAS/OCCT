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
