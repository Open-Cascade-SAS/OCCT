// Author: Kirill Gavrilov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _RWObj_TriangulationReader_HeaderFile
#define _RWObj_TriangulationReader_HeaderFile

#include <RWObj_Reader.hxx>

#include <Poly_Triangulation.hxx>
#include <TopoDS_Compound.hxx>

//! Interface to store shape attributes into document.
class RWObj_IShapeReceiver
{
public:
  //! @param theShape       shape to register
  //! @param theName        shape name
  //! @param theMaterial    shape material
  //! @param theIsRootShape indicates that this is a root object (free shape)
  virtual void BindNamedShape (const TopoDS_Shape& theShape,
                               const TCollection_AsciiString& theName,
                               const RWObj_Material* theMaterial,
                               const Standard_Boolean theIsRootShape) = 0;
};

//! RWObj_Reader implementation dumping OBJ file into Poly_Triangulation.
class RWObj_TriangulationReader : public RWObj_Reader
{
  DEFINE_STANDARD_RTTIEXT(RWObj_TriangulationReader, RWObj_Reader)
public:

  //! Constructor.
  RWObj_TriangulationReader() : myShapeReceiver (NULL), myToCreateShapes (Standard_True) {}

  //! Set flag to create shapes.
  void SetCreateShapes (Standard_Boolean theToCreateShapes) { myToCreateShapes = theToCreateShapes; }

  //! Set shape receiver callback.
  void SetShapeReceiver (RWObj_IShapeReceiver* theReceiver) { myShapeReceiver = theReceiver; }

  //! Create Poly_Triangulation from collected data
  Standard_EXPORT virtual Handle(Poly_Triangulation) GetTriangulation();

  //! Return result shape.
  Standard_EXPORT TopoDS_Shape ResultShape();

protected:

  //! Flush active sub-mesh.
  Standard_EXPORT virtual Standard_Boolean addMesh (const RWObj_SubMesh& theMesh,
                                                    const RWObj_SubMeshReason theReason) Standard_OVERRIDE;

  //! Retrieve sub-mesh node position.
  virtual gp_Pnt getNode (Standard_Integer theIndex) const Standard_OVERRIDE
  {
    return myNodes.Value (theIndex - 1);
  }

  //! Add new node.
  virtual Standard_Integer addNode (const gp_Pnt& thePnt) Standard_OVERRIDE
  {
    myNodes.Append (thePnt);
    return myNodes.Size();
  }

  //! Ignore normal.
  virtual void setNodeNormal (const Standard_Integer theIndex,
                              const Graphic3d_Vec3&  theNormal) Standard_OVERRIDE
  {
    myNormals.SetValue (theIndex - 1, theNormal);
  }

  //! Ignore texture coordinates.
  virtual void setNodeUV (const Standard_Integer theIndex,
                          const Graphic3d_Vec2&  theUV) Standard_OVERRIDE
  {
    myNodesUV.SetValue (theIndex - 1, theUV);
  }

  //! Add element.
  virtual void addElement (Standard_Integer theN1, Standard_Integer theN2, Standard_Integer theN3, Standard_Integer theN4) Standard_OVERRIDE
  {
    myTriangles.Append (Poly_Triangle (theN1, theN2, theN3));
    if (theN4 != -1)
    {
      myTriangles.Append (Poly_Triangle (theN1, theN3, theN4));
    }
  }

protected:

  //! Add sub-shape into specified shape
  Standard_EXPORT Standard_Boolean addSubShape (TopoDS_Shape& theParent,
                                                const TopoDS_Shape& theSubShape,
                                                const Standard_Boolean theToExpandCompound);

protected:

  NCollection_Vector<gp_Pnt>         myNodes;     //!< nodes   of currently filled triangulation
  NCollection_Vector<Graphic3d_Vec3> myNormals;   //!< normals of currently filled triangulation
  NCollection_Vector<Graphic3d_Vec2> myNodesUV;   //!< UVs     of currently filled triangulation
  NCollection_Vector<Poly_Triangle>  myTriangles; //!< indexes of currently filled triangulation

  RWObj_IShapeReceiver*   myShapeReceiver;   //!< optional shape receiver
  TopoDS_Compound         myResultShape;     //!< result shape as Compound of objects
  TopoDS_Compound         myLastObjectShape; //!< Compound containing current object groups
  TopoDS_Shape            myLastGroupShape;  //!< current group shape - either a single Face or Compound of Faces
  TCollection_AsciiString myLastGroupName;   //!< current group name
  TCollection_AsciiString myLastFaceMaterial;//!< last face material name
  Standard_Boolean        myToCreateShapes;  //!< create a single triangulation

};

#endif // _RWObj_TriangulationReader_HeaderFile
