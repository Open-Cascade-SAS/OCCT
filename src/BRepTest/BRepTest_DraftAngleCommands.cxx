// File:	BRepTest_DraftAngleCommands.cxx
// Created:	Wed Feb 22 15:46:48 1995
// Author:	Jacques GOUSSARD
//		<jag@topsn2>


#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>

#include <BRepOffsetAPI_DraftAngle.hxx>
#include <BRepOffsetAPI_MakeDraft.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>

static void Print(Draw_Interpretor& di,
		  const Draft_ErrorStatus St)
{
  di << "  Error Status : ";
  switch (St) {
  case Draft_NoError:
    di << "No error";
    break;

  case Draft_FaceRecomputation:
    di << "Impossible face recomputation";
    break;

  case Draft_EdgeRecomputation:
    di << "Impossible edge recomputation";
    break;

  case Draft_VertexRecomputation:
    di << "Impossible vertex recomputation";
    break;

  }
}

static Standard_Integer DEP(Draw_Interpretor& theCommands,
                            Standard_Integer narg, const char** a)
{
  if ((narg<14)||(narg%8 != 6)) return 1;
  TopoDS_Shape V = DBRep::Get(a[2]);
  BRepOffsetAPI_DraftAngle drft(V);

  gp_Dir Dirextract(atof(a[3]),atof(a[4]),atof(a[5]));

  TopoDS_Face F;
  Standard_Real Angle;
  gp_Pnt Pax;
  gp_Dir Dax;
  for (Standard_Integer ii = 0; ii < (narg-6)/8; ii++){
    TopoDS_Shape aLocalShape(DBRep::Get(a[8*ii+6],TopAbs_FACE));
    F = TopoDS::Face(aLocalShape);
//    F = TopoDS::Face(DBRep::Get(a[8*ii+6],TopAbs_FACE));
    Angle = atof(a[8*ii+7])*PI/180.;
    Pax.SetCoord(atof(a[8*ii+8]),atof(a[8*ii+9]),atof(a[8*ii+10]));
    Dax.SetCoord(atof(a[8*ii+11]),atof(a[8*ii+12]),atof(a[8*ii+13]));
    drft.Add(F,Dirextract,Angle,gp_Pln(Pax,Dax));
    if (!drft.AddDone()) {
      break;
    }
  }

  if (!drft.AddDone()) {
    DBRep::Set("bugdep",drft.ProblematicShape());
    theCommands << "Bad shape in variable bugdep ";
    Print(theCommands,drft.Status());
    return 1;
  }
  drft.Build();
  if (drft.IsDone()) {
    DBRep::Set(a[1],drft);
    return 0;
  }
  DBRep::Set("bugdep",drft.ProblematicShape());
  theCommands << "Problem encountered during the reconstruction : ";
  theCommands << "bad shape in variable bugdep; ";
  Print(theCommands,drft.Status());
  return 1;
}


static Standard_Integer NDEP(Draw_Interpretor& theCommands,
                            Standard_Integer narg, const char** a)
{
  if ((narg<15)||((narg)%9 != 6)) return 1;
  TopoDS_Shape V = DBRep::Get(a[2]);
  if ( V.IsNull()) {
    //cout << a[2] << " is not a Shape" << endl;
    theCommands << a[2] << " is not a Shape" << "\n";
    return 1;
  }

  BRepOffsetAPI_DraftAngle drft(V);

  gp_Dir Dirextract(atof(a[3]),atof(a[4]),atof(a[5]));

  TopoDS_Face F;
  Standard_Real Angle;
  gp_Pnt Pax;
  gp_Dir Dax;
  Standard_Boolean Flag;
  for (Standard_Integer ii = 0; ii < (narg-6)/9; ii++){
    TopoDS_Shape aLocalFace(DBRep::Get(a[9*ii+6],TopAbs_FACE));
    F = TopoDS::Face(aLocalFace);
//    F = TopoDS::Face(DBRep::Get(a[9*ii+6],TopAbs_FACE));

    if ( F.IsNull()) {
      //cout << a[9*ii+6] << " is not a face" << endl;
      theCommands << a[9*ii+6] << " is not a face" << "\n";
      return 1;
    }

//#ifdef DEB
//    Flag = atof(a[9*ii+7]); // BUG?? Real -> Boolean ???
//#else
    Flag = (Standard_Boolean ) atof(a[9*ii+7]);
//#endif
    Angle = atof(a[9*ii+8])*PI/180.;
    Pax.SetCoord(atof(a[9*ii+9]),atof(a[9*ii+10]),atof(a[9*ii+11]));
    Dax.SetCoord(atof(a[9*ii+12]),atof(a[9*ii+13]),atof(a[9*ii+14]));
    drft.Add(F,Dirextract,Angle,gp_Pln(Pax,Dax), Flag);
    if (!drft.AddDone()) {
      break;
    }
  }

  if (!drft.AddDone()) {
    DBRep::Set("bugdep",drft.ProblematicShape());
    theCommands << "Bad shape in variable bugdep ";
    Print(theCommands,drft.Status());
    return 1;
  }
  drft.Build();
  if (drft.IsDone()) {
    DBRep::Set(a[1],drft);
    return 0;
  }
  DBRep::Set("bugdep",drft.ProblematicShape());
  theCommands << "Problem encountered during the reconstruction : ";
  theCommands << "bad shape in variable bugdep; ";
  Print(theCommands,drft.Status());
  return 1;
}

static Standard_Integer draft (Draw_Interpretor& di,
				   Standard_Integer n, const char** a)
{
  Standard_Integer Inside = -1;
  Standard_Boolean Internal = Standard_False;
  if (n < 8) return 1;

  Standard_Real x, y ,z, teta;
  TopoDS_Shape SInit = DBRep::Get(a[2]);//shape d'arret
  
  x = atof(a[3]);   
  y = atof(a[4]);  // direction de depouille
  z = atof(a[5]);  
  teta = atof(a[6]); //angle de depouille (teta)

  gp_Dir D(x,y,z);
 


  BRepOffsetAPI_MakeDraft MkDraft(SInit, D, teta);

  if (n>8) {
    Standard_Integer cur = 8;
    if (!strcmp(a[cur],"-IN")) {
     Inside = 1;
     cur++;
    }
    else if (!strcmp(a[cur],"-OUT")) {
      Inside = 0;
     cur++; 
    }

    if (cur<n) {
      if (!strcmp(a[cur],"-Ri")) {
	MkDraft.SetOptions(BRepBuilderAPI_RightCorner);
	cur++;
      }
      else if (!strcmp(a[cur],"-Ro")) {
	MkDraft.SetOptions(BRepBuilderAPI_RoundCorner);
	cur++;
      }      
    }
    if (cur<n) {
      if (!strcmp(a[cur],"-Internal")) {
	Internal = Standard_True;
	cur++;
      }
    }
  }  

  if (Internal) {
    MkDraft.SetDraft(Internal);
    di << "Internal Draft : " << "\n";
    //cout << "Internal Draft : " << endl;
  }
  else
    di << "External Draft : " << "\n";
    //cout << "External Draft : " << endl;

  TopoDS_Shape Stop = DBRep::Get(a[7]);//shape d'arret 
  if (!Stop.IsNull()) {
    Standard_Boolean KeepOutside = Standard_True;
    if (Inside==0)  KeepOutside = Standard_False;
    MkDraft.Perform(Stop,  KeepOutside);
  }
  else {
    Handle(Geom_Surface) Surf = DrawTrSurf::GetSurface(a[7]); 
    if (! Surf.IsNull()) { // surface d'arret
      Standard_Boolean KeepInside = Standard_True;
      if (Inside==1)  KeepInside = Standard_False;
      MkDraft.Perform(Surf, KeepInside);
    }
    else {  // by Lenght
      Standard_Real L = atof(a[7]);
      if (L > 1.e-7) {
	 MkDraft.Perform(L);
      }
      else  return 1;
    }
  }
 
  DBRep::Set(a[1], MkDraft.Shape());
  DBRep::Set("DraftShell", MkDraft.Shell());

  return 0;
}


//=======================================================================
//function : DraftAngleCommands
//purpose  : 
//=======================================================================

void  BRepTest::DraftAngleCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "Draft angle modification commands";
   
  theCommands.Add("depouille", " Inclines faces of a shape, dep result shape dirx diry dirz face angle x y x dx dy dz [face angle...]",__FILE__,DEP,g);

  theCommands.Add("ndepouille", " Inclines faces of a shape, dep result shape dirx diry dirz face 0/1 angle x y x dx dy dz [face 0/1 angle...]",__FILE__,NDEP,g);

 theCommands.Add("draft"," Compute a draft surface along a shape, \n draft result shape dirx diry dirz  angle shape/surf/length [-IN/-OUT] [Ri/Ro] [-Internal]",
		 __FILE__,draft,g);
}
