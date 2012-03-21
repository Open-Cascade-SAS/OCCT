// Created on: 2002-01-22
// Created by: Julia DOROVSKIKH
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

#if defined(WOKC40)

#include <QAViewer2dTest.hxx>

#include <QAViewer2dTest_DoubleMapOfInteractiveAndName.hxx>
#include <QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>

#include <V2d_Viewer.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_HArray1OfShape.hxx>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Timer.hxx>

#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>

#include <AIS2D_ProjShape.hxx>
#include <AIS2D_InteractiveContext.hxx>

QAViewer2dTest_DoubleMapOfInteractiveAndName& GetMapOfAIS2D();
Handle(AIS2D_InteractiveContext)& TheAIS2DContext();

//=======================================================================
//function : GetTypeNames2d
//purpose  : 
//=======================================================================
static char** GetTypeNames2d ()
{
  static char* names[14] = {"Point","Axis","Trihedron","PlaneTrihedron", "Line","Circle","Plane",
			  "Shape","ConnectedShape","MultiConn.Shape",
			  "ConnectedInter.","MultiConn.",
			  "Constraint","Dimension"};
  static char** ThePointer = names;
  return ThePointer;
}

//=======================================================================
//function : GetTypeAndSignfromString2d
//purpose  : 
//=======================================================================
/*void GetTypeAndSignfromString2d (char* name, AIS_KindOfInteractive& TheType,
                                 Standard_Integer& TheSign)
{
  char ** thefullnames = GetTypeNames2d();
  Standard_Integer index(-1);

  for (Standard_Integer i = 0; i <= 13 && index == -1; i++)
    if (!strcasecmp(name, thefullnames[i]))
      index = i;
  
  if (index == -1)
  {
    TheType = AIS_KOI_None;
    TheSign = -1;
    return;
  }
  
  if (index <= 6)
  {
    TheType = AIS_KOI_Datum;
    TheSign = index+1;
  }
  else if (index <= 9)
  {
    TheType = AIS_KOI_Shape;
    TheSign = index-7;
  }
  else if (index <= 11)
  {
    TheType = AIS_KOI_Object;
    TheSign = index-10;
  }
  else
  {
    TheType = AIS_KOI_Relation;
    TheSign = index-12;
  }
}*/

//=======================================================================
//function : Get Context and active view..
//purpose  : 
//=======================================================================
void GetCtxAndView2d (Handle(AIS2D_InteractiveContext)& Ctx,
                      Handle(V2d_View)& Viou)
{
  Ctx = QAViewer2dTest::GetAIS2DContext();
  const Handle(V2d_Viewer)& Vwr = Ctx->CurrentViewer();
  Vwr->InitActiveViews();
  if (Vwr->MoreActiveViews())
    Viou = Vwr->ActiveView();
}

//==============================================================================
//function : GetShapeFromName2d
//purpose  : Compute an Shape from a draw variable or a file name
//==============================================================================
TopoDS_Shape GetShapeFromName2d (char* name)
{
  TopoDS_Shape S = DBRep::Get(name);

  if (S.IsNull())
  {
    BRep_Builder aBuilder;
    BRepTools::Read( S, name, aBuilder);
  }

  return S; 
}
/*
//==============================================================================
//function : V2dDispAreas
//purpose  : Redraw the view
//Draw arg : No args
//==============================================================================
static int V2dDispAreas (Draw_Interpretor& ,Standard_Integer , char** )
{
  Handle(AIS2D_InteractiveContext) Ctx;
  Handle(V2d_View) Viou;
  GetCtxAndView2d(Ctx,Viou);
//  Ctx->DisplayActiveAreas(Viou);
  return 0;
}

//==============================================================================
//function : V2dClearAreas
//purpose  : Redraw the view
//Draw arg : No args
//==============================================================================
static int V2dClearAreas (Draw_Interpretor& ,Standard_Integer , char** )
{
  Handle(AIS2D_InteractiveContext) Ctx;
  Handle(V2d_View) Viou;
  GetCtxAndView2d(Ctx,Viou);
//  Ctx->ClearActiveAreas(Viou);
  return 0;
}

//==============================================================================
//function : V2dDispSensi
//purpose  : 
//Draw arg : No args
//==============================================================================
static int V2dDispSensi (Draw_Interpretor& ,Standard_Integer , char** )
{
  Handle(AIS2D_InteractiveContext) Ctx;
  Handle(V2d_View) Viou;
  GetCtxAndView2d(Ctx,Viou);
//  Ctx->DisplayActiveSensitive(Viou);
  return 0;

}

//==============================================================================
//function : V2dClearSensi
//purpose  : 
//Draw arg : No args
//==============================================================================
static int V2dClearSensi (Draw_Interpretor& ,Standard_Integer , char** )
{
  Handle(AIS2D_InteractiveContext) Ctx;
  Handle(V2d_View) Viou;
  GetCtxAndView2d(Ctx,Viou);
//  Ctx->ClearActiveSensitive(Viou);
  return 0;
}
*/
//==============================================================================
//function : V2dDebug
//purpose  : To list the displayed object with their attributes
//Draw arg : No args
//==============================================================================
static int V2dDebug (Draw_Interpretor& di, Standard_Integer , char** )
{
  if (!QAViewer2dTest::CurrentView().IsNull())
  {
    di << "List of object in the viewer :" << "\n";

    QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
  
    while (it.More())
    {
      di << "\t" << it.Key2() << "\n";
      it.Next();
    }
  }

  return 0;
}

//=======================================================================
//function :V2dSubInt
//purpose  : 
//=======================================================================
/*static int V2dSubInt (Draw_Interpretor& , Standard_Integer argc, char** argv)
{
  if (argc == 1) return 1;
  Standard_Integer On = atoi(argv[1]);
  const Handle(AIS2D_InteractiveContext)& Ctx = QAViewer2dTest::GetAIS2DContext();
  
  if (argc == 2)
  {
    if (!Ctx->HasOpenedContext())
    {
      cout << "sub intensite ";
      if (On == 1) cout << "On";
      else cout << "Off";
//      cout<<"pour "<<Ctx->NbCurrents()<<"  objets"<<endl;
      for (Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent())
      {
	if(On==1){
	  Ctx->SubIntensityOn(Ctx->Current(),Standard_False);}
	else{
	  cout <<"passage dans off"<<endl;
	  Ctx->SubIntensityOff(Ctx->Current(),Standard_False);
	}
      }
    }
    else
    {
      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected()){
	if(On==1){
	  Ctx->SubIntensityOn(Ctx->Interactive(),Standard_False);}
	else{
	  Ctx->SubIntensityOff(Ctx->Interactive(),Standard_False);}
      }
    }
    Ctx->UpdateCurrentViewer();
  }
  else
  {
    Handle(AIS2D_InteractiveObject) IO;
    TCollection_AsciiString name = argv[2];
    if (GetMapOfAIS2D().IsBound2(name))
    {
      IO = GetMapOfAIS2D().Find2(name);
      if (On == 1) Ctx->SubIntensityOn(IO);
      else         Ctx->SubIntensityOff(IO);
    }
    else return 1;
  }
  return 0;
}*/

//=======================================================================
//function : v2doffsetvlad
//purpose  : check BUG PRO16483
//=======================================================================
Standard_Integer v2doffsetvlad (Draw_Interpretor& , Standard_Integer , char** a)
{
  TopTools_ListOfShape ClosingFaces;

  TopoDS_Shape S = GetShapeFromName2d(a[1]);
  ClosingFaces.Append(GetShapeFromName2d(a[2]));
  Standard_Real offset = -Abs(atof(a[3]));

  TopoDS_Shape Result = BRepOffsetAPI_MakeThickSolid (S, ClosingFaces, offset, 1.e-05, 
                                                      BRepOffset_Skin, Standard_True,
                                                      Standard_False, GeomAbs_Arc);

  Handle(AIS2D_InteractiveContext) Ctx = QAViewer2dTest::GetAIS2DContext();
  Handle(AIS2D_ProjShape) ais = new AIS2D_ProjShape();
  ais->Add(Result);
  Ctx->Display(ais);

//  DBRep::Set("vlad", ais->Shape());

  return 0;
}

//=======================================================================
//function : v2doffsetvlad2
//purpose  : check BUG PRO16483
//=======================================================================
Standard_Integer v2doffsetvlad2 (Draw_Interpretor& , Standard_Integer , char** a)
{
  TopTools_ListOfShape ClosingFaces;

  TopoDS_Shape S = GetShapeFromName2d(a[1]);
  ClosingFaces.Append(GetShapeFromName2d(a[2]));
  ClosingFaces.Append(GetShapeFromName2d(a[3]));
  Standard_Real offset = -Abs(atof(a[4]));

  TopoDS_Shape Result = BRepOffsetAPI_MakeThickSolid (S, ClosingFaces, offset, 1.e-05, 
                                                      BRepOffset_Skin, Standard_True,
                                                      Standard_False, GeomAbs_Arc);

  Handle(AIS2D_InteractiveContext) Ctx = QAViewer2dTest::GetAIS2DContext();
  Handle(AIS2D_ProjShape) ais = new AIS2D_ProjShape();
  ais->Add(Result);
  Ctx->Display(ais);

//  DBRep::Set("vlad", ais->Shape());

  return 0;
}

//=======================================================================
//function : v2doffsetvlad3
//purpose  : check BUG PRO16483
//=======================================================================
Standard_Integer v2doffsetvlad3 (Draw_Interpretor& , Standard_Integer , char** a)
{
  TopTools_ListOfShape ClosingFaces;

  TopoDS_Shape S = GetShapeFromName2d(a[1]);
  ClosingFaces.Append(GetShapeFromName2d(a[2]));
  ClosingFaces.Append(GetShapeFromName2d(a[3]));
  ClosingFaces.Append(GetShapeFromName2d(a[4]));
  Standard_Real offset = -Abs(atof(a[5]));

  TopoDS_Shape Result = BRepOffsetAPI_MakeThickSolid (S, ClosingFaces, offset, 1.e-05, 
                                                      BRepOffset_Skin, Standard_True,
                                                      Standard_False, GeomAbs_Arc);

  Handle(AIS2D_InteractiveContext) Ctx = QAViewer2dTest::GetAIS2DContext();
  Handle(AIS2D_ProjShape) ais = new AIS2D_ProjShape();
  ais->Add(Result);
  Ctx->Display(ais);

//  DBRep::Set("vlad", ais->Shape());

  return 0;
}

//=======================================================================
//function : v2dvlad
//purpose  : 
//=======================================================================
Standard_Integer v2dvlad (Draw_Interpretor& , Standard_Integer , char** a)
{
  ifstream s(a[1]);
  BRep_Builder builder;
  TopoDS_Shape shape;
  BRepTools::Read(shape, s, builder);
  DBRep::Set(a[1], shape);
  Handle(AIS2D_InteractiveContext) Ctx = QAViewer2dTest::GetAIS2DContext();
  Handle(AIS2D_ProjShape) ais = new AIS2D_ProjShape();
  ais->Add(shape);
  Ctx->Display(ais);

  return 0;
}

//==============================================================================
//function : GetTypeNameFromShape2d
//purpose  : get the shape type as a string from a shape
//==============================================================================
static char *GetTypeNameFromShape2d (const TopoDS_Shape& aShape)
{
  char *ret = "????";

  if (aShape.IsNull()) ret = "Null Shape";

  switch (aShape.ShapeType())
  {
  case TopAbs_COMPOUND  : ret = "COMPOUND" ; break;
  case TopAbs_COMPSOLID : ret = "COMPSOLID" ; break;
  case TopAbs_SOLID     : ret = "SOLID" ; break;
  case TopAbs_SHELL     : ret = "SHELL" ; break;
  case TopAbs_FACE      : ret = "FACE" ; break;
  case TopAbs_WIRE      : ret = "WIRE" ; break;
  case TopAbs_EDGE      : ret = "EDGE" ; break;
  case TopAbs_VERTEX    : ret = "VERTEX" ; break;
  case TopAbs_SHAPE     : ret = "SHAPE" ; break;
  }
  return ret;
}

//==============================================================================
//function : GetEnvir2d
//purpose  : 
//==============================================================================
/*static TCollection_AsciiString GetEnvir2d ()
{
  static Standard_Boolean IsDefined = Standard_False ;
  static TCollection_AsciiString VarName;
  if (!IsDefined)
  {
    char *envir, *casroot ;
    envir = getenv("CSF_MDTVTexturesDirectory") ;
    
    Standard_Boolean HasDefinition = Standard_False ;
    if (!envir)
    { 
      casroot = getenv("CASROOT");
      if (casroot)
      {
	VarName = TCollection_AsciiString  (casroot);
	VarName += "/src/Textures";
	HasDefinition = Standard_True;
      }
    }
    else
    {
      VarName = TCollection_AsciiString(envir);
      HasDefinition = Standard_True;
    }

    if (HasDefinition)
    {
      OSD_Path aPath (VarName);
      OSD_Directory aDir (aPath);
      if (aDir.Exists())
      {
	TCollection_AsciiString aTexture = VarName + "/2d_MatraDatavision.rgb";
	OSD_File TextureFile (aTexture);
	if (!TextureFile.Exists())
        {
	  cout << " CSF_MDTVTexturesDirectory or CASROOT not correctly setted " << endl;
	  cout << " not all files are found in : "<<VarName.ToCString() << endl;
	  Standard_Failure::Raise("CSF_MDTVTexturesDirectory or CASROOT not correctly setted ");
	}
      }
      else
      {
	cout << " CSF_MDTVTexturesDirectory or CASROOT not correctly setted " << endl;
	cout << " Directory : "<< VarName.ToCString() << " not exist " << endl;
	Standard_Failure::Raise("CSF_MDTVTexturesDirectory or CASROOT not correctly setted ");
      }      
      return VarName ;
    }
    else
    {
      cout << " CSF_MDTVTexturesDirectory and CASROOT not setted " << endl;
      cout << " one of these variable are mandatory to use this fonctionnality" << endl;
      Standard_Failure::Raise("CSF_MDTVTexturesDirectory and CASROOT not setted ");
    }   
    IsDefined = Standard_True ; 
  }

  return VarName ;
}*/

//==============================================================================
//function : V2dPerf
//purpose  : Test the annimation of an object along a 
//           predefined traectory
//Draw arg : vperf ShapeName 1/0(Transfo/Location) 1/0(Primitives sensibles ON/OFF) 
//==============================================================================
/*static int V2dPerf (Draw_Interpretor& , Standard_Integer , char** argv)
{
  OSD_Timer myTimer;
  TheAIS2DContext()->CloseLocalContext();
  
  Standard_Real Step = 4*M_PI/180;
  Standard_Real Angle = 0;
  
  Handle(AIS2D_InteractiveObject) aIO;
  aIO = GetMapOfAIS2D().Find2(argv[1]);
  Handle(AIS2D_ProjShape) aShape = Handle(AIS2D_ProjShape)::DownCast(aIO);
  
  myTimer.Start();
  
  if (atoi(argv[3]) == 1)
  {
    cout << " Primitives sensibles OFF" << endl;
//    TheAIS2DContext()->Deactivate(aIO);
  }
  else
  {
    cout << " Primitives sensibles ON" << endl;
  }
  // Movement par transformation 
  if (atoi(argv[2]) == 1)
  { 
    cout << " Calcul par Transformation" << endl;
    for (Standard_Real myAngle = 0; Angle < 10*2*M_PI; myAngle++)
    {
      Angle = Step*myAngle;
      gp_Trsf myTransfo;
      myTransfo.SetRotation(gp_Ax1(gp_Pnt(0,0,0), gp_Dir(0,0,1)), Angle);
//      TheAIS2DContext()->SetLocation(aShape,myTransfo);
      TheAIS2DContext()->UpdateCurrentViewer();
    }
  }
  else
  {
    cout << " Calcul par Locations" << endl;
    gp_Trsf myAngleTrsf;
    myAngleTrsf.SetRotation(gp_Ax1(gp_Pnt(0,0,0), gp_Dir(0,0,1)), Step);
    TopLoc_Location myDeltaAngle (myAngleTrsf);
    TopLoc_Location myTrueLoc;
    
    for (Standard_Real myAngle = 0; Angle < 10*2*M_PI; myAngle++)
    {
      Angle = Step*myAngle;
      myTrueLoc = myTrueLoc*myDeltaAngle;
//      TheAIS2DContext()->SetLocation(aShape, myTrueLoc);
      TheAIS2DContext()->UpdateCurrentViewer();
    }
  }
  if (atoi(argv[3]) == 1)
  {
    // On reactive la selection des primitives sensibles 
//    TheAIS2DContext()->Activate(aIO,0);
  }
//  QAViewer2dTest::CurrentView()->Redraw();
  myTimer.Stop();
  cout << " Temps ecoule " << endl;
  myTimer.Show();
  return 0;
}*/

//==================================================================================
// Function : V2dAnimation
//==================================================================================
/*static int V2dAnimation (Draw_Interpretor& , Standard_Integer , char** )
{ 
  Standard_Real thread = 4;
  Standard_Real angleA = 0;
  Standard_Real angleB;
  Standard_Real X;
  gp_Ax1 Ax1 (gp_Pnt(0,0,0), gp_Vec(0,0,1));
  
  BRep_Builder B;
  TopoDS_Shape CrankArm;
  TopoDS_Shape CylinderHead;
  TopoDS_Shape Propeller;
  TopoDS_Shape EngineBlock;
  
  BRepTools::Read(CrankArm,"/dp_26/Indus/ege/assemblage/CrankArm.rle",B);
  BRepTools::Read(CylinderHead,"/dp_26/Indus/ege/assemblage/CylinderHead.rle",B);
  BRepTools::Read(Propeller,"/dp_26/Indus/ege/assemblage/Propeller.rle",B);
  BRepTools::Read(EngineBlock,"/dp_26/Indus/ege/assemblage/EngineBlock.rle",B);
  
  if (CrankArm.IsNull() || CylinderHead.IsNull() || Propeller.IsNull() || EngineBlock.IsNull())
  {
    cout << " Syntaxe error:loading failure." << endl;
  }

  OSD_Timer myTimer;
  myTimer.Start();
  
  Handle(AIS2D_ProjShape) myAisCylinderHead = new AIS2D_ProjShape();
  Handle(AIS2D_ProjShape) myAisEngineBlock  = new AIS2D_ProjShape();
  Handle(AIS2D_ProjShape) myAisCrankArm     = new AIS2D_ProjShape();
  Handle(AIS2D_ProjShape) myAisPropeller    = new AIS2D_ProjShape();

  myAisCylinderHead->Add(CylinderHead);
  myAisCylinderHead->Add(EngineBlock);
  myAisCylinderHead->Add(CrankArm);
  myAisCylinderHead->Add(Propeller);
  
  GetMapOfAIS2D().Bind(myAisCylinderHead,"a");
  GetMapOfAIS2D().Bind(myAisEngineBlock,"b");
  GetMapOfAIS2D().Bind(myAisCrankArm,"c");
  GetMapOfAIS2D().Bind(myAisPropeller,"d");
  
//  TheAIS2DContext()->SetColor(myAisCylinderHead, Quantity_NOC_INDIANRED);
//  TheAIS2DContext()->SetColor(myAisEngineBlock , Quantity_NOC_RED);
//  TheAIS2DContext()->SetColor(myAisPropeller   , Quantity_NOC_GREEN);
 
  TheAIS2DContext()->Display(myAisCylinderHead,Standard_False);
  TheAIS2DContext()->Display(myAisEngineBlock,Standard_False );
  TheAIS2DContext()->Display(myAisCrankArm,Standard_False    );
  TheAIS2DContext()->Display(myAisPropeller,Standard_False);
  
//  TheAIS2DContext()->Deactivate(myAisCylinderHead);
//  TheAIS2DContext()->Deactivate(myAisEngineBlock );
//  TheAIS2DContext()->Deactivate(myAisCrankArm    );
//  TheAIS2DContext()->Deactivate(myAisPropeller   );
  
  // Boucle de mouvement
  for (Standard_Real myAngle = 0; angleA < 2*M_PI*10.175; myAngle++)
  {
    angleA = thread*myAngle*M_PI/180;
    X = Sin(angleA)*3/8;
    angleB = atan(X / Sqrt(-X * X + 1));
//    Standard_Real decal(25*0.6);
    
    //Build a transformation on the display
    gp_Trsf aPropellerTrsf;
    aPropellerTrsf.SetRotation(Ax1,angleA);
//    TheAIS2DContext()->SetLocation(myAisPropeller,aPropellerTrsf);
    
//    gp_Ax3 base(gp_Pnt(3*decal*(1-Cos(angleA)),-3*decal*Sin(angleA),0),gp_Vec(0,0,1),gp_Vec(1,0,0));
//    gp_Trsf aCrankArmTrsf;
//    aCrankArmTrsf.SetTransformation(   base.Rotated(gp_Ax1(gp_Pnt(3*decal,0,0),gp_Dir(0,0,1)),angleB));
//    TheAIS2DContext()->SetLocation(myAisCrankArm,aCrankArmTrsf);
    
    TheAIS2DContext()->UpdateCurrentViewer();
  }
  
  TopoDS_Shape myNewCrankArm;  //=myAisCrankArm ->Shape().Located(myAisCrankArm ->Location());
  TopoDS_Shape myNewPropeller; //=myAisPropeller->Shape().Located(myAisPropeller->Location());
  
//  myAisCrankArm ->ResetLocation();
//  myAisPropeller->ResetLocation();
 
//  myAisCrankArm ->Set(myNewCrankArm );
//  myAisPropeller->Set(myNewPropeller);
  
//  TheAIS2DContext()->Activate(myAisCylinderHead,0);
//  TheAIS2DContext()->Activate(myAisEngineBlock,0 );
//  TheAIS2DContext()->Activate(myAisCrankArm ,0   );
//  TheAIS2DContext()->Activate(myAisPropeller ,0  );
  
  myTimer.Stop();
  myTimer.Show();
  myTimer.Start();
  
  TheAIS2DContext()->Redisplay(myAisCrankArm ,Standard_False);
  TheAIS2DContext()->Redisplay(myAisPropeller,Standard_False);
  
  TheAIS2DContext()->UpdateCurrentViewer();
//  QAViewer2dTest::CurrentView()->Redraw();
  
  myTimer.Stop();
  myTimer.Show();
  
  return 0;
}*/

//==============================================================================
//function : HaveMode2d
//use      : V2dActivatedModes
//==============================================================================
/*Standard_Boolean HaveMode2d (const Handle(AIS2D_InteractiveObject)& TheAisIO, const Standard_Integer mode)
{
//  TColStd_ListOfInteger List;
//  TheAIS2DContext()->ActivatedModes (TheAisIO,List);
//  TColStd_ListIteratorOfListOfInteger it;
  Standard_Boolean Found=Standard_False;
//  for (it.Initialize(List); it.More()&&!Found; it.Next() ){
//    if (it.Value()==mode ) Found=Standard_True;
//  }
  return Found; 
}*/

//==============================================================================
//function : V2dActivatedMode
//purpose  : permet d'attribuer a chacune des shapes un mode d'activation
//           (edges,vertex...)qui lui est propre et le mode de selection standard.
//           La fonction s'applique aux shapes selectionnees(current ou selected dans le viewer)
//             Dans le cas ou on veut psser la shape en argument, la fonction n'autorise
//           qu'un nom et qu'un mode.
//Draw arg : vsetam  [ShapeName] mode(0,1,2,3,4,5,6,7)
//==============================================================================
//#include <AIS2D_ListIteratorOfListOfInteractive.hxx>
/*
static int V2dActivatedMode (Draw_Interpretor& ,Standard_Integer argc,char** argv)
{
  Standard_Boolean HaveToSet;
  Standard_Boolean ThereIsName = Standard_False ;
  
  if (!QAViewer2dTest::CurrentView().IsNull())
  {
    if (!strcasecmp(argv[0],"vsetam")) HaveToSet = Standard_True;
    else HaveToSet = Standard_False;
    
    // verification des arguments 
    if (HaveToSet) {
      if (argc<2||argc>3) { cout<<" Syntaxe error"<<endl;return 1;}
      if (argc==3) ThereIsName=Standard_True;
      else ThereIsName=Standard_False;
    }
    else {
      // vunsetam
      if (argc>1) {cout<<" Syntaxe error"<<endl;return 1;}
      else {
	cout<<" R.A.Z de tous les modes de selecion"<<endl;
	cout<<" Fermeture du Context local"<<endl;
	TheAIS2DContext()->CloseLocalContext();
      }
      
    }
    
    
    // IL n'y a aps de nom de shape passe en argument
    if (HaveToSet && !ThereIsName){
      Standard_Integer aMode=atoi(argv [1]);
      
      char *cmode="???";
      
      switch (aMode) {
      case 0: cmode = "Shape"; break;
      case 1: cmode = "Vertex"; break;
      case 2: cmode = "Edge"; break;
      case 3: cmode = "Wire"; break;
      case 4: cmode = "Face"; break;
      case 5: cmode = "Shell"; break;
      case 6: cmode = "Solid"; break;
      case 7: cmode = "Compound"; break;
      }
      
      if( !TheAIS2DContext()->HasOpenedContext() ) {
	// il n'y a pas de Context local d'ouvert 
	// on en ouvre un et on charge toutes les shapes displayees
	// on load tous les objets displayees et on Activate les objets de la liste
	AIS2D_ListOfInteractive ListOfIO;
	// on sauve dans une AIS2DListOfInteractive tous les objets currents
	if (TheAIS2DContext()->NbCurrents()>0 ){
	  TheAIS2DContext()->UnhilightCurrents(Standard_False);
	  
	  for (TheAIS2DContext()->InitCurrent(); TheAIS2DContext()->MoreCurrent(); TheAIS2DContext()->NextCurrent() ){
	    ListOfIO.Append(TheAIS2DContext()->Current() );
	    
	  }
	}
	
	TheAIS2DContext()->OpenLocalContext(Standard_False);
	QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
	while(it.More()){
	  Handle(AIS2D_InteractiveObject) aIO=it.Key1();
	  TheAIS2DContext()->Load(aIO,0,Standard_False);
	  it.Next();
	}
	// traitement des objets qui etaient currents dans le Contexte global
	if (!ListOfIO.IsEmpty() ) {
	  // il y avait des objets currents
	  AIS2D_ListIteratorOfListOfInteractive iter;
	  for (iter.Initialize(ListOfIO); iter.More() ; iter.Next() ) {
	    Handle(AIS2D_InteractiveObject) aIO=iter.Value();
	    TheAIS2DContext()->Activate(aIO,aMode);
	    cout<<" Mode: "<<cmode<<" ON pour "<<GetMapOfAIS2D().Find1(aIO)  <<endl;
	  }
	}
	else {
	  // On applique le mode a tous les objets displayes
	  QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
	  while(it.More()){
	    Handle(AIS2D_InteractiveObject) aIO=it.Key1();
	    cout<<" Mode: "<<cmode<<" ON pour "<<it.Key2() <<endl;
	    TheAIS2DContext()->Activate(aIO,aMode);
	    it.Next();
	  }
	}
	
      }
      
      else {
	// un Context local est deja ouvert
	// Traitement des objets du Context local
	if (TheAIS2DContext()->NbSelected()>0 ){
	  TheAIS2DContext()->UnhilightSelected(Standard_False);
	  // il y a des objets selected,on les parcourt
	  for (TheAIS2DContext()->InitSelected(); TheAIS2DContext()->MoreSelected(); TheAIS2DContext()->NextSelected() ){
	    Handle(AIS2D_InteractiveObject) aIO=TheAIS2DContext()->Interactive();
	    
	    
	    if (HaveMode2d(aIO,aMode) ) {
	      cout<<" Mode: "<<cmode<<" OFF pour "<<GetMapOfAIS2D().Find1(aIO) <<endl;
	      TheAIS2DContext()->Deactivate(aIO,aMode);
	    }
	    else{
	      cout<<" Mode: "<<cmode<<" ON pour "<<GetMapOfAIS2D().Find1(aIO) <<endl;
	      TheAIS2DContext()->Activate(aIO,aMode);
	    }
	    
	  }
	}
	else{
	  // il n'y a pas d'objets selected
	  // tous les objets diplayes sont traites
	  QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
	  while(it.More()){
	    Handle(AIS2D_InteractiveObject) aIO=it.Key1();
	    if (HaveMode2d(aIO,aMode) ) {
	    cout<<" Mode: "<<cmode<<" OFF pour "<<GetMapOfAIS2D().Find1(aIO) <<endl;
	    TheAIS2DContext()->Deactivate(aIO,aMode);
	  }
	    else{
	    cout<<" Mode: "<<cmode<<" ON pour"<<GetMapOfAIS2D().Find1(aIO) <<endl;
	    TheAIS2DContext()->Activate(aIO,aMode);
	  }
	    it.Next();
	}
	  
	} 
      }
    }
    else if (HaveToSet && ThereIsName){
      Standard_Integer aMode=atoi(argv [2]);
      Handle(AIS2D_InteractiveObject) aIO=GetMapOfAIS2D().Find2(argv[1]);
	
      char *cmode="???";
      
      switch (aMode) {
      case 0: cmode = "Shape"; break;
      case 1: cmode = "Vertex"; break;
      case 2: cmode = "Edge"; break;
      case 3: cmode = "Wire"; break;
      case 4: cmode = "Face"; break;
      case 5: cmode = "Shell"; break;
      case 6: cmode = "Solid"; break;
      case 7: cmode = "Compound"; break;
      }
      
      if( !TheAIS2DContext()->HasOpenedContext() ) {
	TheAIS2DContext()->OpenLocalContext(Standard_False);
	// On charge tous les objets de la map
	QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
	while(it.More()){
	  Handle(AIS2D_InteractiveObject) aShape=it.Key1();
	  TheAIS2DContext()->Load(aShape,0,Standard_False);
	  it.Next();
	}
	TheAIS2DContext()->Activate(aIO,aMode);
	cout<<" Mode: "<<cmode<<" ON pour "<<argv[1]<<endl;
      }
      
      else {
	// un Context local est deja ouvert
	if (HaveMode2d(aIO,aMode) ) {
	  cout<<" Mode: "<<cmode<<" OFF pour "<<argv[1]<<endl;
	  TheAIS2DContext()->Deactivate(aIO,aMode);
	}
	else{
	  cout<<" Mode: "<<cmode<<" ON pour "<<argv[1]<<endl;
	  TheAIS2DContext()->Activate(aIO,aMode);
	}
      }
      
    }
  }
  return 0;
  
}
*/

//==============================================================================
//function : v2dtoto
//==============================================================================
#include <tcl.h>
/*static int V2dToto (Draw_Interpretor& , Standard_Integer argc, char** )
{
  cout << " Fonction toto" << endl;
  Tcl_Interp *interp;
  int code;
  if (argc > 1)
  {
    cout << " Syntaxe error" << endl;
    return 0;
  }
  interp = Tcl_CreateInterp();
  code = Tcl_EvalFile(interp,"/adv_12/DESIGN/k4dev/ege/work/test.tcl");
  //code= Tcl_Eval(interp,riri)
  cout << *interp->result << endl;
  return 0;
}*/

//==============================================================================
// function : WhoAmI
// user : vState
//==============================================================================
/*void WhoAmI (const Handle(AIS2D_InteractiveObject )& theShape ) {
  
  // AIS2D_Datum
  if (theShape->Type()==AIS2D_KOI_Datum) {
    if      (theShape->Signature()==3 ) { cout<<"  AIS2D_Trihedron"; }
    else if (theShape->Signature()==2 ) { cout<<"  AIS2D_Axis"; } 
    else if (theShape->Signature()==6 ) { cout<<"  AIS2D_Circle"; }
    else if (theShape->Signature()==5 ) { cout<<"  AIS2D_Line"; }
    else if (theShape->Signature()==7 ) { cout<<"  AIS2D_Plane"; }
    else if (theShape->Signature()==1 ) { cout<<"  AIS2D_Point"; }
    else if (theShape->Signature()==4 ) { cout<<"  AIS2D_PlaneTrihedron"; }
  }
  // AIS2D_ProjShape
  else if (theShape->Type()==AIS2D_KOI_Shape && theShape->Signature()==0 ) { cout<<"  AIS2D_ProjShape"; }
  // AIS2D_Dimentions et AIS2D_Relations
  else if (theShape->Type()==AIS2D_KOI_Relation) {
    Handle(AIS2D_Relation) TheShape= ((*(Handle(AIS2D_Relation)*)&theShape));
    
    if      (TheShape->KindOfDimension()==AIS2D_KOD_PLANEANGLE)      {cout<<"  AIS2D_AngleDimension";}
    else if (TheShape->KindOfDimension()==AIS2D_KOD_LENGTH )         {cout<<"  AIS2D_Chamf2/3dDimension/AIS2D_LengthDimension ";  }
    else if (TheShape->KindOfDimension()==AIS2D_KOD_DIAMETER  )      {cout<<"  AIS2D_DiameterDimension ";}
    else if (TheShape->KindOfDimension()==AIS2D_KOD_ELLIPSERADIUS  ) {cout<<"  AIS2D_EllipseRadiusDimension ";}
    //else if (TheShape->KindOfDimension()==AIS2D_KOD_FILLETRADIUS  )  {cout<<" AIS2D_FilletRadiusDimension "<<endl;}
    else if (TheShape->KindOfDimension()==AIS2D_KOD_OFFSET  )        {cout<<"  AIS2D_OffsetDimension ";}
    else if (TheShape->KindOfDimension()==AIS2D_KOD_RADIUS  )        {cout<<"  AIS2D_RadiusDimension ";}
    // AIS2D no repertorie.
    else {cout<<"  Type Unknown.";}
  }
}*/

//==============================================================================
//function : V2dState
//purpose  : 
//Draw arg : v2dstate [nameA] ... [nameN]
//==============================================================================
/*static int V2dState (Draw_Interpretor& , Standard_Integer argc, char** argv) 
{
  Standard_Boolean ThereIsCurrent = Standard_False;
  Standard_Boolean ThereIsArguments = Standard_False;
  TheAIS2DContext()->CloseAllContext();
  if (argc >= 2 )
  {
    ThereIsArguments = Standard_True;
  }
//  if (TheAIS2DContext()->NbCurrents()>0 ) {
//    ThereIsCurrent=Standard_True;
//  }

  if (ThereIsArguments)
  {
    for (int cpt = 1; cpt < argc; cpt++)
    {
      // Verification que lq piece est bien bindee.
      if (GetMapOfAIS2D().IsBound2(argv[cpt]))
      {
	Handle(AIS2D_InteractiveObject) theShape = GetMapOfAIS2D().Find2(argv[cpt]);
	cout << argv[cpt];
//        WhoAmI(theShape);
	if (TheAIS2DContext()->IsDisplayed(theShape))
        {
	  cout << "    Displayed" << endl;
	}
	else
        {
	  cout << "    Not Displayed" << endl;
	}
      }
      else
      {
	cout << "vstate error: Shape " << cpt << " doesn't exist;" << endl;
        return 1;
      }
    }
  }
  else if (ThereIsCurrent)
  {
    for (TheAIS2DContext() -> InitCurrent() ; TheAIS2DContext() -> MoreCurrent() ; TheAIS2DContext() ->NextCurrent() )
    {
      Handle(AIS2D_InteractiveObject) theShape=TheAIS2DContext()->Current();
      cout<<GetMapOfAIS2D().Find1(theShape);WhoAmI(theShape );
      if (TheAIS2DContext()->IsDisplayed(theShape) ) {
	cout<<"    Displayed"<<endl;
      }
      else {
	cout<<"    Not Displayed"<<endl;
      }
    }
  }
  else
  {
    QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
    while (it.More())
    {
      Handle(AIS2D_InteractiveObject) theShape = it.Key1();
      cout << it.Key2();
//      WhoAmI(theShape);
      if (TheAIS2DContext()->IsDisplayed(theShape))
      {
	cout << "    Displayed" << endl;
      }
      else
      {
	cout << "    Not Displayed" << endl;
      }
      it.Next();
    }
  }
  
  return 0;
}*/

//=======================================================================
//function : V2dPickShape
//purpose  : 
//=======================================================================
/*static int V2dPickShape (Draw_Interpretor& , Standard_Integer argc, char** argv) 
{
  TopoDS_Shape PickSh;
  TopAbs_ShapeEnum theType = TopAbs_COMPOUND;

  if (argc == 1) theType = TopAbs_SHAPE;
  else
  {
    if (!strcasecmp(argv[1],"V" )) theType = TopAbs_VERTEX;
    else if (!strcasecmp(argv[1],"E" )) theType = TopAbs_EDGE;
    else if (!strcasecmp(argv[1],"W" )) theType = TopAbs_WIRE;
    else if (!strcasecmp(argv[1],"F" )) theType = TopAbs_FACE;
    else if (!strcasecmp(argv[1],"SHAPE" )) theType = TopAbs_SHAPE;
    else if (!strcasecmp(argv[1],"SHELL" )) theType = TopAbs_SHELL;
    else if (!strcasecmp(argv[1],"SOLID" )) theType = TopAbs_SOLID;
  }
  
  static Standard_Integer nbOfSub[8] = {0,0,0,0,0,0,0,0};
  static TCollection_AsciiString nameType[8] = {"COMPS","SOL","SHE","F","W","E","V","SHAP"};

  TCollection_AsciiString name;

  Standard_Integer NbToPick = argc>2 ? argc-2 : 1;
  if (NbToPick == 1)
  {
    PickSh = QAViewer2dTest::PickShape(theType);
    
    if (PickSh.IsNull()) return 1;
    if (argc > 2)
    {
      name += argv[2];
    }
    else
    {
      if (!PickSh.IsNull())
      {
	nbOfSub[Standard_Integer(theType)]++;
	name += "Picked_";
	name += nameType[Standard_Integer(theType)];
	TCollection_AsciiString indxstring(nbOfSub[Standard_Integer(theType)]);
	name +="_";
	name+=indxstring;
      }
    }
    // si on avait une petite methode pour voir si la shape 
    // est deja dans la Double map, ca eviterait de creer....
    DBRep::Set(name.ToCString(),PickSh);
    
    Handle(AIS2D_ProjShape) newsh = new AIS2D_ProjShape();
    newsh->Add(PickSh);
    GetMapOfAIS2D().Bind(newsh, name);
    TheAIS2DContext()->Display(newsh);
    cout << "Nom de la shape pickee : " << name << endl;
  }

  // Plusieurs objets a picker, vite vite vite....
  //
  else
  {
    Standard_Boolean autonaming = !strcasecmp(argv[2],".");
    Handle(TopTools_HArray1OfShape) arr = new TopTools_HArray1OfShape(1,NbToPick);
    if (QAViewer2dTest::PickShapes(theType,arr))
    {
      for (Standard_Integer i = 1; i <= NbToPick; i++)
      {
	PickSh = arr->Value(i);
	if (!PickSh.IsNull())
        {
	  if (autonaming)
          {
	    nbOfSub[Standard_Integer(theType)]++;
	    name.Clear();
	    name += "Picked_";
	    name += nameType[Standard_Integer(theType)];
	    TCollection_AsciiString indxstring (nbOfSub[Standard_Integer(theType)]);
	    name +="_";
	    name+=indxstring;
	  }
	}
	else
	  name = argv[1+i];

	DBRep::Set(name.ToCString(),PickSh);
	Handle(AIS2D_ProjShape) newsh = new AIS2D_ProjShape();
        newsh->Add(PickSh);
	GetMapOfAIS2D().Bind(newsh, name);
	cout << "display of picke shape #" << i << " - nom : " << name << endl;
	TheAIS2DContext()->Display(newsh);
      }
    }
  }

  return 0;  
}*/

//=======================================================================
//function : V2dIOTypes
//purpose  : list of known objects
//=======================================================================
/*static int V2dIOTypes (Draw_Interpretor& , Standard_Integer , char** ) 
{
  //                             1234567890         12345678901234567         123456789
  TCollection_AsciiString Colum [3] = {"Standard Types","Type Of Object","Signature"};
  TCollection_AsciiString BlankLine(64, '_');
  Standard_Integer i;

  cout << "/n" << BlankLine << endl;

  for (i = 0; i <= 2; i++) Colum[i].Center(20,' ');
  for (i = 0; i <= 2; i++) cout << "|" << Colum[i];
  cout << "|" << endl;
  
  cout << BlankLine << endl;

  //  TCollection_AsciiString thetypes[5]={"Datum","Shape","Object","Relation","None"};
  char ** names = GetTypeNames2d();

  TCollection_AsciiString curstring;
  TCollection_AsciiString curcolum[3];
  
  // les objets de type Datum..
  curcolum[1] += "Datum";
  for (i = 0; i <= 6; i++)
  {
    curcolum[0].Clear();
    curcolum[0] += names[i];
    
    curcolum[2].Clear();
    curcolum[2]+=TCollection_AsciiString(i+1);
    
    for (Standard_Integer j = 0; j <= 2; j++)
    {
      curcolum[j].Center(20,' ');
      cout << "|" << curcolum[j];
    }
    cout << "|" << endl;
  }
  cout << BlankLine << endl;
  
  // les objets de type shape
  curcolum[1].Clear();
  curcolum[1] += "Shape";
  curcolum[1].Center(20,' ');

  for (i = 0; i <= 2; i++)
  {
    curcolum[0].Clear();
    curcolum[0] += names[7+i];
    curcolum[2].Clear();
    curcolum[2] += TCollection_AsciiString(i);

    for (Standard_Integer j = 0; j <= 2; j++)
    {
      curcolum[j].Center(20,' ');
      cout << "|" << curcolum[j];
    }
    cout << "|" << endl;
  }
  cout << BlankLine << endl;
  // les IO de type objet...
  curcolum[1].Clear();
  curcolum[1] += "Object";
  curcolum[1].Center(20,' ');
  for (i = 0;i <= 1; i++)
  {
    curcolum[0].Clear();
    curcolum[0] += names[10+i];
    curcolum[2].Clear();
    curcolum[2] += TCollection_AsciiString(i);
    
    for (Standard_Integer j = 0; j <= 2; j++)
    {
      curcolum[j].Center(20,' ');
      cout << "|" << curcolum[j];
    }
    cout << "|" << endl;
  }
  cout << BlankLine << endl;

  // les contraintes et dimensions.
  // pour l'instant on separe juste contraintes et dimensions...
  // plus tard, on detaillera toutes les sortes...
  curcolum[1].Clear();
  curcolum[1] += "Relation";
  curcolum[1].Center(20,' ');
  for (i = 0; i <= 1; i++)
  {
    curcolum[0].Clear();
    curcolum[0] += names[12+i];
    curcolum[2].Clear();
    curcolum[2] += TCollection_AsciiString(i);
    
    for (Standard_Integer j = 0; j <= 2; j++)
    {
      curcolum[j].Center(20,' ');
      cout << "|" << curcolum[j];
    }
    cout << "|" << endl;
  }
  cout << BlankLine << endl;

  return 0;  
}*/

//=======================================================================
//function : v2dr
//purpose  : reading of the shape
//=======================================================================
/*static Standard_Integer v2dr (Draw_Interpretor& , Standard_Integer , char** a)
{
  ifstream s (a[1]);
  BRep_Builder builder;
  TopoDS_Shape shape;
  BRepTools::Read(shape, s, builder);
  DBRep::Set(a[1], shape);
  Handle(AIS2D_InteractiveContext) Ctx = QAViewer2dTest::GetAIS2DContext();
  Handle(AIS2D_ProjShape) ais = new AIS2D_ProjShape();
  ais->Add(shape);
  Ctx->Display(ais);
  return 0;
}*/

//==============================================================================
//function : QAViewer2dTest::GeneralCommands
//purpose  : Add all the general commands in the Draw_Interpretor
//==============================================================================
void QAViewer2dTest::GeneralCommands (Draw_Interpretor& theCommands)
{
  char *group = "2D AIS Viewer";

  theCommands.Add("v2ddir",
		  "v2ddir - list interactive objects",
		  __FILE__, V2dDebug, group);

/*  theCommands.Add("v2dsub",
                  "v2dsub 0/1(off/on) [obj]     : Subintensity(on/off) of selected objects",
		  __FILE__, V2dSubInt, group);

  theCommands.Add("v2dardis",
		  "v2dardis                      : Display active areas",
		  __FILE__, V2dDispAreas, group);

  theCommands.Add("v2darera",
		  "v2darera                      : Erase active areas",
		  __FILE__, V2dClearAreas, group);

  theCommands.Add("v2dsensdis",
		  "v2dsensdis                    : display active entities",
		  __FILE__, V2dDispSensi, group);

  theCommands.Add("v2dsensera",
		  "v2dsensera                    : erase  active entities",
		  __FILE__, V2dClearSensi, group);

  theCommands.Add("v2dperf",
		  "v2dperf ShapeName 1/0(Transfo/Location) 1/0(Primitives sensibles ON/OFF)",
		  __FILE__, V2dPerf, group);

  theCommands.Add("v2danimation",
                  "v2danimation",
		  __FILE__, V2dAnimation, group);
*/
/*theCommands.Add("v2dtexscale",
		  "'v2dtexscale  NameOfShape ScaleU ScaleV' \n \
                   or 'vtexscale NameOfShape ScaleUV' \n \
                   or 'vtexscale NameOfShape' to disable scaling\n",
		  __FILE__,VTexture,group);

  theCommands.Add("v2dtexorigin",
		  "'v2dtexorigin NameOfShape UOrigin VOrigin' \n \
                   or 'vtexorigin NameOfShape UVOrigin' \n \
                   or 'vtexorigin NameOfShape' to disable origin positioning\n",
		  __FILE__,VTexture,group);

  theCommands.Add("v2dtexrepeat",
		  "'v2dtexrepeat  NameOfShape URepeat VRepeat' \n \
                   or 'vtexrepeat NameOfShape UVRepeat \n \
                   or 'vtexrepeat NameOfShape' to disable texture repeat \n ",
		  VTexture,group);

  theCommands.Add("v2dtexdefault",
		  "'v2dtexdefault NameOfShape' to set texture mapping default parameters \n",
		  VTexture,group);*/

//  theCommands.Add("v2dsetam",
//		  "v2dsetActivatedModes: vsetam mode(1->7)",
//		  __FILE__,VActivatedMode,group);

//  theCommands.Add("v2dunsetam",
//		  "v2dunsetActivatedModes:   vunsetam",
//		  __FILE__,VActivatedMode,group);

/*  theCommands.Add("v2dtoto",
                  "v2dtoto",
		  __FILE__, V2dToto, group);

  theCommands.Add("v2dstate",
                  "vstate [Name1] ... [NameN]    : No arg, select currents; no currrent select all",
		  __FILE__,V2dState,group);

  theCommands.Add("v2dpickshapes",
		  "v2dpickshape subtype(VERTEX,EDGE,WIRE,FACE,SHELL,SOLID) [name1 or .] [name2 or .] [name n or .]",
		  __FILE__, V2dPickShape, group);

  theCommands.Add("v2dtypes",
		  "v2dtypes                      : list of known types and signatures in AIS2D - To be Used in vpickobject command for selection with filters",
		  V2dIOTypes,group);

  theCommands.Add("v2dr",
                  "v2dr                          : reading of the shape",
		  __FILE__, v2dr, group);

  theCommands.Add("v2doffsetvlad",
		  "v2doffsetvlad shape offsettedface offsetvalue",
		  __FILE__, v2doffsetvlad, group);

  theCommands.Add("v2doffsetvlad2",
		  "v2doffsetvlad2 shape offsettedface1 offsettedface2 offsetvalue",
		  __FILE__, v2doffsetvlad2, group);

  theCommands.Add("v2doffsetvlad3",
		  "v2doffsetvlad3 shape offsettedface1 offsettedface2 offsettedface3 offsetvalue",
		  __FILE__, v2doffsetvlad3, group);

  theCommands.Add("v2dvlad",
		  "v2dvlad shape",
		  __FILE__, v2dvlad, group);*/
}
#endif
