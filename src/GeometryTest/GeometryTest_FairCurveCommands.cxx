// File:	GeometryTest_FairCurveCommands.cxx
// Created:	Wed Oct  9 11:09:12 1996
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <GeometryTest.ixx>

#include <Draw.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Point.hxx>
#include <gp_Pnt2d.hxx>
#include <FairCurve_AnalysisCode.hxx>
#include <FairCurve_Batten.hxx>
#include <FairCurve_MinimalVariation.hxx>
#include <DrawFairCurve_Batten.hxx>
#include <DrawFairCurve_MinimalVariation.hxx>
#include <string.h>

//=======================================================================
Standard_Boolean IsGoodNumber(Standard_Integer argc, Standard_Integer waiting, Draw_Interpretor& di)
//=======================================================================
{
  // argc vaut 1 de plus, puisque argv[0] contient le nom de la commande
  if (argc != (waiting+1))
    {
      di << "Waiting "<< waiting << " arguments" << "\n";
      return Standard_False;
    }
  else
    return Standard_True;
}


//=======================================================================
static Standard_Integer 
BattenCurve(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,6,di)) return 1;

  const char *cp1 = argv[1];
  const char *cp2 = argv[2];
  const char *cangle1 = argv[3];
  const char *cangle2 = argv[4];
  const char *cheigth = argv[5];
  const char *BattenName = argv[6];

  Standard_Boolean Ok;
  FairCurve_AnalysisCode Iana;
  Standard_Real a1 = atof(cangle1),
                a2 = atof(cangle2),
                h  = atof(cheigth);
  gp_Pnt2d P1, P2;


  if (! DrawTrSurf::GetPoint2d(cp1, P1) ) return 1;
  if (! DrawTrSurf::GetPoint2d(cp2, P2) ) return 1;

  FairCurve_Batten* Bat = new FairCurve_Batten (P1, P2, h);
  Bat->SetAngle1(a1*PI/180);
  Bat->SetAngle2(a2*PI/180);
  
  Ok = Bat->Compute(Iana);
  
  Handle(DrawFairCurve_Batten) aBatten = new DrawFairCurve_Batten(Bat);

  if (aBatten.IsNull()) {
	di << " Batten null "<< "\n";
	return 1;
      }
  Draw::Set(BattenName,aBatten);

  return 0;
 // !!! Delete of Bat have to be make in DrawFairCurve_Batten destructor !!!!! 
}  


//=======================================================================
static Standard_Integer 
MVCurve(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,6,di)) return 1;

  const char *cp1 = argv[1];
  const char *cp2 = argv[2];
  const char *cangle1 = argv[3];
  const char *cangle2 = argv[4];
  const char *cheigth = argv[5];
  const char *MVCName = argv[6];

  Standard_Boolean Ok;
  FairCurve_AnalysisCode Iana;
  Standard_Real a1 = atof(cangle1),
                a2 = atof(cangle2),
                h  = atof(cheigth);
  gp_Pnt2d P1, P2;


  if (! DrawTrSurf::GetPoint2d(cp1, P1) ) return 1;
  if (! DrawTrSurf::GetPoint2d(cp2, P2) ) return 1;

  FairCurve_MinimalVariation* MVC = new  FairCurve_MinimalVariation (P1, P2, h);
  MVC->SetAngle1(a1*PI/180);
  MVC->SetAngle2(a2*PI/180);
  
  Ok = MVC->Compute(Iana);
  
  Handle(DrawFairCurve_MinimalVariation) aMVC = new DrawFairCurve_MinimalVariation(MVC);

  if (aMVC.IsNull()) {
	di << " MVC null "<< "\n";
	return 1;
      }
  Draw::Set(MVCName, aMVC);

  return 0;  
 // !!! Delete of Bat have to be make in DrawFairCurve_MinimalVariation destructor !!!!! 
}  

//=======================================================================
static Standard_Integer 
SetPoint(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,3,di)) return 1;

  const char *side = argv[1];
  const char *PointName = argv[2];
  const char *BattenName = argv[3];

  Standard_Integer cote = atoi(side);

  Handle(DrawTrSurf_Point)
     Pnt =  Handle(DrawTrSurf_Point)::DownCast(Draw::Get(PointName));
  if (Pnt.IsNull()) return 1;  
 
 
  Handle(DrawFairCurve_Batten) 
    Bat = Handle(DrawFairCurve_Batten)::DownCast(Draw::Get(BattenName));
  if (Bat.IsNull()) return 1;  
 
  Bat->SetPoint(cote, Pnt->Point2d());
  Draw::Repaint();
  return 0;
}

//=======================================================================
static Standard_Integer 
SetAngle(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,3,di)) return 1;

  const char *side = argv[1];
  const char *val = argv[2];
  const char *BattenName = argv[3];

  Standard_Real angle = atof(val); 
  Standard_Integer cote = atoi(side);
 
  Handle(DrawFairCurve_Batten) 
    Bat = Handle(DrawFairCurve_Batten)::DownCast(Draw::Get(BattenName));
  if (Bat.IsNull()) return 1;  
 
  Bat->SetAngle(cote, angle);
  Draw::Repaint();
  return 0;
}


//=======================================================================
static Standard_Integer 
SetCurvature(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,3,di)) return 1;

  const char *side = argv[1];
  const char *val = argv[2];
  const char *MVCName = argv[3];

  Standard_Real rho = atof(val); 
  Standard_Integer cote = atoi(side);
 
  Handle(DrawFairCurve_MinimalVariation) 
    MVC = Handle(DrawFairCurve_MinimalVariation)::DownCast(Draw::Get(MVCName));
  if (MVC.IsNull()) return 1;  
 
  MVC->SetCurvature(cote, rho);
  Draw::Repaint(); 
  return 0;
}



//=======================================================================
static Standard_Integer 
SetSlide(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,2,di)) return 1;

  const char *val = argv[1];
  const char *BattenName = argv[2];

  Standard_Real slide = atof(val); 
 
  Handle(DrawFairCurve_Batten) 
    Bat = Handle(DrawFairCurve_Batten)::DownCast(Draw::Get(BattenName));
  if (Bat.IsNull()) return 1;  

  Bat->SetSliding(slide);
  Draw::Repaint();
  return 0;
}


//=======================================================================
static Standard_Integer 
FreeAngle(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,2,di)) return 1;

  const char *side = argv[1];
  const char *BattenName = argv[2];
 
  Standard_Integer cote = atoi(side);
 
  Handle(DrawFairCurve_Batten) 
    Bat = Handle(DrawFairCurve_Batten)::DownCast(Draw::Get(BattenName));
  if (Bat.IsNull()) return 1;  
 
  Bat->FreeAngle(cote);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static Standard_Integer 
FreeCurvature(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,2,di)) return 1;

  const char *side = argv[1];
  const char *MVCName = argv[2];
 
  Standard_Integer cote = atoi(side);
 
  Handle(DrawFairCurve_MinimalVariation) 
    MVC = Handle(DrawFairCurve_MinimalVariation)::DownCast(Draw::Get(MVCName));
  if (MVC.IsNull()) return 1;  
 
  MVC->FreeCurvature(cote);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static Standard_Integer 
FreeSlide(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,1,di)) return 1;

  const char *BattenName = argv[1];
 
  Handle(DrawFairCurve_Batten) 
    Bat = Handle(DrawFairCurve_Batten)::DownCast(Draw::Get(BattenName));
  if (Bat.IsNull()) return 1;  
 
  Bat->FreeSliding();
  Draw::Repaint();
  return 0;
}

//=======================================================================
static Standard_Integer 
SetHeight(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,2,di)) return 1;

  const char *val = argv[1];
  const char *BattenName = argv[2];

  Standard_Real Height = atof(val); 
 
  Handle(DrawFairCurve_Batten) 
    Bat = Handle(DrawFairCurve_Batten)::DownCast(Draw::Get(BattenName));
  if (Bat.IsNull()) return 1;  
 
  Bat->SetHeight(Height);
  Draw::Repaint();
  return 0;
}

//=======================================================================
static Standard_Integer 
SetSlope(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,2,di)) return 1;

  const char *val = argv[1];
  const char *BattenName = argv[2];

  Standard_Real Slope = atof(val); 
 
  Handle(DrawFairCurve_Batten) 
    Bat = Handle(DrawFairCurve_Batten)::DownCast(Draw::Get(BattenName));
  if (Bat.IsNull()) return 1;  
 
  Bat->SetSlope(Slope);
  Draw::Repaint();
  return 0;
}
//=======================================================================
static Standard_Integer 
SetPhysicalRatio(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
//=======================================================================
{
   if (!IsGoodNumber(argc,2,di)) return 1;

  const char *val = argv[1];
  const char *MVCName = argv[2];

  Standard_Real ratio = atof(val); 
 
  Handle(DrawFairCurve_MinimalVariation) 
      MVC = Handle(DrawFairCurve_MinimalVariation)::DownCast(Draw::Get(MVCName));
  if (MVC.IsNull()) return 1;  
 
  MVC->SetPhysicalRatio(ratio);
  Draw::Repaint();
  return 0;
}


//=======================================================================
void GeometryTest::FairCurveCommands(Draw_Interpretor& TheCommande) 
//=======================================================================

{ 
  const char* g;

  g = "FairCurve command";
  
  TheCommande.Add("battencurve","battencurve P1 P2 Angle1 Angle2 Heigth BattenName", 
                   __FILE__,BattenCurve, g);
  TheCommande.Add("minvarcurve","MVCurve P1 P2 Angle1 Angle2 Heigth MVCName", 
                   __FILE__,MVCurve, g);
  TheCommande.Add("setpoint","setpoint side point BattenName ",__FILE__, SetPoint, g);
  TheCommande.Add("setangle","setangle side angle BattenName ",__FILE__, SetAngle, g);
  TheCommande.Add("setslide","setangle slidingfactor BattenName ",__FILE__, SetSlide, g);
  TheCommande.Add("freeangle","freeangle side BattenName",__FILE__, FreeAngle, g);
  TheCommande.Add("freeslide","freeslide BattenName",__FILE__, FreeSlide, g);
  TheCommande.Add("setheight","setheight height BattenName ",__FILE__, SetHeight, g);
  TheCommande.Add("setslope","setslope slope BattenName ",__FILE__, SetSlope, g);
  TheCommande.Add("setcurvature","setcurvature side rho MVCName ",__FILE__, SetCurvature, g);
  TheCommande.Add("freecurvature","freecurvature side  MVCName ",__FILE__, FreeCurvature, g);
  TheCommande.Add("setphysicalratio","physicalratio ratio MVCName ",__FILE__, SetPhysicalRatio, g);   
}
