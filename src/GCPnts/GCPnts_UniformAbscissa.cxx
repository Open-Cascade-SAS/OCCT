#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ConstructionError.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor2d_Curve2d.hxx>

//=======================================================================
//function : GCPnts_UniformAbscissa
//purpose  : 
//=======================================================================

GCPnts_UniformAbscissa::GCPnts_UniformAbscissa ()
     :myDone(Standard_False) 
{
} 

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>

#define TheCurve                 Adaptor3d_Curve
#define Handle_TheBezierCurve   Handle(Geom_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom_BSplineCurve)

#include <GCPnts_UniformAbscissa.gxx>

#undef TheCurve
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

#define TheCurve                 Adaptor2d_Curve2d
#define Handle_TheBezierCurve   Handle(Geom2d_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom2d_BSplineCurve)

#include <GCPnts_UniformAbscissa.gxx>



