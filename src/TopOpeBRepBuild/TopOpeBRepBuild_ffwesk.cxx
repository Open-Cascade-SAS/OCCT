// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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


#include <TopOpeBRepBuild_Builder.ixx>

#include <TopOpeBRepDS.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepTool_EXPORT.hxx>
#include <TopOpeBRepTool_2d.hxx>

#ifdef DEB
Standard_EXPORT void debfctwesmess(const Standard_Integer i,const TCollection_AsciiString& s = "");
Standard_EXPORT void debffwesON(const Standard_Integer i);
Standard_EXPORT void debffwesk(const Standard_Integer i) {cout<<"++ debffwesk "<<i<<endl;}
#endif

Standard_EXPORT Standard_Boolean TopOpeBRepBuild_FUN_aresamegeom(const TopoDS_Shape& S1,const TopoDS_Shape& S2);

#define M_IN(st )      (st == TopAbs_IN)
#define M_OUT(st)      (st == TopAbs_OUT)
#define M_FORWARD(st ) (st == TopAbs_FORWARD)
#define M_REVERSED(st) (st == TopAbs_REVERSED)
#define M_INTERNAL(st) (st == TopAbs_INTERNAL)
#define M_EXTERNAL(st) (st == TopAbs_EXTERNAL)

//=======================================================================
//function : GFillFacesWESK
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GFillFacesWESK(const TopTools_ListOfShape& LS1,const TopTools_ListOfShape& LS2,const TopOpeBRepBuild_GTopo& G1,TopOpeBRepBuild_WireEdgeSet& WES,const Standard_Integer Kfill)
{
  if ( LS1.IsEmpty() ) return;
  TopAbs_State TB1,TB2; G1.StatesON(TB1,TB2);

  const TopOpeBRepDS_DataStructure& BDS = myDataStructure->DS();

  const TopoDS_Shape& F1 = LS1.First();
  myFaceReference = TopoDS::Face(F1);
#ifdef DEB
  Standard_Integer iF1 =
#endif
            BDS.Shape(F1);
  Standard_Integer iref = BDS.SameDomainRef(F1);
  TopAbs_Orientation oref = BDS.Shape(iref).Orientation();

#ifdef DEB
  Standard_Boolean tSPS = GtraceSPS(F1,iF1);
  if(tSPS) cout<<"\n%%%%%%%%%%%%% K = "<<Kfill<<" %%%%%%%%%%%%% ";
  if(tSPS) GdumpSHASTA(iF1,TB1,WES,"GFillFacesWESK","myFaceReference");
  if(tSPS) debffwesk(iF1);
#endif

  TopAbs_State TB;
  TopOpeBRepBuild_GTopo G;
  TopTools_ListIteratorOfListOfShape it;
  Standard_Boolean gistoreverse1;

  G = G1; 
  gistoreverse1 = G.IsToReverse1();
  TB = TB1;
  it.Initialize(LS1); 

#ifdef DEB
  if(tSPS){
    cout<<"\n^^^^^^^^ GFillFacesWESK : traitement de 1/2";
    TopAbs_State TB11,TB21; G.StatesON(TB11,TB21);
    cout<<" TB = ";TopAbs::Print(TB,cout); cout<<" ";
    cout<<"(TB1 = ";TopAbs::Print(TB11,cout);cout<<",";
    cout<<" TB2 = ";TopAbs::Print(TB21,cout);cout<<")\n\n";
  }
#endif

  for(; it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value(); Standard_Integer iS = myDataStructure->Shape(S);
    Standard_Boolean tomerge = !IsMerged(S,TB);
    if (!tomerge) continue;    
    
    // lors de l'insertion des composantes (edges splitees) de S dans le WES :
    // - faces de meme orientation geometrique : 
    //     la grille indique l'orientation des composantes
    // - faces d'orientation geometrique != : 
    //     les aretes de S sont exprimees dans une surface
    //     d'orientation opposee a la surface de la face de reference. 
    //     Leur orientation dans la face resultat (reference) est le contraire 
    //     de la grille.
//  Modified by Sergey KHROMOV - Sat Apr 27 14:40:35 2002 Begin
//     const TopoDS_Shape& Fref = WES.Face();
//     const TopoDS_Shape& F = S;
//     Standard_Boolean samegeom = ::TopOpeBRepBuild_FUN_aresamegeom(Fref,F);
//     Standard_Boolean r = gistoreverse1;
//     if ( !samegeom ) { r = !r; G.SetReverse(r); }
    
//     Standard_Boolean rev = Standard_False;
//  Modified by Sergey KHROMOV - Sat Apr 27 14:40:36 2002 End
    TopAbs_Orientation oS = BDS.Shape(iS).Orientation();
    TopOpeBRepDS_Config conf = BDS.SameDomainOri(S);
    Standard_Boolean b1 = (conf == TopOpeBRepDS_DIFFORIENTED && (oS == oref));
    Standard_Boolean b2 = (conf == TopOpeBRepDS_SAMEORIENTED && (oS != oref));
    Standard_Boolean b = b1 || b2;
//  Modified by Sergey KHROMOV - Sat Apr 27 14:40:01 2002 Begin
//     if (b) {
//       rev = Standard_True;
//       G.SetReverse(rev);
//     }
    if (b)
      G.SetReverse(!gistoreverse1);
//  Modified by Sergey KHROMOV - Sat Apr 27 14:40:03 2002 End
    
    if      (Kfill == 1) {
      GFillFaceWES(S,LS2,G,WES);
    }
    else if (Kfill == 2) {
#ifdef DEB
      if (tSPS) debfctwesmess(iF1);
#endif
      GFillCurveTopologyWES(S,G,WES);
    }
    else if (Kfill == 3) {
#ifdef DEB
      if (tSPS) debffwesON(iF1);
#endif
      GFillONPartsWES(S,G,LS2,WES);
    }
    else {
      // error
    } 
  } // it(LS1)

  
  G = G1.CopyPermuted();
  gistoreverse1 = G.IsToReverse1();
  TB = TB2;
  it.Initialize(LS2);

#ifdef DEB
  if(tSPS){
    cout<<"\n^^^^^^^^ GFillFacesWESK : traitement de 2/1";
    TopAbs_State TB12,TB22; G.StatesON(TB12,TB22);
    cout<<" TB = ";TopAbs::Print(TB,cout); cout<<" ";
    cout<<"(TB1 = ";TopAbs::Print(TB12,cout);cout<<",";
    cout<<" TB2 = ";TopAbs::Print(TB22,cout);cout<<")\n\n";
  }
#endif

  for (; it.More(); it.Next()) {
    const TopoDS_Shape& S = it.Value(); Standard_Integer iS = myDataStructure->Shape(S);
    Standard_Boolean tomerge = !IsMerged(S,TB);
    if (!tomerge) continue;

//  Modified by Sergey KHROMOV - Sat Apr 27 14:38:33 2002 Begin
//     const TopoDS_Shape& Fref = WES.Face();
//     const TopoDS_Shape& F = S;
//     Standard_Boolean samegeom = ::TopOpeBRepBuild_FUN_aresamegeom(Fref,F);
//     Standard_Boolean r = gistoreverse1;
//     if ( !samegeom ) { r = !r; G.SetReverse(r); }
    
//     Standard_Boolean rev = Standard_False;
//  Modified by Sergey KHROMOV - Sat Apr 27 14:38:30 2002 End
    TopAbs_Orientation oS = BDS.Shape(iS).Orientation();
    TopOpeBRepDS_Config conf = BDS.SameDomainOri(S);
    Standard_Boolean b1 = (conf == TopOpeBRepDS_DIFFORIENTED && (oS == oref));
    Standard_Boolean b2 = (conf == TopOpeBRepDS_SAMEORIENTED && (oS != oref));
    Standard_Boolean b = b1 || b2;
//  Modified by Sergey KHROMOV - Sat Apr 27 14:39:04 2002 Begin
//     if (b) {
//       rev = Standard_True;
//       G.SetReverse(rev);
//     }
    if (b)
      G.SetReverse(!gistoreverse1);
//  Modified by Sergey KHROMOV - Sat Apr 27 14:39:08 2002 End
    
    if      (Kfill == 1) {
      GFillFaceWES(S,LS1,G,WES);
    }
    else if (Kfill == 2) {
      GFillCurveTopologyWES(S,G,WES);
    }
    else if (Kfill == 3) {
#ifdef DEB
      if(tSPS) debffwesON(iF1);
#endif
      GFillONPartsWES(S,G,LS1,WES);
    }
    else {
    }

  } // it(LS2)

} // GFillFacesWESK
