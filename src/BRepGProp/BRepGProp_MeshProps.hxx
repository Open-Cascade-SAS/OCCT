// Copyright (c) 2018 OPEN CASCADE SAS
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

#ifndef _BRepGProp_MeshProps_HeaderFile
#define _BRepGProp_MeshProps_HeaderFile

#include <GProp_GProps.hxx>
#include <TopAbs_Orientation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TColgp_Array1OfPnt.hxx>

class Poly_Triangulation;
class TopLoc_Location;


//! Computes the global properties of a surface mesh. The mesh can be
//! interpreted as just a surface or as a piece of volume limited by this surface.
class BRepGProp_MeshProps : public GProp_GProps
{
public:

  DEFINE_STANDARD_ALLOC

  //! Describes types of geometric objects.
  //! - Vinert is 3D closed region of space delimited with
  //!   Point and surface mesh;
  //! - Sinert is surface mesh in 3D space.
  typedef enum { Vinert = 0, Sinert } BRepGProp_MeshObjType;

  //! Constructor takes the type of object.
  BRepGProp_MeshProps(const BRepGProp_MeshObjType theType) :
    myType(theType)
  {}

  //! Sets the point relative which the calculation is to be done
  void SetLocation(const gp_Pnt& theLocation) { loc = theLocation; }

  //! Computes the global properties of a surface mesh of 3D space.
  //! Calculation of surface properties is performed by numerical integration
  //! over triangle surfaces using Gauss cubature formulas. 
  //! Depending on the mesh object type used in constructor this method can 
  //! calculate the surface or volume properties of the mesh.
  Standard_EXPORT void Perform(const Handle(Poly_Triangulation)& theMesh,
                               const TopLoc_Location& theLoc,
                               const TopAbs_Orientation theOri);

  Standard_EXPORT void Perform(const Handle(Poly_Triangulation)& theMesh,
                               const TopAbs_Orientation theOri);

  //! Computes the global properties of triangle {p1, p2, p3} relatively 
  //! point Apex
  //! If isVolume = true, volume properties are calculated
  //! otherwise - surface ones
  Standard_EXPORT static void CalculateProps(const gp_Pnt& p1, const gp_Pnt& p2, const gp_Pnt& p3,
                                             const gp_Pnt& Apex,
                                             const Standard_Boolean isVolume,
                                             Standard_Real GProps[10],
                                             const Standard_Integer NbGaussPoints,
                                             const Standard_Real* GaussPnts);

  //! Get type of mesh object
  BRepGProp_MeshObjType GetMeshObjType() const
  {
    return myType;
  }

private: //! @name private fields


  BRepGProp_MeshObjType myType; //!< Type of geometric object

};

#endif // _BRepGProp_MeshProps_HeaderFile
