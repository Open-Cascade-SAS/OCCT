// File:        BRepBuilderAPI_VertexInspector.hxx
// Created:     Nov 24 16:48:12 2011
// Author:      ANNA MASALSKAYA
// Copyright:   Open CASCADE SAS 2011

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
