// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _RWMesh_ShapeIterator_HeaderFile
#define _RWMesh_ShapeIterator_HeaderFile

#include <BRepLProp_SLProps.hxx>
#include <NCollection_DataMap.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopoDS_Edge.hxx>
#include <XCAFPrs_Style.hxx>
#include <gp_Trsf.hxx>

#include <algorithm>

class TDF_Label;

//! This is a virtual base class for other shape iterators.
//! Provides an abstract interface for iterating over the elements of a shape.
//! It defines a set of pure virtual methods that must be implemented by
//! derived classes to handle specific types of shapes and their elements.
class RWMesh_ShapeIterator
{
public:
  //! Return explored shape.
  const TopoDS_Shape& ExploredShape() const { return myIter.ExploredShape(); }

  //! Return shape.
  Standard_EXPORT virtual const TopoDS_Shape& Shape() const = 0;

  //! Return true if iterator points to the valid triangulation.
  Standard_EXPORT virtual bool More() const = 0;

  //! Find next value.
  Standard_EXPORT virtual void Next() = 0;

  //! Return true if mesh data is defined.
  Standard_EXPORT virtual bool IsEmpty() const = 0;

  //! Return shape material.
  const XCAFPrs_Style& Style() const { return myStyle; }

  //! Return TRUE if shape color is set.
  bool HasColor() const { return myHasColor; }

  //! Return shape color.
  const Quantity_ColorRGBA& Color() const { return myColor; }

  //! Lower element index in current triangulation.
  Standard_EXPORT virtual Standard_Integer ElemLower() const = 0;

  //! Upper element index in current triangulation.
  Standard_EXPORT virtual Standard_Integer ElemUpper() const = 0;

  //! Return number of nodes for the current shape.
  Standard_EXPORT virtual Standard_Integer NbNodes() const = 0;

  //! Lower node index in current shape.
  Standard_EXPORT virtual Standard_Integer NodeLower() const = 0;

  //! Upper node index in current shape.
  Standard_EXPORT virtual Standard_Integer NodeUpper() const = 0;

  //! Return the node with specified index with applied transformation.
  gp_Pnt NodeTransformed(const Standard_Integer theNode) const
  {
    gp_Pnt aNode = node(theNode);
    aNode.Transform(myTrsf);
    return aNode;
  }

protected:
  //! Return the node with specified index with applied transformation.
  virtual gp_Pnt node(const Standard_Integer theNode) const = 0;

  //! Main constructor.
  RWMesh_ShapeIterator(const TDF_Label&       theLabel,
                       const TopLoc_Location& theLocation,
                       const TopAbs_ShapeEnum theShapeTypeFind,
                       const TopAbs_ShapeEnum theShapeTypeAvoid,
                       const Standard_Boolean theToMapColors = false,
                       const XCAFPrs_Style&   theStyle       = XCAFPrs_Style());

  //! Auxiliary constructor.
  RWMesh_ShapeIterator(const TopoDS_Shape&    theShape,
                       const TopAbs_ShapeEnum theShapeTypeFind,
                       const TopAbs_ShapeEnum theShapeTypeAvoid,
                       const XCAFPrs_Style&   theStyle = XCAFPrs_Style());

  //! Dispatch shape styles.
  void dispatchStyles(const TDF_Label&       theLabel,
                      const TopLoc_Location& theLocation,
                      const XCAFPrs_Style&   theStyle);

  //! Reset information for current shape.
  void resetShape()
  {
    myHasColor = false;
    myColor    = Quantity_ColorRGBA();
    myStyle    = XCAFPrs_Style();
  }

  //! Initialize shape properties.
  void initShape();

protected:
  NCollection_DataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher>
                   myStyles;      //!< Shape -> Style map
  XCAFPrs_Style    myDefStyle;    //!< default style for shapes without dedicated style
  Standard_Boolean myToMapColors; //!< flag to dispatch styles

  TopExp_Explorer    myIter;      //!< shape explorer
  TopLoc_Location    myLocation;  //!< current shape location
  gp_Trsf            myTrsf;      //!< current shape transformation
  XCAFPrs_Style      myStyle;     //!< current shape style
  Quantity_ColorRGBA myColor;     //!< current shape color
  TopAbs_ShapeEnum   myShapeType; //!< type of shape
  Standard_Boolean   myHasColor;  //!< flag indicating that current shape has assigned color
};

#endif // _RWMesh_ShapeIterator_HeaderFile
