// OCC_BaseDoc.h: interface for the OCC_BaseDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_BASEDOC_H__2E048CC8_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_BASEDOC_H__2E048CC8_38F9_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdafx.h>

class OCC_BaseDoc : public CDocument  
{
public:

  OCC_BaseDoc();

  virtual ~OCC_BaseDoc();

  Handle(AIS_InteractiveContext)& GetAISContext() { return myAISContext; }

  Handle(V3d_Viewer) GetViewer()  { return myViewer; }

  //Events
  virtual void DragEvent (const Standard_Integer theMouseX,
                          const Standard_Integer theMouseY,
                          const Standard_Integer theState,
                          const Handle(V3d_View)& theView) {}

  virtual void InputEvent (const Standard_Integer theMouseX,
                           const Standard_Integer theMouseY,
                           const Handle(V3d_View)& theView) {}

  virtual void MoveEvent (const Standard_Integer theMouseX,
                          const Standard_Integer theMouseY,
                          const Handle(V3d_View)& theView) {}

  virtual void ShiftMoveEvent (const Standard_Integer theMouseX,
                               const Standard_Integer theMouseY,
                               const Handle(V3d_View)& theView) {}

  virtual void ShiftDragEvent (const Standard_Integer theMouseX,
                               const Standard_Integer theMouseY,
                               const Standard_Integer theState,
                               const Handle(V3d_View)& theView) {}

  virtual void ShiftInputEvent(const Standard_Integer theMouseX,
                               const Standard_Integer theMouseY,
                               const Handle(V3d_View)& theView) {}

  virtual void Popup (const Standard_Integer theMouseX,
                      const Standard_Integer theMouseY,
                      const Handle(V3d_View)& theView) {}
protected:

  Handle(V3d_Viewer) myViewer;
  Handle(AIS_InteractiveContext) myAISContext;
};

#endif // !defined(AFX_OCC_BASEDOC_H__2E048CC8_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
