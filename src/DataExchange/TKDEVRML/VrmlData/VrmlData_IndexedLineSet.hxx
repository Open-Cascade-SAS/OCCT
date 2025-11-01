// Created on: 2007-08-01
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

#ifndef VrmlData_IndexedLineSet_HeaderFile
#define VrmlData_IndexedLineSet_HeaderFile

#include <VrmlData_Geometry.hxx>
#include <VrmlData_Coordinate.hxx>
#include <VrmlData_Color.hxx>

// #include <Quantity_Color.hxx>

/**
 * Data type to store a set of polygons.
 */

class VrmlData_IndexedLineSet : public VrmlData_Geometry
{
public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   */
  inline VrmlData_IndexedLineSet()
      : myArrPolygons(0L),
        myArrColorInd(0L),
        myNbPolygons(0),
        myNbColors(0),
        myColorPerVertex(Standard_True)
  {
  }

  /**
   * Constructor.
   */
  inline VrmlData_IndexedLineSet(const VrmlData_Scene&  theScene,
                                 const char*            theName,
                                 const Standard_Boolean isColorPerVertex = Standard_True)
      : VrmlData_Geometry(theScene, theName),
        myArrPolygons(0L),
        myArrColorInd(0L),
        myNbPolygons(0),
        myNbColors(0),
        myColorPerVertex(isColorPerVertex)
  {
  }

  /**
   * Query the Coordinates.
   */
  inline const Handle(VrmlData_Coordinate)& Coordinates() const { return myCoords; }

  /**
   * Set the nodes
   */
  inline void SetCoordinates(const Handle(VrmlData_Coordinate)& theCoord) { myCoords = theCoord; }

  /**
   * Query the Colors.
   */
  inline const Handle(VrmlData_Color)& Colors() const { return myColors; }

  /**
   * Set the Color node
   */
  inline void SetColors(const Handle(VrmlData_Color)& theColors) { myColors = theColors; }

  // ========================================================================
  // =========================== POLYLINES  =================================
  /**
   * Query the array of polygons
   */
  inline size_t Polygons(const Standard_Integer**& arrPolygons) const
  {
    arrPolygons = myArrPolygons;
    return myNbPolygons;
  }

  /**
   * Query one polygon.
   * @param iPolygon
   *   rank of the polygon [0 .. N-1]
   * @param outIndice
   *   <tt>[out]</tt> array of vertex indice
   * @return
   *   number of vertice in the polygon - the dimension of outIndice array
   */
  inline Standard_Integer Polygon(const Standard_Integer   iPolygon,
                                  const Standard_Integer*& outIndice)
  {
    return *(outIndice = myArrPolygons[iPolygon])++;
  }

  /**
   * Set the polygons
   */
  inline void SetPolygons(const Standard_Size nPolygons, const Standard_Integer** thePolygons)
  {
    myNbPolygons  = nPolygons;
    myArrPolygons = thePolygons;
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
  inline size_t ArrayColorInd(const Standard_Integer**& arrColorInd) const
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
  Standard_EXPORT Quantity_Color GetColor(const Standard_Integer iFace,
                                          const Standard_Integer iVertex);

  /**
   * Set the colors array of indice
   */
  inline void SetColorInd(const Standard_Size nIndice, const Standard_Integer** theIndice)
  {
    myNbColors    = nIndice;
    myArrColorInd = theIndice;
  }

  /**
   * Set the boolean value "colorPerVertex"
   */
  inline void SetColorPerVertex(const Standard_Boolean isColorPerVertex)
  {
    myColorPerVertex = isColorPerVertex;
  }

  /**
   * Query the shape. This method checks the flag myIsModified; if True it
   * should rebuild the shape presentation.
   */
  Standard_EXPORT virtual const Handle(TopoDS_TShape)& TShape() Standard_OVERRIDE;

  /**
   * Create a copy of this node.
   * If the parameter is null, a new copied node is created. Otherwise new node
   * is not created, but rather the given one is modified.
   */
  Standard_EXPORT virtual Handle(VrmlData_Node) Clone(const Handle(VrmlData_Node)& theOther) const
    Standard_OVERRIDE;

  /**
   * Read the Node from input stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus Read(VrmlData_InBuffer& theBuffer) Standard_OVERRIDE;

  /**
   * Write the Node to output stream.
   */
  Standard_EXPORT virtual VrmlData_ErrorStatus Write(const char* thePrefix) const Standard_OVERRIDE;

  /**
   * Returns True if the node is default, so that it should not be written.
   */
  Standard_EXPORT virtual Standard_Boolean IsDefault() const Standard_OVERRIDE;

private:
  // ---------- PRIVATE FIELDS ----------
  Handle(VrmlData_Coordinate) myCoords;
  Handle(VrmlData_Color)      myColors;
  const Standard_Integer**    myArrPolygons;
  const Standard_Integer**    myArrColorInd;
  Standard_Size               myNbPolygons;
  Standard_Size               myNbColors;
  Standard_Boolean            myColorPerVertex;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(VrmlData_IndexedLineSet, VrmlData_Geometry)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE(VrmlData_IndexedLineSet, VrmlData_Geometry)

#endif
