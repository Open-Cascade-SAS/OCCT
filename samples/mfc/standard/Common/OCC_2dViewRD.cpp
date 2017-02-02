// OCC_2dViewRDRD.cpp: implementation of the OCC_2dViewRDRD class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include <res/OCC_Resource.h>
#include "OCC_2dViewRD.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(OCC_2dViewRD, CView)
	//{{AFX_MSG_MAP(OCC_2dViewRD)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
    ON_COMMAND(ID_FILE_EXPORT_IMAGE, OnFileExportImage)
    ON_COMMAND(ID_BUTTON2DGridRectLines, OnBUTTONGridRectLines)
	ON_COMMAND(ID_BUTTON2DGridRectPoints, OnBUTTONGridRectPoints)
	ON_COMMAND(ID_BUTTON2DGridCircLines, OnBUTTONGridCircLines)
	ON_COMMAND(ID_BUTTON2DGridCircPoints, OnBUTTONGridCircPoints)
	ON_COMMAND(ID_BUTTON2DGridValues, OnBUTTONGridValues)
	ON_COMMAND(ID_BUTTON2DGridCancel, OnBUTTONGridCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void OCC_2dViewRD::OnFileExportImage()
{
	OCC_2dViewRD::OnFileExportImage();
  // Update Get information to update Result dialog

  UINT anID=ID_FILE_EXPORT_IMAGE;
  TCollection_AsciiString Message(" \
  CFileDialog dlg(FALSE,_T(\"*.BMP\"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, \n\
                                             _T(\"BMP Files (*.BMP)|*.bmp |GIF Files (*.GIF)|*.gif | XWD Files (*.XWD)|*.xwd||\"), \n\
                                             NULL );                                                \n\
                                                                         \n\
  if (dlg.DoModal() == IDOK)                                             \n\
  {                                                                      \n\
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));                \n\
    CString filename = dlg.GetPathName();                                \n\
    Handle(WNT_Window) aWNTWindow=                                       \n\
    Handle(WNT_Window)::DownCast(myV2dView->Driver()->Window());         \n\
    CString ext = dlg.GetFileExt();                                      \n\
    if (ext == \"bmp\")     aWNTWindow->SetOutputFormat ( WNT_TOI_BMP ); \n\
    if (ext == \"gif\")        aWNTWindow->SetOutputFormat ( WNT_TOI_GIF ); \n\
    if (ext == \"xwd\")     aWNTWindow->SetOutputFormat ( WNT_TOI_XWD ); \n\
    aWNTWindow->Dump ((Standard_CString)(LPCTSTR)filename);              \n\
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));               \n\
  }                                                                      \n");

  // Update The Result Dialog
  GetDocument()->UpdateResultDialog(anID,Message);
}


void OCC_2dViewRD::OnBUTTONGridRectLines() 
{
	OCC_2dViewRD::OnBUTTONGridRectLines();
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON2DGridRectLines;
  TCollection_AsciiString Message;
  Message += "\
  Handle(V2d_Viewer) aViewer = myV2dView->Viewer();  \n\
  aViewer->SetGridColor(Quantity_Color(Quantity_NOC_WHITE), Quantity_Color(Quantity_NOC_WHITE)); \n\
  aViewer->ActivateGrid(Aspect_GT_Rectangular, \n\
                                      Aspect_GDM_Lines);  \n\
  if (TheCircularGridDialog.IsWindowVisible())       \n\
  {                                                  \n\
    TheCircularGridDialog.ShowWindow(SW_HIDE);       \n\
    TheRectangularGridDialog.UpdateValues();         \n\
    TheRectangularGridDialog.ShowWindow(SW_SHOW);    \n\
  }                                                  \n\
  \n";

  // Update The Result Dialog
  GetDocument()->UpdateResultDialog(anID,Message);

}

void OCC_2dViewRD::OnBUTTONGridRectPoints() 
{
	OCC_2dViewRD::OnBUTTONGridRectPoints();
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON2DGridRectPoints;
  TCollection_AsciiString Message;
Message = "\
  Handle(V2d_Viewer) aViewer = myV2dView->Viewer();  \n\
  aViewer->SetGridColor(Quantity_Color(Quantity_NOC_WHITE), Quantity_Color(Quantity_NOC_WHITE)); \n\
  aViewer->ActivateGrid(Aspect_GT_Rectangular, \n\
                                      Aspect_GDM_Points); \n\
  if (TheCircularGridDialog.IsWindowVisible())       \n\
  {                                                  \n\
    TheCircularGridDialog.ShowWindow(SW_HIDE);       \n\
    TheRectangularGridDialog.UpdateValues();         \n\
    TheRectangularGridDialog.ShowWindow(SW_SHOW);    \n\
  }                                                  \n\
";
  // Update The Result Dialog
  GetDocument()->UpdateResultDialog(anID,Message);


}

void OCC_2dViewRD::OnBUTTONGridCircLines() 
{
	OCC_2dViewRD::OnBUTTONGridCircLines();
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON2DGridCircLines;
  TCollection_AsciiString Message("\
  Handle(V2d_Viewer) aViewer = myV2dView->Viewer(); \n\
  aViewer->SetGridColor(Quantity_Color(Quantity_NOC_WHITE), Quantity_Color(Quantity_NOC_WHITE)); \n\
  aViewer->ActivateGrid(Aspect_GT_Circular,   \n\
                                      Aspect_GDM_Lines); \n\
  if (TheRectangularGridDialog.IsWindowVisible())   \n\
  {                                                 \n\
    TheRectangularGridDialog.ShowWindow(SW_HIDE);   \n\
    TheCircularGridDialog.UpdateValues();           \n\
    TheCircularGridDialog.ShowWindow(SW_SHOW);      \n\
  } \n");                                             
    // Update The Result Dialog
  GetDocument()->UpdateResultDialog(anID,Message);

}

void OCC_2dViewRD::OnBUTTONGridCircPoints() 
{
	OCC_2dViewRD::OnBUTTONGridCircPoints();
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON2DGridCircPoints;
  TCollection_AsciiString Message("\
  Handle(V2d_Viewer) aViewer = myV2dView->Viewer();  \n\
  aViewer->SetGridColor(Quantity_Color(Quantity_NOC_WHITE), Quantity_Color(Quantity_NOC_WHITE)); \n\
  aViewer->ActivateGrid(Aspect_GT_Circular,    \n\
                                      Aspect_GDM_Points); \n\
  if (TheRectangularGridDialog.IsWindowVisible())    \n\
  {                                                  \n\
    TheRectangularGridDialog.ShowWindow(SW_HIDE);    \n\
    TheCircularGridDialog.UpdateValues();            \n\
    TheCircularGridDialog.ShowWindow(SW_SHOW);       \n\
  }                                                  \n\
  \n");

  // Update The Result Dialog
  GetDocument()->UpdateResultDialog(anID,Message);

}

void OCC_2dViewRD::OnBUTTONGridValues() 
{
	OCC_2dViewRD::OnBUTTONGridValues();
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON2DGridValues;
  TCollection_AsciiString Message("\
  Handle(V2d_Viewer) aViewer = myV2dView->Viewer();               \n\
  Aspect_GridType  TheGridtype = aViewer->GridType();       \n\
                                                                  \n\
  switch( TheGridtype )                                           \n\
  {                                                               \n\
    case  Aspect_GT_Rectangular:                            \n\
      TheRectangularGridDialog.UpdateValues();                    \n\
      TheRectangularGridDialog.ShowWindow(SW_SHOW);               \n\
    break;                                                        \n\
    case  Aspect_GT_Circular:                               \n\
      TheCircularGridDialog.UpdateValues();                       \n\
      TheCircularGridDialog.ShowWindow(SW_SHOW);                  \n\
    break;                                                        \n\
    default :                                                     \n\
      throw Standard_Failure(\"invalid Aspect_GridType\"); \n\
  }                                                               \n\
\n");
  // Update The Result Dialog
  GetDocument()->UpdateResultDialog(anID,Message);

}

void OCC_2dViewRD::OnBUTTONGridCancel() 
{	
	OCC_2dViewRD::OnBUTTONGridCancel();
  // Update Get information to update Result dialog

  UINT anID=ID_BUTTON2DGridCancel;
  TCollection_AsciiString Message("\
  Handle(V2d_Viewer) aViewer = myV2dView->Viewer(); \n\
  aViewer->DeactivateGrid();                        \n\
  TheRectangularGridDialog.ShowWindow(SW_HIDE);     \n\
  TheCircularGridDialog.ShowWindow(SW_HIDE);        \n\
  aViewer->Update();                                \n\
  \n");

  // Update The Result Dialog
  GetDocument()->UpdateResultDialog(anID,Message);

}


OCC_2dViewRD::OCC_2dViewRD()
{

}

OCC_2dViewRD::~OCC_2dViewRD()
{
}


OCC_2dDoc* OCC_2dViewRD::GetDocument()
{
	return (OCC_2dDoc*)m_pDocument;
}
