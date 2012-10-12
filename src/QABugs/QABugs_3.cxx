// Created on: 2002-06-17
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include<Draw_Interpretor.hxx>
#include<TopLoc_Location.hxx>
#include<TopoDS_Face.hxx>
#include<TopoDS.hxx>
#include<DBRep.hxx>
#include<Geom_Surface.hxx>
#include<BRep_Tool.hxx>
#include<GeomInt_IntSS.hxx>
#include<BRepBuilderAPI_MakeEdge.hxx>
#include <Standard_ErrorHandler.hxx>
#include<tcl.h>

static int BUC60623(Draw_Interpretor& di, Standard_Integer argc, const char ** a)
{
  if(argc!=4)
  {
    di << "Usage : " << a[0] << " result Shape1 Shape2" << "\n";
    return -1;
  }

  TopLoc_Location L1;
  TopLoc_Location L2;
  TopoDS_Face F1 = TopoDS::Face(DBRep::Get(a[2],TopAbs_FACE));
  TopoDS_Face F2 = TopoDS::Face(DBRep::Get(a[3],TopAbs_FACE));
  Handle(Geom_Surface) GSF1 = BRep_Tool::Surface(F1, L1);
  Handle(Geom_Surface) GSF2 = BRep_Tool::Surface(F2, L2);
  GeomInt_IntSS Inter;
  Inter.Perform(GSF1,GSF2, BRep_Tool::Tolerance(F1));
  if (!Inter.IsDone()) {
    di << "Intersection not done" << "\n";
    return 1;
  }
  Standard_Integer nbsol = Inter.NbLines();
  if(!nbsol) {
    di << "The number of solutions is zero!"   << "\n";
    return 0;
  }
  Handle(Geom_Curve) Sol = Inter.Line(1);
  if(!Sol.IsNull()) {
    DBRep::Set(a[1], BRepBuilderAPI_MakeEdge(Sol));
      return 0;
    } else di << "The first solution is Null!"   << "\n";

  di << "fini" << "\n";
  return 0;
}

#include<ViewerTest.hxx>
#include<AIS_InteractiveContext.hxx>
#include<AIS_Shape.hxx>
  
static int BUC60569(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc!=2)
  {
    di << "Usage : " << argv[0] << " shape" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  TopoDS_Shape theShape =  DBRep::Get(argv[1]);

  Handle(AIS_Shape) anAISShape = new AIS_Shape( theShape ); 
  myAISContext->Display( anAISShape, Standard_True );
  myAISContext->OpenLocalContext(); 
  myAISContext->ActivateStandardMode(TopAbs_FACE);
  return 0;
}

static int BUC60614(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc!=2)
  {
    di << "Usage : "<< argv[0] << " shape" << "\n";
    return -1;
  }

  //  di.Eval("vinit");

  TopoDS_Shape theShape =  DBRep::Get(argv[1]);

//  ViewerTest::GetAISContext(); 
  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;    
  }
  Handle(AIS_Shape) anAISShape = new AIS_Shape( theShape ); 
  myAISContext->Display( anAISShape, Standard_True );
  myAISContext->OpenLocalContext(); 
  myAISContext->ActivateStandardMode(TopAbs_COMPOUND);
//  myAISContext->ActivateStandardMode(TopAbs_SOLID);
//  di.Eval("vfit");
//  cout << "vfini" << endl;
  return 0;
}

#include<BRep_Builder.hxx>
#include<BRepTools_ShapeSet.hxx>
#include<BRepTools.hxx>
#include<BRepAdaptor_HSurface.hxx>
#include<TopOpeBRep_PointClassifier.hxx>
#include<Precision.hxx>
#ifdef WNT
#include<stdio.h>
#endif

static int BUC60609(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  gp_Pnt2d uvSurf;
  TopAbs_State state;
  
  if (argc == 3) {
    // BUC60609 shape name
  } else if ( argc == 5 ) {
    // BUC60609 shape name U V
  } else {
    di << "Usage : "<< argv[0] << " shape name [U V]" << "\n";
    return(-1);
  }
  
  TCollection_AsciiString  aFilePath(argv[1]); 
  
  filebuf fic;
  istream in(&fic);
  if (!fic.open(aFilePath.ToCString(),ios::in)) {
    di << "Cannot open file for reading : " << aFilePath << "\n";
    return(-1);
  }

  TopoDS_Shape theShape;
  char typ[255];
  in >> typ;
  if (!in.fail()) {
    if( !strcmp(typ, "DBRep_DrawableShape") ){
      BRep_Builder B;
      BRepTools_ShapeSet S(B);
      S.Read(in);
      S.Read(theShape,in);
    }else{
      di << "Wrong entity type in " << aFilePath << "\n";
      return(-1);
    }
  }

  const TopoDS_Face &face = TopoDS::Face (theShape);

  if(argc > 2){
    DBRep::Set(argv[2],face);
  }

  Standard_Real faceUMin,faceUMax,faceVMin,faceVMax;
  
  BRepTools::UVBounds (face, faceUMin,faceUMax,faceVMin,faceVMax);

  di << "The bounds of the trimmed face:" << "\n";
  di << faceUMin << " <= U <= " << faceUMax << "\n";
  di << faceVMin << " <= V <= " << faceVMax << "\n";
  
  Handle(BRepAdaptor_HSurface) hsurfa = new BRepAdaptor_HSurface(face);
  
  TopOpeBRep_PointClassifier PClass;

  di << "Now test the point classifier by inputting U,V values" << "\n";
  di << "inside or outside the bounds displayed above" << "\n";
  di << "Type stop to exit" << "\n";
  
  // Please register this:
  // ***********************************************
  // Note also that for periodic surfaces such as nimpod_1.topo,
  // the U/V values may be +- 2pi compared to the actual face bounds
  // (because U,V is probably coming from a Geom package routine).
  // Hence IT WOULD BE USEFUL IF TopOpeBRep_PointClassifier COULD
  // COPE WITH PERIODIC SURFACES, i.e. U,V +-Period giving same result.
  // *************************************************

  if (argc == 3) {
    uvSurf = gp_Pnt2d(0.14,5.1);
    state = PClass.Classify(face,uvSurf,Precision::PConfusion());
    if(state == TopAbs_IN || state == TopAbs_ON){
      di << "U=" << 0.14 << " V=" << 5.1 << "  classified INSIDE" << "\n";
    }else{
      di << "U=" << 0.14 << " V=" << 5.1 << "  classified OUTSIDE" << "\n";
    }

    uvSurf = gp_Pnt2d(1.28,5.1);
    state = PClass.Classify(face,uvSurf,Precision::PConfusion());
    if(state == TopAbs_IN || state == TopAbs_ON){
      di << "U=" << 1.28 << " V=" << 5.1 << "  classified INSIDE" << "\n";
    }else{
      di << "U=" << 1.28 << " V=" << 5.1 << "  classified OUTSIDE" << "\n";
    }
  } else {
    uvSurf = gp_Pnt2d(atof(argv[3]),atof(argv[4]));
    state = PClass.Classify(face,uvSurf,Precision::PConfusion());
    if(state == TopAbs_IN || state == TopAbs_ON){
      di << "U=" << atof(argv[3]) << " V=" << atof(argv[4]) << "  classified INSIDE" << "\n";
    }else{
      di << "U=" << atof(argv[3]) << " V=" << atof(argv[4]) << "  classified OUTSIDE" << "\n";
    }
  }
  return 0;
}


#if ! defined(WNT)
void stringerror(int state)
{
 printf("%s",((state&ios::eofbit) !=0)? " [eof]": "");
 printf("%s",((state&ios::failbit)!=0)? " [fail]":"");
 printf("%s",((state&ios::badbit) !=0)? " [bad]": "");
 printf("%s\n",(state==ios::goodbit)? " [ok]": "");
}


//#if defined(LIN)
//#include <strstream>
//#else
//#include <strstream.h>
//#endif
#ifdef HAVE_IOSTREAM
#include <iostream>
#include <sstream>
using namespace std;
#elif defined (HAVE_IOSTREAM_H)
#include <iostream.h>
#include <strstream.h>
#else
#error "check config.h file or compilation options: either HAVE_IOSTREAM or HAVE_IOSTREAM_H should be defined"
#endif
static int UKI61075(Draw_Interpretor& /*di*/, Standard_Integer /*argc*/, const char ** /*argv*/) {
 double da,db;
 char buffer1[128];
#ifndef USE_STL_STREAM
 ostrstream stringout1(buffer1,sizeof(buffer1));
 istrstream stringin1(buffer1,sizeof(buffer1));
#else
 ostringstream stringout1(buffer1);
 istringstream stringin1(buffer1); 
#endif
 char buffer2[128];
#ifndef USE_STL_STREAM
 ostrstream stringout2(buffer2,sizeof(buffer2));
 istrstream stringin2(buffer2,sizeof(buffer2));
#else
 ostringstream stringout2(buffer1);
 istringstream stringin2(buffer1); 
#endif

 stringout1.precision(17);
 stringout2.precision(17);

 da=-(DBL_MAX);
 db=DBL_MAX;
 printf("Valeurs originales :\n\t%.17lg %.17lg\n",da,db);

 stringout1<<da<<' '<<db<<"\n";
#ifndef USE_STL_STREAM
 buffer1[stringout1.pcount()]='\0';
#else
 buffer1[stringout1.str().length()]= '\0' ;
#endif

 printf("Valeurs ecrites dans le fichier :\n\t%s",buffer1);

 da=db=0.;
 stringin1>>da>>db;
 printf("Valeurs relues :\n\t%.17lg %.17lg",da,db);
 stringerror(stringin1.rdstate());

 stringout2<<da<<' '<<db<<"\n";
#ifndef USE_STL_STREAM
 buffer2[stringout2.pcount()]='\0';
#else
 buffer2[stringout2.str().length()]='\0';
#endif

 printf("Valeurs reecrites :\n\t%s",buffer2);

 da=db=0.;
 stringin2>>da>>db;
 printf("Valeurs relues a nouveau :\n\t%.17lg %.17lg",da,db);
 stringerror(stringin2.rdstate());
 
 return(0);
}
#endif

#include<BRepAlgoAPI_Section.hxx>
#include<BRepAlgo_Section.hxx>

#include<Geom_Plane.hxx>
#include<DrawTrSurf.hxx>

//static Standard_CString St = " \"trimsphere\"/\"sphere\" [result] [name] [plane]";
static Standard_CString St = " \"trimsphere\"/\"sphere\" [result] [name] [plane] [BRepAlgoAPI/BRepAlgo = 1/0]";

static int BUC60585(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  
  //if(argc<2) {
  //  cerr << "Usage : " << argv[0] << St << endl;
  //  return -1;
  //}
  if(argc < 2 || argc > 6) {
    di << "Usage : " << argv[0] << " shape1 shape2 shape3 shape4 shape5 shape6 [BRepAlgoAPI/BRepAlgo = 1/0]" << "\n";
    return 1;
  }
  Standard_Boolean IsBRepAlgoAPI = Standard_True;
  if (argc == 6) {
    Standard_Integer IsB = atoi(argv[5]);
    if (IsB != 1) {
      IsBRepAlgoAPI = Standard_False;
#if ! defined(BRepAlgo_def04)
//      di << "Error: There is not BRepAlgo_Section class" << "\n";
//      return 1;
#endif
    }
  }

  
  gp_Dir N;
  if(!strcmp(argv[1],"trimsphere")) {
//////////////////////////////////////////
// Uncomment for trimmed sphere bug:
//    filename = "trimsphere.topo";
    N=gp_Dir( 0.0, -1.0, 0.0 );
//////////////////////////////////////////
  } else if(!strcmp(argv[1],"sphere")) {

//////////////////////////////////////////
// Uncomment for untrimmed sphere bug:

//    filename="sphere.topo";
    N=gp_Dir( 0.0, -0.75103523489975432, -0.66026212668838646 );
    
//////////////////////////////////////////
  } else {
    di << "Usage : " << argv[0] << St << "\n";
    return -1;
  }
  
  // MKV 30.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
  const Standard_Character *DD = Tcl_GetVar(di.Interp(),"Draw_DataDir",TCL_GLOBAL_ONLY);
#else
  Standard_Character *DD = Tcl_GetVar(di.Interp(),"Draw_DataDir",TCL_GLOBAL_ONLY);
#endif

  Standard_Character  *filename = new Standard_Character [strlen(DD)+17];
  sprintf(filename,"%s/%s.topo",DD,argv[1]);

  filebuf fic;
  istream in(&fic);
  if (!fic.open(filename,ios::in)) {
    di << "Cannot open file for reading : " << filename << "\n";
    delete [] filename;
    return -1;
  }
  
  // Read in the shape
  
  BRep_Builder B;
  BRepTools_ShapeSet S(B);
  S.Read(in);
  TopoDS_Shape theShape;
  S.Read(theShape,in);
  
  // Create the plane
  
  gp_Pnt O( 2036.25, -97.5, -1460.499755859375 );
  gp_Dir A( 1.0,  0.0, 0.0 );
  
  gp_Ax3 PLA( O, N, A );
  gp_Pln Pl(PLA);
  
  // Perform the section
  
//#if ! defined(BRepAlgoAPI_def01)
//  BRepAlgoAPI_Section Sec( theShape, Pl, Standard_False);
//#else
//  BRepAlgo_Section Sec( theShape, Pl, Standard_False);
//#endif

  TopoDS_Shape res;
  
  try{
    OCC_CATCH_SIGNALS
//      Sec.Approximation(Standard_True);

    //Sec.Build();
    //if(!Sec.IsDone()){
    //  cout << "Error performing intersection: not done." << endl;
    //  delete filename;
    //  return -1;
    //}
    //res = Sec.Shape();

    if (IsBRepAlgoAPI) {
      di << "BRepAlgoAPI_Section Sec( theShape, Pl, Standard_False)" <<"\n";
      BRepAlgoAPI_Section Sec( theShape, Pl, Standard_False);
      Sec.Build();
      if(!Sec.IsDone()){
	di << "Error performing intersection: not done." << "\n";
	delete [] filename;
	return -1;
      }
      res = Sec.Shape();
    } else {
      di << "BRepAlgo_Section Sec( theShape, Pl, Standard_False)" <<"\n";
      BRepAlgo_Section Sec( theShape, Pl, Standard_False);
      Sec.Build();
      if(!Sec.IsDone()){
	di << "Error performing intersection: not done." << "\n";
	delete [] filename;
	return -1;
      }
      res = Sec.Shape();
    }
    
  }catch(Standard_Failure){
    Handle(Standard_Failure) error = Standard_Failure::Caught();
    di << "Error performing intersection: not done." << "\n";
    delete [] filename;
    return -1;
  }
  
  if(argc>3) DBRep::Set(argv[3],theShape);

  if(argc>2) DBRep::Set(argv[2],res);

  if(argc>4) {
    Handle(Geom_Geometry) result;
    Handle(Geom_Plane) C = new Geom_Plane(Pl);
    result=C;
    DrawTrSurf::Set(argv[4],result);
  }
  
  di << "Done" << "\n";

  delete [] filename;
  
  return 0;
}

#include<TopoDS_Compound.hxx>

static int BUC60547(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=2) {
    di << "Usage : " << argv[0] << " name"   << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  
  // MKV 30.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
  const Standard_Character *DD = Tcl_GetVar(di.Interp(),"Draw_DataDir",TCL_GLOBAL_ONLY);
#else
  Standard_Character *DD = Tcl_GetVar(di.Interp(),"Draw_DataDir",TCL_GLOBAL_ONLY);
#endif

  Standard_Character *Ch = new Standard_Character[strlen(argv[1])+3];

  Standard_Character *FileName = new Standard_Character[strlen(DD)+13];
 
  TopoDS_Shape free_1,free_2,free_3,free_4;
  BRep_Builder B;
  sprintf(FileName,"%s/%s",DD,"buc60547a.brep");
  BRepTools::Read(free_1,FileName,B);
  sprintf(FileName,"%s/%s",DD,"buc60547b.brep");
  BRepTools::Read(free_2,FileName,B);
  sprintf(FileName,"%s/%s",DD,"buc60547c.brep");
  BRepTools::Read(free_3,FileName,B);
  sprintf(FileName,"%s/%s",DD,"buc60547d.brep");
  BRepTools::Read(free_4,FileName,B);
  sprintf(Ch,"%s_%i",argv[1],1);
  DBRep::Set(Ch,free_1);
  di << Ch << " ";
  sprintf(Ch,"%s_%i",argv[1],2);
  DBRep::Set(Ch,free_2);
  di << Ch << " ";
  sprintf(Ch,"%s_%i",argv[1],3);
  DBRep::Set(Ch,free_3);
  di << Ch << " ";
  sprintf(Ch,"%s_%i",argv[1],4);
  DBRep::Set(Ch,free_4);
  di << Ch << " ";
  
//  Handle(AIS_Shape) S1 = new AIS_Shape(free_1); 
//  Handle(AIS_Shape) S2 = new AIS_Shape(free_2); 
//  Handle(AIS_Shape) S3 = new AIS_Shape(free_3); 
//  Handle(AIS_Shape) S4 = new AIS_Shape(free_4);
  
//  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();

//  myAISContext->Display(S1); 
//  myAISContext->Display(S2); 
//  myAISContext->Display(S3); 
//  myAISContext->Display(S4);

//  di.Eval("vfit");
  
  TopoDS_Compound Com; 
  BRep_Builder bui;
  bui.MakeCompound(Com); 
  bui.Add(Com,free_1); 
  bui.Add(Com,free_2); 
  bui.Add(Com,free_3); 
  bui.Add(Com,free_4); 

  sprintf(Ch,"%s_%c",argv[1],'c');
  DBRep::Set(Ch,Com);
  di << Ch << " ";

  Handle(AIS_Shape) SC = new AIS_Shape(Com);
  myAISContext->Display(SC); // nothing on the screen If I save the compound :

  sprintf(FileName,"%s/%s",DD,"free.brep");

  BRepTools::Write(Com,FileName); 

  delete [] Ch;
  delete [] FileName;
  
  return 0;
}

#include<BRepBuilderAPI_MakeVertex.hxx>
#include<TCollection_ExtendedString.hxx>
#include<AIS_LengthDimension.hxx>

static Standard_Integer BUC60632(Draw_Interpretor& di, Standard_Integer /*n*/, const char ** a)
{
  
  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << a[0] << "\n";
    return -1;
  }
  myAIScontext->EraseAll();
  
  TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0,0,0)); 
  TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,10,0)); 
  
  Handle(AIS_Shape) Ve1 = new AIS_Shape(V1);
  Handle(AIS_Shape) Ve2 = new AIS_Shape(V2);
  
  myAIScontext->Display(Ve1); 
  myAIScontext->Display(Ve2); 
  
  Handle(Geom_Plane) Plane1 = new Geom_Plane(gp_Pnt(0,0,0),gp_Dir(0,0,1)); 
  TCollection_ExtendedString Ext1("Dim1"); 
  Handle(AIS_LengthDimension) Dim1 = new AIS_LengthDimension(V1,V2,Plane1,atof(a[2]),Ext1); 
  
  myAIScontext->SetDisplayMode(Dim1, atoi(a[1]));
  myAIScontext->Display(Dim1);
  return 0;
}

#include<TopoDS_Wire.hxx>

static Standard_Integer BUC60652(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc!=2) {
    di << "Usage : BUC60652 fase"   << "\n";
    return 1;
  }
  TopoDS_Shape shape = DBRep::Get( argv[1] ); 
  TopoDS_Face face = TopoDS::Face( shape ); 
  TopoDS_Wire ow = BRepTools::OuterWire( face ); 
  DBRep::Set( "w", ow ); 
  return 0; 
}

#include <BRepAlgo_BooleanOperations.hxx>
  
static Standard_Integer defNbPntMax = 30;
static Standard_Real defTol3d = 1.e-7;
static Standard_Real defTol2d = 1.e-7;
static Standard_Boolean defRelativeTol=Standard_True;
Standard_Integer NbPntMax = defNbPntMax;
Standard_Real Toler3d =defTol3d;
Standard_Real Toler2d = defTol2d;
Standard_Boolean RelativeTol= defRelativeTol;
//              //== // ksection : operateur section appelant BRepAlgo_BooleanOperation
//== // ksection : operateur section appelant BRepAlgo_BooleanOperations
//=======================================================================
Standard_Integer ksection(Draw_Interpretor& di, Standard_Integer n, const char ** a) {
  if (n < 8) {
    di << "Usage : " << a[0] << " resultat shell1 shell2 NbPntMax Toler3d Toler2d RelativeTol"   << "\n";
    return -1;
  }
  // a[1]= resultat
  // a[2]= shell1
  // a[3]= shell2
  // a[4]= NbPntMax
  // a[5]= Toler3d
  // a[6]= Toler2d
  // a[7]= RelativeTol
  TopoDS_Shape s1 = DBRep::Get(a[2],TopAbs_SHELL);
  TopoDS_Shape s2 = DBRep::Get(a[3],TopAbs_SHELL);
  if (s1.IsNull() || s2.IsNull()) return 1;
  NbPntMax=atoi(a[4]);
  Toler3d=atof(a[5]);
  Toler2d=atof(a[6]);
  RelativeTol=atoi(a[7]);

  di << "BRepAlgo_BooleanOperations myalgo" << "\n";
  BRepAlgo_BooleanOperations myalgo;

  myalgo.Shapes(s1, s2);
  myalgo.SetApproxParameters(NbPntMax,Toler3d,Toler2d,RelativeTol);
  TopoDS_Shape res; res = myalgo.Section();
  DBRep::Set(a[1],res);
  return 0;
}

#include <Geom_Axis2Placement.hxx>
#include <AIS_Trihedron.hxx>

static Standard_Integer BUC60574(Draw_Interpretor& di, Standard_Integer /*n*/, const char ** a)
{
  
  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << a[0] << "\n";
    return -1;
  }
  
  Handle(Geom_Axis2Placement) atrihedronAxis = new Geom_Axis2Placement(gp::XOY()); 
  Handle(AIS_Trihedron) atri = new AIS_Trihedron(atrihedronAxis); 
  gp_Trsf aTrsf; 
  gp_Vec trans(5,5,5); 
  aTrsf.SetTranslation(trans); 
  TopLoc_Location aLoc(aTrsf); 
  myAISContext->SetLocation(atri,aLoc); 
  myAISContext->Display(atri,0,-1,Standard_True, Standard_True);
  myAISContext->OpenLocalContext(Standard_False, 
				 Standard_True,Standard_False,Standard_False); 
  myAISContext->Load(atri,3,Standard_True);

  return 0;
}

#include <TopoDS_Solid.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgo_Fuse.hxx>

#include <V3d_Plane.hxx>
#include <V3d_View.hxx>
#include <gce_MakePln.hxx>

static Standard_Integer BUC60698(Draw_Interpretor& di, Standard_Integer argc, const char ** a)
{
  if(argc > 2) {
    di << "Usage : " << a[0] << " [BRepAlgoAPI/BRepAlgo = 1/0]" << "\n";
    return 1;
  }
  Standard_Boolean IsBRepAlgoAPI = Standard_True;
  if (argc == 2) {
    Standard_Integer IsB = atoi(a[1]);
    if (IsB != 1) {
      IsBRepAlgoAPI = Standard_False;
#if ! defined(BRepAlgo_def01)
//      di << "Error: There is not BRepAlgo_Fuse class" << "\n";
//      return 1;
#endif
    }
  }
  
  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << a[0] << "\n";
    return -1;
  }
  TopoDS_Solid box = BRepPrimAPI_MakeBox(1,1,1).Solid();
  TopoDS_Shape sphere = BRepPrimAPI_MakeSphere(gp_Pnt(0.5,0.5,0.5),0.6).Shape();

//#if ! defined(BRepAlgoAPI_def01)
//  TopoDS_Shape fuse = BRepAlgoAPI_Fuse(box,sphere).Shape();
//#else
//  TopoDS_Shape fuse = BRepAlgo_Fuse(box,sphere).Shape();
//#endif

  TopoDS_Shape fuse;
  if (IsBRepAlgoAPI) {
    di << "fuse = BRepAlgoAPI_Fuse(box,sphere).Shape()" <<"\n";
    fuse = BRepAlgoAPI_Fuse(box,sphere).Shape();
  } else {
    di << "fuse = BRepAlgo_Fuse(box,sphere).Shape()" <<"\n";
    fuse = BRepAlgo_Fuse(box,sphere).Shape();
  }

  Handle_AIS_Shape theAISShape = new AIS_Shape(fuse);
  myAISContext->Display(theAISShape);
  di.Eval("vfit");
  gp_Pln thegpPln = gce_MakePln(gp_Pnt(0.5,0.5,0.5),gp_Dir(0,0,1));
  Standard_Real A,B,C,D;
  thegpPln.Coefficients(A,B,C,D);
  Handle_V3d_Plane thePlane = new V3d_Plane(A,B,C,D);
  myAISContext->CurrentViewer()->AddPlane (thePlane); // add to defined planes list
  for (myAISContext->CurrentViewer()->InitActiveViews();
       myAISContext->CurrentViewer()->MoreActiveViews ();
       myAISContext->CurrentViewer()->NextActiveViews ()) {
    try {
      OCC_CATCH_SIGNALS
      myAISContext->CurrentViewer()->ActiveView()->SetPlaneOn(thePlane);
    }
    catch(Standard_Failure) {
      di << "SetPlaneOn catched 1" << "\n";
    }
#ifdef WNT
    catch(...) {
      di << "SetPlaneOn catched 1" << "\n";
    }
#endif
  }//ActiveView loop
  for (myAISContext->CurrentViewer()->InitDefinedViews();
       myAISContext->CurrentViewer()->MoreDefinedViews ();
       myAISContext->CurrentViewer()->NextDefinedViews ()) {
    try {
      OCC_CATCH_SIGNALS
      myAISContext->CurrentViewer()->DefinedView()->SetPlaneOn(thePlane);
    }
    catch(Standard_Failure) {
      di << "SetPlaneOn catched 1" << "\n";
    }
#ifdef WNT
    catch(...) {
      di << "SetPlaneOn catched 2" << "\n";
    }
#endif
  }//DefinedView loop
  myAISContext->UpdateCurrentViewer();
  myAISContext->OpenLocalContext();
  myAISContext->ActivateStandardMode(TopAbs_FACE);
  return 0;
}

static Standard_Integer BUC60699(Draw_Interpretor& di, Standard_Integer /*n*/, const char ** a)
{
  
  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << a[0] << "\n";
    return -1;
  }
  TopoDS_Solid B1 = BRepPrimAPI_MakeBox (1,1,1).Solid();
  TopAbs_ShapeEnum theType = B1.ShapeType();
  if ( theType == TopAbs_SOLID ) {
    di << "It is a solid."   << "\n";
  } else {
    di << "It is not solid."   << "\n";
  }
  myAISContext->Display(new AIS_Shape(B1)); 
  myAISContext->OpenLocalContext();
  TopAbs_ShapeEnum amode = TopAbs_SOLID;
  myAISContext->ActivateStandardMode(amode);
  di.Eval("vfit");
  di.Eval("QAMoveTo 200 200");
  di.Eval("QASelect 200 200");
  myAISContext->InitSelected() ;
  if ( myAISContext->MoreSelected() ) {
    if (myAISContext->HasSelectedShape() ) {
      di << "has selected shape : OK"   << "\n";
    } else {
      di << "has selected shape : bugged - Faulty "   << "\n";
    }
  }
  return 0;
}

static Standard_Integer GER61394(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc > 2) {
    di << "Usage : " << argv[0] << " [1/0]" << "\n";
    return -1;
  }
  
  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  Handle(V3d_View) myV3dView = ViewerTest::CurrentView();
  
  if((argc == 2) && (atof(argv[1]) == 0))
    myV3dView->SetAntialiasingOff();
  else
    myV3dView->SetAntialiasingOn();
  myV3dView->Update();
  return 0;
}


#define DEFAULT_COLOR    Quantity_NOC_GOLDENROD

//=======================================================================
//function : GetColorFromName
//purpose  : get the Quantity_NameOfColor from a string
//=======================================================================

static Quantity_NameOfColor GetColorFromName( const char *name ) 
{ 
  Quantity_NameOfColor ret = DEFAULT_COLOR;
  
  Standard_Boolean Found = Standard_False;
  Standard_CString colstring;
  for(Standard_Integer i=0;i<=514 && !Found;i++)
    {
      colstring = Quantity_Color::StringName(Quantity_NameOfColor(i));
      if (!strcasecmp(name,colstring)) {
	ret = (Quantity_NameOfColor)i;
	Found = Standard_True;
      }
    }
  
  return ret;
}
   
static Standard_Integer setcolor (Draw_Interpretor& di,Standard_Integer argc, const char ** argv )
{

Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
if(myAISContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

Handle(V3d_View) myV3dView = ViewerTest::CurrentView();

switch (argc){

 case 2:
   {
     di <<"case 2 : This command will change the background color to " << argv[1]<< "\n";
//   setcolor <name> 
//   Change the background color of the view with a predefined name Graphic3d_NOC_<name>  

     myV3dView -> SetBackgroundColor(GetColorFromName(argv[1]));
     myV3dView -> Redraw();
   break;
   }

 case 3:
   {
     di <<"case 3 : This command will change the color of the objects to "<< argv[2]<< "\n";
//   setcolor <object> <name> 
//   Change the object color with a predefined name 

     TopoDS_Shape aShape = DBRep::Get(argv[1]);
     Handle(AIS_InteractiveObject) myShape = new AIS_Shape (aShape);
     myAISContext->SetColor(myShape,GetColorFromName(argv[2]),Standard_True);
     myAISContext->Display(myShape,Standard_True);
     myAISContext->UpdateCurrentViewer();
//     return 0;
   break;
   }
 case 4:
   {
     di <<"case 4 : This command will change the background color to <r> <g> <b> :"<< argv[1] << argv[2] << argv[3] << "\n";

//   setcolor <r> <g> <b> 
//   Change the background color of the view with the color values <r>,<g>,<b> 
//   A color value must be defined in the space [0.,1.] 

     Standard_Real QuantityOfRed   = atoi(argv[1]);
     Standard_Real QuantityOfGreen = atoi(argv[2]);
     Standard_Real QuantityOfBlue  = atoi(argv[3]);
     myV3dView->SetBackgroundColor(Quantity_TOC_RGB,QuantityOfRed,QuantityOfGreen,QuantityOfBlue);
     myV3dView->Redraw();
   break;
   }

 case 5:
   {
    di <<"case 5 : This command will change the color of the objects to <r> <g> <b> : "<<argv[2]<< argv[3]<< argv[4]<< "\n";

//  setcolor <object> <r> <g> <b> 
//  change the object color with RGB values. 


    Standard_Real QuantityOfRed   = atof(argv[2]);
    Standard_Real QuantityOfGreen = atof(argv[3]);
    Standard_Real QuantityOfBlue  = atof(argv[4]);

    TopoDS_Shape aShape = DBRep::Get(argv[1]);
    Handle(AIS_InteractiveObject) myShape =  new AIS_Shape (aShape);
    myAISContext->SetColor(myShape,Quantity_Color(QuantityOfRed,QuantityOfGreen,QuantityOfBlue,Quantity_TOC_RGB),Standard_True);
    myAISContext->Display(myShape,Standard_True);
    myAISContext->UpdateCurrentViewer();
//  myShape->SetColor(Quantity_Color(QuantityOfRed,QuantityOfGreen,QuantityOfBlue,Quantity_TOC_RGB));
//  myShape->Redisplay();
   break;
   }
 }
return 0;
}

static Standard_Integer BUC60726 (Draw_Interpretor& di,Standard_Integer argc, const char ** argv )
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
    myAISContext->CloseAllContexts();
    BRepPrimAPI_MakeBox B(gp_Pnt(-400.,-400.,-100.),200.,150.,100.);
    Handle(AIS_Shape) aBox = new AIS_Shape(B.Shape());
    myAISContext->Display(aBox);
  } else if(atoi(argv[1]) == 1) {
    myAISContext->CloseAllContexts();
    myAISContext->OpenLocalContext();
    myAISContext->ActivateStandardMode(TopAbs_EDGE);
  } else if(atoi(argv[1]) == 2) {
    myAISContext->CloseAllContexts();
    myAISContext->OpenLocalContext();
    myAISContext->ActivateStandardMode(TopAbs_FACE);
  } else {
    di << "Usage : " << argv[0] << " 0/1" << "\n";
    return -1;
  }
  
  return 0;
}

#include <BRepBndLib.hxx>
#include <Bnd_HArray1OfBox.hxx>
  
static Standard_Integer BUC60729 (Draw_Interpretor& di,Standard_Integer /*argc*/, const char ** /*argv*/ )
{
  Bnd_Box aMainBox;
  TopoDS_Shape aShape = BRepPrimAPI_MakeBox(1,1,1).Solid();

  BRepBndLib::Add(aShape , aMainBox );

  Standard_Integer siMaxNbrBox = 6;
  Bnd_BoundSortBox m_BoundSortBox;
  m_BoundSortBox.Initialize( aMainBox, siMaxNbrBox );
  TopExp_Explorer aExplorer(aShape,TopAbs_FACE);
  Standard_Integer i;


//  Bnd_Box __emptyBox; // Box is void !
//  Handle_Bnd_HArray1OfBox __aSetOfBox = new Bnd_HArray1OfBox( 1, siMaxNbrBox, __emptyBox ); 

  for (i=1,aExplorer.ReInit(); aExplorer.More(); aExplorer.Next(),i++ ) 
    { 
      const TopoDS_Shape& aFace = aExplorer.Current();
      Bnd_Box aBox;
      BRepBndLib::Add( aFace, aBox );
      m_BoundSortBox.Add( aBox, i );
//      __aSetOfBox->SetValue( i, aBox ); 
    } 
//  m_BoundSortBox.Initialize( aMainBox, siMaxNbrBox );
  
  return 0;
}

static Standard_Integer BUC60724(Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/ )
{
  TCollection_AsciiString as1("");
  TCollection_AsciiString as2('\0');
  if((as1.ToCString()!=NULL) || (as1.ToCString() != ""))
    di << "Faulty : the first string is not zero string : " << as1.ToCString() << "\n";

  if((as2.ToCString()!=NULL) || (as2.ToCString() != ""))
    di << "Faulty : the second string is not zero string : " << as2.ToCString() << "\n";
  
  return 0;
}

#include <UnitsAPI.hxx>

static Standard_Integer BUC60727(Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/ )
{
di <<"Program Test" << "\n";
UnitsAPI::SetLocalSystem(UnitsAPI_MDTV); //length is mm 
di <<"AnyToLS (3,mm) = " << UnitsAPI::AnyToLS(3.,"mm") << "\n"; // result was WRONG. 

   return 0;
}

#include <gp_Circ.hxx>
#include <Geom_Circle.hxx>
#include <GeomAPI.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2d_Circle.hxx>
#include <ProjLib.hxx>

static Standard_Integer BUC60792(Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv )
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  gp_Pnt pt3d(0, 20, 150);
  gp_Ax2 anAx2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 0, 1));
  gp_Circ circ(anAx2, 50.0); 
  Handle_Geom_Circle gcir = new Geom_Circle(circ); 
  Handle_Geom_Plane pln = new Geom_Plane(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0))); 
  Handle_Geom2d_Curve gcir1 = GeomAPI::To2d(gcir, pln->Pln()); 
  TopoDS_Shape sh1 = BRepBuilderAPI_MakeEdge(gcir1, pln).Shape(); 
  Handle_AIS_Shape ais1 = new AIS_Shape(sh1); 
  aContext->SetColor(ais1, Quantity_NOC_INDIANRED); 
  aContext->Display(ais1); 
  DBRep::Set("sh0",sh1);
  gp_Pnt2d thepoint; 
//  local_get_2Dpointfrom3Dpoint(pt3d, pln->Pln(), thepoint); 
  thepoint = ProjLib::Project(pln->Pln(),pt3d);
  Handle_Geom2d_CartesianPoint ThePoint = new Geom2d_CartesianPoint(thepoint); 
  Geom2dAdaptor_Curve acur1(gcir1) ; 
  Geom2dGcc_QualifiedCurve qcur1(acur1, GccEnt_outside) ; 
  Geom2dGcc_Circ2d2TanRad cirtanrad(qcur1, ThePoint, 200.0, 0.0001); 
  printf("\n No. of solutions = %d\n", cirtanrad.NbSolutions()); 
  Handle_Geom2d_Circle gccc; 
  if( cirtanrad.NbSolutions() ) { 
    for( int i = 1; i<=cirtanrad.NbSolutions(); i++) { 
      gp_Circ2d ccc = cirtanrad.ThisSolution(i); 
      gccc = new Geom2d_Circle(ccc); 
      TopoDS_Shape sh = BRepBuilderAPI_MakeEdge(gccc, pln).Shape();
      Standard_Character aStr[5];
      sprintf(aStr,"sh%d",i);
      DBRep::Set(aStr,sh);
      Handle_AIS_Shape ais = new AIS_Shape(sh); 
      if( i ==1 ) 
	aContext->SetColor(ais, Quantity_NOC_GREEN); 
      if( i == 2) 
	aContext->SetColor(ais, Quantity_NOC_HOTPINK); 
      aContext->Display(ais); 
      Standard_Real ParSol1, ParSol2, ParArg1, ParArg2; 
      gp_Pnt2d PntSol1, PntSol2; 
      cirtanrad.Tangency1(i, ParSol1, ParArg1, PntSol1);
      printf("%f\t%f\t\t%f\t%f\n",ParSol1, ParArg1,PntSol1.X(),PntSol1.Y());
      cirtanrad.Tangency2(i, ParSol2, ParArg2, PntSol2); 
      printf("%f\t%f\t\t%f\t%f\n",ParSol2, ParArg2,PntSol2.X(),PntSol2.Y());
    }
  }
  return 0;
}

#include <TColgp_Array2OfPnt.hxx>
#include <Geom_BezierSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Geom_OffsetSurface.hxx>
#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <GeomProjLib.hxx>
#include <Geom_TrimmedCurve.hxx>

static Standard_Integer BUC60811(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc == 4) {
    TopLoc_Location L1;
    TopoDS_Edge aEdge = TopoDS::Edge(DBRep::Get(argv[2],TopAbs_EDGE));
    TopoDS_Face aFace = TopoDS::Face(DBRep::Get(argv[3],TopAbs_FACE));
    Standard_Real f = 0.0, l = 0.0; 
    Handle(Geom_Curve) GC = BRep_Tool::Curve(aEdge,f,l);
    Handle(Geom_Surface) GS = BRep_Tool::Surface(aFace, L1);
    GC = new Geom_TrimmedCurve(GC, f, l); 
    Handle(Geom_Curve) projCurve = GeomProjLib::Project(GC,GS); 
    BRepBuilderAPI_MakeWire *myWire; 
    myWire = new BRepBuilderAPI_MakeWire(); 
    myWire->Add((BRepBuilderAPI_MakeEdge(projCurve)).Edge());
    DBRep::Set(argv[1],myWire->Wire());
    return  0;
  }
  
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  
//step 1. creating a Bezier Surface and a patch 
  TopoDS_Face FP; 
  TopoDS_Shape FP1; 
  TopoDS_Solid solid; 
  Handle(AIS_Shape) ais1; 
  Handle_AIS_Shape ais2; 
  Handle(Geom_BezierSurface) BZ1;
  TColgp_Array2OfPnt array1(1,3,1,3);
  array1.SetValue(1,1,gp_Pnt(0,100,0));
  array1.SetValue(1,2,gp_Pnt(200,100,0));
  array1.SetValue(1,3,gp_Pnt(400,100,0)); 
  array1.SetValue(2,1,gp_Pnt(0,200,100)); 
  array1.SetValue(2,2,gp_Pnt(200,200,100)); 
  array1.SetValue(2,3,gp_Pnt(400,200,100)); 
  array1.SetValue(3,1,gp_Pnt(0,300,0)); 
  array1.SetValue(3,2,gp_Pnt(200,300,0)); 
  array1.SetValue(3,3,gp_Pnt(400,300,0)); 
  BZ1 = new Geom_BezierSurface(array1);
  BRepBuilderAPI_MakeFace bzf1( BZ1, Precision::Confusion() );
  TopoDS_Face F1= bzf1.Face();
  ais1 = new AIS_Shape(F1);
  DBRep::Set("F1",F1);
  aContext->SetMaterial(ais1,Graphic3d_NOM_ALUMINIUM,Standard_False);
  aContext->Display(ais1);
  BRep_Builder B;
  TopoDS_Shell shell;
  B.MakeShell(shell);
  B.Add(shell, bzf1); 
  B.MakeSolid(solid);
  B.Add(solid,shell); 
  gp_Dir D(0, 0, 1.0f); 
  BRepBuilderAPI_MakeWire mkw; 
  gp_Pnt p1 = gp_Pnt(150., 150.0, 260.);
  gp_Pnt p2 = gp_Pnt(350., 150., 260.); 
  BRepBuilderAPI_MakeEdge* E1 = new BRepBuilderAPI_MakeEdge(p1,p2); 
  mkw.Add(*E1); 
  p1 = gp_Pnt(350., 150., 260.); 
  p2 = gp_Pnt(350., 250., 260.); 
  BRepBuilderAPI_MakeEdge* E2 = new BRepBuilderAPI_MakeEdge(p1,p2); 
  mkw.Add(*E2); 
  p1 = gp_Pnt(350., 250., 260.); 
  p2 = gp_Pnt(300., 250.0, 260.); 
  BRepBuilderAPI_MakeEdge* E3 = new BRepBuilderAPI_MakeEdge(p1,p2);
  mkw.Add(*E3); 
  p1 = gp_Pnt(300., 250.0, 260.); 
  p2 = gp_Pnt(200., 200.0, 260.); 
  BRepBuilderAPI_MakeEdge* E4 = new BRepBuilderAPI_MakeEdge(p1,p2); 
  mkw.Add(*E4); 
  p1 = gp_Pnt(200., 200.0, 260.); 
  p2 = gp_Pnt(150., 200.0, 260.); 
  BRepBuilderAPI_MakeEdge* E5 = new BRepBuilderAPI_MakeEdge(p1,p2);
  mkw.Add(*E5); 
  p1 = gp_Pnt(150., 200.0, 260.); 
  p2 = gp_Pnt(150., 150.0, 260.); 
  BRepBuilderAPI_MakeEdge* E6 = new BRepBuilderAPI_MakeEdge(p1,p2);
  mkw.Add(*E6); 
  FP = BRepBuilderAPI_MakeFace(mkw.Wire()); 
  ais2 = new AIS_Shape( FP ); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_ALUMINIUM,Standard_False); 
  aContext->Display( ais2 );

  DBRep::Set("FP",FP);
  
//step 2. offseting the surface. 
  Handle_Geom_OffsetSurface offsurf; 
  offsurf = new Geom_OffsetSurface(BZ1, -100); 
  BRepBuilderAPI_MakeFace bzf2( offsurf, Precision::Confusion() ); 
  TopoDS_Face F2= bzf2.Face(); 
  Handle_AIS_Shape ais22 = new AIS_Shape(F2); 
  aContext->Display(ais22); 
  DBRep::Set("F2",F2);
  
//step 3. filleting the patch. 
//( I want to project wire of this patch on offseted surface above) 
  BRepFilletAPI_MakeFillet2d fillet( FP ); 
  TopExp_Explorer Ex; 
  Ex.Init(FP, TopAbs_VERTEX); 
  TopoDS_Vertex v1 = TopoDS::Vertex(Ex.Current()); 
  fillet.AddFillet(v1, 20); 
  di << "\n" << "Error is " << fillet.Status() << "\n";
//  printf("\nError is %d ", fillet.Status()); 
  Ex.Next(); 
  TopoDS_Vertex V2 = TopoDS::Vertex(Ex.Current()); 
  fillet.AddFillet(V2, 20); 
  di << "\n" << "Error is " << fillet.Status() << "\n";
//  printf("\nError is %d ", fillet.Status());
  fillet.Build(); 
  FP1 = fillet.Shape(); 
  ais2 = new AIS_Shape( FP1 ); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_ALUMINIUM,Standard_False); 
  aContext->Display( ais2 ); 

  DBRep::Set("FP1",FP1);
  
//step 4. Projecting the wire of this patch on offsetted surface. 
//  TopExp_Explorer Ex; 
  BRepBuilderAPI_MakeWire *myWire; 
  myWire = new BRepBuilderAPI_MakeWire(); 
  for (Ex.Init( FP1, TopAbs_EDGE); Ex.More(); Ex.Next()) 
    { 
      TopoDS_Edge e1 = TopoDS::Edge(Ex.Current()); 
      Standard_Real f = 0.0, l = 0.0; 
      Handle_Geom_Curve newBSplin = BRep_Tool::Curve(e1, f, l);
      newBSplin = new Geom_TrimmedCurve(newBSplin, f, l); 
      Handle(Geom_Curve) projCurve = GeomProjLib::Project(newBSplin,offsurf); 
      myWire->Add((BRepBuilderAPI_MakeEdge(projCurve)).Edge()); 
    } 
  Handle_AIS_Shape ais33 = new AIS_Shape( myWire->Wire() ); 
  aContext->Display(ais33);

  DBRep::Set("Wire",myWire->Wire());
  
  return 0;
}

#include<GeomAPI_ExtremaCurveCurve.hxx>

static int BUC60825(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)

{
  if(argc < 3){
    printf("Usage: %s edge1 edge2",argv[0]);
    return(-1);
  }
 
        TopoDS_Edge E1 = TopoDS::Edge(DBRep::Get(argv[1])),
	E2 = TopoDS::Edge(DBRep::Get(argv[2]));

	Standard_Real fp , lp;

	Handle(Geom_Curve) C1 = BRep_Tool::Curve(E1 , fp , lp),
        C2 = BRep_Tool::Curve(E2 , fp , lp);

	GeomAPI_ExtremaCurveCurve aExt(C1 , C2);

	di << "NB RESULTS : " << aExt.NbExtrema() << "\n";

	return 0;
}

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>

static int OCC10006(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if(argc > 2) {
    di << "Usage : " << argv[0] << " [BRepAlgoAPI/BRepAlgo = 1/0]" << "\n";
    return 1;
  }
  Standard_Boolean IsBRepAlgoAPI = Standard_True;
  if (argc == 2) {
    Standard_Integer IsB = atoi(argv[1]);
    if (IsB != 1) {
      IsBRepAlgoAPI = Standard_False;
#if ! defined(BRepAlgo_def01)
//      di << "Error: There is not BRepAlgo_Fuse class" << "\n";
//      return 1;
#endif
    }
  }

  double bottompoints1[12] = { 10, -10, 0, 100, -10, 0, 100, -100, 0, 10, -100, 0};
  double toppoints1[12] = { 0, 0, 10, 100, 0, 10, 100, -100, 10, 0, -100, 10};
  double bottompoints2[12] = { 0, 0, 10.00, 100, 0, 10.00, 100, -100, 10.00, 0, -100, 10.00};
  double toppoints2[12] = { 0, 0, 250, 100, 0, 250, 100, -100, 250, 0, -100, 250};
  BRepBuilderAPI_MakePolygon bottompolygon1, toppolygon1, bottompolygon2, toppolygon2;
  gp_Pnt tmppnt;
  for (int i=0;i<4;i++) {
    tmppnt.SetCoord(bottompoints1[3*i], bottompoints1[3*i+1], bottompoints1[3*i+2]);
    bottompolygon1.Add(tmppnt);
    tmppnt.SetCoord(toppoints1[3*i], toppoints1[3*i+1], toppoints1[3*i+2]);
    toppolygon1.Add(tmppnt);
    tmppnt.SetCoord(bottompoints2[3*i], bottompoints2[3*i+1], bottompoints2[3*i+2]);
    bottompolygon2.Add(tmppnt);
    tmppnt.SetCoord(toppoints2[3*i], toppoints2[3*i+1], toppoints2[3*i+2]);
    toppolygon2.Add(tmppnt);
  }
  bottompolygon1.Close();
  DBRep::Set("B1",bottompolygon1.Shape());
  toppolygon1.Close();
  DBRep::Set("T1",toppolygon1.Shape());
  bottompolygon2.Close();
  DBRep::Set("B2",bottompolygon2.Shape());
  toppolygon2.Close();
  DBRep::Set("T2",toppolygon2.Shape());
  BRepOffsetAPI_ThruSections loft1(Standard_True, Standard_True);
  loft1.AddWire(bottompolygon1.Wire());
  loft1.AddWire(toppolygon1.Wire());
  loft1.Build();
  BRepOffsetAPI_ThruSections loft2(Standard_True, Standard_True);
  loft2.AddWire(bottompolygon2.Wire());
  loft2.AddWire(toppolygon2.Wire());
  loft2.Build();
  if (loft1.Shape().IsNull() || loft2.Shape().IsNull())
    return 1;
  DBRep::Set("TS1",loft1.Shape());
  DBRep::Set("TS2",loft2.Shape());

//#if ! defined(BRepAlgoAPI_def01)
//  BRepAlgoAPI_Fuse result(loft1.Shape(), loft2.Shape());
//#else
//  BRepAlgo_Fuse result(loft1.Shape(), loft2.Shape());
//#endif
  if (IsBRepAlgoAPI) {
    di << "BRepAlgoAPI_Fuse result(loft1.Shape(), loft2.Shape())" <<"\n";
    BRepAlgoAPI_Fuse result(loft1.Shape(), loft2.Shape());
    DBRep::Set("F",result.Shape());
  } else {
    di << "BRepAlgo_Fuse result(loft1.Shape(), loft2.Shape())" <<"\n";
    BRepAlgo_Fuse result(loft1.Shape(), loft2.Shape());
    DBRep::Set("F",result.Shape());
  }

//  DBRep::Set("F",result.Shape());
  return 0;
}

#include <Geom_RectangularTrimmedSurface.hxx>
#include <GC_MakeTrimmedCone.hxx>

static Standard_Integer BUC60856(Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv )
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  gp_Ax2  Cone_Ax;                                                                
  double R1=8, R2=16, H1=20, H2=40, angle;                                       
  gp_Pnt P0(0,0,0),                                                              
  P1(0,0,20), P2(0,0,45);                                                        
  angle = 2*M_PI;                                                                  
  Handle(Geom_RectangularTrimmedSurface) S = GC_MakeTrimmedCone (P1, P2, R1, R2).Value();
  TopoDS_Shape myshape = BRepBuilderAPI_MakeFace(S, Precision::Confusion()).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(myshape);
  aContext->Display(ais1);
  aContext->SetColor(ais1, Quantity_NOC_BLUE1);
  
  Handle(Geom_RectangularTrimmedSurface) S2 = GC_MakeTrimmedCone (P1, P2,R1, 0).Value();
  TopoDS_Shape myshape2 = BRepBuilderAPI_MakeFace(S2, Precision::Confusion()).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(myshape2);
  aContext->Display(ais2);
  aContext->SetColor(ais2, Quantity_NOC_RED);
  return 0;
}

#if ! defined(WNT)
//#include <fstream.h>
#ifdef HAVE_FSTREAM
# include <fstream>
#elif defined (HAVE_FSTREAM_H)
# include <fstream.h> 
#endif
//#include <Standard_Stream.hxx>
//==========================================================================
//function : CoordLoad
//           chargement d une face dans l explorer.
//==========================================================================
static Standard_Integer coordload (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{ 
  char line[256];
  char X[30], Y[30];
  int fr;
  TopoDS_Vertex V1,V2;
  TopoDS_Edge Edge;
  TopoDS_Wire Wire;
  TopoDS_Face Face;

  if (argc < 3) return 1;

  ifstream file(argv[2], ios::in);
  if(!file)
    {
      di<<"unable to open "<<argv[2]<<" for input"<<"\n";
      return 2;
    }
  BRepBuilderAPI_MakeWire WB;

  file.getline(line,80);
  for(int i=0;i<30;i++) X[i]=Y[i]=0;
  fr = sscanf(line,"%20c%20c",&X,&Y);
  V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(atof(X),atof(Y),0.0));

  for(;;)
    {
      file.getline(line,80);
      if (!file) break;
	  for(int i=0;i<30;i++) X[i]=Y[i]=0;
	  fr = sscanf(line,"%20c%20c",&X,&Y);
	  V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(atof(X),atof(Y),0.0));
	  Edge = BRepBuilderAPI_MakeEdge(V1,V2);
	  WB.Add(Edge);
	  V1=V2;
  }
  
  file.close();
  if (WB.IsDone()) Wire = WB.Wire();
  Face = BRepBuilderAPI_MakeFace(Wire);

  DBRep::Set (argv[1],Face);
  return 0;
}
#endif

static Standard_Integer TestMem (Draw_Interpretor& /*di*/,
				 Standard_Integer /*nb*/, 
				 const char ** /*arg*/) 
{
  TCollection_ExtendedString aString(1024*1024, 'A');
  return 0;
}

static Standard_Integer BUC60876_ (Draw_Interpretor& di,
				 Standard_Integer argc, 
				 const char ** argv) 
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) { 
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }	
  if((argc != 2) && (argc != 3)) {
    di<< "usage : " << argv[0] << " shape [mode==1]" << "\n";
    return -1;
  }
  TopoDS_Shape aShape = DBRep::Get(argv[1]);
  Handle(AIS_InteractiveObject) anIO = new AIS_Shape(aShape);
//  Handle(AIS_InteractiveObject) anIOa = ViewerTest::GetAISShapeFromName(argv[1]);
  anIO->SetHilightMode((argc == 3) ? atoi(argv[2]) : 1);
  aContext->Display(anIO);
  return 0;
}

//=======================================================================
//function : buc60773
//purpose  : 
//=======================================================================

#include<TCollection_HAsciiString.hxx>

static Standard_Integer BUC60773 (Draw_Interpretor& /*di*/, Standard_Integer /*n*/, const char ** /*a*/)
{
  Handle(TCollection_HAsciiString) hAscii = new TCollection_HAsciiString();
  Standard_CString aStr = hAscii->ToCString();
  TCollection_AsciiString aAscii(aStr);  
  
  return 0;
}

#include<BRepPrimAPI_MakeCylinder.hxx>
#include<BRepPrimAPI_MakeCone.hxx>

static int TestCMD(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)

{
  if(argc > 2) {
    di << "Usage : " << argv[0] << " [BRepAlgoAPI/BRepAlgo = 1/0]" << "\n";
    return 1;
  }
  Standard_Boolean IsBRepAlgoAPI = Standard_True;
  if (argc == 2) {
    Standard_Integer IsB = atoi(argv[1]);
    if (IsB != 1) {
      IsBRepAlgoAPI = Standard_False;
#if ! defined(BRepAlgo_def01)
//      di << "Error: There is not BRepAlgo_Fuse class" << "\n";
//      return 1;
#endif
    }
  }

  //Cylindre 36.085182 20.0 8.431413 88.04671 20.0 38.931416 10.0

  Standard_Real x11 = 36.085182;
  Standard_Real y11 = 20.0;
  Standard_Real z11 = 8.431413;
  Standard_Real x12 = 88.04671;
  Standard_Real y12 = 20.0;
  Standard_Real z12 = 38.931416;
  Standard_Real radius = 10.0;

  gp_Pnt base1(x11, y11, z11);
  gp_Dir vect1(x12-x11, y12-y11, z12-z11);
  gp_Ax2 axis1(base1, vect1);
  Standard_Real height1 = sqrt( ((x12-x11)*(x12-x11)) + ((y12-y11)*(y12-y11)) + ((z12-z11)*(z12-z11)) );
  BRepPrimAPI_MakeCylinder cylinder(axis1, radius, height1);

  TopoDS_Shape SCyl = cylinder.Shape();
  DBRep::Set("cyl", SCyl);

   
  //Cone 70.7262 20.0 28.431412 105.36722 20.0 48.431416 6.0 3.0
  Standard_Real x21 = 70.7262;
  Standard_Real y21 = 20.0;
  Standard_Real z21 = 28.431412;
  Standard_Real x22 = 105.36722;
  Standard_Real y22 = 20.0;
  Standard_Real z22 = 48.431416;
  Standard_Real radius1 = 6.0;
  Standard_Real radius2 = 3.0;

  gp_Pnt base2(x21, y21, z21);
  gp_Dir vect2(x22-x21, y22-y21, z22-z21);
  gp_Ax2 axis2(base2, vect2);
  Standard_Real height2 = sqrt( ((x22-x21)*(x22-x21)) + ((y22-y21)*(y22-y21)) + ((z22-z21)*(z22-z21)) );
  BRepPrimAPI_MakeCone cone(axis2, radius1, radius2, height2);

  TopoDS_Shape SCon = cone.Shape();
  DBRep::Set("con", SCon);

//#if ! defined(BRepAlgoAPI_def01)
//  BRepAlgoAPI_Fuse SFuse(SCyl, SCon);
//#else
//  BRepAlgo_Fuse SFuse(SCyl, SCon);
//#endif
//
//  if(! SFuse.IsDone() )
//    cout<<"Error: Boolean fuse operation failed !"<<endl;
//
//  TopoDS_Shape fuse = SFuse.Shape();

  TopoDS_Shape fuse;
  if (IsBRepAlgoAPI) {
    di << "BRepAlgoAPI_Fuse SFuse(SCyl, SCon)" <<"\n";
    BRepAlgoAPI_Fuse SFuse(SCyl, SCon);
    if(! SFuse.IsDone() )
      di<<"Error: Boolean fuse operation failed !"<<"\n";
    fuse = SFuse.Shape();
  } else {
    di << "BRepAlgo_Fuse SFuse(SCyl, SCon)" <<"\n";
    BRepAlgo_Fuse SFuse(SCyl, SCon);
    if(! SFuse.IsDone() )
      di<<"Error: Boolean fuse operation failed !"<<"\n";
    fuse = SFuse.Shape();
  }

  DBRep::Set("fus", fuse);
 	
  return 0;
}

#include <Dico_DictionaryOfInteger.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TopExp.hxx>
#include <TopoDS_Iterator.hxx>

//---------------------------------------------------------------------------------------

static Standard_Integer statface (Draw_Interpretor& di,Standard_Integer /*argc*/, const char ** argv )

{  
  TopoDS_Shape aShape = DBRep::Get(argv[1]);
  if(aShape.IsNull())
  {
    di<<"Invalid input shape"<<"\n";
    return 1;
  }
  Handle(Dico_DictionaryOfInteger) aDico = new Dico_DictionaryOfInteger();
  Handle(TColStd_HSequenceOfAsciiString) aSequence = new TColStd_HSequenceOfAsciiString;
  Standard_CString aString;
  Standard_Integer i=1,j=1,k=1,l=1,aa=1;
  TopExp_Explorer expl;
  Standard_Real f3d,l3d;
  for(expl.Init(aShape,TopAbs_FACE);expl.More();expl.Next())
  {
    // SURFACES
    TopoDS_Face aFace = TopoDS::Face (expl.Current());
    Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
    aString = aSurface->DynamicType()->Name();

    if(aDico->GetItem(aString,aa) != 0)
    {
      aDico->GetItem(aString,aa);
      aDico->SetItem(aString,aa+1);
    } else  {
      aDico->SetItem(aString,1);
      aSequence->Append(aString);
      aa=1;
      i++;
    }
  }
  // PCURVES
  for(expl.Init(aShape,TopAbs_FACE);expl.More();expl.Next())
  {
    TopoDS_Face aFace = TopoDS::Face (expl.Current());
    TopoDS_Iterator anIt(aFace);
    TopoDS_Wire aWire = TopoDS::Wire (anIt.Value());
    TopoDS_Iterator it (aWire); 
    for (; it.More(); it.Next()) {
      TopoDS_Edge Edge = TopoDS::Edge (it.Value());
    Handle(Geom2d_Curve) aCurve2d = BRep_Tool::CurveOnSurface(Edge,aFace,f3d,l3d);
    aString = aCurve2d->DynamicType()->Name();
        if(aDico->GetItem(aString,aa) != 0)
        {         
          aDico->GetItem(aString,aa);
          aDico->SetItem(aString,aa+1);
        } else  {
          aDico->SetItem(aString,1);
          aSequence->Append(aString);
          i++;
          aa=1;
        }
    }
  }
  // 3d CURVES
  TopExp_Explorer exp;
  for (exp.Init(aShape,TopAbs_EDGE); exp.More(); exp.Next()) 
  {
      TopoDS_Edge Edge = TopoDS::Edge (exp.Current());
      Handle(Geom_Curve) aCurve3d = BRep_Tool::Curve (Edge,f3d,l3d);
      if(aCurve3d.IsNull())
      {
        l++;
        goto aLabel;
      }
      aString = aCurve3d->DynamicType()->Name();
        if(aDico->GetItem(aString,aa) != 0)
        {
          aDico->GetItem(aString,aa);
          aDico->SetItem(aString,aa+1);
        } else  {
          aDico->SetItem(aString,1);
          aSequence->Append(aString);
          i++;
          aa=1;
        }
    aLabel:;
    }
  // Output 
  di<<"\n";
  for(j=1;j<i;j++)
    {
      aDico->GetItem(aSequence->Value(j),aa);
      di<<aa<<"   --   "<<aSequence->Value(j).ToCString()<<"\n";
    }    

  di<<"\n";
  di<<"Degenerated edges :"<<"\n";
  di<<l<<"   --    "<<"Degenerated edges "<<"\n";

  return 0;

}

#include <BRepBuilderAPI_Transform.hxx>

static Standard_Integer BUC60841(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc > 2) {
    di << "Usage : " << argv[0] << " [BRepAlgoAPI/BRepAlgo = 1/0]" << "\n";
    return 1;
  }
  Standard_Boolean IsBRepAlgoAPI = Standard_True;
  if (argc == 2) {
    Standard_Integer IsB = atoi(argv[1]);
    if (IsB != 1) {
      IsBRepAlgoAPI = Standard_False;
#if ! defined(BRepAlgo_def01)
//      di << "Error: There is not BRepAlgo_Fuse class" << "\n";
//      return 1;
#endif
    }
  }

  gp_Ax2 Ax2 = gp_Ax2(gp_Pnt(0, 621, 78), gp_Dir(0, 1,0));
  BRepPrimAPI_MakeCylinder cyl(Ax2, 260, 150);
  //BRepPrimAPI_MakeCylinder cyl(gp_Ax2(gp_Pnt(0, 621, 78), gp_Dir(0, 1,0)), 260, 150);

  TopoDS_Shape sh1 = cyl.Shape();
  DBRep::Set("sh1",sh1);
  gp_Trsf trsf1, trsf2;
  trsf1.SetTranslation(gp_Pnt(0.000000,700.000000,-170.000000),
                       gp_Pnt(0.000000,700.000000,-95.000000));
  trsf2.SetRotation(gp_Ax1(gp_Pnt(0.000000,700.000000,-170.000000),
                           gp_Dir(0.000000,0.000000,1.000000)), 0.436111);
  BRepBuilderAPI_Transform trans1(sh1, trsf1);
  TopoDS_Shape sh2 = trans1.Shape();
  DBRep::Set("sh2",sh2);

//#if ! defined(BRepAlgoAPI_def01)
//  BRepAlgoAPI_Fuse fuse1(sh1, sh2);
//#else
//  BRepAlgo_Fuse fuse1(sh1, sh2);
//#endif
//
//  TopoDS_Shape fsh1 = fuse1.Shape();

  TopoDS_Shape fsh1;
  if (IsBRepAlgoAPI) {
    di << "BRepAlgoAPI_Fuse fuse1(sh1, sh2)" <<"\n";
    BRepAlgoAPI_Fuse fuse1(sh1, sh2);
    fsh1 = fuse1.Shape();
  } else {
    di << "BRepAlgo_Fuse fuse1(sh1, sh2)" <<"\n";
    BRepAlgo_Fuse fuse1(sh1, sh2);
    fsh1 = fuse1.Shape();
  }

  DBRep::Set("fsh1",fsh1);
  BRepBuilderAPI_Transform trans2(fsh1, trsf2);
  TopoDS_Shape sh3 = trans2.Shape();
  DBRep::Set("sh3",sh3);

//#if ! defined(BRepAlgoAPI_def01)
//  BRepAlgoAPI_Fuse fuse2(fsh1,sh3);
//#else
//  BRepAlgo_Fuse fuse2(fsh1,sh3);
//#endif
//
//  TopoDS_Shape fsh2 = fuse2.Shape();

  TopoDS_Shape fsh2;
  if (IsBRepAlgoAPI) {
    di << "BRepAlgoAPI_Fuse fuse2(fsh1,sh3)" <<"\n";
    BRepAlgoAPI_Fuse fuse2(fsh1,sh3);
    fsh2 = fuse2.Shape();
  } else {
    di << "BRepAlgo_Fuse fuse2(fsh1,sh3)" <<"\n";
    BRepAlgo_Fuse fuse2(fsh1,sh3);
    fsh2 = fuse2.Shape();
  }

  DBRep::Set("fsh2",fsh2);
  Handle_AIS_Shape aisp1 = new AIS_Shape(fsh2);
//  aContext->Display(aisp1);
  return 0;
}

#include <ShapeBuild_Edge.hxx>

static Standard_Integer BUC60874(Draw_Interpretor& /*di*/, Standard_Integer /*argc*/, const char ** argv )
{
  TopoDS_Edge e = TopoDS::Edge(DBRep::Get(argv[1],TopAbs_EDGE));
  ShapeBuild_Edge().BuildCurve3d(e);
  DBRep::Set("ED",e);
  return 0;
}


#include<TDF_Label.hxx>
#include<TDataStd_TreeNode.hxx>

#include<DDocStd.hxx>

#include<DDF.hxx>

#include<TDocStd_Modified.hxx>
#include<TDF_ListIteratorOfDeltaList.hxx>
#include<TDocStd_Document.hxx>
#include<TDocStd_Application.hxx>
#include<TDF_Delta.hxx>
#include<TDataXtd_Constraint.hxx>
#include<TPrsStd_AISPresentation.hxx>
#include<TPrsStd_AISViewer.hxx>
#include<TNaming_Builder.hxx>
#include<TNaming_Naming.hxx>
#include<TNaming_NamedShape.hxx>
  
static int BUC60817(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=2) {
    di << "Usage : " << argv[0] << " D" << "\n";
    di<<1;
    return 0;
  }

  Handle(TDF_Data) DF;
  if (!DDF::GetDF(argv[1],DF)) {di<<2;return 0;}
  
  TDF_Label L1,L2;
  Handle(TDataStd_TreeNode) TN1,TN2;

  DDF::AddLabel(DF,"0:2",L1);
  TN1 = TDataStd_TreeNode::Set(L1);

  DDF::AddLabel(DF,"0:3",L2);
  TN2 = TDataStd_TreeNode::Set(L2);

  TN1->Append(TN2);
  if(!(TN2->IsDescendant(TN1))) {di<<3;return 0;}
  if((TN1->IsDescendant(TN2))) {di<<4;return 0;}

  di<<0;
  return 0;
}

static int BUC60831_1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=2) {
    di << "Usage : " << argv[0] << " D" << "\n";
    di<<-1;
    return 0;
  }

  Handle(TDF_Data) DF;
  if (!DDF::GetDF(argv[1],DF)) {di<<-2;return 0;}
  
  TDF_Label L;
  DDF::FindLabel(DF,"0:1",L,Standard_False);
  Handle(TDocStd_Modified) MDF;
  if (!L.Root().FindAttribute (TDocStd_Modified::GetID(), MDF)) {
    MDF = new TDocStd_Modified();
    L.Root().AddAttribute(MDF);
  }

  di<<!MDF->IsEmpty();
  return 0;
}

static int BUC60831_2(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=3) {
    di << "Usage : " << argv[0] << " D Label" << "\n";
    di<<1;
    return 0;
  }

  Handle(TDF_Data) DF;
  if (!DDF::GetDF(argv[1],DF)) {di<<2;return 0;}
  
  TDF_Label L;
  DDF::FindLabel(DF,argv[2],L,Standard_False);

  TDocStd_Modified::Add(L);
  
  di<<0;
  return 0;
}

static int BUC60836(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=2) {
    di << "Usage : " << argv[0] << " D" << "\n";
    di<<1;
    return 0;
  }


  Handle(TDF_Data) aDF;
  if (!DDF::GetDF(argv[1],aDF)) {di<<2;return 0;}

  Handle(TDocStd_Document) aDocument;
  if (!DDocStd::GetDocument(argv[1], aDocument)) {di<<3;return 0;}
   
  TDF_Label L;
  Handle(TDataStd_TreeNode) TN;

  aDocument->NewCommand();
  DDF::AddLabel(aDF,"0:2",L);
  TN = TDataStd_TreeNode::Set(L);

  aDocument->NewCommand();
  DDF::AddLabel(aDF,"0:3",L);
  TN = TDataStd_TreeNode::Set(L);

  aDocument->NewCommand();
  DDF::AddLabel(aDF,"0:4",L);
  TN = TDataStd_TreeNode::Set(L);
  aDocument->NewCommand();

  TDF_DeltaList Us,Rs;
  Us = aDocument->GetUndos();
  Rs = aDocument->GetUndos();

  Standard_Integer i;
  char Names[10][5]={"n1","n2","n3","n4","n5","n6","n7","n8","n9","n10"};  

  TDF_ListIteratorOfDeltaList IDL;
  for(IDL.Initialize(Us),i=1;IDL.More();IDL.Next(),i++){
    Handle(TDF_Delta) D = IDL.Value();
    TCollection_ExtendedString S(Names[i-1]);
    D->SetName(S);
//    cout<<" U"<<i<<"="<<D->Name()<<endl;
  }
  
  aDocument->Undo();
  aDocument->Undo();
  
  Us = aDocument->GetUndos();
  Rs = aDocument->GetRedos();

  for(IDL.Initialize(Us),i=1;IDL.More();IDL.Next(),i++){
    Handle(TDF_Delta) D = IDL.Value();
//    cout<<" U"<<i<<"="<<D->Name()<<endl;
  }

  TCollection_ExtendedString n2name ("n2");
  for(IDL.Initialize(Rs),i=1;IDL.More();IDL.Next(),i++){
    Handle(TDF_Delta) D = IDL.Value();
    if ( i == 1 && ! D->Name().IsEqual (n2name) ) 
    {
      di << 4;
      return 0;
   }
  }

  di<<0;
  return 0;
}

static int BUC60847(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=3) {
    di << "Usage : " << argv[0] << " D Shape" << "\n";
    di<<1;
    return 0;
  }

  Handle(TDF_Data) aDF;
  if (!DDF::GetDF(argv[1],aDF)) {di<<2;return 0;}
  
  TopoDS_Shape s = DBRep::Get(argv[2]);
  if (s.IsNull()) { di <<"shape not found"<< "\n"; di<<3;return 0;}
  TDF_Label L;
  DDF::AddLabel(aDF, "0:2", L);
  TNaming_Builder SI (L);
  SI.Generated(s);

  Handle(TNaming_NamedShape) NS = new TNaming_NamedShape;

  TNaming_Naming aNN;
  NS=aNN.Name(L,s,s);
//  if (!NS->IsEmpty()) {di<<3;return 0;}
  if (NS->IsEmpty()) {di<<4;return 0;}
  di<<0;
  return 0;
}

static int BUC60862(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=3) {
    di << "Usage : " << argv[0] << " D Shape" << "\n";
    di<<1;
    return 0;
  }

  Handle(TDF_Data) aDF;
  if (!DDF::GetDF(argv[1],aDF)) {di<<2;return 0;}
  
  TopoDS_Shape s = DBRep::Get(argv[2]);
  if (s.IsNull()) { di <<"shape not found"<< "\n"; di<<3;return 0;}
  TDF_Label L;
  DDF::AddLabel(aDF, "0:2", L);
  TNaming_Builder SI (L);
  SI.Generated(s);

  Handle(TNaming_NamedShape) NS = new TNaming_NamedShape;

  TNaming_Naming aNN;
  NS=aNN.Name(L,s,s);
  if (NS->IsEmpty()) {di<<4;return 0;}
  di<<0;
  return 0;
}

static int BUC60867(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if (argc == 2) {
    TCollection_ExtendedString path (argv[1]); 
    Handle(TDocStd_Application) A;
    if (!DDocStd::Find(A)) {di<<1;return 0;}
    Handle(TDocStd_Document) D;
    Standard_Integer insession = A->IsInSession(path);
    if (insession > 0) {  
      di <<"document " << insession << "  is already in session" << "\n";
      di<<2;
      return 0;
    }
    PCDM_ReaderStatus Result = A->Open(path,D);
    if(Result==PCDM_RS_OK){
      di<<0;
      return 0; 
    }
  }
  di<<3;
  return 0;
}

static int BUC60910(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=2) {
    di << "Usage : " << argv[0] << " D" << "\n";
    di<<1;
    return 0;
  }

  Handle(TDF_Data) aDF;
  if (!DDF::GetDF(argv[1],aDF)) {di<<2;return 0;}
  
  TDF_Label L;
  DDF::AddLabel(aDF, "0:2", L);
 
  Handle(TPrsStd_AISPresentation) AISP = new TPrsStd_AISPresentation;

  AISP->Set(L,TDataXtd_Constraint::GetID());

  if (AISP->HasOwnMode()) {di<<3;return 0;}
  AISP->SetMode(3);
  Standard_Integer Mode = AISP->Mode();
  if (Mode!=3) {di<<4;return 0;}
  if (!AISP->HasOwnMode()) {di<<5;return 0;}
  AISP->UnsetMode();
  if (AISP->HasOwnMode()) {di<<6;return 0;}
  di<<0;
  return 0;
}

static int BUC60925(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=2) {
    di << "Usage : " << argv[0] << " D" << "\n";
    di<<1;
    return 0;
  }

  Handle(TDF_Data) aDF;
  if (!DDF::GetDF(argv[1],aDF)) {di<<2;return 0;}
  
  TDF_Label L;
  DDF::AddLabel(aDF, "0:2", L);
  TDF_LabelMap LM;
  LM.Add(L);
  
  Handle(TNaming_NamedShape) NS = new TNaming_NamedShape;
//  Handle(TNaming_Name) NN = new TNaming_Name;
  TNaming_Name NN;

  NN.Type(TNaming_IDENTITY);
  NN.Append(NS);
  Standard_Boolean Res = NN.Solve(L,LM);
  
  if (Res!=Standard_False) {di<<3;return 0;}
  di<<0;
  return 0;
}

static int BUC60932(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc!=2) {
    di << "Usage : " << argv[0] << " D" << "\n";
    di<<1;
    return 0;
  }


  Handle(TDocStd_Document) aDocument;
  if (!DDocStd::GetDocument(argv[1], aDocument)) {di<<2;return 0;}
   
  if(!aDocument->InitDeltaCompaction()) {di<<3;return 0;}
  if(!aDocument->PerformDeltaCompaction()) {di<<4;return 0;}

  di<<0;
  return 0;
}

//=======================================================================
//function : AISWidth
//purpose  : AISWidth (DOC,entry,[width])
// abv: testing command for checking bug BUC60917 in TPrsStd_AISPresentation
//=======================================================================

static int AISWidth(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {

  if (argc >= 3) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(argv[1],D)) {di<<(-1);return 0;}
    TDF_Label L;
    if (!DDF::FindLabel(D->GetData(),argv[2],L)) {di<<(-2);return 0;}

    Handle(TPrsStd_AISViewer) viewer;
    if( !TPrsStd_AISViewer::Find(L, viewer) ) {di<<(-3);return 0;}

    Handle(TPrsStd_AISPresentation) prs;
    if(L.FindAttribute( TPrsStd_AISPresentation::GetID(), prs) ) {   
      if( argc == 4 ) {
        prs->SetWidth(atof(argv[3]));
        TPrsStd_AISViewer::Update(L);
      }
      else {
       if (prs->HasOwnWidth()){ 
//         cout << "Width = " << prs->Width() << endl;
         di<<prs->Width();
       }
       else{
         di << "AISWidth: Warning : Width wasn't set" << "\n";
         di<<(-4);
       }
      }
      return 0;
    }
  }
  di << "AISWidth : Error"   << "\n";
  di<<(-5);
  return 0;
}

//=======================================================================
//function : BUC60921 ( & BUC60954 )
//purpose  : Test memory allocation of OCAF in Undo/Redo operations
//=======================================================================

static Standard_Integer BUC60921 (Draw_Interpretor& di,
                                  Standard_Integer nb, 
                                  const char ** arg) 
{
  if (nb >= 4) {     
    Handle(TDocStd_Document) D;
    if (!DDocStd::GetDocument(arg[1],D)) {di<<1;return 0;}
    TDF_Label L;
    DDF::AddLabel(D->GetData(),arg[2],L);

    BRep_Builder B;
    TopoDS_Shape S;
    BRepTools::Read ( S, arg[3], B );
    
    TNaming_Builder tnBuild(L);
    tnBuild.Generated(S);
//    di << "File " << arg[3] << " added";
    di<<0;
    return 0;
  }
  di << "BUC60921 Doc label brep_file: directly read brep file and put shape to the label"   << "\n";
  di<<2;
  return 0;
}

#include<IGESControl_Reader.hxx>
#include<BRepPrimAPI_MakeHalfSpace.hxx>

static Standard_Integer BUC60951_(Draw_Interpretor& di, Standard_Integer argc, const char ** a)
{
  //if(argc!=2)
  //  {
  //    cerr << "Usage : " << a[0] << " file.igs" << endl;
  //    return -1;
  //  }
  if(argc < 2 || argc > 3) {
    di << "Usage : " << a[0] << " [BRepAlgoAPI/BRepAlgo = 1/0]" << "\n";
    return 1;
  }
  Standard_Boolean IsBRepAlgoAPI = Standard_True;
  if (argc == 3) {
    Standard_Integer IsB = atoi(a[2]);
    if (IsB != 1) {
      IsBRepAlgoAPI = Standard_False;
#if ! defined(BRepAlgo_def01)
//      di << "Error: There is not BRepAlgo_Fuse class" << "\n";
//      return 1;
#endif
    }
  }
  
  Handle(AIS_InteractiveContext) myContext = ViewerTest::GetAISContext(); 

  if(myContext.IsNull()) {
    di << "use 'vinit' command before " << a[0] << "\n";
    return -1;
  }

//  IGESControlStd_Reader reader;
  IGESControl_Reader reader;
  reader.ReadFile(a[1]);
  reader.TransferRoots();
  TopoDS_Shape shape = reader.OneShape();
  printf("\n iges1 shape type = %d", shape.ShapeType() );
  TopTools_IndexedMapOfShape list;
  TopExp::MapShapes(shape, TopAbs_FACE, list);
  printf("\n No. of faces = %d", list.Extent());

  TopoDS_Shell shell;
  BRep_Builder builder;
  builder.MakeShell(shell);
  for(int i=1;i<=list.Extent(); i++) { 
    TopoDS_Face face = TopoDS::Face(list.FindKey(i));
    builder.Add(shell, face);
  }

  BRepPrimAPI_MakeHalfSpace half(shell, gp_Pnt(0, 0, 20));
  TopoDS_Solid sol = half.Solid();
  gp_Ax2 anAx2(gp_Pnt(-800.0, 0.0, 0), gp_Dir(0, 0, -1));
  BRepPrimAPI_MakeCylinder cyl(anAx2, 50, 300);
  TopoDS_Shape sh = cyl.Shape();

//#if ! defined(BRepAlgoAPI_def01)
//  BRepAlgoAPI_Fuse fuse(sol, sh);
//#else
//  BRepAlgo_Fuse fuse(sol, sh);
//#endif
//
//  sh = fuse.Shape();

  if (IsBRepAlgoAPI) {
    di << "BRepAlgoAPI_Fuse fuse(sol, sh)" <<"\n";
    BRepAlgoAPI_Fuse fuse(sol, sh);
    sh = fuse.Shape();
  } else {
    di << "BRepAlgo_Fuse fuse(sol, sh)" <<"\n";
    BRepAlgo_Fuse fuse(sol, sh);
    sh = fuse.Shape();
  }

  Handle(AIS_Shape) res = new AIS_Shape(sh);
  myContext->Display( res );
  return 0;
}

void QABugs::Commands_3(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add("BUC60623","BUC60623 result Shape1 Shape2",__FILE__,BUC60623,group);
  theCommands.Add("BUC60569","BUC60569 shape",__FILE__,BUC60569,group);
  theCommands.Add("BUC60614","BUC60614 shape",__FILE__,BUC60614,group);
  theCommands.Add("BUC60609","BUC60609 shape name [U V]",__FILE__,BUC60609,group);
#if ! defined(WNT)
  theCommands.Add("UKI61075","UKI61075",__FILE__,UKI61075,group);
#endif
  theCommands.Add("BUC60585",St,__FILE__,BUC60585,group);
  theCommands.Add("BUC60547","BUC60547 name",__FILE__,BUC60547,group);
  theCommands.Add("BUC60632","BUC60632 mode length",__FILE__,BUC60632,group);
  theCommands.Add("BUC60652","BUC60652 face",__FILE__,BUC60652,group);
  theCommands.Add("ksection","ksection resultat shell1 shell2 NbPntMax Toler3d Toler2d RelativeTol",__FILE__,ksection,group);
  theCommands.Add("BUC60682","ksection resultat shell1 shell2 NbPntMax Toler3d Toler2d RelativeTol",__FILE__,ksection,group);  
  theCommands.Add("BUC60669","ksection resultat shell1 shell2 NbPntMax Toler3d Toler2d RelativeTol",__FILE__,ksection,group);  
  theCommands.Add("PRO19626","ksection resultat shell1 shell2 NbPntMax Toler3d Toler2d RelativeTol",__FILE__,ksection,group);  
  theCommands.Add("BUC60574","BUC60574 ",__FILE__,BUC60574,group);

  theCommands.Add("BUC60698","BUC60698 [BRepAlgoAPI/BRepAlgo = 1/0]",__FILE__,BUC60698,group);

  theCommands.Add("BUC60699","BUC60699 ",__FILE__,BUC60699,group);
  theCommands.Add("GER61394","GER61394 [1/0]",__FILE__,GER61394,group);
  theCommands.Add("GER61351","GER61351 name/object name/r g b/object r g b",__FILE__,setcolor,group);
  theCommands.Add("setcolor","setcolor name/object name/r g b/object r g b",__FILE__,setcolor,group);

  theCommands.Add("BUC60726","BUC60726 0/1",__FILE__,BUC60726,group);
  theCommands.Add("BUC60729","BUC60729",__FILE__,BUC60729,group);
  theCommands.Add("BUC60724","BUC60724",__FILE__,BUC60724,group);
  theCommands.Add("BUC60727","BUC60727",__FILE__,BUC60727,group);
  theCommands.Add("BUC60792","BUC60792",__FILE__,BUC60792,group);
  theCommands.Add("BUC60811","BUC60811",__FILE__,BUC60811,group);

  theCommands.Add("BUC60825","BUC60825",__FILE__,BUC60825,group);

  theCommands.Add("OCC10006","OCC10006 [BRepAlgoAPI/BRepAlgo = 1/0]",__FILE__,OCC10006,group);

  theCommands.Add("BUC60856","BUC60856",__FILE__,BUC60856,group);

#if ! defined(WNT)
  theCommands.Add("coordload","load coord from file",__FILE__,coordload);
#endif

  theCommands.Add("TestMem","TestMem",__FILE__,TestMem,group);
  theCommands.Add("BUC60945","BUC60945",__FILE__,TestMem,group);
  theCommands.Add("BUC60876","BUC60876 shape",__FILE__,BUC60876_,group); 
  theCommands.Add("BUC60773","BUC60773",__FILE__,BUC60773,group); 

  theCommands.Add("TestCMD","TestCMD [BRepAlgoAPI/BRepAlgo = 1/0]",__FILE__,TestCMD,group);

  theCommands.Add("statface","statface face",__FILE__,statface,group);

  theCommands.Add("BUC60841","BUC60841 [BRepAlgoAPI/BRepAlgo = 1/0]",__FILE__,BUC60841,group);

  theCommands.Add("BUC60874","BUC60874",__FILE__,BUC60874,group);

  theCommands.Add("BUC60817","BUC60817 D",__FILE__,BUC60817,group);
  theCommands.Add("BUC60831_1","BUC60831_1 D",__FILE__,BUC60831_1,group);
  theCommands.Add("BUC60831_2","BUC60831_2 D Label",__FILE__,BUC60831_2,group);
  theCommands.Add("BUC60836","BUC60836 D",__FILE__,BUC60836,group);
  theCommands.Add("BUC60847","BUC60847 D Shape",__FILE__,BUC60847,group);
  theCommands.Add("BUC60862","BUC60862 D Shape",__FILE__,BUC60862,group);
  theCommands.Add("BUC60867","BUC60867",__FILE__,BUC60867,group);
  theCommands.Add("BUC60910","BUC60910 D",__FILE__,BUC60910,group);
  theCommands.Add("BUC60925","BUC60925 D",__FILE__,BUC60925,group);
  theCommands.Add("BUC60932","BUC60932 D",__FILE__,BUC60932,group);
  theCommands.Add("AISWidth","AISWidth (DOC,entry,[width])",__FILE__,AISWidth,group);
  theCommands.Add("BUC60921","BUC60921 Doc label brep_file",__FILE__,BUC60921,group);

  theCommands.Add("BUC60951","BUC60951 file.igs [BRepAlgoAPI/BRepAlgo = 1/0]",__FILE__,BUC60951_, group );

}
