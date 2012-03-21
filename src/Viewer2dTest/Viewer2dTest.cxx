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


#include <Viewer2dTest.hxx>
#include <Viewer2dTest_EventManager.hxx>
#include <Viewer2dTest_DoubleMapOfInteractiveAndName.hxx>
#include <Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>

#include <Draw_Interpretor.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_HArray1OfShape.hxx>

#include <TColStd_ListOfTransient.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_HArray1OfTransient.hxx>

#include <TCollection_AsciiString.hxx>

#include <AIS2D_ProjShape.hxx>
#include <AIS2D_InteractiveObject.hxx>
#include <AIS2D_InteractiveContext.hxx>

extern int ViewerMainLoop2d (Standard_Integer argc, const char** argv);

TopoDS_Shape GetShapeFromName2d (const char* name);

static TColStd_ListOfTransient theEventMgrs2d;

//==============================================================================
//  VIEWER OBJECT MANAGEMENT GLOBAL VARIABLES
//==============================================================================
Standard_EXPORT Viewer2dTest_DoubleMapOfInteractiveAndName& GetMapOfAIS2D()
{
  static Viewer2dTest_DoubleMapOfInteractiveAndName TheMap;
  return TheMap;
}

static void VwrTst_InitEventMgr2d (const Handle(AIS2D_InteractiveContext)& Ctx)
{
  theEventMgrs2d.Clear();
  theEventMgrs2d.Prepend(new Viewer2dTest_EventManager(Ctx));
}

static Handle(V2d_View)& a2DView()
{
  static Handle(V2d_View) Viou;
  return Viou;
}

Standard_EXPORT Handle(AIS2D_InteractiveContext)& TheAIS2DContext()
{ 
  static Handle(AIS2D_InteractiveContext) aContext;
  return aContext;
}

Handle(V2d_View) Viewer2dTest::CurrentView()
{
  return a2DView();
}

void Viewer2dTest::CurrentView (const Handle(V2d_View)& V)
{
  a2DView() = V;
}

Handle(AIS2D_InteractiveContext) Viewer2dTest::GetAIS2DContext()
{
  return TheAIS2DContext();
}

Handle(V2d_Viewer) Viewer2dTest::GetViewerFromContext()
{
  return TheAIS2DContext()->CurrentViewer();
}

Handle(V2d_Viewer) Viewer2dTest::GetCollectorFromContext()
{
  return TheAIS2DContext()->CurrentViewer();
}

void Viewer2dTest::SetAIS2DContext (const Handle(AIS2D_InteractiveContext)& aCtx)
{
  TheAIS2DContext() = aCtx;
  Viewer2dTest::ResetEventManager();
}

void Viewer2dTest::SetEventManager (const Handle(Viewer2dTest_EventManager)& EM)
{
  theEventMgrs2d.Prepend(EM);
}

void Viewer2dTest::UnsetEventManager()
{
  theEventMgrs2d.RemoveFirst();
}

void Viewer2dTest::ResetEventManager()
{
  VwrTst_InitEventMgr2d(Viewer2dTest::GetAIS2DContext());
}

//==============================================================================
//function : CurrentEventManager
//purpose  : 
//==============================================================================
Handle(Viewer2dTest_EventManager) Viewer2dTest::CurrentEventManager()
{
  Handle(Viewer2dTest_EventManager) EM;
  if(theEventMgrs2d.IsEmpty()) return EM;
  Handle(Standard_Transient) Tr =  theEventMgrs2d.First();
  EM = *((Handle(Viewer2dTest_EventManager)*)&Tr);
  return EM;
}

//==============================================================================
//function : Clear
//purpose  : Remove all the object from the viewer
//==============================================================================
void Viewer2dTest::Clear()
{
  if (!Viewer2dTest::CurrentView().IsNull())
  {
    TheAIS2DContext()->CloseLocalContext();
    Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
    while (it.More())
    {
      cout << "Remove " << it.Key2() << endl;
//      TheAIS2DContext()->Remove(it.Key1(), Standard_False);
      TheAIS2DContext()->Erase(it.Key1(), Standard_False, Standard_False);
      it.Next();
    }
    TheAIS2DContext()->UpdateCurrentViewer();
    GetMapOfAIS2D().Clear();
  }
}

//==============================================================================
//function : StandardModeActivation
//purpose  : Activate a selection mode, vertex, edge, wire ..., in a local
//           Context
//==============================================================================
void Viewer2dTest::StandardModeActivation (const Standard_Integer mode)
{
  Handle(AIS2D_InteractiveContext) aContext = Viewer2dTest::GetAIS2DContext();
  aContext->SetPickMode(Graphic2d_PM_INTERSECT);

  const char* cmode = "??????????";

  switch ((AIS2D_TypeOfDetection)mode)
  {
    case AIS2D_TOD_OBJECT:    cmode = "OBJECT"; break;
    case AIS2D_TOD_PRIMITIVE: cmode = "PRIMITIVE"; break;
    case AIS2D_TOD_ELEMENT:   cmode = "ELEMENT"; break;
    case AIS2D_TOD_VERTEX:    cmode = "VERTEX"; break;
    case AIS2D_TOD_NONE:      cmode = "NONE"; break;
  }

  // Activate
  Viewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
  while (it.More())
  {
    Handle(AIS2D_InteractiveObject) ashape = it.Key1();
    ashape->SetContext(aContext);
//      ashape->SetSelectionMode(mode);
    ashape->SetHighlightMode((AIS2D_TypeOfDetection)mode);
    it.Next();
  }
//	aContext->ActivateStandardMode(AIS2D_ProjShape::SelectionType(mode));
  cout << "Mode " << cmode << " ON" << endl;
}

//=======================================================================
//function : PickObjects
//purpose  : 
//=======================================================================
Standard_Boolean Viewer2dTest::PickObjects (Handle(TColStd_HArray1OfTransient)& arr,
                                            const AIS_KindOfInteractive TheType,
                                             const Standard_Integer /*TheSignature*/,
                                             const Standard_Integer MaxPick)
{
  Handle(AIS2D_InteractiveObject) IO;
  Standard_Integer curindex = (TheType == AIS_KOI_None) ? 0 : TheAIS2DContext()->OpenLocalContext();
  
  // step 1: prepare the data
  if (curindex != 0)
  {
//    Handle(AIS2D_SignatureFilter) F1 = new AIS2D_SignatureFilter(TheType,TheSignature);
//    TheAIS2DContext()->AddFilter(F1);
  }
  
  // step 2 : wait for the selection...
//  Standard_Boolean IsGood (Standard_False);
//  Standard_Integer NbPick(0);
  Standard_Boolean NbPickGood (0), NbToReach (arr->Length());
  Standard_Integer NbPickFail(0);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C","D", "E" };
  const char **argvvv = (const char **) bufff;
  
  while (NbPickGood < NbToReach && NbPickFail <= MaxPick)
  {
    while (ViewerMainLoop2d(argccc,argvvv)) {}
/*    Standard_Integer NbStored = TheAIS2DContext()->NbSelected();
    if((unsigned int ) NbStored != NbPickGood)
      NbPickGood= NbStored;
    else
      NbPickFail++;
    cout<<"NbPicked =  "<<NbPickGood<<" |  Nb Pick Fail :"<<NbPickFail<<endl;*/
  }
  
  // step3 get result.
  
  if ((unsigned int ) NbPickFail >= NbToReach) return Standard_False;

  /*
  Standard_Integer i(0);
  for(TheAIS2DContext()->InitSelected();
      TheAIS2DContext()->MoreSelected();
      TheAIS2DContext()->NextSelected()){
    i++;
    Handle(AIS2D_InteractiveObject) IO = TheAIS2DContext()->SelectedInteractive();
    arr->SetValue(i,IO);
  }*/

  if (curindex > 0) TheAIS2DContext()->CloseLocalContext(curindex);
  return Standard_True;
}

//=======================================================================
//function : PickObject
//purpose  : 
//=======================================================================
/*Handle(AIS2D_InteractiveObject) Viewer2dTest::PickObject
       (const AIS2D_KindOfInteractive TheType,
        const Standard_Integer TheSignature,
        const Standard_Integer MaxPick)
{
  Handle(AIS2D_InteractiveObject) IO;
  Standard_Integer curindex = (TheType == AIS2D_KOI_None) ? 0 : TheAIS2DContext()->OpenLocalContext();
  
  // step 1: prepare the data
  
  if(curindex !=0){
    Handle(AIS2D_SignatureFilter) F1 = new AIS2D_SignatureFilter(TheType,TheSignature);
    TheAIS2DContext()->AddFilter(F1);
  }

  // step 2 : wait for the selection...
  Standard_Boolean IsGood (Standard_False);
  Standard_Integer NbPick(0);
  Standard_Integer argccc = 5;
  char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  char **argvvv = bufff;
  
  
  while(!IsGood && NbPick<= MaxPick){
    while(ViewerMainLoop2d(argccc,argvvv)){}
    IsGood = (TheAIS2DContext()->NbSelected()>0) ;
    NbPick++;
    cout<<"Nb Pick :"<<NbPick<<endl;
  }
  
  
  // step3 get result.
  if(IsGood){
    TheAIS2DContext()->InitSelected();
    IO = TheAIS2DContext()->SelectedInteractive();
  }
  
  if(curindex!=0)
    TheAIS2DContext()->CloseLocalContext(curindex);
  return IO;
}*/

//=======================================================================
//function : PickShape
//purpose  : First Activate the rightmode + Put Filters to be able to
//           pick objets that are of type <TheType>...
//=======================================================================
TopoDS_Shape Viewer2dTest::PickShape (const TopAbs_ShapeEnum /*TheType*/,
                                      const Standard_Integer MaxPick)
{
  // step 1: prepare the data
  Standard_Integer curindex = TheAIS2DContext()->OpenLocalContext();
  TopoDS_Shape result;
  
/*  if(TheType==TopAbs_SHAPE)
  {
    Handle(AIS2D_TypeFilter) F1 = new AIS2D_TypeFilter(AIS2D_KOI_Shape);
    TheAIS2DContext()->AddFilter(F1);
  }
  else
  {
    Handle(StdSelect_ShapeTypeFilter) TF = new StdSelect_ShapeTypeFilter(TheType);
    TheAIS2DContext()->AddFilter(TF);
    TheAIS2DContext()->ActivateStandardMode(TheType);
  }*/

  // step 2 : wait for the selection...
  Standard_Boolean NoShape (Standard_True);
  Standard_Integer NbPick (0);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  const char **argvvv = (const char **) bufff;

  while (NoShape && NbPick <= MaxPick)
  {
    while (ViewerMainLoop2d(argccc,argvvv)) {}
//    NoShape = (TheAIS2DContext()->NbSelected() == 0);
    NbPick++;
    cout << "Nb Pick :" << NbPick << endl;
  }
  
  // step3 get result.
  if (!NoShape)
  {
/*    TheAIS2DContext()->InitSelected();
    if (TheAIS2DContext()->HasSelectedShape())
      result = TheAIS2DContext()->SelectedShape();
    else
    {
      Handle(AIS2D_InteractiveObject) IO = TheAIS2DContext()->SelectedInteractive();
      result = (*((Handle(AIS2D_ProjShape)*) &IO))->Shape();
    }*/
  }

  if (curindex > 0) TheAIS2DContext()->CloseLocalContext(curindex);
  return result;
}

//=======================================================================
//function : PickShapes
//purpose  : 
//=======================================================================
Standard_Boolean Viewer2dTest::PickShapes (const TopAbs_ShapeEnum /*TheType*/,
                                           Handle(TopTools_HArray1OfShape)& thearr,
                                           const Standard_Integer /*MaxPick*/)
{
  Standard_Integer Taille = thearr->Length();
  if (Taille > 1)
    cout << " WARNING : Pick with Shift+ MB1 for Selection of more than 1 object" << endl;
  
  // step 1: prepare the data
  Standard_Integer curindex = TheAIS2DContext()->OpenLocalContext();
/*  if (TheType == TopAbs_SHAPE)
  {
    Handle(AIS2D_TypeFilter) F1 = new AIS2D_TypeFilter(AIS2D_KOI_Shape);
    TheAIS2DContext()->AddFilter(F1);
  }
  else
  {
    Handle(StdSelect_ShapeTypeFilter) TF = new StdSelect_ShapeTypeFilter(TheType);
    TheAIS2DContext()->AddFilter(TF);
    TheAIS2DContext()->ActivateStandardMode(TheType);
  }*/

  // step 2 : wait for the selection...
  Standard_Boolean NbToReach (thearr->Length());
  Standard_Integer NbPickFail (0);
  /*
  Standard_Boolean NbPickGood (0) ;
  Standard_Integer argccc = 5;
  char *bufff[] = { "A", "B", "C","D", "E" };
  char **argvvv = bufff;
  
  while (NbPickGood < NbToReach && NbPickFail <= MaxPick)
  {
    while (ViewerMainLoop2d(argccc,argvvv)) {}
    Standard_Integer NbStored = TheAIS2DContext()->NbSelected();
    if ((unsigned int ) NbStored != NbPickGood)
      NbPickGood = NbStored;
    else
      NbPickFail++;
    cout<<"NbPicked =  "<<NbPickGood<<" |  Nb Pick Fail :"<<NbPickFail<<endl;
  }*/
  
  // step3 get result.
  if ((unsigned int) NbPickFail >= NbToReach) return Standard_False;

  /*
  Standard_Integer i(0);
  for (TheAIS2DContext()->InitSelected();
       TheAIS2DContext()->MoreSelected();
       TheAIS2DContext()->NextSelected())
  {
    i++;
    if(TheAIS2DContext()->HasSelectedShape())
      thearr->SetValue(i,TheAIS2DContext()->SelectedShape());
    else
    {
      Handle(AIS2D_InteractiveObject) IO = TheAIS2DContext()->SelectedInteractive();
      thearr->SetValue(i,(*((Handle(AIS2D_ProjShape)*) &IO))->Shape());
    }
  }*/
  
  TheAIS2DContext()->CloseLocalContext(curindex);
  return Standard_True;
}

//=====================================================================
//========================= for testing Draft and Rib =================
//=====================================================================
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <DBRep.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pln.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <Precision.hxx>
#include <BRepAlgo.hxx>
#include <OSD_Environment.hxx>
#include <DrawTrSurf.hxx>

//=======================================================================
//function : IsValid2d
//purpose  : 
//=======================================================================
static Standard_Boolean IsValid2d (const TopTools_ListOfShape& theArgs,
                                   const TopoDS_Shape& theResult,
                                   const Standard_Boolean closedSolid,
                                   const Standard_Boolean GeomCtrl)
{
  OSD_Environment check ("DONT_SWITCH_IS_VALID") ;
  TCollection_AsciiString checkValid = check.Value();
  Standard_Boolean ToCheck = Standard_True;
  if (!checkValid.IsEmpty()) {
#ifdef DEB
    cout <<"DONT_SWITCH_IS_VALID positionnee a :"<<checkValid.ToCString()<<endl;
#endif
    if ( checkValid=="true" || checkValid=="TRUE" ) {
      ToCheck= Standard_False;
    } 
  } else {
#ifdef DEB
    cout <<"DONT_SWITCH_IS_VALID non positionne"<<endl;
#endif
  }
  Standard_Boolean IsValid = Standard_True;
  if (ToCheck) 
    IsValid = BRepAlgo::IsValid(theArgs,theResult,closedSolid,GeomCtrl) ;
  return IsValid;
  
}

//===============================================================================
// TDraft : test draft, uses AIS2D Viewer
// Solid Face Plane Angle  Reverse
//=============================================================================== 
static Standard_Integer TDraft2d (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{ 
  if (argc < 5) return 1;
// argv[1] - TopoDS_Shape Solid
// argv[2] - TopoDS_Shape Face
// argv[3] - TopoDS_Shape Plane
// argv[4] - Standard_Real Angle
// argv[5] - Standard_Integer Reverse

//  sprintf(prefix, argv[1]);
  Standard_Real anAngle = 0;
  Standard_Boolean Rev = Standard_False;
  Standard_Integer rev = 0;
  TopoDS_Shape Solid  = GetShapeFromName2d(argv[1]);
  TopoDS_Shape face   = GetShapeFromName2d(argv[2]);
  TopoDS_Face Face    = TopoDS::Face(face);
  TopoDS_Shape Plane  = GetShapeFromName2d(argv[3]);
  if (Plane.IsNull ()) {
    cout << "TEST : Plane is NULL" << endl;
    return 1;
  }
  anAngle = atof(argv[4]);
  anAngle = 2*M_PI * anAngle / 360.0;
  gp_Pln aPln;
  Handle(Geom_Surface) aSurf;
//  AIS2D_KindOfSurface aSurfType;
//  Standard_Real Offset;
  gp_Dir aDir;
  if(argc > 4) { // == 5
    rev = atoi(argv[5]);
    Rev = (rev)? Standard_True : Standard_False;
  } 
    
  TopoDS_Face face2 = TopoDS::Face(Plane);
//  if (!AIS2D::GetPlaneFromFace(face2, aPln, aSurf, aSurfType, Offset))
//  {
//    cout << "TEST : Can't find plane" << endl;
//    return 1;
//  }

  aDir = aPln.Axis().Direction();
  if (!aPln.Direct()) 
    aDir.Reverse();
  if (Plane.Orientation() == TopAbs_REVERSED) 
    aDir.Reverse();
  cout << "TEST : gp::Resolution() = " << gp::Resolution() << endl;

  BRepOffsetAPI_DraftAngle Draft (Solid);

  if(Abs(anAngle)< Precision::Angular()) {
    cout << "TEST : NULL angle" << endl;
    return 1;}

  if(Rev) anAngle = - anAngle;
  Draft.Add (Face, aDir, anAngle, aPln);
  Draft.Build ();
  if (!Draft.IsDone())  {
    cout << "TEST : Draft Not DONE " << endl;
    return 1;
  }
  TopTools_ListOfShape Larg;
  Larg.Append(Solid);
  if (!IsValid2d(Larg,Draft.Shape(),Standard_True,Standard_False))
  {
    cout << "TEST : DesignAlgo returns Not valid" << endl;
    return 1;
  }

  Handle(AIS2D_InteractiveContext) Ctx = Viewer2dTest::GetAIS2DContext();
  Handle(AIS2D_ProjShape) ais = new AIS2D_ProjShape();
  ais->Add(Draft.Shape());

  if (!ais.IsNull())
  {
//    ais->SetColor(DEFAULT_COLOR);
//    ais->SetMaterial(DEFAULT_MATERIAL);
    // Display the AIS2D_ProjShape without redraw
    Ctx->Display(ais, Standard_False);

    const char *Name = "draft1";
    Standard_Boolean IsBound = GetMapOfAIS2D().IsBound2(Name);
    if (IsBound)
    {
      Handle(AIS2D_InteractiveObject) an_object = GetMapOfAIS2D().Find2(Name);
//      Ctx->Remove(an_object, Standard_True);
      GetMapOfAIS2D().UnBind2(Name);
    }
    GetMapOfAIS2D().Bind(ais, Name);
  }
  Ctx->Display(ais, Standard_True);
  return 0;
}

//============================================================================
//  MyCommands
//============================================================================
void Viewer2dTest::MyCommands (Draw_Interpretor& theCommands)
{
  DrawTrSurf::BasicCommands(theCommands);
  const char* group = "2D AIS Viewer - Check Features Operations commands";

  theCommands.Add("Draft2d",
                  "Draft2d Solid Face Plane Angle Reverse",
		  __FILE__, TDraft2d, group); //Draft_Modification
}

//============================================================================
// All 2D AIS Viewer Commands
//============================================================================
void Viewer2dTest::Commands (Draw_Interpretor& theCommands)
{
  Viewer2dTest::GeneralCommands(theCommands);
  Viewer2dTest::ViewerCommands (theCommands);
  Viewer2dTest::DisplayCommands(theCommands);
  Viewer2dTest::ObjectCommands (theCommands);
}
