// Created on: 2011-11-24
// Created by: ANNA MASALSKAYA
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef BRepBuilderAPI_VertexInspector_HeaderFile
#define BRepBuilderAPI_VertexInspector_HeaderFile

#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_CellFilter.hxx>

typedef NCollection_Vector<gp_XYZ> VectorOfPoint;

//! Inspector for CellFilter algorithm working with gp_XYZ points in 3d space.
//! Used in search of coincidence points with a certain tolerance.

class BRepBuilderAPI_VertexInspector
{
public:
  static constexpr int Dimension = 3;

  typedef gp_XYZ Point;
  typedef int    Target;

  static double Coord(int i, const Point& thePnt) { return thePnt.Coord(i + 1); }

  Point Shift(const Point& thePnt, double theTol) const
  {
    return Point(thePnt.X() + theTol, thePnt.Y() + theTol, thePnt.Z() + theTol);
  }

  //! Constructor; remembers the tolerance
  BRepBuilderAPI_VertexInspector(const double theTol)
      : myTol(theTol * theTol)
  {
  }

  //! Keep the points used for comparison
  void Add(const gp_XYZ& thePnt) { myPoints.Append(thePnt); }

  //! Clear the list of adjacent points
  void ClearResList() { myResInd.Clear(); }

  //! Set current point to search for coincidence
  void SetCurrent(const gp_XYZ& theCurPnt) { myCurrent = theCurPnt; }

  //! Get list of indexes of points adjacent with the current
  const NCollection_List<int>& ResInd() { return myResInd; }

  //! Implementation of inspection method
  Standard_EXPORT NCollection_CellFilter_Action Inspect(const int theTarget);

private:
  double                myTol;
  NCollection_List<int> myResInd;
  VectorOfPoint         myPoints;
  gp_XYZ                myCurrent;
};

#endif
