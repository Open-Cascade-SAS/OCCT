// Created on: 1994-10-24
// Created by: Jean Yves LEBEY
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

#include <TestTopOpe.hxx>
#include <TestTopOpe_HDSDisplayer.hxx>
#include <TestTopOpeDraw_DrawableSHA.hxx>
#include <TestTopOpeDraw_DrawableP3D.hxx>
#include <TestTopOpeDraw_DrawableC3D.hxx>
#include <TestTopOpeDraw_TTOT.hxx>
#include <TestTopOpeDraw_Displayer.hxx>
#include <TopOpeBRepDS_connex.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_PointExplorer.hxx>
#include <TopOpeBRepDS_CurveExplorer.hxx>
#include <TopOpeBRepDS_Explorer.hxx>
#include <TopOpeBRepDS_TKI.hxx>
#include <TopOpeBRepDS_Check.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopLoc_Location.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <DBRep.hxx>
#include <Draw_Appli.hxx>
#include <DrawTrSurf_Curve.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>

class TSEEpar {
public:
  TSEEpar(const TCollection_AsciiString& s) { set(s); }
  void set(const TCollection_AsciiString& s) {
    miskind = misshap = misgeom = missect = misdege = misafev = misedcu = Standard_False; 
    mTK = TopOpeBRepDS_UNKNOWN; mTS = TopAbs_SHAPE;
    char* sK = (char *)s.ToCString();
    miskind = TestTopOpeDraw_TTOT::StringToKind(s,mTK);
    if ( miskind ) {
      misshap = TopOpeBRepDS::IsTopology(mTK);
      if (misshap) mTS = TopOpeBRepDS::KindToShape(mTK);
      else misgeom = TopOpeBRepDS::IsGeometry(mTK);
    }
    else if ( ! strcasecmp(sK,"s") ) {
      misshap = Standard_True; mTS = TopAbs_SHAPE;
    }
    else if ( ! strcasecmp(sK,"se") ) {
      missect = Standard_True; mTK = TopOpeBRepDS_EDGE;
    }
    else if ( ! strcasecmp(sK,"de") ) {
      misdege = Standard_True; mTK = TopOpeBRepDS_EDGE;
    }
    else if ( ! strcasecmp(sK,"fev") ) {
      misafev = Standard_True;
    }
    else if ( ! strcasecmp(sK,"ec") ) {
      misedcu = Standard_True;
    }
  }
  Standard_Boolean iskind() { return miskind;}
  Standard_Boolean isshap() { return misshap;}
  Standard_Boolean isgeom() { return misgeom;}
  Standard_Boolean issect() { return missect;}
  Standard_Boolean isdege() { return misdege;}
  Standard_Boolean isafev() { return misafev;}
  Standard_Boolean isedcu() { return misedcu;}
  TopOpeBRepDS_Kind TK() {return mTK;}
  TopAbs_ShapeEnum TS() {return mTS;}
private:
  Standard_Boolean miskind,misshap,misgeom,missect,misdege,misafev,misedcu;
  TopOpeBRepDS_Kind mTK; TopAbs_ShapeEnum mTS;
};

class TDSpar : public TSEEpar {
public:
  TDSpar(const TCollection_AsciiString& s) : TSEEpar(s) {}
};

#define ISINTEGER(MMstr) ((strspn((MMstr),"0123456789") == strlen((MMstr))))

TestTopOpe_HDSDisplayer* PHDSD = NULL;
static TestTopOpeDraw_Displayer* POCD = NULL;
//Standard_IMPORT extern Standard_Integer TopOpeBRepDS_GLOBALHDSinterfselector;
Standard_IMPORT Standard_Integer TopOpeBRepDS_GLOBALHDSinterfselector;

static Standard_Boolean SFindKeep = Standard_False;
static Standard_Boolean GFindKeep = Standard_False;
static Standard_Boolean PrintDistancePP = Standard_False;
static Standard_Integer SelectRankShape = 0;
static Standard_Boolean DisOri = Standard_False;
static Standard_Boolean TolFlag = Standard_False;
static Standard_Real OldParFlag = -1.0;
static Standard_Real ParFlag = -1.0;

class tsee_entity {
public:
  Standard_Boolean myEOK;
  TestTopOpe_HDSDisplayer* myEPHDSD;
  Handle(TopOpeBRepDS_HDataStructure) myEHDS; TopOpeBRepDS_DataStructure *myEPDS;
  TopOpeBRepDS_Kind myETK;TopAbs_ShapeEnum myETS;Standard_Integer myEindex;
  Standard_Boolean myEiskind,myEisshape,myEisgeome,myEissect;
  TCollection_AsciiString myEnamedbrep,myEnamedisp; Draw_ColorKind myEnamecolor;
  
//JR/Hp :
  tsee_entity() {
//  tsee_entity::tsee_entity() {
    myEOK = Standard_False;
    if (PHDSD == NULL) return; myEPHDSD = PHDSD;
    myEHDS = myEPHDSD->CurrentHDS(); myEPDS = (TopOpeBRepDS_DataStructure*)&myEHDS->ChangeDS();  
    myETK = TopOpeBRepDS_UNKNOWN;myETS = TopAbs_SHAPE; myEindex = 0;
    myEiskind = myEisshape = myEisgeome = myEissect = Standard_False;
    myEnamedbrep = ""; myEnamedisp = ""; myEnamecolor = Draw_blanc;
    myEOK = Standard_True;
  }
  
  Standard_Boolean IsP() const { return (myETK == TopOpeBRepDS_POINT); }
  Standard_Boolean IsV() const { return (myETK == TopOpeBRepDS_VERTEX); }
  Standard_Boolean Is0() const { return IsP() || IsV(); }
  Standard_Integer Set(Standard_Integer na,const char** a,Standard_Integer iargK,Standard_Integer iargI);
  void Delete() ;
  Standard_EXPORT virtual ~tsee_entity() {Delete() ;} ;
  void virtual Dump() const;
  void virtual See() = 0;
};

void tsee_entity::Delete()
{}

Standard_Integer tsee_entity::Set(Standard_Integer na,const char** a,Standard_Integer iargK,Standard_Integer iargI) {
  myEOK = Standard_False;
  if (iargK>=na) return 1; if (iargI>=na) return 1;
  TCollection_AsciiString ascK = a[iargK]; myEindex = Draw::Atoi(a[iargI]);
  myEiskind = TestTopOpeDraw_TTOT::StringToKind(ascK,myETK);
  if (!myEiskind) return 1; if (myETK == TopOpeBRepDS_UNKNOWN) return 1;
  
  myEisshape = TopOpeBRepDS::IsTopology(myETK);
  if (myEisshape) {
    if ((myEindex < 1) || (myEindex > myEPDS->NbShapes())) return 0;
    myETS = TopOpeBRepDS::KindToShape(myETK);
    const TopoDS_Shape& S = myEPDS->Shape(myEindex);
    if (myETS != S.ShapeType()) return 0;
    myEPHDSD->ShapeName(myEindex,S,myEnamedbrep);
    myEPHDSD->ShapeDisplayName(myEindex,S,myEnamedisp);
  }
  
  myEisgeome = TopOpeBRepDS::IsGeometry(myETK);
  if (myEisgeome) {
    if (IsP()) {
      TopOpeBRepDS_PointExplorer pex(myEHDS->DS());
      if (!pex.IsPoint(myEindex)) return 1;
    }
    TestTopOpeDraw_TTOT::GeometryName(myEindex,myETK,myEnamedbrep);
    myEPHDSD->GeometryDisplayName(myEindex,myETK,myEnamedisp);
  }
  
  myEnamecolor = TestTopOpeDraw_TTOT::GeometryColor(myETK);
  myEOK = Standard_True;
  return 0;
}

void tsee_entity::Dump() const { 
  TopOpeBRepDS_Dumper Dumper(myEHDS);
  if (myEisgeome) Dumper.DumpGeometry(myETK,myEindex,cout);
  if (myEisshape) Dumper.DumpTopology(myETK,myEindex,cout);
}

class tsee_entity0 : public tsee_entity {
public:
  
  gp_Pnt Pnt() const;  
  void See();
  
};

gp_Pnt tsee_entity0::Pnt() const
{
  if (!Is0()) Standard_Failure::Raise("not 0d");
  gp_Pnt P; 
  if      (IsP()) P = myEPDS->Point(myEindex).Point();
  else if (IsV()) P = BRep_Tool::Pnt(TopoDS::Vertex(myEPDS->Shape(myEindex)));
  return P;
}

static void SetPoint
(const TCollection_AsciiString& namedbrep,const TCollection_AsciiString& namedisp,const Draw_Color& namecolor,const TopOpeBRepDS_Point& P)
{
  Handle(TestTopOpeDraw_DrawableP3D) D;
  if(TolFlag) {
    D = new TestTopOpeDraw_DrawableP3D(P.Point(),Draw_CircleZoom,namecolor,
					 namedisp.ToCString(),namecolor,
					 P.Tolerance());
  } else {
    D = new TestTopOpeDraw_DrawableP3D(P.Point(),Draw_Square,namecolor,
					 namedisp.ToCString(),namecolor);
  }
  char *pname = (char *)namedbrep.ToCString();
  Draw::Set(pname,Handle(Draw_Marker3D)::DownCast(D));
  
}
void tsee_entity0::See() 
{
  if (IsP()) ::SetPoint(myEnamedbrep,myEnamedisp,myEnamecolor,myEPDS->Point(myEindex));
  if (IsV()) POCD->DisplayShape(myEnamedbrep,myEnamedisp,myEPDS->Shape(myEindex));
}

//-----------------------------------------------
static void SetCurve
//-----------------------------------------------
(const TCollection_AsciiString& namedbrep,
 const TCollection_AsciiString& namedisp,
 const Draw_Color& namecolor,
 const TopOpeBRepDS_Curve& DSC,
 const Standard_Integer iC)
{
  if ( !PHDSD ) return;
  const Handle(Geom_Curve) GC = DSC.Curve();
  if ( GC.IsNull() ) { cout<<"Curve() nulle"<<endl; return; }
  
  Standard_Real f = GC->FirstParameter();
  Standard_Real l = GC->LastParameter();
  
  GeomAdaptor_Curve GAC(GC);
  GeomAbs_CurveType GACt = GAC.GetType();
  if ( GACt == GeomAbs_Line ) {
    Standard_Real fline = RealLast(),lline = RealFirst(); 
    Standard_Integer imother = DSC.Mother(),igood;
    if(imother) igood = imother;
    else        igood = iC;
    TopOpeBRepDS_PointIterator it = PHDSD->CurrentHDS()->CurvePoints(igood);
    for(; it.More(); it.Next()) {
      Standard_Real p = it.Parameter();
      fline = Min(fline,p); lline = Max(fline,p);
    }
    f = fline;
    l = lline;
  }
  
  Handle(Geom_TrimmedCurve) GTC = new Geom_TrimmedCurve(GC,f,l);
  Handle(TestTopOpeDraw_DrawableC3D) D;
  D = new TestTopOpeDraw_DrawableC3D(GTC,namecolor,namedisp.ToCString(),namecolor);
  char *pname = (char *)namedbrep.ToCString();
  Draw::Set(pname,Handle(DrawTrSurf_Curve)::DownCast(D));
}

static TopoDS_Shape bidbid;
static const TopoDS_Shape& GetShape(const Standard_Integer IS,const TopAbs_ShapeEnum TS)
{
  Standard_Boolean TypeControl = Standard_True;
  if ( !PHDSD ) return bidbid;
  
  static TopoDS_Shape ShapeNull;
  const TopOpeBRepDS_DataStructure& BDS = PHDSD->CurrentBDS();
  
  // check on shape index and on shape kind if kind differs from TopAbs_SHAPE
  Standard_Integer ns = BDS.NbShapes();
  if ( IS < 1 || IS > ns) {
    TopAbs::Print(TS,cout);cout<<" "<<IS<<" does not exist in DS"<<endl;
    return ShapeNull;
  }
  else if ( !PHDSD->ShapeKind(IS,TS) ) {
    if ( TypeControl ) {
      cout<<"Shape "<<IS;
      cout<<" is not a ";TopAbs::Print(TS,cout);
      cout<<" but a "; TopAbs::Print(BDS.Shape(IS,SFindKeep).ShapeType(),cout);
      cout<<endl;
    }
    return ShapeNull;
  }
  
  const TopoDS_Shape& S = BDS.Shape(IS,SFindKeep);  
  return S;
}

static const TopoDS_Shape& GetShape(const Standard_Integer IS)
{
  if ( !PHDSD ) return bidbid;
  const TopOpeBRepDS_DataStructure& BDS = PHDSD->CurrentBDS();
  const TopoDS_Shape& S = BDS.Shape(IS);
  return S;
}

//-----------------------------------------------------------------------
// SeeSectionEdge
//-----------------------------------------------------------------------
static Standard_Integer SeeSectionEdge(const Standard_Integer ISE)
{
  if ( !PHDSD || !POCD ) return 0;
  
  Standard_Integer nse = PHDSD->CurrentBDS().NbSectionEdges();
  if ( ISE < 1 || ISE > nse ) return 0;
  const TopoDS_Shape& SE  = PHDSD->CurrentBDS().SectionEdge(ISE);
  if (SE.IsNull()) return 0;
  
  TCollection_AsciiString namedbrep; PHDSD->SectionEdgeName(ISE,SE,namedbrep); 
  TCollection_AsciiString namedisp; PHDSD->SectionEdgeDisplayName(ISE,SE,namedisp);
  POCD->DisplayGeometry(Standard_True);
  POCD->DisplayShape(namedbrep,namedisp,SE);
  return 0;
}

//-----------------------------------------------------------------------
// SeeShapeIS
//-----------------------------------------------------------------------
Standard_Integer SeeShapeIS(const Standard_Integer I,const TopoDS_Shape& S)
{
  if ( !PHDSD || !POCD) return 0;
  
  if (S.IsNull()) return 0;  
  TCollection_AsciiString namedbrep; PHDSD->ShapeName(I,S,namedbrep);
  TCollection_AsciiString namedisp = namedbrep;
  // NameDisplay = *name/name* if S subshape of shape1/shape2
  TCollection_AsciiString ast = "*";
  const TopOpeBRepDS_DataStructure& BDS = PHDSD->CurrentBDS();
  // if the Shape was removed from the DS, "*" is replaced by "~"
  if(!BDS.HasShape(S))
    ast = "~";
  Standard_Boolean of1 = PHDSD->SubShape(S,1);if(of1) namedisp = ast+namedisp;
  Standard_Boolean of2 = PHDSD->SubShape(S,2);if(of2) namedisp = namedisp+ast;
  if (SelectRankShape == 1 && !of1) return 0;
  if (SelectRankShape == 2 && !of2) return 0;
  
  if (S.ShapeType()==TopAbs_EDGE) POCD->DisplayGeometry(Standard_True);
  else POCD->DisplayGeometry(DisOri);
  if((S.ShapeType()==TopAbs_VERTEX) && TolFlag) {
    const TopoDS_Vertex& V = TopoDS::Vertex(S);
    Standard_Real Tol = BRep_Tool::Tolerance(V);
    POCD->SetTol(Tol);
  }
  POCD->DisplayShape(namedbrep,namedisp,S);
  
  return 0;
}


//-----------------------------------------------------------------------
// SeeShape
//-----------------------------------------------------------------------
static Standard_Integer SeeShape(TCollection_AsciiString namedbrep)
{
  if ( !PHDSD || !POCD ) return 0;
  
  const char* pname = (const char *)namedbrep.ToCString();
  TopoDS_Shape S = DBRep::Get(pname);
  if ( S.IsNull() ) return 0;
  TCollection_AsciiString namedisp; 
  PHDSD->ShapeDisplayName(namedbrep,S,namedisp);
  POCD->DisplayShape(namedbrep,namedisp,S);
  return 0;
}

//-----------------------------------------------------------------------
// SeePoint
//-----------------------------------------------------------------------
static Standard_Integer SeePoint(const Standard_Integer I)
{
  if ( !PHDSD ) return 0;
  
  const TopOpeBRepDS_DataStructure& BDS = PHDSD->CurrentBDS();
  if (I < 1 || I > BDS.NbPoints()) return 0;
  
  TopOpeBRepDS_PointExplorer pex(BDS);
  Standard_Boolean isp = pex.IsPoint(I);
  if (!isp) return 0;
  
  const TopOpeBRepDS_Point& P = BDS.Point(I);
  TopOpeBRepDS_Kind K = TopOpeBRepDS_POINT;
  TCollection_AsciiString namedbrep; TestTopOpeDraw_TTOT::GeometryName(I,K,namedbrep);
  TCollection_AsciiString namedisp; PHDSD->GeometryDisplayName(I,K,namedisp);
  Draw_ColorKind namecolor = TestTopOpeDraw_TTOT::GeometryColor(K);
  SetPoint(namedbrep,namedisp,namecolor,P);
  return 0;
}

//-----------------------------------------------------------------------
// SeeCurve
//-----------------------------------------------------------------------
static Standard_Integer SeeCurve(const Standard_Integer I)
{
  if ( !PHDSD ) return 0;
  
  const TopOpeBRepDS_DataStructure& BDS = PHDSD->CurrentBDS();
  if (I < 1 || I > BDS.NbCurves()) return 0;
  
  TopOpeBRepDS_CurveExplorer cex(BDS,GFindKeep);
  Standard_Boolean isc = cex.IsCurve(I);
  if (!isc) return 0;
  
  const TopOpeBRepDS_Curve& C = BDS.Curve(I);
  TopOpeBRepDS_Kind K = TopOpeBRepDS_CURVE;
  TCollection_AsciiString namedbrep; TestTopOpeDraw_TTOT::GeometryName(I,K,namedbrep);
  TCollection_AsciiString namedisp; PHDSD->GeometryDisplayName(I,K,namedisp);
  Draw_ColorKind namecolor = TestTopOpeDraw_TTOT::GeometryColor(K);
  SetCurve(namedbrep,namedisp,namecolor,C,I);
  return 0;
}


//-----------------------------------------------------------------------
// SeeSurface NYI
//-----------------------------------------------------------------------
Standard_Integer SeeSurface(const Standard_Integer I) // NYI
{
  if ( !PHDSD ) return 0;
  
  const TopOpeBRepDS_DataStructure& BDS = PHDSD->CurrentBDS();
  if (I < 1 || I > BDS.NbSurfaces()) return 0;
  return 0;
}

//-----------------------------------------------------------------------
// SeeEdgeCurve
//-----------------------------------------------------------------------
static Standard_Integer SeeEdgeCurve(const Standard_Integer IE,const TopoDS_Shape& SE)
{
  if ( !POCD ) return 0;
  
  if (SE.IsNull()) return 0;
  const TopoDS_Edge& E = TopoDS::Edge(SE);
  TopLoc_Location L;Standard_Real f,l;Handle(Geom_Curve) GC=BRep_Tool::Curve(E,L,f,l);  
  TCollection_AsciiString namedbrep="E";TestTopOpeDraw_TTOT::CatCurveName(IE,namedbrep);
  TCollection_AsciiString namedisp="E";TestTopOpeDraw_TTOT::CatCurveDisplayName(IE,GC,namedisp);
  POCD->DisplayGeometry(Standard_True);
  POCD->DisplayShape(namedbrep,namedisp,SE);
  return 0;
}

//-----------------------------------------------------------------------
// SeeGeometry
//-----------------------------------------------------------------------
static Standard_Integer SeeGeometry(const Standard_Integer IG,const TopOpeBRepDS_Kind TK)
{ 
  if      (TK == TopOpeBRepDS_POINT) return SeePoint(IG);
  else if (TK == TopOpeBRepDS_CURVE) return SeeCurve(IG);
  else if (TK == TopOpeBRepDS_SURFACE) return SeeSurface(IG);
  return 0;
}

//-----------------------------------------------------------------------
// SeeGeometry
//-----------------------------------------------------------------------
static Standard_Integer SeeGeometry(const TopOpeBRepDS_Kind TK)
{
  if ( !PHDSD ) return 0; 
  
  const TopOpeBRepDS_DataStructure& BDS = PHDSD->CurrentBDS();

  if      (TK == TopOpeBRepDS_POINT) {
    TopOpeBRepDS_PointExplorer pex(BDS,GFindKeep);
    for (; pex.More(); pex.Next()) {
      Standard_Integer ig = pex.Index();
      SeeGeometry(ig,TK);
    }
  }
  else if (TK == TopOpeBRepDS_CURVE) {
    TopOpeBRepDS_CurveExplorer cex(BDS,GFindKeep);
    for (; cex.More(); cex.Next()) {
      Standard_Integer ig = cex.Index();
      SeeGeometry(ig,TK);
    }
  }
  else if (TK == TopOpeBRepDS_SURFACE) { //NYI
  }
  return 0;
}

//-----------------------------------------------------------------------
// TOPOPE_SeeShape
//-----------------------------------------------------------------------
Standard_Integer TOPOPE_SeeShape(char* name)
{
  TCollection_AsciiString asc = name;
  Standard_Integer res = SeeShape(asc);
  return res;
}

//-----------------------------------------------------------------------
// suppressarg : remove a[d],modify na--
//-----------------------------------------------------------------------
void suppressarg(Standard_Integer& na,const char** a,const Standard_Integer d) 
{
  for(Standard_Integer i=d;i<na;i++) {
    a[i]=a[i+1];
    a[i+1]=NULL;
  }
  na--;
}

//-----------------------------------------------------------------------
// suppressargs : remove a[*] from <f> to <l>,modify na = na - (l - f)
//-----------------------------------------------------------------------
Standard_EXPORT void suppressargs(Standard_Integer& na,const char** a,const Standard_Integer f,const Standard_Integer l) 
{
  if(l == f) 
    suppressarg(na,a,l);
  if(l <= f) return;
  for(Standard_Integer i=0;i<na-l;i++) {
    a[i+f]=a[i+l+1];
    a[i+l+1]=NULL;
  }
  na -= l-f+1;
}

//-----------------------------------------------------------------------
// tsee_flags
//-----------------------------------------------------------------------
static void tsee_flags(Standard_Integer& na,const char** a) {
  GFindKeep = Standard_False;
  PrintDistancePP = Standard_False;
  SelectRankShape = 0;
  
  for (Standard_Integer ia = 1; ia < na; ia++) {
    if (!strcasecmp(a[ia],"-k")) { 
      suppressarg(na,a,ia);
      GFindKeep = Standard_True;
    }
    else if (!strcasecmp(a[ia],"-d")) { 
      // tsee -d p 4 7 : display p_4,p_7 + tds p 4 7 + calculate distance (p_4,p_7)
      suppressarg(na,a,ia);
      PrintDistancePP = Standard_True;
    }
    else if (!strcasecmp(a[ia],"-1")) { 
      // tsee -1 s 4 : display s_4 <=> s_4 is subshape of the 1st argument
      suppressarg(na,a,ia);
      SelectRankShape = 1;
    }
    else if (!strcasecmp(a[ia],"-2")) { 
      // tsee -2 s 4 : display s_4 <=> s_4 is subshape of the 2nd argument
      suppressarg(na,a,ia);
      SelectRankShape = 2;
    }
    else if (!strcasecmp(a[ia],"-p") && (ia < na-1)) {
      suppressargs(na,a,ia,ia+1);
      Standard_Real t = Draw::Atof(a[ia+1]);
      if (t >= 0. && t <= 1.) {
	OldParFlag = ParFlag; ParFlag = t;
      }
    }
  }
}

void OthersCommands_help(const char* CommandName,const char* syntaxe = "");
//-----------------------------------------------------------------------
// tsee_help
//-----------------------------------------------------------------------
static void tsee_help(const char* CommandName, Draw_Interpretor& di)
{
  di<<""<<"\n";
  di<<"tsee : Draw entities of data structure (DS) loaded by TestTopOpe::CurrentDS()."<<"\n";
  di<<"DS entities are shapes and geometries accessed by a <type> and an <index>."<<"\n";
  di<<"basic usage :"<<"\n";
  di<<"              tsee <type> <index1 index2 ...>"<<"\n";
  di<<"<type>  = so sh f w e v s for solid/shell/face/wire/edge/vertex/shape"<<"\n";
  di<<"          su cu p for surface/curve/point (N.B : su is NYI)"<<"\n";
  di<<"<index> = integer"<<"\n";
  di<<"example : 'tsee e 3' will draw the edge of index 3"<<"\n";
  di<<"          'tsee e 3 6' will draw the edges of index 3 and 6"<<"\n";
  di<<"          'tsee e' will draw all the edges."<<"\n";
  di<<""<<"\n";
  di<<"Index values of DS entities may be given by the 'tds' command which prints"<<"\n";
  di<<"the content of the DS for each shape and geometry type."<<"\n";
  di<<"Entities are drawn with a specific color according to their type."<<"\n";
  di<<""<<"\n";
  di<<"Naming : "<<"\n";
  di<<"'tsee <t> <i>' creates the drawable object named <t>_<i>."<<"\n";
  di<<"This name is displayed near the entity in one if the forms : "<<"\n";
  di<<"     *<t>_<i> meaning entity belongs to the first shape of current operation."<<"\n";
  di<<"     <t>_<i>* meaning entity belongs to the second shape of current operation."<<"\n";
  di<<""<<"\n";
  di<<"Same Domain shapes : "<<"\n";
  di<<"'tsee <type> sd' to display all shapes of <type> having same domain data."<<"\n";
  di<<"example : 'tsee e sd' to display all edges sharing a 1d geometric domain"<<"\n";
  di<<"          with other edges."<<"\n";
  di<<""<<"\n";
  di<<"Optional flags :"<<"\n";
  OthersCommands_help(CommandName);
  di<<"Other types :"<<"\n";
  di<<"'tsee fev' : displays all faces,then edges,then vertices."<<"\n";
  di<<"'tsee es <i>' displays edge described as section edge <i>."<<"\n";
  di<<"      The name of a section edge likes t_i<j>,where :"<<"\n";
  di<<"         - i is the edge index in the set of section edge,"<<"\n";
  di<<"         - j is edge index in the DS (as any edge)."<<"\n";
  di<<""<<"\n";
}

static void tseei_help(Draw_Interpretor& di)
{
  di<<""<<"\n";
  di<<"tseei : Draws entities of data structure (DS) loaded by TestTopOpe::CurrentDS()."<<"\n";
  di<<"Draws the objects appearing in interferences DS objects."<<"\n";
  di<<"See command 'tsee' for discussion about DS objects."<<"\n";
  di<<"\n";
}

static void tki_help(Draw_Interpretor& di)
{
  di<<""<<"\n";
  di<<"tki,tkig,tkis : Groups interferences of DS objects"<<"\n";
  di<<"  by geometry (g),or support (s),and index."<<"\n";
  di<<"See command 'tsee' for discussion about DS objects."<<"\n";
  di<<"(Test of class TopOpeBRepDS_TKI)"<<"\n";
  di<<"\n";
}

static void tds_help(Draw_Interpretor& di)
{
  di<<""<<"\n";
  di<<"tds : dump current DS (loaded by CurrentDS())"<<"\n";
  di<<"basic usage : tds <-arg> type <index1 index2 ...>"<<"\n";
  di<<"\n";
  di<<"Description of curve geometry (type = c) : <-arg>  = -t -k -s -l"<<"\n";
  di<<"\t-k : print only the curve with keep = Standard_True"<<"\n";
  di<<"\t-l : (long) print all poles and knots for BSplines curves"<<"\n";
  di<<"\t-s : (short) print only 4 poles and 4 knots for BSplines curves"<<"\n";
  di<<"on shapes (type = s|so|sh|f|w|e|v) : <-arg>  = -ssi"<<"\n";
  di<<"\t-ssi : print only ShapeShapeInterferences"<<"\n";
  di<<""<<"\n";
  di<<"See command 'tsee' for discussion about type and <index1 index2 ...>."<<"\n";
  di<<"\n";
}

typedef Standard_Integer (*tf_SeeShape)(const Standard_Integer I,const TopoDS_Shape& S);
extern void OthersCommands_flags(Standard_Integer& na,const char** a,TestTopOpeDraw_Displayer& TD);
void COUTNOHDS(Draw_Interpretor& di) {di<<"no current HDS"<<"\n";}
Standard_Integer tsee_SeeShapefunc(Draw_Interpretor& di,Standard_Integer na_in,const char** a,tf_SeeShape SeeShapefunc)
{
  if (na_in == 1 || (na_in == 2 && !strcasecmp(a[1],"-h"))) {
    tsee_help(a[0], di);
    return 0; 
  }
  
  Standard_Integer na = na_in;
  if ((PHDSD == NULL) || (POCD == NULL)) return 0;
  
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if (HDS.IsNull()) { COUTNOHDS(di); return 0; }
  if (na < 2) return 1;
  const TopOpeBRepDS_DataStructure& BDS = PHDSD->CurrentBDS();
  
  POCD->InitDisplayer();
  TolFlag = Standard_False;
  tsee_flags(na,a);
  OthersCommands_flags(na,a,*POCD);
  TolFlag = POCD->TolIs();
  DisOri = POCD->DisplayGeometry();
  
  Standard_Integer i1arg = 1;
  TSEEpar Tpar(a[i1arg]);
  
//  Standard_Integer ia,is,ig;
  Standard_Integer ia,is;
  
  if ( PrintDistancePP ) {
    tsee_entity0 E1,E2;    
    if (na == 4) { // tsee p 3 4
      if (E1.Set(na,a,i1arg+0,i1arg+1)) return 1;
      if (E2.Set(na,a,i1arg+0,i1arg+2)) return 1;
    }
    else if (na == 5) { // tsee p 3 v 4
      if (E1.Set(na,a,i1arg+0,i1arg+1)) return 1;
      if (E2.Set(na,a,i1arg+2,i1arg+3)) return 1;
    }
    if (!E1.myEOK || !E2.myEOK) return 1;
    
    E1.See();E2.See();
    E1.Dump();E2.Dump();
    di<<"Distance "<<E1.myEnamedbrep.ToCString()<<" "<<E2.myEnamedbrep.ToCString()<<" : "<<E1.Pnt().Distance(E2.Pnt())<<"\n";
    
  } // PrintDistancePP
  
  else if ( Tpar.isshap() ) {
    // na == 2 : display all DS shapes of type TS
    // na  > 2 : display DS shapes of type TS of index Draw::Atoi(a[2]..a[narg-1])
    if (na == 2 ) {
      TopOpeBRepDS_Explorer x(HDS,Tpar.TS(),Standard_False);
      for(;x.More();x.Next()) (*SeeShapefunc)(x.Index(),x.Current());
    }
    else {
      if ( !strcmp(a[i1arg+1],"sd") ) { // shapes HasSameDomain of type TS
	TopOpeBRepDS_Explorer x(HDS,Tpar.TS(),Standard_False);
	for (;x.More();x.Next()) {
	  Standard_Boolean isSsd = HDS->HasSameDomain(x.Current());
	  if (isSsd ) (*SeeShapefunc)(x.Index(),x.Current());
	}
      }
      else if (Tpar.isshap() && (Tpar.TS() == TopAbs_EDGE) && (na>=2) && POCD->ParIs()) {
	// see edges [4..na[ with of the name in t in [0..1] / range
	if (na == 2) {
	  TopOpeBRepDS_Explorer x(HDS,TopAbs_EDGE,Standard_False);
	  for (;x.More();x.Next()) (*SeeShapefunc)(x.Index(),x.Current());
	}
	else {
	  for (ia = i1arg+1; ia < na; ia++) { 
	    is = Draw::Atoi(a[ia]);
	    (*SeeShapefunc)(is,GetShape(is,Tpar.TS()));
	  }
	}
	ParFlag = OldParFlag;
      }
      else {
	for (ia = i1arg+1; ia < na; ia++) { 
	  is = Draw::Atoi(a[ia]);
	  (*SeeShapefunc)(is,GetShape(is,Tpar.TS()));
	}
      }
    }
  } // isshap
  
  else if ( Tpar.isgeom() ) { 
    // na == 2 : display all DS geometries of type TK
    // na  > 2 : display DS geometries of type TK index Draw::Atoi(a[2])..a[narg-1])
    if (na == 2 ) SeeGeometry(Tpar.TK());
    else for (ia = i1arg+1; ia < na; ia++) SeeGeometry(Standard_Integer(Draw::Atoi(a[ia])),Tpar.TK());
  }
  
  else if ( Tpar.isafev() ) {
    // faces then edges then vertices
    TopOpeBRepDS_Explorer x;
    for (x.Init(HDS,TopAbs_FACE,Standard_False);x.More();x.Next()) (*SeeShapefunc)(x.Index(),GetShape(x.Index()));
    for (x.Init(HDS,TopAbs_EDGE,Standard_False);x.More();x.Next()) (*SeeShapefunc)(x.Index(),GetShape(x.Index()));
    for (x.Init(HDS,TopAbs_VERTEX,Standard_False);x.More();x.Next()) (*SeeShapefunc)(x.Index(),GetShape(x.Index()));
  }
  
  else if ( Tpar.isedcu() ) {
    // na == 2 : display curves of all DS edges
    // na  > 2 : display curve of DS edges index Draw::Atoi(a[2])..a[narg-1])
    if ( na == 2 ) {
      TopOpeBRepDS_Explorer x(HDS,TopAbs_EDGE,Standard_False);
      for (;x.More();x.Next()) SeeEdgeCurve(x.Index(),x.Edge());
    }
    else {
      for (ia = i1arg+1; ia < na; ia++) { 
	is = Draw::Atoi(a[ia]);
	SeeEdgeCurve(is,GetShape(is,TopAbs_EDGE));
      }
    }
  }
  
  else if ( Tpar.issect() ) { // na=2 all section edges,na>2 section edges (a[2]..a[na-1])
    Standard_Integer ise,nse = BDS.NbSectionEdges(); 
    if (na == 2) for (ise = 1; ise<=nse; ise++) SeeSectionEdge(ise);
    else         for (ia = i1arg+1; ia<na; ia++) SeeSectionEdge(Draw::Atoi(a[ia]));
  }

  else if (Tpar.isdege()) {      
    TopOpeBRepDS_Explorer x;
    for (x.Init(HDS,TopAbs_EDGE);x.More();x.Next()) {
      if (BRep_Tool::Degenerated(x.Edge())) (*SeeShapefunc)(x.Index(),GetShape(x.Index()));
    }
  }
  
  return 0;
} // tsee_SeeShapefunc

//-----------------------------------------------------------------------
// tsee
//-----------------------------------------------------------------------
Standard_Integer tsee(Draw_Interpretor& di,Standard_Integer na_in,const char** a)
{
  if (na_in == 1 || (na_in == 2 && !strcasecmp(a[1],"-h"))) {
    tsee_help(a[0],di);
    return 0; 
  }
  tf_SeeShape ssf = (tf_SeeShape)SeeShapeIS;
  Standard_Integer r = tsee_SeeShapefunc(di,na_in,a,ssf);
  return r;
} // tsee

//-----------------------------------------------------------------------
// SeeAnyKI
//-----------------------------------------------------------------------
Standard_Integer SeeAnyKI(const TopOpeBRepDS_Kind TK,const Standard_Integer I) {
  TopAbs_ShapeEnum TS = TopAbs_COMPOUND;
  Standard_Boolean isshape,isgeome; isshape = isgeome = Standard_False;
  isshape = TopOpeBRepDS::IsTopology(TK);
  if (isshape) TS = TopOpeBRepDS::KindToShape(TK);
  else isgeome = TopOpeBRepDS::IsGeometry(TK);
  if      (isshape) SeeShapeIS(I,GetShape(I,TS));
  else if (isgeome) SeeGeometry(I,TK);
  return 0;
}

//-----------------------------------------------------------------------
void AddShapeKI
//-----------------------------------------------------------------------
(TColStd_ListOfInteger& LOK,TColStd_ListOfInteger& LOI,
 const TopOpeBRepDS_Kind K,const Standard_Integer I)
{
  Standard_Boolean isshape,isgeome; isshape = isgeome = Standard_False;
  isshape = TopOpeBRepDS::IsTopology(K);
  if (!isshape)
    isgeome = TopOpeBRepDS::IsGeometry(K);
  
  if (LOK.IsEmpty() && LOI.IsEmpty()) { 
    LOK.Append((Standard_Integer)K); LOI.Append(I); 
    return; 
  }
  
  // to sort, it is considered that : 
  // POINT == VERTEX,CURVE == EDGE,SURFACE == FACE
  
  TopAbs_ShapeEnum SKtri=TopAbs_COMPOUND;

  if (isshape) SKtri = TopOpeBRepDS::KindToShape(K);
  else if (isgeome) {
    if      (K == TopOpeBRepDS_POINT) SKtri = TopAbs_VERTEX; 
    else if (K == TopOpeBRepDS_CURVE) SKtri = TopAbs_EDGE; 
    else if (K == TopOpeBRepDS_SURFACE) SKtri = TopAbs_FACE; 
  }
  else return;
  
  TColStd_ListIteratorOfListOfInteger ITLOK(LOK);
  TColStd_ListIteratorOfListOfInteger ITLOI(LOI);
  Standard_Boolean equa = Standard_False;
  for (; ITLOK.More(),ITLOI.More(); ITLOK.Next(),ITLOI.Next()) {
//JR/Hp
    Standard_Integer ifK = ITLOK.Value() ;
    TopOpeBRepDS_Kind fK = (TopOpeBRepDS_Kind) ifK ;
    TopAbs_ShapeEnum fSK = TopOpeBRepDS::KindToShape(fK);
//    TopOpeBRepDS_Kind fK = (TopOpeBRepDS_Kind)ITLOK.Value(); TopAbs_ShapeEnum fSK = TopOpeBRepDS::KindToShape(fK);
    Standard_Integer fI = ITLOI.Value();
    if (SKtri == fSK && I == fI) {
      equa = Standard_True;
      break;
    }
    if (SKtri <= fSK) {
      LOK.InsertBefore((Standard_Integer)K,ITLOK);
      LOI.InsertBefore(I,ITLOI);
      break;
    }
  }
  if ( !ITLOK.More() && !equa) {
    LOK.Append((Standard_Integer)K);
    LOI.Append(I);
  }
}

//-----------------------------------------------------------------------
// SeeShapeISI
//-----------------------------------------------------------------------
Standard_Integer SeeShapeISI(const Standard_Integer I,const TopoDS_Shape& S,Draw_Interpretor& di)
{
  if ( !PHDSD ) return 0;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if (S.IsNull()) return 0;  
  SeeShapeIS(I,S);
  
  TopOpeBRepDS_Kind SK = TopOpeBRepDS::ShapeToKind(S.ShapeType());
  const TopOpeBRepDS_DataStructure& BDS = HDS->DS();
  const TopOpeBRepDS_ListOfInterference& LOI = BDS.ShapeInterferences(S);
  TopOpeBRepDS_ListIteratorOfListOfInterference ITLOI(LOI);
  TColStd_ListOfInteger LOKK,LOII;
  
  for (;ITLOI.More();ITLOI.Next()) {
    const Handle(TopOpeBRepDS_Interference)& HI = ITLOI.Value();
    const TopOpeBRepDS_Transition& T = HI->Transition();
    
    TopAbs_ShapeEnum sb = T.ShapeBefore();TopOpeBRepDS_Kind sbk = TopOpeBRepDS::ShapeToKind(sb);
    Standard_Integer sbi = T.IndexBefore();
    TopAbs_ShapeEnum sa = T.ShapeAfter();TopOpeBRepDS_Kind sak = TopOpeBRepDS::ShapeToKind(sa);
    Standard_Integer sai = T.IndexAfter();
    TopOpeBRepDS_Kind gk,sk; Standard_Integer gi,si; HI->GKGSKS(gk,gi,sk,si);
    
    AddShapeKI(LOKK,LOII,sbk,sbi);
    AddShapeKI(LOKK,LOII,sak,sai);
    AddShapeKI(LOKK,LOII,gk,gi);
    AddShapeKI(LOKK,LOII,sk,si);
  }
  
  TColStd_ListIteratorOfListOfInteger ITLOKK,ITLOII;
  
  ITLOKK.Initialize(LOKK); ITLOII.Initialize(LOII);
  Standard_Boolean ya = ITLOKK.More();
  TopOpeBRepDS::Print(SK,I,cout,""," : ");
  for(; ITLOKK.More(),ITLOII.More(); ITLOKK.Next(),ITLOII.Next()) {
//JR/Hp
    Standard_Integer ikk = ITLOKK.Value() ;
    TopOpeBRepDS_Kind kk = (TopOpeBRepDS_Kind) ikk ;
    Standard_Integer ii = ITLOII.Value();
//    TopOpeBRepDS_Kind kk = (TopOpeBRepDS_Kind)ITLOKK.Value(); Standard_Integer ii = ITLOII.Value();
    TopOpeBRepDS::Print(kk,ii,cout,""," ");
  }
  if (ya) di<<"\n"; else di<<"no shape interference"<<"\n";
  
  ITLOKK.Initialize(LOKK); ITLOII.Initialize(LOII);
  for(; ITLOKK.More(),ITLOII.More(); ITLOKK.Next(),ITLOII.Next()) {
//JR/Hp
    Standard_Integer ikk =ITLOKK.Value() ;
    TopOpeBRepDS_Kind kk = (TopOpeBRepDS_Kind) ikk ;
    Standard_Integer ii = ITLOII.Value();
//    TopOpeBRepDS_Kind kk = (TopOpeBRepDS_Kind)ITLOKK.Value(); Standard_Integer ii = ITLOII.Value();
    SeeAnyKI(kk,ii);
  }
  //POP pour WNT
  return 0;
  
} // SeeShapeISI

//-----------------------------------------------------------------------
// SeeShapeTKIG
//-----------------------------------------------------------------------
Standard_Integer SeeShapeTKIG(const Standard_Integer I,const TopoDS_Shape& S)
{
  if ( !PHDSD ) return 0;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if (S.IsNull()) return 0;
  SeeShapeIS(I,S);
  
  TopOpeBRepDS_Kind SK = TopOpeBRepDS::ShapeToKind(S.ShapeType());
  const TopOpeBRepDS_ListOfInterference& LOI = HDS->DS().ShapeInterferences(S);
  TopOpeBRepDS_TKI tki;
  TCollection_AsciiString s1,s2;
  
  tki.Clear();
  tki.FillOnGeometry(LOI);
  s1=TopOpeBRepDS::SPrint(SK,I,""," by G : \n");
  tki.Init();if (tki.More()) s2="\n";else s2="";
  tki.DumpTKIIterator(s1,s2);
  
  return 0;
} // SeeShapeTKIG

//-----------------------------------------------------------------------
// SeeShapeTKIS
//-----------------------------------------------------------------------
Standard_Integer SeeShapeTKIS(const Standard_Integer I,const TopoDS_Shape& S)
{
  if ( !PHDSD ) return 0;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if (S.IsNull()) return 0;
  SeeShapeIS(I,S);
  
  TopOpeBRepDS_Kind SK = TopOpeBRepDS::ShapeToKind(S.ShapeType());
  const TopOpeBRepDS_ListOfInterference& LOI = HDS->DS().ShapeInterferences(S);
  TopOpeBRepDS_TKI tki;
  TCollection_AsciiString s1,s2;
  
  tki.Clear();
  tki.FillOnSupport(LOI);
  s1=TopOpeBRepDS::SPrint(SK,I,""," by S : \n");
  tki.Init();if (tki.More()) s2="\n";else s2="";
  tki.DumpTKIIterator(s1,s2); 
  
  return 0;
} // SeeShapeTKIS

//-----------------------------------------------------------------------
// SeeShapeTKI
//-----------------------------------------------------------------------
Standard_Integer SeeShapeTKI(const Standard_Integer I,const TopoDS_Shape& S,Draw_Interpretor& di)
{
  di<<"\n";
  SeeShapeTKIG(I,S);
  SeeShapeTKIS(I,S);
  return 0;
} // SeeShapeTKI

//-----------------------------------------------------------------------
// tseei
//-----------------------------------------------------------------------
Standard_Integer tseei(Draw_Interpretor& di,Standard_Integer na_in,const char** a)
{
  if (na_in == 1) { tseei_help(di); return 0; }
  tf_SeeShape ssf = (tf_SeeShape)SeeShapeISI;
  Standard_Integer r = tsee_SeeShapefunc(di,na_in,a,ssf);
  return r;
} // tseei

//-----------------------------------------------------------------------
// tki
//-----------------------------------------------------------------------
Standard_Integer tki(Draw_Interpretor& di,Standard_Integer na_in,const char** a)
{
  if (na_in == 1) { tki_help(di); return 0; }
  
  tf_SeeShape ssf = NULL;
  if      ( !strcasecmp(a[0],"tki") )  ssf = (tf_SeeShape)SeeShapeTKI;
  else if ( !strcasecmp(a[0],"tkig") ) ssf = (tf_SeeShape)SeeShapeTKIG;
  else if ( !strcasecmp(a[0],"tkis") ) ssf = (tf_SeeShape)SeeShapeTKIS;
  if (ssf == NULL) return 0;
  
  Standard_Integer r = tsee_SeeShapefunc(di,na_in,a,ssf);
  return r;
} // tki

//-----------------------------------------------------------------------
// 0 = tds
// 1 = kind of DS object to dump = [s] [so sh f w e v] [se] [su c p]
// 2,3 ... = [i1,i2 ...] = indices of DS object to see
//-----------------------------------------------------------------------
Standard_Integer tds(Draw_Interpretor& di,Standard_Integer na,const char** a)
{
  Standard_Boolean TDSkeep,TDScompact; TDSkeep = TDScompact = Standard_False;
  TopOpeBRepDS_GLOBALHDSinterfselector = 0;

  Standard_Integer ia ;
  for ( ia = 0; ia < na; ia++) {
    if (!strcasecmp(a[ia],"-ssi")) {
      TopOpeBRepDS_GLOBALHDSinterfselector = 1;
      suppressarg(na,a,ia);
      continue;
    }
    else if (!strncmp(a[ia],"-",1)) {
      TCollection_AsciiString as = a[ia];
      as.Remove(1,1);
      for(Standard_Integer i = 1;i <= as.Length(); i++) {
	if     (as.Value(i) == 'k') TDSkeep = Standard_True;
	else if(as.Value(i) == 'l') TDScompact = Standard_False;
	else if(as.Value(i) == 's') TDScompact = Standard_True;
	else if(as.Value(i) == 'h') { tds_help(di); return 0; }
      }
      suppressarg(na,a,ia);
    }
  }
  
  if (PHDSD == NULL) return 0;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if (HDS.IsNull()) {COUTNOHDS(di);return 0;}
  PHDSD->CurrentBDS();
  
  TopOpeBRepDS_Dumper Dumper(HDS);
  if ( na == 1 ) { Dumper.Dump(cout,TDSkeep,TDScompact); return 0; }
  
  TDSpar Tpar(a[1]);  
  Standard_Integer ids;
  
  if (na == 2) {
    if      (Tpar.isshap()) {
      if (Tpar.TS() != TopAbs_SHAPE) Dumper.DumpTopology(Tpar.TK(),cout);
      else                           Dumper.DumpTopology(cout);
    }
    else if (Tpar.isgeom()) {
      Dumper.DumpGeometry(Tpar.TK(),cout,TDSkeep,TDScompact);
    }
    else if (Tpar.issect()) {
      Dumper.DumpSectionEdge(Tpar.TK(),cout);
    }
    else if (Tpar.isafev()) {
      TopOpeBRepDS_Explorer x;
      for (x.Init(HDS,TopAbs_FACE,TDSkeep);x.More();x.Next()) Dumper.DumpTopology(x.Type(),x.Index(),cout);
      for (x.Init(HDS,TopAbs_EDGE,TDSkeep);x.More();x.Next()) Dumper.DumpTopology(x.Type(),x.Index(),cout);
      for (x.Init(HDS,TopAbs_VERTEX,TDSkeep);x.More();x.Next()) Dumper.DumpTopology(x.Type(),x.Index(),cout);
    }
    
    else if (Tpar.isdege()) {      
      TopOpeBRepDS_Explorer x;
      for (x.Init(HDS,TopAbs_EDGE,TDSkeep);x.More();x.Next()) {
	if (BRep_Tool::Degenerated(x.Edge())) Dumper.DumpTopology(x.Type(),x.Index(),cout);
      }
    }
    return 0;
  } // (na == 2)
  
  // nna  > 2 : dump DS entities of type TK index Draw::Atoi(a[2])..a[narg-1])  
  for (ia = 2; ia < na; ia++) {
    
    if ( !strcmp(a[ia],"sd") ) { // dump all shapes HasSameDomain of type TS
      for (TopOpeBRepDS_Explorer x(HDS,Tpar.TS(),Standard_False);x.More();x.Next()) {
	Standard_Boolean isSsd = HDS->HasSameDomain(x.Current());
	if (isSsd ) Dumper.DumpTopology(Tpar.TK(),x.Index(),cout);
      }
    }
    else {
      ids = Draw::Atoi(a[ia]); 
      if ( Tpar.isshap() ) {
	if (Tpar.TS() != TopAbs_SHAPE) Dumper.DumpTopology(Tpar.TK(),ids,cout);
	else {
	  TopAbs_ShapeEnum t = HDS->Shape(ids,SFindKeep).ShapeType();
	  TopOpeBRepDS_Kind k = TopOpeBRepDS::ShapeToKind(t);
	  Dumper.DumpTopology(k,ids,cout);
	}
      }
      else if ( Tpar.isgeom() ) {
	Dumper.DumpGeometry(Tpar.TK(),ids,cout,TDSkeep,TDScompact);
      }
      else if ( Tpar.issect() ) {
	Dumper.DumpSectionEdge(TopOpeBRepDS_EDGE,ids,cout);
      }
    }
  } // ia < na 
  
  return 0;
}

Standard_Integer TPPE(Draw_Interpretor& di,Standard_Integer na,const char** a)
{
  if (PHDSD == NULL) return 0;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if ( HDS.IsNull() ) { COUTNOHDS(di); return 0; }
  
  if (na != 5) return 1;
  Standard_Boolean ok = Standard_True;
  
  gp_Pnt pds;
  TopoDS_Edge ed;
  
  Standard_Integer ip = Draw::Atoi(a[2]);
  Standard_Integer ie = Draw::Atoi(a[4]);
  ok = ok && (ip >= 1); ok = ok && (ie >= 1);
  if (!ok) {
    di <<" bad data"<<"\n";
    return 1;
  }
  
  ok = Standard_False;
  if (!strcmp("p",a[1])){
    ok = Standard_True;
    ok = ok && (ip <= HDS->NbPoints());
    if (ok) pds = HDS->Point(ip).Point();
    if (!ok) di<<" geometry "<<ip<<" is not a point"<<"\n";
  }
  if (!strcmp("v",a[1])){
    ok = Standard_True;
    ok = ok && (ip <= HDS->NbShapes());
    if (ok) {
      TopoDS_Shape sh = HDS->Shape(ip,SFindKeep);
      ok = ok && (sh.ShapeType() == TopAbs_VERTEX);
      if (ok) pds =BRep_Tool::Pnt(TopoDS::Vertex(sh));
    }
    if (!ok) di<<" shape "<<ip<<" is not a vertex"<<"\n";
  } 
  if (!strcmp("e",a[3])){
    ok = ok && (ie <= HDS->NbShapes());
    if (ok) {
      TopoDS_Shape sh = HDS->Shape(ie,SFindKeep);
      ok = ok && (sh.ShapeType() == TopAbs_EDGE);
      if (ok) ed = TopoDS::Edge(sh);
    }
    if (!ok) di<<" shape "<<ie<<" is not an edge"<<"\n";
    
  } else ok = Standard_False;
  
  Standard_Real dmin=0;
  gp_Pnt pproj;
  if (ok) {
    Standard_Real f,l;
    Handle(Geom_Curve) cu = BRep_Tool::Curve(ed,f,l);
    GeomAPI_ProjectPointOnCurve ponc(pds,cu,f,l);
    ok = ponc.NbPoints() >= 1;
    
    if (ok) {
      dmin = ponc.LowerDistance();
      Standard_Real par = ponc.LowerDistanceParameter();
      BRepAdaptor_Curve bc(ed);
      pproj = bc.Value(par);
    } 
  }
  
  if (ok) {
    TCollection_AsciiString pp("p");
    pp.AssignCat(ip); pp.AssignCat("e"); pp.AssignCat(ie);
    char* ppp = (char *)pp.ToCString(); 
    DrawTrSurf::Set(ppp,pproj);
    di<<"dist point "<<ip<<" on edge "<<ie<<" = "<<dmin<<"\n";
  } else di<<"projection failed"<<"\n";
  
  return ok;
} // TPPE

void FUN_ChkIntgInterf(Draw_Interpretor& di) //***Check Integrity Interference
{
  if ( PHDSD == NULL ) return;
  PHDSD->CurrentHDS()->ChkIntg();
  di<<"Check interference integrity completed"<<"\n";
}

void FUN_ChkIntgSamDomain(Draw_Interpretor& di) //***Check Integrity SameDomain
{
  if ( PHDSD == NULL ) return;
  TopOpeBRepDS_Check C(PHDSD->CurrentHDS());
  if(C.ChkIntgSamDom())
    di<<"Check SameDomain integrity completed"<<"\n";
}

void FUN_ChkIntg(Draw_Interpretor& di) //***Check Integrity
{
  if ( PHDSD == NULL ) return;
  FUN_ChkIntgInterf(di);
  FUN_ChkIntgSamDomain(di);
}

void FUN_ChkVertex(Draw_Interpretor& di) //***Check Vertex SameDomain
{
  if ( PHDSD == NULL ) return;
  TopOpeBRepDS_Check C(PHDSD->CurrentHDS());
  if(C.OneVertexOnPnt())
    di<<"Check Structure Vertex SameDomain Complete"<<"\n";
}

//---------------------------------------------------------
Standard_Integer topochk(Draw_Interpretor& di,Standard_Integer na,const char** a)
{
  if (strcmp(a[0],"topochk")) return 1;
  
  if(na < 2) {
    ::FUN_ChkIntg(di);
    ::FUN_ChkVertex(di);
  } 
  else if(na > 2) return 1;
  else if(!strcmp (a[1],"-i")) ::FUN_ChkIntg(di);
  else if(!strncmp(a[1],"-i",2)) {
    if     (!strcmp(a[1],"-ii"))  ::FUN_ChkIntgInterf(di);
    else if(!strcmp(a[1],"-isd")) ::FUN_ChkIntgSamDomain(di);
  }
  else if(!strcmp (a[1],"-s")) ::FUN_ChkVertex(di);
  else {
    di<<"option : -i   = check DS full integrity"<<"\n";
    di<<"option : -ii  = check only interferences"<<"\n";
    di<<"option : -isd = check only SameDomain data"<<"\n"<<"\n";
    di<<"option : -s   = check only SameDomain data on vertex"<<"\n";
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------
// SeeShapeTCX
//-----------------------------------------------------------------------
Standard_Integer SeeShapeTCX(const Standard_Integer I,const TopoDS_Shape& S)
{
  if ( !PHDSD ) return 0;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if (S.IsNull()) return 0;
  FDSCNX_Dump(HDS,I);
  return 0;
} // SeeShapeTCX

static void tcx_help(Draw_Interpretor& di)
{
  di<<"tcx : function not documented"<<"\n";
}

//-----------------------------------------------------------------------
// tcx
//-----------------------------------------------------------------------
Standard_Integer tcx(Draw_Interpretor& di,Standard_Integer na_in,const char** a)
{
  if (na_in == 1) { tcx_help(di); return 0; }
  Standard_Integer r = tsee_SeeShapefunc(di,na_in,a,SeeShapeTCX);
  return r;
} // tcx

#if 0
//---------------------------------------------------------
Standard_Integer tcx(Draw_Interpretor& di,Standard_Integer na,const char** a)
{
  if (PHDSD == NULL) return 0;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if ( HDS.IsNull() ) { COUTNOHDS(di); return 0; }
  if      (na == 1) FDSCNX_Dump(HDS);
  else if (na == 2) {
    if   (ISINTEGER(a[1])) FDSCNX_Dump(HDS,Draw::Atoi(a[1]));
    else 
  }
  return 0;
}
#endif

//-----------------------------------------------------------------------
Standard_Integer tdsri(Draw_Interpretor& di,Standard_Integer na_in,const char** a)
{
  if (na_in == 1 || (na_in == 2 && !strcasecmp(a[1],"-h"))) {
    tsee_help(a[0],di);
    return 0; 
  }
  Standard_Integer na = na_in;
  if ((PHDSD == NULL) || (POCD == NULL)) return 0;
  const Handle(TopOpeBRepDS_HDataStructure)& HDS = PHDSD->CurrentHDS();
  if (HDS.IsNull()) { COUTNOHDS(di); return 0; }
  if (na < 5) return 1;
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  
  Standard_Integer i1arg = 1;
  TDSpar Tpar(a[i1arg]);
  
  if ( strcasecmp(a[i1arg + 2],"i") ) return 0;
  Standard_Integer ii = Draw::Atoi(a[i1arg + 3]);  
//  Standard_Integer ia,is,ig;
  Standard_Integer is;
  if ( Tpar.isshap() ) {
    is = Draw::Atoi(a[i1arg + 1]);
    const TopoDS_Shape& s = GetShape(is,Tpar.TS()); if (s.IsNull()) return 0;
    TopOpeBRepDS_ListOfInterference& li = BDS.ChangeShapeInterferences(is);
    Standard_Integer i=0; TopOpeBRepDS_ListIteratorOfListOfInterference it(li);
    while (it.More()) {
      if(++i == ii) {
	it.Value()->Dump(cout,"\n--> remove ","\n");
	li.Remove(it);
      }
      else it.Next();
    }
  }
  return 0;
} // tdsri

//=======================================================================
//function : DSCommands
//purpose  : 
//=======================================================================
void  TestTopOpe::HDSCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  PHDSD = (TestTopOpe_HDSDisplayer*) new (TestTopOpe_HDSDisplayer);
  POCD = (TestTopOpeDraw_Displayer*) new (TestTopOpeDraw_Displayer);
  
  const char* g = "Topological operation data structure commands";
  theCommands.Add("tseei","no args to get help",__FILE__,tseei,g);
  theCommands.Add("tsee","no args to get help ",__FILE__,tsee,g);
  theCommands.Add("tki","no args to get help ",__FILE__,tki,g);
  theCommands.Add("tkig","no args to get help ",__FILE__,tki,g);
  theCommands.Add("tkis","no args to get help ",__FILE__,tki,g);
  theCommands.Add("tds","-h to get help ",__FILE__,tds,g);
  theCommands.Add("tppe","tppe p/v <ip> e <ie>",__FILE__,TPPE);
  theCommands.Add("topochk","check current DS (loaded by CurrentDS()",__FILE__,topochk,g);
  theCommands.Add("tcx","dump connexity information",__FILE__,tcx,g);
  theCommands.Add("tdsri","remove interference : tdsri type itype iinterf",__FILE__,tdsri,g);
}
