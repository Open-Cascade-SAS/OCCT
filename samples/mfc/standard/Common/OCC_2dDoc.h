// OCC_2dDoc.h: interface for the OCC_2dDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_2dDOC_H__2E048CC7_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_2dDOC_H__2E048CC7_38F9_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OCC_BaseDoc.h"
#include "ResultDialog.h"
#include <WNT_Window.hxx>

class AFX_EXT_CLASS OCC_2dDoc : public OCC_BaseDoc 
{
  DECLARE_DYNCREATE(OCC_2dDoc)

public:

  OCC_2dDoc();

// Operations
public:

  Handle_V3d_Viewer GetViewer2D () { return myViewer; };

  void FitAll2DViews(Standard_Boolean theUpdateViewer = Standard_False);

  Handle(AIS_InteractiveContext)& GetInteractiveContext() { return myAISContext; };

  virtual void MoveEvent(const Standard_Integer theMouseX,
                         const Standard_Integer theMouseY,
                         const Handle(V3d_View)& theView);

  virtual void ShiftMoveEvent(const Standard_Integer theMouseX,
                              const Standard_Integer theMouseY,
                              const Handle(V3d_View)& theViewView);
};

#endif // !defined(AFX_OCC_2dDOC_H__2E048CC7_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
