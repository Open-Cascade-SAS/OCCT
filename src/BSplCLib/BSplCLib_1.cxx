#include <BSplCLib.hxx>

#include <Standard_NotImplemented.hxx>
// BSpline Curve in 2d space
// **************************

#define No_Standard_RangeError
#define No_Standard_OutOfRange

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

