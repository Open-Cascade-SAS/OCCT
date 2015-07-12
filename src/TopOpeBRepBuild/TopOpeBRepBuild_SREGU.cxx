// Created on: 1997-10-21
// Created by: Jean Yves LEBEY
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


#include <gp_Pnt.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_ShellToSolid.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <TopOpeBRepDS_CurveIterator.hxx>
#include <TopOpeBRepDS_define.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_PointIterator.hxx>
#include <TopOpeBRepDS_SurfaceIterator.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GetcontextNOREGUSO();
extern Standard_Boolean TopOpeBRepBuild_GetcontextREGUXPU();
extern Standard_Boolean TopOpeBRepBuild_GettraceSAVSREGU();
Standard_EXPORT void debreguso(const Standard_Integer iS) {cout<<"++ debreguso "<<iS<<endl;}
#endif

#ifdef DRAW
#include <DBRep.hxx>
#endif

//=======================================================================
//function : RegularizeSolids
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::RegularizeSolids
(const TopoDS_Shape& SO,const TopTools_ListOfShape& lnewSolid,TopTools_ListOfShape& LOSO)
{
  LOSO.Clear();
  myMemoSplit.Clear();

  TopTools_ListIteratorOfListOfShape itl(lnewSolid);
  for (;itl.More();itl.Next()) {
    const TopoDS_Shape& newSolid = itl.Value();
    TopTools_ListOfShape newSolidLOSO;
    RegularizeSolid(SO,newSolid,newSolidLOSO);
#ifdef OCCT_DEBUG
//    Standard_Integer nnewSolidLOSO = newSolidLOSO.Extent(); // DEB
#endif
    LOSO.Append(newSolidLOSO);
  }
#ifdef OCCT_DEBUG
//  Standard_Integer nLOSO = LOSO.Extent(); // DEB
#endif
  Standard_Integer nr = myMemoSplit.Extent();
  if (nr == 0 ) return;

  // lsosdSO = solids SameDomain de SO
  TopTools_ListOfShape lsosdSO,lsosdSO1,lsosdSO2;
  GFindSamDom(SO,lsosdSO1,lsosdSO2);
  lsosdSO.Append(lsosdSO1);
  lsosdSO.Append(lsosdSO2);
  
  TopTools_ListIteratorOfListOfShape itlsosdSO(lsosdSO);
  for (; itlsosdSO.More(); itlsosdSO.Next()) {
    const TopoDS_Shape& sosdSO = itlsosdSO.Value();
    // au moins une arete de SO dont le Split() est lui meme Split()
    TopExp_Explorer x;
    for (x.Init(sosdSO,TopAbs_FACE);x.More();x.Next()) {
//    for (TopExp_Explorer x(sosdSO,TopAbs_FACE);x.More();x.Next()) {
      const TopoDS_Shape& f = x.Current();
      Standard_Integer rankf=GShapeRank(f);TopAbs_State staf=(rankf==1)?myState1:myState2;
      Standard_Boolean issplitf = IsSplit(f,staf);
      if (!issplitf) continue;
      
      TopTools_ListOfShape newlspf;
      TopTools_ListOfShape& lspf = ChangeSplit(f,staf);
#ifdef OCCT_DEBUG
//      Standard_Integer nlspf = lspf.Extent(); // DEB
#endif
      for (TopTools_ListIteratorOfListOfShape itl1(lspf);itl1.More();itl1.Next()) {
	const TopoDS_Shape& fsp = itl1.Value();
	Standard_Boolean fspmemo = myMemoSplit.Contains(fsp);
	if (!fspmemo) newlspf.Append(fsp);
	else {
	  TopTools_ListOfShape& lspfsp = ChangeSplit(fsp,staf);
	  GCopyList(lspfsp,newlspf);
	}
      }
      lspf.Clear();
      GCopyList(newlspf,lspf);

      //      if (staf == TopAbs_IN) { 
      //	// IN Solide <=> ON ??? : M.A.J de Split(TopAbs_ON);
      //	Standard_Boolean issplitON = IsSplit(f,TopAbs_ON);
      //	TopTools_ListOfShape& lONf = ChangeSplit(f,TopAbs_ON);
      //	Standard_Integer nONf = lONf.Extent(); // DEB
      //	lONf.Clear();
      //	GCopyList(newlspf,lONf);
      //      } // TopAbs_IN

    } // (sosdSO,TopAbs_FACE)
  }
}

//=======================================================================
//function : RegularizeSolid
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::RegularizeSolid
(const TopoDS_Shape& SS,const TopoDS_Shape& anewSolid,TopTools_ListOfShape& LOSO)
{
  LOSO.Clear();
  const TopoDS_Solid& newSolid = TopoDS::Solid(anewSolid);
  Standard_Boolean toregu = Standard_True;
  Standard_Boolean usestos = Standard_True;
  
#ifdef OCCT_DEBUG
  Standard_Integer iS;Standard_Boolean tSPS=GtraceSPS(SS,iS);
//  Standard_Boolean savsregu = TopOpeBRepBuild_GettraceSAVSREGU();
  if (TopOpeBRepBuild_GetcontextNOREGUSO()) toregu = Standard_False;
  if (TopOpeBRepBuild_GetcontextREGUXPU()) usestos = Standard_False;
  if (tSPS) debreguso(iS);
#endif
  
  if (!toregu) {
    LOSO.Append(newSolid);
    return;
  }
  
  TopTools_DataMapOfShapeListOfShape osns; // OldShells --> NewShells;
  Standard_Boolean rw = Standard_False;
  Standard_Boolean rf = Standard_False;
  myFSplits.Clear();
  
  rw = TopOpeBRepTool::RegularizeShells(newSolid,osns,myFSplits);
  
  if ( !rw ) {
    LOSO.Append(newSolid);
    return;
  }      
  
  TopTools_ListOfShape newSolids;
  if (usestos) {
    TopOpeBRepBuild_ShellToSolid stos;
    TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itosns(osns);
    for (; itosns.More(); itosns.Next()) {
      
/* JYL 120499 : a mettre dans l'areabuilder du solidbuilder du shelltosolid
       
      // solution 1 : alerter le SolidClassifier utilise dans le SolidBuilder
      //              de ShellToSolid qu'il y a de des aretes mutilconnexes (2 faces)
      //              dans oldshe QU'IL NE FAUT PAS UTILISER pour tirer un point
      //              representatif du shell.

      // map des edges -> list of face de oldshe
      TopTools_IndexedDataMapOfShapeListOfShape maef;
      TopExp::MapShapesAndAncestors(oldshe,TopAbs_EDGE,TopAbs_FACE,maef);
      Standard_Integer ima=1,nma=maef.Extent();
      for(;ima<=nma;ima++) {
	const TopoDS_Edge& eevit = TopoDS::Edge(maef.FindKey(ima));
	const TopTools_ListOfShape& lfa = maef(ima);
	Standard_Integer nlfa = lfa.Extent();
	if (nlfa > 2) {
	}
      }
      // fin solution1
*/

      const TopTools_ListOfShape& lns = itosns.Value();
      for(TopTools_ListIteratorOfListOfShape iw(lns);iw.More();iw.Next()) {
	stos.AddShell(TopoDS::Shell(iw.Value()));
      }
    }
    stos.MakeSolids(newSolid,newSolids);
    rf = (newSolids.Extent() != 0);
  }
  else {
    rf = Standard_False;
//    rf = TopOpeBRepTool::RegularizeSolid(newSolid,osns,newSolids);
  }
  
  if (!rf) {
    LOSO.Append(newSolid);
    return;
  }
  
#ifdef OCCT_DEBUG
  if (tSPS) {
    cout<<"RegularizeSolid "<<iS<<endl;
    debreguso(iS);
  }
#endif
  
  // LOSO = nouvelles Solids regularisees de newSolid
  TopTools_ListIteratorOfListOfShape itlnf(newSolids);
  for (; itlnf.More(); itlnf.Next()) 
    LOSO.Append(TopoDS::Solid(itlnf.Value()));
  
  // mise a jour des faces decoupees
  // Face(SS) = {E}, E-->Split(E) = {E'}, E'-->myFSplits(E') = {E''}
  // manc : E'-->E pour pouvoir relier
  // Split(manc(E')) = {myFSplits(E')}
  TopTools_MapOfShape mfns; // mfns = faces de newSolid
  TopExp_Explorer x;
  for (x.Init(newSolid,TopAbs_FACE);x.More();x.Next()) {
    const TopoDS_Shape& F = x.Current();
    mfns.Add(F);
  }
  
  // lssdSS = Solids SameDomain de SS
  TopTools_ListOfShape lssdSS,lssdSS1,lssdSS2;
  GFindSamDom(SS,lssdSS1,lssdSS2);
  lssdSS.Append(lssdSS1);
  lssdSS.Append(lssdSS2);
  
  TopTools_DataMapOfShapeShape manc;
  
  TopTools_ListIteratorOfListOfShape itlssdSS(lssdSS);
  for (; itlssdSS.More(); itlssdSS.Next()) {
    const TopoDS_Shape& ssdSS = itlssdSS.Value();
#ifdef OCCT_DEBUG
//    Standard_Integer issdSS = myDataStructure->Shape(ssdSS); // DEB
#endif
    
    Standard_Integer rankssdSS = GShapeRank(ssdSS);
    TopAbs_State stassdSS = (rankssdSS == 1) ? myState1 : myState2;
#ifdef OCCT_DEBUG
//    Standard_Boolean issplitssdSS = IsSplit(ssdSS,stassdSS);
//    const TopTools_ListOfShape& lspssdSS = Splits(ssdSS,stassdSS);
//    Standard_Integer nlspssdSS = lspssdSS.Extent();
#endif
    
    // iteration sur les faces de ssdSS
    for (x.Init(ssdSS,TopAbs_FACE);x.More();x.Next()) {
      
      //ssdSSf : 1 face de ssdSS = 1 solid SameDomain de Ss
      const TopoDS_Shape& ssdSSf = x.Current();

#ifdef OCCT_DEBUG
      Standard_Integer issdSSf = 0;Standard_Boolean tSPSssdSSf=GtraceSPS(ssdSSf,issdSSf);
      if (tSPSssdSSf) debreguso(issdSSf);
#endif    
   
      TopAbs_State stassdSSf = stassdSS;
      
      TopTools_ListOfShape& lspssdSSf = ChangeSplit(ssdSSf,stassdSSf);
#ifdef OCCT_DEBUG
//      Standard_Boolean issplitssdSSf = IsSplit(ssdSSf,stassdSSf);
//      Standard_Integer nlspssdSSf = lspssdSSf.Extent();
#endif    

      TopTools_ListOfShape newlspssdSSf; // nouvel ensemble de faces splittees de ssdSSf

      for (TopTools_ListIteratorOfListOfShape it(lspssdSSf);it.More();it.Next()) {
	const TopoDS_Shape& fspssdSSf = it.Value();
	
	Standard_Boolean inmfns = mfns.Contains(fspssdSSf);
	if (!inmfns) continue;

	// ssdSSf est une face de ssdSS (Cf supra E)
	// fspssdSSf de Split(ssdSSf) figure dans newSolid (Cf supra E')
	
	Standard_Boolean rfsplitloc = myFSplits.IsBound(fspssdSSf);
	if (rfsplitloc) {

	  // ssdSSf (Cf supra E) a ete splittee, fspssdSSf = arete splittee de ssdSSf
	  // fspssdSSf est une arete de Split(ssdSSf) ET figure dans newFace (Cf supra E')
	  // fspssdSSf de newFace a ete redecoupee par RegularizeWires

	  // son decoupage lrfsplit est stocke dans la DS du Builder
	  const TopTools_ListOfShape& lrfsplit = myFSplits.Find(fspssdSSf);//Cf supra E''

	  // on memorise que fspssdSSf est redecoupee ...
	  myMemoSplit.Add(fspssdSSf);
	  
	  // on stocke le nouveau decoupage de fspssdSSf dans la DS du builder ...
	  TopTools_ListOfShape& lsp = ChangeSplit(fspssdSSf,stassdSSf);  
	  GCopyList(lrfsplit,lsp);
	}	
      } // lspssdSSf.More()
    } // explore(ssdSS,TopAbs_FACE)
  } // itlssdSS.More()

#ifdef DRAW
  if (tSPS) debreguso(iS);
  if (tSPS && savsregu) {
    TCollection_AsciiString str("sregu"); str = str + iS;
    DBRep::Set(str.ToCString(),newSolid);
    cout<<"newSolid "<<str<<" built on Solid "<<iS<<" saved"<<endl;
  }
#endif
  
} // RegularizeSolid
