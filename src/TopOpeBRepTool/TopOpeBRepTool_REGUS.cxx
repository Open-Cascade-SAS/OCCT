// Created on: 1999-01-04
// Created by: Xuan PHAM PHU
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopOpeBRepTool_CLASSI.hxx>
#include <TopOpeBRepTool_connexity.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_REGUS.hxx>
#include <TopOpeBRepTool_REGUW.hxx>
#include <TopOpeBRepTool_TOOL.hxx>

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

#define M_FORWARD(ori)  (ori == TopAbs_FORWARD) 
#define M_REVERSED(ori) (ori == TopAbs_REVERSED) 
#define M_INTERNAL(ori) (ori == TopAbs_INTERNAL) 
#define M_EXTERNAL(ori) (ori == TopAbs_EXTERNAL) 

#define FORWARD  (1)
#define REVERSED (2)
#define INTERNAL (3)
#define EXTERNAL (4)
#define CLOSING  (5)

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepTool_GettraceREGUSO(); 
static TopTools_IndexedMapOfShape STATIC_mape, STATIC_mapf, STATIC_mapw, STATIC_mapsh;
static Standard_Integer FUN_adds(const TopoDS_Shape& s) {
  TopAbs_ShapeEnum typ = s.ShapeType();
  TCollection_AsciiString aa; Standard_Integer is = 0;
  if (typ == TopAbs_SHELL)  {aa = TCollection_AsciiString("s"); is = STATIC_mapsh.Add(s);}
  if (typ == TopAbs_WIRE)   {aa = TCollection_AsciiString("w"); is = STATIC_mapw.Add(s); }
  if (typ == TopAbs_FACE)   {aa = TCollection_AsciiString("f"); is = STATIC_mapf.Add(s); }
  if (typ == TopAbs_EDGE)   {aa = TCollection_AsciiString("e"); is = STATIC_mape.Add(s); }
#ifdef DRAW
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO(); 
  if (trc) FUN_tool_draw(aa,s,is);  
#endif
  return is;
}
#endif

static void FUN_Raise()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO(); 
  if (trc) cout<<"***** Failure in REGUS **********"<<endl;
//  throw Standard_Failure("REGUS");
#endif
}

//=======================================================================
//function : Create
//purpose  : 
//=======================================================================

TopOpeBRepTool_REGUS::TopOpeBRepTool_REGUS()
{
  hasnewsplits = Standard_False;
  mynF = myoldnF = 0;

  myFsplits.Clear(); 
  myOshNsh.Clear(); 

  myS.Nullify(); 
  mymapeFs.Clear(); 
  mymapeFsstatic.Clear();
  mymapemult.Clear();

  myedstoconnect.Clear();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUS::Init(const TopoDS_Shape& S)
{
  hasnewsplits = Standard_False;

  mynF = myoldnF = 0;
  myS = S;

  mymapeFs.Clear();
  mymapeFsstatic.Clear();
  mymapemult.Clear();
  myedstoconnect.Clear();
}

//=======================================================================
//function : S
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepTool_REGUS::S() const 
{
  return myS;
}


//=======================================================================
//function : SetFsplits
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUS::SetFsplits(TopTools_DataMapOfShapeListOfShape& Fsplits)
{
  myFsplits = Fsplits;
}
//=======================================================================
//function : GetFsplits
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUS::GetFsplits(TopTools_DataMapOfShapeListOfShape& Fsplits) const 
{
  Fsplits = myFsplits;
}

//=======================================================================
//function : SetOshNsh
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUS::SetOshNsh(TopTools_DataMapOfShapeListOfShape& OshNsh)
{
  myOshNsh = OshNsh;
}
//=======================================================================
//function : GetOshNsh
//purpose  : 
//=======================================================================

void TopOpeBRepTool_REGUS::GetOshNsh(TopTools_DataMapOfShapeListOfShape& OshNsh) const
{
  OshNsh = myOshNsh;
}

//=======================================================================
//function : MapS
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUS::MapS()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO();
  Standard_Integer ish = FUN_adds(S());
  if (trc) cout<<"**    MAPPING    ** shape"<<ish<<endl;
#endif

  // mymapeFs, myoldnF : 
  myoldnF = 0;
  TopExp_Explorer exf(myS, TopAbs_FACE);
  for (; exf.More(); exf.Next()){
    const TopoDS_Shape& f = exf.Current(); myoldnF++;

    TopExp_Explorer exe(f, TopAbs_EDGE);
    for (; exe.More(); exe.Next()){
      const TopoDS_Shape& e = exe.Current();
      Standard_Boolean isb = mymapeFs.IsBound(e);
      if (isb) {mymapeFs.ChangeFind(e).Append(f); mymapeFsstatic.ChangeFind(e).Append(f);}
      else     {TopTools_ListOfShape lof; lof.Append(f); mymapeFs.Bind(e,lof); mymapeFsstatic.Bind(e,lof);}      
    }//exe
  }//exf
  mynF = myoldnF;

  // mymapemult : 
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(mymapeFs);
  for (; itm.More(); itm.Next()){
    const TopoDS_Shape& e = itm.Key();
    const TopTools_ListOfShape& lof  = itm.Value();
    Standard_Integer nf = lof.Extent();
    if (nf > 2) mymapemult.Add(e);
#ifdef OCCT_DEBUG
    if (trc) {
      cout <<"co(e"<<FUN_adds(e)<<")= ";
      TopTools_ListIteratorOfListOfShape it(lof);
      for (; it.More(); it.Next())cout<<" f"<<FUN_adds(it.Value());
      cout<<endl;}
#endif
  }//itm(mymapeFs)
  return Standard_True;
}

//=======================================================================
//function : WireToFace
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUS::WireToFace(const TopoDS_Face& Fanc, const TopTools_ListOfShape& nWs, TopTools_ListOfShape& nFs)
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO();
#endif
  nFs.Clear();
  TopTools_DataMapOfShapeListOfShape mapWlow;
  TopoDS_Shape aLocalShape = Fanc.Oriented(TopAbs_FORWARD);
  TopoDS_Face aFace = TopoDS::Face(aLocalShape);
//  TopoDS_Face aFace = TopoDS::Face(Fanc.Oriented(TopAbs_FORWARD));
  TopOpeBRepTool_CLASSI classi; classi.Init2d(aFace);

  Standard_Boolean classifok = classi.Classilist(nWs,mapWlow);
  if (!classifok) {
#ifdef OCCT_DEBUG
    if (trc) cout<<"** classif fails"<<endl;
#endif
    return Standard_False;
  }
    
  Standard_Boolean facesbuilt = TopOpeBRepTool_TOOL::WireToFace(Fanc, mapWlow, nFs); 
  if (!facesbuilt) {
#ifdef OCCT_DEBUG
    if (trc) cout<<"** facesbuilt fails"<<endl;
#endif
    return Standard_False; 
  }
  return Standard_True;
}


//=======================================================================
//function : SplitF
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUS::SplitF(const TopoDS_Face& Fanc, TopTools_ListOfShape& FSplits) 
{
  // prequesitory : All edges have already been splitted, there is no 
  //                internal vertex on edge, except for internal edge.
  TopAbs_Orientation oAnc = Fanc.Orientation();
  TopoDS_Shape aLocalShapeFromFace = Fanc.Oriented(TopAbs_FORWARD);
  TopoDS_Face aFace = TopoDS::Face(aLocalShapeFromFace);
//  TopoDS_Face aFace = TopoDS::Face(Fanc.Oriented(TopAbs_FORWARD));
  
  FSplits.Clear();

  TopOpeBRepTool_REGUW REGUW(aFace);
  
  TopTools_ListOfShape nWs; Standard_Boolean hassp = Standard_False; 
  TopExp_Explorer exw(aFace, TopAbs_WIRE);
  for (; exw.More(); exw.Next()) {
    const TopoDS_Shape& w = exw.Current();
    REGUW.Init(w);
    REGUW.MapS();

    TopTools_ListOfShape eIs;
    // --------
    TopExp_Explorer exe(w, TopAbs_EDGE);
    for (; exe.More(); exe.Next()){
      const TopoDS_Shape& e = exe.Current();
      if (M_INTERNAL(e.Orientation())) eIs.Append(e);
    }//exe

    TopTools_ListIteratorOfListOfShape ite(eIs);
//    if (!ite.More()) {nWs.Append(w); continue;}

    while (ite.More()) {
      const TopoDS_Edge& eI = TopoDS::Edge(ite.Value());
      TopoDS_Vertex vf,vl; 
      TopoDS_Shape aLocalShape = eI.Oriented(TopAbs_FORWARD);      
      TopExp::Vertices(TopoDS::Edge(aLocalShape),vf,vl);
//      TopExp::Vertices(TopoDS::Edge(eI.Oriented(TopAbs_FORWARD)),vf,vl);
      TopOpeBRepTool_connexity cof; REGUW.Connexity(vf,cof); TopTools_ListOfShape lef; Standard_Integer nef = cof.AllItems(lef);
      TopOpeBRepTool_connexity col; REGUW.Connexity(vl,col); TopTools_ListOfShape lel; Standard_Integer nel = col.AllItems(lel);      
      if ((nef <= 1)||(nel <= 1)) {eIs.Remove(ite);continue;}

      // prequesitory : we do not have internal vertices in edges oriented FOR     
      aLocalShape = eI.Oriented(TopAbs_REVERSED);
      TopoDS_Edge eR = TopoDS::Edge(aLocalShape);
      aLocalShape = eI.Oriented(TopAbs_FORWARD);
      TopoDS_Edge eF = TopoDS::Edge(aLocalShape);
//      TopoDS_Edge eR = TopoDS::Edge(eI.Oriented(TopAbs_REVERSED));
//      TopoDS_Edge eF = TopoDS::Edge(eI.Oriented(TopAbs_FORWARD));

      TopExp_Explorer exv(eI, TopAbs_VERTEX);
      for (; exv.More(); exv.Next()){
	const TopoDS_Vertex& v = TopoDS::Vertex(exv.Current());
	Standard_Boolean ok = REGUW.RemoveOldConnexity(v,INTERNAL,eI);
	if (!ok) return Standard_False;
	Standard_Integer ivF = TopOpeBRepTool_TOOL::OriinSor(v,eF);
	ok = REGUW.AddNewConnexity(v,ivF,eF);
	if (!ok) return Standard_False;
	Standard_Integer ivR = TopOpeBRepTool_TOOL::OriinSor(v,eR);
	ok = REGUW.AddNewConnexity(v,ivR,eR);
	if (!ok) return Standard_False;
	ok = REGUW.UpdateMultiple(v);
	if (!ok) return Standard_False;
      }//exv
      ite.Next();
    }//ite(eIs)

    // now all edges of <eIs> are INTERNAL edges of <w>
    // their 2 bounds are of connexity > 1.
//    if (eIs.IsEmpty()) {nWs.Append(w); continue;}

    TopTools_ListOfShape spW; 
    // --------
    Standard_Boolean spok = REGUW.REGU(); // only first step 
    if (!spok) {FUN_Raise(); return Standard_False;}
    REGUW.GetSplits(spW);
    if (!spW.IsEmpty()) {nWs.Append(spW); hassp = Standard_True;}
  }//exw 

  if (!hassp) return Standard_False;
  TopTools_ListOfShape nFs; Standard_Boolean ok = TopOpeBRepTool_REGUS::WireToFace(aFace, nWs,nFs);
  if (!ok) {FUN_Raise(); return Standard_False;}
  
  TopTools_ListIteratorOfListOfShape itf(nFs);
  for (; itf.More(); itf.Next()) FSplits.Append(itf.Value().Oriented(oAnc));  
  return Standard_True;
}

//=======================================================================
//function : SplitFaces
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUS::SplitFaces()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO();
  Standard_Integer ish = FUN_adds(S());
  if (trc) cout<<"**    SPLITTING FACES    ** shape"<<ish<<endl;
#endif
  TopExp_Explorer exf(myS, TopAbs_FACE);
  for (; exf.More(); exf.Next()){

    // splitting face :
    const TopoDS_Face& f = TopoDS::Face(exf.Current()); 
    TopTools_ListOfShape lfsp; Standard_Boolean issp = TopOpeBRepTool_REGUS::SplitF(f,lfsp);

    if (!issp) continue;

    myFsplits.Bind(f,lfsp);
    
    // updating the map of connexity : 
    // f -> lfsp = {fsp}
    mynF--;
    TopTools_ListIteratorOfListOfShape itf(lfsp);
    for (; itf.More(); itf.Next()){
      const TopoDS_Shape& fsp = itf.Value(); mynF++;

      TopExp_Explorer exe(fsp, TopAbs_EDGE);
      for (; exe.More(); exe.Next()) {
	// fsp -> {e}
	const TopoDS_Shape& e = exe.Current();
	Standard_Boolean isb = mymapeFs.IsBound(e);
	if (!isb) {FUN_Raise(); return Standard_False;}
	
	// <mymapeFs>
	TopTools_ListOfShape& lof = mymapeFs.ChangeFind(e);
	TopOpeBRepTool_TOOL::Remove(lof,f);
	lof.Append(fsp);

	// <mymapemult>
	Standard_Integer nf = lof.Extent();
	if (nf > 2) mymapemult.Add(e);
      }//exe(fsp)
    }//itf(lfsp)

#ifdef OCCT_DEBUG
    if (trc)  {
      cout <<"split(f"<<FUN_adds(f)<<")= ";
      TopTools_ListIteratorOfListOfShape it(lfsp);
      for (; it.More(); it.Next()) cout<<" f"<<FUN_adds(it.Value());
      cout<<endl;}
#endif
  }//exf(myS)
  return Standard_True;  
}

static void FUN_update(const TopoDS_Shape& fcur, TopTools_MapOfShape& edstoconnect)
// purpose : <e> edge of <fcur>
//  1. <e> is INTERNAL or EXTERNAL   -> nothing is done
//  2. <e> is closing edge of <fcur> -> nothing is done
//  3. <e> is already bound in <edstoconnect> -> remove it from the map
//     (then has 2 ancestor faces stored in the current Block)
//  4. elsewhere, add it in the map.
//
// !! if <fcur> is INTERNAL/EXTERNAL -> nothing is done
{
  TopAbs_Orientation ofcur = fcur.Orientation();
  if (M_INTERNAL(ofcur) || M_EXTERNAL(ofcur)) return;

  TopExp_Explorer exe(fcur, TopAbs_EDGE);
  for (; exe.More(); exe.Next()){
    const TopoDS_Shape& e = exe.Current();
    TopAbs_Orientation oe = e.Orientation();
    if (M_INTERNAL(oe) || M_EXTERNAL(oe)) continue;

    Standard_Boolean isclo = TopOpeBRepTool_TOOL::IsClosingE(TopoDS::Edge(e),TopoDS::Face(fcur));
    if (isclo) continue;

    Standard_Boolean isb = edstoconnect.Contains(e);
    if (isb) edstoconnect.Remove(e);
    else     edstoconnect.Add(e);
  }//exe
}

//=======================================================================
//function : REGU
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUS::REGU()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO();
  Standard_Integer ishe = FUN_adds(myS);
  if (trc) cout<<"**    REGU    **"<<ishe<<endl;
#endif
  TopTools_ListOfShape Splits;
  Standard_Boolean toregu = !mymapemult.IsEmpty() || (mynF != myoldnF);
  if (!toregu) return Standard_False;

  // purpose : myS -> {Blocks}, 
  //           a Block is a closed shell with "valid" edges.
  //           - a valid edge in a Block has at most two ancestor faces -
  //
  // Give us the starting couple (<ei>, <fi>) :
  // * If <ei> has only one untouched ancestor face <fj> left, fj+1 <- fj
  //   Else among the untouched ancestors faces, we choose the one for which
  //   angle (<veci>, <vecj>) is the smallest; providing face <fj> reduces
  //   the matter described by <fi>.
  // * update <mymapeFs> for <ei> (<fj> as touched).
  // * Update <mymapemult> for <fi>'s bound edges :
  //   - if bound edge is not in the map, add it.
  //   - else if bound edge has two ancestor faces in current list <mylFinBlock>,
  //     delete it form the map.
  //
//  TopTools_ListOfShape lFinBlock; // <lFinBlock> describes a valid closed shell when <myedstoconnect> is emptied.   
  mylFinBlock.Clear();
  Standard_Integer nite = 0;
  while (nite <= mynF) {
    Standard_Boolean startBlock = mylFinBlock.IsEmpty();
    Standard_Boolean endBlock = myedstoconnect.IsEmpty() && (!startBlock);

#ifdef OCCT_DEBUG
    Standard_Boolean tr=Standard_False;
    if (tr) {
      TopTools_MapIteratorOfMapOfShape it(myedstoconnect);
      cout<<"still to connect : ";
      for (; it.More(); it.Next()) cout<<" e"<<FUN_adds(it.Key());
      cout<<endl;
    }
#endif

    //* endBlock
    // ---------
    if (endBlock) {
      // building up shell on <mylFinBlock>
      Standard_Integer nFcur = mylFinBlock.Extent();
      Standard_Boolean unchanged = (nFcur==myoldnF) && (mynF==myoldnF);
      if (unchanged) {
#ifdef OCCT_DEBUG
	if (trc) cout<<"#** shell"<<ishe<<" valid\n";
#endif
	return Standard_False; // nyi analysis if we should raise or not  
      }
      else {
	TopoDS_Shell newShe; TopOpeBRepTool_TOOL::MkShell(mylFinBlock,newShe);
	Splits.Append(newShe);
#ifdef OCCT_DEBUG
	if (trc) {cout<<"#** shell "<<ishe<<" gives new shell "<<FUN_adds(newShe)<<endl;
		  for (TopTools_ListIteratorOfListOfShape it(mylFinBlock); it.More(); it.Next()) cout <<";dins f"<<FUN_adds(it.Value());
		  cout<<endl<<endl;}
#endif
	mylFinBlock.Clear(); 
	startBlock = Standard_True;
      }
    }//endBlock

    //* all faces touched
    // ------------------
    Standard_Boolean FINI = (nite == mynF);
    if (FINI) break;

    Standard_Integer advance=Standard_False;
    //* initializing a new Block
    // -------------------------
    if (startBlock || endBlock) {
      advance = InitBlock();
      if (!advance) return Standard_False;
    }//startBlock||endBlock

    //* choosing next face
    // -------------------
    else {
     advance  = NextinBlock();      
    }

    // ** updating connexity     
    ::FUN_update(myf,myedstoconnect);

    if (!advance) {
      endBlock = myedstoconnect.IsEmpty() && (!startBlock);
      if (!endBlock) return Standard_False;
      else           continue;
    }
    
    TopExp_Explorer exe(myf, TopAbs_EDGE);
    for (; exe.More(); exe.Next()){
      const TopoDS_Shape& e = exe.Current();
      Standard_Boolean isb = mymapeFs.IsBound(e);
      if (!isb) continue; // ancestors faces of <e> are stored in Blocks	
      TopOpeBRepTool_TOOL::Remove(mymapeFs.ChangeFind(e),myf);
    }//exe        

    mylFinBlock.Append(myf); nite++;
  }//nite <= mynF

  myOshNsh.Bind(S(), Splits);
  return Standard_True;
}
 
//=======================================================================
//function : InitBlock
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUS::InitBlock()
{
  Standard_Integer nec = myedstoconnect.Extent();
  if (nec != 0) return Standard_False; // should be empty

  TopTools_ListOfShape eds;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itm(mymapeFs);
  for (; itm.More(); itm.Next()) eds.Append(itm.Key());

  TopTools_ListIteratorOfListOfShape ite(eds);
  for (; ite.More(); ite.Next()){
    const TopoDS_Shape& e = ite.Value();
    const TopTools_ListOfShape& lof = mymapeFs.Find(e);
    if (lof.IsEmpty()) {mymapeFs.UnBind(e); continue;}
    myf = lof.First(); 
#ifdef OCCT_DEBUG
    Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO(); 
    if (trc) cout<<"* Block : first face = f"<<FUN_adds(myf)<<endl;
#endif 
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : NextinBlock
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUS::NextinBlock()
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO(); 
#endif
  // we try to connect first edge of <myf> bound in <myedstoconnect> 
  TopTools_ListOfShape eds;
  TopExp_Explorer exe(myf, TopAbs_EDGE);
  for (; exe.More(); exe.Next()){
    const TopoDS_Shape& e = exe.Current();
    Standard_Boolean isb = myedstoconnect.Contains(e);
    if (isb) eds.Append(e);
  }//exe
  Standard_Boolean alleftouched = eds.IsEmpty();
  if (alleftouched) {
    TopTools_MapIteratorOfMapOfShape itc(myedstoconnect);
    for (; itc.More(); itc.Next()){
      const TopoDS_Shape& e = itc.Key();
      Standard_Boolean isBound = mymapeFs.IsBound(e);
      // all ancestor faces of <e> have been stored
      if (!isBound)    {myedstoconnect.Remove(e); continue;}

      const TopTools_ListOfShape& lof = mymapeFs.Find(e); Standard_Integer nf = lof.Extent();
      if (nf == 0) {myedstoconnect.Remove(e); mymapeFs.UnBind(e);
		    continue;}

//      myf = lof.First(); 130499
      if (lof.Extent() == 1) myf = lof.First();
      else {
	// looking for first face stored in the current block
	// connexed to e

	TopTools_ListIteratorOfListOfShape itff(mylFinBlock);
	TopTools_MapOfShape mapf;for (; itff.More(); itff.Next()) mapf.Add(itff.Value());	
	// lofc : the list of faces connexed to e in <myS>
	// lof  : the list of untouched faces connexed to e in <myS>	
	const TopTools_ListOfShape& lofc = mymapeFsstatic.Find(e);

	itff.Initialize(lofc);
	TopoDS_Face fref;
	for (; itff.More(); itff.Next()) {
	  const TopoDS_Face& fc = TopoDS::Face(itff.Value());
	  Standard_Boolean isb = mapf.Contains(fc);
	  if (isb) {fref = fc; break;}
	} // itff(lofc)
	if (fref.IsNull()) {
	  return Standard_False; // !!!!!!!!!! a revoir 130499
	}
	else {
	  myf = fref;
	  TopoDS_Face ffound; Standard_Boolean ok = NearestF(TopoDS::Edge(e),lof,ffound);
	  if (!ok) return Standard_False;
	  myf = ffound;
	}
      }

      return Standard_True;
    }
    return Standard_False;
  }

  TopTools_ListIteratorOfListOfShape ite(eds);
  for (; ite.More(); ite.Next()){
    const TopoDS_Shape& e = ite.Value();
    Standard_Boolean isb = mymapeFs.IsBound(e);
    // all ancestor faces of <e> have been stored
    if (!isb)    {myedstoconnect.Remove(e); continue;}

    const TopTools_ListOfShape& lof = mymapeFs.Find(e); Standard_Integer nf = lof.Extent();
    if (nf == 0) {myedstoconnect.Remove(e); mymapeFs.UnBind(e);
		  continue;}
#ifdef OCCT_DEBUG
    if (trc) {cout<<"e"<<FUN_adds(e)<<" on "<<nf<<" untouched f:"<<endl;}
#endif
    if (nf == 1) myf = lof.First();
    else {
      TopoDS_Face ffound; Standard_Boolean ok = NearestF(TopoDS::Edge(e),lof,ffound);
      if (!ok) return Standard_False;
      myf = ffound;
    }
#ifdef OCCT_DEBUG
    if (trc) cout<<"->myf = f"<<FUN_adds(myf)<<endl;
#endif
    return Standard_True;
  }//itm(myedstoconnect)
  return Standard_False;
}

static Standard_Boolean FUN_vectors(const TopoDS_Face& f, const TopoDS_Edge& e, const Standard_Real pare,
		       gp_Dir& nt, gp_Dir& xx, const Standard_Real tola, const Standard_Boolean approx)
{
  // <nt> :  
  if (approx) {
    Standard_Boolean ok = TopOpeBRepTool_TOOL::tryNgApp(pare,e,f,tola,nt);  
    if (!ok) return Standard_False; 
  }  
  else   {
    gp_Vec tmp; Standard_Boolean  ok = FUN_tool_nggeomF(pare,e,f,tmp);
    if (!ok) return Standard_False; 
    nt = gp_Dir(tmp); 
  }
  if (M_REVERSED(f.Orientation())) nt.Reverse();
  // <xx> : 
  Standard_Boolean ok = FUN_tool_getxx(f,e,pare,xx);
  if (!ok) return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : NearestF
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_REGUS::NearestF(const TopoDS_Edge& e, const TopTools_ListOfShape& lof, TopoDS_Face& ffound) const
// prequesitory : <e> is shared by <myf> and faces of <lof>.  
//
// NYIXPU!!!!!!!! if (xx1 tg xx2) -> use curvatures
//
{
#ifdef OCCT_DEBUG
  Standard_Boolean trc = TopOpeBRepTool_GettraceREGUSO(); 
#endif
  ffound.Nullify();
  TopoDS_Face fref = TopoDS::Face(myf);

  // Give us egde <e>, and a reference face <fref> (= <myf>)
  // - parameter on <e> = <pare>.
  // - xxi = tangent fo face fi at pnt(e,pare) oriented INSIDE 2d(fi)
  //        normal to tge = tg(e,pare).
  // purpose : looking for ffound / 
  //  MatterAng(xxref, xxfound) = Min{ MatterAng(xxref, xxi), xxi for fi in <lof> 
  //                                 providing fi reduces 3d(fref) }
  
  // <parone> :
  Standard_Real f,l; FUN_tool_bounds(e,f,l); Standard_Real eps = 0.45678; Standard_Real pare = (1-eps)*f+eps*l;

  // RONd (x,y,z) = (xxref,ntref,x^y)
  Standard_Real tola = Precision::Angular()*1.e3; //gp_Dir xapp,yapp; Standard_Boolean refapp = Standard_False;
  gp_Dir x,y; Standard_Boolean ok = ::FUN_vectors(fref,e,pare,y,x,tola,Standard_False);
  if (!ok) {FUN_Raise(); return Standard_False;}
  
  // initializing
  // ------------
  Standard_Real angfound = 0;   
  TopTools_ListIteratorOfListOfShape itf(lof);
  for (; itf.More(); itf.Next()){
    ffound = TopoDS::Face(itf.Value());
    gp_Dir ntfound,xxfound;
    ok = ::FUN_vectors(ffound,e,pare,ntfound,xxfound,tola,Standard_False);
    if (!ok) {FUN_Raise(); return Standard_False;}

    Standard_Boolean oppo = TopOpeBRepTool_TOOL::Matter(x,y,xxfound,ntfound,tola, angfound);
#ifdef OCCT_DEBUG
    if (trc&&!oppo) cout<<"   f"<<FUN_adds(fref)<<",f"<<FUN_adds(ffound)<<" not oppo"<<endl;
#endif
    if (!oppo) {ffound.Nullify(); continue;}
    
    if (angfound < tola) { 
//      refapp = Standard_True; ::FUN_vectors(fref,e,pare,yapp,xapp,tola,Standard_True);
//      ::FUN_vectors(ffound,e,pare,ntfound,xxfound,tola,Standard_True);
//      TopOpeBRepTool_TOOL::Matter(xapp,yapp,xxfound,ntfound,tola, angfound);
      ok = TopOpeBRepTool_TOOL::MatterKPtg(fref,ffound,e,angfound);
      if (!ok) {FUN_Raise(); return Standard_False;}
    }
#ifdef OCCT_DEBUG
    if (trc) cout<<"   ang(f"<<FUN_adds(fref)<<",f"<<FUN_adds(ffound)<<")="<<angfound<<endl;
#endif
    break;
  }
  if (ffound.IsNull()) {FUN_Raise(); return Standard_False;}
  if (itf.More()) itf.Next();
  else            return Standard_True;

  // selecting nearest face
  // ----------------------
  for (; itf.More(); itf.Next()){
    gp_Dir nti,xxi;
    const TopoDS_Face& fi = TopoDS::Face(itf.Value());
    ok = ::FUN_vectors(fi,e,pare,nti,xxi,tola,Standard_False);
    if (!ok) {FUN_Raise(); return Standard_False;}
        
    Standard_Real angi = 0; 
    Standard_Boolean oppo = TopOpeBRepTool_TOOL::Matter(x,y,xxi,nti,tola, angi);
#ifdef OCCT_DEBUG
    if (trc&&!oppo) cout<<"   f"<<FUN_adds(fref)<<",f"<<FUN_adds(fi)<<" not oppo"<<endl;
#endif
    if (!oppo) continue;    

    if (angi < tola) {
//      if (!refapp) ::FUN_vectors(fref,e,pare,yapp,xapp,tola,Standard_True);
//      ::FUN_vectors(fi,e,pare,nti,xxi,tola,Standard_True);
//      TopOpeBRepTool_TOOL::Matter(xapp,yapp,xxi,nti,tola, angi);
      ok = TopOpeBRepTool_TOOL::MatterKPtg(fref,fi,e,angi);
      if (!ok) {FUN_Raise(); return Standard_False;}
    }
#ifdef OCCT_DEBUG
    if (trc) cout<<"   ang(f"<<FUN_adds(fref)<<",f"<<FUN_adds(fi)<<")="<<angi<<endl;
#endif
    if (angi > angfound) continue; 
    angfound = angi;
    ffound = fi;
  }
  return Standard_True;
}



