// HLRDoc.cpp : implementation of the CHLRDoc class
//


#include "stdafx.h"

#include "HLRDoc.h"

#include "HLRApp.h"
#include <OCC_2dView.h>
#include <OCC_3dView.h>


#include <ImportExport/ImportExport.h>
#include "AISDialogs.h"
#include "Properties/PropertiesSheet.h"

#include <AIS_ListOfInteractive.hxx>

#ifdef _DEBUG
//#define new DEBUG_NEW  // by cascade
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHLRDoc

IMPLEMENT_DYNCREATE(CHLRDoc, CDocument)


BEGIN_MESSAGE_MAP(CHLRDoc, OCC_3dBaseDoc)
	//{{AFX_MSG_MAP(CHLRDoc)
	ON_COMMAND(ID_WINDOW_NEW3D, OnWindowNew3d)
	ON_COMMAND(ID_WINDOW_NEW2D, OnWindowNew2d)
	ON_COMMAND(ID_FILE_IMPORT_BREP, OnFileImportBrep)
	ON_COMMAND(ID_BUTTON_HLRDialog, OnBUTTONHLRDialog)
	ON_COMMAND(ID_MENU_CASCADE_PROPERTIES, OnBUTTONTest2DProperties)
  //}}AFX_MSG_MAP


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHLRDoc construction/destruction

CHLRDoc::CHLRDoc()
{
	// TODO: add one-time construction code here

	Handle(Graphic3d_WNTGraphicDevice) theGraphicDevice = 
		((CHLRApp*)AfxGetApp())->GetGraphicDevice();

    TCollection_ExtendedString a3DName("Visu3D");
	myViewer = new V3d_Viewer(theGraphicDevice,a3DName.ToExtString(),"", 1000.0, 
                              V3d_XposYnegZpos, Quantity_NOC_GRAY30,
                              V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT, 
                              Standard_True, Standard_False);
	/*
	Handle(V3d_Viewer) myViewerCollector;
	myViewerCollector = new V3d_Viewer(theGraphicDevice,a3DName.ToExtString(),"", 1000.0, 
                              V3d_XposYnegZpos, Quantity_NOC_GRAY30,
                              V3d_ZBUFFER,V3d_GOURAUD,V3d_WAIT, 
                              Standard_True, Standard_False);
	*/
	myViewer->SetDefaultLights();
	myViewer->SetLightOn();

	//myAISContext =new AIS_InteractiveContext(myViewer,myViewerCollector);

	myAISContext =new AIS_InteractiveContext(myViewer);

    // VIEWER 2D
    TCollection_ExtendedString a2DName("Visu2D");
    my2DViewer = new V2d_Viewer(theGraphicDevice,a2DName.ToExtString());
    my2DViewer->SetCircularGridValues(0,0,10,8,0);
    my2DViewer->SetRectangularGridValues(0,0,10,10,0);
	
    myInteractiveContext2D= new ISession2D_InteractiveContext(my2DViewer);

    CFrameWnd* pFrame2d=((CHLRApp*)AfxGetApp())->CreateView2D(this);
	pFrame2d->ShowWindow(SW_SHOWNORMAL/*SW_MINIMIZE*/);

    // update the Maps :
    //  entries are reserve for utilisation :
    //   - 1 for Visible edges HighLighted
    //   - 2 for Visible edges 
    //   - 3 for Hidden  edges HighLighted
    //   - 4 for Hidden  edges 

    Handle(Aspect_GenericColorMap) aColorMap= Handle(Aspect_GenericColorMap)::DownCast(my2DViewer->ColorMap());
    ASSERT(!aColorMap.IsNull());
    aColorMap->AddEntry(Aspect_ColorMapEntry (1,Quantity_Color(Quantity_NOC_RED  ))); // in fact just update
    aColorMap->AddEntry(Aspect_ColorMapEntry (2,Quantity_Color(Quantity_NOC_WHITE))); // in fact just update
    aColorMap->AddEntry(Aspect_ColorMapEntry (3,Quantity_Color(Quantity_NOC_RED  ))); // in fact just update
    aColorMap->AddEntry(Aspect_ColorMapEntry (4,Quantity_Color(Quantity_NOC_BLUE1))); // in fact just update
    my2DViewer->SetColorMap(aColorMap);

    Handle(Aspect_WidthMap) aWidthMap = my2DViewer->WidthMap();
    aWidthMap->AddEntry(Aspect_WidthMapEntry(1,0.8));  // in fact just update
    aWidthMap->AddEntry(Aspect_WidthMapEntry(2,0.4));  // in fact just update
    aWidthMap->AddEntry(Aspect_WidthMapEntry(3,0.6));  // in fact just update
    aWidthMap->AddEntry(Aspect_WidthMapEntry(4,0.2));  // in fact just update
    my2DViewer->SetWidthMap(aWidthMap);

    Handle(Aspect_TypeMap) aTypeMap = my2DViewer->TypeMap();
    aTypeMap->AddEntry(Aspect_TypeMapEntry(1,Aspect_LineStyle(Aspect_TOL_SOLID)));
    aTypeMap->AddEntry(Aspect_TypeMapEntry(2,Aspect_LineStyle(Aspect_TOL_SOLID)));
    TColQuantity_Array1OfLength anArray(1,2);
    anArray(1) = 0.5;    anArray(2) = 0.5;
    aTypeMap->AddEntry(Aspect_TypeMapEntry(3,Aspect_LineStyle(anArray)));
    aTypeMap->AddEntry(Aspect_TypeMapEntry(4,Aspect_LineStyle(anArray)));
    my2DViewer->SetTypeMap(aTypeMap);
    
    myCSelectionDialogIsCreated=false;
}

CHLRDoc::~CHLRDoc()
{
    if (myCSelectionDialogIsCreated)
    {
      myCSelectionDialog->ShowWindow(SW_ERASE);
      delete myCSelectionDialog;
    }
}

void CHLRDoc::OnWindowNew2d() 
{
	((CHLRApp*)AfxGetApp())->CreateView2D(this);	
}

void CHLRDoc::OnWindowNew3d() 
{
	((CHLRApp*)AfxGetApp())->CreateView3D(this);	
}

//  nCmdShow could be :    ( default is SW_RESTORE ) 
// SW_HIDE   SW_SHOWNORMAL   SW_NORMAL   
// SW_SHOWMINIMIZED     SW_SHOWMAXIMIZED    
// SW_MAXIMIZE          SW_SHOWNOACTIVATE   
// SW_SHOW              SW_MINIMIZE         
// SW_SHOWMINNOACTIVE   SW_SHOWNA           
// SW_RESTORE           SW_SHOWDEFAULT      
// SW_MAX    

// use pViewClass = RUNTIME_CLASS( CHLRView3D ) for 3D Views
// use pViewClass = RUNTIME_CLASS( CHLRView2D ) for 2D Views

void CHLRDoc::ActivateFrame(CRuntimeClass* pViewClass,int nCmdShow)
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
     if(pCurrentView->IsKindOf(pViewClass) )
    {
        ASSERT_VALID(pCurrentView);
        CFrameWnd* pParentFrm = pCurrentView->GetParentFrame();
	    ASSERT(pParentFrm != (CFrameWnd *)NULL);
        // simply make the frame window visible
	    pParentFrm->ActivateFrame(nCmdShow);
    }
  }
}

void CHLRDoc::FitAll2DViews(Standard_Boolean UpdateViewer)
{
  if (UpdateViewer)   my2DViewer->Update();
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
    if(pCurrentView->IsKindOf(RUNTIME_CLASS(OCC_2dView)) )
     {
      ASSERT_VALID(pCurrentView);
      ((OCC_2dView*)pCurrentView)->GetV2dView()->Fitall();
    }
  }
}

void CHLRDoc::OnBUTTONTest2DProperties() 
{
	// TODO: Add your command handler code here
  CPropertiesSheet aDial;
  aDial.SetViewer(my2DViewer);
  aDial.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CHLRDoc diagnostics

#ifdef _DEBUG
void CHLRDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHLRDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHLRDoc commands
void CHLRDoc::OnBUTTONHLRDialog() 

{
  if (!myCSelectionDialogIsCreated)
    {
     myCSelectionDialog = new CSelectionDialog(this,AfxGetMainWnd());
     myCSelectionDialog->Create(CSelectionDialog::IDD, AfxGetMainWnd());
     myCSelectionDialogIsCreated = true;
    }
  myCSelectionDialog->ShowWindow(SW_RESTORE);	
}

void CHLRDoc::OnFileImportBrep() 
{   CImportExport::ReadBREP(myAISContext);
	Fit();
}
void CHLRDoc::Fit()
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
	CView* pCurrentView = (CView*)GetNextView(position);
	if(pCurrentView->IsKindOf(RUNTIME_CLASS(OCC_3dView)) )
    {
		((OCC_3dView *) pCurrentView)->FitAll();
    }
  }
}
