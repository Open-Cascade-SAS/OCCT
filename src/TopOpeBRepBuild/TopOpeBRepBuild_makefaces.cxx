// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_2d.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GetcontextNOPURGE();
extern Standard_Boolean TopOpeBRepBuild_GetcontextNOCORRISO();
extern Standard_Boolean TopOpeBRepBuild_GettraceCHK();
#define DEBSHASET(sarg,meth,shaset,str) \
TCollection_AsciiString sarg((meth));(sarg)=(sarg)+(shaset).DEBNumber()+(str);
Standard_EXPORT void debgfabu(const Standard_Integer i) {cout<<"++ debgfabu "<<i<<endl;}
Standard_EXPORT void debwesmf(const Standard_Integer i) {cout<<"++ debwesmf "<<i<<endl;}
Standard_EXPORT Standard_Boolean DEBpurclo = Standard_False;
void debpurclo() {}
void debpurclomess(Standard_Integer i){cout<<"++ debpurclo "<<i<<endl;debpurclo();}
Standard_EXPORT void debcorriso(const Standard_Integer i) {cout<<"++ debcorriso "<<i<<endl;}
extern void* GFABUMAKEFACEPWES_DEB;
#endif

#ifdef DRAW
#include <DBRep.hxx>
#include <TopOpeBRepTool_DRAW.hxx>
#endif

Standard_EXPORT Standard_Boolean FUN_tool_ClosedW(const TopoDS_Wire& W);
// Unused :
/*#ifdef OCCT_DEBUG
static void FUN_Raise(){cout<<"--------- ERROR in GWESMakeFaces ---------"<<endl;}
#endif*/

//=======================================================================
//function : GWESMakeFaces
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GWESMakeFaces
(const TopoDS_Shape& FF,TopOpeBRepBuild_WireEdgeSet& WES,TopTools_ListOfShape& LOF)  
{
#ifdef OCCT_DEBUG
  Standard_Integer iF; Standard_Boolean tSPS = GtraceSPS(FF,iF);
  DEBSHASET(s,"#--- GWESMakeFaces ",WES," ");
  if(tSPS){ GdumpSHA(FF,(Standard_Address)s.ToCString());cout<<endl; WES.DumpSS();}
  if(tSPS){debwesmf(iF);}
  GFABUMAKEFACEPWES_DEB = (void*)&WES;
#endif
  
  const Standard_Boolean ForceClass = Standard_True;
  TopOpeBRepBuild_FaceBuilder FABU;
  FABU.InitFaceBuilder(WES,FF,ForceClass);
  
  // Wire checking,the aim is to rebuild faces having
  // edges unconnected to the others (in the face UV representation)
  // This can occur when the face has a closing edge. To avoid this,
  // we delete the lonesome closing edge from the wire.
  Standard_Boolean topurge = Standard_True;
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GetcontextNOPURGE()) topurge = Standard_False;
#endif

#ifdef DRAW
  Standard_Boolean traceF = Standard_False;
  if (traceF) {
    TopTools_IndexedMapOfShape mapW;
    for (FABU.InitFace(); FABU.MoreFace(); FABU.NextFace()) {
      for (FABU.InitWire(); FABU.MoreWire(); FABU.NextWire()) {      
	TopoDS_Shape W;
	Standard_Boolean isold = FABU.IsOldWire();
	if (isold) W = FABU.OldWire();
	else {
	  BRep_Builder BB;
	  TopoDS_Compound cmp; BB.MakeCompound(cmp);
	  FABU.InitEdge();
	  for (; FABU.MoreEdge(); FABU.NextEdge()) FABU.AddEdgeWire(FABU.Edge(),cmp);
	  W = cmp;
	}
	if (W.IsNull()) continue;
	Standard_Integer iiwi = mapW.Add(W);	TCollection_AsciiString aa("wii_");FUN_tool_draw(aa,W,iiwi);
      }
    }
  }
#endif
  
  if (topurge) {
    TopOpeBRepDS_DataStructure& BDS = myDataStructure->ChangeDS();
    
    TopTools_IndexedMapOfShape mapPIE; // pseudo internal edges
    FABU.DetectPseudoInternalEdge(mapPIE);

    TopTools_IndexedDataMapOfShapeShape mapVVsameG,mapVon1Edge,mapVVref; 
    FABU.DetectUnclosedWire(mapVVsameG,mapVon1Edge);
    
    Standard_Integer nVV = mapVVsameG.Extent();
    if (nVV > 0) {
      // Updating the DS with same domain vertices,
      // filling up map <mapVVref>
      for (Standard_Integer i = 1; i <= nVV; i++) {
	const TopoDS_Shape& V = mapVVsameG.FindKey(i);
	Standard_Boolean hsdm = myDataStructure->HasSameDomain(V);
	if (!hsdm) {
	  Standard_Integer rankV = BDS.AncestorRank(V);
	  
	  const TopoDS_Shape& VsameG = mapVVsameG.FindFromIndex(i);

          // MSV Oct 4, 2001: prefer old vertex as SameDomainReference
	  Standard_Integer rankVsameG = BDS.AncestorRank(VsameG);
          Standard_Boolean otherRef = (rankVsameG != 0 && rankV != 1);

	  if (otherRef)
	    BDS.FillShapesSameDomain(VsameG,V);
          else
	    BDS.FillShapesSameDomain(V,VsameG);

	  hsdm = myDataStructure->HasSameDomain(V);  
	}
	if (hsdm) {
	  Standard_Integer Iref = myDataStructure->SameDomainReference(V);
	  const TopoDS_Shape& Vref = myDataStructure->Shape(Iref);
	  mapVVref.Add(V,Vref);
	}
      }
      FABU.CorrectGclosedWire(mapVVref,mapVon1Edge);
      FABU.DetectUnclosedWire(mapVVsameG,mapVon1Edge);
    }    
  }
    
  TopTools_DataMapOfShapeInteger MWisOld;
  TopTools_IndexedMapOfOrientedShape MshNOK;
  GFABUMakeFaces(FF,FABU,LOF,MWisOld);
  
  // 2.  on periodic face F :
  //   finds up faulty shapes MshNOK to avoid when building up shapes
  //   (edge no longer closing that appears twice in the new face)

  if (topurge) {

#ifdef DRAW
    if (tSPS) {
      cout<<endl<<"#<< AVANT PurgeClosingEdges "<<endl; 
      GdumpFABU(FABU);
      TopTools_ListOfShape dLOF;TopTools_DataMapOfShapeInteger dMWisOld;
      GFABUMakeFaces(FF,FABU,dLOF,dMWisOld);
      TopTools_ListIteratorOfListOfShape X(dLOF); for (Standard_Integer i=1;X.More();X.Next(),i++) {
	TCollection_AsciiString ss("purclo");ss=ss+i;DBRep::Set(ss.ToCString(),X.Value());
	cout<<"... face "<<ss<<endl;
      }
      debpurclomess(iF);
      DEBpurclo = Standard_True;
    }
#endif
    
    const TopoDS_Face& FA = TopoDS::Face(FF);
    Standard_Boolean puok = TopOpeBRepTool::PurgeClosingEdges(FA,LOF,MWisOld,MshNOK);
    if (!puok) throw Standard_Failure("TopOpeBRepBuild::GWESMakeFaces");
    topurge = !MshNOK.IsEmpty();

#ifdef OCCT_DEBUG
    if (tSPS) DEBpurclo = Standard_False;
#endif
  } // topurge

  if (topurge) {
    TopTools_ListOfShape LOFF;
    Standard_Boolean puok = TopOpeBRepTool::MakeFaces(TopoDS::Face(FF),LOF,MshNOK,LOFF);
    if (!puok) throw Standard_Failure("TopOpeBRepBuild::GWESMakeFaces");
    LOF.Clear(); LOF.Assign(LOFF);
  }

  //1.  on periodic face F :
  //  translates edge's pcurve to have it in F's UVbounds  
  //  translates edge's pcurve to have it connexed to others in UV space 
  Standard_Boolean corronISO = Standard_True;
#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GetcontextNOCORRISO()) corronISO = Standard_False;
  if (tSPS) debcorriso(iF);
#endif
  Standard_Boolean ffcloseds = FUN_tool_closedS(FF);
  corronISO = corronISO && ffcloseds;
  if (corronISO) {
    TopTools_ListIteratorOfListOfShape itFF(LOF);
    TopTools_ListOfShape newLOF;
    const TopoDS_Face& FFa = TopoDS::Face(FF);
    for (; itFF.More(); itFF.Next()){
      TopoDS_Face Fa = TopoDS::Face(itFF.Value());
      TopOpeBRepTool::CorrectONUVISO(FFa,Fa);
      newLOF.Append(Fa);
    }
    LOF.Clear(); LOF.Assign(newLOF);
  }

  // xpu280898 : regularisation after GFABUMakeFaces,purge processings
  TopTools_ListOfShape newLOF; RegularizeFaces(FF,LOF,newLOF);
  LOF.Clear(); LOF.Assign(newLOF);

} // GWESMakeFaces

//------------------------------------------------------
// retourne vrai si newFace contient une seule arete non orientee
//------------------------------------------------------
static Standard_Boolean FUN_purgeFon1nonoriE(const TopoDS_Shape& newFace)
{
  TopExp_Explorer ex(newFace,TopAbs_EDGE);
  Standard_Integer nE = 0;
  for (; ex.More(); ex.Next()) nE++;
  if (nE == 1) {
    ex.Init(newFace,TopAbs_EDGE);
    const TopoDS_Shape& ed = ex.Current();
    TopAbs_Orientation ori = ed.Orientation();
    Standard_Boolean hasori = (ori == TopAbs_FORWARD) || (ori == TopAbs_REVERSED);
    if (!hasori) return Standard_True;
    //// modified by jgv, 6.06.02 for OCC424 ////
    TopoDS_Edge theEdge = TopoDS::Edge( ed );
    if (BRep_Tool::Degenerated( theEdge ))
      return Standard_True;
    /////////////////////////////////////////////
  }
  return Standard_False;
}

//-- ofv --------------------------------------------------------------------
// function : FUN_ReOrientIntExtEdge
// purpose  : change orientation of INTERNAL (EXTERNAL) edge, if necessary 
//            FRE  - tool (edge with FORWARD or REVERSED orientation),
//            OFRE - orientation of tool (FORWARD or REVERSED),
//            INE  - object (edge with INTERNAL (EXTERNAL) orientation)
//---------------------------------------------------------------------------
static TopAbs_Orientation FUN_ReOrientIntExtEdge(const TopoDS_Edge& FRE,
						 TopAbs_Orientation OFRE,
						 const TopoDS_Edge& INE)
{
  TopAbs_Orientation result = INE.Orientation();
  TopoDS_Vertex Vf1,Vl1,Vf2,Vl2;

  TopExp::Vertices(FRE, Vf1, Vl1, Standard_False);
  TopExp::Vertices(INE, Vf2, Vl2, Standard_False);

  if(OFRE == TopAbs_FORWARD)
    {
      if(Vl1.IsSame(Vf2)) result = TopAbs_FORWARD;
      if(Vl1.IsSame(Vl2)) result = TopAbs_REVERSED;
      if(Vf1.IsSame(Vf2)) result = TopAbs_REVERSED;
      if(Vf1.IsSame(Vl2)) result = TopAbs_FORWARD;
    }
  if(OFRE == TopAbs_REVERSED)
    {
      if(Vl1.IsSame(Vf2)) result = TopAbs_REVERSED;
      if(Vl1.IsSame(Vl2)) result = TopAbs_FORWARD;
      if(Vf1.IsSame(Vf2)) result = TopAbs_FORWARD;
      if(Vf1.IsSame(Vl2)) result = TopAbs_REVERSED;
    }
  return result;
}
//----------------------------------------------------------------------------

//-- ofv --------------------------------------------------------------------
// function : FUN_CheckORI
// purpose  :
//----------------------------------------------------------------------------
static Standard_Integer FUN_CheckORI(TopAbs_Orientation O1,
				     TopAbs_Orientation O2)
{
  Standard_Integer result;
  if((O1 == TopAbs_INTERNAL || O1 == TopAbs_EXTERNAL) && (O2 == TopAbs_INTERNAL || O2 == TopAbs_EXTERNAL)) result = 0;
  else if((O1 == TopAbs_INTERNAL || O1 == TopAbs_EXTERNAL) && (O2 == TopAbs_FORWARD || O2 == TopAbs_REVERSED)) result = 1;
  else if((O1 == TopAbs_FORWARD || O1 == TopAbs_REVERSED) && (O2 == TopAbs_INTERNAL || O2 == TopAbs_EXTERNAL)) result = 2;
  else result = 4;
  return result;
}
//----------------------------------------------------------------------------
//=======================================================================
//function : GFABUMakeFaces
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::GFABUMakeFaces(const TopoDS_Shape& FF,TopOpeBRepBuild_FaceBuilder& FABU,
			  TopTools_ListOfShape& LOF,TopTools_DataMapOfShapeInteger& MWisOld)
{
#ifdef OCCT_DEBUG
  Standard_Integer iF;Standard_Boolean tSPS=GtraceSPS(FF,iF);
  if(tSPS) {
    cout<<endl;GdumpSHA(FF,(char *) "#--- GFABUMakeFaces ");cout<<endl;
    GdumpFABU(FABU);debgfabu(iF);
  }
#endif
  
  TopTools_ListOfShape lnewFace;
  TopoDS_Face newFace;
  TopoDS_Wire newWire;
  
  TopLoc_Location Loc;
  Handle(Geom_Surface) Surf = BRep_Tool::Surface(TopoDS::Face(FF),Loc);
// JYL : mise en // des 5 lignes suivantes pour reprendre la correction de DPF
//       du 29/07/1998
//  GeomAdaptor_Surface GAS1(Surf);
//  GeomAbs_SurfaceType tt1 = GAS1.GetType();
//  Handle(Standard_Type) T = Surf->DynamicType();
//  Standard_Boolean istrim = ( T == STANDARD_TYPE(Geom_RectangularTrimmedSurface) );
//  if ( istrim && tt1 == GeomAbs_Plane) Surf = Handle(Geom_RectangularTrimmedSurface)::DownCast(Surf)->BasisSurface();
  Standard_Real tolFF = BRep_Tool::Tolerance(TopoDS::Face(FF));
  BRep_Builder BB;


  //--ofv:
  //       Unfortunately, the function GFillONPartsWES2() from file TopOpeBRepBuild_BuilderON.cxx sets orientation of
  //       some section edges as INTERNAL or EXTERNAL, but they should be FORWARD or REVERSED. It probably makes faces
  //       without closed boundary, for example. So, we must check carefuly edges with orientation INTERNAL(EXTERNAL).
  //       Bugs: 60936, 60937, 60938 (cut, fuse, common shapes)
  TopoDS_Compound CmpOfEdges;
  BRep_Builder BldCmpOfEdges;
  TopTools_IndexedDataMapOfShapeListOfShape mapVOE;
  TopoDS_Face tdF = TopoDS::Face(FF);
  //--ofv.

  FABU.InitFace();
  for (; FABU.MoreFace(); FABU.NextFace())
    {
      Standard_Integer nbnewWwithe = 0;
      Standard_Integer nboldW = 0;
    
      BB.MakeFace(newFace,Surf,Loc,tolFF);
//    myBuildTool.CopyFace(FF,newFace);

      Standard_Integer nbw = FABU.InitWire();
      for (; FABU.MoreWire(); FABU.NextWire())
	{
	  Standard_Integer ne = 0;
	  Standard_Integer neFORWARD  = 0;
	  Standard_Integer neREVERSED = 0;
	  Standard_Integer neINTERNAL = 0;
	  Standard_Integer neEXTERNAL = 0;

	  Standard_Boolean isold = FABU.IsOldWire();
	  if(isold)
	    {
	      nboldW++;
	      newWire = TopoDS::Wire(FABU.OldWire());
	    }
	  else
	    {
	      BldCmpOfEdges.MakeCompound(CmpOfEdges);//new compound
	      myBuildTool.MakeWire(newWire);
	      FABU.InitEdge();
	      for(; FABU.MoreEdge(); FABU.NextEdge())
		{
		  TopoDS_Edge newEdge = TopoDS::Edge(FABU.Edge());
//		  if (mEtouched.Contains(newEdge)) continue;// xpu290498
//		  mEtouched.Add(newEdge);// xpu290498

		  //--ofv:
		  Standard_Integer nadde = FABU.AddEdgeWire(newEdge,CmpOfEdges);
		  ne += nadde;
		  //Standard_Integer nadde = FABU.AddEdgeWire(newEdge,newWire);
		  //ne += nadde;
		  //--ofv.

		  TopAbs_Orientation oE = newEdge.Orientation();
		  if      (oE == TopAbs_INTERNAL) neINTERNAL++;
		  else if (oE == TopAbs_EXTERNAL) neEXTERNAL++;
		  else if (oE == TopAbs_FORWARD)  neFORWARD++;
		  else if (oE == TopAbs_REVERSED) neREVERSED++;
	  
		  Standard_Boolean hasPC = FC2D_HasCurveOnSurface(newEdge,newFace);                                     // jyl980402+
		  if (!hasPC)                                                                                           // jyl980402+
		    {                                                                                                   // jyl980402+
		      Standard_Real tolE = BRep_Tool::Tolerance(newEdge);                                               // jyl980402+
		      Standard_Real f2,l2,tolpc; Handle(Geom2d_Curve) C2D;                                              // jyl980402+
		      //C2D = FC2D_CurveOnSurface(newEdge,newFace,f2,l2,tolpc);                                         // jyl980402+
		      C2D = FC2D_CurveOnSurface(newEdge,newFace,f2,l2,tolpc, Standard_True);                            // xpu051198 (CTS21701)
		      if(C2D.IsNull()) throw Standard_ProgramError("TopOpeBRepBuild_Builder::GFABUMakeFaces null PC"); // jyl980402+
		      Standard_Real tol = Max(tolE,tolpc);                                                              // jyl980402+
		      BRep_Builder BB_PC; BB_PC.UpdateEdge(newEdge,C2D,newFace,tol);                                    // jyl980402+
		    }                                                                                                   // jyl980402+
		} // FABU.MoreEdge()

	      //--ofv:
	      if((neINTERNAL == 0 && neEXTERNAL == 0) || (ne == neINTERNAL || ne == neEXTERNAL))
		{
		  TopExp_Explorer EdgeEx;
		  if(nbw == 1 && ne == 2)
		    {
		      EdgeEx.Init(CmpOfEdges,TopAbs_EDGE);
		      TopoDS_Edge nEdge1 = TopoDS::Edge(EdgeEx.Current());
		      EdgeEx.Next();
		      TopoDS_Edge nEdge2 = TopoDS::Edge(EdgeEx.Current());
		      if( nEdge1.IsSame(nEdge2) )
			return;
		    }
		  for(EdgeEx.Init(CmpOfEdges,TopAbs_EDGE); EdgeEx.More(); EdgeEx.Next())
		    {
		      TopoDS_Edge newEdge = TopoDS::Edge(EdgeEx.Current());
		      FABU.AddEdgeWire(newEdge,newWire);
		    }
		}
	      else
		{
		  //TopTools_IndexedDataMapOfShapeListOfShape mapVOE;
		  mapVOE.Clear();
		  TopExp::MapShapesAndAncestors(CmpOfEdges,TopAbs_VERTEX,TopAbs_EDGE,mapVOE);
		  // checking: wire is closed and regular. If wire is not close or not regular: vertex has only the one edge
		  // or vetrex has more then two shared edges, we don't modify it. 
		  Standard_Boolean WisClsd = Standard_True;
		  for(Standard_Integer MapStep = 1; MapStep <= mapVOE.Extent(); MapStep++)
		    {
		      const TopTools_ListOfShape& LofE = mapVOE.FindFromIndex(MapStep);
		      if(LofE.Extent() != 2) { WisClsd = Standard_False; break; }
		    }
		  if(!WisClsd)
		    {
		      //wire is not regular:
		      TopExp_Explorer EdgeEx;
		      for(EdgeEx.Init(CmpOfEdges,TopAbs_EDGE); EdgeEx.More(); EdgeEx.Next())
			{
			  TopoDS_Edge newEdge = TopoDS::Edge(EdgeEx.Current());
			  FABU.AddEdgeWire(newEdge,newWire);
			}
		    }
		  else
		    {
		      //wire seems to be regular:
		      TopTools_ListOfShape LofAddE;  // list of edges has already been added in wire
		      Standard_Integer naddsame = 0; 
		      while( ne > (LofAddE.Extent() + naddsame) )
			{
			  for(Standard_Integer StepMap = 1; StepMap <= mapVOE.Extent(); StepMap++)
			    {
			      const TopTools_ListOfShape& LofE = mapVOE.FindFromIndex(StepMap);
			      TopTools_ListIteratorOfListOfShape itLofE(LofE);
			      TopoDS_Edge E1 = TopoDS::Edge(itLofE.Value());
			      itLofE.Next();
			      TopoDS_Edge E2 = TopoDS::Edge(itLofE.Value());
			      TopAbs_Orientation O1 = E1.Orientation();
			      TopAbs_Orientation O2 = E2.Orientation();
			      Standard_Boolean IsSameE1 = BRep_Tool::IsClosed(E1,tdF);
			      Standard_Boolean IsSameE2 = BRep_Tool::IsClosed(E2,tdF);
			      Standard_Boolean AddE1 = Standard_True;
			      Standard_Boolean AddE2 = Standard_True;

			      //checking current edges in the list of added edges
			      TopTools_ListIteratorOfListOfShape itLofAddE(LofAddE);
			      for(; itLofAddE.More(); itLofAddE.Next() )
				{
				  const TopoDS_Shape& LE = itLofAddE.Value();
				  TopAbs_Orientation OLE = LE.Orientation();
				  if(E1.IsSame(LE) && !IsSameE1) { AddE1 = Standard_False; E1.Orientation(OLE); O1 = OLE; }
				  if(E2.IsSame(LE) && !IsSameE2) { AddE2 = Standard_False; E2.Orientation(OLE); O2 = OLE; }
				}
			      Standard_Integer chkORI = FUN_CheckORI(O1,O2);
			      if(chkORI == 0){ AddE1 = Standard_False; AddE2 = Standard_False; }
			      if(chkORI == 1)
				{
				  TopAbs_Orientation ori = FUN_ReOrientIntExtEdge(E2,O2,E1);
				  if(ori == TopAbs_FORWARD) { E1.Orientation(TopAbs_FORWARD); neFORWARD++; }
				  if(ori == TopAbs_REVERSED){ E1.Orientation(TopAbs_REVERSED); neREVERSED++; }
				  if(ori == TopAbs_REVERSED || ori == TopAbs_FORWARD)
				    {
				      if(O1 == TopAbs_INTERNAL) neINTERNAL--;
				      else neEXTERNAL--;
				    }
				}
			      if(chkORI == 2)
				{
				  TopAbs_Orientation ori = FUN_ReOrientIntExtEdge(E1,O1,E2);
				  if(ori == TopAbs_FORWARD) { E2.Orientation(TopAbs_FORWARD); neFORWARD++; }
				  if(ori == TopAbs_REVERSED){ E2.Orientation(TopAbs_REVERSED); neREVERSED++; }
				  if(ori == TopAbs_REVERSED || ori == TopAbs_FORWARD)
				    {
				      if(O2 == TopAbs_INTERNAL) neINTERNAL--;
				      else neEXTERNAL--;
				    }
				}



			      if(AddE1)
				{
				  FABU.AddEdgeWire(E1,newWire);
				  if(!IsSameE1) LofAddE.Append(E1);
				  else naddsame++;
				}
			      if(AddE2)
				{
				  FABU.AddEdgeWire(E2,newWire);
				  if(!IsSameE2) LofAddE.Append(E2);
				  else naddsame++;
				}
			    }//for StepMap
			} // while ne >
		    } // not regular
		} // neINTERNAL(neEXTERNAL) != 0
	    } // !isold
	  //--ofv.

#ifdef OCCT_DEBUG
      if ( tSPS ) cout<<"#--- GFABUMakeFaces "<<iF<<" : "<<ne<<" edges"<<endl; 
#endif	  

      // xpu : 13-11-97
      if (ne != 0) {
	Standard_Integer iow = isold? 1: 0;
	MWisOld.Bind(newWire,iow);
      }
      
      // <newWire> is empty :
      if ( !isold ) {
	if (ne == 0) continue;
	else if (nbw == 1 && (ne == neINTERNAL+neEXTERNAL)) continue;
	else nbnewWwithe++;
      }

      // caractere Closed() du nouveau wire newWire
      if ( !isold ) {
	Standard_Boolean closed = FUN_tool_ClosedW(newWire);
	myBuildTool.Closed(newWire,closed); 
      } // !isold
      
      myBuildTool.AddFaceWire(newFace,newWire);
      
    } // FABU.MoreWire()
    
    if ( nbnewWwithe == 0 && nboldW == 0 ) {
      continue;
    }

    Standard_Boolean topurge = FUN_purgeFon1nonoriE(newFace);
    if (topurge) {
      continue;
    }
    
// Le changement de surface de trim a basis causait la perte des regularites de l'edge
// j'ai change par un recadrage du trim en attendant mieux. DPF le 29/07/1998.
// Le danger est de modifier une donnee d'entree.
    Handle(Standard_Type) T = Surf->DynamicType();
    Standard_Boolean istrim = ( T == STANDARD_TYPE(Geom_RectangularTrimmedSurface) );
    if (istrim) {
      Handle(Geom_RectangularTrimmedSurface) 
	hrts=Handle(Geom_RectangularTrimmedSurface)::DownCast (Surf);
      Standard_Real oumin,oumax,ovmin,ovmax;
      hrts->Bounds(oumin,oumax,ovmin,ovmax);
      Standard_Real umin,umax,vmin,vmax;
      BRepTools::UVBounds(newFace, umin, umax, vmin, vmax);
      if (umin < oumin) oumin=umin;
      if (umax > oumax) oumax=umax;
      if (vmin < ovmin) ovmin=vmin;
      if (vmax > ovmax) ovmax=vmax;
      hrts->SetTrim(oumin, oumax, ovmin, ovmax, Standard_True, Standard_True);
    }
    lnewFace.Append(newFace);

  } // FABU.MoreFace()

#ifdef OCCT_DEBUG
  if(tSPS) {
    cout<<endl;GdumpSHA(FF, (char *) "#--- GFABUMakeFaces avant regularize");cout<<endl;
    GdumpFABU(FABU);debgfabu(iF);
  }
#endif

  // xpu281098 : regularisation after purge processings (cto009L2,f4ou)
//  RegularizeFaces(FF,lnewFace,LOF);
//  Standard_Integer nLOF = LOF.Extent(); // DEB
  LOF.Append(lnewFace);

} // GFABUMakeFaces
