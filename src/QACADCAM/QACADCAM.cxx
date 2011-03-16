// File:	QACADCAM.cxx
// Created:	Tue Mar 19 17:44:15 2002
// Author:	QA Admin
//		<qa@umnox.nnov.matra-dtv.fr>


#include <QACADCAM.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <BRepPrimAPI_MakeSphere.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <AIS_Drawer.hxx>
#include <Prs3d_ShadingAspect.hxx>

//#include <AcisData_AcisModel.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <tcl.h>

static Standard_Integer BUC60738 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv)
{
  
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  
  TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(-40,0,0),20).Shape();
  Handle_AIS_Shape theAISShape = new AIS_Shape(theSphere);

  //display mode = Shading
  theAISShape->SetDisplayMode(1);
	
  //get the drawer
  Handle_AIS_Drawer theDrawer = theAISShape->Attributes();
  Handle_Prs3d_ShadingAspect theShadingAspect = theDrawer->ShadingAspect();
  Handle_Graphic3d_AspectFillArea3d theAspectFillArea3d = theShadingAspect->Aspect();
	
  //allow to display the edges
  theAspectFillArea3d->SetEdgeOn();
  //set the style to Dash

  //but the style is not set to dash : it is always solid
  theAspectFillArea3d->SetEdgeLineType (Aspect_TOL_DASH); 
  theAspectFillArea3d->SetEdgeColor(Quantity_Color(Quantity_NOC_GREEN)); 
  theAspectFillArea3d->SetInteriorStyle(Aspect_IS_EMPTY);
  theShadingAspect->SetAspect(theAspectFillArea3d);
  theDrawer->SetShadingAspect(theShadingAspect);
  theAISShape->SetAttributes(theDrawer);

  aContext->Display(theAISShape);  
  
  return 0;
}

static int BUC60606(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {

  di << "Use satread command instead of " << argv[0] << "\n";
  return -1;

  if(argc!=3) {
    di << "Usage : " << argv[0] << "name filename" << "\n";
    return -1;
  }
//   Handle_AcisData_AcisModel satModel = new AcisData_AcisModel(); 
//   Standard_Character *DD = Tcl_GetVar(di.Interp(),"Draw_DataDir",TCL_GLOBAL_ONLY);

//   Standard_Character  *filename = new Standard_Character [strlen(DD)+strlen(argv[2])+1];
//   sprintf(filename,"%s/%s",DD,argv[2]);

//   satModel->ReadSaveFile(filename);
  
//   Standard_Boolean success = !satModel.IsNull(); 
//   cout << "n<-- SAT model read : " << (success?"true":"false") << endl << flush; 
//   if ( success ) {
//     Standard_Character *Ch=new Standard_Character [strlen(argv[1])+3];
//     Handle_TopTools_HSequenceOfShape satShapes = satModel->GetShapes();
//     Standard_Integer nbs=satShapes->Length();
//     for(Standard_Integer i=1;i<=nbs;i++) {
//       TopoDS_Shape xShape = satShapes->Value(i);
//       if(!xShape.IsNull()) {
// 	sprintf(Ch,"%s_%i",argv[1],i);
// 	DBRep::Set(Ch,xShape);
//       } else cout << "The shape " << argv[1] << "_" << i << " is NULL !" << endl;
//     }
//   }
  

  return 0;
}

static int BUC60627(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  di << "Use satread command instead of " << argv[0] << "\n";
  return -1;

  // perform the conversion
  // as in AcisData.cdl --- that's alright
//   if(argc!=3) {
//     cerr << "Usage : " << argv[0] << " BREP file" << endl;
//     return -1;
//   }
//   TopoDS_Shape shapeModel=DBRep::Get(argv[1]);
//   Handle(AcisData_AcisModel) satModel = new AcisData_AcisModel();
//   satModel->AddShape( shapeModel ); // memorizes "kegel2.brep"
//   Standard_CString stringData;
//   Standard_Integer nbChar; // write intermediate stringin order to compute ACIS data
//   satModel->WriteStringData( stringData, nbChar );
//   if ( satModel->IsDone() ) {
//     satModel = new AcisData_AcisModel();
//     satModel->ReadStringData( stringData );
//     if ( satModel->IsDone() ) {
//       // write into file
//       satModel->WriteSaveFile( argv[2] );
//       if ( ! satModel->IsDone() ) {
// 	cout << "The SAT madel does not wroute."   << endl;
//       }
//     } else cout << "The SAT model does not read from a string."   << endl;
//     // satModel->ReadStringData( stringData );
//   } else cout << "The SAT model does not wroute into a string."   << endl;
//   // satModel->WriteStringData( stringData, nbChar );
  return 1;
}

void QACADCAM::Commands(Draw_Interpretor& theCommands) {
  char *group = "QACADCAM";

  theCommands.Add("BUC60738","BUC60738",__FILE__,BUC60738,group);
  theCommands.Add("BUC60606","BUC60606 name",__FILE__,BUC60606,group);
  theCommands.Add("BUC60627","BUC60627 name",__FILE__,BUC60627,group);
  
  return;
}
