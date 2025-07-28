// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _HelixGeom_Tools_HeaderFile
#define _HelixGeom_Tools_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>

class Geom_BSplineCurve;

//! Static utility class providing approximation algorithms for helix curves.
//!
//! This class contains static methods for:
//! - Converting analytical helix curves to B-spline approximations
//! - Generic curve approximation with specified tolerances and continuity
//! - High-quality approximation suitable for CAD/CAM applications
//!
//! The approximation algorithms use advanced techniques to ensure:
//! - Accurate representation within specified tolerances
//! - Smooth continuity (C0, C1, C2) as required
//! - Efficient B-spline parameterization
//! - Robust handling of edge cases
class HelixGeom_Tools
{
public:
  DEFINE_STANDARD_ALLOC

  //! Approximates a parametric helix curve using B-spline representation.
  //! @param aT1 [in] Start parameter (angular position in radians)
  //! @param aT2 [in] End parameter (angular position in radians)
  //! @param aPitch [in] Helix pitch (vertical distance per 2*PI radians)
  //! @param aRStart [in] Starting radius at parameter aT1
  //! @param aTaperAngle [in] Taper angle in radians (0 = cylindrical helix)
  //! @param aIsCW [in] True for clockwise, false for counter-clockwise
  //! @param aTol [in] Approximation tolerance
  //! @param theBSpl [out] Resulting B-spline curve
  //! @param theMaxError [out] Maximum approximation error achieved
  //! @return 0 on success, error code otherwise
  Standard_EXPORT static Standard_Integer ApprHelix(const Standard_Real        aT1,
                                                    const Standard_Real        aT2,
                                                    const Standard_Real        aPitch,
                                                    const Standard_Real        aRStart,
                                                    const Standard_Real        aTaperAngle,
                                                    const Standard_Boolean     aIsCW,
                                                    const Standard_Real        aTol,
                                                    Handle(Geom_BSplineCurve)& theBSpl,
                                                    Standard_Real&             theMaxError);

  //! Approximates a generic 3D curve using B-spline representation.
  //! @param theHC [in] Handle to the curve adaptor to approximate
  //! @param theTol [in] Approximation tolerance
  //! @param theCont [in] Required continuity (C0, C1, C2)
  //! @param theMaxSeg [in] Maximum number of curve segments
  //! @param theMaxDeg [in] Maximum degree of B-spline curve
  //! @param theBSpl [out] Resulting B-spline curve
  //! @param theMaxError [out] Maximum approximation error achieved
  //! @return 0 on success, error code otherwise
  Standard_EXPORT static Standard_Integer ApprCurve3D(const Handle(Adaptor3d_Curve)& theHC,
                                                      const Standard_Real            theTol,
                                                      const GeomAbs_Shape            theCont,
                                                      const Standard_Integer         theMaxSeg,
                                                      const Standard_Integer         theMaxDeg,
                                                      Handle(Geom_BSplineCurve)&     theBSpl,
                                                      Standard_Real&                 theMaxError);
};

#endif // _HelixGeom_Tools_HeaderFile
