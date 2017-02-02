// TriangulationDoc.cpp : implementation of the CTriangulationDoc class
//

#include "stdafx.h"

#include "TriangulationDoc.h"

#include "TriangulationApp.h"

#include "OCCDemo_Presentation.h"

#include <OCC_3dView.h>

#include "..\res\resource.h"

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

/////////////////////////////////////////////////////////////////////////////
// CTriangulationDoc

IMPLEMENT_DYNCREATE(CTriangulationDoc, CDocument)

BEGIN_MESSAGE_MAP(CTriangulationDoc, OCC_3dBaseDoc)
	//{{AFX_MSG_MAP(CTriangulationDoc)
	ON_COMMAND(ID_TRIANGU, OnTriangu)
	ON_COMMAND(ID_Clear, OnClear)
	ON_COMMAND(ID_Visu, OnVisu)
	ON_COMMAND(ID_BUTTONNext, OnBUTTONNext)
	ON_COMMAND(ID_BUTTONStart, OnBUTTONStart)
	ON_COMMAND(ID_BUTTONRepeat, OnBUTTONRepeat)
	ON_COMMAND(ID_BUTTONPrev, OnBUTTONPrev)
	ON_COMMAND(ID_BUTTONEnd, OnBUTTONEnd)
	ON_UPDATE_COMMAND_UI(ID_BUTTONNext, OnUpdateBUTTONNext)
	ON_UPDATE_COMMAND_UI(ID_BUTTONPrev, OnUpdateBUTTONPrev)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_DUMP_VIEW, OnDumpView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTriangulationDoc construction/destruction

CTriangulationDoc::CTriangulationDoc()
{
	myPresentation = OCCDemo_Presentation::Current;
	myPresentation->SetDocument(this);

	strcpy_s(myDataDir, "Data");
	strcpy_s(myLastPath, ".");
}

CTriangulationDoc::~CTriangulationDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTriangulationDoc diagnostics

#ifdef _DEBUG
void CTriangulationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTriangulationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CTriangulationDoc::OnTriangu() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60);
	TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80);
	TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);
	BRepMesh_IncrementalMesh(ShapeFused,1);

	Handle (AIS_Shape)	aSection = new AIS_Shape(ShapeFused);
	myAISContext->SetDisplayMode (aSection, 1, Standard_False);
	myAISContext->SetColor (aSection, Quantity_NOC_RED, Standard_False);
	myAISContext->SetMaterial (aSection, Graphic3d_NOM_GOLD, Standard_False);
	myAISContext->Display (aSection, Standard_False);

	Standard_Integer result(0);

	for (TopExp_Explorer ex(ShapeFused,TopAbs_FACE) ; ex.More(); ex.Next()) {
		TopoDS_Face F =TopoDS::Face(ex.Current());
		TopLoc_Location L;
		Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(F,L);
		result = result + facing->NbTriangles();
	}
	Fit();

 TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60);	\n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80);	\n\
	\n\
TopoDS_Shape ShapeFused = BRepBuilderAPI_Fuse(theSphere,theBox);	\n\
	\n\
BRepMesh::Mesh(ShapeFused,1);	\n\
	\n\
Standard_Integer result(0);	\n\
	\n\
for (TopExp_Explorer ex(ShapeFused,TopAbs_FACE) ; ex.More(); ex.Next()) {	\n\
	TopoDS_Face F =TopoDS::Face(ex.Current());	\n\
	TopLoc_Location L;	\n\
	Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(F,L);	\n\
	result = result + facing->NbTriangles();	\n\
} 	\n\
\n\
--- Number of created triangles ---\n");
	TCollection_AsciiString nombre(result);
	Message += nombre;
	Message +=("\
				  \n\
				\n");
	PocessTextInDialog("Compute the triangulation on a shape", Message);
}

void CTriangulationDoc::OnVisu() 
{

	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60).Shape();
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100, 20, 20), 80).Shape();
TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere, theBox).Shape();
BRepMesh_IncrementalMesh(ShapeFused,1);

Handle (AIS_Shape)	aSection = new AIS_Shape(ShapeFused);
myAISContext->SetDisplayMode (aSection, 1, Standard_False);
myAISContext->SetColor (aSection, Quantity_NOC_RED, Standard_False);
myAISContext->SetMaterial (aSection, Graphic3d_NOM_GOLD, Standard_False);
myAISContext->SetTransparency (aSection, 0.1, Standard_False);
myAISContext->Display (aSection, Standard_False);

BRep_Builder builder;
TopoDS_Compound Comp;
builder.MakeCompound(Comp);

for (TopExp_Explorer ex(ShapeFused,TopAbs_FACE) ; ex.More(); ex.Next()) {
  		
	TopoDS_Face F =TopoDS::Face(ex.Current());
    TopLoc_Location L;
	Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(F,L);
    TColgp_Array1OfPnt tab(1,(facing->NbNodes()));
	tab = facing->Nodes();
	Poly_Array1OfTriangle tri(1,facing->NbTriangles());
	tri = facing->Triangles();

	for (Standard_Integer i=1;i<=(facing->NbTriangles());i++) {
		Poly_Triangle trian = tri.Value(i);
		Standard_Integer index1,index2,index3,M = 0, N = 0;
		trian.Get(index1,index2,index3);
	
		for (Standard_Integer j=1;j<=3;j++) {
			switch (j) {
			case 1 :
				M = index1;
				N = index2;
			break;
			case 2 :
				N = index3;
			break;
			case 3 :
				M = index2;
			}
			
			BRepBuilderAPI_MakeEdge ME(tab.Value(M),tab.Value(N));
			if (ME.IsDone()) {
				builder.Add(Comp,ME.Edge());
			}
		}
	}
}
Handle (AIS_Shape)	atriangulation = new AIS_Shape(Comp);
myAISContext->SetDisplayMode (atriangulation, 0, Standard_False);
myAISContext->SetColor (atriangulation, Quantity_NOC_WHITE, Standard_False);
myAISContext->Display (atriangulation, Standard_False);

Fit();

  TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60);	\n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80);	\n\
TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);	\n\
BRepMesh::Mesh(ShapeFused,1);	\n\
	\n\
BRep_Builder builder;	\n\
TopoDS_Compound Comp;	\n\
builder.MakeCompound(Comp);	\n\
	\n\
for (TopExp_Explorer ex(ShapeFused,TopAbs_FACE) ; ex.More(); ex.Next()) {	\n\
  			\n\
	TopoDS_Face F =TopoDS::Face(ex.Current());	\n\
	TopLoc_Location L;	\n\
	Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(F,L);	\n\
	TColgp_Array1OfPnt tab(1,(facing->NbNodes()));	\n\
	tab = facing->Nodes();	\n\
	Poly_Array1OfTriangle tri(1,facing->NbTriangles());	\n\
	tri = facing->Triangles();	\n\
	\n\
	for (Standard_Integer i=1;i<=(facing->NbTriangles());i++) {	\n\
		Poly_Triangle trian = tri.Value(i);	\n\
		Standard_Integer index1,index2,index3,M,N;	\n\
		trian.Get(index1,index2,index3);	\n\
		\n\
		for (Standard_Integer j=1;j<=3;j++) {	\n\
			switch (j) {	\n\
			case 1 :	\n\
				M = index1;	\n\
				N = index2;	\n\
			break;	\n\
			case 2 :	\n\
				N = index3;	\n\
			break;	\n\
			case 3 :	\n\
				M = index2;	\n\
			}	\n\
				\n\
			BRepBuilderAPI_MakeEdge ME(tab.Value(M),tab.Value(N));	\n\
			if (ME.IsDone()) {	\n\
				builder.Add(Comp,ME.Edge());	\n\
			}	\n\
		}	\n\
	}	\n\
}	\n\
	\n\
Warning : The visualisation of the mesh is not optimised.\n\
The shared edges between triangles are dispayed twice.\n\
The purpose here is only to show how to decode the data structure of triangulation.\n\
	\n");
  PocessTextInDialog("Visualize the triangulation on a shape", Message);

}




void CTriangulationDoc::OnClear() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200, 60, 60).Shape();
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100, 20, 20), 80).Shape();
TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);
BRepMesh_IncrementalMesh(ShapeFused,1);


Handle (AIS_Shape)	aSection = new AIS_Shape(ShapeFused);
myAISContext->SetDisplayMode (aSection, 1, Standard_False);
myAISContext->SetColor (aSection, Quantity_NOC_RED, Standard_False);
myAISContext->SetMaterial (aSection, Graphic3d_NOM_GOLD, Standard_False);
myAISContext->Display (aSection, Standard_False);

BRepTools::Clean(ShapeFused);

TCollection_AsciiString test;
if (!BRepTools::Triangulation(ShapeFused,1)) {
	test = ("In fact the triangulation has been removed\n");
}

Fit();
 TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60);	\n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80);	\n\
TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);	\n\
BRepMesh::Mesh(ShapeFused,1);	\n\
	\n\
BRepTools::Clean(ShapeFused);	\n\
	\n\
if (!BRepTools::Triangulation(ShapeFused,1)) {	\n\
	TCollection_AsciiString test(<In fact the triangulation has been removed>);	\n\
}	\n\
	\n\
	--- Result ---\n");

	Message += test;
	Message +=("\
				  \n\
				\n");

	PocessTextInDialog("Remove the triangulation", Message);

}

/*******************************************************************************
*********************  T E S S E L A T E  **************************************
*******************************************************************************/

void CTriangulationDoc::Start()
{
  myPresentation->Init();
  OnBUTTONStart();
}

void CTriangulationDoc::OnFileNew()
{
  OnNewDocument();
  Start();
}

void CTriangulationDoc::InitViewButtons()
{
  //POSITION pos = GetFirstViewPosition();
/* LLS
  while (pos != NULL)
  {
    COCCDemoView* pView = (COCCDemoView*) GetNextView(pos);
    pView->InitButtons();
  }
*/	
}

void CTriangulationDoc::DoSample()
{
  InitViewButtons();

  HCURSOR hOldCursor = ::GetCursor();
  HCURSOR hNewCursor = AfxGetApp()->LoadStandardCursor(IDC_APPSTARTING);

  SetCursor(hNewCursor);
  {
    try
    {
      myPresentation->DoSample();
    }
    catch (Standard_Failure const& anException)
    {
      Standard_SStream aSStream;
      aSStream << "An exception was caught: " << anException << ends;
      CString aMsg = aSStream.str().c_str();
//      aSStream.rdbuf()->freeze(0);   // allow deletion of dynamic array
      AfxMessageBox (aMsg);
    }
  }
  SetCursor(hOldCursor);
}

void CTriangulationDoc::OnBUTTONStart() 
{
  myAISContext->EraseAll (Standard_True);
  myPresentation->FirstSample();
  DoSample();
}

void CTriangulationDoc::OnBUTTONEnd()
{
  myAISContext->EraseAll (Standard_True);
  myPresentation->LastSample();
  DoSample();
}

void CTriangulationDoc::OnBUTTONRepeat() 
{
  DoSample();
}

void CTriangulationDoc::OnBUTTONNext() 
{
  if (!myPresentation->AtLastSample())
  {
    myPresentation->NextSample();
    DoSample();
  }
}

void CTriangulationDoc::OnBUTTONPrev() 
{
  if (!myPresentation->AtFirstSample())
  {
    myPresentation->PrevSample();
    DoSample();
  }
}

void CTriangulationDoc::OnUpdateBUTTONNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!myPresentation->AtLastSample());
}

void CTriangulationDoc::OnUpdateBUTTONPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (!myPresentation->AtFirstSample());
}

void CTriangulationDoc::OnDumpView()
{
  for (POSITION aPos = GetFirstViewPosition(); aPos != NULL;)
  {
    OCC_3dView* pView = (OCC_3dView* )GetNextView (aPos);
    pView->UpdateWindow();
  }

  myViewer->InitActiveViews();
  Handle(V3d_View) aView = myViewer->ActiveView();
  ExportView (aView);
}

void CTriangulationDoc::Fit()
{
	CMDIFrameWnd *pFrame =  (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild =  (CMDIChildWnd *) pFrame->GetActiveFrame();
	OCC_3dView *pView = (OCC_3dView*)pChild->GetActiveView();
	pView->FitAll();
}
