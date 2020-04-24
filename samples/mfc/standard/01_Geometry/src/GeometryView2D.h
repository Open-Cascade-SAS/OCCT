// NSGViewBib.h : interface of the CNSGView class
//

#if !defined(AFX_NSGVIEWBIB_H__7BCE3513_40A8_11D1_8BEC_00C04FB657CF__INCLUDED_)
#define AFX_NSGVIEWBIB_H__7BCE3513_40A8_11D1_8BEC_00C04FB657CF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <OCC_2dView.h>

/////////////////////////////////////////////////////////////////////////////

#define ValZWMin 1

class CGeometryView2D : public OCC_2dView
{
protected: // create from serialization only
  CGeometryView2D();
  DECLARE_DYNCREATE(CGeometryView2D)

  // Attributes
public:
  CGeometryDoc* GetDocument();
public:
  // Overrides
  // ClassWizard generated virtual function overrides
public:
  virtual void OnInitialUpdate(); // called first time after construct

  // Implementation
public:
  virtual ~CGeometryView2D();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  //! Return interactive context for 2d presentations.
  virtual const Handle(AIS_InteractiveContext)& GetAISContext() const Standard_OVERRIDE;

  // Generated message map functions
protected:
  //{{AFX_MSG(CGeometryView2D)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NSGViewBibliotheque.cpp
inline CGeometryDoc* CGeometryView2D::GetDocument()
   { return (CGeometryDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
#endif
