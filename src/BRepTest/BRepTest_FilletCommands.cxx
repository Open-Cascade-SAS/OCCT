// Created on: 1994-06-20
// Created by: Modeling
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepTest.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BiTgte_Blend.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BOPAlgo_PaveFiller.hxx>

#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Section.hxx>

#include <FilletSurf_Builder.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <TopTools_ListOfShape.hxx>
#include <FilletSurf_StatusType.hxx>
#include <FilletSurf_ErrorTypeStatus.hxx>
#include <TopAbs.hxx>
#include <DrawTrSurf.hxx>

#include <stdio.h>


static Standard_Real t3d = 1.e-4;
static Standard_Real t2d = 1.e-5;
static Standard_Real ta  = 1.e-2;
static Standard_Real fl  = 1.e-3;
static Standard_Real tapp_angle = 1.e-2;
static GeomAbs_Shape blend_cont = GeomAbs_C1;

static BRepFilletAPI_MakeFillet* Rakk = 0;
static BRepFilletAPI_MakeFillet* Rake = 0;
static char name[100];


static Standard_Integer contblend(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if(narg ==1) {
    //cout<<"tolerance angular of approximation  : "<< tapp_angle <<endl;
    //cout<<"internal continuity                 : ";
    di<<"tolerance angular of approximation  : "<< tapp_angle <<"\n";
    di<<"internal continuity                 : ";
    switch (blend_cont) {
      case GeomAbs_C0:
         //cout << ""<<endl;
         di << "C0\n";
	 break;
      case GeomAbs_C1:
         //cout << "C1"<<endl;
         di << "C1\n";
	 break;
      case GeomAbs_C2:
         //cout << "C2"<<endl;
         di << "C2\n";
	 break;
       default:
	 break;
       }
    return 0;
  }
  else {
    if (narg >3) return 1;
    if (narg == 3) {  tapp_angle = Abs(Draw::Atof(a[2])); }
    char c=a[1][1];
    switch (c) {
    case '0':
      blend_cont = GeomAbs_C0;
      break;
    case '2':
      blend_cont = GeomAbs_C2;
      break;
    default :
       blend_cont = GeomAbs_C1;    
    }
    return 0;
  }
}

static void printtolblend(Draw_Interpretor& di)
{
  //cout<<"tolerance ang : "<<ta<<endl;
  //cout<<"tolerance 3d  : "<<t3d<<endl;
  //cout<<"tolerance 2d  : "<<t2d<<endl;
  //cout<<"fleche        : "<<fl<<endl;

  //cout<<"tolblend "<<ta<<" "<<t3d<<" "<<t2d<<" "<<fl<<endl;

  di<<"tolerance ang : "<<ta<<"\n";
  di<<"tolerance 3d  : "<<t3d<<"\n";
  di<<"tolerance 2d  : "<<t2d<<"\n";
  di<<"fleche        : "<<fl<<"\n";

  di<<"tolblend "<<ta<<" "<<t3d<<" "<<t2d<<" "<<fl<<"\n";
}

static Standard_Integer tolblend(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if(narg == 1){
    printtolblend(di);
    return 0;
  }
  else if(narg == 5){
    ta = Draw::Atof(a[1]);
    t3d = Draw::Atof(a[2]);
    t2d = Draw::Atof(a[3]);
    fl = Draw::Atof(a[4]);
    return 0;
  }
  return 1;
}
static Standard_Integer BLEND(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if(Rakk != 0) {delete Rakk; Rakk = 0;}
  printtolblend(di);
  if (narg<5) return 1;
  TopoDS_Shape V = DBRep::Get(a[2]);
  if(V.IsNull()) return 1;
  ChFi3d_FilletShape FSh = ChFi3d_Rational;
  if (narg%2 == 0) {
    if (!strcasecmp(a[narg-1], "Q")) {
      FSh = ChFi3d_QuasiAngular;
    }
    else if (!strcasecmp(a[narg-1], "P")) {
      FSh = ChFi3d_Polynomial;
    }
  }
  Rakk = new BRepFilletAPI_MakeFillet(V,FSh);
  Rakk->SetParams(ta,t3d,t2d,t3d,t2d,fl);
  Rakk->SetContinuity(blend_cont, tapp_angle);
  Standard_Real Rad;
  TopoDS_Edge E;
  Standard_Integer nbedge = 0;
  for (Standard_Integer ii = 1; ii < (narg-1)/2; ii++){
    Rad = Draw::Atof(a[2*ii + 1]);
    TopoDS_Shape aLocalEdge(DBRep::Get(a[(2*ii+2)],TopAbs_EDGE));
    E = TopoDS::Edge(aLocalEdge);
//    E = TopoDS::Edge(DBRep::Get(a[(2*ii+2)],TopAbs_EDGE));
    if(!E.IsNull()){
      Rakk->Add(Rad,E);
      nbedge++;
    }
  }
  if(!nbedge) return 1;
  Rakk->Build();
  if(!Rakk->IsDone()) return 1;
  TopoDS_Shape res = Rakk->Shape();
  DBRep::Set(a[1],res);
  return 0;
}

static void PrintHist(const TopoDS_Shape& S,
		      TopTools_ListIteratorOfListOfShape& It,
		      Standard_Integer& nbgen)
{
  TopoDS_Compound C;
  BRep_Builder B;
  B.MakeCompound(C);
  B.Add(C,S);
  char localname[100];
  if(nbgen<10){
    Sprintf(localname,"generated_00%d", nbgen++);
  }
  else if(nbgen<100){
    Sprintf(localname,"generated_0%d", nbgen++);
  }
  else {
    Sprintf(localname,"generated_%d", nbgen++);
  }
  for(; It.More(); It.Next()){
    B.Add(C,It.Value());
  }
  DBRep::Set(localname,C);
}
  
static Standard_Integer CheckHist(Draw_Interpretor& di, 
				  Standard_Integer , 
				  const char** )
{
  if(Rakk == 0) {
    //cout<<"No active Builder"<<endl;
    di<<"No active Builder\n";
    return 1;
  }
  if(!Rakk->IsDone()) {
    //cout<<"Active Builder Not Done"<<endl;
    di<<"Active Builder Not Done\n";
    return 1;
  }
  Standard_Integer nbc = Rakk->NbContours();
  Standard_Integer nbgen = 0;
  TopTools_ListIteratorOfListOfShape It;
  TopoDS_Shape curshape;
  for(Standard_Integer i = 1; i <= nbc; i++){
    curshape = Rakk->FirstVertex(i);
    It.Initialize(Rakk->Generated(curshape));
    PrintHist(curshape,It,nbgen);
    Standard_Integer nbe = Rakk->NbEdges(i);
    for(Standard_Integer j = 1; j <= nbe; j++){
      curshape = Rakk->Edge(i,j);
      It.Initialize(Rakk->Generated(curshape));
      PrintHist(curshape,It,nbgen);
    }
    curshape = Rakk->LastVertex(i);
    It.Initialize(Rakk->Generated(curshape));
    PrintHist(curshape,It,nbgen);
  }
  //cout<<"foreach g [lsort [dir gen*]] { wclick; puts [dname $g]; donl $g; }"<<endl;
  di<<"foreach g [lsort [dir gen*]] { wclick; puts [dname $g]; donl $g; }\n";
  return 0;
}

static Standard_Integer MKEVOL(Draw_Interpretor& di, 
			       Standard_Integer narg, 
			       const char** a)
{
  if(Rake != 0) {delete Rake; Rake = 0;}
  printtolblend(di);
  if (narg < 3) return 1;
  TopoDS_Shape V = DBRep::Get(a[2]);
  Rake = new BRepFilletAPI_MakeFillet(V);
  Rake->SetParams(ta,t3d,t2d,t3d,t2d,fl);
  Rake->SetContinuity(blend_cont, tapp_angle);
  if (narg == 4) {
    ChFi3d_FilletShape FSh = ChFi3d_Rational;
    if (!strcasecmp(a[3], "Q")) {
      FSh = ChFi3d_QuasiAngular;
    }
    else if (!strcasecmp(a[3], "P")) {
      FSh = ChFi3d_Polynomial;
    }
    Rake->SetFilletShape(FSh);
  }
  strcpy(name, a[1]);
  return 0;
}

static Standard_Integer UPDATEVOL(Draw_Interpretor& di, 
				  Standard_Integer narg, 
				  const char** a)
{
  if(Rake == 0){
    //cout << "MakeFillet not initialized"<<endl;
    di << "MakeFillet not initialized\n";
    return 1 ;
  }
  if(narg%2 != 0 || narg < 4) return 1;
  TColgp_Array1OfPnt2d uandr(1,(narg/2)-1);
  Standard_Real Rad, Par;
  TopoDS_Shape aLocalEdge(DBRep::Get(a[1],TopAbs_EDGE));
  TopoDS_Edge E = TopoDS::Edge(aLocalEdge);
//  TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[1],TopAbs_EDGE));
  for (Standard_Integer ii = 1; ii <= (narg/2)-1; ii++){
    Par = Draw::Atof(a[2*ii]);
    Rad = Draw::Atof(a[2*ii + 1]);
    uandr.ChangeValue(ii).SetCoord(Par,Rad);
  }
  Rake->Add(uandr,E);
  return 0;
}

static Standard_Integer BUILDEVOL(Draw_Interpretor& di, 
				  Standard_Integer, 
				  const char**)
{
  if(Rake == 0){
    //cout << "MakeFillet not initialized"<<endl;
    di << "MakeFillet not initialized\n";
    return 1 ;
  }
  Rake->Build();
  if(Rake->IsDone()){
    TopoDS_Shape result = Rake->Shape();
    DBRep::Set(name,result);
    if(Rake != 0) {delete Rake; Rake = 0;}
    return 0;
  }
  if(Rake != 0) {delete Rake; Rake = 0;}
  return 1;
}




//**********************************************
// bfuse or bcut and then blend the section
//**********************************************

Standard_Integer boptopoblend(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  printtolblend(di);
  if(narg < 5)
  {
    cout << "Use <command name> result shape1 shape2 radius [-d]" << endl;
    return 1;
  }

  Standard_Boolean fuse  = !strcmp(a[0],"bfuseblend");
  TopoDS_Shape S1 = DBRep::Get(a[2]);
  TopoDS_Shape S2 = DBRep::Get(a[3]);
  if (S1.IsNull() || S2.IsNull()) {
    printf(" Null shapes are not allowed \n");
    return 1;
  }
  Standard_Real Rad = Draw::Atof(a[4]);
  Standard_Boolean isDebug = Standard_False;

  if(narg == 6)
  {
    if(!strcmp(a[5], "-d"))
    {
      isDebug = Standard_True;
    }
  }

  BOPAlgo_PaveFiller theDSFiller;
  BOPCol_ListOfShape aLS;
  aLS.Append(S1); 
  aLS.Append(S2); 
  theDSFiller.SetArguments(aLS);
  //
  theDSFiller.Perform();
  if (theDSFiller.HasErrors()) {
    printf("Check types of the arguments, please\n");
    return 1;
  }

  BRepAlgoAPI_BooleanOperation* pBuilder=NULL;

  if (fuse)
    pBuilder = new BRepAlgoAPI_Fuse( S1, S2, theDSFiller );
  else
    pBuilder = new BRepAlgoAPI_Cut ( S1, S2, theDSFiller );

  Standard_Boolean anIsDone = pBuilder->IsDone();
  if (!anIsDone)
  {
    printf("boolean operation not done HasErrors()=%d\n", pBuilder->HasErrors());
    return 1;
  }

  TopoDS_Shape ResultOfBop = pBuilder->Shape();
  
  delete pBuilder;

  const int aStrLen = 1000;
  char aBuff[aStrLen];

  if(isDebug)
  {
    strcpy(aBuff, a[1]);
    DBRep::Set( strcat(aBuff, "_bop"), ResultOfBop );

    di << "Intermediate result of BOP-operation is saved to \"" << aBuff << "\" variable\n";
  }

  pBuilder = new BRepAlgoAPI_Section( S1, S2, theDSFiller );
  TopoDS_Shape theSection = pBuilder->Shape();

  if(isDebug)
  {
    strcpy(aBuff, a[1]);
    DBRep::Set( strcat(aBuff, "_sec"), theSection );
     di << "Intermediate bopsection result is saved to \"" << aBuff << "\" variable\n";
  }

  TopoDS_Compound result;
  BRep_Builder BB; 
  BB.MakeCompound(result);
  
  TopExp_Explorer Explo( ResultOfBop, TopAbs_SOLID );
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Shape& aSolid = Explo.Current();

    BRepFilletAPI_MakeFillet Blender(aSolid);
    Blender.SetParams(ta,t3d,t2d,t3d,t2d,fl);
    Blender.SetContinuity( blend_cont, tapp_angle );

    TopExp_Explorer expsec( theSection, TopAbs_EDGE );
    for (; expsec.More(); expsec.Next())
    {
      TopoDS_Edge anEdge = TopoDS::Edge(expsec.Current());
      Blender.Add( Rad, anEdge );
    }

    Blender.Build();
    if (Blender.IsDone())
      BB.Add( result, Blender.Shape() );
    else
    {
      di << "Error: Cannot find the result of BLEND-operation."
        " The result of BOP operation will be returned.\n";
      BB.Add( result, aSolid );
    }
  }

  delete pBuilder;
  DBRep::Set( a[1], result );
  return 0;
}


static Standard_Integer blend1(Draw_Interpretor& di, Standard_Integer narg, const char** a)
{
  if (narg<5) return 1;
  TopoDS_Shape V = DBRep::Get(a[2]);
  if(V.IsNull()) return 1;
  Standard_Integer nb ,i;
  Standard_Real Rad;
  Standard_Boolean simul=Standard_False;
  const char *ns0=(a[1]);
  Rad = Draw::Atof(a[3]);
  TopTools_ListOfShape E;
  for (i=4; i <=(narg-1) ; i++){
    TopoDS_Shape edge= DBRep::Get(a[i],TopAbs_EDGE);
   if (edge.IsNull()) return 1 ;
   if(edge.ShapeType()!=TopAbs_EDGE) return 1;
    E.Append(edge);
  } 
  FilletSurf_Builder aRakk(V,E,Rad);
  if (simul) aRakk.Simulate();
  else  aRakk.Perform();

  //if (Rakk.IsDone()==FilletSurf_IsNotOk) 
  // { FilletSurf_ErrorTypeStatus err=Rakk.StatusError();
  //   if (err==FilletSurf_EmptyList) cout<< "StatusError=EmptyList"<<endl;
  //   else if (err==FilletSurf_EdgeNotG1) cout<< "StatusError=NotG1"<<endl;
  //   else if (err==FilletSurf_FacesNotG1) cout<< "StatusError=facesNotG1"<<endl;   
  //   else if (err==FilletSurf_EdgeNotOnShape) 
  //   cout<< "StatusError=edgenotonshape"<<endl;
  //   else if (err==FilletSurf_NotSharpEdge ) cout<< "StatusError=notsharpedge"<<endl;
  //   else if (err==FilletSurf_PbFilletCompute) cout <<"StatusError=PBFillet"<<endl;
  // }
  // else {
  //  if (Rakk.IsDone()==FilletSurf_IsPartial) cout <<"resultat partiel"<<endl; 
  if (aRakk.IsDone()==FilletSurf_IsNotOk)
   { FilletSurf_ErrorTypeStatus err=aRakk.StatusError();
     if (err==FilletSurf_EmptyList) di<< "StatusError=EmptyList\n";
     else if (err==FilletSurf_EdgeNotG1) di<< "StatusError=NotG1\n";
     else if (err==FilletSurf_FacesNotG1) di<< "StatusError=facesNotG1\n";
     else if (err==FilletSurf_EdgeNotOnShape)
     di<< "StatusError=edgenotonshape\n";
     else if (err==FilletSurf_NotSharpEdge ) di<< "StatusError=notsharpedge\n";
     else if (err==FilletSurf_PbFilletCompute) di <<"StatusError=PBFillet\n";
   }
   else {
    if (aRakk.IsDone()==FilletSurf_IsPartial) di <<"partial result\n";
 
    nb=aRakk.NbSurface();
    char localname [100];
    char *temp; 
    
    // affichage du type d'arret 
    
   if (!simul)
   { 
    //if (Rakk.StartSectionStatus()==FilletSurf_NoExtremityOnEdge)
    //  {cout<<" type deb conges = WLBLOUT"<<endl;}
    //else if (Rakk.StartSectionStatus()==FilletSurf_OneExtremityOnEdge )
    //  { cout<<" type deb conges = WLBLSTOP"<<endl;}
    //else if (Rakk.StartSectionStatus()==FilletSurf_TwoExtremityOnEdge)
    //  {cout<<" type deb conges = WLBLEND"<<endl;}
    if (aRakk.StartSectionStatus()==FilletSurf_NoExtremityOnEdge)
      {di<<" type start fillets = WLBLOUT\n";}
    else if (aRakk.StartSectionStatus()==FilletSurf_OneExtremityOnEdge)
      { di<<" type start fillets = WLBLSTOP\n";}
    else if (aRakk.StartSectionStatus()==FilletSurf_TwoExtremityOnEdge)
      {di<<" type start fillets = WLBLEND\n";}
    
    //if (Rakk.EndSectionStatus()==FilletSurf_NoExtremityOnEdge) 
    //  {cout<<" type fin  conges = WLBLOUT"<<endl;}
    //else if (Rakk.EndSectionStatus()==FilletSurf_OneExtremityOnEdge) 
    //  {cout<<" type fin  conges = WLBLSTOP"<<endl;}
    //else if (Rakk.EndSectionStatus()==FilletSurf_TwoExtremityOnEdge) 
    //  { cout<<" type fin  conges = WLBLEND"<<endl;}
    if (aRakk.EndSectionStatus()==FilletSurf_NoExtremityOnEdge)
      {di<<" type end fillets = WLBLOUT\n";}
    else if (aRakk.EndSectionStatus()==FilletSurf_OneExtremityOnEdge)
      {di<<" type end fillets = WLBLSTOP\n";}
    else if (aRakk.EndSectionStatus()==FilletSurf_TwoExtremityOnEdge)
      { di<<" type end fillets = WLBLEND\n";}
    Standard_Real f,l;
    f = aRakk.FirstParameter();
    l = aRakk.LastParameter();
    //cout<<"parameter on edge start : "<<f<<endl;
    //cout<<"parameter on edge end   : "<<l<<endl;
    di<<"parametre on edge start : "<<f<<"\n";
    di<<"parametre on edge end   : "<<l<<"\n";
    for (i=1;i<=nb;i++){
      //precision 
      //cout<<"precision "<< i << "= "<<Rakk.TolApp3d(i)<<endl;
      di<<"precision "<< i << "= "<<aRakk.TolApp3d(i)<<"\n";
      
      // display resulting surfaces  
      Sprintf(localname, "%s%d" ,ns0,i);
      temp = localname;
      DrawTrSurf::Set(temp,aRakk.SurfaceFillet(i));
      di << localname<< " ";
      
      // display curves 3d 
      Sprintf(localname, "%s%d" ,"courb1",i);
      temp =localname; 
      DrawTrSurf::Set(temp,aRakk.CurveOnFace1(i));
      di << localname<< " ";
      Sprintf(localname, "%s%d" ,"courb2",i);
      temp =localname;
      DrawTrSurf::Set(temp,aRakk.CurveOnFace2(i));
      di << localname<< " ";     
      
      // display supports 
      Sprintf(localname, "%s%d" ,"face1",i);
      temp =localname ;
      DBRep::Set(temp,aRakk.SupportFace1(i));
      di << localname<< " ";
      Sprintf(localname, "%s%d" ,"face2",i);
      temp =localname; 
      DBRep::Set(temp,aRakk.SupportFace2(i));
      di << localname<< " ";
      
      // display Pcurves on faces 
      Sprintf(localname, "%s%d" ,"pcurveonface1",i);
      temp =localname ;
      DrawTrSurf::Set(temp,aRakk.PCurveOnFace1(i));
      di << localname<< " ";
      Sprintf(localname, "%s%d" ,"pcurveonface2",i);
      temp =localname; 
      DrawTrSurf::Set(temp,aRakk.PCurveOnFace2(i));
      di << localname<< " ";
      
      // display Pcurves on the fillet
      Sprintf(localname, "%s%d" ,"pcurveonconge1",i);
      temp =localname;
      DrawTrSurf::Set(temp,aRakk.PCurve1OnFillet(i));
      di << localname<< " ";
      Sprintf(localname, "%s%d" ,"pcurveonconge2",i);
      temp =localname; 
      DrawTrSurf::Set(temp,aRakk.PCurve2OnFillet(i));
      di << localname<< " ";
      
    } }
   else{
   Standard_Integer j;
  
   for (i=1;i<=nb;i++)
    {Standard_Integer s=aRakk.NbSection(i);
    for (j=1;j<=s;j++)
     {Handle(Geom_TrimmedCurve) Sec;
     aRakk.Section(i,j,Sec);
    Sprintf(localname, "%s%d%d" ,"sec",i,j);
     temp =localname;
    DrawTrSurf::Set (temp,Sec);
    di << localname<< " ";}
   }}
 }
  return 0;
}

//=======================================================================
//function : rollingball
//purpose  : 
//=======================================================================

Standard_Integer rollingball(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if ( n < 2) return 1;

  TopoDS_Shape S = DBRep::Get(a[2]);
  if ( S.IsNull()) return 1;
  Standard_Real Rad = Draw::Atof(a[3]);
  
  Standard_Real Tol = t3d; //the same as blend ! 1.e-7;
  
  BiTgte_Blend Roll;
  Roll.Init(S,Rad,Tol,Standard_False);
  
  Standard_Integer Nb = 0;
  for ( Standard_Integer i = 4; i <= n-1; i++) {
    if ( !strcmp(a[i],"@")) {
      Nb++;
      continue;
    }

    if ( Nb == 0) { // return stop faces.
      TopoDS_Shape aLocalFace(DBRep::Get(a[i],TopAbs_FACE));
      TopoDS_Face F1 = TopoDS::Face(aLocalFace);
//      TopoDS_Face F1 = TopoDS::Face(DBRep::Get(a[i],TopAbs_FACE));
      if ( F1.IsNull()) {
	//cout << " Stop face not referenced." << endl;
	di << " Stop face not referenced.\n";
	return 1;
      }
      Roll.SetStoppingFace(F1);
    }
    else if (Nb == 1) { // return faces on which the ball rotates
      TopoDS_Shape aLocalFace(DBRep::Get(a[i],TopAbs_FACE));
      TopoDS_Face F1 = TopoDS::Face(aLocalFace);
//      TopoDS_Face F1 = TopoDS::Face(DBRep::Get(a[i],TopAbs_FACE));
      i++;
      if ( !strcmp(a[i],"@")) {
	//cout << " Even number of ball support faces is required " << endl;
	di << " Even number of ball support faces is required \n";
	return 1;
      }
      aLocalFace = DBRep::Get(a[i],TopAbs_FACE);
      TopoDS_Face F2 = TopoDS::Face(aLocalFace);
//      TopoDS_Face F2 = TopoDS::Face(DBRep::Get(a[i],TopAbs_FACE));
      if ( F1.IsNull() || F2.IsNull()) {
	//cout << " Support face not referenced." << endl;
	di << " Support face not referenced.\n";
	return 1;
      }
      Roll.SetFaces(F1,F2);
    }
    else if (Nb == 2) { // return the edge on which the ball rotates
      TopoDS_Shape aLocalShape(DBRep::Get(a[i],TopAbs_EDGE));
      TopoDS_Edge E = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge E = TopoDS::Edge(DBRep::Get(a[i],TopAbs_EDGE));
      if ( E.IsNull()) {
	//cout << " Edge not referenced." << endl;
	di << " Edge not referenced.\n";
	return 1;
      }
      Roll.SetEdge(E);
    }

  }
  Standard_Boolean BuildShape = (!strcmp(a[0],"brollingball"));

  Roll.Perform(BuildShape);

  Standard_Boolean ComputeBranches = (!strcmp(a[0],"trollingball"));
  char localname[100];
  if (ComputeBranches) {
    Standard_Integer NbBranches = Roll.NbBranches();
    for (Standard_Integer i = 1; i <= NbBranches; i++) {
      Standard_Integer From,To;
      Roll.IndicesOfBranche(i,From,To);
      //cout << " Indexes of the " << i << "th Branch : ";
      //cout << "   " << From << "     " << To << endl;
      di << " Indexes of the " << i << "th Branch : ";
      di << "   " << From << "     " << To << "\n";
      for (Standard_Integer j = From; j <= To; j++) {
	const TopoDS_Shape& CurF = Roll.Face(j);
	Sprintf(localname,"%s_%d_%d",a[1],i,j);
	DBRep::Set(localname,CurF);
      }
    }
    
  }
  else 
    DBRep::Set(a[1],Roll.Shape());

  return 0;
}

//=======================================================================
//function : FilletCommands
//purpose  : 
//=======================================================================

void  BRepTest::FilletCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Fillet construction commands";
   
  theCommands.Add("continuityblend",
		  "continuityblend C0/C1/C2  [tangle]",__FILE__,
		  contblend,g);

  theCommands.Add("tolblend",
		  "tolblend [ta t3d t2d fl]",__FILE__,
		  tolblend,g);

  theCommands.Add("blend",
		  "blend result object rad1 ed1 rad2 ed2 ... [R/Q/P]",__FILE__,
		  BLEND,g);

  theCommands.Add("checkhist",
		  "checkhist",__FILE__,
		  CheckHist,g);

  theCommands.Add("mkevol",
		  "mkevol result object (then use updatevol) [R/Q/P]",__FILE__,
		  MKEVOL,g);

  theCommands.Add("updatevol",
		  "updatevol edge u1 rad1 u2 rad2 ...",__FILE__,
		  UPDATEVOL,g);

  theCommands.Add("buildevol",
		  "buildevol end of the evol fillet computation",__FILE__,
		  BUILDEVOL,g);

  theCommands.Add("bfuseblend",
		  "bfuseblend result shape1 shape2 radius [-d]",__FILE__,
		  boptopoblend,g);
  
  theCommands.Add("bcutblend",
		  "bcutblend result shape1 tool radius [-d]",__FILE__,
		  boptopoblend,g);

  theCommands.Add("blend1",
		  "blend1 result object rad ed1  ed2 ...",__FILE__,
		  blend1,g); 

  theCommands.Add("rollingball",
		  "rollingball  r S radius [stopf1 ..] @ [f1 f2 ..] @ [e1 ..]",
		  __FILE__,
		  rollingball);

  theCommands.Add("brollingball",
		  "brollingball r S radius [stopf1 ..] @ [f1 f2 ..] @ [e1 ..]",
		  __FILE__,
		  rollingball);

  theCommands.Add("trollingball",
		  "trollingball r S radius [stopf1 ..] @ [f1 f2 ..] @ [e1 ..]",
		  __FILE__,
		  rollingball);
}
