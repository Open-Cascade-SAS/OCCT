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

#ifndef _Select3D_PointData_HeaderFile
#define _Select3D_PointData_HeaderFile

#include <Select3D_Pnt.hxx>
#include <Select3D_Pnt2d.hxx>

// A framework for safe management of Select3D_SensitivePoly polygons of 3D and 2D points
class Select3D_PointData {

public:

  // Constructs internal arrays of 2D and 3D points defined
  // by number of points theNbPoints
  Select3D_PointData (const Standard_Integer theNbPoints)
  {
    if (theNbPoints <= 0)
      Standard_ConstructionError::Raise("Select3D_PointData");

    mynbpoints = theNbPoints;
    mypolyg3d = new Select3D_Pnt[mynbpoints];
    mypolyg2d = new Select3D_Pnt2d[mynbpoints];
  }

  // Destructor
  ~Select3D_PointData ()
  {
    delete [] mypolyg3d;
    delete [] mypolyg2d;
  }

  // Sets Select3D_Pnt to internal array
  // of 3D points if theIndex is valid
  void SetPnt (const Standard_Integer theIndex,
               const Select3D_Pnt& theValue)
  {
    if (theIndex < 0 || theIndex >= mynbpoints)
      Standard_OutOfRange::Raise("Select3D_PointData::SetPnt");
    mypolyg3d[theIndex] = theValue;
  }

  // Sets gp_Pnt to internal array
  // of 3D points if theIndex is valid
  void SetPnt (const Standard_Integer theIndex,
               const gp_Pnt& theValue)
  {
    if (theIndex < 0 || theIndex >= mynbpoints)
      Standard_OutOfRange::Raise("Select3D_PointData::SetPnt");
    mypolyg3d[theIndex] = theValue;
  }

  // Sets Select3D_Pnt2d to internal array
  // of 2D points if theIndex is valid
  void SetPnt2d (const Standard_Integer theIndex,
                 const Select3D_Pnt2d& theValue)
  {
    if (theIndex < 0 || theIndex >= mynbpoints)
      Standard_OutOfRange::Raise("Select3D_PointData::SetPnt2d");
    mypolyg2d[theIndex] = theValue;
  }

  // Sets gp_Pnt2d to internal array
  // of 2D points if theIndex is valid
  void SetPnt2d (const Standard_Integer theIndex,
                 const gp_Pnt2d& theValue)
  {
    if (theIndex < 0 || theIndex >= mynbpoints)
      Standard_OutOfRange::Raise("Select3D_PointData::SetPnt2d");
    mypolyg2d[theIndex] = theValue;
  }

  // Returns 3D point from internal array
  // if theIndex is valid
  Select3D_Pnt Pnt (const Standard_Integer theIndex) const
  {
    if (theIndex < 0 || theIndex >= mynbpoints)
      Standard_OutOfRange::Raise("Select3D_PointData::Pnt");
    return mypolyg3d[theIndex];
  }

  // Returns 2D point from internal array
  // if theIndex is valid
  Select3D_Pnt2d Pnt2d (const Standard_Integer theIndex) const
  {
    if (theIndex < 0 || theIndex >= mynbpoints)
      Standard_OutOfRange::Raise("Select3D_PointData::Pnt2d");
    return mypolyg2d[theIndex];
  }

  // Returns size of internal arrays
  const Standard_Integer Size () const
  {
    return mynbpoints;
  }

private:

  // Default constructor
  Select3D_PointData () {};

  Select3D_Pnt*    mypolyg3d;
  Select3D_Pnt2d*  mypolyg2d;
  Standard_Integer mynbpoints;
};

#endif
