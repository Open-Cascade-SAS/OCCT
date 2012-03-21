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

#include <IGESDimen_CenterLine.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <gp_GTrsf.hxx>


IGESDimen_CenterLine::IGESDimen_CenterLine ()    {  }


    void  IGESDimen_CenterLine::Init
  (const Standard_Integer aDataType,
   const Standard_Real aZdisp,
   const Handle(TColgp_HArray1OfXY)& dataPnts)
{
  if (dataPnts->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESDimen_CenterLine : Init");
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
