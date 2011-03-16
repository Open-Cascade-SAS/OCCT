// File:	MeshAlgo_CircleInspector.hxx
// Created:	May 26 16:40:53 2008
// Author:	Ekaterina SMIRNOVA
// Copyright: Open CASCADE SAS 2008


#ifndef MeshAlgo_CircleInspector_Header
#define MeshAlgo_CircleInspector_Header

#include <MeshAlgo_Circ.hxx>

#include <MeshDS_ListOfInteger.hxx>
#include <Precision.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>

#include <NCollection_CellFilter.hxx>
#include <NCollection_Vector.hxx>

typedef NCollection_Vector<MeshAlgo_Circ> CircVector;

//=======================================================================
//! The class to find in the coincidence points 
//=======================================================================

class MeshAlgo_CircleInspector : public NCollection_CellFilter_InspectorXY
{
public:
  typedef Standard_Integer Target;
  //! Constructor; remembers tolerance and collector data structure.
  //! All the found points are put in the map and excluded from further
  //! consideration.
  MeshAlgo_CircleInspector (Standard_Real theTol,
                            Standard_Integer nbComp,
                            const MeshDS_BaseAllocator& theAlloc);

  void Add(Standard_Integer theInd,const MeshAlgo_Circ& theCircle)
  {
    myInitCircle.SetValue(theInd, theCircle);
  }

  void ClerResList()
  {
    myResInd.Clear();
  }
  
  CircVector& MapOfCirc()
  {
    return myInitCircle; 
  }
  
  MeshAlgo_Circ& GetCirc(Standard_Integer theInd)
  {
    return myInitCircle(theInd);
  }

  //! Set current node to be checked
  void SetCurrent (const gp_XY& theCurCircle) 
  { 
    myCurrent = theCurCircle;
  }

  //!Get result index of node
  MeshDS_ListOfInteger& GetCoincidentInd()
  {
    return myResInd;
  }

   //! Implementation of inspection method
  NCollection_CellFilter_Action Inspect (const Standard_Integer theTarget); 
  
  static Standard_Boolean IsEqual (Standard_Integer theIdx, const Standard_Integer theTarget)
  {
    return (theIdx == theTarget);
  }

private:
  Standard_Real                        myTol;
  MeshDS_ListOfInteger                 myResInd;
  CircVector                           myInitCircle;
  gp_XY                                myCurrent;
};

#endif
