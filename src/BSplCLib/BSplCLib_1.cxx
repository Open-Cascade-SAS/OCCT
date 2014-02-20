// Copyright (c) 1995-1999 Matra Datavision
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

#include <BSplCLib.hxx>

#include <Standard_NotImplemented.hxx>
// BSpline Curve in 2d space
// **************************

#define Dimension_gen 2

#define Array1OfPoints  TColgp_Array1OfPnt2d
#define Point           gp_Pnt2d
#define Vector          gp_Vec2d

#define PointToCoords(carr,pnt,op) \
(carr)[0] = (pnt).X() op,  \
        (carr)[1] = (pnt).Y() op

#define CoordsToPoint(pnt,carr,op) \
        (pnt).SetX ((carr)[0] op), \
        (pnt).SetY ((carr)[1] op)

#define NullifyPoint(pnt) \
        (pnt).SetCoord (0.,0.)

#define NullifyCoords(carr) \
        (carr)[0] = (carr)[1] = 0.

#define ModifyCoords(carr,op) \
        (carr)[0] op,          \
        (carr)[1] op

#define CopyCoords(carr,carr2) \
        (carr)[0] = (carr2)[0], \
        (carr)[1] = (carr2)[1]

#define BSplCLib_DataContainer BSplCLib_DataContainer_2d  
  
#include <BSplCLib_CurveComputation.gxx>
