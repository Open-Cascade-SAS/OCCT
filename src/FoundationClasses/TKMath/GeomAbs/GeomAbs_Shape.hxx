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

#ifndef _GeomAbs_Shape_HeaderFile
#define _GeomAbs_Shape_HeaderFile

//! Provides information about the continuity of a curve:
//! -   C0: only geometric continuity.
//! -   G1: for each point on the curve, the tangent vectors
//! "on the right" and "on the left" are collinear with the same orientation.
//! -   C1: continuity of the first derivative. The "C1" curve is
//! also "G1" but, in addition, the tangent vectors " on the
//! right" and "on the left" are equal.
//! -   G2: for each point on the curve, the normalized
//! normal vectors "on the right" and "on the left" are equal.
//! -   C2: continuity of the second derivative.
//! -   C3: continuity of the third derivative.
//! -   CN: continuity of the N-th derivative, whatever is the
//! value given for N (infinite order of continuity).
//! Also provides information about the continuity of a surface:
//! -   C0: only geometric continuity.
//! -   C1: continuity of the first derivatives; any
//! isoparametric (in U or V) of a surface "C1" is also "C1".
//! -   G2: for BSpline curves only; "on the right" and "on the
//! left" of a knot the computation of the "main curvature
//! radii" and the "main directions" (when they exist) gives the same result.
//! -   C2: continuity of the second derivative.
//! -   C3: continuity of the third derivative.
//! -   CN: continuity of any N-th derivative, whatever is the
//! value given for N (infinite order of continuity).
//! We may also say that a surface is "Ci" in u, and "Cj" in v
//! to indicate the continuity of its derivatives up to the order
//! i in the u parametric direction, and j in the v parametric direction.
enum GeomAbs_Shape
{
  GeomAbs_C0,
  GeomAbs_G1,
  GeomAbs_C1,
  GeomAbs_G2,
  GeomAbs_C2,
  GeomAbs_C3,
  GeomAbs_CN
};

#endif // _GeomAbs_Shape_HeaderFile
