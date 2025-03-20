// Created on: 2016-04-07
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

#ifndef _IMeshTools_Parameters_HeaderFile
#define _IMeshTools_Parameters_HeaderFile

#include <IMeshTools_MeshAlgoType.hxx>
#include <Precision.hxx>

//! Structure storing meshing parameters
struct IMeshTools_Parameters
{

  //! Default constructor
  IMeshTools_Parameters()
      : MeshAlgo(IMeshTools_MeshAlgoType_DEFAULT),
        Angle(0.5),
        Deflection(0.001),
        AngleInterior(-1.0),
        DeflectionInterior(-1.0),
        MinSize(-1.0),
        InParallel(Standard_False),
        Relative(Standard_False),
        InternalVerticesMode(Standard_True),
        ControlSurfaceDeflection(Standard_True),
        EnableControlSurfaceDeflectionAllSurfaces(Standard_False),
        CleanModel(Standard_True),
        AdjustMinSize(Standard_False),
        ForceFaceDeflection(Standard_False),
        AllowQualityDecrease(Standard_False)
  {
  }

  //! Returns factor used to compute default value of MinSize
  //! (minimum mesh edge length) from deflection
  static Standard_Real RelMinSize() { return 0.1; }

  //! 2D Delaunay triangulation algorithm factory to use
  IMeshTools_MeshAlgoType MeshAlgo;

  //! Angular deflection used to tessellate the boundary edges
  Standard_Real Angle;

  //! Linear deflection used to tessellate the boundary edges
  Standard_Real Deflection;

  //! Angular deflection used to tessellate the face interior
  Standard_Real AngleInterior;

  //! Linear deflection used to tessellate the face interior
  Standard_Real DeflectionInterior;

  //! Minimum size parameter limiting size of triangle's edges to prevent
  //! sinking into amplification in case of distorted curves and surfaces.
  Standard_Real MinSize;

  //! Switches on/off multi-thread computation
  Standard_Boolean InParallel;

  //! Switches on/off relative computation of edge tolerance<br>
  //! If true, deflection used for the polygonalisation of each edge will be
  //! <defle> * Size of Edge. The deflection used for the faces will be the
  //! maximum deflection of their edges.
  Standard_Boolean Relative;

  //! Mode to take or not to take internal face vertices into account
  //! in triangulation process
  Standard_Boolean InternalVerticesMode;

  //! Parameter to check the deviation of triangulation and interior of
  //! the face
  Standard_Boolean ControlSurfaceDeflection;

  // Enables/disables check triggered by ControlSurfaceDeflection flag
  // for all types of surfaces including analytical.
  Standard_Boolean EnableControlSurfaceDeflectionAllSurfaces;

  //! Cleans temporary data model when algorithm is finished.
  Standard_Boolean CleanModel;

  //! Enables/disables local adjustment of min size depending on edge size.
  //! Disabled by default.
  Standard_Boolean AdjustMinSize;

  //! Enables/disables usage of shape tolerances for computing face deflection.
  //! Disabled by default.
  Standard_Boolean ForceFaceDeflection;

  //! Allows/forbids the decrease of the quality of the generated mesh
  //! over the existing one.
  Standard_Boolean AllowQualityDecrease;
};

#endif
