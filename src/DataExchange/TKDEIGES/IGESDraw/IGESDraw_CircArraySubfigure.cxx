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
#include <gp_XYZ.hxx>
#include <IGESDraw_CircArraySubfigure.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_CircArraySubfigure, IGESData_IGESEntity)

IGESDraw_CircArraySubfigure::IGESDraw_CircArraySubfigure() {}

void IGESDraw_CircArraySubfigure::Init(const occ::handle<IGESData_IGESEntity>&      aBase,
                                       const int                                    aNumLocs,
                                       const gp_XYZ&                                aCenter,
                                       const double                                 aRadius,
                                       const double                                 aStAngle,
                                       const double                                 aDelAngle,
                                       const int                                    aFlag,
                                       const occ::handle<NCollection_HArray1<int>>& allNumPos)
{
  if (!allNumPos.IsNull())
    if (allNumPos->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDraw_CircArraySubfigure : Init");
  theBaseEntity  = aBase;
  theNbLocations = aNumLocs;
  theCenter      = aCenter;
  theRadius      = aRadius;
  theStartAngle  = aStAngle;
  theDeltaAngle  = aDelAngle;
  theDoDontFlag  = aFlag != 0;
  thePositions   = allNumPos;
  InitTypeAndForm(414, 0);
}

occ::handle<IGESData_IGESEntity> IGESDraw_CircArraySubfigure::BaseEntity() const
{
  return theBaseEntity;
}

int IGESDraw_CircArraySubfigure::NbLocations() const
{
  return theNbLocations;
}

gp_Pnt IGESDraw_CircArraySubfigure::CenterPoint() const
{
  gp_Pnt tempCenterPoint(theCenter);
  return tempCenterPoint;
}

gp_Pnt IGESDraw_CircArraySubfigure::TransformedCenterPoint() const
{
  gp_XYZ tempCenterPoint = theCenter;
  if (HasTransf())
    Location().Transforms(tempCenterPoint);
  gp_Pnt tempRes(tempCenterPoint);

  return tempRes;
}

double IGESDraw_CircArraySubfigure::CircleRadius() const
{
  return theRadius;
}

double IGESDraw_CircArraySubfigure::StartAngle() const
{
  return theStartAngle;
}

double IGESDraw_CircArraySubfigure::DeltaAngle() const
{
  return theDeltaAngle;
}

bool IGESDraw_CircArraySubfigure::DisplayFlag() const
{
  return (thePositions.IsNull());
  // if LC == 0 then there is no allocation made for thePositions array
  // i.e., thePositions == Null Handle
}

int IGESDraw_CircArraySubfigure::ListCount() const
{
  return (thePositions.IsNull() ? 0 : thePositions->Length());
  // Return 0 if HArray1 thePositions is NULL Handle
}

bool IGESDraw_CircArraySubfigure::DoDontFlag() const
{
  return theDoDontFlag;
}

bool IGESDraw_CircArraySubfigure::PositionNum(const int Index) const
{
  // Method : If thePositions array length is Zero return theDoDontFlag;
  //          else Search Index in to the Array. If 'Index' found in the
  //          array return theDoDontFlag else return !theDoDontFlag.

  if (thePositions.IsNull())
    return theDoDontFlag;

  else
  {
    int I;
    int up = thePositions->Upper();
    for (I = 1; I <= up; I++)
    {
      if (thePositions->Value(I) == Index)
        return theDoDontFlag;
    }
    return (!theDoDontFlag);
  }
}

int IGESDraw_CircArraySubfigure::ListPosition(const int Index) const
{
  return thePositions->Value(Index);
  // raise OutOfRange from Standard if Index is out-of-bound
  // Exception NoSuchObject will be raised if thePositions == Null Handle
}
