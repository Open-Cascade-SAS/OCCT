// Created on: 1998-12-23
// Created by: Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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

#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_CLASSI.hxx>
#include <TopOpeBRepTool_REGUW.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#define SAME     (-1)
#define DIFF     (-2)
#define UNKNOWN  ( 0)
#define oneINtwo ( 1)
#define twoINone ( 2)

#define M_FORWARD(sta)  (sta == TopAbs_FORWARD)
#define M_REVERSED(sta) (sta == TopAbs_REVERSED)
#define M_INTERNAL(sta) (sta == TopAbs_INTERNAL)
#define M_EXTERNAL(sta) (sta == TopAbs_EXTERNAL)

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepTool_GettraceREGUFA(); 
static TopTools_IndexedMapOfShape STATIC_mapw,STATIC_mapf;
static Standard_Integer FUN_adds(const TopoDS_Shape& s) {
  TopAbs_ShapeEnum typ = s.ShapeType();
  TCollection_AsciiString aa; Standard_Integer is = 0;
  if (typ == TopAbs_WIRE)   {aa = TCollection_AsciiString("wi"); is = STATIC_mapw.Add(s); }
  if (typ == TopAbs_FACE)   {aa = TCollection_AsciiString("fa"); is = STATIC_mapf.Add(s); }
#ifdef DRAW 
  FUN_tool_draw(aa,s,is);  
#endif
  return is;
}
Standard_EXPORT void FUN_tool_coutsta(const Standard_Integer& sta, const Standard_Integer& i1, const Standard_Integer& i2)
{
  switch (sta) {
  case SAME:
    cout<<i1<<" gives SAME bnd with "<<i2<<endl; break;
  case DIFF:
    cout<<i1<<" gives  OUT bnd with "<<i2<<endl; break;
  case oneINtwo:
    cout<<i1<<" is IN "<<i2<<endl; break;
  case twoINone:
    cout<<i2<<" is IN "<<i1<<endl; break;
  }
}
#endif

Standard_EXPORT void FUN_addOwlw(const TopoDS_Shape& Ow, const TopTools_ListOfShape& lw, TopTools_ListOfShape& lresu);

//=======================================================================
//function : RegularizeWires
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool::RegularizeWires(const TopoDS_Face& theFace,
				    TopTools_DataMapOfShapeListOfShape& mapoldWnewW,
				    TopTools_DataMapOfShapeListOfShape& ESplits) // (e,esp); esp = splits of e
{
  if (theFace.IsNull()) return Standard_False;
 TopoDS_Shape aLocalShape = theFace.Oriented(TopAbs_FORWARD);  
  TopoDS_Face aFace = TopoDS::Face(aLocalShape);
//  TopoDS_Face aFace = TopoDS::Face(theFace.Oriented(TopAbs_FORWARD));

  TopOpeBRepTool_REGUW REGUW(aFace);
  REGUW.SetOwNw(mapoldWnewW);
  REGUW.SetEsplits(ESplits);

//  Standard_Boolean hasregu = Standard_False;
  TopExp_Explorer exw(aFace, TopAbs_WIRE);
  for (; exw.More(); exw.Next()) {   
    const TopoDS_Shape& W = exw.Current(); 
    REGUW.Init(W);
    Standard_Boolean ok = REGUW.MapS();
    if (!ok) return Standard_False;
    ok = REGUW.SplitEds();
    if (!ok) return Standard_False;
    ok = REGUW.REGU();
    if (!ok) return Standard_False;
  }

  REGUW.GetEsplits(ESplits); 
  REGUW.GetOwNw(mapoldWnewW);
  return Standard_True;
}

//=======================================================================
//function : Regularize
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool::Regularize(const TopoDS_Face& theFace,
			       TopTools_ListOfShape& aListOfFaces,
			       TopTools_DataMapOfShapeListOfShape& ESplits) 
{   
  TopOpeBRepTool_REGUW REGUW(theFace);
  aListOfFaces.Clear();
  TopTools_DataMapOfShapeListOfShape mapoldWnewW;
  Standard_Boolean regu = TopOpeBRepTool::RegularizeWires(theFace,mapoldWnewW,ESplits);
  if (regu) {
    regu = TopOpeBRepTool::RegularizeFace(theFace,mapoldWnewW,aListOfFaces);
  }
  return regu;
}

//**********************************************************************
//   classifying wires
//**********************************************************************

// ------------------------------------------------------------
// ------------------ classifying wires -----------------------

// function : bFgreaterbFF (F,FF)
// purpose  : returns False if <F>'s 3d bounding box smaller than <FF>'s

// function : mkBnd2d (W,F,B2d)
// purpose  : get 2d bounding box <B2d> of wire <W>'s UV  
//            representation on face <F>.

// function : classiBnd2d (B,ismaller)
// purpose  : compare the two 2d bounding boxes of array <B>
//            if found, B<ismaller> is the smaller one and returns IN
//            else if boxes are disjoint, returns OUT
//            else return UNKNOWN
// function : classiBnd2d (B)
// purpose  : returns SAME,DIFF,UNKNOWN,oneINtwo or twoINone


// function : mkboundedF(W,boundedF)
// purpose  : 

// function : FindAPointInTheFace(F,u,v)
// purpose  : 

// function : GetFP2d(W,boundedF,p2d)
// purpose  : computes <boundedF> the bounded face built up with wire <W>
//            and <p2d> a point in <boundedF>
//            

// function : classiwithp2d(wi)
// purpose  : classify wires (wi(k),k = 1,2)
//            prequesitory : wires of <wi> are not intersecting each other.

// function : ClassifW(F,mapoldWnewW,mapWlow)
// purpose  : all wires described in <mapoldWnewW> are on face <F>
//            <mapoldWnewW> = map with :
//             key = a wire of <F>
//             item = the splits of the previous wire (can be an empty list)
//            the aim is to get map <mapWlow> with :
//             key = a new face's boundary
//             item = wires dexcribing holes in the previous face 
//                    (can be an empty list) 
// ------------------------------------------------------------

/*static TopAbs_State FUN_tool_classiBnd2d(const Bnd_Array1OfBox2d& B,Standard_Integer& ismaller,
				 const Standard_Boolean chklarge = Standard_True)
{
  // purpose : 
  //   Getting <ismaller>, index of the smallest Bnd Box
  //   if B(i) is IN B(j): ismaller = i,
  //                       return IN.
  //   else: ismaller = 1,
  //         if B(1) and B(2) are disjoint, return OUT
  //         if B(1) and B(2) are same, return ON
  //         else return UNKNOWN.
  ismaller = 1;
  
  TColStd_Array2OfReal UV(1,2, 1,4);
  for (Standard_Integer i = 1; i <= 2; i++)
    //      (Umin(i), Vmin(i), Umax(i), Vmax(i))
    B(i).Get(UV(i,1), UV(i,3), UV(i,2), UV(i,4));

#ifdef OCCT_DEBUG
  Standard_Boolean trc = Standard_False;
  if (trc) {
    for (Standard_Integer i = 1; i <= 2; i++)
      cout<<"B("<<i<<") = ("<<UV(i,1)<<" "<<UV(i,3)<<" "<<UV(i,2)<<" "<<UV(i,4)<<")"<<endl;
  }
#endif

  Standard_Boolean smaller, same;
  Standard_Integer ii, jj;
  Standard_Real tol = 1.e-6;

  Standard_Boolean disjoint = Standard_False;
  for (Standard_Integer k = 1; k <= 3; k+=2) { 
    for (i = 1; i <= 2; i++) {
      ii = i; jj = (i == 1) ?  2 : 1;         
      //  diff = Umin<ii> - Umax<jj> : k = 1
      //  diff = Vmin<ii> - Vmax<jj> : k = 3
      Standard_Real diff = UV(ii,k) - UV(jj,k+1);
      // IMPORTANT : for splitted faces sharing same edge, use
      // chklarge = True.
      disjoint = chklarge ? (diff >= -tol) : (diff > 0.);
      if (disjoint) {ismaller = 1; return TopAbs_OUT;}
    }
  }
  
  for (i = 1; i <= 2; i++) {
    ii = i; jj = (i == 1) ? 2 : 1; 
    smaller = same = Standard_True;
    for (Standard_Integer k = 1; k <= 3; k += 2){         
      //  diff = Umin<ii> - Umin<jj> : k = 1        
      //  diff = Vmin<ii> - Vmin<jj> : k = 3 
      Standard_Real diff = UV(ii,k) - UV(jj,k);  
      smaller = chklarge ? (smaller && (diff > -tol)) : (smaller && (diff > 0.));
      same = same && (Abs(diff) <= tol);
    }    
    for (k = 2; k <= 4; k +=2){      
      //  diff = Umax<ii> - Umax<jj> : k = 2        
      //  diff = Vmax<ii> - Vmax<jj> : k = 4
      Standard_Real diff = UV(ii,k) - UV(jj,k); 
      smaller = chklarge ? (smaller && (diff < tol)) : (smaller && (diff < 0.));
      same = same && (Abs(diff) <= tol);
    }

    if (same) return TopAbs_ON;
    if (smaller) {
      ismaller = ii;
      return TopAbs_IN;
    }
  }
  return TopAbs_UNKNOWN;
}

#define SAME     (-1)
#define DIFF     (-2)
#define UNKNOWN  ( 0)
#define oneINtwo ( 1)
#define twoINone ( 2)

Standard_EXPORT Standard_Integer FUN_tool_classiBnd2d(const Bnd_Array1OfBox2d& B,
					 const Standard_Boolean chklarge = Standard_True)
{
  Standard_Integer ismaller;
  TopAbs_State sta = FUN_tool_classiBnd2d(B, ismaller, chklarge);
  Standard_Integer res = -10;
  switch (sta) {
  case TopAbs_IN :
    res = ismaller; break;
  case TopAbs_OUT :
    res = DIFF; break;
  case TopAbs_ON :
    res = SAME; break;
  case TopAbs_UNKNOWN :
    res = UNKNOWN; break;
  }
  return res;
}

static Standard_Boolean FUN_tool_mkboundedF(const TopoDS_Wire& W, TopoDS_Face& boundedF)
{
  BRepLib_MakeFace mf(W, Standard_False);
  Standard_Boolean done = mf.IsDone();
  if (done) boundedF = mf.Face();
  return done;
}
Standard_Boolean FUN_tool_FindAPointInTheFace(const TopoDS_Face& F,
				 Standard_Real& u, Standard_Real& v)
{
  Standard_Boolean ok = BRepClass3d_SolidExplorer::FindAPointInTheFace(F,u,v);
  return ok;
}
static Standard_Boolean FUN_tool_GetFP2d(const TopoDS_Shape& W,
			    TopoDS_Shape& boundedF, gp_Pnt2d& p2d)
{
  Standard_Boolean ok = FUN_tool_mkboundedF(TopoDS::Wire(W), TopoDS::Face(boundedF)); 
  if (!ok) return Standard_False;
  
  Standard_Real u,v; ok = FUN_tool_FindAPointInTheFace(TopoDS::Face(boundedF),u,v);
  if (!ok) return Standard_False;
  p2d = gp_Pnt2d(u,v);
  return Standard_True;
}
static Standard_Integer FUN_tool_classiwithp2d(const TopTools_Array1OfShape& wi)
{
  Standard_Integer stares = UNKNOWN;
  TopTools_Array1OfShape fa(1,2);
  TColgp_Array1OfPnt     p3d(1,2);
  for (Standard_Integer k = 1; k <= 2; k++) {
    gp_Pnt2d p2d;
    Standard_Boolean ok = FUN_tool_GetFP2d(wi(k), fa(k), p2d);
    if (!ok) return UNKNOWN;
    BRepAdaptor_Surface BS(TopoDS::Face(fa(k)));
    p3d(k) = BS.Value(p2d.X(),p2d.Y());
  }

  TopAbs_State sta;
  Standard_Integer i,j; i = j = 0;
  for (Standard_Integer nite = 1; nite <= 2; nite++) {
    i = nite;
    j = (i == 1) ? 2 : 1;
    TopoDS_Face f = TopoDS::Face(fa(j));
    const gp_Pnt p = p3d(i);
    Standard_Real tol = BRep_Tool::Tolerance(f);
    BRepClass_FaceClassifier Fclass(f, p, tol);
    sta = Fclass.State();
    if (sta == TopAbs_IN) break;
  }
  switch (sta) {
  case TopAbs_IN :
    stares = i; break;
  case TopAbs_OUT :
    stares = DIFF; break;
  case TopAbs_ON :
  case TopAbs_UNKNOWN :
    stares = UNKNOWN; break;
  }
  return stares;
}

Standard_EXPORT Standard_Boolean FUN_tool_ClassifW(const TopoDS_Face& F,
				      const TopTools_DataMapOfShapeListOfShape& mapoldWnewW,
				      TopTools_DataMapOfShapeListOfShape& mapWlow)
{
  // NYI : create maps to store Bnd_Box2d, and faces.

#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) cout<<"** ClassifW :"<<endl;
  STATIC_mapw.Clear();    
#endif  

  // Purpose :
  // --------
  // Filling the map <mapWlow> : with (key + item) = new face,
  //  item = (newface has holes) ? list of wires IN the wire key: empty list

  // prequesitory : <mapoldWnewW> binds (non splitted wire of <F>, emptylos)
  //                                    (splitted wire of <F>, splits of the wire)

  // Mapping :
  // --------
  // Filling <oldW> : list of wires of <F>
  // Filling <mapWlow> : with (non-splitted old wire, emptylos),
  //                          (split of old wire, emptylos)
  TopTools_ListOfShape oldW;
  Standard_Integer noldW = mapoldWnewW.Extent();
  Standard_Boolean oneoldW = (noldW == 1);
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape ite(mapoldWnewW);
  TopTools_ListOfShape emptylos;

  // --------------
  // * noldW == 1 :
  // --------------
  if (oneoldW) {
    const TopoDS_Wire& oldwi = TopoDS::Wire(ite.Key());
    const TopTools_ListOfShape& low = ite.Value();
    Standard_Integer nw = low.Extent();
    if (nw == 0) {mapWlow.Bind(oldwi,emptylos); return Standard_True;}
    if (nw == 1) {mapWlow.Bind(low.First(),emptylos); return Standard_True;}

    // <complWoldw> = {(newwire, emptylos)}
    TopTools_DataMapOfShapeListOfShape complWoldw;
    TopTools_ListIteratorOfListOfShape itlw(low);
    for (; itlw.More(); itlw.Next()) complWoldw.Bind(itlw.Value(), emptylos);    

    // iteration on <complWoldw> :
    Standard_Integer ncompl = complWoldw.Extent();
    Standard_Boolean go = Standard_True;
    Standard_Integer nite = 0, nitemax = Standard_Integer(ncompl*(ncompl-1)/2);
    while (go && (nite <= nitemax)){
      Bnd_Array1OfBox2d Bnd2d(1,2);
      TopTools_Array1OfShape wi(1,2);
      
      TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itmap(complWoldw);
      wi(1) = itmap.Key();
      if (ncompl == 1) {
	mapWlow.Bind(wi(1),itmap.Value()); 
	break;
      }
      FUN_tool_mkBnd2d(wi(1), F, Bnd2d(1));      
      Standard_Boolean OUTall = Standard_False;
      Standard_Boolean oneINother = Standard_False; 
      Standard_Integer sma,gre; // dummy if !oneINother

      for (; itmap.More(); itmap.Next()) {	
	wi(2) = itmap.Key();
	if (wi(1).IsSame(wi(2))) continue;
	FUN_tool_mkBnd2d(wi(2), F, Bnd2d(2));

	// sta : wi(1) / wi(2)
	Standard_Integer sta = FUN_tool_classiBnd2d(Bnd2d);
	nite++;
	if ((sta == SAME)||(sta == UNKNOWN)) sta = FUN_tool_classiwithp2d(wi);
#ifdef OCCT_DEBUG	
	if (trc) {cout<<"#wi :";FUN_tool_coutsta(sta,FUN_adds(wi(1)),FUN_adds(wi(2)));}
#endif		
	if ((sta == SAME)||(sta == UNKNOWN)) return Standard_False;    	 
	if ((sta == DIFF) && itmap.More()) {OUTall = Standard_True; continue;}// w(1) OUT w(2)    
	sma = (sta == oneINtwo) ? 1 : 2;
	gre = (sta == oneINtwo) ? 2 : 1;	
	oneINother = Standard_True;
	break;
      } // itmap

      if (oneINother) {
	// Updating map <complWoldw> with : 
	//  - key = wi(gre), 
	//    item += wi(sma) && item += item(wi(sma))
	//  - unbinding key = (wi(sma))
	TopTools_ListOfShape& lwgre = complWoldw.ChangeFind(wi(gre));
	lwgre.Append(wi(sma));	  
	TopTools_ListIteratorOfListOfShape itwsma(complWoldw.Find(wi(sma)));
	for (; itwsma.More(); itwsma.Next()) lwgre.Append(itwsma.Value());
	complWoldw.UnBind(wi(sma));
      }
      else if (OUTall) {mapWlow.Bind(wi(1),emptylos); complWoldw.UnBind(wi(1));}
      else return Standard_False;

      ncompl = complWoldw.Extent();
      go = (ncompl >= 1);
    }
    return Standard_True;
  } // oneoldW


  // -------------
  // * noldW > 1 :
  // -------------
  for (; ite.More(); ite.Next()){
    const TopoDS_Wire& oldwi = TopoDS::Wire(ite.Key());
    const TopTools_ListOfShape& low = ite.Value();
    TopTools_ListIteratorOfListOfShape itlow(low);
    if (low.IsEmpty()) mapWlow.Bind(oldwi, emptylos);
    else
      for (; itlow.More(); itlow.Next()) mapWlow.Bind(itlow.Value(), emptylos);
    oldW.Append(oldwi);
  }
    
  // classifying wires of <mapoldWnewW> :
  // -----------------------------------
  // <Owi>            : old wires
  // <OBnd2d>         : old wires' bounding boxes
  // <Owhassp>(k) : Owi(k) has splits

  TopTools_ListOfShape oldWcopy; oldWcopy.Assign(oldW); 
  for (TopTools_ListIteratorOfListOfShape itoldW(oldW); itoldW.More(); itoldW.Next()) { 
 
    TopTools_Array1OfShape Owi(1,2);
    Bnd_Array1OfBox2d OBnd2d(1,2);
    TColStd_Array1OfBoolean Owhassp(1,2);

    Owi(1) = itoldW.Value();
    if (oldWcopy.Extent() <1) break; 
    oldWcopy.RemoveFirst();
    
    Owhassp(1) = !mapoldWnewW.Find(Owi(1)).IsEmpty();
    Standard_Boolean Owi1notkey = !mapWlow.IsBound(Owi(1));
    if (Owi1notkey && !Owhassp(1)) continue;
    
    FUN_tool_mkBnd2d(Owi(1), F, OBnd2d(1));      

    // Classifying oldwire(i) with oldwires(j): j = i+1..nwiresofF 
    Standard_Integer osma,ogre,osta;
    TopTools_ListIteratorOfListOfShape itoldWcopy(oldWcopy);
    for (; itoldWcopy.More(); itoldWcopy.Next()) {

      TopTools_Array1OfListOfShape newwi(1,2); 
      Owi(2) = TopoDS::Wire(itoldWcopy.Value()); 
      Standard_Boolean Owi2notkey = !mapWlow.IsBound(Owi(2));
      Owhassp(2) = !mapoldWnewW.Find(Owi(2)).IsEmpty(); 
      if (Owi2notkey && !Owhassp(2)) continue;	
      FUN_tool_mkBnd2d(Owi(2), F, OBnd2d(2));
      
      // <osma>, <ogre> :
      // ----------------
      // Classifying Ow<i> with Ow<j> :
      osta = FUN_tool_classiBnd2d(OBnd2d);
      if ((osta == SAME)||(osta == UNKNOWN)) osta = FUN_tool_classiwithp2d(Owi);
#ifdef OCCT_DEBUG	
      if (trc) {cout<<"wi : "; FUN_tool_coutsta(osta,FUN_adds(Owi(1)),FUN_adds(Owi(2)));}
#endif		
      if ((osta == SAME)||(osta == UNKNOWN)) return Standard_False;      
      if (osta == DIFF)                    continue; // Ow(1), Ow(2) are disjoint		
      // Owi<sma> is IN Owi<grea>
      osma = (osta == oneINtwo) ? 1 : 2;
      ogre = (osta == oneINtwo) ? 2 : 1;

      // Owhassp<k>  : newwi<k> = splits (Owi<k>)
      // !Owhassp<k> : newwi<k> = Owi<k> 
      for (Standard_Integer i = 1; i <= 2; i++) {
	const TopoDS_Shape& owi = Owi(i);
	if (!Owhassp(i)) newwi(i).Append(owi);
	else             newwi(i) = mapoldWnewW.Find(owi);
      }

      //
      // classifying wires of newwi<sma> with wires of newwi<gre> :
      //
      Standard_Integer sta, sma, gre;
      TopTools_ListIteratorOfListOfShape itnwi(newwi(osma));
      for (; itnwi.More(); itnwi.Next()) {
	// <wi>    : new wires
	// <Bnd2d> : new wires' bounding boxes
	TopTools_Array1OfShape wi(1,2);
	Bnd_Array1OfBox2d Bnd2d(1,2);
	
	wi(1) = itnwi.Value(); // wi(1) in {newwi(osma)}
	Standard_Boolean wi1notkey = !mapWlow.IsBound(wi(1));
	if (wi1notkey) continue;

	if (!Owhassp(osma)) Bnd2d(1).Add(OBnd2d(osma));
	else               FUN_tool_mkBnd2d(wi(1), F, Bnd2d(1));
	
	TopTools_ListIteratorOfListOfShape itnwj(newwi(ogre));
	for (; itnwj.More(); itnwj.Next()) {

	  wi(2) = itnwj.Value(); // wi(2) in {newwi(ogre)}
	  Standard_Boolean wi2notkey = !mapWlow.IsBound(wi(2));
	  if (wi2notkey) continue;

	  // empty the bounding box
	  Bnd_Box2d newB2d;	  
	  if (!Owhassp(ogre)) newB2d.Add(OBnd2d(ogre));
	  else                FUN_tool_mkBnd2d(wi(2), F, newB2d);
	   FUN_tool_UpdateBnd2d(Bnd2d(2),newB2d);

	  // Classifying wi(1)  with wi(2) :
	  sta = FUN_tool_classiBnd2d(Bnd2d); 
#ifdef OCCT_DEBUG
	  if (trc) {cout<<"wi : "; FUN_tool_coutsta(sta,STATIC_mapw.FindIndex(wi(1)),
						    STATIC_mapw.FindIndex(wi(2)));}
#endif
	  if ((sta == SAME)||(sta == UNKNOWN)) sta = FUN_tool_classiwithp2d(wi);
	  if ((sta == SAME)||(sta == UNKNOWN)) return Standard_False;
	  if (sta == DIFF)                   continue;	
	  // wi<sma> is IN wi<grea>
	  sma = (sta == oneINtwo) ? 1 : 2;
	  gre = (sta == oneINtwo) ? 2 : 1;	
	  
	  // Updating map <mapWlow> with : 
	  //  - key = wi(gre), 
	  //    item += wi(sma) && item += item(wi(sma))
	  //  - unbinding key = (wi(sma))
	  TopTools_ListOfShape& lwgre = mapWlow.ChangeFind(wi(gre));
	  lwgre.Append(wi(sma));	  
	  TopTools_ListIteratorOfListOfShape itwsma(mapWlow.Find(wi(sma)));
	  for (; itwsma.More(); itwsma.Next()) lwgre.Append(itwsma.Value());
	  mapWlow.UnBind(wi(sma));	    
	  break; 
	  // wi<sma> IN wi<gre>, wi<sma> is OUT {newwi<gre>} / wi<gre>
	  // wi<sma> is classified / all newwires.
	} 
      } // itnwi(newwi(sma))
    }// itoldWcopy   
  } // itoldW 
  return Standard_True;
}
// ------------------------------------------------------------
// -------------------- building up faces ---------------------
// ------------------------------------------------------------

Standard_EXPORT Standard_Boolean FUN_tool_MakeFaces(const TopoDS_Face& theFace,
				       TopTools_DataMapOfShapeListOfShape& mapWlow,
				       TopTools_ListOfShape& aListOfFaces)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) cout<<"** MakeFaces :"<<endl;
#endif
  Standard_Boolean toreverse = M_REVERSED(theFace.Orientation());
  TopoDS_Face F = TopoDS::Face(theFace.Oriented(TopAbs_FORWARD));
  BRep_Builder BB;

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(mapWlow);
  for (; itm.More(); itm.Next()) {
    const TopoDS_Wire& wi = TopoDS::Wire(itm.Key());  
    TopoDS_Shape FF = F.EmptyCopied(); BB.Add(FF,wi);
//    BB.MakeFace(FF); // put a TShape
      
    TopTools_ListIteratorOfListOfShape itlow(itm.Value());      
    for (; itlow.More(); itlow.Next()) {
      const TopoDS_Wire& wwi = TopoDS::Wire(itlow.Value());
      BB.Add(FF,wwi);
    }

    if (toreverse) FF.Orientation(TopAbs_REVERSED);
    aListOfFaces.Append(FF);
  }

#ifdef OCCT_DEBUG
  if (trc) {
    cout<<"sp(fa"<<FUN_adds(theFace)<<")=";
    TopTools_ListIteratorOfListOfShape it(aListOfFaces);
    for (; it.More(); it.Next()) cout<<" fa"<<FUN_adds(it.Value());
    cout<<endl;
  }
#endif

  return Standard_True;
}*/

Standard_EXPORT Standard_Boolean FUN_tool_ClassifW(const TopoDS_Face& F,
				      const TopTools_DataMapOfShapeListOfShape& mapOwNw,
				      TopTools_DataMapOfShapeListOfShape& mapWlow)
{  
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUFA();
  if (trc) cout<<"** ClassifW :"<<endl;
#endif  
  Standard_Real tolF = BRep_Tool::Tolerance(F);
  Standard_Real toluv = TopOpeBRepTool_TOOL::TolUV(F,tolF);
  TopoDS_Shape aLocalShape = F.Oriented(TopAbs_FORWARD);  
  TopoDS_Face FFOR = TopoDS::Face(aLocalShape);
//  TopoDS_Face FFOR = TopoDS::Face(F.Oriented(TopAbs_FORWARD));
  TopOpeBRepTool_CLASSI CLASSI; CLASSI.Init2d(FFOR);

  TopTools_ListOfShape null;
  TopTools_ListOfShape oldW;
  Standard_Integer noldW = mapOwNw.Extent();
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(mapOwNw);
  
  // noldW = 1
  // ---------
  if (noldW == 1) {
    const TopTools_ListOfShape& low = itm.Value();
    Standard_Boolean ok = CLASSI.Classilist(low,mapWlow);
    if (!ok) return Standard_False;
    return Standard_True;
  }

  // noldW > 1
  // ---------
  TopTools_ListOfShape lOws;
  for (; itm.More(); itm.Next()){
    const TopoDS_Shape& owi = itm.Key(); 
    lOws.Append(owi);
    const TopTools_ListOfShape& low = itm.Value();
    TopTools_ListOfShape lwresu; FUN_addOwlw(owi,low,lwresu);
    TopTools_ListIteratorOfListOfShape itw(lwresu);
    for (; itw.More(); itw.Next()) mapWlow.Bind(itw.Value(), null);    
  }//itm(mapOwNw)

  TopTools_MapOfShape mapdone;
  Standard_Integer nOw = noldW;
  Standard_Integer nite = 0, nitemax = Standard_Integer(nOw*(nOw-1)/2);
  while (nite <= nitemax){
    nOw = lOws.Extent();
    if (nOw == 0) break;

    TopTools_ListIteratorOfListOfShape itOw(lOws);
    const TopoDS_Shape& Ow1 = itOw.Value();
    Standard_Boolean isb1 = mapWlow.IsBound(Ow1);
    isb1 = isb1 || !mapdone.Contains(Ow1);
    if (!isb1) continue; 

    const TopTools_ListOfShape& lw1 = mapOwNw.Find(Ow1);

    if (nOw == 1) { 
      // all wires of <mapWs> have been treated, except the last one
      // if (nw1 == 0) mapWlow binds already (Ow1,null); 
      // else         {mapWlow binds already (w1k,null), w1k in lw1} 
      break;
    }//nOw == 1

    itOw.Next();
    Standard_Boolean OUTall = Standard_False;
    TopoDS_Shape Ow2;
    Standard_Integer sta12 = UNKNOWN;
    for (; itOw.More(); itOw.Next()){
      Ow2 = itOw.Value();
      Standard_Boolean isb2 = mapWlow.IsBound(Ow2);
      isb2 = isb2 || !mapdone.Contains(Ow2);
      if (!isb2) continue;
      Standard_Integer stabnd2d12 = CLASSI.ClassiBnd2d(Ow1,Ow2,toluv,Standard_True);
      sta12 = CLASSI.Classip2d(Ow1,Ow2, stabnd2d12);
      if      (sta12 == DIFF)                       {OUTall = Standard_True; continue;}
      else if ((sta12 == UNKNOWN)||(sta12 == SAME)) return Standard_False;
      break;
    }
    if (OUTall) {
      // if (nw1 == 0) mapWlow binds already (Ow1,null); 
      // else         {mapWlow binds already (w1k,null), w1k in lw1} 
      TopTools_ListOfShape ldone; FUN_addOwlw(Ow1,lw1,ldone);
      TopTools_ListIteratorOfListOfShape itw(ldone);
      for (; itw.More(); itw.Next()) mapdone.Add(itw.Value());
#ifdef OCCT_DEBUG
      if (trc) cout<<"old wires :wi"<<FUN_adds(Ow1)<<" is OUT all old wires"<<endl;
#endif      
      lOws.RemoveFirst();
    }//OUTall    
    else {
#ifdef OCCT_DEBUG
      if (trc) {cout<<"old wires :wi -> ";
		FUN_tool_coutsta(sta12,FUN_adds(Ow1),FUN_adds(Ow2));
		cout<<endl;}
#endif
      const TopTools_ListOfShape& lw2 = mapOwNw.Find(Ow2);
     
      TopTools_ListOfShape lw1r; FUN_addOwlw(Ow1,lw1,lw1r);
      TopTools_ListOfShape lw2r; FUN_addOwlw(Ow2,lw2,lw2r);
      TopTools_ListOfShape lgre,lsma;
      if (sta12 == oneINtwo) {lgre.Append(lw2r); lsma.Append(lw1r);}
      if (sta12 == twoINone) {lgre.Append(lw1r); lsma.Append(lw2r);}
      
      TopTools_ListIteratorOfListOfShape itsma(lsma);
      for (; itsma.More(); itsma.Next()){
	const TopoDS_Shape& wsma = itsma.Value();
	Standard_Boolean isbsma = mapWlow.IsBound(wsma);
	isbsma = isbsma || !mapdone.Contains(wsma);
	if (!isbsma) continue;
	
	TopTools_ListIteratorOfListOfShape itgre(lgre);
	for (; itgre.More(); itgre.Next()){
	  const TopoDS_Shape& wgre = itgre.Value();
	  Standard_Boolean isbgre = mapWlow.IsBound(wgre);
	  isbgre = isbgre || !mapdone.Contains(wgre);
	  if (!isbgre) continue;
	
	  Standard_Integer stabnd2d = CLASSI.ClassiBnd2d(wsma,wgre,toluv,Standard_True);
	  Standard_Integer sta = CLASSI.Classip2d(wsma,wgre, stabnd2d);
#ifdef OCCT_DEBUG
      if (trc) {cout<<" wires :wi -> ";
		FUN_tool_coutsta(sta,FUN_adds(wsma),FUN_adds(wgre));
		cout<<endl;}
#endif
	
	  if      (sta == DIFF) continue;
	  else if (sta == oneINtwo) {// wsma IN wgre
	    mapWlow.ChangeFind(wgre).Append(mapWlow.ChangeFind(wsma));
	    mapWlow.UnBind(wsma);
	  }
	  else if (sta == twoINone) {// wgre IN wsma
	    mapWlow.ChangeFind(wsma).Append(mapWlow.ChangeFind(wgre));
	    mapWlow.UnBind(wgre);
	  }
	  else return Standard_False;
	}//itgre
      }//itsma
      lOws.RemoveFirst(); 
    } //!OUTall
  }//nite
  return Standard_True;
}

//=======================================================================
//function : RegularizeFace
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool::RegularizeFace(const TopoDS_Face& theFace,
				   const TopTools_DataMapOfShapeListOfShape& mapoldWnewW,
				   TopTools_ListOfShape& newFaces) 
{  
  // <mapWlow> 
  // ---------
  // key = wire <w>,
  // item = if the new face has holes, the item contains wires
  //        classified IN the area described by the boundary <w>
  //        on <aFace>,
  //        else : the item is an empty list, <w> describes the
  //        whole new face.  
  TopTools_DataMapOfShapeListOfShape mapWlow;
  
  // Classifying  wires :
  // -------------------
  //  Standard_Boolean classifok = FUN_tool_ClassifW(theFace, mapoldWnewW, mapWlow);
  TopoDS_Shape aLocalShape = theFace.Oriented(TopAbs_FORWARD);
  TopoDS_Face aFace = TopoDS::Face(aLocalShape);
//  TopoDS_Face aFace = TopoDS::Face(theFace.Oriented(TopAbs_FORWARD));
  
  Standard_Boolean classifok = FUN_tool_ClassifW(aFace, mapoldWnewW, mapWlow);  
  if (!classifok) return Standard_False;
  
  // <aListOfFaces>
  // -------------  
  Standard_Boolean facesbuilt = TopOpeBRepTool_TOOL::WireToFace(theFace, mapWlow, newFaces); 
  if (!facesbuilt) return Standard_False;  
  return Standard_True;
}
