// File:	QADBMReflex.cxx
// Created:	Mon Oct  7 14:56:07 2002
// Author:	QA Admin
//		<qa@russox>


#include <QADBMReflex.ixx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>


//=======================================================================
//function : GetMaterialFromName
//purpose  : get the Graphic3d_NameOfMaterial from a string
//=======================================================================

#define DEFAULT_MATERIAL Graphic3d_NOM_BRASS
static Graphic3d_NameOfMaterial GetMaterialFromName( const char *name, Draw_Interpretor& di ) 
{ 
  Graphic3d_NameOfMaterial mat = DEFAULT_MATERIAL;
  
  if      ( !strcasecmp(name,"BRASS" ) ) 	 mat = Graphic3d_NOM_BRASS;
  else if ( !strcasecmp(name,"BRONZE" ) )        mat = Graphic3d_NOM_BRONZE;
  else if ( !strcasecmp(name,"COPPER" ) ) 	 mat = Graphic3d_NOM_COPPER;
  else if ( !strcasecmp(name,"GOLD" ) ) 	 mat = Graphic3d_NOM_GOLD;
  else if ( !strcasecmp(name,"PEWTER" ) ) 	 mat = Graphic3d_NOM_PEWTER;
  else if ( !strcasecmp(name,"SILVER" ) ) 	 mat = Graphic3d_NOM_SILVER;
  else if ( !strcasecmp(name,"STEEL" ) ) 	 mat = Graphic3d_NOM_STEEL;
  else if ( !strcasecmp(name,"METALIZED" ) ) 	 mat = Graphic3d_NOM_METALIZED;
  else if ( !strcasecmp(name,"STONE" ) ) 	 mat = Graphic3d_NOM_STONE;
  else if ( !strcasecmp(name,"CHROME" ) )	 mat = Graphic3d_NOM_CHROME;
  else if ( !strcasecmp(name,"ALUMINIUM" ) )     mat = Graphic3d_NOM_ALUMINIUM;
  else if ( !strcasecmp(name,"NEON_PHC" ) )	 mat = Graphic3d_NOM_NEON_PHC;
  else if ( !strcasecmp(name,"NEON_GNC" ) )	 mat = Graphic3d_NOM_NEON_GNC;
  else if ( !strcasecmp(name,"PLASTER" ) )	 mat = Graphic3d_NOM_PLASTER;
  else if ( !strcasecmp(name,"SHINY_PLASTIC" ) ) mat = Graphic3d_NOM_SHINY_PLASTIC;
  else if ( !strcasecmp(name,"SATIN" ) )	 mat = Graphic3d_NOM_SATIN;
  else if ( !strcasecmp(name,"PLASTIC" ) )	 mat = Graphic3d_NOM_PLASTIC;
  else if ( !strcasecmp(name,"OBSIDIAN" ) )	 mat = Graphic3d_NOM_OBSIDIAN;
  else if ( !strcasecmp(name,"JADE" ) )	         mat = Graphic3d_NOM_JADE;

#ifdef DEB
  di << " materiau" << name <<" "<<Standard_Integer(mat) << "\n";
#endif
  return mat;
}

#include <QADBMReflex_OCC749PrsUseVertex.hxx>
#include <QADBMReflex_OCC749PrsUseVertexC.hxx>
static Standard_Integer OCC749 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0];
    return 1;
  }

  //if(!(argc == 14 || argc == 17)) {
  //  cerr << "Usage : " << argv[0] << " Reflection(=0/1) UseVertexC(=0/1) Timer(=0/1) XCount YCount BoxSize R1 G1 B1 R2 G2 B2 NameOfMaterial [R3 G3 B3]" << endl;
  if(!(argc == 14)) {
    di << "Usage : " << argv[0] << " Reflection(=0/1) UseVertexC(=0/1) Timer(=0/1) XCount YCount BoxSize R1 G1 B1 R2 G2 B2 NameOfMaterial" << "\n";
    return -1;
  }

  Standard_Integer IntegerReflection = atoi(argv[1]);
  Standard_Boolean Reflection;
  if (IntegerReflection == 0) {
    Reflection = Standard_False;
  } else {
    Reflection = Standard_True;
  }

  Standard_Integer IntegerUseVertexC = atoi(argv[2]);
  Standard_Boolean UseVertexC;
  if (IntegerUseVertexC == 0) {
    UseVertexC = Standard_False;
  } else {
    UseVertexC = Standard_True;
  }

  Standard_Integer IntegerTimer = atoi(argv[3]);
  Standard_Boolean Timer;
  if (IntegerTimer == 0) {
    Timer = Standard_False;
  } else {
    Timer = Standard_True;
  }

  Standard_Integer XCount = atoi(argv[4]);
  Standard_Integer YCount = atoi(argv[5]);
  Standard_Integer BoxSize = atoi(argv[6]);

  Quantity_Parameter R1,G1,B1,R2,G2,B2;
  Quantity_Parameter R3,G3,B3;
  Graphic3d_MaterialAspect MaterialAspect;
  Standard_Boolean Material;

  Standard_Integer R1_Integer = atoi(argv[7]);
  Standard_Integer G1_Integer = atoi(argv[8]);
  Standard_Integer B1_Integer = atoi(argv[9]);
  Standard_Integer R2_Integer = atoi(argv[10]);
  Standard_Integer G2_Integer = atoi(argv[11]);
  Standard_Integer B2_Integer = atoi(argv[12]);
  R1 = R1_Integer / 255.;
  G1 = G1_Integer / 255.;
  B1 = B1_Integer / 255.;
  R2 = R2_Integer / 255.;
  G2 = G2_Integer / 255.;
  B2 = B2_Integer / 255.;
  di << "RED1 : " << R1  << " GREEN1 : " << G1  << " BLUE1 : " << B1 <<"\n";
  di << "RED2 : " << R2  << " GREEN2 : " << G2  << " BLUE2 : " << B2 <<"\n";

  MaterialAspect = GetMaterialFromName(argv[13],di);
  Material = Standard_True;

  Quantity_Parameter MaterialR,MaterialG,MaterialB;
  MaterialR = MaterialAspect.Color().Red();
  MaterialG = MaterialAspect.Color().Green();
  MaterialB = MaterialAspect.Color().Blue();
  di << "MaterialRED : " << MaterialR  << " MaterialGREEN : " << MaterialG  << " MaterialBLUE : " << MaterialB <<"\n";

  Standard_Integer R3_Integer,G3_Integer,B3_Integer;
  if(argc == 17) {
    R3_Integer = atoi(argv[14]);
    G3_Integer = atoi(argv[15]);
    B3_Integer = atoi(argv[16]);
  } else {
    R3_Integer = R2_Integer;
    G3_Integer = G2_Integer;
    B3_Integer = B2_Integer;
  }
  R3 = R3_Integer / 255.;
  G3 = G3_Integer / 255.;
  B3 = B3_Integer / 255.;
  if(argc == 17) {
    di << "RED3 : " << R3  << " GREEN3 : " << G3  << " BLUE3 : " << B3 <<"\n";
  }

  myAISContext->EraseAll(Standard_False);
  if (UseVertexC) {
    Handle(QADBMReflex_OCC749PrsUseVertexC) anIntearactiveObject;
    anIntearactiveObject = new QADBMReflex_OCC749PrsUseVertexC(Reflection,
							       Quantity_Color(R1, G1, B1, Quantity_TOC_RGB),
							       Quantity_Color(R2, G2, B2, Quantity_TOC_RGB),
							       Quantity_Color(R3, G3, B3, Quantity_TOC_RGB),
							       XCount,
							       YCount,
							       BoxSize,
							       MaterialAspect,
							       Material,
							       Timer);
    myAISContext->Display(anIntearactiveObject);
  } else {
    Handle(QADBMReflex_OCC749PrsUseVertex) anIntearactiveObject;
    anIntearactiveObject = new QADBMReflex_OCC749PrsUseVertex(Reflection,
							      Quantity_Color(R1, G1, B1, Quantity_TOC_RGB),
							      Quantity_Color(R2, G2, B2, Quantity_TOC_RGB),
							      Quantity_Color(R3, G3, B3, Quantity_TOC_RGB),
							      XCount,
							      YCount,
							      BoxSize,
							      MaterialAspect,
							      Material,
							      Timer);
    myAISContext->Display(anIntearactiveObject);
  }
  return 0;
}

#include <QADBMReflex_OCC749PrsUseVertexABV.hxx>
#include <QADBMReflex_OCC749PrsUseVertexCABV.hxx>
static Standard_Integer OCC749_abv (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{

  Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
  if(myAISContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0];
    return 1;
  }

  if(!(argc == 13)) {
    di << "Usage : " << argv[0] << " Reflection(=0/1) UseVertexC(=0/1) Timer(=0/1) XCount YCount R1 G1 B1 R2 G2 B2 NameOfMaterial" << "\n";
    return -1;
  }

  Standard_Integer IntegerReflection = atoi(argv[1]);
  Standard_Boolean Reflection;
  if (IntegerReflection == 0) {
    Reflection = Standard_False;
  } else {
    Reflection = Standard_True;
  }

  Standard_Integer IntegerUseVertexC = atoi(argv[2]);
  Standard_Boolean UseVertexC;
  if (IntegerUseVertexC == 0) {
    UseVertexC = Standard_False;
  } else {
    UseVertexC = Standard_True;
  }

  Standard_Integer IntegerTimer = atoi(argv[3]);
  Standard_Boolean Timer;
  if (IntegerTimer == 0) {
    Timer = Standard_False;
  } else {
    Timer = Standard_True;
  }

  Standard_Integer XCount = atoi(argv[4]);
  Standard_Integer YCount = atoi(argv[5]);
  //Standard_Integer BoxSize = atoi(argv[6]);
  Standard_Integer BoxSize = 1;

  Quantity_Parameter R1,G1,B1,R2,G2,B2;
  Quantity_Parameter R3,G3,B3;
  Graphic3d_MaterialAspect MaterialAspect;
  Standard_Boolean Material;

  //Standard_Integer R1_Integer = atoi(argv[7]);
  //Standard_Integer G1_Integer = atoi(argv[8]);
  //Standard_Integer B1_Integer = atoi(argv[9]);
  //Standard_Integer R2_Integer = atoi(argv[10]);
  //Standard_Integer G2_Integer = atoi(argv[11]);
  //Standard_Integer B2_Integer = atoi(argv[12]);
  Standard_Integer R1_Integer = atoi(argv[6]);
  Standard_Integer G1_Integer = atoi(argv[7]);
  Standard_Integer B1_Integer = atoi(argv[8]);
  Standard_Integer R2_Integer = atoi(argv[9]);
  Standard_Integer G2_Integer = atoi(argv[10]);
  Standard_Integer B2_Integer = atoi(argv[11]);
  R1 = R1_Integer / 255.;
  G1 = G1_Integer / 255.;
  B1 = B1_Integer / 255.;
  R2 = R2_Integer / 255.;
  G2 = G2_Integer / 255.;
  B2 = B2_Integer / 255.;
  di << "RED1 : " << R1  << " GREEN1 : " << G1  << " BLUE1 : " << B1 <<"\n";
  di << "RED2 : " << R2  << " GREEN2 : " << G2  << " BLUE2 : " << B2 <<"\n";

  //MaterialAspect = GetMaterialFromName(argv[13]);
  MaterialAspect = GetMaterialFromName(argv[12],di);
  Material = Standard_True;

  Quantity_Parameter MaterialR,MaterialG,MaterialB;
  MaterialR = MaterialAspect.Color().Red();
  MaterialG = MaterialAspect.Color().Green();
  MaterialB = MaterialAspect.Color().Blue();
  di << "MaterialRED : " << MaterialR  << " MaterialGREEN : " << MaterialG  << " MaterialBLUE : " << MaterialB <<"\n";

  Standard_Integer R3_Integer,G3_Integer,B3_Integer;
  if(argc == 17) {
    R3_Integer = atoi(argv[14]);
    G3_Integer = atoi(argv[15]);
    B3_Integer = atoi(argv[16]);
  } else {
    R3_Integer = R2_Integer;
    G3_Integer = G2_Integer;
    B3_Integer = B2_Integer;
  }
  R3 = R3_Integer / 255.;
  G3 = G3_Integer / 255.;
  B3 = B3_Integer / 255.;
  if(argc == 17) {
    di << "RED3 : " << R3  << " GREEN3 : " << G3  << " BLUE3 : " << B3 <<"\n";
  }

  myAISContext->EraseAll(Standard_False);
  if (UseVertexC) {
    Handle(QADBMReflex_OCC749PrsUseVertexCABV) anIntearactiveObject;
    anIntearactiveObject = new QADBMReflex_OCC749PrsUseVertexCABV(Reflection,
								  Quantity_Color(R1, G1, B1, Quantity_TOC_RGB),
								  Quantity_Color(R2, G2, B2, Quantity_TOC_RGB),
								  Quantity_Color(R3, G3, B3, Quantity_TOC_RGB),
								  XCount,
								  YCount,
								  BoxSize,
								  MaterialAspect,
								  Material,
								  Timer);
    myAISContext->Display(anIntearactiveObject);
  } else {
    Handle(QADBMReflex_OCC749PrsUseVertexABV) anIntearactiveObject;
    anIntearactiveObject = new QADBMReflex_OCC749PrsUseVertexABV(Reflection,
								 Quantity_Color(R1, G1, B1, Quantity_TOC_RGB),
								 Quantity_Color(R2, G2, B2, Quantity_TOC_RGB),
								 Quantity_Color(R3, G3, B3, Quantity_TOC_RGB),
								 XCount,
								 YCount,
								 BoxSize,
								 MaterialAspect,
								 Material,
								 Timer);
    myAISContext->Display(anIntearactiveObject);
  }
  return 0;
}

void QADBMReflex::Commands(Draw_Interpretor& theCommands) {
  char *group = "QADBMReflex";

  //theCommands.Add("OCC749", "OCC749 Reflection(=0/1) UseVertexC(=0/1) Timer(=0/1) XCount YCount BoxSize R1 G1 B1 R2 G2 B2 NameOfMaterial [R3 G3 B3]", __FILE__, OCC749, group);
  theCommands.Add("OCC749", "OCC749 Reflection(=0/1) UseVertexC(=0/1) Timer(=0/1) XCount YCount BoxSize R1 G1 B1 R2 G2 B2 NameOfMaterial", __FILE__, OCC749, group);
  theCommands.Add("OCC749_abv", "OCC749_abv Reflection(=0/1) UseVertexC(=0/1) Timer(=0/1) XCount YCount R1 G1 B1 R2 G2 B2 NameOfMaterial", __FILE__, OCC749_abv, group);
  return;
}
