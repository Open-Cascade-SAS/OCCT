// OCC_BaseChildFrame.cpp: implementation of the OCC_BaseChildFrame class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "OCC_BaseChildFrame.h"

#include <res\OCC_Resource.h>

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseChildFrame

IMPLEMENT_DYNCREATE (OCC_BaseChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(OCC_BaseChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(OCC_BaseChildFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseChildFrame construction/destruction

OCC_BaseChildFrame::OCC_BaseChildFrame()
{
}

OCC_BaseChildFrame::~OCC_BaseChildFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseChildFrame diagnostics

#ifdef _DEBUG
void OCC_BaseChildFrame::AssertValid() const
{
  CMDIChildWnd::AssertValid();
}

void OCC_BaseChildFrame::Dump(CDumpContext& dc) const
{
  CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// OCC_BaseChildFrame message handlers

void OCC_BaseChildFrame::ActivateFrame(int nCmdShow)
{
  POSITION pos=AfxGetApp()->GetFirstDocTemplatePosition();
  CDocTemplate* DocT=AfxGetApp()->GetNextDocTemplate(pos);

  POSITION FirstDocPosition =DocT->GetFirstDocPosition();
  POSITION p = FirstDocPosition;

  CDocument* doc;
  doc = NULL;

  if(FirstDocPosition!=NULL)
  {
    doc = DocT->GetNextDoc(p);
    if(AfxIsValidAddress(doc, sizeof(CDocument)) && p == NULL)
    {
      ASSERT_VALID(doc);
      POSITION position = doc->GetFirstViewPosition();
      if(position != NULL)
        doc ->GetNextView(position);
      if (position == NULL)
        nCmdShow = SW_SHOWMAXIMIZED;      
    }
  }

  CMDIChildWnd::ActivateFrame(nCmdShow);
}
 
