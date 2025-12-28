// Created on: 2016-04-19
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_EdgeTessellationExtractor_HeaderFile
#define _BRepMesh_EdgeTessellationExtractor_HeaderFile

#include <IMeshTools_CurveTessellator.hxx>
#include <BRepMesh_EdgeParameterProvider.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <TopLoc_Location.hxx>

//! Auxiliary class implements functionality retrieving tessellated
//! representation of an edge stored in polygon.
class BRepMesh_EdgeTessellationExtractor : public IMeshTools_CurveTessellator
{
public:
  //! Constructor.
  Standard_EXPORT BRepMesh_EdgeTessellationExtractor(const IMeshData::IEdgeHandle& theEdge,
                                                     const IMeshData::IFaceHandle& theFace);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_EdgeTessellationExtractor();

  //! Returns number of tessellation points.
  Standard_EXPORT virtual int PointsNb() const override;

  //! Returns parameters of solution with the given index.
  //! @param theIndex index of tessellation point.
  //! @param theParameter parameters on PCurve corresponded to the solution.
  //! @param thePoint tessellation point.
  //! @return True in case of valid result, false elewhere.
  Standard_EXPORT virtual bool Value(const int theIndex,
                                     gp_Pnt&   thePoint,
                                     double&   theParameter) const override;

  DEFINE_STANDARD_RTTIEXT(BRepMesh_EdgeTessellationExtractor, IMeshTools_CurveTessellator)

private:
  BRepMesh_EdgeParameterProvider<occ::handle<NCollection_HArray1<double>>> myProvider;
  const Poly_Triangulation*                                                myTriangulation;
  const NCollection_Array1<int>*                                           myIndices;
  TopLoc_Location                                                          myLoc;
};

#endif
