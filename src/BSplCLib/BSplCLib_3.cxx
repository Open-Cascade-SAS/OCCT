// Created on: 1995-05-19
// Created by: Xavier BENVENISTE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BSplCLib.ixx>
#include <Standard_NotImplemented.hxx>

// BSpline Curve in 3d space
// ***************************

#define Dimension_gen 3

#define Array1OfPoints  TColgp_Array1OfPnt
#define Point           gp_Pnt
#define Vector          gp_Vec

#define PointToCoords(carr,pnt,op) \
(carr)[0] = (pnt).X() op,  \
        (carr)[1] = (pnt).Y() op,  \
        (carr)[2] = (pnt).Z() op

#define CoordsToPoint(pnt,carr,op) \
        (pnt).SetX ((carr)[0] op), \
        (pnt).SetY ((carr)[1] op), \
        (pnt).SetZ ((carr)[2] op)

#define NullifyPoint(pnt) \
        (pnt).SetCoord (0.,0.,0.)

#define NullifyCoords(carr) \
        (carr)[0] = (carr)[1] = (carr)[2] = 0.

#define ModifyCoords(carr,op) \
        (carr)[0] op,          \
        (carr)[1] op,          \
        (carr)[2] op

#define CopyCoords(carr,carr2) \
        (carr)[0] = (carr2)[0], \
        (carr)[1] = (carr2)[1], \
        (carr)[2] = (carr2)[2]

#define BSplCLib_DataContainer BSplCLib_DataContainer_3d
  
#include <BSplCLib_CurveComputation.gxx>
