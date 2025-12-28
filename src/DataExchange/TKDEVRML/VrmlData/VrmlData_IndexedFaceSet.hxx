// Created on: 2006-05-26
// Created by: Alexander GRIGORIEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef VrmlData_IndexedFaceSet_HeaderFile
#define VrmlData_IndexedFaceSet_HeaderFile

#include <VrmlData_Faceted.hxx>
#include <VrmlData_Coordinate.hxx>
#include <VrmlData_Normal.hxx>
#include <VrmlData_Color.hxx>
#include <VrmlData_TextureCoordinate.hxx>
#include <gp_XYZ.hxx>
#include <Quantity_Color.hxx>

/**
 *  Implementation of IndexedFaceSet node
 */
class VrmlData_IndexedFaceSet : public VrmlData_Faceted
{
public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor
   */
  inline VrmlData_IndexedFaceSet()
      : myArrPolygons(0L),
        myArrNormalInd(0L),
        myArrColorInd(0L),
        myArrTextureInd(0L),
        myNbPolygons(0),
        myNbNormals(0),
        myNbColors(0),
        myNbTextures(0),
        myNormalPerVertex(true),
        myColorPerVertex(true)
  {
  }

  /**
   * Constructor
   */
  inline VrmlData_IndexedFaceSet(const VrmlData_Scene& theScene,
                                 const char*           theName,
                                 const bool            isCCW          = true,
                                 const bool            isSolid        = true,
                                 const bool            isConvex       = true,
                                 const double          theCreaseAngle = 0.)
      : VrmlData_Faceted(theScene, theName, isCCW, isSolid, isConvex, theCreaseAngle),
        myArrPolygons(0L),
        myArrNormalInd(0L),
        myArrColorInd(0L),
        myArrTextureInd(0L),
        myNbPolygons(0),
        myNbNormals(0),
        myNbColors(0),
        myNbTextures(0),
        myNormalPerVertex(true),
        myColorPerVertex(true)
  {
  }

  /**
   * Query the Normals.
   */
  inline const occ::handle<VrmlData_Normal>& Normals() const { return myNormals; }

  /**
   * Query the Colors.
   */
  inline const occ::handle<VrmlData_Color>& Colors() const { return myColors; }

  /**
   * Query the Texture Coordinates.
   */
  inline const occ::handle<VrmlData_TextureCoordinate>& TextureCoords() const { return myTxCoords; }

  // ========================================================================
  // =========================== TRIANGULATION GRID =========================
  /**
   * Query the Coordinates.
   */
  inline const occ::handle<VrmlData_Coordinate>& Coordinates() const { return myCoords; }

  /**
   * Query the array of polygons
   */
  inline size_t Polygons(const int**& arrPolygons) const
  {
    arrPolygons = myArrPolygons;
    return myNbPolygons;
  }

  /**
   * Query one polygon.
   * @param iFace
   *   rank of the polygon [0 .. N-1]
   * @param outIndice
   *   <tt>[out]</tt> array of vertex indice
   * @return
   *   number of vertice in the polygon - the dimension of outIndice array
   */
  inline int Polygon(const int iFace, const int*& outIndice)
  {
    return *(outIndice = myArrPolygons[iFace])++;
  }

  /**
   * Set the nodes
   */
  inline void SetCoordinates(const occ::handle<VrmlData_Coordinate>& theCoord)
  {
    myCoords = theCoord;
  }

  /**
   * Set the polygons
   */
  inline void SetPolygons(const size_t nPolygons, const int** thePolygons)
  {
    myNbPolygons  = nPolygons;
    myArrPolygons = thePolygons;
  }

  // ========================================================================
  // ================================ NORMALS ===============================

  /**
   * Query the array of normal indice
   * @param arrNormalInd
   *   <tt>[out]</tt> array of normalIndex as it is described in VRML2.0 spec
   * @return
   *   Number of integers in the array arrNormalInd.
   */
  inline size_t ArrayNormalInd(const int**& arrNormalInd) const
  {
    arrNormalInd = myArrNormalInd;
    return myNbNormals;
  }

  /**
   * Query normals indice for one face. This method should be called after
   * checking myArrNormalInd != NULL, otherwise exception will be thrown.
   * @param iFace
   *   rank of the face [0 .. N-1]
   * @param outIndice
   *   <tt>[out]</tt> array of normals indice
   * @return
   *   number of indice in the array - the dimension of outIndice array
   */
  inline int IndiceNormals(const int iFace, const int*& outIndice)
  {
    return *(outIndice = myArrNormalInd[iFace])++;
  }

  /**
   * Query a normal for one node in the given element. The normal is
   * interpreted according to fields myNormals, myArrNormalInd,
   * myNormalPerVertex, as defined in VRML 2.0.
   * @param iFace
   *   rank of the polygon [0 .. N-1]
   * @param iVertex
   *   rank of the vertex in the polygon [0 .. M-1].  This parameter is ignored
   *   if (myNormalPerVertex == False)
   * @return
   *   Normal vector; if the normal is indefinite then returns (0., 0., 0.)
   */
  Standard_EXPORT gp_XYZ GetNormal(const int iFace, const int iVertex);

  /**
   * Set the normals array of indice
   */
  inline void SetNormalInd(const size_t nIndice, const int** theIndice)
  {
    myNbNormals    = nIndice;
    myArrNormalInd = theIndice;
  }

  /**
   * Set the normals node
   */
  inline void SetNormals(const occ::handle<VrmlData_Normal>& theNormals) { myNormals = theNormals; }

  /**
   * Set the boolean value "normalPerVertex"
   */
  inline void SetNormalPerVertex(const bool isNormalPerVertex)
  {
    myNormalPerVertex = isNormalPerVertex;
  }

  // ========================================================================
  // ================================ COLORS ================================

  /**
   * Query the array of color indice
   * @param arrColorInd
   *   <tt>[out]</tt> array of colorIndex as it is described in VRML2.0 spec
   * @return
   *   Number of integers in the array arrColorInd.
   */
  inline size_t ArrayColorInd(const int**& arrColorInd) const
  {
    arrColorInd = myArrColorInd;
    return myNbColors;
  }

  /**
   * Query a color for one node in the given element. The color is
   * interpreted according to fields myColors, myArrColorInd,
   * myColorPerVertex, as defined in VRML 2.0.
   * @param iFace
   *   rank of the polygon [0 .. N-1]
   * @param iVertex
   *   rank of the vertex in the polygon [0 .. M-1]. This parameter is ignored
   *   if (myColorPerVertex == False)
   * @return
   *   Color value (RGB); if the color is indefinite then returns (0., 0., 0.)
   */
  Standard_EXPORT Quantity_Color GetColor(const int iFace, const int iVertex);

  /**
   * Set the colors array of indice
   */
  inline void SetColorInd(const size_t nIndice, const int** theIndice)
  {
    myNbColors    = nIndice;
    myArrColorInd = theIndice;
  }

  /**
   * Set the Color node
   */
  inline void SetColors(const occ::handle<VrmlData_Color>& theColors) { myColors = theColors; }

  /**
   * Set the boolean value "colorPerVertex"
   */
  inline void SetColorPerVertex(const bool isColorPerVertex)
  {
    myColorPerVertex = isColorPerVertex;
  }

  // ========================================================================
  // ========================== TEXTURE COIRDINATES =========================

  /**
   * Query the array of texture coordinate indice
   * @param arrTextureCoordInd
   *   <tt>[out]</tt> array of texCoordIndex as it is described in VRML2.0 spec
   * @return
   *   Number of integers in the array texCoordIndex.
   */
  inline size_t ArrayTextureCoordInd(const int**& arrTextureCoordInd) const
  {
    arrTextureCoordInd = myArrTextureInd;
    return myNbTextures;
  }

  /**
   * Set the TexCoordiante array of indice
   */
  inline void SetTextureCoordInd(const size_t nIndice, const int** theIndice)
  {
    myNbTextures    = nIndice;
    myArrTextureInd = theIndice;
  }

  /**
   * Set the Texture Coordinate node
   */
  inline void SetTextureCoords(const occ::handle<VrmlData_TextureCoordinate>& tc)
  {
    myTxCoords = tc;
  }

  /**
   * Query the shape. This method checks the flag myIsModified; if True it
   * should rebuild the shape presentation.
   */
  Standard_EXPORT virtual const occ::handle<TopoDS_TShape>& TShape() override;

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual occ::handle<VrmlData_Node> Clone(
    const occ::handle<VrmlData_Node>& theOther) const override;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus Read(VrmlData_InBuffer& theBuffer) override;

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus Write(const char* thePrefix) const override;

  /**
   * Returns True if the node is default, so that it should not be written.
   */
  Standard_EXPORT virtual bool IsDefault() const override;

private:
  // ---------- PRIVATE FIELDS ----------
  occ::handle<VrmlData_Coordinate>        myCoords;
  occ::handle<VrmlData_Normal>            myNormals;
  occ::handle<VrmlData_Color>             myColors;
  occ::handle<VrmlData_TextureCoordinate> myTxCoords;
  const int**                             myArrPolygons;
  const int**                             myArrNormalInd;
  const int**                             myArrColorInd;
  const int**                             myArrTextureInd;
  size_t                                  myNbPolygons;
  size_t                                  myNbNormals;
  size_t                                  myNbColors;
  size_t                                  myNbTextures;
  bool                                    myNormalPerVertex;
  bool                                    myColorPerVertex;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(VrmlData_IndexedFaceSet, VrmlData_Faceted)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
#endif
