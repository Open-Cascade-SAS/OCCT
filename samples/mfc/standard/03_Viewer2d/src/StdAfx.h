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
# pragma comment (lib,"TKGeom.lib")
# pragma comment (lib,"TKGlt.lib")
# pragma comment (lib,"TKGraphic.lib")
# pragma comment (lib,"TKPrsMgr.lib")
# pragma comment (lib,"TKViewers.lib")
# pragma comment (lib,"gp.lib")
# pragma comment (lib,"TKernel.lib")
*/

#include <Standard_ErrorHandler.hxx>

#include <WNT_Window.hxx>


#include <V2d_Viewer.hxx>

#include <V2d_View.hxx>

#include <WNT_GraphicDevice.hxx>
#include <Aspect_WindowDriver.hxx>

#include "gp_Pnt2d.hxx"
#include "Geom2d_Curve.hxx"
#include "GCE2d_MakeSegment.hxx"
#include "gp_Pnt2d.hxx"

#include <Graphic2d_SetOfCurves.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <Precision.hxx>
#include <Select2D_SensitiveBox.hxx>
#include <Select2D_SensitiveSegment.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>

#include <Select2D_SensitiveArc.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <gp_Vec2d.hxx>
#include <OSD_Environment.hxx>
#include <Graphic2d_Array1OfVertex.hxx>
#include <Graphic2d_PolyLine.hxx>

#include "SelectMgr_Selection.hxx"


#include <Quantity_Length.hxx>
#include <Aspect_TypeOfline.hxx>
#include <Aspect_WidthOfline.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Handle_PrsMgr_PresentationManager2d.hxx>
#include <Handle_Graphic2d_GraphicObject.hxx>
#include <Handle_SelectMgr_Selection.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Standard_CString.hxx>

#include "Geom2d_Curve.hxx"


#include <Aspect_Window.hxx>
#include <Aspect_Background.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>

#include <PrsMgr_PresentationManager2d.hxx>
#include <StdSelect_ViewerSelector2d.hxx>

#include "SelectMgr_SelectableObject.hxx"

#include <Handle_V2d_Viewer.hxx>
#include <Handle_PrsMgr_PresentationManager2d.hxx>
#include <Handle_SelectMgr_SelectionManager.hxx>
#include <Handle_StdSelect_ViewerSelector2d.hxx>
#include <MMgt_TShared.hxx>
#include <Handle_SelectMgr_SelectableObject.hxx>
#include <Standard_Boolean.hxx>
#include <Handle_V2d_View.hxx>
#include <Graphic2d_Buffer.hxx>

class V2d_Viewer;
class PrsMgr_PresentationManager2d;
class SelectMgr_SelectionManager;
#include <StdSelect_ViewerSelector2d.hxx>
class SelectMgr_SelectableObject;
class V2d_View;
#include <SelectMgr_SelectableObject.hxx>

class ISession2D_InteractiveObject;
#include <SelectMgr_EntityOwner.hxx>

#include <Bnd_Box2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <SelectBasics_BasicTool.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <Geom2dAdaptor_Curve.hxx>

#include <Standard_Integer.hxx>
#include <gp_Pnt2d.hxx>
#include <Select2D_SensitiveEntity.hxx>
#include <Handle_SelectBasics_EntityOwner.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class SelectBasics_EntityOwner;
class gp_Pnt2d;
#include <SelectBasics_ListOfBox2d.hxx>
#include "TColgp_HArray1OfPnt2d.hxx"
#include <Geom2d_Curve.hxx>

#include <Graphic2d_Text.hxx>
#include <Select2D_SensitiveBox.hxx>
#include <Graphic2d_Segment.hxx>
#include <OSD_Environment.hxx>
#include <Graphic2d_View.hxx>
#include <Graphic2d_Drawer.hxx>
#include "PrsMgr_PresentationManager2d.hxx"
#include "SelectMgr_Selection.hxx"
#include "Graphic2d_Array1OfVertex.hxx"
#include "Graphic2d_Polyline.hxx"
#include "Graphic2d_Vertex.hxx"
#include "Graphic2d_DisplayList.hxx"
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Standard_CString.hxx>
#include <SelectMgr_SelectableObject.hxx>

#include <TCollection_AsciiString.hxx>
#include <Aspect_TypeOfText.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Quantity_Factor.hxx>
#include <Quantity_PlaneAngle.hxx>
#include <Handle_PrsMgr_PresentationManager2d.hxx>
#include <Handle_Graphic2d_GraphicObject.hxx>
#include <Handle_SelectMgr_Selection.hxx>

class TCollection_AsciiString;
class Graphic2d_GraphicObject;
#include <SelectMgr_EntityOwner.hxx>
#include "Aspect_MarkMap.hxx"
#include "V2d_View.hxx"
#include "Aspect_ColorMap.hxx"
#include "Aspect_FontMap.hxx"
#include "Aspect_MarkMap.hxx"
#include "Aspect_TypeMap.hxx"
#include "Aspect_WidthMap.hxx"
#include "Aspect_MarkMap.hxx"
#include "StdSelect_TextProjector2d.hxx"
#include "StdSelect_SensitiveText2d.hxx"
#include "WNT_FontMapEntry.hxx"
#include "WNT_WDriver.hxx"
#include "HLRAlgo_Projector.hxx"

#include "AIS2D_InteractiveContext.hxx"
#include "AIS2D_InteractiveObject.hxx"

#include "Graphic2d_TypeOfPolygonFilling.hxx"
#include "Prs2d_AspectLine.hxx"


#include <..\res\resource.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

