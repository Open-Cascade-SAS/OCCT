// File:	BSplCLib_3.cxx
// Created:	Fri May 19 18:33:07 1995
// Author:	Xavier BENVENISTE
//		<xab@nonox>

#include <BSplCLib.ixx>
#include <Standard_NotImplemented.hxx>

// BSpline Curve in 3d space
// ***************************

#define No_Standard_RangeError
#define No_Standard_OutOfRange

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
  
