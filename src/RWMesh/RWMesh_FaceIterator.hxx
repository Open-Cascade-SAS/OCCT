// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#ifndef _RWMesh_FaceIterator_HeaderFile
#define _RWMesh_FaceIterator_HeaderFile

#include <BRepLProp_SLProps.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_DataMap.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <XCAFPrs_Style.hxx>

#include <algorithm>

class TDF_Label;

//! Auxiliary class to iterate through triangulated faces.
class RWMesh_FaceIterator
{
public:

  //! Main constructor.
  Standard_EXPORT RWMesh_FaceIterator (const TDF_Label&       theLabel,
                                       const TopLoc_Location& theLocation,
                                       const Standard_Boolean theToMapColors = false,
                                       const XCAFPrs_Style&   theStyle = XCAFPrs_Style());

  //! Auxiliary constructor.
  Standard_EXPORT RWMesh_FaceIterator (const TopoDS_Shape&  theShape,
                                       const XCAFPrs_Style& theStyle = XCAFPrs_Style());

  //! Return explored shape.
  const TopoDS_Shape& ExploredShape() const { return myFaceIter.ExploredShape(); }

  //! Return true if iterator points to the valid triangulation.
  bool More() const { return !myPolyTriang.IsNull(); }

  //! Find next value.
  Standard_EXPORT void Next();

  //! Return current face.
  const TopoDS_Face& Face() const { return myFace; }

  //! Return current face triangulation.
  const Handle(Poly_Triangulation)& Triangulation() const { return myPolyTriang; }

  //! Return true if mesh data is defined.
  bool IsEmptyMesh() const
  {
    return myPolyTriang.IsNull()
       || (myPolyTriang->NbNodes() < 1 && myPolyTriang->NbTriangles() < 1);
  }

public:

  //! Return face material.
  const XCAFPrs_Style& FaceStyle() const { return myFaceStyle; }

  //! Return TRUE if face color is set.
  bool HasFaceColor() const { return myHasFaceColor; }

  //! Return face color.
  const Quantity_ColorRGBA& FaceColor() const { return myFaceColor; }

public:

  //! Return number of elements of specific type for the current face.
  Standard_Integer NbTriangles() const { return myPolyTriang->NbTriangles(); }

  //! Lower element index in current triangulation.
  Standard_Integer ElemLower() const { return 1; }

  //! Upper element index in current triangulation.
  Standard_Integer ElemUpper() const { return myPolyTriang->NbTriangles(); }

  //! Return triangle with specified index with applied Face orientation.
  Poly_Triangle TriangleOriented (Standard_Integer theElemIndex) const
  {
    Poly_Triangle aTri = triangle (theElemIndex);
    if ((myFace.Orientation() == TopAbs_REVERSED) ^ myIsMirrored)
    {
      return Poly_Triangle (aTri.Value (1), aTri.Value (3), aTri.Value (2));
    }
    return aTri;
  }

public:

  //! Return true if triangulation has defined normals.
  bool HasNormals() const { return myHasNormals; }

  //! Return true if triangulation has defined normals.
  bool HasTexCoords() const { return !myPolyTriang.IsNull() && myPolyTriang->HasUVNodes(); }

  //! Return normal at specified node index with face transformation applied and face orientation applied.
  gp_Dir NormalTransformed (Standard_Integer theNode) const
  {
    gp_Dir aNorm = normal (theNode);
    if (myTrsf.Form() != gp_Identity)
    {
      aNorm.Transform (myTrsf);
    }
    if (myFace.Orientation() == TopAbs_REVERSED)
    {
      aNorm.Reverse();
    }
    return aNorm;
  }

  //! Return number of nodes for the current face.
  Standard_Integer NbNodes() const
  {
    return !myPolyTriang.IsNull()
          ? myPolyTriang->NbNodes()
          : 0;
  }

  //! Lower node index in current triangulation.
  Standard_Integer NodeLower() const { return 1; }

  //! Upper node index in current triangulation.
  Standard_Integer NodeUpper() const { return myPolyTriang->NbNodes(); }

  //! Return the node with specified index with applied transformation.
  gp_Pnt NodeTransformed (const Standard_Integer theNode) const
  {
    gp_Pnt aNode = node (theNode);
    aNode.Transform (myTrsf);
    return aNode;
  }

  //! Return texture coordinates for the node.
  gp_Pnt2d NodeTexCoord (const Standard_Integer theNode) const
  {
    return myPolyTriang->HasUVNodes() ? myPolyTriang->UVNode (theNode) : gp_Pnt2d();
  }

public:

  //! Return the node with specified index with applied transformation.
  gp_Pnt node (const Standard_Integer theNode) const { return myPolyTriang->Node (theNode); }

  //! Return normal at specified node index without face transformation applied.
  Standard_EXPORT gp_Dir normal (Standard_Integer theNode) const;

  //! Return triangle with specified index.
  Poly_Triangle triangle (Standard_Integer theElemIndex) const { return myPolyTriang->Triangle (theElemIndex); }

private:

  //! Dispatch face styles.
  void dispatchStyles (const TDF_Label&       theLabel,
                       const TopLoc_Location& theLocation,
                       const XCAFPrs_Style&   theStyle);

  //! Reset information for current face.
  void resetFace()
  {
    myPolyTriang.Nullify();
    myFace.Nullify();
    myHasNormals = false;
    myHasFaceColor = false;
    myFaceColor = Quantity_ColorRGBA();
    myFaceStyle = XCAFPrs_Style();
  }

  //! Initialize face properties.
  void initFace();

private:

  NCollection_DataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher>
                                  myStyles;       //!< Face -> Style map
  XCAFPrs_Style                   myDefStyle;     //!< default style for faces without dedicated style
  Standard_Boolean                myToMapColors;  //!< flag to dispatch styles

  TopExp_Explorer                 myFaceIter;     //!< face explorer
  TopoDS_Face                     myFace;         //!< current face
  Handle(Poly_Triangulation)      myPolyTriang;   //!< triangulation of current face
  TopLoc_Location                 myFaceLocation; //!< current face location
  mutable BRepLProp_SLProps       mySLTool;       //!< auxiliary tool for fetching normals from surface
  BRepAdaptor_Surface             myFaceAdaptor;  //!< surface adaptor for fetching normals from surface
  Standard_Boolean                myHasNormals;   //!< flag indicating that current face has normals
  gp_Trsf                         myTrsf;         //!< current face transformation
  Standard_Boolean                myIsMirrored;   //!< flag indicating that face triangles should be mirrored
  XCAFPrs_Style                   myFaceStyle;    //!< current face style
  Quantity_ColorRGBA              myFaceColor;    //!< current face color
  Standard_Boolean                myHasFaceColor; //!< flag indicating that current face has assigned color

};

#endif // _RWMesh_FaceIterator_HeaderFile
