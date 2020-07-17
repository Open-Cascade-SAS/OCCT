// Created on: 2016-07-07
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

#ifndef _BRepMesh_NURBSRangeSplitter_HeaderFile
#define _BRepMesh_NURBSRangeSplitter_HeaderFile

#include <BRepMesh_UVParamRangeSplitter.hxx>
#include <IMeshData_Types.hxx>
#include <IMeshTools_Parameters.hxx>

//! Auxiliary class extending UV range splitter in order to generate
//! internal nodes for NURBS surface.
class BRepMesh_NURBSRangeSplitter : public BRepMesh_UVParamRangeSplitter
{
public:

  //! Constructor.
  BRepMesh_NURBSRangeSplitter()
  : mySurfaceType(GeomAbs_OtherSurface)
  {
  }

  //! Destructor.
  virtual ~BRepMesh_NURBSRangeSplitter()
  {
  }

  //! Updates discrete range of surface according to its geometric range.
  Standard_EXPORT virtual void AdjustRange() Standard_OVERRIDE;

  //! Returns list of nodes generated using surface data and specified parameters.
  Standard_EXPORT virtual Handle(IMeshData::ListOfPnt2d) GenerateSurfaceNodes(
    const IMeshTools_Parameters& theParameters) const Standard_OVERRIDE;

protected:

  //! Initializes U and V parameters lists using CN continuity intervals.
  Standard_EXPORT virtual Standard_Boolean initParameters() const;

private:

  //! Computes parameters of filter and applies it to the source parameters.
  Handle(IMeshData::SequenceOfReal) computeGrainAndFilterParameters(
    const IMeshData::IMapOfReal&            theSourceParams,
    const Standard_Real                     theTol2d,
    const Standard_Real                     theRangeDiff,
    const Standard_Real                     theDelta,
    const IMeshTools_Parameters&            theParameters,
    const Handle(NCollection_IncAllocator)& theAllocator) const;

  //! Filters parameters in order to avoid too dence distribution.
  Handle(IMeshData::SequenceOfReal) filterParameters(
    const IMeshData::IMapOfReal&            theParams,
    const Standard_Real                     theMinDist,
    const Standard_Real                     theFilterDist,
    const Handle(NCollection_IncAllocator)& theAllocator) const;

  enum EdgeType
  {
    Edge_Internal,
    Edge_Frontier
  };

  enum ParamDimension
  {
    Param_U = 0x1,
    Param_V = 0x2
  };

  //! Finds edges of discrete face and uses its points 
  //! as auxiliary control parameters for generation of nodes.
  Standard_Boolean grabParamsOfEdges (const EdgeType         theEdgeType,
                                      const Standard_Integer theParamDimensionFlag) const;

private:

  GeomAbs_SurfaceType mySurfaceType;
};

#endif
