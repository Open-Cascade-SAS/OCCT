// Created on: 2007-08-04
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef VrmlData_ShapeConvert_HeaderFile
#define VrmlData_ShapeConvert_HeaderFile

#include <VrmlData_Geometry.hxx>
#include <VrmlData_Appearance.hxx>
#include <VrmlData_Group.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>

class VrmlData_Scene;
class VrmlData_Coordinate;
class TopoDS_Face;
class Poly_Polygon3D;
class Poly_Triangulation;
class XCAFPrs_Style;
class TDocStd_Document;
class TDF_Label;


/**
 * Algorithm converting one shape or a set of shapes to VrmlData_Scene.
 */

class VrmlData_ShapeConvert 
{
 public:

  typedef struct {
    TCollection_AsciiString Name;
    TopoDS_Shape            Shape;
    Handle(VrmlData_Node)   Node;
  } ShapeData;

  // ---------- PUBLIC METHODS ----------


  /**
   * Constructor.
   * @param theScene
   *   Scene receiving all Vrml data.
   * @param theScale
   *   Scale factor, considering that VRML standard specifies coordinates in
   *   meters. So if your data are in mm, you should provide theScale=0.001
   */
  inline VrmlData_ShapeConvert (VrmlData_Scene&     theScene,
                                const Standard_Real theScale = 1.)
    : myScene (theScene),
      myScale (theScale),
      myDeflection(0.0),
      myDeflAngle(0.0)
  {}

  /**
   * Add one shape to the internal list, may be called several times with
   * different shapes.
   */
  Standard_EXPORT void AddShape (const TopoDS_Shape& theShape,
                                 const char *        theName = 0L);

  /**
   * Convert all accumulated shapes and store them in myScene.
   * The internal data structures are cleared in the end of conversion.
   * @param theExtractFaces
   *   If True,  converter extracst faces from the shapes. 
   * @param theExtractEdges
   *   If True,  converter extracts edges from the shapes.
   * @param theDeflection 
   *   Deflection for tessellation of geometrical lines/surfaces. Existing mesh
   *   is used if its deflection is smaller than the one given by this
   *   parameter.
   * @param theDeflAngle 
   *   Angular deflection for tessellation of geometrical lines. 
   */
  Standard_EXPORT void Convert (const Standard_Boolean theExtractFaces,
				const Standard_Boolean theExtractEdges,
                                const Standard_Real    theDeflection = 0.01,
				const Standard_Real    theDeflAngle = 20.*M_PI/180.);
                                //this value of theDeflAngle is used by default 
                                //for tesselation while shading (Drawer->HLRAngle())

  /**
   * Add all shapes start from given document with colors and names to the internal structure
   */
  Standard_EXPORT void ConvertDocument(const Handle(TDocStd_Document)& theDoc);

 protected:
  // ---------- PROTECTED METHODS ----------

  Handle(VrmlData_Geometry) triToIndexedFaceSet
                                (const Handle(Poly_Triangulation)&,
                                 const TopoDS_Face&,
                                 const Handle(VrmlData_Coordinate)&);

  Handle(VrmlData_Geometry) polToIndexedLineSet
                                (const Handle(Poly_Polygon3D)&);

  Handle(VrmlData_Appearance) defaultMaterialFace () const;

  Handle(VrmlData_Appearance) defaultMaterialEdge () const;

  Handle(VrmlData_Geometry) makeTShapeNode(const TopoDS_Shape& theShape,
                                           const TopAbs_ShapeEnum theShapeType,
                                           TopLoc_Location& theLoc);

  void addAssembly (const Handle(VrmlData_Group)& theParent,
                    const TDF_Label& theLabel,
                    const Handle(TDocStd_Document)& theDoc,
                    const Standard_Boolean theNeedCreateGroup);

  void addInstance (const Handle(VrmlData_Group)& theParent,
                    const TDF_Label& theLabel,
                    const Handle(TDocStd_Document)& theDoc);

  void addShape (const Handle(VrmlData_Group)& theParent,
                 const TDF_Label& theLabel,
                 const Handle(TDocStd_Document)& theDoc);

  Handle(VrmlData_Appearance) makeMaterialFromStyle (const XCAFPrs_Style& theStyle,
                                                     const TDF_Label& theAttribLab) const;

 private:
  // ---------- PRIVATE FIELDS ----------

  VrmlData_Scene&                       myScene;
  Standard_Real                         myScale;
  NCollection_List <ShapeData>          myShapes;

  Standard_Real myDeflection;
  Standard_Real myDeflAngle;
  NCollection_DataMap <TopoDS_Shape, Handle(VrmlData_Geometry)> myRelMap;

  // ---------- PRIVATE METHODS ----------
  void operator= (const VrmlData_ShapeConvert&);
};

#endif
