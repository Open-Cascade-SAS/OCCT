// OCCDemo_Presentation.cpp: implementation of the OCCDemo_Presentation class.
// This is a base class for all presentations
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OCCDemo_Presentation.h"
#include "OCCDemoView.h"
#include "ISession_Curve.h"

#include <AIS_InteractiveObject.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <Quantity_Color.hxx>
#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Precision.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <GeomAPI.hxx>
#include <gp_Pln.hxx>
#include <Geom_OffsetCurve.hxx>

#define MAX_PARAM 1000 // if a surface parameter is infinite, it is assingned
// this value in order to display the "infinit" object in the viewer.


Standard_Boolean OCCDemo_Presentation::WaitForInput (unsigned long aMilliSeconds)
{
  //::WaitForSingleObject(::CreateEvent (NULL, FALSE, FALSE, NULL), aMilliSeconds);
  if (::MsgWaitForMultipleObjects(0, NULL, FALSE, aMilliSeconds,
    QS_KEY | QS_MOUSEBUTTON) != WAIT_TIMEOUT)
  {
    MSG msg;
    if (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
    {
      if ( msg.message == WM_KEYUP ||
           msg.message == WM_MOUSEFIRST ||
           msg.message == WM_PAINT )
      {
        ::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE);
        return WaitForInput (aMilliSeconds);
      }
      else
        return Standard_True;
    }
  }
  return Standard_False;
}

//================================================================
// Function : fixParam
// Purpose  : assings a finite value to theParam if it intinite
//            (equal to +- Precision::Infinite())
//================================================================
static Standard_Boolean fixParam(Standard_Real& theParam)
{
  Standard_Boolean aResult = Standard_False;
  if (Precision::IsNegativeInfinite(theParam))
  {
    theParam = -MAX_PARAM;
    aResult = Standard_True;
  }
  if (Precision::IsPositiveInfinite(theParam))
  {
    theParam = MAX_PARAM;
    aResult = Standard_True;
  }
  return aResult;
}

//================================================================
// Function : DrawSurface                           
// Purpose  : displays a given geometric surface in 3d viewer
//            (creates a finite face and displays it)
//================================================================
Handle(AIS_InteractiveObject) OCCDemo_Presentation::drawSurface
                                  (const Handle(Geom_Surface)& theSurface,
                                   const Quantity_Color& theColor,
                                   const Standard_Boolean toDisplay)
{
  Standard_Real u1, u2, v1, v2;
  theSurface->Bounds(u1,u2,v1,v2);
  fixParam(u1);
  fixParam(u2);
  fixParam(v1);
  fixParam(v2);

  Handle(AIS_Shape) aGraphicSurface = 
    new AIS_Shape(BRepBuilderAPI_MakeFace (theSurface, u1, u2, v1, v2, Precision::Confusion()));

  getAISContext()->SetMaterial(aGraphicSurface, Graphic3d_NOM_PLASTIC, toDisplay);
  getAISContext()->SetColor(aGraphicSurface, theColor, toDisplay);
  if (toDisplay) {
    if (FitMode){
		getAISContext()->Display (aGraphicSurface, Standard_False);
		COCCDemoDoc::Fit();
	}
	else
		getAISContext()->Display (aGraphicSurface, Standard_True);
  }
  
  return aGraphicSurface;
}

//================================================================
// Function : DrawCurve                                 
// Purpose  : displays a given curve 3d
//================================================================
Handle(AIS_InteractiveObject) OCCDemo_Presentation::drawCurve
                                  (const Handle(Geom_Curve)& theCurve,
                                   const Quantity_Color& theColor,
                                   const Standard_Boolean toDisplay)
{
  Handle(ISession_Curve) aGraphicCurve = new ISession_Curve (theCurve);

  getAISContext()->SetColor (aGraphicCurve, theColor, toDisplay);
  aGraphicCurve->Attributes()->Link()->SetLineArrowDraw(Standard_False);
  if (toDisplay){ 
    if (FitMode){
		getAISContext()->Display (aGraphicCurve, Standard_False);
		COCCDemoDoc::Fit();
	}
	else
		getAISContext()->Display (aGraphicCurve, Standard_True);
  }

  return aGraphicCurve;
}

//================================================================
// Function : DrawCurve                                 
// Purpose  : displays a given curve 2d
//================================================================
Handle(AIS_InteractiveObject) OCCDemo_Presentation::drawCurve
                                  (const Handle(Geom2d_Curve)& theCurve,
                                   const Quantity_Color& theColor,
                                   const Standard_Boolean toDisplay,
                                   const gp_Ax2& aPosition)
{
  // create 3D curve in plane
  Handle(Geom_Curve) aCurve3d;
  if (theCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    Handle(Geom2d_OffsetCurve) aOffCurve =
      Handle(Geom2d_OffsetCurve)::DownCast(theCurve);
    Handle(Geom_Curve) aBasCurve3d =
      GeomAPI::To3d (aOffCurve->BasisCurve(), gp_Pln(aPosition));
    Standard_Real aDist = aOffCurve->Offset();
    aCurve3d = new Geom_OffsetCurve (aBasCurve3d, aDist, aPosition.Direction());
  }
  else
  {
    aCurve3d = GeomAPI::To3d (theCurve, gp_Pln(aPosition));
  }
  return drawCurve (aCurve3d, theColor, toDisplay);
}

//================================================================
// Function : drawPoint
// Purpose  : displays a given point
//================================================================
Handle(AIS_Point) OCCDemo_Presentation::drawPoint
                                  (const gp_Pnt& aPnt,
                                   const Quantity_Color& theColor,
                                   const Standard_Boolean toDisplay)
{
  Handle(AIS_Point) aGraphicPoint = new AIS_Point (new Geom_CartesianPoint(aPnt));

  getAISContext()->SetColor (aGraphicPoint, theColor, toDisplay);
  if (toDisplay) {
    getAISContext()->Display (aGraphicPoint, Standard_True);
    //COCCDemoDoc::Fit();
  }

  return aGraphicPoint;
}

//================================================================
// Function : drawVector
// Purpose  : displays a given vector in 3d viewer
//            (segment of line starting at thePnt with the arrow at the end,
//             the length of segment is the length of the vector)
//================================================================
Handle(AIS_InteractiveObject) OCCDemo_Presentation::drawVector 
                                  (const gp_Pnt& thePnt,
                                   const gp_Vec& theVec,
                                   const Quantity_Color& theColor,
                                   const Standard_Boolean toDisplay)
{
  Standard_Real aLength = theVec.Magnitude();
  if (aLength < Precision::Confusion())
    return Handle(AIS_InteractiveObject)();

  Handle(Geom_Curve) aCurve = new Geom_Line (thePnt, theVec);
  aCurve = new Geom_TrimmedCurve (aCurve, 0, aLength);

  Handle(ISession_Curve) aGraphicCurve = new ISession_Curve (aCurve);

  getAISContext()->SetColor (aGraphicCurve, theColor, toDisplay);
  Handle(Prs3d_Drawer) aDrawer = aGraphicCurve->Attributes()->Link();
  aDrawer->SetLineArrowDraw(Standard_True);
  aDrawer->ArrowAspect()->SetLength(aLength/10);
  if (toDisplay) {
    if (FitMode){
		getAISContext()->Display (aGraphicCurve, Standard_False);
		COCCDemoDoc::Fit();
	}
	else
		getAISContext()->Display (aGraphicCurve, Standard_True);
  }

  return aGraphicCurve;
}


Handle(AIS_Shape) OCCDemo_Presentation::drawShape 
         (const TopoDS_Shape& theShape,const Quantity_Color& theColor,
          const Standard_Boolean toDisplay)
{
  Handle(AIS_Shape) aGraphicShape = new AIS_Shape(theShape);

  getAISContext()->SetMaterial(aGraphicShape, Graphic3d_NOM_PLASTIC, toDisplay);
  getAISContext()->SetColor (aGraphicShape, theColor, toDisplay);
  if (toDisplay){ 
    if (FitMode){
		getAISContext()->Display (aGraphicShape, Standard_False);
		COCCDemoDoc::Fit();
	}
	else
		getAISContext()->Display (aGraphicShape, Standard_True);
  }

  return aGraphicShape;
}

Handle(AIS_Shape) OCCDemo_Presentation::drawShape
         (const TopoDS_Shape& theShape,
          const Graphic3d_NameOfMaterial theMaterial,
          const Standard_Boolean toDisplay)
{
  Handle(AIS_Shape) aGraphicShape = new AIS_Shape(theShape);

  getAISContext()->SetMaterial(aGraphicShape, theMaterial, toDisplay);
  if (toDisplay) {
    if (FitMode){
		getAISContext()->Display (aGraphicShape, Standard_False);
		COCCDemoDoc::Fit();
	}
	else
		getAISContext()->Display (aGraphicShape, Standard_True);
  }

  return aGraphicShape;
}

void OCCDemo_Presentation::GetViewAt (Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ)
{
  CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
  CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
  COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
  pView->GetViewAt (theX, theY, theZ);
}

void OCCDemo_Presentation::SetViewAt (const Standard_Real theX, const Standard_Real theY, const Standard_Real theZ)
{
  CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
  CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
  COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
  pView->SetViewAt (theX, theY, theZ);
}

void OCCDemo_Presentation::GetViewEye(Standard_Real& X, Standard_Real& Y, Standard_Real& Z)
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
	pView->GetViewEye(X,Y,Z);
}

void OCCDemo_Presentation::SetViewEye(Standard_Real X, Standard_Real Y, Standard_Real Z)
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
	pView->SetViewEye(X,Y,Z);
}

Standard_Real OCCDemo_Presentation::GetViewScale()
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
	return pView->GetViewScale();
}

void OCCDemo_Presentation::SetViewScale(Standard_Real Coef)
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
	pView->SetViewScale(Coef);
}

void OCCDemo_Presentation::ResetView()
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
	pView->Reset();
}

void OCCDemo_Presentation::TranslateView (const Standard_Real theX, const Standard_Real theY)
{
  CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
  CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
  COCCDemoView *pView = (COCCDemoView *) pChild->GetActiveView();
  pView->Translate (theX, theY);
}
