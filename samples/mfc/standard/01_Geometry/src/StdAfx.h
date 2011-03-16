// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#if !defined(WNT)
   #error WNT precompiler directive is mandatory for CasCade 
#endif

#pragma warning(  disable : 4244 )        // Issue warning 4244
#include "Standard_ShortReal.hxx"
#pragma warning(  default : 4244 )        // Issue warning 4244

#include <Standard.hxx>
/*
#ifndef Version15B
# ifndef Version15D
#  ifndef Version20
#   define Version15B
#  endif // Version20
# endif // Version15D
#endif // Version15B

#pragma message ("=============================")
#ifdef Version15B
# pragma message ("Set the libs for version 1.5B")
#endif // Version15B

#ifdef Version15D
# pragma message ("Set the libs for version 1.5D")
#endif // Version15D

#ifdef Version20
# pragma message ("Set the libs for version 2.0 ")
#endif // Version20
#pragma message ("=============================")

#ifdef Version15B
# pragma comment (lib,"TKTop.lib")
# pragma comment (lib,"TShort.lib")
# pragma comment (lib,"TColQuantity.lib")
#endif

#ifdef Version15D
# pragma comment (lib,"TKTop.lib")
#endif

#ifdef Version20
# pragma comment (lib,"TKTop1.lib")
# pragma comment (lib,"TKTop2.lib")
#endif

#pragma message ("Set the specific libs for the application")
# pragma comment (lib,"TKPrs.lib")
# pragma comment (lib,"TKGeom.lib")
# pragma comment (lib,"TKGlt.lib")
# pragma comment (lib,"TKGraphic.lib")
# pragma comment (lib,"TKPrsMgr.lib")
# pragma comment (lib,"TKViewers.lib")
# pragma comment (lib,"TKSession.lib")
# pragma comment (lib,"Bnd.lib")
# pragma comment (lib,"gp.lib")
# pragma comment (lib,"TColgp.lib")
# pragma comment (lib,"TKernel.lib")
# pragma comment (lib,"TKXSBase.lib")
# pragma comment (lib,"TKXSIGES.lib")
# pragma comment (lib,"TKXS214.lib")
# pragma comment (lib,"UnitsAPI.lib")
*/
 
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
//#include <AIS_Trihedron.hxx>
#include <BRepAlgo.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <WNT_Window.hxx>


#include <Standard_ErrorHandler.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
//#include <ShapeTool_Face.hxx>
//#include <ShapeTool_PCurves.hxx>
//#include <ShapeTool_Reshape.hxx>
//#include <ShapeTool_ShellSolid.hxx>
#include <TopoDS_ListIteratorOfListOfShape.hxx>

#include "WNT_WDriver.hxx"
#include "V2d_View.hxx"
#include "V2d_Viewer.hxx"


//#include "UnitsAPI.hxx"


#include "GProp_PEquation.hxx"
#include "gp.hxx"
#include "TColgp_Array1OfPnt.hxx"
#include "Geom2d_Circle.hxx"
#include "gp_Vec2d.hxx"
#include "Geom2d_Circle.hxx"
#include "Geom2dLProp_Curve2dTool.hxx"

#include "Geom2dLProp_CLProps2d.hxx"
#include "gp_Dir.hxx"
#include "Precision.hxx"
#include "GCPnts_AbscissaPoint.hxx"
#include "Geom2dAdaptor_Curve.hxx"
#include "GCPnts_UniformAbscissa.hxx"
#include "Geom_SphericalSurface.hxx"
#include "Geom_Circle.hxx"
#include "GC_MakeTranslation.hxx"
#include "Geom_Transformation.hxx"
#include "GC_MakeCircle.hxx"
#include "gp_Mat.hxx"
#include "GeomAPI_IntCS.hxx"
#include "GC_MakeSegment.hxx"
#include "GC_MakeConicalSurface.hxx"
#include "gp_Lin.hxx"
#include "Geom2d_Ellipse.hxx"
#include "GCE2d_MakeEllipse.hxx"
#include "GCE2d_MakeArcOfEllipse.hxx"
#include "TColgp_HArray1OfPnt.hxx"
#include "Geom_BSplineCurve.hxx"
#include "GeomAPI_PointsToBSpline.hxx"
#include "GeomAPI_Interpolate.hxx"
#include "GccEnt_QualifiedLin.hxx"
#include "GccAna_Circ2d2TanRad.hxx"
#include "gce_MakeCirc2d.hxx"
#include "GccEnt.hxx"
#include "GccAna_Lin2d2Tan.hxx"
#include "GccEnt_QualifiedCirc.hxx"
#include "gce_MakeLin2d.hxx"
#include "Geom2d_OffsetCurve.hxx"
#include "Geom2dAPI_InterCurveCurve.hxx"
#include "Geom2dAPI_ExtremaCurveCurve.hxx"
#include "GeomAPI.hxx"
#include "Geom2d_TrimmedCurve.hxx"
#include "GCE2d_MakeSegment.hxx"
#include "Geom2dConvert.hxx"
#include "TColgp_Array1OfPnt2d.hxx"
#include "TColgp_HArray1OfPnt2d.hxx"
#include "gp_Parab2d.hxx"
#include "gp_Trsf.hxx"
#include "gp_Pln.hxx"
#include "gp_Elips.hxx"
#include "IntAna_IntConicQuad.hxx"
#include "GeomAPI_ProjectPointOnSurf.hxx"
#include "GeomAPI_ProjectPointOnCurve.hxx"
#include "Geom2dAPI_PointsToBSpline.hxx"
#include "Geom2dAPI_Interpolate.hxx"
#include "GccAna_Pnt2dBisec.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "GeomTools.hxx"

#include "TColgp_SequenceOfPnt2d.hxx"
#include "GC_MakePlane.hxx"
#include "GC_MakeEllipse.hxx"
#include "Geom_Plane.hxx"
#include "Geom2d_Line.hxx"
#include "Geom_Circle.hxx"
#include "AIS_Drawer.hxx"
#include "Prs3d_IsoAspect.hxx"
#include "GCE2d_MakeArcOfCircle.hxx"
#include "Geom_Line.hxx"
#include "Geom2d_BSplineCurve.hxx"
#include "TColgp_Array2OfPnt2d.hxx"
#include "Geom_RectangularTrimmedSurface.hxx"
#include "GeomFill_FillingStyle.hxx"
#include "GeomFill_BSplineCurves.hxx"
#include "TopoDS_Vertex.hxx"
#include "TopoDS_Shape.hxx"
#include "TopoDS_Face.hxx"

#include "GeomAdaptor_HCurve.hxx"
#include "GeomFill_SimpleBound.hxx"
#include "GeomFill_Pipe.hxx"
#include "GeomConvert.hxx"
#include "TColgp_Array2OfPnt.hxx"
#include "Geom_BezierSurface.hxx"
#include "TColGeom_Array2OfBezierSurface.hxx"
#include "GeomConvert_CompBezierSurfacesToBSplineSurface.hxx"
#include "TColStd_HArray1OfInteger.hxx"
#include "Geom_OffsetSurface.hxx"
#include "Geom_SurfaceOfLinearExtrusion.hxx"
#include "Geom_SurfaceOfRevolution.hxx"
#include "GeomLib.hxx"
#include "Geom_BSplineSurface.hxx"
#include "Geom2d_Parabola.hxx"
#include "GCE2d_MakeParabola.hxx"
#include "Geom_TrimmedCurve.hxx"
#include "GeomFill_ConstrainedFilling.hxx"

#include "Geom_Curve.hxx"
#include "Geom_Surface.hxx"
#include "BndLib_Add2dCurve.hxx"
#include "BndLib.hxx"
#include "FairCurve_Batten.hxx"
#include "FairCurve_MinimalVariation.hxx"
#include "Geom2d_Parabola.hxx"
#include "GCE2d_MakeParabola.hxx"
#include "Geom_TrimmedCurve.hxx"
#include "GeomFill_ConstrainedFilling.hxx"
#include <GeomAPI_ExtremaSurfaceSurface.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <BndLib_AddSurface.hxx>   
#include <BndLib_Add3dCurve.hxx>
#include "Prs3d_TextAspect.hxx"
#include "Prs3d_ArrowAspect.hxx"
#include "Prs3d_PointAspect.hxx"
#include <BRepMesh.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <BRep_Tool.hxx>
#include <Poly_Triangulation.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Builder.hxx>
#include <ShapeFix_Shape.hxx>
#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>

#include "ISession2D_Curve.h"
#include "ISession_Direction.h"
#include "ISession2D_InteractiveContext.h"
#include "ISession_Curve.h"
#include "ISession_Surface.h"
#include "ISession_Text.h"
#include "ISession_Point.h"

#include <UnitsAPI.hxx>
#include "..\res\resource.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

