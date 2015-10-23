// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__79840E80_1001_11D2_B8C1_0000F87A77C1__INCLUDED_)
#define AFX_STDAFX_H__79840E80_1001_11D2_B8C1_0000F87A77C1__INCLUDED_

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



// CasCade :

enum CurrentAction3d { 
  CurAction3d_Nothing,
  CurAction3d_DynamicZooming,
  CurAction3d_WindowZooming,
  CurAction3d_DynamicPanning,
  CurAction3d_GlobalPanning,
  CurAction3d_DynamicRotation
};


#define DEFAULT_DEVIATIONCOEFFICIENT 0.001
#define DEFAULT_DCBIG 0.005
#define DEFAULT_DCVBIG 0.01
#define DEFAULT_DCSMALL 0.0002
#define DEFAULT_DCVSMALL 0.00004
#define DEFAULT_COLOR Quantity_NOC_CYAN1
#define DEFAULT_MATERIAL Graphic3d_NOM_PLASTER
#define DEFAULT_BACKGROUNDCOLOR Quantity_NOC_MATRAGRAY
#define DEFAULT_HILIGHTCOLOR Quantity_NOC_YELLOW

#pragma warning(  disable : 4244 )        // Issue warning 4244
#include <Standard_ShortReal.hxx>
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
# pragma comment (lib,"gp.lib")
# pragma comment (lib,"TKernel.lib")
# pragma comment (lib,"UnitsAPI.lib")
# pragma comment (lib,"PTColStd.lib")
# pragma comment (lib,"TKMgtShape.lib")
# pragma comment (lib,"TKPShape.lib")
# pragma comment (lib,"ShapeSchema.lib")
# pragma comment (lib,"FSD.lib")
# pragma comment (lib,"OSDriver.lib")
# pragma comment (lib,"ObjMgt.lib")
# pragma comment (lib,"TKXSBase.lib")
# pragma comment (lib,"TKXSIGES.lib")
# pragma comment (lib,"TKXS214.lib")
*/

 
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>

#include <Aspect_Background.hxx>
#include <Aspect_TypeOfline.hxx>
#include <Aspect_WidthOfline.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepTools.hxx>
#include <Standard_DefineHandle.hxx>
#include <DsgPrs_LengthPresentation.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <MMgt_TShared.hxx>
#include <OSD_Environment.hxx>
#include <Precision.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Quantity_Factor.hxx>
#include <Quantity_Length.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Quantity_PhysicalQuantity.hxx>
#include <Quantity_PlaneAngle.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectionManager.hxx>
// had for opencascade
//#include <Session_Selection.hxx>
//#include <Session_Session.hxx>
//#include <ShapeSchema.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Integer.hxx>
#include <Standard_IStream.hxx>
#include <Standard_Macro.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Real.hxx>
#include <StdPrs_Curve.hxx>
#include <StdPrs_Point.hxx>
#include <StdPrs_PoleCurve.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <UnitsAPI.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <WNT_Window.hxx>
#include "HLRAlgo_Projector.hxx"

#include <Geom_BSplineSurface.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <BndLib_AddSurface.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBndLib.hxx>

#include "..\res\resource.h"

//End CasCade



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__79840E80_1001_11D2_B8C1_0000F87A77C1__INCLUDED_)
