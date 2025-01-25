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
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <Geom_Line.hxx>
#include <Bnd_Box.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Precision.hxx>

// Typedef to reduce code complexity.
typedef NCollection_UBTree<Standard_Integer, Bnd_Box> BRepClass3d_BndBoxTree;

// Class representing tree selector for point object.
class BRepClass3d_BndBoxTreeSelectorPoint : public BRepClass3d_BndBoxTree::Selector
{
public:
  BRepClass3d_BndBoxTreeSelectorPoint(const TopTools_IndexedMapOfShape& theMapOfShape)
      : BRepClass3d_BndBoxTreeSelectorPoint::Selector(),
        myMapOfShape(theMapOfShape)
  {
  }

  Standard_Boolean Reject(const Bnd_Box& theBox) const { return (theBox.IsOut(myP)); }

  Standard_Boolean Accept(const Standard_Integer& theObj);

  // Sets current point for boxes-point collisions.
  void SetCurrentPoint(const gp_Pnt& theP) { myP = theP; }

private:
  BRepClass3d_BndBoxTreeSelectorPoint(const BRepClass3d_BndBoxTreeSelectorPoint&);
  BRepClass3d_BndBoxTreeSelectorPoint& operator=(const BRepClass3d_BndBoxTreeSelectorPoint&);

private:
  const TopTools_IndexedMapOfShape& myMapOfShape; // shapes (vertices + edges)
  gp_Pnt                            myP;
};

// Class representing tree selector for line object.
class BRepClass3d_BndBoxTreeSelectorLine : public BRepClass3d_BndBoxTree::Selector
{
public:
  struct EdgeParam
  {
    TopoDS_Edge   myE;
    Standard_Real myParam;  // par on myE
    Standard_Real myLParam; // par on line
  };

  struct VertParam
  {
    TopoDS_Vertex myV;
    Standard_Real myLParam; // par on line
  };

public:
  BRepClass3d_BndBoxTreeSelectorLine(const TopTools_IndexedMapOfShape& theMapOfShape)
      : BRepClass3d_BndBoxTreeSelectorLine::Selector(),
        myMapOfShape(theMapOfShape),
        myIsValid(Standard_True)
  {
  }

  Standard_Boolean Reject(const Bnd_Box& theBox) const { return (theBox.IsOut(myL)); }

  Standard_Boolean Accept(const Standard_Integer& theObj);

  // Sets current line for boxes-line collisions
  void SetCurrentLine(const gp_Lin& theL, const Standard_Real theMaxParam)
  {
    myL = theL;
    myLC.Load(new Geom_Line(theL), -Precision::PConfusion(), theMaxParam);
  }

  void GetEdgeParam(const Standard_Integer i,
                    TopoDS_Edge&           theOutE,
                    Standard_Real&         theOutParam,
                    Standard_Real&         outLParam) const
  {
    const EdgeParam& EP = myEP.Value(i);
    theOutE             = EP.myE;
    theOutParam         = EP.myParam;
    outLParam           = EP.myLParam;
  }

  void GetVertParam(const Standard_Integer i,
                    TopoDS_Vertex&         theOutV,
                    Standard_Real&         outLParam) const
  {
    const VertParam& VP = myVP.Value(i);
    theOutV             = VP.myV;
    outLParam           = VP.myLParam;
  }

  Standard_Integer GetNbEdgeParam() const { return myEP.Length(); }

  Standard_Integer GetNbVertParam() const { return myVP.Length(); }

  void ClearResults()
  {
    myEP.Clear();
    myVP.Clear();
    myIsValid = Standard_True;
  }

  //! Returns TRUE if correct classification is possible
  Standard_Boolean IsCorrect() const { return myIsValid; }

private:
  BRepClass3d_BndBoxTreeSelectorLine(const BRepClass3d_BndBoxTreeSelectorLine&);
  BRepClass3d_BndBoxTreeSelectorLine& operator=(const BRepClass3d_BndBoxTreeSelectorLine&);

private:
  const TopTools_IndexedMapOfShape& myMapOfShape; // shapes (vertices + edges)
  gp_Lin                            myL;
  NCollection_Sequence<EdgeParam>   myEP; // output result (edge vs line)
  NCollection_Sequence<VertParam>   myVP; // output result (vertex vs line)
  GeomAdaptor_Curve                 myLC;
  Standard_Boolean                  myIsValid;
};

#endif
