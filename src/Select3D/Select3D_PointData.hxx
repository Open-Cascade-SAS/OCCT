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

#ifndef _Select3D_PointData_HeaderFile
#define _Select3D_PointData_HeaderFile

#include <Select3D_Pnt.hxx>

// A framework for safe management of Select3D_SensitivePoly polygons of 3D points
class Select3D_PointData {

public:

  // Constructs internal array of 3D points defined
  // by number of points theNbPoints
  Select3D_PointData (const Standard_Integer theNbPoints)
  : mynbpoints(theNbPoints)
  {
    if (theNbPoints <= 0)
      Standard_ConstructionError::Raise("Select3D_PointData");

    mypolyg3d = new Select3D_Pnt[mynbpoints];
  }

  // Destructor
  ~Select3D_PointData ()
  {
    delete [] mypolyg3d;
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

  // Returns 3D point from internal array
  // if theIndex is valid
  Select3D_Pnt Pnt (const Standard_Integer theIndex) const
  {
    if (theIndex < 0 || theIndex >= mynbpoints)
      Standard_OutOfRange::Raise("Select3D_PointData::Pnt");
    return mypolyg3d[theIndex];
  }

  // Returns 3D point from internal array
  // if theIndex is valid
  gp_Pnt Pnt3d (const Standard_Integer theIndex) const
  {
    if (theIndex < 0 || theIndex >= mynbpoints)
      Standard_OutOfRange::Raise("Select3D_PointData::Pnt");
    return mypolyg3d[theIndex];
  }

  // Returns size of internal arrays
  Standard_Integer Size () const
  {
    return mynbpoints;
  }

private:
  Select3D_PointData (const Select3D_PointData&);
  Select3D_PointData& operator= (const Select3D_PointData&);

private:

  Select3D_Pnt*    mypolyg3d;
  Standard_Integer mynbpoints;
};

#endif
