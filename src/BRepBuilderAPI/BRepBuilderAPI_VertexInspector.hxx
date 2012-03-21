// Created on: 2011-11-24
// Created by: ANNA MASALSKAYA
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#ifndef _BRepBuilderAPI_VertexInspector_Header
#define _BRepBuilderAPI_VertexInspector_Header

#ifndef _TColStd_ListOfInteger_HeaderFile
#include <TColStd_ListOfInteger.hxx>
#endif
#ifndef NCollection_Vector_HeaderFile
#include <NCollection_Vector.hxx>
#endif
#ifndef _gp_XY_HeaderFile
#include <gp_XY.hxx>
#endif
#ifndef _gp_XYZ_HeaderFile
#include <gp_XYZ.hxx>
#endif

#ifndef NCollection_CellFilter_HeaderFile
#include <NCollection_CellFilter.hxx>
#endif

typedef NCollection_Vector<gp_XYZ> VectorOfPoint;

//=======================================================================
//! Class BRepBuilderAPI_VertexInspector 
//!   derived from NCollection_CellFilter_InspectorXYZ
//!   This class define the Inspector interface for CellFilter algorithm, 
//!   working with gp_XYZ points in 3d space.
//!   Used in search of coincidence points with a certain tolerance.
//=======================================================================

class BRepBuilderAPI_VertexInspector : public NCollection_CellFilter_InspectorXYZ
{
public:
  typedef Standard_Integer Target;
  //! Constructor; remembers the tolerance
  BRepBuilderAPI_VertexInspector (const Standard_Real theTol)
                                  : myTol(theTol*theTol)
  {}

  //! Keep the points used for comparison
  void Add (const gp_XYZ& thePnt)
  {
    myPoints.Append (thePnt);
  }
  
  //! Clear the list of adjacent points
  void ClearResList()
  {
    myResInd.Clear();
  }
  
  //! Set current point to search for coincidence
  void SetCurrent (const gp_XYZ& theCurPnt) 
  { 
    myCurrent = theCurPnt;
  }

  //! Get list of indexes of points adjacent with the current
  const TColStd_ListOfInteger& ResInd()
  {
    return myResInd;
  }

  //! Implementation of inspection method
  NCollection_CellFilter_Action Inspect (const Standard_Integer theTarget); 

private:
  Standard_Real myTol;
  TColStd_ListOfInteger myResInd;
  VectorOfPoint myPoints;
  gp_XYZ myCurrent;
};

#endif
