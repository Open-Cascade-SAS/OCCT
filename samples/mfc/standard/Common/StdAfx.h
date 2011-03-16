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

#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>
#include <AIS2D_InteractiveContext.hxx>
#include <WNT_WDriver.hxx>

#include <Standard.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <WNT_Window.hxx>

#include <AIS2D_InteractiveObject.hxx>
#include <Graphic2d_SetOfCurves.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Prs2d_AspectLine.hxx>
#include <Graphic2d_Array1OfVertex.hxx>
#include <Graphic2d_Polyline.hxx>
#include <Graphic2d_Text.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <AIS_Drawer.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_HLRPolyShape.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>

#include <TopTools_HSequenceOfShape.hxx>
#include <BRepTools.hxx>

#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Aspect_PolygonOffsetMode.hxx>
#include <Graphic3d_Group.hxx>

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

