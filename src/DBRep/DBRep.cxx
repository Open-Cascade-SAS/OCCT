// Created on: 1993-07-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRep_TEdge.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BinTools.hxx>
#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Draw_Segment3D.hxx>
#include <gp_Ax2.hxx>
#include <GProp.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Vector.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Standard.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <stdio.h>
// memory management
#ifdef _WIN32
extern Draw_Viewer dout;
#endif

#define Characters(IArg) (strspn (Arg[IArg], "0123456789.+-eE") != strlen (Arg[IArg]))
#define Float(IArg)      (strspn (Arg[IArg], "0123456789+-")    != strlen (Arg[IArg]))


//==========================================
// useful methods
//==========================================

Standard_EXPORT void DBRep_WriteColorOrientation ()
{
  cout << "\nrouge  FORWARD";
  cout << "\nbleu   REVERSED";
  cout << "\nrose   EXTERNAL";
  cout << "\norange INTERNAL"<<endl;
}

Standard_EXPORT Draw_Color DBRep_ColorOrientation (const TopAbs_Orientation Or) 
{
  Draw_Color col;
  switch (Or) {

  case TopAbs_FORWARD :
    col = Draw_rouge;
    break;
    
  case TopAbs_REVERSED :
    col = Draw_bleu;
    break;
    
  case TopAbs_EXTERNAL :
    col = Draw_rose;
    break;
    
  case TopAbs_INTERNAL :
    col = Draw_orange;
    break;
    
  }
  return col;
}

//==========================================
// static variables
//==========================================

static Standard_Integer nbIsos  = 2;
static Standard_Real    size    = 100.;
static Standard_Integer discret = 30;
static Standard_Boolean disptriangles = Standard_False;
static Standard_Boolean disppolygons = Standard_False;
static Standard_Real    anglHLR = 35 * M_PI / 180;
static Standard_Real    HAngMin =  1 * M_PI / 180;
static Standard_Real    HAngMax = 35 * M_PI / 180;
static Standard_Boolean withHLR = Standard_False;
static Standard_Boolean withRg1 = Standard_True;
static Standard_Boolean withRgN = Standard_False;
static Standard_Boolean withHid = Standard_False;

//=======================================================================
// isos
//=======================================================================

static Standard_Integer isos (Draw_Interpretor& di,
			      Standard_Integer NbArg, const char **Arg)
{
  NbArg-- ;
  
  if (NbArg <= 0) {
    di << "Current number of isos : " << nbIsos << "\n" ;
    return 0 ;
  }

  Standard_Integer NbIsos = 0 ;
  Standard_Boolean Change = Standard_False ;
  if (!Characters (NbArg) && Float (NbArg)) return 1 ;
  if (!Characters (NbArg)) {
    NbIsos = Draw::Atoi (Arg[NbArg]) ;
    NbArg-- ;
    Change = Standard_True ;
  }

  if (NbArg <= 0) {
    nbIsos = NbIsos ;
    di << "New current number of isos : " << nbIsos << "\n" ;
  } else {
    for (Standard_Integer IArg = 1 ; IArg <= NbArg ; IArg++) {
      Handle (Draw_Drawable3D) Shape1 = Draw::Get (Arg[IArg]) ;
      if (!Shape1.IsNull()) {
	Handle (DBRep_DrawableShape) Shape2 =
	  Handle (DBRep_DrawableShape)::DownCast (Shape1) ;	
	if (!Shape2.IsNull()) {
	  if (Change) {
	    Shape2->ChangeNbIsos (NbIsos) ;
	  } else {
	    di << "Number of isos for " << Arg[IArg] << " : " << Shape2->NbIsos() << "\n";
	  }
	}
      }
    }
    if (Change) dout.RepaintAll() ;
  }

  return 0 ;
}

//=======================================================================
// hlr
//=======================================================================

static Standard_Integer hlr (Draw_Interpretor& di,
			     Standard_Integer n, const char **a)
{
  if (n <= 1) {
    if (withHLR) {
      di << " HLR";
      if (withRgN) di << " RgNLines";
      else {
	if (withRg1) di << " Rg1Lines";
	else         di << " no RegLines";
      }
      if (withHid) di << " HiddenLines";
      else         di << " no HiddenLines";
      di << "\n";
      if (withHLR) {
	di << "Angle of discretization : ";
	di << anglHLR * 180 / M_PI << " degrees\n";
      }
    }
    else di << " wireframe";
    di << "\n";
    return 0 ;
  }

  if (n == 2) {
    if      (!strcasecmp(a[1],"nohlr")) withHLR = Standard_False;
    else if (!strcasecmp(a[1],"hlr"  )) withHLR = Standard_True;
    else if (!strcasecmp(a[1],"nohid")) withHid = Standard_False;
    else if (!strcasecmp(a[1],"hid"  )) {
      withHLR = Standard_True;
      withHid = Standard_True;
    }
    else if (!strcasecmp(a[1],"norg1")) {
      withRg1 = Standard_False;
      withRgN = Standard_False;
    }
    else if (!strcasecmp(a[1],"rg1"  )) {
      withHLR = Standard_True;
      withRg1 = Standard_True;
      withRgN = Standard_False;
    }
    else if (!strcasecmp(a[1],"norgn")) {
      withRgN = Standard_False;
    }
    else if (!strcasecmp(a[1],"rgn"  )) {
      withHLR = Standard_True;
      withRg1 = Standard_True;
      withRgN = Standard_True;
    }
    else if (!strcasecmp(a[1],"ang"  )) {
      di << "Angle de discretisation : ";
      di << anglHLR * 180 / M_PI << " degres\n";
    }
    else return 1;
  }

  Standard_Integer nFirst = 2;

  if (n >= 3 && !strcasecmp(a[1],"ang"  )) {
    nFirst = 3;
    if (n == 3) {
      Standard_Real ang = Draw::Atof(a[2]);
      anglHLR = ang * M_PI / 180;
      if (anglHLR < HAngMin) anglHLR = HAngMin;
      if (anglHLR > HAngMax) anglHLR = HAngMax;
    }
    di << "Angle of discretization : ";
    di << anglHLR * 180 / M_PI << " degrees\n";
  }

  if (n >= nFirst + 1) {

    for (Standard_Integer i = nFirst ; i < n; i++) {
      Handle (Draw_Drawable3D) D = Draw::Get (a[i]) ;
      if (!D.IsNull()) {
	Handle (DBRep_DrawableShape) S =
	  Handle (DBRep_DrawableShape)::DownCast (D) ;	
	if (!S.IsNull()) {
	  Standard_Boolean localHLR, localRg1, localRgN, localHid;
	  Standard_Real localAng;
	  S->GetDisplayHLR(localHLR, localRg1, localRgN, localHid,
			   localAng);
	  if      (!strcasecmp(a[1],"nohlr")) localHLR = Standard_False;
	  else if (!strcasecmp(a[1],"hlr"  )) localHLR = Standard_True;
	  else if (!strcasecmp(a[1],"nohid")) localHid = Standard_False;
	  else if (!strcasecmp(a[1],"hid"  )) {
	    localHLR = Standard_True;
	    localHid = Standard_True;
	  }
	  else if (!strcasecmp(a[1],"norg1")) {
	    localRg1 = Standard_False;
	    localRgN = Standard_False;
	  }
	  else if (!strcasecmp(a[1],"rg1"  )) {
	    localHLR = Standard_True;
	    localRg1 = Standard_True;
	    localRgN = Standard_False;
	  }
	  else if (!strcasecmp(a[1],"norgn")) {
	    localRgN = Standard_False;
	  }
	  else if (!strcasecmp(a[1],"rgn"  )) {
	    localHLR = Standard_True;
	    localRg1 = Standard_True;
	    localRgN = Standard_True;
	  }
	  else if (!strcasecmp(a[1],"ang"  )) {
	    Standard_Real ang = Draw::Atof(a[2]);
	    localAng = ang * M_PI / 180;
	  }
	  else return 1;
	  S->DisplayHLR(localHLR, localRg1, localRgN, localHid,
			localAng);
	}
      }
    }
  }
  dout.RepaintAll() ;

  return 0 ;
}


//=======================================================================
// dispor, dispcon
//=======================================================================

static Standard_Integer dispor (Draw_Interpretor& ,
				Standard_Integer n, const char** a)
{
  Standard_Boolean d = !strcasecmp(a[0],"vori");

  if (d)
    DBRep_WriteColorOrientation();

  Standard_Integer i;
  for (i = 1; i < n; i++) {
    Handle(Draw_Drawable3D) d1 = Draw::Get(a[i]);
    if (!d1.IsNull()) {
      Handle(DBRep_DrawableShape) d2 = 
	Handle(DBRep_DrawableShape)::DownCast(d1);
      if (!d2.IsNull()) {
	d2->DisplayOrientation(d);
      Draw::Repaint();
      }
    }
  }
  return 0;
}

//=======================================================================
// discretisation
//=======================================================================

static Standard_Integer discretisation(Draw_Interpretor& di,
				       Standard_Integer n, const char** a)
{
  if (n <= 1)
    di << "Current number of points : "<<discret<<"\n";
  else {
    discret = Draw::Atoi(a[1]);
  }
  return 0;
}


//=======================================================================
// triangles
//=======================================================================

static Standard_Integer triangles(Draw_Interpretor& , 
				  Standard_Integer n, const char** a)
{
  if (n < 1) return 1;

  if (n == 1) {
    disptriangles = !disptriangles;
#ifdef OCCT_DEBUG
    if (disptriangles) cout <<"Triangulations are always displayed"<<endl;
    else cout <<"Triangulations are displayed only if there is no geometric representation"<<endl;
#endif
  }
  else {
    Standard_Integer i;
    for (i = 1; i <= n-1; i++) {
      Handle(Draw_Drawable3D) d1 = Draw::Get(a[i]);
      if (!d1.IsNull()) {
	Handle(DBRep_DrawableShape) d2 = 
	  Handle(DBRep_DrawableShape)::DownCast(d1);
	if (!d2.IsNull()) {
	  d2->DisplayTriangulation(!(d2->DisplayTriangulation()));
	}
      }
    }
  }

  Draw::Repaint();
  return 0;
}

//=======================================================================
// tclean
//=======================================================================

static Standard_Integer tclean(Draw_Interpretor& , 
			       Standard_Integer n, const char** a)
{
  if (n < 1) return 1;
  
  for (Standard_Integer i = 1; i < n; i++) {
    TopoDS_Shape S = DBRep::Get(a[i]);
    BRepTools::Clean(S);
  }
  return 0;
}

//=======================================================================
// polygons
//=======================================================================

static Standard_Integer polygons(Draw_Interpretor& , 
				 Standard_Integer n, const char** a)
{
  if (n < 1)  return 1;

  if (n == 1) {
    disppolygons = !disppolygons;
#ifdef OCCT_DEBUG
    if (disppolygons) cout <<"Polygons are always displayed"<<endl;
    else cout <<"Polygons are displayed only if there is no geometric representation"<<endl;
#endif
  }
  else {
    Standard_Integer i;
    for (i = 1; i <= n-1; i++) {
      Handle(Draw_Drawable3D) d1 = Draw::Get(a[i]);
      if (!d1.IsNull()) {
	Handle(DBRep_DrawableShape) d2 = 
	  Handle(DBRep_DrawableShape)::DownCast(d1);
	if (!d2.IsNull()) {
	  d2->DisplayPolygons(!(d2->DisplayPolygons()));
	}
      }
    }
  }

  Draw::Repaint();
  return 0;
}


//=======================================================================
// compound
//=======================================================================

static Standard_Integer compound(Draw_Interpretor& ,
				 Standard_Integer n, const char** a)
{
  if (n <= 1) return 1;
  BRep_Builder B;
  TopoDS_Compound C;
  B.MakeCompound(C);
  for (Standard_Integer i = 1; i < n-1; i++) {
    TopoDS_Shape S2 = DBRep::Get(a[i]);
    if (!S2.IsNull()) B.Add(C,S2);
  }
  DBRep::Set(a[n-1],C);
  return 0;
}

//=======================================================================
// emptycopy
//=======================================================================

static Standard_Integer emptycopy(Draw_Interpretor& ,
				  Standard_Integer n, const char** a)
{
  if (n <= 1) return 1;
  TopoDS_Shape S = DBRep::Get(a[(n == 2) ? 1 : 2]);
  if (S.IsNull()) return 1;
  S.EmptyCopy();
  DBRep::Set(a[1],S);
  return 0;
}

//=======================================================================
// add
//=======================================================================

static Standard_Integer add(Draw_Interpretor& ,
			    Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  BRep_Builder B;
  TopoDS_Shape S1 = DBRep::Get(a[1]);
  if (S1.IsNull()) return 1;
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  if (S2.IsNull()) return 1;
  B.Add(S2,S1);
  DBRep::Set(a[2],S2);
  return 0;
}

//=======================================================================
// explode
//=======================================================================

static Standard_Integer explode(Draw_Interpretor& di,
				Standard_Integer n, const char** a)
{
  if (n <= 1) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 0;
  char newname[1024];
  strcpy(newname,a[1]);
  char* p = newname;
  while (*p != '\0') p++;
  *p = '_';
  p++;
  Standard_Integer i = 0;
  if (n == 2) {
    TopoDS_Iterator itr(S);
    while (itr.More()) {
      i++;
      Sprintf(p,"%d",i);
      DBRep::Set(newname,itr.Value());
      di.AppendElement(newname);
      itr.Next();
    }
  }
  else {
    // explode a type
    TopAbs_ShapeEnum typ;
    switch (a[2][0]) {
      
    case 'C' :
    case 'c' :
      if ((a[2][1] == 'd')||(a[2][1] == 'D')) 
	typ = TopAbs_COMPOUND;
      else
	typ = TopAbs_COMPSOLID;
      break;
      
    case 'S' :
    case 's' :
      if ((a[2][1] == 'O')||(a[2][1] == 'o')) 
	typ = TopAbs_SOLID;
      else if ((a[2][1] == 'H')||(a[2][1] == 'h')) 
	typ = TopAbs_SHELL;
      else
	return 1;
      break;
      
    case 'F' :
    case 'f' :
      typ = TopAbs_FACE;
      break;
      
    case 'W' :
    case 'w' :
      typ = TopAbs_WIRE;
      break;
      
    case 'E' :
    case 'e' :
      typ = TopAbs_EDGE;
      break;
      
    case 'V' :
    case 'v' :
      typ = TopAbs_VERTEX;
      break;
      
      default :
	return 1;
    }
    
    TopTools_MapOfShape M;
    M.Add(S);
    TopExp_Explorer ex(S,typ);
    for (; ex.More(); ex.Next()) {
      const TopoDS_Shape& Sx = ex.Current();
      Standard_Boolean added = M.Add(Sx);
      if (added) {
	i++;
	Sprintf(p,"%d",i);
	DBRep::Set(newname,Sx);
	di.AppendElement(newname);
      }
    }
  }
  return 0;
}

//=======================================================================
// nexplode : stable numbered explode (from Serguey Nizhny)
//=======================================================================

static Standard_Integer nexplode(Draw_Interpretor& di, 
				 Standard_Integer n, const char** a)
{ 
  if (n <= 2) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 0;
  char newname[1024];
  strcpy(newname,a[1]);
  char* p = newname;
  while (*p != '\0') p++;
  *p = '_';
  p++;
  TopAbs_ShapeEnum typ;
  // explode a type
  switch (a[2][0]) {    
  case 'F' :
  case 'f' :
    typ = TopAbs_FACE;
    break;
    
  case 'E' :
  case 'e' :
    typ = TopAbs_EDGE;
    break;
    
  case 'V' :
  case 'v' :
    typ = TopAbs_VERTEX;
    break;
    
    default :
      return 1;
  }
  TopTools_IndexedMapOfShape IMOStmp;
  TopTools_MapOfShape MShape;
  IMOStmp.Add(S);
  TopExp::MapShapes(S,typ,IMOStmp);
  TopExp_Explorer Exp(S,typ);
  Standard_Integer MaxShapes, Index = 0;
  MaxShapes = IMOStmp.Extent()-1;
  TopTools_Array1OfShape aShapes(1,MaxShapes);
  
  // explode 
  while (Exp.More()) {
    if (MShape.Add(Exp.Current())) {
      Index++;
      aShapes.SetValue(Index,Exp.Current());
    }
    Exp.Next();
  }
  //
  TColStd_Array1OfInteger OrderInd(1,MaxShapes);
  gp_Pnt GPoint;
  GProp_GProps GPr;
  Standard_Integer aTemp;
  TColStd_Array1OfReal MidXYZ(1,MaxShapes); //X,Y,Z;
  Standard_Boolean NoSort = Standard_True;
  //
  // Computing of CentreOfMass for edge and face
  // and for vertex use its point
  for (Index=1; Index <= MaxShapes; Index++) {
    OrderInd.SetValue(Index,Index);
    const TopoDS_Shape& aS = aShapes(Index);
    if (aS.ShapeType() != TopAbs_VERTEX) {
      BRepGProp::LinearProperties(aS, GPr);
      GPoint = GPr.CentreOfMass();
    }
    else {
      GPoint = BRep_Tool::Pnt(TopoDS::Vertex(aS));
    }
    MidXYZ.SetValue(Index, GPoint.X()*999 + GPoint.Y()*99 +
		    GPoint.Z()*0.9);
  }   
  // Sorting
  while(NoSort) {
    NoSort = Standard_False;
    for (Index=1; Index < MaxShapes; Index++) {
      if (MidXYZ(OrderInd(Index)) > MidXYZ(OrderInd(Index+1))) {
        aTemp = OrderInd(Index);
        OrderInd(Index) = OrderInd(Index+1);
        OrderInd(Index+1) = aTemp;
        NoSort = Standard_True;
      }
    }
  }
  // Check of equality of MidXYZ
  for (Index=1; Index < MaxShapes; Index++) {
    if (MidXYZ(OrderInd(Index+1)) == MidXYZ(OrderInd(Index)))
      di<<"Warning! For this shape the results may be incorrect.\n";
  }
  
  for (Index=1 ;Index <= MaxShapes; Index++) {
    Sprintf(p,"%d",Index);
    DBRep::Set(newname,aShapes(OrderInd(Index)));
    di.AppendElement(newname);    
  }
  
  return 0;
}

//=======================================================================
// exwire
//=======================================================================

static Standard_Integer exwire(Draw_Interpretor& ,
			       Standard_Integer n, const char** a)
{
  if (n <= 1) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 0;
  if (S.ShapeType() != TopAbs_WIRE) return 0;
  char newname[1024];
  strcpy(newname,a[1]);
  char* p = newname;
  while (*p != '\0') p++;
  *p = '_';
  p++;
  Standard_Integer i = 0;
  BRepTools_WireExplorer ex(TopoDS::Wire(S));
  while (ex.More()) {
    i++;
    Sprintf(p,"%d",i);
    DBRep::Set(newname,ex.Current());
    ex.Next();
  }
  return 0;
}

//=======================================================================
// invert
//=======================================================================

static Standard_Integer invert(Draw_Interpretor& ,
			       Standard_Integer n, const char** a)
{
  if (n <= 1) return 1;
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull()) return 0;
  
  BRep_Builder B;
  TopoDS_Shape NS = S.EmptyCopied();
  NS.Closed (S.Closed());

  TopoDS_Iterator itr(S);
  while (itr.More()) {
    B.Add(NS,itr.Value().Reversed());
    itr.Next();
  }
  DBRep::Set(a[1],NS);
  
  return 0;
}

//=======================================================================
// orientation, reverse, complement
//=======================================================================

static Standard_Integer orientation(Draw_Interpretor& ,
				    Standard_Integer n, const char** a)
{
  if (n <= 1) return 1;
  Standard_Integer cas = 0;
  TopAbs_Orientation ori=TopAbs_FORWARD;
  Standard_Integer last = n;
  if (!strcasecmp(a[0],"orientation")) {
    if (n <= 2) return 1;
    last--;
    switch (*a[n-1]) {
	
      case 'F' :
	ori = TopAbs_FORWARD;
	break;
	
      case 'R' :
	ori = TopAbs_REVERSED;
	break;
	
      case 'I' :
	ori = TopAbs_INTERNAL;
	break;
	
      case 'E' :
	ori = TopAbs_EXTERNAL;
	break;
      }
  }

  else if (!strcasecmp(a[0],"treverse")) {
    cas = -1;
  }

  else if (!strcasecmp(a[0],"complement")) {
    cas = -2;
  }

  for (Standard_Integer i = 1; i < last; i++) {
    TopoDS_Shape S = DBRep::Get(a[i]);
    if (!S.IsNull()) {
      if (cas == -2)
	S.Complement();
      else if (cas == -1)
	S.Reverse();
      else
	S.Orientation(ori);
      DBRep::Set(a[i],S);
    }
  }
  return 0;
}

#include <TCollection_AsciiString.hxx>

//=======================================================================
// numshapes same as nbshapes but the output is cout
//=======================================================================

static Standard_Integer numshapes(Draw_Interpretor& di,
				 Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Standard_Integer i;
  TopExp_Explorer ex;
  for (i = 1; i < n; i++) {
    TopoDS_Shape S = DBRep::Get(a[i]);
    if (!S.IsNull()) {
      BRepTools_ShapeSet BS;
      BS.Add(S);
      di <<"Number of shapes in "<<a[i]<<"\n";
      TCollection_AsciiString Astr; 
      BS.DumpExtent(Astr);
      di <<Astr.ToCString();
      di << "\n" ;
    }
  }

  return 0;
}

//=======================================================================
// function : DumpExtent
// purpose  : Dumps the number of sub-shapes in <aStr>.
//=======================================================================
static void DumpExtent(const TopoDS_Shape& aS,
                       TCollection_AsciiString& aStr)
{
  const int aNbTypes=8;
  const char *pNames[aNbTypes+1]={
    " SHAPE     : ",
    " COMPOUND  : ",
    " COMPSOLID : ",
    " SOLID     : ",
    " SHELL     : ",
    " FACE      : ",
    " WIRE      : ",
    " EDGE      : ",
    " VERTEX    : "
  };
  Standard_Integer i, aNb, aNbSh;
  TopAbs_ShapeEnum aType;
  TopTools_IndexedMapOfShape aM;
  //
  aNbSh=0;
  //
  for (i=aNbTypes-1; i>=0; --i) {
    aM.Clear();
    aType=(TopAbs_ShapeEnum)i;
    TopExp::MapShapes(aS, aType, aM);
    aNb=aM.Extent();
    aStr=aStr+pNames[i+1]+TCollection_AsciiString(aNb)+"\n";
    aNbSh+=aNb;
  }
  aStr=aStr+pNames[0]+TCollection_AsciiString(aNbSh)+"\n";
}

//=======================================================================
// nbshapes
//=======================================================================

static Standard_Integer nbshapes(Draw_Interpretor& di,
                                 Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Standard_Integer i;
  Standard_Boolean aTotal;
  TopExp_Explorer ex;
  //
  aTotal = !strcmp(a[n-1], "-t") ? Standard_True : Standard_False;
  //
  for (i = 1; i < n; i++) {
    TopoDS_Shape S = DBRep::Get(a[i]);
    if (!S.IsNull()) {
      di<<"Number of shapes in "<<a[i]<<"\n";
      TCollection_AsciiString Astr; 
      if (aTotal) {
        DumpExtent(S, Astr);
      } else {
        BRepTools_ShapeSet BS;
        BS.Add(S);
        BS.DumpExtent(Astr);
      }
      di<<Astr.ToCString();
    }
  }

  return 0;
}

//=======================================================================
// 
//=======================================================================

static Standard_Integer countshapes(Draw_Interpretor& di,
                                    Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Standard_Integer i;
  TopExp_Explorer ex;
  for (i = 1; i < n; i++) {
    TopoDS_Shape Sh = DBRep::Get(a[i]);
    Standard_Integer nbElem = 0;
    if (!Sh.IsNull()) {
      di <<"Number of shapes in "<<a[i]<<"\n";
      TopTools_MapOfShape M;

      for (ex.Init (Sh,TopAbs_VERTEX); ex.More(); ex.Next()) {
        const TopoDS_Shape& S = ex.Current();
        Standard_Boolean added = M.Add(S);
        if (added) {
          nbElem++;
        }
      }
      di << " VERTEX     : " << nbElem << "\n";
      nbElem = 0;
    
      for (ex.Init (Sh,TopAbs_EDGE); ex.More(); ex.Next()) {
        const TopoDS_Shape& S = ex.Current();
        Standard_Boolean added = M.Add(S);
        if (added) {
          nbElem++;
        }
      }
      di << " EDGE       : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init (Sh,TopAbs_WIRE); ex.More(); ex.Next()) {
        const TopoDS_Shape& S = ex.Current();
        Standard_Boolean added = M.Add(S);
        if (added) {
          nbElem++;
        }
      }
      di << " WIRE       : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init (Sh,TopAbs_FACE); ex.More(); ex.Next()) {
        const TopoDS_Shape& S = ex.Current();
        Standard_Boolean added = M.Add(S);
        if (added) {
          nbElem++;
        }
      }
      di << " FACE       : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init (Sh,TopAbs_SHELL); ex.More(); ex.Next()) {
        const TopoDS_Shape& S = ex.Current();
        Standard_Boolean added = M.Add(S);
        if (added) {
          nbElem++;
        }
      }
      di << " SHELL      : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init (Sh,TopAbs_SOLID); ex.More(); ex.Next()) {
        const TopoDS_Shape& S = ex.Current();
        Standard_Boolean added = M.Add(S);
        if (added) {
          nbElem++;
        }
      }
      di << " SOLID      : " << nbElem << "\n";
      nbElem = 0;

      for (ex.Init (Sh,TopAbs_COMPSOLID); ex.More(); ex.Next()) {
        const TopoDS_Shape& S = ex.Current();
        Standard_Boolean added = M.Add(S);
        if (added) {
          nbElem++;
        }
      }
      di << " COMPSOLID  : " << nbElem << "\n";
      nbElem = 0;
      
      for (ex.Init (Sh,TopAbs_COMPOUND); ex.More(); ex.Next()) {
        const TopoDS_Shape& S = ex.Current();
        Standard_Boolean added = M.Add(S);
        if (added) {
          nbElem++;
        }
      }
      di << " COMPOUND   : " << nbElem << "\n";
      nbElem = 0;

      di << " SHAPE      : " << M.Extent() << "\n";
      di << "\n" ;
    }
}

  return 0;
}

//=======================================================================
// 
//=======================================================================
void setProp(TopoDS_Shape Sh, const char** a, Standard_Integer n)
{
  Standard_Integer i;
  for(i = 2; i < n; i++) {
    if (strstr ( a[i], "free" )) {
      if(a[i][0] == '-') {
        Sh.Free(Standard_False);
      }
      else {
        Sh.Free(Standard_True);
      }
    }
    if (strstr ( a[i], "modified" )) {
      if(a[i][0] == '-') {
        Sh.Modified(Standard_False);
      }
      else {
        Sh.Modified(Standard_True);
      }
    }
    if (strstr ( a[i], "checked" )) {
      if(a[i][0] == '-') {
        Sh.Checked(Standard_False);
      }
      else {
        Sh.Checked(Standard_True);
      }
    }
    if (strstr ( a[i], "orientable" )) {
      if(a[i][0] == '-') {
        Sh.Orientable(Standard_False);
      }
      else {
        Sh.Orientable(Standard_True);
      }
    }
    if (strstr ( a[i], "closed" )) {
      if(a[i][0] == '-') {
        Sh.Closed(Standard_False);
      }
      else {
        Sh.Closed(Standard_True);
      }
    }
    if (strstr ( a[i], "infinite" )) {
      if(a[i][0] == '-') {
        Sh.Infinite(Standard_False);
      }
      else {
        Sh.Infinite(Standard_True);
      }
    }
    if (strstr ( a[i], "convex" )) {
      if(a[i][0] == '-') {
        Sh.Convex(Standard_False);
      }
      else {
        Sh.Convex(Standard_True);
      }
    }
    if (strstr ( a[i], "locked" )) {
      if(a[i][0] == '-') {
        Sh.Locked(Standard_False);
      }
      else {
        Sh.Locked(Standard_True);
      }
    }
  }
}

//=======================================================================
// 
//=======================================================================
static Standard_Integer setFlags(Draw_Interpretor& ,
                                    Standard_Integer n, const char** a)
{
  if (n < 3) return 1;

  TopExp_Explorer ex;
  TopoDS_Shape Sh = DBRep::Get(a[1]);

  if (Sh.IsNull()) return 1;

  setProp(Sh, a, n);
  for (ex.Init (Sh,TopAbs_VERTEX); ex.More(); ex.Next()) {
    TopoDS_Shape S = ex.Current();
    setProp(S, a, n);
  }

  for (ex.Init (Sh,TopAbs_EDGE); ex.More(); ex.Next()) {
    TopoDS_Shape S = ex.Current();
    setProp(S, a, n);
  }

  for (ex.Init (Sh,TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Shape S = ex.Current();
    setProp(S, a, n);
  }

  return 0;
}

//=======================================================================
//memory management
//=======================================================================
static Standard_Integer purgemmgt(Draw_Interpretor&, Standard_Integer , const char**) {
  Standard::Purge();
  return 0;
}
//=======================================================================

//=======================================================================
// check
//=======================================================================

static Standard_Integer check(Draw_Interpretor& ,
			      Standard_Integer n, const char** a)
{
  if (n < 2) return 1;

  Standard_Integer i;
  TopExp_Explorer ex;
  for (i = 1; i < n; i++) {
    TopoDS_Shape S = DBRep::Get(a[i]);
    TopoDS_Shape C;
    if (S.IsNull()) continue;
    for (ex.Init(S,TopAbs_FACE);ex.More();ex.Next()) {
      C = ex.Current();
      C.Checked(Standard_False);
      BRepTools::Update(C);
    }
  }

  return 0;
}

//=======================================================================
// normals
//=======================================================================
static Standard_Integer normals (Draw_Interpretor& theDI,
                                 Standard_Integer  theArgNum,
                                 const char**      theArgs)
{
  if (theArgNum < 2)
  {
    std::cout << "Syntax error: wrong number of arguments!\n";
    theDI.PrintHelp (theArgs[0]);
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get (theArgs[1]);
  if (aShape.IsNull())
  {
    std::cout << "Error: shape with name " << theArgs[1] << " is not found\n";
    return 1;
  }

  Standard_Boolean toUseMesh = Standard_False;
  Standard_Real    aLength   = 10.0;
  Standard_Integer aNbAlongU = 1, aNbAlongV = 1;
  for (Standard_Integer anArgIter = 2; anArgIter< theArgNum; ++anArgIter)
  {
    TCollection_AsciiString aParam (theArgs[anArgIter]);
    aParam.LowerCase();
    if (anArgIter == 2
     && aParam.IsRealValue())
    {
      aLength = aParam.RealValue();
      if (Abs (aLength) <= gp::Resolution())
      {
        std::cout << "Syntax error: length should not be zero\n";
        return 1;
      }
    }
    else if (aParam == "-usemesh"
          || aParam == "-mesh")
    {
      toUseMesh = Standard_True;
    }
    else if (aParam == "-length"
          || aParam == "-len")
    {
      ++anArgIter;
      aLength = anArgIter < theArgNum ? Draw::Atof(theArgs[anArgIter]) : 0.0;
      if (Abs(aLength) <= gp::Resolution())
      {
        std::cout << "Syntax error: length should not be zero\n";
        return 1;
      }
    }
    else if (aParam == "-nbalongu"
          || aParam == "-nbu")
    {
      ++anArgIter;
      aNbAlongU = anArgIter< theArgNum ? Draw::Atoi (theArgs[anArgIter]) : 0;
      if (aNbAlongU < 1)
      {
        std::cout << "Syntax error: NbAlongU should be >=1\n";
        return 1;
      }
    }
    else if (aParam == "-nbalongv"
          || aParam == "-nbv")
    {
      ++anArgIter;
      aNbAlongV = anArgIter< theArgNum ? Draw::Atoi (theArgs[anArgIter]) : 0;
      if (aNbAlongV < 1)
      {
        std::cout << "Syntax error: NbAlongV should be >=1\n";
        return 1;
      }
    }
    else if (aParam == "-nbalong"
          || aParam == "-nbuv")
    {
      ++anArgIter;
      aNbAlongU = anArgIter< theArgNum ? Draw::Atoi (theArgs[anArgIter]) : 0;
      aNbAlongV = aNbAlongU;
      if (aNbAlongU < 1)
      {
        std::cout << "Syntax error: NbAlong should be >=1\n";
        return 1;
      }
    }
    else
    {
      std::cout << "Syntax error: unknwon argument '" << aParam << "'\n";
      return 1;
    }
  }

  DBRep_WriteColorOrientation();

  NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> > > aNormals;
  if (toUseMesh)
  {
    DBRep_DrawableShape::addMeshNormals (aNormals, aShape, aLength);
  }
  else
  {
    DBRep_DrawableShape::addSurfaceNormals (aNormals, aShape, aLength, aNbAlongU, aNbAlongV);
  }

  for (NCollection_DataMap<TopoDS_Face, NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> > >::Iterator aFaceIt (aNormals); aFaceIt.More(); aFaceIt.Next())
  {
    const Draw_Color aColor = DBRep_ColorOrientation (aFaceIt.Key().Orientation());
    for (NCollection_Vector<std::pair<gp_Pnt, gp_Pnt> >::Iterator aNormalsIt (aFaceIt.Value()); aNormalsIt.More(); aNormalsIt.Next())
    {
      const std::pair<gp_Pnt, gp_Pnt>& aVec = aNormalsIt.Value();
      Handle(Draw_Segment3D) aSeg = new Draw_Segment3D(aVec.first, aVec.second, aColor);
      dout << aSeg;
    }
  }

  return 0;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void  DBRep::Set(const Standard_CString Name, const TopoDS_Shape& S)
{
  Handle(DBRep_DrawableShape) D =
    new DBRep_DrawableShape(S,
			    Draw_vert,
			    Draw_jaune,
			    Draw_rouge,
			    Draw_bleu,
			    size,
			    nbIsos,
			    discret);
  D->DisplayTriangulation(disptriangles);
  D->DisplayPolygons(disppolygons);
  D->DisplayHLR(withHLR,withRg1,withRgN,withHid,anglHLR);
  Draw::Set(Name,D);
}
//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
TopoDS_Shape  DBRep::Get(Standard_CString& name,
			 const TopAbs_ShapeEnum typ,
			 const Standard_Boolean complain)
{
  Standard_Boolean pick = name[0] == '.';
  TopoDS_Shape S;
  Handle(DBRep_DrawableShape) D;
  Handle(Draw_Drawable3D) DD = Draw::Get(name,complain);
  if (!DD.IsNull()) 
    D = Handle(DBRep_DrawableShape)::DownCast(DD);
  if (!D.IsNull()) {
    S = D->Shape();
    if (typ != TopAbs_SHAPE) {
      if (typ != S.ShapeType()) {
	// try to find prom pick
	if (pick) {
	  Standard_Real u,v;
	  DBRep_DrawableShape::LastPick(S,u,v);
	}
      } 
      if (typ != S.ShapeType()) {
	if (complain) {
	  cout << name << " is not a ";
	  TopAbs::Print(typ,cout);
	  cout << " but a ";
	  TopAbs::Print(S.ShapeType(),cout);
	  cout << endl;
	}
	S = TopoDS_Shape();
      }
    }
  }
  return S;
}

static Standard_Integer XProgress (Draw_Interpretor& di, Standard_Integer argc, const char **argv)
{
  for ( Standard_Integer i=1; i < argc; i++ ) {
    Standard_Boolean turn = Standard_True;
    if ( argv[i][0] == '-' ) turn = Standard_False;
    else if ( argv[i][0] != '+' ) continue;
    if ( argv[i][1] == 't' ) Draw_ProgressIndicator::DefaultTextMode() = turn;
    else if ( argv[i][1] == 'g' ) Draw_ProgressIndicator::DefaultGraphMode() = turn;
    else if ( ! strcmp ( argv[i], "-stop" ) && i+1 < argc ) {
      Standard_Address aPtr = 0;
      if (sscanf (argv[++i], "%p", &aPtr) == 1)
        Draw_ProgressIndicator::StopIndicator() = aPtr;
      return 0;
    }
  }
  di << "Progress Indicator defaults: text mode is ";
  if ( Draw_ProgressIndicator::DefaultTextMode() ) {
    di<<"ON";
  } else {
    di<<"OFF";
  }
  di<<", graphical mode is ";
  if ( Draw_ProgressIndicator::DefaultGraphMode() ) {
    di<<"ON";
  } else {
    di<<"OFF";
  }
  di<< "\n";
  return 0;
}

//=======================================================================
// binsave
//=======================================================================

static Standard_Integer binsave(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n <= 2) return 1;

  TopoDS_Shape aShape = DBRep::Get (a[1]);
  if (aShape.IsNull())
  {
    di << a[1] << " is not a shape";
    return 1;
  }

  if (!BinTools::Write (aShape, a[2]))
  {
    di << "Cannot write to the file " << a[2];
    return 1;
  }

  di << a[1];
  return 0;
}

//=======================================================================
// binrestore
//=======================================================================

static Standard_Integer binrestore(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n <= 2) return 1;

  TopoDS_Shape aShape;
  if (!BinTools::Read (aShape, a[1]))
  {
    di << "Cannot read from the file " << a[1];
    return 1;
  }

  DBRep::Set (a[2], aShape);
  di << a[2];
  return 0;
}

//=======================================================================
//function : BasicCommands
//purpose  : 
//=======================================================================

static Standard_Boolean done = Standard_False;
void  DBRep::BasicCommands(Draw_Interpretor& theCommands)
{
  if (done) return;
  done = Standard_True;
  Draw::Commands(theCommands);

  const char* g = "Basic shape commands";

  theCommands.Add("isos","isos [name1 ...] [nbisos]",__FILE__,isos,g);
  theCommands.Add("hlr" ,"[no]hlr, rg1, rgn, hid, ang",__FILE__,hlr ,g);
  theCommands.Add("vori","vori [name1 ...], edges are colored by orientation (see vconn)",__FILE__,dispor,g);
  theCommands.Add("triangles", "triangles [name1]..., display triangles of shapes if exists",__FILE__, triangles, g);
  theCommands.Add("tclean", "tclean [name1]..., erase triangulations and polygons on triangulations from shapes",__FILE__, tclean, g); 
  theCommands.Add("polygons", "polygons [name1]..., display polygons of shapes if exists",__FILE__, polygons, g);
  theCommands.Add("vconn","vconn [name1 ...] , edges are colored by number of faces (see vori)",__FILE__,dispor,g);
  theCommands.Add("discretisation","discretisation [nbpoints]",__FILE__,discretisation,g);
  theCommands.Add("compound","compound [name1 name2 ..] compound",__FILE__,compound,g);
  theCommands.Add("add","add name1 name2",__FILE__,add,g);
  theCommands.Add("explode","explode name [Cd/C/So/Sh/F/W/E/V]",__FILE__,explode,g);
  theCommands.Add("nexplode","stable numbered explode for vertex, edge and face: nexplode name [V/E/F]",__FILE__,nexplode,g);
  theCommands.Add("exwire","exwire wirename",__FILE__,exwire,g);
  theCommands.Add("emptycopy","emptycopy [copyshape] originalshape",__FILE__,emptycopy,g);
  theCommands.Add("check","check shape1 shape2 ...",__FILE__,check,g);

  theCommands.Add("orientation","orientation name1 name2.. F/R/E/I",__FILE__,orientation,g);
  theCommands.Add("treverse","treverse name1 name2 ...",__FILE__,orientation,g);
  theCommands.Add("complement","complement name1 name2 ...",__FILE__,orientation,g);
  theCommands.Add("invert","invert name, reverse subshapes",__FILE__,invert,g);
  theCommands.Add("normals","normals shape [Length {10}] [-NbAlongU {1}] [-NbAlongV {1}] [-UseMesh], display normals",__FILE__,normals,g);
  theCommands.Add("nbshapes",
                  "\n nbshapes s - shows the number of sub-shapes in <s>;\n nbshapes s -t - shows the number of sub-shapes in <s> counting the same sub-shapes with different location as different sub-shapes.",
                  __FILE__,nbshapes,g);
  theCommands.Add("numshapes","numshapes s; size of shape",__FILE__,numshapes,g);
  theCommands.Add("countshapes","countshapes s; count of shape",__FILE__,countshapes,g);
  theCommands.Add("setflags",
                  "setflags shape_name flag1[flag2...]\n sets flags for shape(free, modidfied, checked, orientable, closed, infinite, convex, locked), for exmple <setflags a free> or <setflags a -free> if necessary unflag ",
                  __FILE__,setFlags,g);

//  theCommands.Add("dumpmmgt",
//		  "dump le contenu du gestionnaire de memoire",__FILE__,dumpmmgt,g);
  theCommands.Add("purgemmgt",
		  "returns the free memory from the system to the memory manager",
		  __FILE__,purgemmgt,g);
  
  // Add command for DRAW-specific ProgressIndicator
  theCommands.Add ( "XProgress","XProgress [+|-t] [+|-g]: switch on/off textual and graphical mode of Progress Indicator",XProgress,"DE: General");

  theCommands.Add("binsave", "binsave shape filename\n"
                  "\t\tsave the shape in the binary format file",
                  __FILE__, binsave, g);
  theCommands.Add("binrestore", "binrestore filename shape\n"
                  "\t\trestore the shape from the binary format file",
                  __FILE__, binrestore, g);
}

//=======================================================================
//function : HLRMode
//purpose  : 
//=======================================================================

Standard_Boolean DBRep::HLRMode()
{ return withHLR; }

//=======================================================================
//function : Rg1Mode
//purpose  : 
//=======================================================================

Standard_Boolean DBRep::Rg1Mode()
{ return withRg1; }

//=======================================================================
//function : RgNMode
//purpose  : 
//=======================================================================

Standard_Boolean DBRep::RgNMode()
{ return withRgN; }

//=======================================================================
//function : HidMode
//purpose  : 
//=======================================================================

Standard_Boolean DBRep::HidMode()
{ return withHid; }

//=======================================================================
//function : HLRAngle
//purpose  : 
//=======================================================================

Standard_Real DBRep::HLRAngle()
{ return anglHLR; }

//=======================================================================
//function : 
//purpose  : save and restore shapes
//=======================================================================

static Standard_Boolean stest(const Handle(Draw_Drawable3D)& d) 
{
  return d->IsInstance(STANDARD_TYPE(DBRep_DrawableShape));
}

static void ssave(const Handle(Draw_Drawable3D)&d, ostream& OS)
{
  Handle(DBRep_DrawableShape) 
    N = Handle(DBRep_DrawableShape)::DownCast(d);
  BRep_Builder B;
  BRepTools_ShapeSet S(B);
  if(!Draw::GetProgressBar().IsNull())
    S.SetProgress(Draw::GetProgressBar());
  S.Add(N->Shape());
  S.Write(OS);
  if(!Draw::GetProgressBar().IsNull() && Draw::GetProgressBar()->UserBreak())
    return;
  S.Write(N->Shape(),OS);
}

static Handle(Draw_Drawable3D) srestore (istream& IS)
{
  BRep_Builder B;
  BRepTools_ShapeSet S(B);
  if(!Draw::GetProgressBar().IsNull())
    S.SetProgress(Draw::GetProgressBar());
  S.Read(IS);
  Handle(DBRep_DrawableShape) N;
  if(!Draw::GetProgressBar().IsNull() && Draw::GetProgressBar()->UserBreak())
    return N;
  TopoDS_Shape theShape;
  S.Read(theShape,IS );
  N = new DBRep_DrawableShape(theShape,
			    Draw_vert,
			    Draw_jaune,
			    Draw_rouge,
			    Draw_bleu,
			    size,
			    nbIsos,
			    discret);
  N->DisplayTriangulation(disptriangles);
  N->DisplayPolygons(disppolygons);
  N->DisplayHLR(withHLR,withRg1,withRgN,withHid,anglHLR);
  
  return N;
}


static Draw_SaveAndRestore ssr("DBRep_DrawableShape",
			       stest,ssave,srestore);


void dumps (const TopoDS_Shape& S)
{
 BRepTools::Dump(S,cout);
}

//=======================================================================
//function : NbIsos
//purpose  : 
//=======================================================================

Standard_Integer DBRep::NbIsos()
{ return nbIsos; }


//=======================================================================
//function : Discretisation
//purpose  : 
//=======================================================================

Standard_Integer DBRep::Discretisation()
{ return discret; }
