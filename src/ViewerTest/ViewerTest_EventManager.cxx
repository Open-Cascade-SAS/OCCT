// File:	ViewerTest_EventManager.cxx
// Created:	Thu Aug 27 14:20:19 1998
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>


#include <ViewerTest_EventManager.ixx>
#include <AIS_InteractiveContext.hxx>
#include <NIS_View.hxx>

//=======================================================================
//function : ViewerTest_EventManager
//purpose  : 
//=======================================================================

ViewerTest_EventManager::ViewerTest_EventManager
        (const Handle(V3d_View)& aView,
         const Handle(AIS_InteractiveContext)& Ctx)
  : myView (aView),
    myCtx  (Ctx),
    myX    (-1),
    myY    (-1)
{}

//=======================================================================
//function : MoveTo
//purpose  : 
//=======================================================================

void ViewerTest_EventManager::MoveTo(const Standard_Integer XPix, 
				     const Standard_Integer YPix)
{
  if(!myCtx.IsNull() && !myView.IsNull())
    myCtx->MoveTo(XPix,YPix,myView);
  myX = XPix;
  myY = YPix;
  const Handle(NIS_View) aView = Handle(NIS_View)::DownCast(myView);
  if (!aView.IsNull())
    aView->DynamicHilight (XPix, YPix);
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================

void ViewerTest_EventManager::Select(const Standard_Integer  XPMin,
				     const Standard_Integer  YPMin,
				     const Standard_Integer  XPMax,
				     const Standard_Integer  YPMax)
{
#define IS_FULL_INCLUSION Standard_True
  if(!myCtx.IsNull() && !myView.IsNull())
    myCtx->Select(XPMin,YPMin,XPMax,YPMax,myView);
  const Handle(NIS_View) aView = Handle(NIS_View)::DownCast(myView);
  if (!aView.IsNull())
    aView->Select(XPMin,YPMin,XPMax,YPMax, Standard_False, IS_FULL_INCLUSION);
}

//=======================================================================
//function : ShiftSelect
//purpose  : 
//=======================================================================

void ViewerTest_EventManager::ShiftSelect(const Standard_Integer  XPMin,
					  const Standard_Integer  YPMin,
					  const Standard_Integer  XPMax,
					  const Standard_Integer  YPMax)
{ 
  if(!myCtx.IsNull() && !myView.IsNull())
    myCtx->AIS_InteractiveContext::ShiftSelect(XPMin,YPMin,XPMax,YPMax,myView,
                                               Standard_True);
  const Handle(NIS_View) aView = Handle(NIS_View)::DownCast(myView);
  if (!aView.IsNull())
    aView->Select(XPMin,YPMin,XPMax,YPMax, Standard_True, IS_FULL_INCLUSION);
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================

void ViewerTest_EventManager::Select()
{
  if (!myCtx.IsNull() && !myView.IsNull())
    myCtx->Select();
  const Handle(NIS_View) aView = Handle(NIS_View)::DownCast(myView);
  if (!aView.IsNull())
    aView->Select(myX, myY);
}

//=======================================================================
//function : ShiftSelect
//purpose  : 
//=======================================================================

void ViewerTest_EventManager::ShiftSelect()
{
  if(!myCtx.IsNull() && !myView.IsNull())
    myCtx->ShiftSelect(Standard_True);
  const Handle(NIS_View) aView = Handle(NIS_View)::DownCast(myView);
  if (!aView.IsNull())
    aView->Select(myX, myY, Standard_True);
}
