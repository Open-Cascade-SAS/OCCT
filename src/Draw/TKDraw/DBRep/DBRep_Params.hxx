// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef Draw_Params_HeaderFile
#define Draw_Params_HeaderFile

#include <Standard_Real.hxx>

//! DBRep parameters.
struct DBRep_Params
{
public:
  Standard_Integer NbIsos; //!< number of iso in U and V
  Standard_Real    Size;
  Standard_Integer Discretization; //!< Discretization number of points for curves
  Standard_Boolean DispTriangles;
  Standard_Boolean DisplayPolygons;
  Standard_Real    HLRAngle; //!< Discretization angle for edges
  Standard_Real    HAngMin;
  Standard_Real    HAngMax;
  Standard_Boolean WithHLR; //!< True if HLR, False if wireframe
  Standard_Boolean WithRg1; //!< True if display Rg1Lines
  Standard_Boolean WithRgN; //!< True if display RgNLines
  Standard_Boolean WithHid; //!< True if display HiddenLines

  DBRep_Params()
      : NbIsos(2),
        Size(100.0),
        Discretization(30),
        DispTriangles(false),
        DisplayPolygons(false),
        HLRAngle(35.0 * M_PI / 180.0),
        HAngMin(1.0 * M_PI / 180.0),
        HAngMax(35.0 * M_PI / 180.0),
        WithHLR(false),
        WithRg1(true),
        WithRgN(false),
        WithHid(false)
  {
  }
};

#endif
