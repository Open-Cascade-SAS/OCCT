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

// Mouse and clipboard event tracker for OCC_2dView
// NO 3D operation are supported (like rotation)
class Standard_EXPORT OCC_2dDoc : public OCC_BaseDoc
{
  DECLARE_DYNCREATE(OCC_2dDoc)

public:

  OCC_2dDoc();
  ~OCC_2dDoc();

public: // New operations

  void FitAll2DViews(Standard_Boolean theUpdateViewer = Standard_False);

public: // Operations to override

  virtual void Popup (const Standard_Integer theMouseX,
                      const Standard_Integer theMouseY,
                      const Handle(V3d_View)& theView) Standard_OVERRIDE;
};

#endif // !defined(AFX_OCC_2dDOC_H__2E048CC7_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
