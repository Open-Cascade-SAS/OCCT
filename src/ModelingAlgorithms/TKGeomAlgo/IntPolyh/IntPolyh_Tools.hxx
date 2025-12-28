// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _IntPolyh_Tools_HeaderFile
#define _IntPolyh_Tools_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <NCollection_Array1.hxx>
#include <IntPolyh_ArrayOfPointNormal.hxx>

//! The class provides tools for surface sampling.
class IntPolyh_Tools
{
public:
  //! Checks if the surface can be enlarged in U or V direction.
  Standard_EXPORT static void IsEnlargePossible(const occ::handle<Adaptor3d_Surface>& theSurf,
                                                bool&                theUEnlarge,
                                                bool&                theVEnlarge);

  //! Makes the sampling of the given surface <theSurf>
  //! making the net of <theNbSU> x <theNbSV> sampling points.
  //! The flag <theEnlargeZone> controls the enlargement of the
  //! sampling zone on the surface.
  //! The parameters of the sampling points are stored into
  //! <theUPars> and <theVPars> arrays.
  Standard_EXPORT static void MakeSampling(const occ::handle<Adaptor3d_Surface>& theSurf,
                                           const int           theNbSU,
                                           const int           theNbSV,
                                           const bool           theEnlargeZone,
                                           NCollection_Array1<double>&            theUPars,
                                           NCollection_Array1<double>&            theVPars);

  //! Computes the deflection tolerance on the surface for the given sampling.
  Standard_EXPORT static double ComputeDeflection(const occ::handle<Adaptor3d_Surface>& theSurf,
                                                         const NCollection_Array1<double>&      theUPars,
                                                         const NCollection_Array1<double>&      theVPars);

  //! Fills the array <thePoints> with the points (triangulation nodes) on the surface
  //! and normal directions of the surface in these points.
  Standard_EXPORT static void FillArrayOfPointNormal(const occ::handle<Adaptor3d_Surface>& theSurf,
                                                     const NCollection_Array1<double>&      theUPars,
                                                     const NCollection_Array1<double>&      theVPars,
                                                     IntPolyh_ArrayOfPointNormal&     thePoints);
};

#endif // _IntPolyh_Tools_HeaderFile
