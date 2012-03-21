// Created on: 1995-09-12
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef TopOpeBRepBuild_SplitSolid_INCLUDED
#define TopOpeBRepBuild_SplitSolid_INCLUDED

#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_define.hxx>

#ifdef DEB
#define DEBSHASET(sarg,meth,shaset,str) TCollection_AsciiString sarg((meth));(sarg)=(sarg)+(shaset).DEBNumber()+(str);
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSTRANGE();
Standard_EXPORT void debsplitf(const Standard_Integer i);
Standard_EXPORT void debspanc(const Standard_Integer i);
//Standard_IMPORT extern Standard_Integer GLOBAL_iexF;
Standard_IMPORT Standard_Integer GLOBAL_iexF;
#endif

//=======================================================================
//function : SplitSolid
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_Builder::SplitSolid(const TopoDS_Shape& S1oriented,
					 const TopAbs_State ToBuild1,
					 const TopAbs_State ToBuild2)
{
  //modified by IFV for treating shell
  Standard_Boolean tosplit = Standard_False;
  Standard_Boolean IsShell = (S1oriented.ShapeType() == TopAbs_SHELL);
  if(IsShell) {
    TopExp_Explorer ex;
    ex.Init(S1oriented, TopAbs_FACE);
    for (; ex.More(); ex.Next()) {
      const TopoDS_Shape& sh = ex.Current();
      tosplit = ToSplit(sh,ToBuild1);
      if(tosplit) break;
    }
  }
  else tosplit = ToSplit(S1oriented,ToBuild1);

  if ( ! tosplit ) return;
  // end IFV

  Standard_Boolean RevOri1 = Reverse(ToBuild1,ToBuild2);
  Standard_Boolean RevOri2 = Reverse(ToBuild2,ToBuild1);
  Standard_Boolean ConnectTo1 = Standard_True;
  Standard_Boolean ConnectTo2 = Standard_False;

  // work on a FORWARD solid <S1forward>
  TopoDS_Shape S1forward = S1oriented; 
  myBuildTool.Orientation(S1forward,TopAbs_FORWARD);
  
  // build the list of solids to split : LS1, LS2
  TopTools_ListOfShape LS1,LS2;
  LS1.Append(S1forward);
  FindSameDomain(LS1,LS2);
  Standard_Integer n1 = LS1.Extent();
  Standard_Integer n2 = LS2.Extent();
  
  if (!n2) RevOri1 = Standard_False;
  if (!n1) RevOri2 = Standard_False;
  
  // Create a face set <FS> connected by edges
  // -----------------------------------------
  TopOpeBRepBuild_ShellFaceSet SFS;
  
#ifdef DEB
  Standard_Boolean tSPS = TopOpeBRepBuild_GettraceSPS();
//  Standard_Integer iSolid = myDataStructure->Shape(S1oriented);
  if (tSPS) {
    cout<<endl;
    GdumpSHASTA(S1oriented,ToBuild1,"___ SplitSolid ");
    GdumpSAMDOM(LS1, (char *) "1 : "); 
    GdumpSAMDOM(LS2, (char *) "2 : ");
  }
  SFS.DEBNumber(GdumpSHASETindex());
#endif

  STATIC_SOLIDINDEX = 1;
  TopTools_ListIteratorOfListOfShape itLS1;
  for (itLS1.Initialize(LS1); itLS1.More(); itLS1.Next()) {
    TopoDS_Shape Scur = itLS1.Value();
    FillSolid(Scur,ToBuild1,LS2,ToBuild2,SFS,RevOri1);
  }
  
  STATIC_SOLIDINDEX = 2;
  TopTools_ListIteratorOfListOfShape itLS2;
  for (itLS2.Initialize(LS2); itLS2.More(); itLS2.Next()) {
    TopoDS_Shape Scur = itLS2.Value();
    FillSolid(Scur,ToBuild2,LS1,ToBuild1,SFS,RevOri2);
  }
  
  // Add the intersection surfaces
  // -----------------------------
  if (myDataStructure->NbSurfaces() > 0) {
    TopOpeBRepDS_SurfaceIterator SSurfaces = myDataStructure->SolidSurfaces(S1forward);
    for (; SSurfaces.More(); SSurfaces.Next()) {
      Standard_Integer iS = SSurfaces.Current();
      const TopTools_ListOfShape& LnewF = NewFaces(iS);
      for (TopTools_ListIteratorOfListOfShape Iti(LnewF); Iti.More(); Iti.Next()) {
	TopoDS_Shape aFace = Iti.Value();
	TopAbs_Orientation ori = SSurfaces.Orientation(ToBuild1);
	myBuildTool.Orientation(aFace,ori);
	
#ifdef DEB
	if (tSPS){
	  DEBSHASET(ss,"--- SplitSolid ",SFS," AddElement SFS+ face ");  
	  GdumpSHA(aFace,(Standard_Address)ss.ToCString());
	  cout<<" ";TopAbs::Print(ToBuild1,cout)<<" : 1 face ";
	  TopAbs::Print(ori,cout); cout<<endl;
	}
#endif
	SFS.AddElement(aFace);
      }
    }
  }

  // Create a Solid Builder SOBU
  // -------------------------
  TopOpeBRepBuild_SolidBuilder SOBU(SFS);

  // Build the new solids on S1
  // --------------------------
  TopTools_ListOfShape& SolidList = ChangeMerged(S1oriented,ToBuild1);
  if(IsShell)
    MakeShells(SOBU,SolidList);
  else
    MakeSolids(SOBU,SolidList);

  // connect list of new solids <SolidList> as solids built on LS1 solids
  // --------------------------------------------------------------------

  for (itLS1.Initialize(LS1); itLS1.More(); itLS1.Next()) {
    TopoDS_Shape Scur = itLS1.Value();
    MarkSplit(Scur,ToBuild1);
    TopTools_ListOfShape& SL = ChangeSplit(Scur,ToBuild1);
    if ( ConnectTo1 ) SL = SolidList;

  }
  
  // connect list of new solids <SolidList> as solids built on LS2 solids
  // --------------------------------------------------------------------
  for (itLS2.Initialize(LS2); itLS2.More(); itLS2.Next()) {
    TopoDS_Shape Scur = itLS2.Value();
    MarkSplit(Scur,ToBuild2);
    TopTools_ListOfShape& SL = ChangeSplit(Scur,ToBuild2);
    if ( ConnectTo2 ) SL = SolidList;
  }

} // SplitSolid

//#ifndef TopOpeBRepBuild_SplitSolid_INCLUDED
#endif
