// File:	Viewer2dTest_EventManager.cxx

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
