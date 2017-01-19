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

#pragma warning(  disable : 4244 )        // Issue warning 4244
#include "Standard_ShortReal.hxx"
#pragma warning(  default : 4244 )        // Issue warning 4244

#include <Standard.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <WNT_Window.hxx>
#include <Prs3d_Drawer.hxx>
#include <Standard_ErrorHandler.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>
#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Elips.hxx>
#include <gp_Sphere.hxx>
#include <AIS_Axis.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <AIS_Point.hxx>
#include <AIS_Plane.hxx>
#include <AIS_Line.hxx>
#include <GProp_GProps.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Transformation.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepOffsetAPI_MakeEvolved.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRepGProp.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopOpeBRepTool.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TopExp.hxx>
#include <GCE2d_MakeLine.hxx>
#include <BRepLib.hxx>
#include <BRepFeat_MakePrism.hxx>
#include <BRepFeat_MakeDPrism.hxx>
#include <BRepFeat_MakeRevol.hxx>
#include <BRepFeat_Gluer.hxx>
#include <BRepFeat_MakePipe.hxx>
#include <BRepFeat_MakeLinearForm.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <LocOpe_FindEdges.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Solid.hxx>
#include <GeomPlate_HArray1OfHCurve.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <GeomPlate_BuildAveragePlane.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <Plate_D1.hxx>
#include <Plate_GtoCConstraint.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Root.hxx>
#include <GeomPlate_Surface.hxx>
#include <GeomProjLib.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include "ISession_Direction.h"

#include <UnitsAPI.hxx>
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

