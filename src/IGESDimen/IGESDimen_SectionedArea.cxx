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

#include <IGESDimen_SectionedArea.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <gp_GTrsf.hxx>


IGESDimen_SectionedArea::IGESDimen_SectionedArea ()    {  }


    void IGESDimen_SectionedArea::Init
  (const Handle(IGESData_IGESEntity)& aCurve,
   const Standard_Integer aPattern,
   const gp_XYZ& aPoint,
   const Standard_Real aDistance,
   const Standard_Real anAngle,
   const Handle(IGESData_HArray1OfIGESEntity)& someIslands)
{
  if (!someIslands.IsNull())
    if (someIslands->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDimen_SectionedArea : Init");
  theExteriorCurve = aCurve;
  thePattern       = aPattern;
  thePassingPoint  = aPoint;
  theDistance      = aDistance;
  theAngle         = anAngle;
  theIslandCurves  = someIslands;
  InitTypeAndForm(230,FormNumber());
}

    void IGESDimen_SectionedArea::SetInverted (const Standard_Boolean mode)
{
  InitTypeAndForm (230, (mode ? 1 : 0));
}

    Standard_Boolean IGESDimen_SectionedArea::IsInverted () const
{
  return (FormNumber() != 0);
}


    Handle(IGESData_IGESEntity) IGESDimen_SectionedArea::ExteriorCurve () const
{
  return theExteriorCurve;
}

    Standard_Integer IGESDimen_SectionedArea::Pattern () const
{
  return thePattern;
}

    gp_Pnt IGESDimen_SectionedArea::PassingPoint () const
{
  return gp_Pnt(thePassingPoint);
}

    gp_Pnt IGESDimen_SectionedArea::TransformedPassingPoint () const
{
  gp_XYZ tmpXYZ(thePassingPoint);
  if (HasTransf()) Location().Transforms(tmpXYZ);
  return gp_Pnt(tmpXYZ);
}

    Standard_Real IGESDimen_SectionedArea::ZDepth () const
{
  return thePassingPoint.Z();
}

    Standard_Real IGESDimen_SectionedArea::Distance () const
{
  return theDistance;
}

    Standard_Real IGESDimen_SectionedArea::Angle () const
{
  return theAngle;
}

    Standard_Integer IGESDimen_SectionedArea::NbIslands () const
{
  return (theIslandCurves.IsNull() ? 0 : theIslandCurves->Length());
}

    Handle(IGESData_IGESEntity) IGESDimen_SectionedArea::IslandCurve
  (const Standard_Integer num) const
{
  return  theIslandCurves->Value(num);
}
