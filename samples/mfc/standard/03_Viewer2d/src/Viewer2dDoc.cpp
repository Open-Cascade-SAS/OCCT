// Viewer2dDoc.cpp : implementation of the CViewer2dDoc class
//

#include "stdafx.h"
#include "Viewer2dDoc.h"
#include "OCC_App.h"
#include "Primitive\Sample2D_Markers.h"
#include "Primitive\Sample2D_Face.h"
#include "Primitive\Sample2D_Image.h"

/////////////////////////////////////////////////////////////////////////////
// CViewer2dDoc

IMPLEMENT_DYNCREATE(CViewer2dDoc, CDocument)

BEGIN_MESSAGE_MAP(CViewer2dDoc, CDocument)
	//{{AFX_MSG_MAP(CViewer2dDoc)
	ON_COMMAND(ID_BUTTON_Test_Text, OnBUTTONTestText)
 	ON_COMMAND(ID_BUTTON_Test_Markers, OnBUTTONTestMarkers)
	ON_COMMAND(ID_BUTTON_Test_Line, OnBUTTONTestLine)
	ON_COMMAND(ID_BUTTON_Erase, OnBUTTONErase)
	ON_COMMAND(ID_BUTTON_Test_Face, OnBUTTONTestFace)
	ON_COMMAND(ID_BUTTON_Test_Rect, OnBUTTONTestRect)
	ON_COMMAND(ID_BUTTON_Test_Curve, OnBUTTONTestCurve)
	ON_COMMAND(ID_BUTTON32793, OnBUTTONTestImage) // test image
	ON_COMMAND(ID_BUTTON_Test_MultiImages, OnBUTTONMultipleImage)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewer2dDoc construction/destruction

CViewer2dDoc::CViewer2dDoc()
{
}

CViewer2dDoc::~CViewer2dDoc()
{
}

#ifdef _DEBUG
void CViewer2dDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CViewer2dDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewer2dDoc commands

void CViewer2dDoc::OnBUTTONErase() 
{
  myAISContext->EraseAll (Standard_True);
}

void CViewer2dDoc::OnBUTTONTestText() 
{
  int aColor = Quantity_NOC_MATRABLUE;
  for (Standard_Real j = 15; j <= 20; j++)
  {
    Handle(AIS_TextLabel) aText = new AIS_TextLabel();
    aText->SetText (TCollection_AsciiString ("font 0 scale ") + (j / 20.0));
    aText->SetPosition (gp_Pnt (0.0, 15.0 * (j - 15.0), 0.0));
    aText->SetAngle (30.0 * M_PI / 180.0);
    aText->SetColor (Quantity_NameOfColor(aColor++));
    aText->SetFontAspect (Font_FA_Regular);
    aText->SetFont ("Courier");
    aText->SetHeight (j);
    aText->SetHJustification (Graphic3d_HTA_LEFT);
    aText->SetVJustification (Graphic3d_VTA_BOTTOM);
    aText->SetZoomable (Standard_False);
    myAISContext->Display(aText, Standard_False);
  }

  for (Standard_Real j = 10; j <= 15; j++)
  {
    Handle(AIS_TextLabel) aText = new AIS_TextLabel();
    aText->SetText (TCollection_AsciiString ("font 1 scale ") + (j / 10.0));
    aText->SetPosition (gp_Pnt (80.0, 15.0 * (j - 10.0), 0.0));
    aText->SetAngle (0.0);
    aText->SetColor (Quantity_NameOfColor(aColor++));
    aText->SetFontAspect (Font_FA_BoldItalic);
    aText->SetFont ("Cambria");
    aText->SetHeight (j * 2);
    aText->SetHJustification (Graphic3d_HTA_LEFT);
    aText->SetVJustification (Graphic3d_VTA_BOTTOM);
    aText->SetZoomable (Standard_False);
    myAISContext->Display(aText, Standard_False);
  }

  aColor = Quantity_NOC_MATRABLUE;
  for (Standard_Real j = 5; j <= 10; j++)
  {
    Handle(AIS_TextLabel) aText = new AIS_TextLabel();
    aText->SetText (TCollection_AsciiString ("font 2 scale ") + (j / 10.0));
    aText->SetPosition (gp_Pnt (140.0, 15.0 * (j - 5.0), 0.0));
    aText->SetAngle (0.0);
    aText->SetColor (Quantity_NameOfColor(aColor++));
    aText->SetFontAspect (Font_FA_Bold);
    aText->SetFont ("Arial");
    aText->SetHeight (j * 2);
    aText->SetHJustification (Graphic3d_HTA_LEFT);
    aText->SetVJustification (Graphic3d_VTA_BOTTOM);
    aText->SetZoomable (Standard_False);
    myAISContext->Display(aText, Standard_False);
  }
  for (Standard_Real j = 10; j <= 15; j++)
  {
    Handle(AIS_TextLabel) aText = new AIS_TextLabel();
    aText->SetText (TCollection_AsciiString ("font 3 scale ") + (j / 10.0));
    aText->SetPosition (gp_Pnt (200.0, 15.0 * (j - 10.0), 0.0));
    aText->SetAngle (0.0);
    aText->SetColor (Quantity_NameOfColor(aColor++));
    aText->SetFontAspect (Font_FA_Italic);
    aText->SetFont ("Georgia");
    aText->SetHeight (j * 2);
    aText->SetHJustification (Graphic3d_HTA_LEFT);
    aText->SetVJustification (Graphic3d_VTA_BOTTOM);
    aText->SetZoomable (Standard_False);
    myAISContext->Display(aText, Standard_False);
  }

  FitAll2DViews(Standard_True); // Update Viewer
}


void CViewer2dDoc::OnBUTTONTestMarkers() 
{
  // generic Markers
  Standard_Integer aColor = 20;
  for (int i=1;i<=2;i++)
  {
    Handle (Sample2D_Markers) aMarker  = 
      new Sample2D_Markers(10+5,5*i,Aspect_TOM_POINT,Quantity_NOC_YELLOW,2.0);
    myAISContext->Display(aMarker, Standard_False);
  }
  for (int i=1;i<=2;i++)
  {
    Handle (Sample2D_Markers) aMarker  = 
      new Sample2D_Markers(10+10,5*i,Aspect_TOM_O,(Quantity_NameOfColor)(aColor++));
    myAISContext->Display(aMarker, Standard_False);
  }
  for (int i=1;i<=2;i++)
  {
    Handle (Sample2D_Markers) aMarker  = 
      new Sample2D_Markers(10+15,5*i,Aspect_TOM_O_PLUS,(Quantity_NameOfColor)(aColor++));
    myAISContext->Display(aMarker, Standard_False);
  }
  for (int i=1;i<=2;i++)
  {
    Handle (Sample2D_Markers) aMarker  = 
      new Sample2D_Markers(10+20,5*i,Aspect_TOM_RING1,(Quantity_NameOfColor)(aColor++));
    myAISContext->Display(aMarker, Standard_False);
  }
  for (int i=1;i<=2;i++)
  {

    Handle (Sample2D_Markers) aMarker  = 
      new Sample2D_Markers(10+25,5*i,Aspect_TOM_STAR,(Quantity_NameOfColor)(aColor++));
    myAISContext->Display(aMarker, Standard_False);
  }
  for (int i=1;i<=2;i++)
  {
    Handle (Sample2D_Markers) aMarker  = 
      new Sample2D_Markers(10+30,5*i,Aspect_TOM_O_X,(Quantity_NameOfColor)(aColor++));
    myAISContext->Display(aMarker, Standard_False);
  }

  FitAll2DViews(Standard_False); // Update Viewer
}
void CViewer2dDoc::OnBUTTONTestLine() 
{
  for (int i=0;i<=13;++i)
   for (int j=0;j<=5;++j)
   {
      //set of rectangles here
      TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(10*i,10*j,0.), gp_Pnt(10*i+7,10*j,0.));
      TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(10*i+7,10*j,0.), gp_Pnt(10*i+7,10*j+5,0.));
      TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(10*i+7,10*j+5,0.), gp_Pnt(10*i,10*j+5,0.));
      TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(gp_Pnt(10*i,10*j+5,0.), gp_Pnt(10*i,10*j,0.));
      TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3,E4);
      TopoDS_Face F = BRepBuilderAPI_MakeFace(W);
      Handle(AIS_Shape) aRect = new AIS_Shape(F);
      //set attributes of boundaries
      Handle(Prs3d_Drawer) aDrawer = new Prs3d_Drawer();
      Handle(Prs3d_LineAspect) aLineAttrib = 
        new Prs3d_LineAspect(Quantity_NOC_YELLOW,
        (Aspect_TypeOfLine)(Aspect_TOL_SOLID+j),1);
      aDrawer->SetFaceBoundaryAspect(aLineAttrib);
      aDrawer->SetFaceBoundaryDraw(Standard_True);
      aRect->SetAttributes(aDrawer);
      
      myAISContext->SetDisplayMode (aRect, 1, Standard_False);
      myAISContext->SetColor(aRect,(Quantity_NameOfColor)(Quantity_NOC_CADETBLUE+2*i),Standard_False);
	    myAISContext->SetMaterial(aRect,Graphic3d_NOM_PLASTIC,Standard_False);
      myAISContext->Display(aRect, Standard_False);

   }
   this->FitAll2DViews(Standard_True);
}


#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>

void CViewer2dDoc::OnBUTTONTestFace()
{
  //erase all
  myAISContext->EraseAll (Standard_True);

  CFileDialog dlg(TRUE,
    NULL,
    NULL,
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
    L"BRep Files (*.brep)|*.brep; ||",
    NULL );

  CString initdir;
  initdir.GetEnvironmentVariable (L"CSF_OCCTDataPath");
  initdir += L"\\occ";

  dlg.m_ofn.lpstrInitialDir = initdir;

  if (dlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

    std::filebuf aFileBuf;
    std::istream aStream (&aFileBuf);
    if (!aFileBuf.open (dlg.GetPathName(), ios::in))
    {
      AfxMessageBox (L"The shape must be not a null Face");
      return;
    }

    TopoDS_Shape aFaceShape;
    BRep_Builder aBuilder;
    BRepTools::Read (aFaceShape, aStream, aBuilder);
    if(aFaceShape.IsNull() || aFaceShape.ShapeType() != TopAbs_FACE) 
    {
      AfxMessageBox (L"The shape must be not a null Face");
      return;
    }

    TopoDS_Face aFace = TopoDS::Face(aFaceShape);
    Handle(Sample2D_Face) anAISFace = new Sample2D_Face(aFaceShape);
    myAISContext->Display(anAISFace,Standard_True);
    //activate selection mode for edges selection
    myAISContext->Activate(anAISFace,2);

    FitAll2DViews(Standard_False);

  }
}



void CViewer2dDoc::OnBUTTONTestRect()
{
  //First rectangle
  TopoDS_Edge E11 = BRepBuilderAPI_MakeEdge(gp_Pnt(40.,0.,0.), gp_Pnt(82.5,25.,0.));
	TopoDS_Edge E12 = BRepBuilderAPI_MakeEdge(gp_Pnt(82.5,25.,0.), gp_Pnt(42.5,93.,0.));
	TopoDS_Edge E13 = BRepBuilderAPI_MakeEdge(gp_Pnt(42.5,93.,0.), gp_Pnt(0.,68.,0.));
  TopoDS_Edge E14 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,68.,0.), gp_Pnt(40.,0.,0.));
	TopoDS_Wire W1 = BRepBuilderAPI_MakeWire(E11,E12,E13,E14);
  Handle(AIS_Shape) aRect1 = new AIS_Shape(W1); 
  myAISContext->Display (aRect1, Standard_False);
  myAISContext->SetColor (aRect1, Quantity_NOC_YELLOW, Standard_False);
  
  //Second rectangle
  TopoDS_Edge E21 = BRepBuilderAPI_MakeEdge(gp_Pnt(110.,0.,0.), gp_Pnt(152.5,25.,0.));
	TopoDS_Edge E22 = BRepBuilderAPI_MakeEdge(gp_Pnt(152.5,25.,0.), gp_Pnt(112.5,93.,0.));
	TopoDS_Edge E23 = BRepBuilderAPI_MakeEdge(gp_Pnt(112.5,93.,0.), gp_Pnt(70.,68.,0.));
  TopoDS_Edge E24 = BRepBuilderAPI_MakeEdge(gp_Pnt(70.,68.,0.), gp_Pnt(110.,0.,0.));
	TopoDS_Wire W2 = BRepBuilderAPI_MakeWire(E21,E22,E23,E24);
  Handle(AIS_Shape) aRect2 = new AIS_Shape(W2); 
  myAISContext->Display (aRect2, Standard_False);
  myAISContext->SetColor (aRect2, Quantity_NOC_YELLOW, Standard_False);
  myAISContext->Activate(aRect2,2);

  FitAll2DViews(Standard_True); // Update Viewer
}

void CViewer2dDoc::OnBUTTONTestCurve()
{
  for(int i=0;i<=5;++i)
    for(int j=0;j<=5;++j)
    {
      Handle(Geom_Point) aStart = new Geom_CartesianPoint(gp_Pnt(10*i,10*j,0.));
      Handle(Geom_Point) anEnd = new Geom_CartesianPoint(gp_Pnt(10*i+5,10*j+10,0.));
      Handle(AIS_Line) aLine = new AIS_Line(aStart,anEnd);
      Handle(Prs3d_LineAspect) aLineAttrib = 
        new Prs3d_LineAspect((Quantity_NameOfColor)(Quantity_NOC_CADETBLUE+2*i+2*j),
        (Aspect_TypeOfLine)((Aspect_TOL_DASH+i+j)%5),2+i+j);
      aLine->Attributes()->SetLineAspect(aLineAttrib);
      myAISContext->Display(aLine,Standard_False);   
    }
   FitAll2DViews(Standard_True);
}

void CViewer2dDoc::OnBUTTONTestImage()
{
  CFileDialog anOpenImageDlg (TRUE,
                              NULL,
                              NULL,
                              OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                              SupportedImageFormats() + L"| all files (*.*)|*.*;||",
                              NULL);

  CString anInitDir;
  anInitDir.GetEnvironmentVariable (L"CASROOT");
  anInitDir += L"\\data\\images";

  anOpenImageDlg.m_ofn.lpstrInitialDir = anInitDir;
  if(anOpenImageDlg.DoModal() == IDOK)
  {
    SetCursor(AfxGetApp()->LoadStandardCursor (IDC_WAIT));
    CString aFilePath = anOpenImageDlg.GetPathName();
    TCollection_AsciiString aFileName ((const wchar_t* )aFilePath);

    //erase viewer
    myAISContext->EraseAll (Standard_False);

    Handle(Sample2D_Image) anImage = new Sample2D_Image (aFileName);
    anImage->SetCoord (40,50) ;
    anImage->SetScale (1.0);
    myAISContext->Display (anImage, Standard_False);
    myAISContext->SetDisplayMode (anImage,3,Standard_False);
    FitAll2DViews (Standard_True);
  }
}

void CViewer2dDoc::OnBUTTONMultipleImage()
{
  CFileDialog anOpenImageDlg (TRUE,
                              NULL,
                              NULL,
                              OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                              SupportedImageFormats() + L"| all files (*.*)|*.*;||",
                              NULL);

  CString anInitDir;
  anInitDir.GetEnvironmentVariable (L"CASROOT");
  anInitDir += L"\\data\\images";

  anOpenImageDlg.m_ofn.lpstrInitialDir = anInitDir;

  if (anOpenImageDlg.DoModal() == IDOK) 
  {
    SetCursor(AfxGetApp()->LoadStandardCursor (IDC_WAIT));
    CString aFilePath = anOpenImageDlg.GetPathName();
    TCollection_AsciiString aFileName ((const wchar_t* )aFilePath);

    //erase viewer
    myAISContext->EraseAll (Standard_False);

    //create images
    {  // 1
    Handle(Sample2D_Image) anImage = new Sample2D_Image (aFileName);
    anImage->SetCoord (40, 50);
    anImage->SetScale (0.5);
    myAISContext->Display (anImage, Standard_False);
    myAISContext->SetDisplayMode (anImage, 3, Standard_False);
    }
    {  // 2
    Handle(Sample2D_Image) anImage = new Sample2D_Image (aFileName);
    anImage->SetCoord (100, 50);
    anImage->SetScale (0.9);
    myAISContext->Display (anImage, Standard_False);
    myAISContext->SetDisplayMode (anImage, 3, Standard_False);
    }
    {  // 3
    Handle(Sample2D_Image) anImage = new Sample2D_Image (aFileName);
    anImage->SetCoord (40, 40);
    anImage->SetScale (0.3);
    myAISContext->Display (anImage, Standard_False);
    myAISContext->SetDisplayMode (anImage, 3, Standard_False);
    }
    {  // 4
    Handle(Sample2D_Image) anImage = new Sample2D_Image (aFileName);
    anImage->SetCoord (50, 40);
    myAISContext->Display (anImage, Standard_False);
    myAISContext->SetDisplayMode (anImage, 3, Standard_False);
    }
    {  // 5
    Handle(Sample2D_Image) anImage = new Sample2D_Image (aFileName);
    anImage->SetCoord (80, 45);
    anImage->SetScale (2);
    myAISContext->Display (anImage, Standard_False);
    myAISContext->SetDisplayMode (anImage, 3, Standard_False);
    }
    {  // 6
    Handle(Sample2D_Image) anImage = new Sample2D_Image (aFileName);
    anImage->SetCoord (20, -20);
    myAISContext->Display (anImage, Standard_False);
    myAISContext->SetDisplayMode (anImage, 3, Standard_False);
    }
    {  // 7
    Handle(Sample2D_Image) anImage = new Sample2D_Image (aFileName);
    anImage->SetCoord (0, 0);
    anImage->SetScale (0.5);
    myAISContext->Display (anImage, Standard_False);
    myAISContext->SetDisplayMode (anImage, 3, Standard_False);
    }
    FitAll2DViews (Standard_True); // Update Viewer
  }
}
