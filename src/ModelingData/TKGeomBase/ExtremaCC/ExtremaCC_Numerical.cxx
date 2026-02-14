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

#include <ExtremaCC_Numerical.hxx>

// Include curve evaluator types
#include <ExtremaCC_Circle.hxx>
#include <ExtremaCC_CurveAdapter.hxx>
#include <ExtremaCC_Ellipse.hxx>
#include <ExtremaCC_Hyperbola.hxx>
#include <ExtremaCC_Line.hxx>
#include <ExtremaCC_Parabola.hxx>

// This file provides explicit template instantiations for numerical curve pairs.
// ExtremaCC_CurveAdapter is the unified adapter for all non-elementary curves.

//==================================================================================================
// CurveAdapter pairs (for general curve types: Bezier, BSpline, Offset, Other)
//==================================================================================================

template class ExtremaCC_Numerical<ExtremaCC_CurveAdapter, ExtremaCC_CurveAdapter>;

//==================================================================================================
// Mixed pairs: Elementary x CurveAdapter
//==================================================================================================

template class ExtremaCC_Numerical<ExtremaCC_Line, ExtremaCC_CurveAdapter>;
template class ExtremaCC_Numerical<ExtremaCC_CurveAdapter, ExtremaCC_Line>;

template class ExtremaCC_Numerical<ExtremaCC_Circle, ExtremaCC_CurveAdapter>;
template class ExtremaCC_Numerical<ExtremaCC_CurveAdapter, ExtremaCC_Circle>;

template class ExtremaCC_Numerical<ExtremaCC_Ellipse, ExtremaCC_CurveAdapter>;
template class ExtremaCC_Numerical<ExtremaCC_CurveAdapter, ExtremaCC_Ellipse>;

template class ExtremaCC_Numerical<ExtremaCC_Hyperbola, ExtremaCC_CurveAdapter>;
template class ExtremaCC_Numerical<ExtremaCC_CurveAdapter, ExtremaCC_Hyperbola>;

template class ExtremaCC_Numerical<ExtremaCC_Parabola, ExtremaCC_CurveAdapter>;
template class ExtremaCC_Numerical<ExtremaCC_CurveAdapter, ExtremaCC_Parabola>;
