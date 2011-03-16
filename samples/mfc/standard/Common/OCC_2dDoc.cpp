// OCC_2dDoc.cpp: implementation of the OCC_2dDoc class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include "OCC_2dDoc.h"

#include "OCC_2dApp.h"
#include "OCC_2dView.h"

IMPLEMENT_DYNCREATE(OCC_2dDoc, CDocument)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OCC_2dDoc::OCC_2dDoc()
{
	// Get the Graphic Device from the application 
	Handle(WNT_GraphicDevice) theGraphicDevice = 
	  ((OCC_2dApp*)AfxGetApp())->GetGraphicDevice();

	// create the Viewer
	TCollection_ExtendedString Name("Viewer 2D");
	TCollection_AsciiString Domain("My Domain");
	my2DViewer = new V2d_Viewer(theGraphicDevice,
							  Name.ToExtString(),
							  Domain.ToCString());

	// set default values for grids
	my2DViewer->SetCircularGridValues(0,0,10,8,0);
	my2DViewer->SetRectangularGridValues(0,0,10,10,0);

	myAISInteractiveContext2D = new AIS2D_InteractiveContext(my2DViewer);

	
	AfxInitRichEdit();
	myCResultDialog.Create(CResultDialog::IDD,NULL);

	RECT dlgrect;
	myCResultDialog.GetWindowRect(&dlgrect);
	LONG width = dlgrect.right-dlgrect.left;
	LONG height = dlgrect.bottom-dlgrect.top;

	RECT MainWndRect;
	AfxGetApp()->m_pMainWnd->GetWindowRect(&MainWndRect);
	LONG left = MainWndRect.left+3;
	LONG top = MainWndRect.top + 108;
	//LONG height = MainWndRect.bottom - MainWndRect.top;

	myCResultDialog.MoveWindow(left,top,width,height);
	
}

OCC_2dDoc::~OCC_2dDoc()
{

}

void OCC_2dDoc::FitAll2DViews(Standard_Boolean UpdateViewer)
{
  if (UpdateViewer)   my2DViewer->Update();
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    OCC_2dView* pCurrentView = (OCC_2dView*)GetNextView(position);
    ASSERT_VALID(pCurrentView);
    pCurrentView->GetV2dView()->Fitall();
  }
}

void OCC_2dDoc::UpdateResultDialog(UINT anID,TCollection_AsciiString aMessage)
{
    CString text(aMessage.ToCString());
    myCResultDialog.SetText(text);

    CString s;
    if (! s.LoadString( anID )) AfxMessageBox("Error Loading String: ");
    CString Title = s.Left( s.Find( '\n' ));
    myCResultDialog.SetTitle(Title);
    SetTitle(Title);
//    myCResultDialog.ShowWindow(SW_RESTORE);
}

void OCC_2dDoc::UpdateResultDialog(CString& title,TCollection_AsciiString aMessage)
{
    CString text(aMessage.ToCString());
    myCResultDialog.SetText(text);
    myCResultDialog.SetTitle(title);
    SetTitle(title);
//    myCResultDialog.ShowWindow(SW_RESTORE);
}

