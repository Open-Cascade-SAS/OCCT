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

#ifndef _GeomGridEval_HeaderFile
#define _GeomGridEval_HeaderFile

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//! @brief Namespace containing result structures for grid evaluators.
//!
//! Provides lightweight POD structures for returning evaluation results
//! with derivatives from curve and surface grid evaluators.
namespace GeomGridEval
{

//! Result structure for curve D1 evaluation (point and first derivative).
struct CurveD1
{
  gp_Pnt Point;
  gp_Vec D1;
};

//! Result structure for curve D2 evaluation (point and first two derivatives).
struct CurveD2
{
  gp_Pnt Point;
  gp_Vec D1;
  gp_Vec D2;
};

//! Result structure for curve D3 evaluation (point and first three derivatives).
struct CurveD3
{
  gp_Pnt Point;
  gp_Vec D1;
  gp_Vec D2;
  gp_Vec D3;
};

//! Result structure for surface D1 evaluation (point and partial derivatives).
struct SurfD1
{
  gp_Pnt Point;
  gp_Vec D1U;
  gp_Vec D1V;
};

//! Result structure for surface D2 evaluation (point and partial derivatives up to 2nd order).
struct SurfD2
{
  gp_Pnt Point;
  gp_Vec D1U;
  gp_Vec D1V;
  gp_Vec D2U;
  gp_Vec D2V;
  gp_Vec D2UV;
};

//! Result structure for surface D3 evaluation (point and partial derivatives up to 3rd order).
struct SurfD3
{
  gp_Pnt Point;
  gp_Vec D1U;
  gp_Vec D1V;
  gp_Vec D2U;
  gp_Vec D2V;
  gp_Vec D2UV;
  gp_Vec D3U;
  gp_Vec D3V;
  gp_Vec D3UUV;
  gp_Vec D3UVV;
};

} // namespace GeomGridEval

#endif // _GeomGridEval_HeaderFile
