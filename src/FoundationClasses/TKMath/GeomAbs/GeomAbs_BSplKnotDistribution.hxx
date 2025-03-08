// Created on: 1993-02-22
// Created by: Modelistation
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _GeomAbs_BSplKnotDistribution_HeaderFile
#define _GeomAbs_BSplKnotDistribution_HeaderFile

//! This enumeration is used in the classes BSplineCurve and
//! BSplineSurface to describe the repartition of set of knots.
//! (comments in classes BSplineCurve and BSplineSurface)
enum GeomAbs_BSplKnotDistribution
{
  GeomAbs_NonUniform,
  GeomAbs_Uniform,
  GeomAbs_QuasiUniform,
  GeomAbs_PiecewiseBezier
};

#endif // _GeomAbs_BSplKnotDistribution_HeaderFile
