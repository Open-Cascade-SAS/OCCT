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

#include <IGESDraw_CircArraySubfigure.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <gp_GTrsf.hxx>


IGESDraw_CircArraySubfigure::IGESDraw_CircArraySubfigure ()    {  }


    void IGESDraw_CircArraySubfigure::Init
  (const Handle(IGESData_IGESEntity)&      aBase,
   const Standard_Integer                  aNumLocs,
   const gp_XYZ&                           aCenter,
   const Standard_Real                     aRadius,
   const Standard_Real                     aStAngle,
   const Standard_Real                     aDelAngle,
   const Standard_Integer                  aFlag,
   const Handle(TColStd_HArray1OfInteger)& allNumPos)
{
  if (!allNumPos.IsNull())
    if (allNumPos->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDraw_CircArraySubfigure : Init");
  theBaseEntity  = aBase;
  theNbLocations = aNumLocs;
  theCenter      = aCenter;
  theRadius      = aRadius;
  theStartAngle  = aStAngle;
  theDeltaAngle  = aDelAngle;
  theDoDontFlag  = aFlag;
  thePositions   = allNumPos;
  InitTypeAndForm(414,0);
}

    Handle(IGESData_IGESEntity) IGESDraw_CircArraySubfigure::BaseEntity () const
{
  return theBaseEntity;
}

    Standard_Integer IGESDraw_CircArraySubfigure::NbLocations () const
{
  return theNbLocations;
}

    gp_Pnt IGESDraw_CircArraySubfigure::CenterPoint () const
{
  gp_Pnt tempCenterPoint(theCenter);
  return tempCenterPoint;
}

    gp_Pnt IGESDraw_CircArraySubfigure::TransformedCenterPoint () const
{
  gp_XYZ tempCenterPoint = theCenter;
  if (HasTransf()) Location().Transforms(tempCenterPoint);
  gp_Pnt tempRes(tempCenterPoint);

  return tempRes;
}

    Standard_Real IGESDraw_CircArraySubfigure::CircleRadius () const
{
  return theRadius;
}

    Standard_Real IGESDraw_CircArraySubfigure::StartAngle () const
{
  return theStartAngle;
}

    Standard_Real IGESDraw_CircArraySubfigure::DeltaAngle () const
{
  return theDeltaAngle;
}

    Standard_Boolean IGESDraw_CircArraySubfigure::DisplayFlag () const
{
  return (thePositions.IsNull());
  // if LC == 0 then there is no allocation made for thePositions array
  // i.e., thePositions == Null Handle
}

    Standard_Integer IGESDraw_CircArraySubfigure::ListCount () const
{
  return ( thePositions.IsNull() ? 0 : thePositions->Length() );
  // Return 0 if HArray1 thePositions is NULL Handle
}

    Standard_Boolean IGESDraw_CircArraySubfigure::DoDontFlag () const
{
  return (theDoDontFlag == 1);
}

    Standard_Boolean IGESDraw_CircArraySubfigure::PositionNum
  (const Standard_Integer Index) const
{
  // Method : If thePositions array length is Zero return theDoDontFlag;
  //          else Search Index in to the Array. If 'Index' found in the
  //          array return theDoDontFlag else return !theDoDontFlag.

  if (thePositions.IsNull())        return theDoDontFlag;

  else {
    Standard_Integer I;
    Standard_Integer up  = thePositions->Upper();
    for (I = 1; I <= up; I ++) {
      if (thePositions->Value(I) == Index)     return theDoDontFlag;
    }
    return (! theDoDontFlag);
  }
}

    Standard_Integer IGESDraw_CircArraySubfigure::ListPosition
  (const Standard_Integer Index) const
{
  return thePositions->Value(Index);
  // raise OutOfRange from Standard if Index is out-of-bound
  // Exception NoSuchObject will be raised if thePositions == Null Handle
}
