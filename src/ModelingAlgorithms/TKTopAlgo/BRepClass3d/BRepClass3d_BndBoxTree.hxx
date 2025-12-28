// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2016 OPEN CASCADE SAS
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

#ifndef _BRepClass3d_BndBoxTree_HeaderFile
#define _BRepClass3d_BndBoxTree_HeaderFile

#include <NCollection_Sequence.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_UBTree.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <Geom_Line.hxx>
#include <Bnd_Box.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Precision.hxx>

// Typedef to reduce code complexity.
typedef NCollection_UBTree<int, Bnd_Box> BRepClass3d_BndBoxTree;

// Class representing tree selector for point object.
class BRepClass3d_BndBoxTreeSelectorPoint : public NCollection_UBTree<int, Bnd_Box>::Selector
{
public:
  BRepClass3d_BndBoxTreeSelectorPoint(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfShape)
      : BRepClass3d_BndBoxTreeSelectorPoint::Selector(),
        myMapOfShape(theMapOfShape)
  {
  }

  bool Reject(const Bnd_Box& theBox) const { return (theBox.IsOut(myP)); }

  bool Accept(const int& theObj);

  // Sets current point for boxes-point collisions.
  void SetCurrentPoint(const gp_Pnt& theP) { myP = theP; }

private:
  BRepClass3d_BndBoxTreeSelectorPoint(const BRepClass3d_BndBoxTreeSelectorPoint&);
  BRepClass3d_BndBoxTreeSelectorPoint& operator=(const BRepClass3d_BndBoxTreeSelectorPoint&);

private:
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&
         myMapOfShape; // shapes (vertices + edges)
  gp_Pnt myP;
};

// Class representing tree selector for line object.
class BRepClass3d_BndBoxTreeSelectorLine : public NCollection_UBTree<int, Bnd_Box>::Selector
{
public:
  struct EdgeParam
  {
    TopoDS_Edge myE;
    double      myParam;  // par on myE
    double      myLParam; // par on line
  };

  struct VertParam
  {
    TopoDS_Vertex myV;
    double        myLParam; // par on line
  };

public:
  BRepClass3d_BndBoxTreeSelectorLine(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapOfShape)
      : BRepClass3d_BndBoxTreeSelectorLine::Selector(),
        myMapOfShape(theMapOfShape),
        myIsValid(true)
  {
  }

  bool Reject(const Bnd_Box& theBox) const { return (theBox.IsOut(myL)); }

  bool Accept(const int& theObj);

  // Sets current line for boxes-line collisions
  void SetCurrentLine(const gp_Lin& theL, const double theMaxParam)
  {
    myL = theL;
    myLC.Load(new Geom_Line(theL), -Precision::PConfusion(), theMaxParam);
  }

  void GetEdgeParam(const int i, TopoDS_Edge& theOutE, double& theOutParam, double& outLParam) const
  {
    const EdgeParam& EP = myEP.Value(i);
    theOutE             = EP.myE;
    theOutParam         = EP.myParam;
    outLParam           = EP.myLParam;
  }

  void GetVertParam(const int i, TopoDS_Vertex& theOutV, double& outLParam) const
  {
    const VertParam& VP = myVP.Value(i);
    theOutV             = VP.myV;
    outLParam           = VP.myLParam;
  }

  int GetNbEdgeParam() const { return myEP.Length(); }

  int GetNbVertParam() const { return myVP.Length(); }

  void ClearResults()
  {
    myEP.Clear();
    myVP.Clear();
    myIsValid = true;
  }

  //! Returns TRUE if correct classification is possible
  bool IsCorrect() const { return myIsValid; }

private:
  BRepClass3d_BndBoxTreeSelectorLine(const BRepClass3d_BndBoxTreeSelectorLine&);
  BRepClass3d_BndBoxTreeSelectorLine& operator=(const BRepClass3d_BndBoxTreeSelectorLine&);

private:
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>&
                                  myMapOfShape; // shapes (vertices + edges)
  gp_Lin                          myL;
  NCollection_Sequence<EdgeParam> myEP; // output result (edge vs line)
  NCollection_Sequence<VertParam> myVP; // output result (vertex vs line)
  GeomAdaptor_Curve               myLC;
  bool                            myIsValid;
};

#endif
