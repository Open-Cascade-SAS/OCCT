// OCC_BaseDoc.cpp: implementation of the OCC_BaseDoc class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "OCC_BaseDoc.h"

const CString OCC_BaseDoc::SupportedImageFormats() const
{
  return ("BMP Files (*.BMP)|*.bmp|GIF Files (*.GIF)|*.gif|TIFF Files (*.TIFF)|*.tiff|"
          "PPM Files (*.PPM)|*.ppm|JPEG Files(*.JPEG)|*.jpeg|PNG Files (*.PNG)|*.png|"
          "EXR Files (*.EXR)|*.exr|TGA Files (*.TGA)|*.tga|PS Files (*.PS)|*.ps|"
          "EPS Files (*.EPS)|*.eps|TEX Files (*.TEX)|*.tex|PDF Files (*.PDF)|*.pdf"
          "|SVG Files (*.SVG)|*.svg|PGF Files (*.PGF)|*.pgf");
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
    TCollection_ExtendedString aFileNameW ((Standard_ExtString )(const wchar_t* )anExportDlg.GetPathName());
    TCollection_AsciiString    aFileName  (aFileNameW, '?');

    // For vector formats use V3d_View::Export() method
    if (!(aFileExt.CompareNoCase (L"ps"))  || !(aFileExt.CompareNoCase (L"pdf"))
     || !(aFileExt.CompareNoCase (L"eps")) || !(aFileExt.CompareNoCase (L"tex"))
     || !(aFileExt.CompareNoCase (L"svg")) || !(aFileExt.CompareNoCase (L"pgf")))
    {
      Graphic3d_ExportFormat anExportFormat;

      if      (!(aFileExt.CompareNoCase (L"ps")))  anExportFormat = Graphic3d_EF_PostScript;
      else if (!(aFileExt.CompareNoCase (L"eps"))) anExportFormat = Graphic3d_EF_EnhPostScript;
      else if (!(aFileExt.CompareNoCase (L"pdf"))) anExportFormat = Graphic3d_EF_PDF;
      else if (!(aFileExt.CompareNoCase (L"tex"))) anExportFormat = Graphic3d_EF_TEX;
      else if (!(aFileExt.CompareNoCase (L"svg"))) anExportFormat = Graphic3d_EF_SVG;
      else anExportFormat = Graphic3d_EF_PGF;

      theView->Export (aFileName.ToCString(), anExportFormat);
    }
    else
    {
      // For pixel formats use V3d_View:Dump() method
      theView->Dump (aFileName.ToCString());
    }

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
