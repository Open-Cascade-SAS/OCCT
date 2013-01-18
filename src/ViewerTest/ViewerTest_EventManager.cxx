// Created on: 1998-08-27
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

//=======================================================================
//function : Select
//purpose  : Selection with polyline
//=======================================================================

void ViewerTest_EventManager::Select(const TColgp_Array1OfPnt2d& thePolyline)
{
  if(!myCtx.IsNull() && !myView.IsNull())
    myCtx->Select(thePolyline,myView);
  const Handle(NIS_View) aView = Handle(NIS_View)::DownCast(myView);
  if (!aView.IsNull())
  {
    NCollection_List<gp_XY> aPolylist;
    for(Standard_Integer anIter = thePolyline.Lower();anIter <= thePolyline.Upper();++anIter)
    {
      aPolylist.Append(gp_XY(thePolyline.Value(anIter).X(),
        thePolyline.Value(anIter).Y()));
    }
    aView->Select(aPolylist);
  }
}

//=======================================================================
//function : ShiftSelect
//purpose  : Selection with polyline without erasing of current selection
//=======================================================================

void ViewerTest_EventManager::ShiftSelect(const TColgp_Array1OfPnt2d& thePolyline)
{
  if(!myCtx.IsNull() && !myView.IsNull())
    myCtx->ShiftSelect(thePolyline,myView);
  const Handle(NIS_View) aView = Handle(NIS_View)::DownCast(myView);
  if (!aView.IsNull())
  {
    NCollection_List<gp_XY> aPolylist;
    for(Standard_Integer anIter = thePolyline.Lower();anIter <= thePolyline.Upper();++anIter)
    {
      aPolylist.Append(gp_XY(thePolyline.Value(anIter).X(),
        thePolyline.Value(anIter).Y()));
    }
    aView->Select(aPolylist, Standard_True);
  }
}
