// File:	DPrsStd_AISViewerCommnds.cxx
// Created:	Tue Oct 27 10:24:49 1998
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>



#include <DPrsStd.hxx>

#include <DPrsStd.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ColorKind.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TDF_Data.hxx> 
#include <TDF_Tool.hxx> 
#include <TDF_ChildIterator.hxx>
#include <DDF.hxx>
#include <DDocStd.hxx>
#include <TDocStd_Document.hxx> 
#include <ViewerTest.hxx>

#include <V3d_View.hxx>

// for AIS

#include <TPrsStd_AISPresentation.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <ViewerTest.hxx>
#include <AIS_InteractiveContext.hxx> 
#include <ViewerTest.hxx>
#include <ViewerTest_Tool.hxx>
#include <V3d_View.hxx>


  
extern int ViewerMainLoop (Standard_Integer, const char**);

//=======================================================================
//function : DPrsStd_AISInitViewer
//purpose  : AISInitViewer (DOC)
//=======================================================================
extern void ViewerTest_InitViewerTest (const Handle(AIS_InteractiveContext)&);

static Standard_Integer DPrsStd_AISInitViewer (Draw_Interpretor& di,
					     Standard_Integer nb, 
					     const char** arg) 
{   
  if (nb == 2) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1; 
    TDF_Label acces = D->GetData()->Root();   
    Handle(TPrsStd_AISViewer) viewer;
    if (!TPrsStd_AISViewer::Find (acces,viewer)) {
      TCollection_AsciiString title;  
      title.Prepend(arg[1]);   
      title.Prepend("_"); 
      title.Prepend("Document");  
      Handle(V3d_Viewer) vw = ViewerTest_Tool::MakeViewer (title.ToCString());
      viewer = TPrsStd_AISViewer::New (acces,vw);
    } 
    ViewerTest_Tool::InitViewerTest (viewer->GetInteractiveContext());
    DDF::ReturnLabel(di,viewer->Label());
    return 0;
  }
  di << "DPrsStd_AISInitViewer : Error" << "\n";
  return 1;
}


//=======================================================================
//function : TPrsStd_AISRepaint
//purpose  : 
//=======================================================================

static Standard_Integer DPrsStd_AISRepaint (Draw_Interpretor& di,
					    Standard_Integer nb, 
					    const char** arg) 
{   
  if (nb == 2) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) return 1; 
    TDF_Label acces = D->GetData()->Root(); 
    TPrsStd_AISViewer::Update(acces);
    return 0;
  }
  di << "DPrsStd_AISRepaint : Error" << "\n";
  return 1; 
}

//=======================================================================
//function : AISViewerCommands
//purpose  :
//=======================================================================


void DPrsStd::AISViewerCommands (Draw_Interpretor& theCommands)
{  

  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "DPrsStd : standard presentation commands" ;  
 
  // standard commands working on AISViewer
  
  theCommands.Add ("AISInitViewer", 
                   "AISInitViewer (DOC)",
		   __FILE__, DPrsStd_AISInitViewer, g);    

  theCommands.Add ("AISRepaint", 
                   "update the AIS viewer",
		   __FILE__, DPrsStd_AISRepaint, g);      
}
