// HLRView.h : interface of the CHLRView2D class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HLRVIEW2D_H__376C7013_0B3D_11D2_8E0A_0800369C8A03_2D_INCLUDED_)
#define AFX_HLRVIEW2D_H__376C7013_0B3D_11D2_8E0A_0800369C8A03_2D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "HLRDoc.h"
#include <OCC_2dView.h>
#include "Resource2d/RectangularGrid.h"
#include "Resource2d/CircularGrid.h"


class CHLRView2D : public OCC_2dView
{
protected: // create from serialization only
  CHLRView2D();
  DECLARE_DYNCREATE(CHLRView2D)
  DECLARE_MESSAGE_MAP()

  //! Return interactive context for HLR presentations.
  virtual const Handle(AIS_InteractiveContext)& GetAISContext() const Standard_OVERRIDE;

// Implementation
public:
  virtual ~CHLRView2D();
  CHLRDoc* GetDocument();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HLRVIEW2D_H__376C7013_0B3D_11D2_8E0A_0800369C8A03_2D_INCLUDED_)
