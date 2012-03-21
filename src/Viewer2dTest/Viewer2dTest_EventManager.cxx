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


#include <Viewer2dTest_EventManager.ixx>
#include <AIS2D_InteractiveContext.hxx>

Viewer2dTest_EventManager::Viewer2dTest_EventManager (const Handle(AIS2D_InteractiveContext)& Ctx)
     : myCtx(Ctx)
{
}

const Handle(AIS2D_InteractiveContext)& Viewer2dTest_EventManager::Context() const
{
  return myCtx;
}

void Viewer2dTest_EventManager::MoveTo(const Standard_Integer XPix, 
                                       const Standard_Integer YPix, 
                                       const Handle(V2d_View)& aView)
{
  if (!myCtx.IsNull())
    myCtx->MoveTo(XPix,YPix,aView);
}

void Viewer2dTest_EventManager::Select(const Standard_Integer  /*XPMin*/,
                                       const Standard_Integer  /*YPMin*/,
                                       const Standard_Integer  /*XPMax*/,
                                       const Standard_Integer  /*YPMax*/, 
                                       const Handle(V2d_View)& /*aView*/)
{
  cout << "Selection by rectangle is not yet implemented" << endl;
//  if (!myCtx.IsNull())
//    myCtx->Select(XPMin,YPMin,XPMax,YPMax,aView);
}

void Viewer2dTest_EventManager::ShiftSelect(const Standard_Integer  XPMin,
                                            const Standard_Integer  YPMin,
                                            const Standard_Integer  XPMax,
                                            const Standard_Integer  YPMax, 
                                            const Handle(V2d_View)& aView)
{ 
  if (!myCtx.IsNull())
    myCtx->ShiftSelect(XPMin,YPMin,XPMax,YPMax,aView,Standard_True);
}

void Viewer2dTest_EventManager::Select()
{
  if (!myCtx.IsNull())
    myCtx->Select();
}

void Viewer2dTest_EventManager::ShiftSelect()
{
  if (!myCtx.IsNull())
    myCtx->ShiftSelect(Standard_True);
}
