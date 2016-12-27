// ImportExportDoc.cpp : implementation of the CImportExportDoc class
//


#include "stdafx.h"
#include "ImportExportApp.h"

#include "ImportExportDoc.h"

#include <ImportExport/ImportExport.h>

#include <AISDialogs.h>
#include "res/resource.h"


#ifdef _DEBUG
//#define new DEBUG_NEW  // by cascade
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc

IMPLEMENT_DYNCREATE(CImportExportDoc, OCC_3dDoc)

BEGIN_MESSAGE_MAP(CImportExportDoc, OCC_3dDoc)
	//{{AFX_MSG_MAP(CImportExportDoc)
	ON_COMMAND(ID_FILE_IMPORT_BREP, OnFileImportBrep)
	ON_COMMAND(ID_FILE_IMPORT_IGES, OnFileImportIges)
	ON_COMMAND(ID_FILE_EXPORT_IGES, OnFileExportIges)
	ON_COMMAND(ID_FILE_IMPORT_STEP, OnFileImportStep)
	ON_COMMAND(ID_FILE_EXPORT_STEP, OnFileExportStep)
	ON_COMMAND(ID_FILE_EXPORT_VRML, OnFileExportVrml)
	ON_COMMAND(ID_FILE_EXPORT_STL, OnFileExportStl)
	ON_COMMAND(ID_BOX, OnBox)
	ON_COMMAND(ID_Cylinder, OnCylinder)
	ON_COMMAND(ID_OBJECT_REMOVE, OnObjectRemove)
	ON_COMMAND(ID_OBJECT_ERASE, OnObjectErase)
	ON_COMMAND(ID_OBJECT_DISPLAYALL, OnObjectDisplayall)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc construction/destruction

CImportExportDoc::CImportExportDoc()
: OCC_3dDoc (false)
{
/*
    // TRIHEDRON
	Handle(AIS_Trihedron) aTrihedron;
	Handle(Geom_Axis2Placement) aTrihedronAxis=new Geom_Axis2Placement(gp::XOY());
	aTrihedron=new AIS_Trihedron(aTrihedronAxis);
	myAISContext->Display(aTrihedron);
*/

	m_pcoloredshapeList = new CColoredShapes();
}

CImportExportDoc::~CImportExportDoc()
{
	if( m_pcoloredshapeList ) delete m_pcoloredshapeList;
}


/////////////////////////////////////////////////////////////////////////////
// CSerializeDoc serialization

void CImportExportDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// Put the curent CColoredShape in the archive
		ar << m_pcoloredshapeList;
	}
	else
	{
		// Read from the archive the current CColoredShape
		ar >> m_pcoloredshapeList;

		// Display the new object
		m_pcoloredshapeList->Display(myAISContext);
	}
}


/*
void CImportExportDoc::OnWindowNew3d() 
{
	((CImportExportApp*)AfxGetApp())->CreateView3D(this);	
}
*/

//  nCmdShow could be :    ( default is SW_RESTORE ) 
// SW_HIDE   SW_SHOWNORMAL   SW_NORMAL   
// SW_SHOWMINIMIZED     SW_SHOWMAXIMIZED    
// SW_MAXIMIZE          SW_SHOWNOACTIVATE   
// SW_SHOW              SW_MINIMIZE         
// SW_SHOWMINNOACTIVE   SW_SHOWNA           
// SW_RESTORE           SW_SHOWDEFAULT      
// SW_MAX    

// use pViewClass = RUNTIME_CLASS( CImportExportView3D ) for 3D Views

void CImportExportDoc::ActivateFrame(CRuntimeClass* pViewClass,int nCmdShow)
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

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc diagnostics

#ifdef _DEBUG
void CImportExportDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImportExportDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImportExportDoc commands


void CImportExportDoc::OnFileImportBrep()
{
	Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadBREP();
	for(int i=1;i<= aSeqOfShape->Length();i++)
	{
		m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
        m_pcoloredshapeList->Display(myAISContext);
	}
	Fit();
}

void CImportExportDoc::OnFileImportIges() 
{   
	Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadIGES();
	for(int i=1;i<= aSeqOfShape->Length();i++)
	{
		m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
        m_pcoloredshapeList->Display(myAISContext);
	}
	Fit();
}
void CImportExportDoc::OnFileExportIges() 
{   CImportExport::SaveIGES(myAISContext);}

void CImportExportDoc::OnFileImportStep() 
{   
	Handle(TopTools_HSequenceOfShape) aSeqOfShape = CImportExport::ReadSTEP();
	for(int i=1;i<= aSeqOfShape->Length();i++)
	{
		m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, aSeqOfShape->Value(i));
        m_pcoloredshapeList->Display(myAISContext);
	}
	Fit();
}
void CImportExportDoc::OnFileExportStep() 
{   CImportExport::SaveSTEP(myAISContext);}


void CImportExportDoc::OnFileExportVrml() 
{   CImportExport::SaveVRML(myAISContext);}

void CImportExportDoc::OnFileExportStl() 
{   CImportExport::SaveSTL(myAISContext);}

void  CImportExportDoc::Popup(const Standard_Integer  x,
							   const Standard_Integer  y ,
                               const Handle(V3d_View)& aView   ) 
{
  Standard_Integer PopupMenuNumber=0;
 myAISContext->InitSelected();
  if (myAISContext->MoreSelected())
    PopupMenuNumber=1;

  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_Popup3D));
  CMenu* pPopup = menu.GetSubMenu(PopupMenuNumber);

  ASSERT(pPopup != NULL);
   if (PopupMenuNumber == 1) // more than 1 object.
  {
    bool OneOrMoreInShading = false;
	for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1)) OneOrMoreInShading=true;
	if(!OneOrMoreInShading)
   	pPopup->EnableMenuItem(5, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
   }

  POINT winCoord = { x , y };
  Handle(WNT_Window) aWNTWindow=
  Handle(WNT_Window)::DownCast(aView->Window());
  ClientToScreen ( (HWND)(aWNTWindow->HWindow()),&winCoord);
  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , winCoord.x, winCoord.y , 
                         AfxGetMainWnd());


}

void CImportExportDoc::OnBox() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	BRepPrimAPI_MakeBox B(200.,150.,100.);

	m_pcoloredshapeList->Add(Quantity_NOC_YELLOW, B.Shape());

	m_pcoloredshapeList->Display(myAISContext);
	Fit();

	// document has been modified
	SetModifiedFlag(TRUE);
}

void CImportExportDoc::OnCylinder() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	BRepPrimAPI_MakeCylinder C(50.,200.);

	m_pcoloredshapeList->Add(Quantity_NOC_GREEN, C.Shape());

	m_pcoloredshapeList->Display(myAISContext);
	Fit();

	// document has been modified
	SetModifiedFlag(TRUE);
}
void CImportExportDoc::OnObjectRemove() 

{
	for(GetAISContext()->InitSelected();GetAISContext()->MoreSelected();GetAISContext()->NextSelected()) {
		Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(GetAISContext()->SelectedInteractive());
		if(!aShape.IsNull()) {
			m_pcoloredshapeList->Remove(aShape->Shape());
		}
	}
	OCC_3dBaseDoc::OnObjectRemove();
}

void CImportExportDoc::OnObjectErase() 

{
	for(GetAISContext()->InitSelected();GetAISContext()->MoreSelected();GetAISContext()->NextSelected()) {
		Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(GetAISContext()->SelectedInteractive());
		if(!aShape.IsNull()) {
			m_pcoloredshapeList->Remove(aShape->Shape());
		}
	}
	OCC_3dBaseDoc::OnObjectErase(); 
}

void CImportExportDoc::OnObjectDisplayall() 

{
	OCC_3dBaseDoc::OnObjectDisplayall(); 
}