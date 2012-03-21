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

#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopOpeBRepTool_TOOL.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepBuild_define.hxx>

#ifdef DEB
Standard_IMPORT void debfillp(const Standard_Integer i);
Standard_EXPORT void debedbu(const Standard_Integer i) {cout<<"++ debedbu "<<i<<endl;}
#endif

#ifdef DRAW
#include <TopOpeBRepTool_DRAW.hxx>
#endif

//=======================================================================
//function : GPVSMakeEdges
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GPVSMakeEdges
(const TopoDS_Shape& EF,TopOpeBRepBuild_PaveSet& PVS,TopTools_ListOfShape& LOE) const 
{
#ifdef DEB
  Standard_Integer iE; Standard_Boolean tSPS = GtraceSPS(EF,iE);
  if (tSPS) debfillp(iE);
#endif
  
  TopOpeBRepBuild_PaveClassifier VCL(EF);
  Standard_Boolean equalpar = PVS.HasEqualParameters();
  if (equalpar) VCL.SetFirstParameter(PVS.EqualParameters());
  
  PVS.InitLoop();
  Standard_Boolean novertex = ( ! PVS.MoreLoop() );
#ifdef DEB
  if(tSPS&&novertex)cout<<"#--- GPVSMakeEdges : no vertex from edge "<<iE<<endl;
#endif
  if (novertex) return;
  
  TopOpeBRepBuild_EdgeBuilder EDBU;
  Standard_Boolean ForceClass = Standard_False;
  EDBU.InitEdgeBuilder(PVS,VCL,ForceClass);
  GEDBUMakeEdges(EF,EDBU,LOE);
  
} // GPVSMakeEdges

//=======================================================================
//function : GEDBUMakeEdges
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_Builder::GEDBUMakeEdges
(const TopoDS_Shape& EF,TopOpeBRepBuild_EdgeBuilder& EDBU,TopTools_ListOfShape& LOE) const
{
#ifdef DEB
  Standard_Integer iE; Standard_Boolean tSPS = GtraceSPS(EF,iE);
  if(tSPS){cout<<endl;GdumpSHA(EF, (char *) "#--- GEDBUMakeEdges ");cout<<endl;}
  if(tSPS){GdumpEDBU(EDBU);}
  if(tSPS){debedbu(iE);}
#endif

  TopoDS_Shape newEdge;
  for (EDBU.InitEdge(); EDBU.MoreEdge(); EDBU.NextEdge()) {
    
    Standard_Integer nloop = 0;
    Standard_Boolean tosplit = Standard_False;
    for (EDBU.InitVertex(); EDBU.MoreVertex(); EDBU.NextVertex()) nloop++; 
    // 0 ou 1 vertex sur edge courante => suppression edge
    if ( nloop <= 1 ) continue;
    
    myBuildTool.CopyEdge(EF,newEdge);
    
    Standard_Integer nVF = 0, nVR = 0; // nb vertex FORWARD,REVERSED
    
    TopoDS_Shape VF,VR; // gestion du bit Closed
    VF.Nullify();
    VR.Nullify();
    
    for (EDBU.InitVertex(); EDBU.MoreVertex(); EDBU.NextVertex()) {
      TopoDS_Shape V = EDBU.Vertex();
      TopAbs_Orientation Vori = V.Orientation();
      
      Standard_Boolean hassd = myDataStructure->HasSameDomain(V);
      if (hassd) { // on prend le vertex reference de V
	Standard_Integer iref = myDataStructure->SameDomainReference(V);
	V = myDataStructure->Shape(iref);
	V.Orientation(Vori);
      }
      
      TopAbs_Orientation oriV = V.Orientation();
      if ( oriV == TopAbs_EXTERNAL ) continue;
      
      Standard_Boolean equafound = Standard_False;
      TopExp_Explorer exE(newEdge,TopAbs_VERTEX);
      for (; exE.More(); exE.Next() ) {
	const TopoDS_Shape& VE = exE.Current();
	TopAbs_Orientation oriVE = VE.Orientation();

	if ( V.IsEqual(VE) ) {
	  equafound = Standard_True;
	  break;
	}
	else if (oriVE == TopAbs_FORWARD || oriVE == TopAbs_REVERSED) {
	  if (oriV == oriVE) {
	    equafound = Standard_True;
	    break;
	  }
	}
	else if (oriVE == TopAbs_INTERNAL || oriVE == TopAbs_EXTERNAL) {
	  Standard_Real parV = EDBU.Parameter();
	  Standard_Real parVE = BRep_Tool::Parameter(TopoDS::Vertex(VE),TopoDS::Edge(newEdge));
	  if ( parV == parVE ) {
	    equafound = Standard_True;
	    break;
	  }
	}
      }
      if ( !equafound  ) {
	if (Vori == TopAbs_FORWARD)  {
	  nVF++;
	  if (nVF == 1) VF = V;
	} 
	if (Vori == TopAbs_REVERSED) {
	  nVR++;
	  if (nVR == 1) VR = V;
	}
	if (oriV == TopAbs_INTERNAL) tosplit = Standard_True;
	Standard_Real parV = EDBU.Parameter();
	myBuildTool.AddEdgeVertex(newEdge,V);
	myBuildTool.Parameter(newEdge,V,parV);
      } // !equafound
      
    } // EDBUloop.InitVertex :  on vertices of new edge newEdge
    
    Standard_Boolean addedge = (nVF == 1 && nVR == 1);
    Standard_Boolean bitclosed = Standard_False;
    if (nVF == 1 && nVR == 1) {
      bitclosed = VF.IsSame(VR);
      newEdge.Closed(bitclosed);
    }
    if (addedge) {
      if (tosplit) {
	TopTools_ListOfShape loe; Standard_Boolean ok = TopOpeBRepTool_TOOL::SplitE(TopoDS::Edge(newEdge),loe);
	if (!ok) tosplit = Standard_False;
	else     LOE.Append(loe);
      }
      if (!tosplit) LOE.Append(newEdge);
    }    
  } // EDBU.InitEdge : loop on EDBU edges
  
  
#ifdef DRAW
  if(tSPS) {
    TCollection_AsciiString str1; str1 = "e";
    TCollection_AsciiString str2; str2 = iE;
    FDRAW_DINLOE("",LOE,str1,str2);
  }
#endif
  
} // GEDBUMakeEdges
