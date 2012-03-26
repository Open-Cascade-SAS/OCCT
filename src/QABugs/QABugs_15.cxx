// Created on: 2002-04-09
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#include <QABugs.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <QABugs_PresentableObject.hxx>

Handle(QABugs_PresentableObject) theObject1=NULL;
Handle(QABugs_PresentableObject) theObject2=NULL;

static Standard_Integer BUC60720 (Draw_Interpretor& di,Standard_Integer argc,const char ** argv )
{
  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  if(argc != 2) {
    di << "Usage : " << argv[0] << " 0/1" << "\n";
  }

  if(atoi(argv[1]) == 0) {
    if ( theObject1.IsNull() )
      {
	theObject1 = new QABugs_PresentableObject();
	theObject1->SetDisplayMode(0);
	myAISContext->Display(theObject1);
      }
  } else if(atoi(argv[1]) == 1) {
    if ( theObject2.IsNull() )
      {
	theObject2 = new QABugs_PresentableObject();
	theObject2->SetDisplayMode(1);
	myAISContext->Display(theObject2);
      }
  } else {
    di << "Usage : " << argv[0] << " 0/1" << "\n";
    return -1;
  }
  return 0;
}

void QABugs::Commands_15(Draw_Interpretor& theCommands)
{
  char *group = "QABugs";
  theCommands.Add("BUC60720","BUC60720 0/1",__FILE__,BUC60720,group);
}
