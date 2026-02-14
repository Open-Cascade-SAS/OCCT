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

// Include all curve evaluator types
#include <ExtremaCC_BezierCurve.hxx>
#include <ExtremaCC_BSplineCurve.hxx>
#include <ExtremaCC_Circle.hxx>
#include <ExtremaCC_Ellipse.hxx>
#include <ExtremaCC_Hyperbola.hxx>
#include <ExtremaCC_Line.hxx>
#include <ExtremaCC_OffsetCurve.hxx>
#include <ExtremaCC_OtherCurve.hxx>
#include <ExtremaCC_Parabola.hxx>

// This file provides explicit template instantiations for all 30 numerical curve pairs.
// These instantiations ensure the template code is compiled into the library.

//==================================================================================================
// Mixed pairs: Elementary x Numerical (20 pairs)
//==================================================================================================

// Line x Numerical (5)
template class ExtremaCC_Numerical<ExtremaCC_Line, ExtremaCC_BezierCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Line, ExtremaCC_BSplineCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Line, ExtremaCC_OffsetCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Line, ExtremaCC_OtherCurve>;
template class ExtremaCC_Numerical<ExtremaCC_BezierCurve, ExtremaCC_Line>;

// Circle x Numerical (4 - Line already covered above via BezierCurve-Line)
template class ExtremaCC_Numerical<ExtremaCC_Circle, ExtremaCC_BezierCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Circle, ExtremaCC_BSplineCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Circle, ExtremaCC_OffsetCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Circle, ExtremaCC_OtherCurve>;

// Ellipse x Numerical (4)
template class ExtremaCC_Numerical<ExtremaCC_Ellipse, ExtremaCC_BezierCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Ellipse, ExtremaCC_BSplineCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Ellipse, ExtremaCC_OffsetCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Ellipse, ExtremaCC_OtherCurve>;

// Hyperbola x Numerical (4)
template class ExtremaCC_Numerical<ExtremaCC_Hyperbola, ExtremaCC_BezierCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Hyperbola, ExtremaCC_BSplineCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Hyperbola, ExtremaCC_OffsetCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Hyperbola, ExtremaCC_OtherCurve>;

// Parabola x Numerical (4)
template class ExtremaCC_Numerical<ExtremaCC_Parabola, ExtremaCC_BezierCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Parabola, ExtremaCC_BSplineCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Parabola, ExtremaCC_OffsetCurve>;
template class ExtremaCC_Numerical<ExtremaCC_Parabola, ExtremaCC_OtherCurve>;

//==================================================================================================
// Pure numerical pairs (10 pairs)
//==================================================================================================

// BezierCurve x Numerical (4)
template class ExtremaCC_Numerical<ExtremaCC_BezierCurve, ExtremaCC_BezierCurve>;
template class ExtremaCC_Numerical<ExtremaCC_BezierCurve, ExtremaCC_BSplineCurve>;
template class ExtremaCC_Numerical<ExtremaCC_BezierCurve, ExtremaCC_OffsetCurve>;
template class ExtremaCC_Numerical<ExtremaCC_BezierCurve, ExtremaCC_OtherCurve>;

// BSplineCurve x Numerical (3 - BezierCurve already covered above)
template class ExtremaCC_Numerical<ExtremaCC_BSplineCurve, ExtremaCC_BSplineCurve>;
template class ExtremaCC_Numerical<ExtremaCC_BSplineCurve, ExtremaCC_OffsetCurve>;
template class ExtremaCC_Numerical<ExtremaCC_BSplineCurve, ExtremaCC_OtherCurve>;

// OffsetCurve x Numerical (2 - Bezier and BSpline already covered above)
template class ExtremaCC_Numerical<ExtremaCC_OffsetCurve, ExtremaCC_OffsetCurve>;
template class ExtremaCC_Numerical<ExtremaCC_OffsetCurve, ExtremaCC_OtherCurve>;

// OtherCurve x OtherCurve (1)
template class ExtremaCC_Numerical<ExtremaCC_OtherCurve, ExtremaCC_OtherCurve>;
