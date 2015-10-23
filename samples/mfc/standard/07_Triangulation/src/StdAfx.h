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
# pragma comment (lib,"TColgp.lib")
# pragma comment (lib,"TKernel.lib")
*/

 
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <WNT_Window.hxx>
#include <Standard_ErrorHandler.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

#include "TopoDS_Shape.hxx"
#include "BRepPrimAPI_MakeBox.hxx"
#include "BRepPrimAPI_MakeSphere.hxx"
#include "gp_Pnt.hxx"
#include "BRepAlgoAPI_Cut.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS_Face.hxx"
#include "TopLoc_Location.hxx"
#include "Poly_Triangulation.hxx"
#include "BRep_Tool.hxx"
#include "TopoDS.hxx"
#include "TColgp_Array1OfPnt.hxx"
#include "Poly_Array1OfTriangle.hxx"
#include "Poly_Triangle.hxx"
#include "gp_Pnt.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRep_Builder.hxx"
#include "TopoDS_Compound.hxx"
#include "BRepAlgoAPI_Fuse.hxx"
#include "BRepBuilderAPI_MakeVertex.hxx"
#include "TopoDS.hxx"
#include "BRepTools.hxx"
#include "BRepMesh_IncrementalMesh.hxx"

#include <UnitsAPI.hxx>

#include <res\resource.h>
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

