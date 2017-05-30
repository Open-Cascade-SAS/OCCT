// OCC_BaseView.h: interface for the OCC_BaseView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_BASEVIEW_H__2E048CCA_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_BASEVIEW_H__2E048CCA_38F9_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdafx.h>

#include "OCC_BaseDoc.h"
#include "AIS_RubberBand.hxx"

class AFX_EXT_CLASS OCC_BaseView  : public CView
{
  
public:
  
	OCC_BaseView();
	virtual ~OCC_BaseView();
  
  OCC_BaseDoc* GetDocument();

protected:
  
  virtual void drawRectangle (const Standard_Integer theMinX,
                              const Standard_Integer theMinY,
                              const Standard_Integer theMaxX,
                              const Standard_Integer theMaxY,
                              const Handle(AIS_InteractiveContext)& theContext,
                              const Standard_Boolean toDraw = Standard_True);


protected:

  Standard_Real    myCurZoom;
  Standard_Integer myXmin;
  Standard_Integer myYmin;
  Standard_Integer myXmax;
  Standard_Integer myYmax;

  Handle(AIS_RubberBand) myRect; //!< Rubber rectangle for selection
};

#endif // !defined(AFX_OCC_BASEVIEW_H__2E048CCA_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
