// File:	QARoutelous.cxx
// Created:	Tue Apr  9 18:30:59 2002
// Author:	QA Admin
//		<qa@umnox.nnov.matra-dtv.fr>


#include <QARoutelous.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <QARoutelous_PresentableObject.hxx>

Handle(QARoutelous_PresentableObject) theObject1=NULL;
Handle(QARoutelous_PresentableObject) theObject2=NULL;

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
	theObject1 = new QARoutelous_PresentableObject();
	theObject1->SetDisplayMode(0);
	myAISContext->Display(theObject1);
      }
  } else if(atoi(argv[1]) == 1) {
    if ( theObject2.IsNull() )
      {
	theObject2 = new QARoutelous_PresentableObject();
	theObject2->SetDisplayMode(1);
	myAISContext->Display(theObject2);
      }
  } else {
    di << "Usage : " << argv[0] << " 0/1" << "\n";
    return -1;
  }
  return 0;
}

void QARoutelous::Commands(Draw_Interpretor& theCommands)
{
  char *group = "QARoutelous";
  theCommands.Add("BUC60720","BUC60720 0/1",__FILE__,BUC60720,group);
}
