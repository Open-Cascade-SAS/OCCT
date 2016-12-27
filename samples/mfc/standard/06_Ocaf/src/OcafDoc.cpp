// OcafDoc.cpp : implementation of the COcafDoc class
//

#include "StdAfx.h"

#include "OcafDoc.h"

#include "OcafApp.h"
#include <ImportExport/ImportExport.h>
#include "AISDialogs.h"

// Dialog boxes classes
#include <ResultDialog.h>
#include <NewBoxDlg.h>
#include <NewCylDlg.h>

#include <TDF_Tool.hxx>

#include <DebugBrowser.hxx>

#ifdef _DEBUG
//#define new DEBUG_NEW  // by cascade
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COcafDoc

IMPLEMENT_DYNCREATE(COcafDoc, CDocument)

BEGIN_MESSAGE_MAP(COcafDoc, OCC_3dBaseDoc)
	//{{AFX_MSG_MAP(COcafDoc)
	ON_COMMAND(ID_CREATEBOX, OnCreatebox)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_MODIFY, OnModify)
	ON_UPDATE_COMMAND_UI(ID_MODIFY, OnUpdateModify)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_CUT, OnCut)
	ON_COMMAND(ID_OBJECT_DELETE, OnObjectDelete)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_DELETE, OnUpdateObjectDelete)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_CREATECYL, OnCreatecyl)
//	ON_COMMAND(ID_DFBR, OnDfbr)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COcafDoc construction/destruction

COcafDoc::COcafDoc()
{
}

COcafDoc::~COcafDoc()
{
}

 BOOL COcafDoc::OnNewDocument()
 {
 	if (!CDocument::OnNewDocument())
 		return FALSE;
 
 	// Get an Handle on the current TOcaf_Application (which is initialized in the "Ocaf.h" file)
 	Handle(TOcaf_Application) OcafApp = ((COcafApp*)AfxGetApp())->GetApp();
 
 	// Create a new Ocaf document
 	OcafApp->NewDocument("BinOcaf",myOcafDoc);
 	TPrsStd_AISViewer::New(myOcafDoc->Main(),myViewer);
 
 	Handle(AIS_InteractiveContext) CTX;
 	TPrsStd_AISViewer::Find(myOcafDoc->Main(), CTX);
 	CTX->SetDisplayMode (AIS_Shaded, Standard_True);
 	myAISContext=CTX;
 
 	// Set the maximum number of available "undo" actions
 	myOcafDoc->SetUndoLimit(10);
 
 
 		TCollection_AsciiString Message ("\
 //  Creation of a new document \n\
  \n\
 Handle(TOcaf_Application) OcafApp= ((COcafApp*)AfxGetApp())->GetApp(); \n\
  \n\
 //  Creating the new document \n\
 OcafApp->NewDocument(\"BinOcaf\", myOcafDoc); \n\
  \n\
 //  Creation of a new TPrsStd_AISViewer connected to the current V3d_Viewer\n\
 TPrsStd_AISViewer::New(myOcafDoc->Main(),myViewer); \n\
  \n\
 //  Setting the number of memorized undos \n\
 myOcafDoc->SetUndoLimit(10); \n\
 \n");

  myCResultDialog.SetTitle("New document");
 	CString text(Message.ToCString());
 	myCResultDialog.SetText(text);
 
 	PathName="";
 
 	return TRUE;
 }

void COcafDoc::ActivateFrame(CRuntimeClass* pViewClass,int nCmdShow)
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
// COcafDoc diagnostics

#ifdef _DEBUG
void COcafDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COcafDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COcafDoc commands

void COcafDoc::OnEditRedo() 
{
	myOcafDoc->Redo();
	myOcafDoc->CommitCommand();
	myAISContext->UpdateCurrentViewer();

	UpdateAllViews(NULL);

	TCollection_AsciiString Message = TCollection_AsciiString("\
//  Redo last undoes operation \n\
 \n\
myOcafDoc->Redo(); \n\
 \n\
myOcafDoc->CommitCommand(); \n\
 \n\
\n");

	myCResultDialog.SetTitle("Redo");
	CString text(Message.ToCString());
	myCResultDialog.SetText(text);
}

void COcafDoc::OnEditUndo() 
{
	myOcafDoc->Undo();
	myOcafDoc->CommitCommand();
	myAISContext->UpdateCurrentViewer();
	
	UpdateAllViews(NULL);

	TCollection_AsciiString Message = TCollection_AsciiString("\
//  Undo last operation \n\
 \n\
myOcafDoc->Undo(); \n\
 \n\
myOcafDoc->CommitCommand(); \n\
 \n\
\n");

	myCResultDialog.SetTitle("Undo");
	CString text(Message.ToCString());
	myCResultDialog.SetText(text);
}

void COcafDoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	// Disable the "redo" button if there is no availlables redo actions
	if (myOcafDoc->GetAvailableRedos()>0) pCmdUI->Enable(Standard_True);
	else pCmdUI->Enable(Standard_False);	
}

void COcafDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// Disable the "undo" button if there is no availlables undo actions
	if (myOcafDoc->GetAvailableUndos()>0) pCmdUI->Enable(Standard_True);
	else pCmdUI->Enable(Standard_False);
}

void COcafDoc::OnCreatebox() 
{
	EraseAll();
	CNewBoxDlg Dlg;
	if(Dlg.DoModal()!=IDOK) return;

	Handle(TDocStd_Document) D = GetOcafDoc();

	// Open a new command (for undo)
	D->NewCommand();
	TOcaf_Commands TSC(D->Main());

	// Create a new box using the CNewBoxDlg Dialog parameters as attributes
  TDF_Label L = TSC.CreateBox (Dlg.m_x, Dlg.m_y, Dlg.m_z, Dlg.m_w, Dlg.m_l, Dlg.m_h,
                               TCollection_ExtendedString ((const wchar_t* )Dlg.m_Name));

	// Get the TPrsStd_AISPresentation of the new box TNaming_NamedShape
	Handle(TPrsStd_AISPresentation) prs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); 
	// Display it
	prs->Display(1);
	Fit3DViews();
	// Attach an integer attribute to L to memorize it's displayed
	TDataStd_Integer::Set(L, 1);
	myAISContext->UpdateCurrentViewer();

	// Close the command (for undo)
	D->CommitCommand();


		TCollection_AsciiString Message ("\
//  Creation of a new box using Ocaf attributes \n\
 \n\
Handle(TDocStd_Document) D = GetOcafDoc(); \n\
 \n\
//  Openning a new command (for undo/redo) \n\
D->NewCommand(); \n\
 \n\
TOcaf_Commands TSC(D->Main()); \n\
// Look at the TOcaf_Commands::CreateBox() function \n\
TDF_Label L=TSC.CreateBox(m_x, m_y, m_z, m_w, m_l, m_h, Name); \n\
 \n\
// Set the TPrsStd_AISPresentation of the box \n\
Handle(TPrsStd_AISPresentation) prs; \n\
prs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); \n\
 \n\
//  Displaying the box \n\
prs->Display(1); \n\
 \n\
//  Commint the command (for undo/redo) \n\
D->CommitCommand(); \n\
\n");

	myCResultDialog.SetTitle("Create box");
	CString text(Message.ToCString());
	myCResultDialog.SetText(text);
}

void COcafDoc::OnCreatecyl() 
{
	EraseAll();
	CNewCylDlg Dlg;
	if(Dlg.DoModal()!=IDOK) return;

	Handle(TDocStd_Document) D = GetOcafDoc();

	// Open a new command (for undo)
	D->NewCommand();
	TOcaf_Commands TSC(D->Main());

	// Create a new box using the CNewCylDlg Dialog parameters as attributes
  TDF_Label L = TSC.CreateCyl (Dlg.m_x, Dlg.m_y, Dlg.m_z, Dlg.m_r, Dlg.m_h,
                               TCollection_ExtendedString ((const wchar_t* )Dlg.m_Name));

	// Get the TPrsStd_AISPresentation of the new cylinder TNaming_NamedShape
	Handle(TPrsStd_AISPresentation) prs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); 
	// Display it
	prs->Display(1);
	Fit3DViews();
	// Attach an integer attribute to L to memorize it's displayed
	TDataStd_Integer::Set(L, 1);
	myAISContext->UpdateCurrentViewer();

	// Close the command (for undo)
	D->CommitCommand();


		TCollection_AsciiString Message ("\
//  Creation of a new cylinder using Ocaf attributes \n\
 \n\
Handle(TDocStd_Document) D = GetOcafDoc(); \n\
 \n\
//  Openning a new command (for undo/redo) \n\
D->NewCommand(); \n\
 \n\
TOcaf_Commands TSC(D->Main()); \n\
// Look at the TOcaf_Commands::CreateCyl() function \n\
TDF_Label L=TSC.CreateCyl(m_x, m_y, m_z, m_r, m_h, Name); \n\
 \n\
// Set the TPrsStd_AISPresentation of the cylinder \n\
Handle(TPrsStd_AISPresentation) prs; \n\
prs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); \n\
 \n\
//  Displaying the cylinder \n\
prs->Display(1); \n\
 \n\
//  Commint the command (for undo/redo) \n\
D->CommitCommand(); \n\
\n");

	myCResultDialog.SetTitle("Create cylinder");
	CString text(Message.ToCString());
	myCResultDialog.SetText(text);
}

void COcafDoc::OnModify() 
{
	// Get the selected interactive object
	myAISContext->InitSelected();
	Handle(AIS_InteractiveObject) curAISObject = myAISContext->SelectedInteractive();


	// Get the main label of the selected object
	Handle(TPrsStd_AISPresentation) ObjectPrs = 
		Handle(TPrsStd_AISPresentation)::DownCast(curAISObject->GetOwner());
	TDF_Label LabObject = ObjectPrs->Label();

	// Get the TFunction_Function attribute of the selected object
	Handle(TFunction_Function) TFF; 
	if ( !LabObject.FindAttribute(TFunction_Function::GetID(),TFF) )
	{
		MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Object cannot be modify.", L"Modification", MB_ICONEXCLAMATION);
		return;
	}
	// Get the Standard_GUID of the TFunction_FunctionDriver of the selected object TFunction_Function attribute
	Standard_GUID myDriverID=TFF->GetDriverGUID();

	Handle(TDocStd_Document) D = GetOcafDoc();
	Handle(TFunction_Logbook) log = TFunction_Logbook::Set(D->Main());

	TCollection_AsciiString Message("\
//  Modification and recomputation of the selected object \n\
 \n\
Handle(TDocStd_Document) D = GetOcafDoc(); \n\
 \n\
//  Getting the TPrsStd_AISPresentation of the selected object \n\
Handle(TPrsStd_AISPresentation) ObjectPrs =  \n\
	Handle(TPrsStd_AISPresentation)::DownCast(curAISObject->GetOwner()); \n\
 \n\
//  Getting the Label of the selected object using the TPrsStd_AISPresentation \n\
TDF_Label LabObject = ObjectPrs->Label(); \n\
 \n\
//  Getting the TFunction_FunctionDriver ID attached to this label \n\
Handle(TFunction_Function) TFF; \n\
\n");
	TCollection_AsciiString Suite;

	// Case of a box created with the box function driver
	if(myDriverID==TOcafFunction_BoxDriver::GetID())
	{
		CNewBoxDlg Dlg;
		Standard_Real x, y, z, w, l, h;

		// Get the attributes values of the current box
		Handle(TDataStd_Real) curReal;
		LabObject.FindChild(1).FindAttribute(TDataStd_Real::GetID(),curReal);
		w=curReal->Get();
		LabObject.FindChild(2).FindAttribute(TDataStd_Real::GetID(),curReal);
		l=curReal->Get();
		LabObject.FindChild(3).FindAttribute(TDataStd_Real::GetID(),curReal);
		h=curReal->Get();
		LabObject.FindChild(4).FindAttribute(TDataStd_Real::GetID(),curReal);
		x=curReal->Get();
		LabObject.FindChild(5).FindAttribute(TDataStd_Real::GetID(),curReal);
		y=curReal->Get();
		LabObject.FindChild(6).FindAttribute(TDataStd_Real::GetID(),curReal);
		z=curReal->Get();
		Handle(TDataStd_Name) stdName;
		LabObject.FindAttribute(TDataStd_Name::GetID(),stdName);

		// Initialize the dialog box with the values of the current box
		Dlg.InitFields(x, y, z, w, l, h, stdName->Get());

		if(Dlg.DoModal()!=IDOK) return;

		// Open a new command (for undo)
		D->NewCommand();

		// Modify the box
		TOcaf_Commands TSC(LabObject);
		TSC.ModifyBox (Dlg.m_x, Dlg.m_y, Dlg.m_z, Dlg.m_w, Dlg.m_l, Dlg.m_h,
                   TCollection_ExtendedString ((const wchar_t* )Dlg.m_Name), log);

		// Get the presentation of the box, display it and set it selected
		Handle(TPrsStd_AISPresentation) prs= TPrsStd_AISPresentation::Set(LabObject, TNaming_NamedShape::GetID()); 
		TDataStd_Integer::Set(LabObject, 1);
		prs->Display(1);
		myAISContext->UpdateCurrentViewer();
		// Close the command (for undo)
		D->CommitCommand();

		Message+=("\
 \n\
//  In this case the TFunction_FunctionDriver ID is a BoxDriver \n\
if(myDriverID==TOcafFunction_BoxDriver::GetID()){ \n\
 \n\
//  Getting values of box attributes \n\
Handle(TDataStd_Real) curReal; \n\
LabObject.FindChild(1).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
w=curReal->Get(); \n\
LabObject.FindChild(2).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
l=curReal->Get(); \n\
LabObject.FindChild(3).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
h=curReal->Get(); \n\
LabObject.FindChild(4).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
x=curReal->Get(); \n\
LabObject.FindChild(5).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
y=curReal->Get(); \n\
LabObject.FindChild(6).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
z=curReal->Get(); \n\
Handle(TDataStd_Name) stdName; \n\
LabObject.FindAttribute(TDataStd_Name::GetID(),stdName); \n\
 \n\
//  Openning a new command \n\
D->NewCommand(); \n\
 \n\
TOcaf_Commands TSC(LabObject); \n\
// Look at the TOcaf_Commands::ModifyBox() function \n\
TSC.ModifyBox(m_x, m_y, m_z, m_w, m_l, m_h, Name); \n\
 \n\
// Set the TPrsStd_AISPresentation of the box \n\
Handle(TPrsStd_AISPresentation) prs; \n\
prs= TPrsStd_AISPresentation::Set(LabObject, TNaming_NamedShape::GetID()); \n\
 \n\
// Display the box \n\
prs->Display(1); \n\
 \n\
// Commit the command \n\
D->CommitCommand(); \n\
} \n\
\n");

		myCResultDialog.SetTitle("Modify Box");
	}
	// Case of a cylinder created with the box function driver
	else if(myDriverID==TOcafFunction_CylDriver::GetID())
	{
		CNewCylDlg Dlg;
		Standard_Real x, y, z, r, h;

		// Get the attributes values of the current box
		Handle(TDataStd_Real) curReal;
		LabObject.FindChild(1).FindAttribute(TDataStd_Real::GetID(),curReal);
		r=curReal->Get();
		LabObject.FindChild(2).FindAttribute(TDataStd_Real::GetID(),curReal);
		h=curReal->Get();
		LabObject.FindChild(3).FindAttribute(TDataStd_Real::GetID(),curReal);
		x=curReal->Get();
		LabObject.FindChild(4).FindAttribute(TDataStd_Real::GetID(),curReal);
		y=curReal->Get();
		LabObject.FindChild(5).FindAttribute(TDataStd_Real::GetID(),curReal);
		z=curReal->Get();
		Handle(TDataStd_Name) stdName;
		LabObject.FindAttribute(TDataStd_Name::GetID(),stdName);

		// Initialize the dialog cylinder with the values of the current cylinder
		Dlg.InitFields(x, y, z, r, h, stdName->Get());

		if(Dlg.DoModal()!=IDOK) return;

		// Open a new command (for undo)
		D->NewCommand();

		// Modify the cylinder
		TOcaf_Commands TSC(LabObject);
		TSC.ModifyCyl (Dlg.m_x, Dlg.m_y, Dlg.m_z, Dlg.m_r, Dlg.m_h,
                   TCollection_ExtendedString ((const wchar_t* )Dlg.m_Name), log);

		// Get the presentation of the cylinder, display it and set it selected
		Handle(TPrsStd_AISPresentation) prs= TPrsStd_AISPresentation::Set(LabObject, TNaming_NamedShape::GetID()); 
		TDataStd_Integer::Set(LabObject, 1);
		prs->Display(1);
		myAISContext->UpdateCurrentViewer();
		// Close the command (for undo)
		D->CommitCommand();

		Message+=("\
 \n\
//  In this case the TFunction_FunctionDriver ID is a CylDriver \n\
if(myDriverID==TOcafFunction_CylDriver::GetID()){ \n\
 \n\
//  Getting values of box cylinder \n\
Handle(TDataStd_Real) curReal; \n\
LabObject.FindChild(1).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
r=curReal->Get(); \n\
LabObject.FindChild(2).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
h=curReal->Get(); \n\
LabObject.FindChild(3).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
x=curReal->Get(); \n\
LabObject.FindChild(4).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
y=curReal->Get(); \n\
LabObject.FindChild(5).FindAttribute(TDataStd_Real::GetID(),curReal); \n\
z=curReal->Get(); \n\
Handle(TDataStd_Name) stdName; \n\
LabObject.FindAttribute(TDataStd_Name::GetID(),stdName); \n\
 \n\
//  Openning a new command \n\
D->NewCommand(); \n\
 \n\
TOcaf_Commands TSC(LabObject); \n\
// Look at the TOcaf_Commands::ModifyCyl() function \n\
TSC.ModifyCyl(m_x, m_y, m_z, m_r, m_h, Name); \n\
 \n\
// Set the TPrsStd_AISPresentation of the cylinder \n\
Handle(TPrsStd_AISPresentation) prs; \n\
prs= TPrsStd_AISPresentation::Set(LabObject, TNaming_NamedShape::GetID()); \n\
 \n\
// Display the cylinder \n\
prs->Display(1); \n\
 \n\
// Commit the command \n\
D->CommitCommand(); \n\
} \n\
\n");

		myCResultDialog.SetTitle("Modify cylinder");
	}
	// Case of a cut solid created with the cut function driver
	else if(myDriverID==TOcafFunction_CutDriver::GetID())
	{
		// Open a new command (for undo)
		D->NewCommand();

		// Get the reference of the Original object used to make the cut object, 
		// this reference is here attached to the first child of the cut object label 
		Handle(TDF_Reference) OriginalRef;
		LabObject.FindChild(1).FindAttribute(TDF_Reference::GetID(),OriginalRef);

		// Get the presentation of the Original object
		Handle(TPrsStd_AISPresentation) OriginalPrs= TPrsStd_AISPresentation::Set(OriginalRef->Get(), TNaming_NamedShape::GetID()); 

		// Get the reference of the Tool object used to make the cut object, 
		// this reference is here attached to the second child of the cut object label 
		Handle(TDF_Reference) ToolRef;
		LabObject.FindChild(2).FindAttribute(TDF_Reference::GetID(),ToolRef);
		TDF_Label ToolLab=ToolRef->Get();

		
		// Get the presentation of the Tool object
		Handle(TPrsStd_AISPresentation) ToolPrs= TPrsStd_AISPresentation::Set(ToolLab, TNaming_NamedShape::GetID()); 

		// Display the Original object and the Tool object, erase the cut object.
		TDataStd_Integer::Set(OriginalRef->Get(), 1);
		OriginalPrs->Display(1);
		TDataStd_Integer::Set(ToolLab, 1);
		ToolPrs->Display(1);
		TDataStd_Integer::Set(LabObject, 0);
		ObjectPrs->Erase(1);
		myAISContext->UpdateCurrentViewer();

		// In this  we decided to modify the tool object which is a cylinder, 
		// so we open the cylinder parameters dialog box
		CNewCylDlg Dlg;
		Standard_Real x, y, z, h, r;

		// Get the attributes values of the tool cylinder
		Handle(TDataStd_Real) curReal;
		ToolLab.FindChild(1).FindAttribute(TDataStd_Real::GetID(),curReal);
		r=curReal->Get();
		ToolLab.FindChild(2).FindAttribute(TDataStd_Real::GetID(),curReal);
		h=curReal->Get();
		ToolLab.FindChild(3).FindAttribute(TDataStd_Real::GetID(),curReal);
		x=curReal->Get();
		ToolLab.FindChild(4).FindAttribute(TDataStd_Real::GetID(),curReal);
		y=curReal->Get();
		ToolLab.FindChild(5).FindAttribute(TDataStd_Real::GetID(),curReal);
		z=curReal->Get();
		Handle(TDataStd_Name) stdName;
		ToolLab.FindAttribute(TDataStd_Name::GetID(),stdName);

		// Initialize the dialog box with the values of the tool cylinder
		Dlg.InitFields(x, y, z, r, h, stdName->Get());

		if(Dlg.DoModal()!=IDOK) 
		{
			D->AbortCommand();
			myAISContext->UpdateCurrentViewer();
			return;
		}

		// Modify the cylinder
		TOcaf_Commands ToolTSC(ToolLab);
		ToolTSC.ModifyCyl (Dlg.m_x, Dlg.m_y, Dlg.m_z, Dlg.m_r, Dlg.m_h,
                       TCollection_ExtendedString ((const wchar_t* )Dlg.m_Name), log);

		// Redisplay the modified Tool object
		TDataStd_Integer::Set(ToolLab, 1);
		ToolPrs->Display(1);
		myAISContext->UpdateCurrentViewer();
		Sleep(1000);

		// Get the TOcafFunction_CutDriver using its Standard_GUID in the TFunction_DriverTable
		Handle(TFunction_Driver) myCutDriver;
		if (TFunction_DriverTable::Get()->FindDriver(myDriverID, myCutDriver))
            myCutDriver->Init(LabObject);

		// Recompute the cut object if it must be (look at the MustExecute function code)
//		if (myCutDriver->MustExecute(log))
//		{
			log->SetTouched(LabObject);
			if(myCutDriver->Execute(log))
				MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Recompute failed", L"Modify cut", MB_ICONEXCLAMATION);
//		}

		// Erase the Original object and the Tool objectedisplay the modified Tool object
		TDataStd_Integer::Set(ToolLab, 0);
		ToolPrs->Erase(0);
		TDataStd_Integer::Set(OriginalRef->Get(), 0);
		OriginalPrs->Erase(0);
		ObjectPrs=TPrsStd_AISPresentation::Set(LabObject, TNaming_NamedShape::GetID()); 
		TDataStd_Integer::Set(LabObject, 1);
		ObjectPrs->Display(1);
		myAISContext->UpdateCurrentViewer();
		// Close the command (for undo)
		D->CommitCommand();

		Message+=("\
 \n\
//  In this case the TFunction_FunctionDriver ID is a CutDriver \n\
if(myDriverID==TOcafFunction_CutDriver::GetID()){ \n\
 \n\
//  Getting values of cut attributes (which are reference to the shapes)\n\
Handle(TDF_Reference) OriginalRef; \n\
LabObject.FindChild(1).FindAttribute(TDF_Reference::GetID(),OriginalRef); \n\
Handle(TDF_Reference) ToolRef; \n\
LabObject.FindChild(2).FindAttribute(TDF_Reference::GetID(),ToolRef); \n\
 \n\
//  Getting the label of the tool shape (to modify it)\n\
TDF_Label ToolLab=ToolRef->Get(); \n\
 \n\
TOcaf_Commands TSC(ToolLab); \n\
// Look at the TOcaf_Commands::ModifyBox() function \n\
TSC.ModifyBox(m_x, m_y, m_z, m_w, m_l, m_h, Name); \n\
 \n\
//  Getting the TOcafFunction_CutDriver used to create the cut\n\
Handle(TFunction_Driver) myCutDriver; \n\
TFunction_DriverTable::Get()->FindDriver(myDriverID, myCutDriver); \n\
 \n\
//  Recompute the cut if it must be (if an attribute was modified)\n\
if (myCutDriver->MustExecute(log)) { \n\
	log.SetTouched(LabObject); \n\
	myCutDriver->Execute(log) \n\
} \n\
Handle(TPrsStd_AISPresentation) prs; \n\
 \n\
//  Setting the TPrsStd_AISPresentation of the cut object\n\
prs= TPrsStd_AISPresentation::Set(LabObject, TNaming_NamedShape::GetID()); \n\
 \n\
//  Display the TPrsStd_AISPresentation of the cut object\n\
prs->Display(1); \n\
 \n\
//  Commit the command\n\
D->CommitCommand(); \n\
} \n\
\n");

		myCResultDialog.SetTitle("Modify Cut");
	}
	else
	{
		MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"No associated function driver", L"Modify", MB_OK);
	}

	CString text(Message.ToCString());
	myCResultDialog.SetText(text);
}

void COcafDoc::OnUpdateModify(CCmdUI* pCmdUI) 
{
	// Disable the "modify" button if there is no selected object or several selected objects 
	myAISContext->InitSelected();
	if(myAISContext->NbSelected()!=1)
	{
		pCmdUI->Enable(Standard_False);
		return;
	}

	// Get the root label of the selected object using its TPrsStd_AISPresentation
	myAISContext->InitSelected();
	Handle(TPrsStd_AISPresentation) ObjectPrs = 
		Handle(TPrsStd_AISPresentation)::DownCast(myAISContext->SelectedInteractive()->GetOwner());
	if (!ObjectPrs.IsNull()){
		TDF_Label LabObject = ObjectPrs->Label();


		// Disable the "modify" button if selected object don't have a TFunction_Function attribute
		Handle(TFunction_Function) TFF; 
		pCmdUI->Enable(LabObject.FindAttribute(TFunction_Function::GetID(),TFF) );
	}
	else
		pCmdUI->Enable(FALSE);
}


void COcafDoc::OnCut() 
{
	EraseAll();
	Handle(TDocStd_Document) D = GetOcafDoc();
	TDF_Label L = D->Main();

	TOcaf_Commands TSC(L);

	// Open a new command (for undo)
	D->NewCommand();

	// Create a new cyl (10*20 dimensions)
	TDF_Label L2=TSC.CreateCyl(0, 10, -10, 5, 80, "Cylinder");
	Handle(TPrsStd_AISPresentation) prs1= TPrsStd_AISPresentation::Set(L2, TNaming_NamedShape::GetID()); 
	prs1->SetColor(Quantity_NOC_MATRABLUE);
	TDataStd_Integer::Set(L2, 1);
	prs1->Display(1);
	Fit3DViews();

	// Wait a second to see the construction
	Sleep(500);

	// Create a new box (20*20*20 dimensions, at the (-12,0,10) position)
	TDF_Label L1=TSC.CreateBox(-12, 0, 10, 20, 30, 40, "Box");
	Handle(TPrsStd_AISPresentation) prs2= TPrsStd_AISPresentation::Set(L1, TNaming_NamedShape::GetID()); 
	prs2->SetColor(Quantity_NOC_YELLOW);
	TDataStd_Integer::Set(L1, 1);
	prs2->Display(1);
	Fit3DViews();

	Sleep(1000);

	// Cut the boxes
	L=TSC.Cut(L1, L2);
	Handle(TPrsStd_AISPresentation) prs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID());

	// Erase the two boxes and display the cut object
	TDataStd_Integer::Set(L1, 0);
	prs1->Erase(0);
	TDataStd_Integer::Set(L2, 0);
	prs2->Erase(0);
	TDataStd_Integer::Set(L, 1);
	prs->Display(1);
	myAISContext->UpdateCurrentViewer();

	// Close the cut operation command (for undo)
	D->CommitCommand();


		TCollection_AsciiString Message ("\
//  Cut operation between a cylinder and a box \n\
 \n\
Handle(TDocStd_Document) D = GetOcafDoc(); \n\
TDF_Label L = D->Main(); \n\
 \n\
//  Openning a new command\n\
D->NewCommand(); \n\
 \n\
TOcaf_Commands TSC(D->Main()); \n\
 \n\
//  Create a box \n\
// Look at the TOcaf_Commands::CreateBox() function \n\
TDF_Label L1=TSC.CreateBox(-12, 0, 10, 24, 20, 20, '\"'Box'\"'); \n\
// Look at the TOcaf_Commands::Cut() function \n\
 \n\
//  Create a cylinder \n\
// Look at the TOcaf_Commands::CreateCyl() function \n\
//  Create two boxes \n\
TDF_Label L2=TSC.CreateCyl(0, 0, 0, 10, 20, '\"'Cylinder'\"'); \n\
 \n\
//  Cut the cylinder with the box \n\
L=TSC.Cut(L1, L2); \n\
Handle(TPrsStd_AISPresentation) prs; \n\
 \n\
//  Setting the TPrsStd_AISPresentation of the cut object\n\
prs= TPrsStd_AISPresentation::Set(L, TNaming_NamedShape::GetID()); \n\
 \n\
//  Displaying the TPrsStd_AISPresentation of the cut object\n\
prs->Display(1); \n\
 \n\
//  Commit the command\n\
D->CommitCommand(); \n\
\n");

	myCResultDialog.SetTitle("Cut operation");
	CString text(Message.ToCString());
	myCResultDialog.SetText(text);
}


void COcafDoc::Fit3DViews()
{
  POSITION position = GetFirstViewPosition();
  while (position != (POSITION)NULL)
  {
    CView* pCurrentView = (CView*)GetNextView(position);
     if(pCurrentView->IsKindOf( RUNTIME_CLASS( OCC_3dView ) ) )
    {
        ASSERT_VALID(pCurrentView);
        OCC_3dView* aOCC_3dView = (OCC_3dView*)pCurrentView;
        aOCC_3dView->FitAll();
    }
  }

}

void COcafDoc::OnObjectDelete() 
{
	Handle(TDocStd_Document) D = GetOcafDoc();

	D->NewCommand();

	AIS_SequenceOfInteractive aSequence;
	for(myAISContext->InitSelected();
      myAISContext->MoreSelected();
      myAISContext->NextSelected())
        aSequence.Append(myAISContext->SelectedInteractive());
	
	for(int iter=1;iter <=aSequence.Length();iter++)
	{
		if (myAISContext->DisplayStatus(aSequence(iter)) == AIS_DS_Displayed)
 		{
 			Handle(TPrsStd_AISPresentation) CurrentPrs
 				= Handle(TPrsStd_AISPresentation)::DownCast(aSequence(iter)->GetOwner()); 
 			TDataStd_Integer::Set(CurrentPrs->Label(), 0);
 			CurrentPrs->Erase(1);
 		}
 	}
 
 	D->CommitCommand();
 	myAISContext->UpdateCurrentViewer();
 
 		TCollection_AsciiString Message ("\
 // Delete selected object \n\
  \n\
 Handle(TDocStd_Document) D = GetOcafDoc(); \n\
  \n\
 //  Openning a new command\n\
 D->NewCommand(); \n\
  \n\
 // Getting the  TPrsStd_AISPresentation of the selected object\n\
 Handle(TPrsStd_AISPresentation) CurrentPrs \n\
     =Handle(TPrsStd_AISPresentation)::DownCast(myAISContext->Current()->GetOwner());  \n\
  \n\
 // Erasing the  TPrsStd_AISPresentation of the selected object\n\
 CurrentPrs->Erase(1); \n\
  \n\
 //  Commit the command\n\
 D->CommitCommand(); \n\
 \n");

  myCResultDialog.SetTitle("Delete");
 	CString text(Message.ToCString());
 	myCResultDialog.SetText(text);
 }
 
 void COcafDoc::OnUpdateObjectDelete(CCmdUI* pCmdUI) 
 {
     myAISContext->InitSelected();
	pCmdUI->Enable (myAISContext->MoreSelected());
 }
 
 void COcafDoc::DisplayPrs()
 {
 	TDF_Label LabSat = myOcafDoc->Main();
 
 	for (TDF_ChildIterator it(LabSat); it.More(); it.Next())
 	{
 		TDF_Label L = it.Value();
 		Handle(TNaming_NamedShape) TNS;
		if (!L.FindAttribute(TNaming_NamedShape::GetID(), TNS)) continue;
		Handle(TDataStd_Integer) TDI;

		// To know if the object was displayed
		if (L.FindAttribute(TDataStd_Integer::GetID(), TDI))
			if(!TDI->Get())  continue;

		Handle(TPrsStd_AISPresentation) prs;
		if (!L.FindAttribute(TPrsStd_AISPresentation::GetID(),prs))
			prs = TPrsStd_AISPresentation::Set(L,TNaming_NamedShape::GetID());
		prs->SetColor(Quantity_NOC_ORANGE);
		prs->Display(1);
    }

	myAISContext->UpdateCurrentViewer();
}

void COcafDoc::OnCloseDocument() 
{
	Handle(TOcaf_Application) OcafApp = ((COcafApp*)AfxGetApp())->GetApp();
	OcafApp->Close(myOcafDoc);
	CDocument::OnCloseDocument();
}

void COcafDoc::OnFileSaveAs() 
{
	const wchar_t* SPathName = PathName;
	TCollection_ExtendedString TPathName (SPathName);

	CString Filter;

	if (TPathName.SearchFromEnd(".xml") > 0){
		Filter = "OCAFSample(XML) (*.xml)|*.xml|OCAFSample(Binary) (*.cbf)|*.cbf||";
	} else {
		Filter = "OCAFSample(Binary) (*.cbf)|*.cbf|OCAFSample(XML) (*.xml)|*.xml||";
	}
	
	CFileDialog dlg(FALSE,
			L"cbf",
			GetTitle(),
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			Filter,
			NULL );
	

	Handle(TOcaf_Application) m_App= ((COcafApp*)AfxGetApp())->GetApp();
	if (dlg.DoModal() != IDOK) return;

	SetTitle(dlg.GetFileTitle());

	CWaitCursor aWaitCursor;
	CString CSPath = dlg.GetPathName();

	cout << "Save As " << CSPath << endl;
	PathName=CSPath;
        const wchar_t* SPath = CSPath;
	TCollection_ExtendedString TPath (SPath);

	    if (TPath.SearchFromEnd(".xml") > 0)
		{
		  // The document must be saved in XML format
			myOcafDoc->ChangeStorageFormat("XmlOcaf");
		}
		else if (TPath.SearchFromEnd(".cbf") > 0)
		{
			// The document must be saved in binary format
			myOcafDoc->ChangeStorageFormat("BinOcaf");
		}

		try
		{
			// Saves the document in the current application
			m_App->SaveAs(myOcafDoc,TPath);
		}
		catch(...)
		{
			AfxMessageBox (L"Error! The file wasn't saved.");
    		return;
		}
//	// save the document in the current application
//	m_App->SaveAs(myOcafDoc,TPath);

	SetPathName(SPath);

	TCollection_AsciiString Message = TCollection_AsciiString("\
//  Storing the document as \n\
 \n\
Handle(TOcaf_Application) m_App= ((COcafApp*)AfxGetApp())->GetApp(); \n\
 \n\
//  Saving current OCAF document at TPath \n\
m_App->SaveAs(myOcafDoc,(TCollection_ExtendedString) TPath); \n\
\n");

	myCResultDialog.SetTitle("Save a document");
	CString text(Message.ToCString());
	myCResultDialog.SetText(text);
}

void COcafDoc::OnFileSave() 
{
	Handle(TOcaf_Application) m_App= ((COcafApp*)AfxGetApp())->GetApp();

	
	if (myOcafDoc.IsNull())
	{
	  AfxMessageBox (L"Error during saving! Empty document.");
	  return;
	}

	if(PathName!="")
	{
          const wchar_t* SPath = PathName;
          TCollection_ExtendedString TPath (SPath);

	    if (TPath.SearchFromEnd(".xml") > 0)
		{
		  // The document must be saved in XML format
			myOcafDoc->ChangeStorageFormat("XmlOcaf");
		}
		else if (TPath.SearchFromEnd(".cbf") > 0)
		{
			// The document must be saved in binary format
			myOcafDoc->ChangeStorageFormat("BinOcaf");
		}

		try
		{
			// Saves the document in the current application
			m_App->SaveAs(myOcafDoc,TPath);
		}
		catch(...)
		{
			AfxMessageBox (L"Error! The file wasn't saved.");
   		    return;
		}

	TCollection_AsciiString Message = TCollection_AsciiString("\
//  Storing the document \n\
 \n\
Handle(TOcaf_Application) m_App= ((COcafApp*)AfxGetApp())->GetApp(); \n\
 \n\
//  Saving current OCAF document at TPath \n\
m_App->SaveAs(myOcafDoc,(TCollection_ExtendedString) TPath); \n\
\n");

		myCResultDialog.SetTitle("Save a document");
		CString text(Message.ToCString());
		myCResultDialog.SetText(text);
		return;
	}

	CFileDialog dlg(FALSE,
			L"cbf",
			GetTitle(),
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			L"OCAFSample(Binary) (*.cbf)|*.cbf|OCAFSample(XML) (*.xml)|*.xml||",
			NULL );

	if (dlg.DoModal() != IDOK) return;

	SetTitle(dlg.GetFileTitle());

	CWaitCursor aWaitCursor;
	CString CSPath = dlg.GetPathName();

	const wchar_t* SPath = CSPath;
    TCollection_ExtendedString TPath (SPath);

    // Choose storage format
    if (TPath.SearchFromEnd(".xml") > 0)
	{
	  // The document must be saved in XML format
	  myOcafDoc->ChangeStorageFormat("XmlOcaf");
	}
	else if (TPath.SearchFromEnd(".cbf") > 0)
	{
	  // The document must be saved in binary format
	  myOcafDoc->ChangeStorageFormat("BinOcaf");
	}

   try
   {
	  // Saves the document in the current application
	  m_App->SaveAs(myOcafDoc,TPath);
   }
   catch(...)
   {
	  AfxMessageBox (L"Error! The file wasn't saved.");
	  return;
   }

	SetPathName(SPath);
	PathName=CSPath;


	TCollection_AsciiString Message = TCollection_AsciiString("\
//  Storing the document as \n\
 \n\
Handle(TOcaf_Application) m_App= ((COcafApp*)AfxGetApp())->GetApp(); \n\
 \n\
//  Saving current OCAF document at TPath \n\
m_App->SaveAs(myOcafDoc,(TCollection_ExtendedString) TPath); \n\
\n");

	myCResultDialog.SetTitle("Save a document");
	CString text(Message.ToCString());
	myCResultDialog.SetText(text);
}

BOOL COcafDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
  if (!CDocument::OnOpenDocument(lpszPathName))
  {
    return FALSE;
  }

  Handle(TOcaf_Application) m_App= ((COcafApp*)AfxGetApp())->GetApp();

  CWaitCursor aWaitCursor;
  PathName = lpszPathName;

  const wchar_t* aPathName = lpszPathName;
  TCollection_ExtendedString anOccPathName (aPathName);

  // Open the document in the current application
  //PCDM_ReaderStatus RS = m_App->Open(TPath,myOcafDoc);
  m_App->Open(anOccPathName, myOcafDoc);
  //CDF_RetrievableStatus RS = m_App->Open(TPath,myOcafDoc);

  // Connect the document CAF (myDoc) with the AISContext (myAISContext)
  TPrsStd_AISViewer::New (myOcafDoc->Main(), myViewer);
  myOcafDoc->SetUndoLimit (10);

  Handle(AIS_InteractiveContext) aContext;
  TPrsStd_AISViewer::Find (myOcafDoc->Main(), aContext);
  aContext->SetDisplayMode (AIS_Shaded, Standard_True);
  myAISContext = aContext;

  // Display the presentations (which was not stored in the document)
  DisplayPrs();

  TCollection_AsciiString Message = TCollection_AsciiString("\
//  Retrieve a document \n\
 \n\
 Handle(TOcaf_Application) m_App= ((COcafApp*)AfxGetApp())->GetApp(); \n\
 \n\
//  Openning the OCAF document from the TPath file\n\
m_App->Open((TCollection_ExtendedString) TPath,myOcafDoc); \n\
 \n\
//  Creation of a new TPrsStd_AISViewer connected to the current V3d_Viewer\n\
TPrsStd_AISViewer::New(myOcafDoc->Main(),myViewer); \n\
 \n\
//  Setting the number of memorized undos \n\
myOcafDoc->SetUndoLimit(10); \n\
 \n\
\n");

  myCResultDialog.SetTitle ("Open a document");
  CString text (Message.ToCString());
  myCResultDialog.SetText (text);

  return TRUE;

}


void COcafDoc::EraseAll()
{
	myOcafDoc->NewCommand();

	TDF_Label LabSat = myOcafDoc->Main();

	for (TDF_ChildIterator it(LabSat); it.More(); it.Next())
	{
		TDF_Label L = it.Value();
		Handle(TNaming_NamedShape) TNS;
		if (!L.FindAttribute(TNaming_NamedShape::GetID(), TNS)) continue;
		Handle(TDataStd_Integer) TDI;

		// To know if the object was displayed
		if (L.FindAttribute(TDataStd_Integer::GetID(), TDI))
			if(!TDI->Get())  continue;

		Handle(TPrsStd_AISPresentation) prs;
		if (!L.FindAttribute(TPrsStd_AISPresentation::GetID(),prs))
			prs = TPrsStd_AISPresentation::Set(L,TNaming_NamedShape::GetID());
		prs->SetColor(Quantity_NOC_ORANGE);
		prs->Erase(1);
    }

	myAISContext->UpdateCurrentViewer();

	myOcafDoc->CommitCommand();
}

void  COcafDoc::Popup(const Standard_Integer  x,
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


//void COcafDoc::OnDfbr() 
//{
//	// TODO: Add your command handler code here
//	DebugBrowser::DFBrowser(myOcafDoc);
//	
//}
