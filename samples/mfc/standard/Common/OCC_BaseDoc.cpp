// OCC_BaseDoc.cpp: implementation of the OCC_BaseDoc class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "OCC_BaseDoc.h"

const CString OCC_BaseDoc::SupportedImageFormats() const
{
  return ("BMP Files (*.BMP)|*.bmp|GIF Files (*.GIF)|*.gif|TIFF Files (*.TIFF)|*.tiff|"
          "PPM Files (*.PPM)|*.ppm|JPEG Files(*.JPEG)|*.jpeg|PNG Files (*.PNG)|*.png|"
          "EXR Files (*.EXR)|*.exr|TGA Files (*.TGA)|*.tga");
}

void OCC_BaseDoc::ExportView (const Handle(V3d_View)& theView) const
{
   CFileDialog anExportDlg (FALSE,_T("*.BMP"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                            SupportedImageFormats() + "||", NULL );

  if (anExportDlg.DoModal() == IDOK)
  {
    // Set waiting cursor
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

    CString aFileExt = anExportDlg.GetFileExt();
    TCollection_AsciiString aFileName ((const wchar_t* )anExportDlg.GetPathName());

    // For pixel formats use V3d_View:Dump() method
    theView->Dump (aFileName.ToCString());

    // Restore cursor
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OCC_BaseDoc::OCC_BaseDoc()
{

}

OCC_BaseDoc::~OCC_BaseDoc()
{

}

//=============================================================================
// function: ResetDocumentViews
// purpose:
//=============================================================================
void OCC_BaseDoc::ResetDocumentViews (CDocTemplate* theTemplate)
{
  // do not delete document if no views
  BOOL isAutoDelete = m_bAutoDelete;
  m_bAutoDelete = FALSE;

  // close all opened views
  POSITION aViewIt = GetFirstViewPosition();
  while (aViewIt)
  {
    CView* aView = GetNextView (aViewIt);
    if (aView == NULL)
    {
      continue;
    }

    RemoveView (aView);

    aView->GetParentFrame()->SendMessage (WM_CLOSE);
  }

  // create new view frame
  CFrameWnd* aNewFrame = theTemplate->CreateNewFrame (this, NULL);
  m_bAutoDelete = isAutoDelete;

  // init frame
  theTemplate->InitialUpdateFrame(aNewFrame, this);



}
