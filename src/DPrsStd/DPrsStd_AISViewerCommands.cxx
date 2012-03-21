// Created on: 1998-10-27
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.




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
