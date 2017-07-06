// Created on: 1997-04-17
// Created by: Christophe MARION
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

//#define No_Standard_OutOfRange

#include <BRepTopAdaptor_Tool.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomInt.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <HLRAlgo.hxx>
#include <HLRAlgo_Interference.hxx>
#include <HLRAlgo_ListIteratorOfInterferenceList.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <HLRBRep_EdgeFaceTool.hxx>
#include <HLRBRep_FaceData.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_TransitionOnCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <StdFail_UndefinedDerivative.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(HLRBRep_Data,Standard_Transient)

Standard_Integer nbOkIntersection;
Standard_Integer nbPtIntersection;
Standard_Integer nbSegIntersection;
Standard_Integer nbClassification;
Standard_Integer nbCal1Intersection; // pairs of unrejected edges
Standard_Integer nbCal2Intersection; // true intersections (not vertex)
Standard_Integer nbCal3Intersection; // Curve-Surface intersections

static const Standard_Real CutLar = 2.e-1;
static const Standard_Real CutBig = 1.e-1;

//-- voir HLRAlgo.cxx 

static const Standard_Real DERIVEE_PREMIERE_NULLE = 0.000000000001;

//-- ======================================================================
//--  

#include <IntRes2d_TypeTrans.hxx>
#include <IntRes2d_Position.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_Transition.hxx>

static long unsigned Mask32[32] = { 1,2,4,8,  16,32,64,128,  256,512,1024,2048,
				 4096,8192,16384,32768,
				 65536,131072,262144,524288,
				 1048576,2097152,4194304,8388608,
				 16777216,33554432,67108864,134217728,
				 268435456,536870912,1073741824,2147483648U};

static const Standard_Integer SIZEUV = 8;

class TableauRejection { 
public:
  Standard_Real **UV;               //-- UV[i][j]     contient le param (U sur Ci) de l intersection de Ci avec C(IndUV[j])
  Standard_Integer **IndUV;         //-- IndUV[i][j]  = J0   -> Intersection entre i et J0
  Standard_Integer *nbUV;           //-- nbUV[i][j]   nombre de valeurs pour la ligne i
  Standard_Integer N;
  
  long unsigned **TabBit;
  Standard_Integer nTabBit;

#ifdef OCCT_DEBUG
  Standard_Integer StNbLect,StNbEcr,StNbMax,StNbMoy,StNbMoyNonNul; //-- STAT
#endif

private:
  TableauRejection(const TableauRejection&);
  TableauRejection& operator=(const TableauRejection&);

public:
  //-- ============================================================
  TableauRejection() { 
    N=0; nTabBit=0;  UV=NULL;  nbUV=NULL;  IndUV=NULL; TabBit=NULL;
#ifdef OCCT_DEBUG
    StNbLect=StNbEcr=StNbMax=StNbMoy=StNbMoyNonNul=0;
#endif
  }
  //-- ============================================================
  void SetDim(const Standard_Integer n) {
#ifdef OCCT_DEBUG
    cout<<"\n@#@#@#@#@# SetDim "<<n<<endl;
#endif
    if(UV) 
      Destroy();
#ifdef OCCT_DEBUG
    StNbLect=StNbEcr=StNbMax=StNbMoy=0;
#endif
    N=n;
    UV   = (Standard_Real **)       malloc(N*sizeof(Standard_Real *));
    IndUV = (Standard_Integer **)   malloc(N*sizeof(Standard_Integer *));
    nbUV = (Standard_Integer *)     malloc(N*sizeof(Standard_Integer));
//    for(Standard_Integer i=0;i<N;i++) { 
    Standard_Integer i;
    for( i=0;i<N;i++) { 
      UV[i]=(Standard_Real *)       malloc(SIZEUV*sizeof(Standard_Real));
    }
    for(i=0;i<N;i++) {
      IndUV[i]=(Standard_Integer *) malloc(SIZEUV*sizeof(Standard_Integer));
      for(Standard_Integer k=0;k<SIZEUV;k++) { 
	IndUV[i][k]=-1;
      }
      nbUV[i]=SIZEUV;
    }
    InitTabBit(n);
  }
  //-- ============================================================
  ~TableauRejection() { 
    //-- cout<<"\n Destructeur TableauRejection"<<endl;
    Destroy(); 
  } 
  //-- ============================================================
  void Destroy() {
#ifdef OCCT_DEBUG
    if(N) { 
      Standard_Integer nnn=0;
      StNbMoy=StNbMoyNonNul=0;
      StNbMax=0;
      for(Standard_Integer i=0; i<N; i++) { 
	Standard_Integer nb=0;
	for(Standard_Integer j=0; IndUV[i][j]!=-1 && j<nbUV[i]; j++,nb++);
	if(nb>StNbMax) StNbMax=nb;
	StNbMoy+=nb;
	if(nb) { StNbMoyNonNul+=nb; nnn++; } 
      }
      
      printf("\n----------------------------------------");
      printf("\nNbLignes  : %10d",N);
      printf("\nNbLect    : %10d",StNbLect);
      printf("\nNbEcr     : %10d",StNbEcr);
      printf("\nNbMax     : %10d",StNbMax);
      printf("\nNbMoy     : %10d / %10d -> %d",StNbMoy,N,StNbMoy/N);
      if(nnn) { 
	printf("\nNbMoy !=0 : %10d / %10d -> %d",StNbMoyNonNul,nnn,StNbMoyNonNul/nnn);
      }
      printf("\n----------------------------------------\n");
    }
#endif
    if(N) { 
      ResetTabBit(N);
//      for(Standard_Integer i=0;i<N;i++) { 
      Standard_Integer i;
      for(i=0;i<N;i++) { 
	if(IndUV[i]) { 
	  free(IndUV[i]);
	  IndUV[i]=NULL;
	}
#ifdef OCCT_DEBUG
	else
          cout<<" IndUV ~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
#endif
      }
      for(i=0;i<N;i++) { 
	if(UV[i]) { 
	  free(UV[i]);
	  UV[i]=NULL;
	}
#ifdef OCCT_DEBUG
	else { cout<<" UV ~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl; } 
#endif
      }
      
      if(nbUV)  { free(nbUV);  nbUV=NULL; } 
      if(IndUV) { free(IndUV); IndUV=NULL;}
      if(UV) { free(UV);    UV=NULL; }
      N=0;
    }
  }
  //-- ============================================================
  void Set(Standard_Integer i0,Standard_Integer j0,const Standard_Real u) { 
    i0--; j0--;
#ifdef OCCT_DEBUG
    StNbEcr++;
#endif
    Standard_Integer k=-1;
//    for(Standard_Integer i=0; k==-1 && i<nbUV[i0]; i++) { 
    Standard_Integer i;
    for( i=0; k==-1 && i<nbUV[i0]; i++) { 
      if(IndUV[i0][i]==-1) { 
	k=i;
      }
    }
    if(k==-1) { //-- on agrandit le tableau
      //--
      //-- declaration de la Nv ligne de taille : ancienne taille + SIZEUV
      //-- 
      
      //-- cout<<" \n alloc nbUV["<<i0<<"]="<<nbUV[i0];

      Standard_Real    *NvLigneUV  = (Standard_Real *)   malloc((nbUV[i0]+SIZEUV)*sizeof(Standard_Real));
      Standard_Integer *NvLigneInd = (Standard_Integer *)malloc((nbUV[i0]+SIZEUV)*sizeof(Standard_Integer));
      //--
      //-- Recopie des anciennes valeurs ds la nouvelle ligne 
      //-- 
      for(i=0;i<nbUV[i0];i++) { 
	NvLigneUV[i]=UV[i0][i];
	NvLigneInd[i]=IndUV[i0][i];	
      }
      
      //-- mise a jour de la nouvelle dimension   ;  free des anciennes lignes et affectation
      k=nbUV[i0];
      nbUV[i0]+=SIZEUV;
      free(UV[i0]);
      free(IndUV[i0]);
      UV[i0]=NvLigneUV;
      IndUV[i0]=NvLigneInd;
      for(Standard_Integer kk=k ; kk<nbUV[i0];kk++) { 
	IndUV[i0][kk]=-1;
      }
    }
    IndUV[i0][k]=j0;
    UV[i0][k]=u;
    
    //-- tri par ordre decroissant
    Standard_Boolean TriOk;
    do { 
      TriOk=Standard_True;
      Standard_Integer im1=0;
      for(i=1; IndUV[i0][i]!=-1 && i<nbUV[i0]; i++,im1++) { 
	if(IndUV[i0][i]>IndUV[i0][im1]) { 
	  TriOk=Standard_False;
	  k=IndUV[i0][i]; IndUV[i0][i]=IndUV[i0][im1]; IndUV[i0][im1]=k;
	  Standard_Real t=UV[i0][i]; UV[i0][i]=UV[i0][im1]; UV[i0][im1]=t;
	}
      }
    }
    while(TriOk==Standard_False);   
  }
  //-- ============================================================
  Standard_Real Get(Standard_Integer i0,Standard_Integer j0) { 
    i0--; j0--;
#ifdef OCCT_DEBUG
    StNbLect++;
#endif

//--    for(Standard_Integer i=0; IndUV[i0][i]!=-1 && i<nbUV[i0]; i++) { 
//--      if(IndUV[i0][i]==j0) { 
//--	return(UV[i0][i]);
//--      }
//--    }
    //-- ordre decroissant
    Standard_Integer a=0,b=nbUV[i0]-1,ab;
    if(IndUV[i0][a]==-1) return(RealLast());
    if(IndUV[i0][a]==j0) return(UV[i0][a]);
    if(IndUV[i0][b]==j0) return(UV[i0][b]);
    while((IndUV[i0][a]>j0) && (IndUV[i0][b]<j0)) { 
      ab=(a+b)>>1;
      if(IndUV[i0][ab] < j0)      { if(b==ab) return(RealLast()); else b=ab; }
      else if(IndUV[i0][ab] > j0) { if(a==ab) return(RealLast()); else a=ab; } 
      else { return(UV[i0][ab]); } 
    }

    return(RealLast());
  }
  //-- ============================================================
  void ResetTabBit(const Standard_Integer nbedgs) { 
    //-- cout<<"\n ResetTabBit"<<endl;
    if(TabBit) { 
      for(Standard_Integer i=0;i<nbedgs;i++) { 
	if(TabBit[i]) {
	  free(TabBit[i]);
	  TabBit[i]=NULL;
	}
      }
      free(TabBit);
      TabBit=NULL;
      nTabBit=0;
    }
  }
  //-- ============================================================
  void InitTabBit(const Standard_Integer nbedgs) { 
    //--  cout<<"\n InitTabBit"<<endl;
    if(TabBit && nTabBit) { 
      ResetTabBit(nTabBit);
    }
    TabBit = (long unsigned **) malloc((nbedgs)*sizeof(long unsigned *));
    nTabBit=nbedgs;
    Standard_Integer n=1+(nbedgs>>5);
    
    for(Standard_Integer i=0;i<nbedgs;i++) { 
      TabBit[i]=(long unsigned *) malloc(n*sizeof(long unsigned));
      for(Standard_Integer j=0;j<n;j++) { 
	TabBit[i][j]=0;
      }
    }
  }
  //-- ============================================================
  void SetNoIntersection(Standard_Integer i0,Standard_Integer i1) {
    //  cout<<" SetNoIntersection : "<<i0<<" "<<i1<<endl;
    i0--;
    i1--;
    if(i0>i1) { 
      Standard_Integer t = i0; i0=i1; i1=t;
    }
    Standard_Integer c=i1>>5;
    Standard_Integer o=i1 & 31;
    TabBit[i0][c] |=  Mask32[o];
  }
  //-- ============================================================
  Standard_Boolean NoIntersection(Standard_Integer i0,Standard_Integer i1) { 
    //  cout<<" ??NoIntersection : "<<i0<<" "<<i1<<" ";
    i0--;
    i1--;
    if(i0>i1) { 
      Standard_Integer t = i0; i0=i1; i1=t;
    }
    Standard_Integer c=i1>>5;
    Standard_Integer o=i1 & 31;
    if(TabBit[i0][c] & Mask32[o]) { 
      //--    cout<<" TRUE "<<endl;
      return(Standard_True);
    }
    //--  cout<<" FALSE "<<endl;
    return(Standard_False);
  }
  //-- ============================================================
  void SetIntersection(Standard_Integer i0,
		       Standard_Integer i1,
		       const IntRes2d_IntersectionPoint& IP) { 
    const IntRes2d_Transition& T1=IP.TransitionOfFirst();
    const IntRes2d_Transition& T2=IP.TransitionOfSecond();
    if(T1.PositionOnCurve()==IntRes2d_Middle) { 
      if(T2.PositionOnCurve()==IntRes2d_Middle) { 
	if(   T1.TransitionType()==IntRes2d_In 
	   || T1.TransitionType()==IntRes2d_Out) { 
	  Set(i0,i1,IP.ParamOnFirst());
	  Set(i1,i0,IP.ParamOnSecond());
	}
      }
    }
  }
  //-- ============================================================
  void GetSingleIntersection(Standard_Integer i0,Standard_Integer i1,
			     Standard_Real& u,Standard_Real& v ) { 
    u=Get(i0,i1);
    if(u!=RealLast()) { 
      v=Get(i1,i0);
    }
    else { 
      v=RealLast();
    }
  }
};
 
//-- ================================================================================


//=======================================================================
//function : AdjustParameter
//purpose  : 
//=======================================================================

static void AdjustParameter (HLRBRep_EdgeData* E,
			     const Standard_Boolean h,
			     Standard_Real& p,
			     Standard_ShortReal& t)
{
  Standard_Real p1,p2;
  Standard_ShortReal t1,t2;
  if (h) {
    E->Status().Bounds(p,t,p2,t2);
    if (E->VerAtSta()) p = p + (p2 - p) * CutBig;
  }
  else {
    E->Status().Bounds(p1,t1,p,t);
    if (E->VerAtEnd()) p = p - (p - p1) * CutBig;
  }
}

//=======================================================================
//function : Data
//purpose  : 
//=======================================================================

HLRBRep_Data::HLRBRep_Data (const Standard_Integer NV,
			    const Standard_Integer NE,
			    const Standard_Integer NF) :
			    myNbVertices   (NV),
			    myNbEdges      (NE),
			    myNbFaces      (NF),
			    myEData      (0,NE),
			    myFData      (0,NF),
			    myEdgeIndices(0,NE),
			    myToler((Standard_ShortReal)1e-5),
			    myLLProps(2,Epsilon(1.)),
			    myFLProps(2,Epsilon(1.)),
			    mySLProps(2,Epsilon(1.)),
			    myHideCount(0)
{
  myReject = new TableauRejection();
  ((TableauRejection *)myReject)->SetDim(myNbEdges);
}

void HLRBRep_Data::Destroy() { 
  //-- cout<<"\n HLRBRep_Data::~HLRBRep_Data()"<<endl;
  ((TableauRejection *)myReject)->Destroy();
  delete ((TableauRejection *)myReject);
}
//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void HLRBRep_Data::Write (const Handle(HLRBRep_Data)& DS,
			  const Standard_Integer dv,
			  const Standard_Integer de,
			  const Standard_Integer df)
{
  Standard_Integer n1edge = DS->NbEdges();
  Standard_Integer n1face = DS->NbFaces();

  HLRBRep_EdgeData* ed = &(myEData         .ChangeValue(de));
  HLRBRep_EdgeData* e1 = &(DS->EDataArray().ChangeValue(0 ));
  ed++;
  e1++;

  HLRBRep_FaceData* fd = &(myFData         .ChangeValue(df));
  HLRBRep_FaceData* f1 = &(DS->FDataArray().ChangeValue(0 ));
  fd++;
  f1++;

  for (Standard_Integer iedge = 1; iedge <= n1edge; iedge++) {
    *ed = *e1;

    if (dv != 0) {
      ed->VSta(ed->VSta() + dv);
      ed->VEnd(ed->VEnd() + dv);
    }

    myEMap.Add(DS->EdgeMap().FindKey(iedge));

    ed++;
    e1++;
  } 

  for (Standard_Integer iface = 1; iface <= n1face; iface++) {
    *fd = *f1;

    if (de != 0) {
      const Handle(HLRAlgo_WiresBlock)& wb = fd->Wires();
      Standard_Integer nw = wb->NbWires();

      for (Standard_Integer iw = 1; iw <= nw; iw++) {
	const Handle(HLRAlgo_EdgesBlock)& eb = wb->Wire(iw);
	Standard_Integer ne = eb->NbEdges();

	for (Standard_Integer ie = 1; ie <= ne; ie++)
	  eb->Edge(ie,eb->Edge(ie) + de);
      }
    }

    myFMap.Add(DS->FaceMap().FindKey(iface));

    fd++;
    f1++;
  } 
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void HLRBRep_Data::Update (const HLRAlgo_Projector& P)
{
  myProj = P;
  const gp_Trsf& T = myProj.Transformation();
  Standard_Integer i;
  Standard_Real tolMinMax = 0;

  HLRAlgo_EdgesBlock::MinMaxIndices FaceMin, FaceMax;
  HLRAlgo_EdgesBlock::MinMaxIndices MinMaxFace;
  HLRAlgo_EdgesBlock::MinMaxIndices WireMin, WireMax, MinMaxWire;
  HLRAlgo_EdgesBlock::MinMaxIndices EdgeMin, EdgeMax;
  HLRAlgo_EdgesBlock::MinMaxIndices MinMaxEdge;
  Standard_Real TotMin[16],TotMax[16];
  HLRAlgo::InitMinMax(Precision::Infinite(), TotMin, TotMax);

  // compute the global MinMax
  // *************************
//  for (Standard_Integer edge = 1; edge <= myNbEdges; edge++) {
  Standard_Integer edge;
  for ( edge = 1; edge <= myNbEdges; edge++) {
    HLRBRep_EdgeData& ed = myEData.ChangeValue(edge);
    HLRBRep_Curve& EC = ed.ChangeGeometry();
    EC.Projector(&myProj);
    Standard_Real enl =EC.Update(TotMin, TotMax);
    if (enl > tolMinMax) tolMinMax = enl;
  }
  HLRAlgo::EnlargeMinMax(tolMinMax, TotMin, TotMax);
  Standard_Real d[16];
  Standard_Real precad = -Precision::Infinite();

  for (i = 0; i <= 15; i++) {
    d[i] = TotMax[i] - TotMin[i];
    if (precad < d[i]) precad = d[i];
  }
  myBigSize = precad;
  precad = precad * 0.0005;

  for (i = 0; i <= 15; i++)
    mySurD[i] = 0x00007fff / (d[i] + precad);
  precad = precad * 0.5;

  for (i = 0; i <= 15; i++)
    myDeca[i] = - TotMin[i] + precad;

  Standard_Real tol;
  Standard_Boolean ver1,ver2;

  // update the edges
  // ****************
  
  for (edge = 1; edge <= myNbEdges; edge++) {

    HLRBRep_EdgeData& ed = myEData.ChangeValue(edge);
    HLRBRep_Curve& EC = ed.ChangeGeometry();
    HLRAlgo::InitMinMax(Precision::Infinite(), TotMin, TotMax);
    tolMinMax = EC.UpdateMinMax(TotMin, TotMax);
    tol = (Standard_Real)(ed.Tolerance());
    ed.Vertical(TotMax[0] - TotMin[0] < tol &&
		 TotMax[1] - TotMin[1] < tol &&
		 TotMax[2] - TotMin[2] < tol &&
		 TotMax[3] - TotMin[3] < tol &&
		 TotMax[4] - TotMin[4] < tol &&
		 TotMax[5] - TotMin[5] < tol &&
		 TotMax[6] - TotMin[6] < tol );
    HLRAlgo::EnlargeMinMax(tolMinMax, TotMin, TotMax);
// Linux warning :  assignment to `int' from `double'. Cast has been added.
    EdgeMin.Min[0] = (Standard_Integer)( (myDeca[ 0] + TotMin[ 0]) * mySurD[ 0]);
    EdgeMax.Min[0] = (Standard_Integer)( (myDeca[ 0] + TotMax[ 0]) * mySurD[ 0]);
    EdgeMin.Min[1] = (Standard_Integer)( (myDeca[ 1] + TotMin[ 1]) * mySurD[ 1]);
    EdgeMax.Min[1] = (Standard_Integer)( (myDeca[ 1] + TotMax[ 1]) * mySurD[ 1]);
    EdgeMin.Min[2] = (Standard_Integer)( (myDeca[ 2] + TotMin[ 2]) * mySurD[ 2]);
    EdgeMax.Min[2] = (Standard_Integer)( (myDeca[ 2] + TotMax[ 2]) * mySurD[ 2]);
    EdgeMin.Min[3] = (Standard_Integer)( (myDeca[ 3] + TotMin[ 3]) * mySurD[ 3]);
    EdgeMax.Min[3] = (Standard_Integer)( (myDeca[ 3] + TotMax[ 3]) * mySurD[ 3]);
    EdgeMin.Min[4] = (Standard_Integer)( (myDeca[ 4] + TotMin[ 4]) * mySurD[ 4]);
    EdgeMax.Min[4] = (Standard_Integer)( (myDeca[ 4] + TotMax[ 4]) * mySurD[ 4]);
    EdgeMin.Min[5] = (Standard_Integer)( (myDeca[ 5] + TotMin[ 5]) * mySurD[ 5]);
    EdgeMax.Min[5] = (Standard_Integer)( (myDeca[ 5] + TotMax[ 5]) * mySurD[ 5]);
    EdgeMin.Min[6] = (Standard_Integer)( (myDeca[ 6] + TotMin[ 6]) * mySurD[ 6]);
    EdgeMax.Min[6] = (Standard_Integer)( (myDeca[ 6] + TotMax[ 6]) * mySurD[ 6]);
    EdgeMin.Min[7] = (Standard_Integer)( (myDeca[ 7] + TotMin[ 7]) * mySurD[ 7]);
    EdgeMax.Min[7] = (Standard_Integer)( (myDeca[ 7] + TotMax[ 7]) * mySurD[ 7]);
    EdgeMin.Max[0] = (Standard_Integer)( (myDeca[ 8] + TotMin[ 8]) * mySurD[ 8]);
    EdgeMax.Max[0] = (Standard_Integer)( (myDeca[ 8] + TotMax[ 8]) * mySurD[ 8]);
    EdgeMin.Max[1] = (Standard_Integer)( (myDeca[ 9] + TotMin[ 9]) * mySurD[ 9]);
    EdgeMax.Max[1] = (Standard_Integer)( (myDeca[ 9] + TotMax[ 9]) * mySurD[ 9]);
    EdgeMin.Max[2] = (Standard_Integer)( (myDeca[10] + TotMin[10]) * mySurD[10]);
    EdgeMax.Max[2] = (Standard_Integer)( (myDeca[10] + TotMax[10]) * mySurD[10]);
    EdgeMin.Max[3] = (Standard_Integer)( (myDeca[11] + TotMin[11]) * mySurD[11]);
    EdgeMax.Max[3] = (Standard_Integer)( (myDeca[11] + TotMax[11]) * mySurD[11]);
    EdgeMin.Max[4] = (Standard_Integer)( (myDeca[12] + TotMin[12]) * mySurD[12]);
    EdgeMax.Max[4] = (Standard_Integer)( (myDeca[12] + TotMax[12]) * mySurD[12]);
    EdgeMin.Max[5] = (Standard_Integer)( (myDeca[13] + TotMin[13]) * mySurD[13]);
    EdgeMax.Max[5] = (Standard_Integer)( (myDeca[13] + TotMax[13]) * mySurD[13]);
    EdgeMin.Max[6] = (Standard_Integer)( (myDeca[14] + TotMin[14]) * mySurD[14]);
    EdgeMax.Max[6] = (Standard_Integer)( (myDeca[14] + TotMax[14]) * mySurD[14]);
    EdgeMin.Max[7] = (Standard_Integer)( (myDeca[15] + TotMin[15]) * mySurD[15]);
    EdgeMax.Max[7] = (Standard_Integer)( (myDeca[15] + TotMax[15]) * mySurD[15]);

    HLRAlgo::EncodeMinMax(EdgeMin, EdgeMax, MinMaxEdge);
    ed.UpdateMinMax(MinMaxEdge);
    if (ed.Vertical()) {
      ver1 = Standard_True;
      ver2 = Standard_True;
      Standard_Integer vsta = ed.VSta();
      Standard_Integer vend = ed.VEnd();
      Standard_Boolean vout = ed.OutLVSta() || ed.OutLVEnd();
      Standard_Boolean vcut = ed.CutAtSta() || ed.CutAtEnd();
      
      for (Standard_Integer ebis = 1; ebis <= myNbEdges; ebis++) {
        HLRBRep_EdgeData& eb = myEData.ChangeValue(ebis);
        if (vsta == eb.VSta()) {
	  eb.VSta    (vend);
	  eb.OutLVSta(vout);
	  eb.CutAtSta(vcut);
	}
	else if (vsta == eb.VEnd()) {
	  eb.VEnd    (vend);
	  eb.OutLVEnd(vout);
	  eb.CutAtEnd(vcut);
	}
      }
    }
    else {
      gp_Pnt Pt;
      gp_Vec Tg1,Tg2;
      EC.D1(EC.Parameter3d(EC.FirstParameter()),Pt,Tg1);
      EC.D1(EC.Parameter3d(EC.LastParameter ()),Pt,Tg2);
      Tg1.Transform(T);
      Tg2.Transform(T);
      if (Abs(Tg1.X()) + Abs(Tg1.Y()) < myToler * 10) ver1 = Standard_True;
      else {
	gp_Dir Dir1(Tg1);
	ver1 = Abs(Dir1.X()) + Abs(Dir1.Y()) < myToler * 10;
      }
      if (Abs(Tg2.X()) + Abs(Tg2.Y()) < myToler * 10) ver2 = Standard_True;
      else {
	gp_Dir Dir2(Tg2);
	ver2 = Abs(Dir2.X()) + Abs(Dir2.Y()) < myToler * 10;
      }
    }
    ed.VerAtSta(ed.Vertical() || ver1);
    ed.VerAtEnd(ed.Vertical() || ver2);
    ed.AutoIntersectionDone(Standard_True);
    ed.Simple(Standard_True);
  }
  
  // update the faces
  // ****************
  
  for (Standard_Integer face = 1; face <= myNbFaces; face++) {

    HLRBRep_FaceData& fd = myFData.ChangeValue(face);
    HLRBRep_Surface& FS = fd.Geometry();
    iFaceGeom = &(fd.Geometry());
    mySLProps.SetSurface(iFaceGeom);
    FS.Projector(&myProj);
    iFaceType = FS.GetType();

    // Is the face cut by an outline

    Standard_Boolean cut      = Standard_False;
    Standard_Boolean withOutL = Standard_False;
    
    for (myFaceItr1.InitEdge(fd);
	 myFaceItr1.MoreEdge();
	 myFaceItr1.NextEdge()) {
      if (myFaceItr1.Internal()) {
	withOutL = Standard_True;
	cut      = Standard_True;
      }
      else if (myFaceItr1.OutLine()) {
	withOutL = Standard_True;
	if (myFaceItr1.Double()) cut = Standard_True;
      }
    }
    fd.Cut     (cut);
    fd.WithOutL(withOutL);

    // Is the face simple = no auto-hiding
    // not cut and simple surface

    if (!withOutL && 
	(iFaceType == GeomAbs_Plane    ||
	 iFaceType == GeomAbs_Cylinder ||
	 iFaceType == GeomAbs_Cone     ||
	 iFaceType == GeomAbs_Sphere   ||
	 iFaceType == GeomAbs_Torus )) fd.Simple(Standard_True );
    else                               fd.Simple(Standard_False);

    fd.Plane   (iFaceType == GeomAbs_Plane   );
    fd.Cylinder(iFaceType == GeomAbs_Cylinder);
    fd.Cone    (iFaceType == GeomAbs_Cone    );
    fd.Sphere  (iFaceType == GeomAbs_Sphere  );
    fd.Torus   (iFaceType == GeomAbs_Torus   );
    tol = (Standard_Real)(fd.Tolerance());
    fd.Side(FS.IsSide(tol,myToler*10));
    Standard_Boolean inverted = Standard_False;
    if (fd.WithOutL() && !fd.Side()) {
      inverted = OrientOutLine(face,fd);
      OrientOthEdge(face,fd);
    }
    if (fd.Side()) {
      fd.Hiding(Standard_False);
      fd.Back(Standard_False);
    }
    else if (!fd.WithOutL()) {
      Standard_Real p,pu,pv,r;
      fd.Back(Standard_False);
      Standard_Boolean found = Standard_False;

      for (myFaceItr1.InitEdge(fd);
	   myFaceItr1.MoreEdge() && !found;
	   myFaceItr1.NextEdge()) {
	myFE         = myFaceItr1.Edge       ();
	myFEOri      = myFaceItr1.Orientation();
	myFEOutLine  = myFaceItr1.OutLine    ();
	myFEInternal = myFaceItr1.Internal   ();
	myFEDouble   = myFaceItr1.Double     ();
	HLRBRep_EdgeData& EDataFE1 = myEData(myFE);
	if (!myFEDouble &&
	    (myFEOri == TopAbs_FORWARD ||
	     myFEOri == TopAbs_REVERSED)) {
	  myFEGeom = &(EDataFE1.ChangeGeometry());
	  const HLRBRep_Curve& EC = EDataFE1.Geometry();
	  p = EC.Parameter3d((EC.LastParameter () +
			      EC.FirstParameter()) / 2);
	  if (HLRBRep_EdgeFaceTool::UVPoint(p,myFEGeom,iFaceGeom,pu,pv)) {
	    mySLProps.SetParameters(pu,pv);
	    gp_Pnt Pt;
	    Pt = EC.Value3D(p);
            if (mySLProps.IsNormalDefined())
            {
              gp_Vec Nm = mySLProps.Normal();
              Pt.Transform(T);
              Nm.Transform(T);
              if (myProj.Perspective()) {
                r = Nm.Z() * myProj.Focus() - 
                  ( Nm.X() * Pt.X() + Nm.Y() * Pt.Y() + Nm.Z() * Pt.Z() );
              }
              else r = Nm.Z();
              if (Abs(r) > myToler*10) {
                fd.Back( r < 0 );
                found = Standard_True;
                break;
              }
            }
          }
	}
      }

      if (!found) {
	fd.Side(Standard_True);
	fd.Hiding(Standard_False);
	fd.Back(Standard_False);
      }
      else if (fd.Closed()) {
	switch (fd.Orientation()) {
	case TopAbs_REVERSED : fd.Hiding( fd.Back()   ); break;
	case TopAbs_FORWARD  : fd.Hiding(!fd.Back()   ); break;
	case TopAbs_EXTERNAL : fd.Hiding(Standard_True ); break;
	case TopAbs_INTERNAL : fd.Hiding(Standard_False); break;
	}
      }
      else fd.Hiding(Standard_True);
    }
    else {
      if (inverted) {
	fd.Hiding(Standard_False);
	fd.Back(Standard_True);
      }
      else {
	fd.Hiding(Standard_True);
	fd.Back(Standard_False);
      }
    }

    Standard_Boolean FirstTime = Standard_True;

    for (myFaceItr1.InitEdge(fd);
	 myFaceItr1.MoreEdge();
	 myFaceItr1.NextEdge()) {
      myFE = myFaceItr1.Edge();
      HLRBRep_EdgeData& EDataFE2 = myEData(myFE);
      if (!fd.Simple()) EDataFE2.AutoIntersectionDone(Standard_False);
      HLRAlgo::DecodeMinMax(EDataFE2.MinMax(), EdgeMin, EdgeMax);
      if (myFaceItr1.BeginningOfWire())
	HLRAlgo::CopyMinMax(EdgeMin, EdgeMax, WireMin, WireMax);
      else
	HLRAlgo::AddMinMax(EdgeMin, EdgeMax, WireMin, WireMax);
      if (myFaceItr1.EndOfWire()) {
	HLRAlgo::EncodeMinMax(WireMin, WireMax, MinMaxWire);
	myFaceItr1.Wire()->UpdateMinMax(MinMaxWire);
	if (FirstTime) {
	  FirstTime = Standard_False;
	  HLRAlgo::CopyMinMax(WireMin, WireMax, FaceMin, FaceMax);
	}
	else
	  HLRAlgo::AddMinMax(WireMin, WireMax, FaceMin, FaceMax);
      }
    }
    HLRAlgo::EncodeMinMax(FaceMin, FaceMax, MinMaxFace);
    fd.Wires()->UpdateMinMax(MinMaxFace);  
    fd.Size(HLRAlgo::SizeBox(FaceMin,FaceMax));
  }
}

//=======================================================================
//function : InitBoundSort
//purpose  : 
//=======================================================================

void 
HLRBRep_Data::InitBoundSort (const HLRAlgo_EdgesBlock::MinMaxIndices& MinMaxTot,
			     const Standard_Integer e1,
			     const Standard_Integer e2)
{
  myNbrSortEd = 0;
  const HLRAlgo_EdgesBlock::MinMaxIndices& MinMaxShap = MinMaxTot;

  for (Standard_Integer e = e1; e <= e2; e++) {
    HLRBRep_EdgeData& ed = myEData(e);
    if (!ed.Status().AllHidden()) {
      myLEMinMax = &ed.MinMax();
      if (((MinMaxShap.Max[0] - myLEMinMax->Min[0]) & 0x80008000) == 0 &&
	  ((myLEMinMax->Max[0] - MinMaxShap.Min[0]) & 0x80008000) == 0 &&
	  ((MinMaxShap.Max[1] - myLEMinMax->Min[1]) & 0x80008000) == 0 &&
	  ((myLEMinMax->Max[1] - MinMaxShap.Min[1]) & 0x80008000) == 0 &&
	  ((MinMaxShap.Max[2] - myLEMinMax->Min[2]) & 0x80008000) == 0 &&
	  ((myLEMinMax->Max[2] - MinMaxShap.Min[2]) & 0x80008000) == 0 &&
	  ((MinMaxShap.Max[3] - myLEMinMax->Min[3]) & 0x80008000) == 0 &&
	  ((myLEMinMax->Max[3] - MinMaxShap.Min[3]) & 0x80008000) == 0 &&
	  ((MinMaxShap.Max[4] - myLEMinMax->Min[4]) & 0x80008000) == 0 &&
	  ((myLEMinMax->Max[4] - MinMaxShap.Min[4]) & 0x80008000) == 0 &&
	  ((MinMaxShap.Max[5] - myLEMinMax->Min[5]) & 0x80008000) == 0 &&
	  ((myLEMinMax->Max[5] - MinMaxShap.Min[5]) & 0x80008000) == 0 &&
	  ((MinMaxShap.Max[6] - myLEMinMax->Min[6]) & 0x80008000) == 0 &&
	  ((myLEMinMax->Max[6] - MinMaxShap.Min[6]) & 0x80008000) == 0 &&
	  ((MinMaxShap.Max[7] - myLEMinMax->Min[7]) & 0x80008000) == 0) {  //- rejection en z 
	myNbrSortEd++;
	myEdgeIndices(myNbrSortEd) = e;
      }
    }
  }
}

//=======================================================================
//function : InitEdge
//purpose  : 
//=======================================================================
void HLRBRep_Data::InitEdge (const Standard_Integer FI,
			     BRepTopAdaptor_MapOfShapeTool& MST)
{
  myHideCount++;
  myHideCount++;

  iFace       = FI;
  iFaceData   = &myFData(iFace);
  iFaceGeom   = &iFaceData->Geometry();
  iFaceBack   = iFaceData->Back();
  iFaceSimp   = iFaceData->Simple();
  iFaceMinMax = &iFaceData->Wires()->MinMax();
  iFaceType   =   ((HLRBRep_Surface*)iFaceGeom)->GetType();
  iFaceTest   = !iFaceSimp;
  mySLProps.SetSurface(iFaceGeom);
  myIntersector.Load(iFaceGeom);


  HLRBRep_Surface  *p1 = (HLRBRep_Surface*)iFaceGeom;
  const BRepAdaptor_Surface& bras=p1->Surface();
  

  const TopoDS_Face& topodsface=bras.Face();
  

  

  if(MST.IsBound(topodsface)) {  
    BRepTopAdaptor_Tool& BRT = MST.ChangeFind(topodsface);
    myClassifier  = BRT.GetTopolTool();
  }
  else { 
    BRepTopAdaptor_Tool BRT(topodsface,Precision::PConfusion());
    MST.Bind(topodsface,BRT);
    myClassifier = BRT.GetTopolTool();
  }
  
  if (iFaceTest) {
    iFaceSmpl = !iFaceData->Cut();
    myFaceItr2.InitEdge(*iFaceData);
  }
  else {

    for (myFaceItr1.InitEdge(*iFaceData);
	 myFaceItr1.MoreEdge();
	 myFaceItr1.NextEdge()) {
      myFE = myFaceItr1.Edge();               // edges of a simple hiding
      myEData(myFE).HideCount(myHideCount-1); // face must be jumped.
    }
    myCurSortEd = 1;
  }
  NextEdge(Standard_False);
}

//=======================================================================
//function : MoreEdge
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_Data::MoreEdge ()
{


  if (iFaceTest) {
    if (myFaceItr2.MoreEdge()) {            // all edges must be tested if
      myLE         = myFaceItr2.Edge    (); // the face is not a simple
      myLEOutLine  = myFaceItr2.OutLine (); // one. 
      myLEInternal = myFaceItr2.Internal();
      myLEDouble   = myFaceItr2.Double  ();
      myLEIsoLine  = myFaceItr2.IsoLine ();
      myLEData     = &myEData(myLE);
      myLEGeom     = &myLEData->ChangeGeometry();
      myLEMinMax   = &myLEData->MinMax();
      myLETol      = myLEData->Tolerance();
      myLEType     = myLEGeom->GetType();
      if (!myLEDouble)
        myLEData->HideCount(myHideCount-1);
      return Standard_True;
    }
    else {
      iFaceTest = Standard_False;      // at the end of the test
      iFaceSimp = iFaceSmpl;           // we know if it is a simple face
      iFaceData->Simple(iFaceSimp);
      myCurSortEd = 1;
      NextEdge(Standard_False);
    }
  }
  return myCurSortEd <= myNbrSortEd;
}
//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void HLRBRep_Data::NextEdge (const Standard_Boolean skip)
{

  if (skip) {
    if (iFaceTest) myFaceItr2.NextEdge();
    else           myCurSortEd++;
  }
  if (!MoreEdge()) return;
  if (iFaceTest) {
    myLE         = myFaceItr2.Edge    ();
    myLEOutLine  = myFaceItr2.OutLine ();
    myLEInternal = myFaceItr2.Internal();
    myLEDouble   = myFaceItr2.Double  ();
    myLEIsoLine  = myFaceItr2.IsoLine ();
    myLEData     = &myEData(myLE);
    myLEGeom     = &myLEData->ChangeGeometry();
    myLEMinMax   = &myLEData->MinMax();
    myLETol      = myLEData->Tolerance();
    myLEType     = myLEGeom->GetType();
    if (((HLRBRep_EdgeData*)myLEData)->Vertical() ||
	(myLEDouble &&
	 ((HLRBRep_EdgeData*)myLEData)->HideCount() == myHideCount-1))
      NextEdge();
    ((HLRBRep_EdgeData*)myLEData)->HideCount(myHideCount-1);
    return;
  }
  else {
    myLE         = Edge();
    myLEOutLine  = Standard_False;
    myLEInternal = Standard_False;
    myLEDouble   = Standard_False;
    myLEIsoLine  = Standard_False;
    myLEData     = &myEData(myLE);
    myLEGeom     = &myLEData->ChangeGeometry();
    myLEMinMax   = &myLEData->MinMax();
    myLETol      = myLEData->Tolerance();
    myLEType     = myLEGeom->GetType();
  }
  if (((HLRBRep_EdgeData*)myLEData)->Vertical()) {
    NextEdge();
    return;
  }
  if (((HLRBRep_EdgeData*)myLEData)->HideCount() > myHideCount-2) {
    NextEdge();
    return;
  }
  if (((HLRBRep_EdgeData*)myLEData)->Status().AllHidden()) {
    NextEdge();
    return;
  }
  if (((iFaceMinMax->Max[0] - myLEMinMax->Min[0]) & 0x80008000) != 0 ||
      ((myLEMinMax->Max[0] - iFaceMinMax->Min[0]) & 0x80008000) != 0 ||
      ((iFaceMinMax->Max[1] - myLEMinMax->Min[1]) & 0x80008000) != 0 ||
      ((myLEMinMax->Max[1] - iFaceMinMax->Min[1]) & 0x80008000) != 0 ||
      ((iFaceMinMax->Max[2] - myLEMinMax->Min[2]) & 0x80008000) != 0 ||
      ((myLEMinMax->Max[2] - iFaceMinMax->Min[2]) & 0x80008000) != 0 ||
      ((iFaceMinMax->Max[3] - myLEMinMax->Min[3]) & 0x80008000) != 0 ||
      ((myLEMinMax->Max[3] - iFaceMinMax->Min[3]) & 0x80008000) != 0 ||
      ((iFaceMinMax->Max[4] - myLEMinMax->Min[4]) & 0x80008000) != 0 ||
      ((myLEMinMax->Max[4] - iFaceMinMax->Min[4]) & 0x80008000) != 0 ||
      ((iFaceMinMax->Max[5] - myLEMinMax->Min[5]) & 0x80008000) != 0 ||
      ((myLEMinMax->Max[5] - iFaceMinMax->Min[5]) & 0x80008000) != 0 ||
      ((iFaceMinMax->Max[6] - myLEMinMax->Min[6]) & 0x80008000) != 0 ||
      ((myLEMinMax->Max[6] - iFaceMinMax->Min[6]) & 0x80008000) != 0 ||
      ((iFaceMinMax->Max[7] - myLEMinMax->Min[7]) & 0x80008000) != 0) { //-- rejection en z 
    NextEdge();
    return;
  }
  if (((HLRBRep_Surface*)iFaceGeom)->IsAbove
      (iFaceBack,myLEGeom,(Standard_Real)myLETol)) {
    NextEdge();
    return;
  }
  return;               // edge is OK
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

Standard_Integer HLRBRep_Data::Edge () const
{
  if (iFaceTest) return myFaceItr2.Edge();
  else           return myEdgeIndices(myCurSortEd);
}

//=======================================================================
//function : InitInterference
//purpose  : 
//=======================================================================

void HLRBRep_Data::InitInterference ()
{
  myLLProps.SetCurve(myLEGeom);
  myFaceItr1.InitEdge(*((HLRBRep_FaceData*)iFaceData));
  myNbPoints = myNbSegments = iInterf = 0;
  NextInterference();
}

//=======================================================================
//function : NextInterference
//purpose  : 
//=======================================================================

void HLRBRep_Data::NextInterference ()
{
  // are there more intersections on the current edge
  iInterf++;
//  Standard_Integer miniWire1,miniWire2;
//  Standard_Integer maxiWire1,maxiWire2,maxiWire3,maxiWire4;

  while (!MoreInterference() && myFaceItr1.MoreEdge()) {
    
    // rejection of current wire
    if (myFaceItr1.BeginningOfWire()) {
      HLRAlgo_EdgesBlock::MinMaxIndices& MinMaxWire = myFaceItr1.Wire()->MinMax();
      if (((MinMaxWire.Max[0] - myLEMinMax->Min[0]) & 0x80008000) != 0 ||
	  ((myLEMinMax->Max[0] - MinMaxWire.Min[0]) & 0x80008000) != 0 ||
	  ((MinMaxWire.Max[1] - myLEMinMax->Min[1]) & 0x80008000) != 0 ||
	  ((myLEMinMax->Max[1] - MinMaxWire.Min[1]) & 0x80008000) != 0 ||
	  ((MinMaxWire.Max[2] - myLEMinMax->Min[2]) & 0x80008000) != 0 ||
	  ((myLEMinMax->Max[2] - MinMaxWire.Min[2]) & 0x80008000) != 0 ||
	  ((MinMaxWire.Max[3] - myLEMinMax->Min[3]) & 0x80008000) != 0 ||
	  ((myLEMinMax->Max[3] - MinMaxWire.Min[3]) & 0x80008000) != 0 ||
	  ((MinMaxWire.Max[4] - myLEMinMax->Min[4]) & 0x80008000) != 0 ||
	  ((myLEMinMax->Max[4] - MinMaxWire.Min[4]) & 0x80008000) != 0 ||
	  ((MinMaxWire.Max[5] - myLEMinMax->Min[5]) & 0x80008000) != 0 ||
	  ((myLEMinMax->Max[5] - MinMaxWire.Min[5]) & 0x80008000) != 0 ||
	  ((MinMaxWire.Max[6] - myLEMinMax->Min[6]) & 0x80008000) != 0 ||
	  ((myLEMinMax->Max[6] - MinMaxWire.Min[6]) & 0x80008000) != 0 ||
	  ((MinMaxWire.Max[7] - myLEMinMax->Min[7]) & 0x80008000) != 0) { //-- Rejection en Z
	myFaceItr1.SkipWire();
	continue;
      }
    }
    myFE         = myFaceItr1.Edge();
    myFEOri      = myFaceItr1.Orientation();
    myFEOutLine  = myFaceItr1.OutLine    ();
    myFEInternal = myFaceItr1.Internal   ();
    myFEDouble   = myFaceItr1.Double     ();
    myFEData = &myEData(myFE);
    myFEGeom = &(((HLRBRep_EdgeData*)myFEData)->ChangeGeometry());
    myFETol  =   ((HLRBRep_EdgeData*)myFEData)->Tolerance();
    myFEType =   ((HLRBRep_Curve   *)myFEGeom)->GetType();

    
    if (myFEOri == TopAbs_FORWARD ||
	myFEOri == TopAbs_REVERSED) {
      // Edge from the boundary
      if (!((HLRBRep_EdgeData*)myFEData)->Vertical() && !(myFEDouble && !myFEOutLine)) {
	// not a vertical edge and not a double Edge
	HLRAlgo_EdgesBlock::MinMaxIndices* MinMaxFEdg = &((HLRBRep_EdgeData*)myFEData)->MinMax();
	//-- -----------------------------------------------------------------------
	//-- Max - Min doit etre positif pour toutes les directions 
	//--
	//-- Rejection 1   (FEMax-LEMin)& 0x80008000  !=0
	//--
	//--                   FE Min ...........  FE Max 
	//--                                                LE Min ....   LE Max   
	//-- 
	//-- Rejection 2   (LEMax-FEMin)& 0x80008000  !=0
	//--                            FE Min ...........  FE Max 
	//--     LE Min ....   LE Max
	//-- ----------------------------------------------------------------------	

	if(((TableauRejection *)myReject)->
	   NoIntersection(myLE,myFE) == Standard_False) { 

	  
	  if (((MinMaxFEdg->Max[0] - myLEMinMax->Min[0]) & 0x80008000) == 0 &&
	      ((myLEMinMax->Max[0] - MinMaxFEdg->Min[0]) & 0x80008000) == 0 &&
	      ((MinMaxFEdg->Max[1] - myLEMinMax->Min[1]) & 0x80008000) == 0 &&
	      ((myLEMinMax->Max[1] - MinMaxFEdg->Min[1]) & 0x80008000) == 0 &&
	      ((MinMaxFEdg->Max[2] - myLEMinMax->Min[2]) & 0x80008000) == 0 &&
	      ((myLEMinMax->Max[2] - MinMaxFEdg->Min[2]) & 0x80008000) == 0 &&
	      ((MinMaxFEdg->Max[3] - myLEMinMax->Min[3]) & 0x80008000) == 0 &&
	      ((myLEMinMax->Max[3] - MinMaxFEdg->Min[3]) & 0x80008000) == 0 &&
	      ((MinMaxFEdg->Max[4] - myLEMinMax->Min[4]) & 0x80008000) == 0 &&
	      ((myLEMinMax->Max[4] - MinMaxFEdg->Min[4]) & 0x80008000) == 0 &&
	      ((MinMaxFEdg->Max[5] - myLEMinMax->Min[5]) & 0x80008000) == 0 &&
	      ((myLEMinMax->Max[5] - MinMaxFEdg->Min[5]) & 0x80008000) == 0 &&
	      ((MinMaxFEdg->Max[6] - myLEMinMax->Min[6]) & 0x80008000) == 0 && 
	      ((myLEMinMax->Max[6] - MinMaxFEdg->Min[6]) & 0x80008000) == 0 && 
	      ((MinMaxFEdg->Max[7] - myLEMinMax->Min[7]) & 0x80008000) == 0) {   //-- Rejection en Z
	    // not rejected perform intersection
	    Standard_Boolean rej = Standard_False;
	    if (myLE == myFE) { // test if an auto-intersection is not usefull
	      if (((HLRBRep_EdgeData*)myLEData)->AutoIntersectionDone()) {
		((HLRBRep_EdgeData*)myLEData)->
		  AutoIntersectionDone(Standard_True);
		if (((HLRBRep_EdgeData*)myLEData)->Simple()) {
		  rej = Standard_True;
		}
	      }
	    }
	    if (!rej) {
	      nbCal1Intersection++;
	      Standard_Boolean h1 = Standard_False;
	      Standard_Boolean e1 = Standard_False;
	      Standard_Boolean h2 = Standard_False;
	      Standard_Boolean e2 = Standard_False;
	      mySameVertex = Standard_False;
	      
	      if (myLE == myFE) {
		myIntersected = Standard_True;
		mySameVertex  = Standard_False;
	      }
	      else {
		myIntersected = Standard_True;
		if (SameVertex(Standard_True ,Standard_True )) {
		  mySameVertex = Standard_True;
		  h1           = Standard_True;
		  h2           = Standard_True;
		}
		if (SameVertex(Standard_True ,Standard_False)) {
		  mySameVertex = Standard_True;
		  h1           = Standard_True;
		  e2           = Standard_True;
		}
		if (SameVertex(Standard_False,Standard_True )) {
		  mySameVertex = Standard_True;
		  e1           = Standard_True;
		  h2           = Standard_True;
		}
		if (SameVertex(Standard_False,Standard_False)) {
		  mySameVertex = Standard_True;
		  e1           = Standard_True;
		  e2           = Standard_True;
		}
	      }
	      
	      myNbPoints = myNbSegments = 0;
	      iInterf = 1;
	      
	      if (myIntersected) {           // compute real intersection
		nbCal2Intersection++;
		
		Standard_Real da1 = 0;
		Standard_Real db1 = 0;
		Standard_Real da2 = 0;
		Standard_Real db2 = 0;
		
		if (mySameVertex || myLE == myFE) {
		  if (h1) da1 = CutLar;
		  if (e1) db1 = CutLar;
		  if (h2) da2 = CutLar;
		  if (e2) db2 = CutLar;
		}
		Standard_Integer NoInter=0;
		if (myLE == myFE) {
		  myIntersector.Perform(myLEData,da1,db1);
		}
		else {
		  Standard_Real su,sv;
		  ((TableauRejection *)myReject)->
		    GetSingleIntersection(myLE,myFE,su,sv);
		  if(su!=RealLast()) { 
		    myIntersector.SimulateOnePoint(myLEData,su,myFEData,sv);
		    //-- cout<<"p";
		  }
		  else { 
		    myIntersector.Perform
		      (myLE,myLEData,da1,db1,
		       myFE,myFEData,da2,db2,mySameVertex);
		    if(myIntersector.IsDone()) { 
		      if(myIntersector.NbPoints() == 1 &&
			 myIntersector.NbSegments()==0) { 
			  ((TableauRejection *)myReject)->
			    SetIntersection(myLE,myFE,myIntersector.Point(1));
		      }
		    }
		  }
		  NoInter=0;
		}	  
		if(NoInter) { 
		  myNbPoints = myNbSegments = 0;
		}
		else { 
		  if (myIntersector.IsDone()) {
		    myNbPoints   = myIntersector.NbPoints();
		    myNbSegments = myIntersector.NbSegments();
		    if ((myNbSegments + myNbPoints) > 0) { 
		      nbOkIntersection++;
		    }
		    else { 
		      ((TableauRejection *)myReject)->
			SetNoIntersection(myLE,myFE);
		    }
		  }
		  else {
		    myNbPoints = myNbSegments = 0;
#ifdef OCCT_DEBUG
		    cout << "HLRBRep_Data::NextInterference : "; 
		    if (myLE == myFE) 
		      cout << "Edge " << myLE 
			<< " : Intersection not done" << endl;
		    else
		      cout << "Edges " << myLE << " , " << myFE
			<< " : Intersection not done" << endl;
#endif
		  }
		}
	      }
	      nbPtIntersection  += myNbPoints;
	      nbSegIntersection += myNbSegments;
	    }
	  }
	  else { 
#if 0
	    printf("\n Rejection myFE:%5d   myLE:%5d\n",myFE,myLE);
#endif
	  }
	}
	else { 
	  //-- cout<<"+";
	}
      }
    }
    // next edge in face
    myFaceItr1.NextEdge();
  }
}

//=======================================================================
//function : RejectedInterference
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_Data::RejectedInterference ()
{
  if (iInterf <= myNbPoints) {
    return RejectedPoint(myIntersector.Point(iInterf),
			 TopAbs_EXTERNAL,0);
  }
  else {
    Standard_Integer n = iInterf - myNbPoints;
    Standard_Boolean firstPoint = (n & 1) != 0;
    Standard_Integer nseg=n>>1;
    if (firstPoint)
      nseg++;
    Standard_Real pf = ((HLRBRep_Curve*)myLEGeom)->Parameter3d
      (myIntersector.Segment(nseg).FirstPoint().ParamOnFirst());
    Standard_Real pl = ((HLRBRep_Curve*)myLEGeom)->Parameter3d
      (myIntersector.Segment(nseg).LastPoint ().ParamOnFirst());
    if (pf > pl)
      firstPoint = !firstPoint;

    if (firstPoint) { 
      Standard_Boolean ret1 = RejectedPoint
	(myIntersector.Segment(nseg).FirstPoint(),TopAbs_FORWARD,nseg);      
      return(ret1);
    }
    else { 
      Standard_Boolean ret2 = RejectedPoint
	(myIntersector.Segment(nseg).LastPoint (),TopAbs_REVERSED,-nseg);
      return(ret2);
    }
  }
}

//=======================================================================
//function : AboveInterference
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_Data::AboveInterference ()
{ return myAboveIntf; }

//=======================================================================
//function : LocalLEGeometry2D
//purpose  : 
//=======================================================================

void HLRBRep_Data::LocalLEGeometry2D (const Standard_Real Param,
				      gp_Dir2d& Tg,
				      gp_Dir2d& Nm,
				      Standard_Real& Cu)
{
  myLLProps.SetParameter(Param);
  if (!myLLProps.IsTangentDefined())
    throw Standard_Failure("HLRBRep_Data::LocalGeometry2D");
  myLLProps.Tangent(Tg);
  Cu = myLLProps.Curvature();
  if (Cu > Epsilon(1.) && !Precision::IsInfinite(Cu)) myLLProps.Normal(Nm);
  else Nm = gp_Dir2d(-Tg.Y(),Tg.X());
}

//=======================================================================
//function : LocalFEGeometry2D
//purpose  : 
//=======================================================================

void HLRBRep_Data::LocalFEGeometry2D (const Standard_Integer FE,
				      const Standard_Real Param,
				      gp_Dir2d& Tg,
				      gp_Dir2d& Nm,
				      Standard_Real& Cu)
{
  const HLRBRep_Curve* aCurve = &myEData(FE).ChangeGeometry();
  myFLProps.SetCurve(aCurve);
  myFLProps.SetParameter(Param);
  if (!myFLProps.IsTangentDefined())
    throw Standard_Failure("HLRBRep_Data::LocalGeometry2D");
  myFLProps.Tangent(Tg);
  Cu = myFLProps.Curvature();
  if (Cu > Epsilon(1.) && !Precision::IsInfinite(Cu)) myFLProps.Normal(Nm);
  else Nm = gp_Dir2d(-Tg.Y(),Tg.X());
}

//=======================================================================
//function : EdgeState
//purpose  : 
//=======================================================================

void HLRBRep_Data::EdgeState (const Standard_Real p1,
                              const Standard_Real p2,
                              TopAbs_State& stbef,
                              TopAbs_State& staft)
{
  // compute the state of The Edge near the Intersection
  // this method should give the states before and after
  // it should get the parameters on the surface

  Standard_Real pu,pv;
  if (HLRBRep_EdgeFaceTool::UVPoint(p2,myFEGeom,iFaceGeom,pu,pv))
  {
    mySLProps.SetParameters(pu,pv);
    if (mySLProps.IsNormalDefined())
    {
      gp_Dir NrmFace  = mySLProps.Normal();

      gp_Pnt Pbid;
      gp_Vec TngEdge;
      ((HLRBRep_Curve*)myLEGeom)->D1(p1,Pbid,TngEdge);

      const gp_Trsf& TI = myProj.InvertedTransformation();
      gp_Dir V;
      if (myProj.Perspective()) {
        gp_Pnt2d P2d;
        myProj.Project(Pbid,P2d);
        V = gp_Dir(P2d.X(),P2d.Y(),-myProj.Focus());
      }
      else {
        V = gp_Dir(0,0,-1);
      }
      V.Transform(TI);
      if (NrmFace.Dot(V) > 0.)
        NrmFace.Reverse();
    
      const Standard_Real scal = (TngEdge.SquareMagnitude()>1.e-10)? NrmFace.Dot(gp_Dir(TngEdge)) : 0.;

      if      (scal >  myToler*10) {stbef = TopAbs_IN ;staft = TopAbs_OUT;}
      else if (scal < -myToler*10) {stbef = TopAbs_OUT;staft = TopAbs_IN ;}
      else                         {stbef = TopAbs_ON ;staft = TopAbs_ON ;}
    }
    else {
      stbef = TopAbs_OUT;
      staft = TopAbs_OUT;
#ifdef OCCT_DEBUG
    cout << "HLRBRep_Data::EdgeState : undefined" << endl;
#endif
    }
  }
  else {
    stbef = TopAbs_OUT;
    staft = TopAbs_OUT; 
#ifdef OCCT_DEBUG
    cout << "HLRBRep_Data::EdgeState : undefined" << endl;
#endif
  }
}

//=======================================================================
//function : HidingStartLevel
//purpose  : 
//=======================================================================

Standard_Integer 
HLRBRep_Data::HidingStartLevel (const Standard_Integer E,
				const HLRBRep_EdgeData& ED,
				const HLRAlgo_InterferenceList& IL)
{
  Standard_Boolean Loop;
  HLRAlgo_ListIteratorOfInterferenceList It;
  const HLRBRep_Curve& EC = ED.Geometry();
  Standard_Real sta = EC.Parameter3d(EC.FirstParameter());
  Standard_Real end = EC.Parameter3d(EC.LastParameter());
  Standard_Real tolpar = (end - sta) * 0.01;
  Standard_Real param;
  Loop = Standard_True;
  It.Initialize(IL);
  
  while(It.More() && Loop) {
    param = It.Value().Intersection().Parameter();
    if (param > end)
      Loop = Standard_False;
    else {
      if (Abs(param-sta) > Abs(param-end))
        end = param;
      else
        sta = param;
    }
    It.Next();
  }
  param = 0.5 * (sta + end);
  Standard_Integer level = 0;
  /*TopAbs_State st = */Classify(E,ED,Standard_True,level,param);
  Loop = Standard_True;
  It.Initialize(IL);

  while(It.More() && Loop) {
    HLRAlgo_Interference& Int = It.Value();
    Standard_Real p = Int.Intersection().Parameter();
    if (p < param - tolpar) {
      switch (Int.Transition()) {
	
      case TopAbs_FORWARD  :
        level -= Int.Intersection().Level();
        break;
      case TopAbs_REVERSED :
        level += Int.Intersection().Level();
        break;
      case TopAbs_EXTERNAL :
      case TopAbs_INTERNAL :
	  default :
	    break;
      }
    }
    else if (p > param + tolpar)
      Loop = Standard_False;
    else {
#ifdef OCCT_DEBUG
      cout << "HLRBRep_Data::HidingStartLevel : ";
      cout << "Bad Parameter." << endl;
#endif
    }
    It.Next();
  }
  return level;
}

//=======================================================================
//function : Compare
//purpose  : 
//=======================================================================

TopAbs_State HLRBRep_Data::Compare (const Standard_Integer E,
                                    const HLRBRep_EdgeData& ED)
{
  Standard_Integer level = 0;
  Standard_Real parbid = 0.;
  return Classify(E,ED,Standard_False,level,parbid);
}

//=======================================================================
//function : OrientOutLine
//purpose  : 
//=======================================================================


Standard_Boolean HLRBRep_Data::OrientOutLine (const Standard_Integer I, HLRBRep_FaceData& FD)
{
  (void)I; // avoid compiler warning

  const Handle(HLRAlgo_WiresBlock)& wb = FD.Wires();
  Standard_Integer nw = wb->NbWires();
  Standard_Integer iw1,ie1,ne1;
  const gp_Trsf& T  = myProj.Transformation();
  const gp_Trsf& TI = myProj.InvertedTransformation();
  Standard_Boolean inverted       = Standard_False;
  Standard_Boolean FirstInversion = Standard_True;
  
  for (iw1 = 1; iw1 <= nw; iw1++) {
    const Handle(HLRAlgo_EdgesBlock)& eb1 = wb->Wire(iw1);
    ne1 = eb1->NbEdges();
    
    for (ie1 = 1; ie1 <= ne1; ie1++) {
      myFE = eb1->Edge(ie1);
      HLRBRep_EdgeData& ed1 = myEData(myFE);
      if (eb1->Double (ie1) ||
	  eb1->IsoLine(ie1) ||
          ed1.Vertical()) ed1.Used(Standard_True );
      else                 ed1.Used(Standard_False);
      if ((eb1->OutLine(ie1) || eb1->Internal(ie1)) &&
	  !ed1.Vertical()) {
	Standard_Real p,pu,pv,r;
	myFEGeom = &(ed1.ChangeGeometry());
	const HLRBRep_Curve& EC = ed1.Geometry();
	Standard_Integer vsta = ed1.VSta();
	Standard_Integer vend = ed1.VEnd();
	if      (vsta == 0 &&
		 vend == 0) p = 0;
	else if (vsta == 0) p = EC.Parameter3d(EC.LastParameter ());
	else if (vend == 0) p = EC.Parameter3d(EC.FirstParameter());
	else                p = EC.Parameter3d((EC.LastParameter () +
						EC.FirstParameter()) / 2);
	if (HLRBRep_EdgeFaceTool::UVPoint(p,myFEGeom,iFaceGeom,pu,pv)) {
	  gp_Pnt Pt;
	  gp_Vec Tg;
	  mySLProps.SetParameters(pu,pv);
	  EC.D1(p,Pt,Tg);
	  gp_Dir V;
	  if (myProj.Perspective()) {
	    gp_Pnt2d P2d;
	    myProj.Project(Pt,P2d);
	    V = gp_Dir(P2d.X(),P2d.Y(),-myProj.Focus());
	  }
	  else {
	    V = gp_Dir(0,0,-1);
	  }
	  V.Transform(TI);
	  Standard_Real curv = HLRBRep_EdgeFaceTool::CurvatureValue
	    (iFaceGeom,pu,pv,V);
	  gp_Vec Nm = mySLProps.Normal();
	  if (curv == 0) {
#ifdef OCCT_DEBUG
	    cout << "HLRBRep_Data::OrientOutLine " << I;
	    cout << " Edge " << myFE << " : ";
	    cout << "CurvatureValue == 0." << endl;
#endif
	  }
	  if (curv > 0)
	    Nm.Reverse();
	  Tg.Transform(T);
	  Pt.Transform(T);
	  Nm.Transform(T);
	  Nm.Cross(Tg);
	  if (Tg.Magnitude() < gp::Resolution()) {
#ifdef OCCT_DEBUG
	    cout << "HLRBRep_Data::OrientOutLine " << I;
	    cout << " Edge " << myFE << " : ";
	    cout << "Tg.Magnitude() == 0." << endl;
#endif  
	  }
	  if (myProj.Perspective())
	    r = Nm.Z() * myProj.Focus() - 
	      ( Nm.X() * Pt.X() + Nm.Y() * Pt.Y() + Nm.Z() * Pt.Z() );
	  else
	    r = Nm.Z();
	  myFEOri = (r > 0) ? TopAbs_FORWARD : TopAbs_REVERSED;
	  if (!FD.Cut() && FD.Closed() && FirstInversion) {
	    if ((eb1->Orientation(ie1) == myFEOri) != 
		(FD.Orientation() == TopAbs_FORWARD)) {
	      FirstInversion = Standard_False;
	      inverted = Standard_True;
	    }
	  }
	  eb1->Orientation(ie1,myFEOri);
	}
	else {
#ifdef OCCT_DEBUG
	  cout << "HLRBRep_Data::OrientOutLine " << I;
	  cout << " Edge " << myFE << " : ";
	  cout << "UVPoint not found, OutLine not Oriented" << endl;
#endif
	}
	ed1.Used(Standard_True);
      }
    }
  }
  return inverted;
}

//=======================================================================
//function : OrientOthEdge
//purpose  : 
//=======================================================================

void HLRBRep_Data::OrientOthEdge (const Standard_Integer I,
				  HLRBRep_FaceData& FD)
{
  Standard_Real p,pu,pv,r;
  const Handle(HLRAlgo_WiresBlock)& wb = FD.Wires();
  Standard_Integer nw = wb->NbWires();
  Standard_Integer iw1,ie1,ne1;
  const gp_Trsf& T = myProj.Transformation();
  
  for (iw1 = 1; iw1 <= nw; iw1++) {
    const Handle(HLRAlgo_EdgesBlock)& eb1 = wb->Wire(iw1);
    ne1 = eb1->NbEdges();
    
    for (ie1 = 1; ie1 <= ne1; ie1++) {
      myFE    = eb1->Edge       (ie1);
      myFEOri = eb1->Orientation(ie1);
      HLRBRep_EdgeData& ed1 = myEData(myFE);
      
      if (!ed1.Used()) {
	ed1.Used(Standard_True);
	myFEGeom = &(ed1.ChangeGeometry());
	const HLRBRep_Curve& EC = ed1.Geometry();
	p = EC.Parameter3d((EC.LastParameter () +
			    EC.FirstParameter()) / 2);
	if (HLRBRep_EdgeFaceTool::UVPoint(p,myFEGeom,iFaceGeom,pu,pv)) {
	  gp_Pnt Pt = EC.Value3D(p);
	  mySLProps.SetParameters(pu,pv);
	  gp_Vec Nm = mySLProps.Normal();
	  Pt.Transform(T);
	  Nm.Transform(T);
	  if (myProj.Perspective()) {
	    r = Nm.Z() * myProj.Focus() - 
	      ( Nm.X() * Pt.X() + Nm.Y() * Pt.Y() + Nm.Z() * Pt.Z() );
	  }
	  else {
	    r = Nm.Z();
	  }
	  if (r < 0) {
	    myFEOri = TopAbs::Reverse(myFEOri);
	    eb1->Orientation(ie1,myFEOri);
	  }
	}
#ifdef OCCT_DEBUG
	else {
	  cout << "HLRBRep_Data::OrientOthEdge " << I;
	  cout << " Edge " << myFE << " : ";
	  cout << "UVPoint not found, Edge not Oriented" << endl;
	}
#else
        (void)I; // avoid compiler warning
#endif
      }
    }
  }
}

//=======================================================================
//function : Classify
//purpose  : 
//=======================================================================
namespace
{

static void REJECT1(
  const Standard_Real theDeca[],
  const Standard_Real theTotMin[],
  const Standard_Real theTotMax[],
  const Standard_Real theSurD[],
  HLRAlgo_EdgesBlock::MinMaxIndices& theVertMin,
  HLRAlgo_EdgesBlock::MinMaxIndices& theVertMax)
{
  theVertMin.Min[0] = (Standard_Integer)((theDeca[ 0]+theTotMin[ 0]) * theSurD[ 0]);
  theVertMax.Min[0] = (Standard_Integer)((theDeca[ 0]+theTotMax[ 0]) * theSurD[ 0]);
  theVertMin.Min[1] = (Standard_Integer)((theDeca[ 1]+theTotMin[ 1]) * theSurD[ 1]);
  theVertMax.Min[1] = (Standard_Integer)((theDeca[ 1]+theTotMax[ 1]) * theSurD[ 1]);
  theVertMin.Min[2] = (Standard_Integer)((theDeca[ 2]+theTotMin[ 2]) * theSurD[ 2]);
  theVertMax.Min[2] = (Standard_Integer)((theDeca[ 2]+theTotMax[ 2]) * theSurD[ 2]);
  theVertMin.Min[3] = (Standard_Integer)((theDeca[ 3]+theTotMin[ 3]) * theSurD[ 3]);
  theVertMax.Min[3] = (Standard_Integer)((theDeca[ 3]+theTotMax[ 3]) * theSurD[ 3]);
  theVertMin.Min[4] = (Standard_Integer)((theDeca[ 4]+theTotMin[ 4]) * theSurD[ 4]);
  theVertMax.Min[4] = (Standard_Integer)((theDeca[ 4]+theTotMax[ 4]) * theSurD[ 4]);
  theVertMin.Min[5] = (Standard_Integer)((theDeca[ 5]+theTotMin[ 5]) * theSurD[ 5]);
  theVertMax.Min[5] = (Standard_Integer)((theDeca[ 5]+theTotMax[ 5]) * theSurD[ 5]);
  theVertMin.Min[6] = (Standard_Integer)((theDeca[ 6]+theTotMin[ 6]) * theSurD[ 6]);
  theVertMax.Min[6] = (Standard_Integer)((theDeca[ 6]+theTotMax[ 6]) * theSurD[ 6]);
  theVertMin.Min[7] = (Standard_Integer)((theDeca[ 7]+theTotMin[ 7]) * theSurD[ 7]);
  theVertMax.Min[7] = (Standard_Integer)((theDeca[ 7]+theTotMax[ 7]) * theSurD[ 7]);
  theVertMin.Max[0] = (Standard_Integer)((theDeca[ 8]+theTotMin[ 8]) * theSurD[ 8]);
  theVertMax.Max[0] = (Standard_Integer)((theDeca[ 8]+theTotMax[ 8]) * theSurD[ 8]);
  theVertMin.Max[1] = (Standard_Integer)((theDeca[ 9]+theTotMin[ 9]) * theSurD[ 9]);
  theVertMax.Max[1] = (Standard_Integer)((theDeca[ 9]+theTotMax[ 9]) * theSurD[ 9]);
  theVertMin.Max[2] = (Standard_Integer)((theDeca[10]+theTotMin[10]) * theSurD[10]);
  theVertMax.Max[2] = (Standard_Integer)((theDeca[10]+theTotMax[10]) * theSurD[10]);
  theVertMin.Max[3] = (Standard_Integer)((theDeca[11]+theTotMin[11]) * theSurD[11]);
  theVertMax.Max[3] = (Standard_Integer)((theDeca[11]+theTotMax[11]) * theSurD[11]);
  theVertMin.Max[4] = (Standard_Integer)((theDeca[12]+theTotMin[12]) * theSurD[12]);
  theVertMax.Max[4] = (Standard_Integer)((theDeca[12]+theTotMax[12]) * theSurD[12]);
  theVertMin.Max[5] = (Standard_Integer)((theDeca[13]+theTotMin[13]) * theSurD[13]);
  theVertMax.Max[5] = (Standard_Integer)((theDeca[13]+theTotMax[13]) * theSurD[13]);
  theVertMin.Max[6] = (Standard_Integer)((theDeca[14]+theTotMin[14]) * theSurD[14]);
  theVertMax.Max[6] = (Standard_Integer)((theDeca[14]+theTotMax[14]) * theSurD[14]);
  theVertMin.Max[7] = (Standard_Integer)((theDeca[15]+theTotMin[15]) * theSurD[15]);
  theVertMax.Max[7] = (Standard_Integer)((theDeca[15]+theTotMax[15]) * theSurD[15]);
}

}

TopAbs_State 
HLRBRep_Data::Classify (const Standard_Integer E,
			const HLRBRep_EdgeData& ED,
			const Standard_Boolean LevelFlag,
			Standard_Integer& Level,
			const Standard_Real param)
{
  (void)E; // avoid compiler warning

  nbClassification++;
  HLRAlgo_EdgesBlock::MinMaxIndices VertMin, VertMax, MinMaxVert;
  Standard_Real TotMin[16],TotMax[16];
  
  Standard_Integer i;
  Level = 0;
  TopAbs_State state = TopAbs_OUT;
//  Standard_Boolean rej = Standard_False;
  const HLRBRep_Curve& EC = ED.Geometry();
  Standard_Real sta,xsta,ysta,zsta,end,xend,yend,zend;
  Standard_Real tol = (Standard_Real)(ED.Tolerance());
  
  if (LevelFlag) {
    sta = param;
    myProj.Project(EC.Value3D(sta),xsta,ysta,zsta);
    
    //-- les rejections sont faites dans l intersecteur a moindre frais 
    //-- puisque la surface sera chargee
    HLRAlgo::InitMinMax(Precision::Infinite(), TotMin, TotMax);
    HLRAlgo::UpdateMinMax(xsta,ysta,zsta, TotMin, TotMax);
    HLRAlgo::EnlargeMinMax(tol, TotMin, TotMax);
    REJECT1(myDeca, TotMin, TotMax, mySurD, VertMin, VertMax);

    HLRAlgo::EncodeMinMax(VertMin, VertMax, MinMaxVert);
    if (((iFaceMinMax->Max[0] - MinMaxVert.Min[0]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[0] - iFaceMinMax->Min[0]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[1] - MinMaxVert.Min[1]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[1] - iFaceMinMax->Min[1]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[2] - MinMaxVert.Min[2]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[2] - iFaceMinMax->Min[2]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[3] - MinMaxVert.Min[3]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[3] - iFaceMinMax->Min[3]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[4] - MinMaxVert.Min[4]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[4] - iFaceMinMax->Min[4]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[5] - MinMaxVert.Min[5]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[5] - iFaceMinMax->Min[5]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[6] - MinMaxVert.Min[6]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[6] - iFaceMinMax->Min[6]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[7] - MinMaxVert.Min[7]) & 0x80008000) != 0) { //-- Rejection en Z 
      return state;
    }
  }
  else {
    sta  = EC.Parameter3d(EC.FirstParameter());
    myProj.Project(EC.Value3D(sta),xsta,ysta,zsta);
    
    //-- les rejections sont faites dans l intersecteur a moindre frais 
    //-- puisque la surface sera chargee
    HLRAlgo::InitMinMax(Precision::Infinite(), TotMin, TotMax);
    HLRAlgo::UpdateMinMax(xsta,ysta,zsta, TotMin, TotMax);
    HLRAlgo::EnlargeMinMax(tol, TotMin, TotMax);

    REJECT1(myDeca, TotMin, TotMax, mySurD, VertMin, VertMax);

    HLRAlgo::EncodeMinMax(VertMin, VertMax, MinMaxVert);
    if (((iFaceMinMax->Max[0] - MinMaxVert.Min[0]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[0] - iFaceMinMax->Min[0]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[1] - MinMaxVert.Min[1]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[1] - iFaceMinMax->Min[1]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[2] - MinMaxVert.Min[2]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[2] - iFaceMinMax->Min[2]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[3] - MinMaxVert.Min[3]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[3] - iFaceMinMax->Min[3]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[4] - MinMaxVert.Min[4]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[4] - iFaceMinMax->Min[4]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[5] - MinMaxVert.Min[5]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[5] - iFaceMinMax->Min[5]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[6] - MinMaxVert.Min[6]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[6] - iFaceMinMax->Min[6]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[7] - MinMaxVert.Min[7]) & 0x80008000) != 0) { //-- Rejection en Z 
      return state;
    }
    end = EC.Parameter3d(EC.LastParameter());
    myProj.Project(EC.Value3D(end),xend,yend,zend);
    
    HLRAlgo::InitMinMax(Precision::Infinite(), TotMin, TotMax);
    HLRAlgo::UpdateMinMax(xend,yend,zend, TotMin, TotMax);
    HLRAlgo::EnlargeMinMax(tol, TotMin, TotMax);

    REJECT1(myDeca, TotMin, TotMax, mySurD, VertMin, VertMax);

    HLRAlgo::EncodeMinMax(VertMin, VertMax, MinMaxVert);
    if (((iFaceMinMax->Max[0] - MinMaxVert.Min[0]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[0] - iFaceMinMax->Min[0]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[1] - MinMaxVert.Min[1]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[1] - iFaceMinMax->Min[1]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[2] - MinMaxVert.Min[2]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[2] - iFaceMinMax->Min[2]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[3] - MinMaxVert.Min[3]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[3] - iFaceMinMax->Min[3]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[4] - MinMaxVert.Min[4]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[4] - iFaceMinMax->Min[4]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[5] - MinMaxVert.Min[5]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[5] - iFaceMinMax->Min[5]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[6] - MinMaxVert.Min[6]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[6] - iFaceMinMax->Min[6]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[7] - MinMaxVert.Min[7]) & 0x80008000) != 0) { //-- Rejection en Z 
      return state;
    }
    sta = 0.4 * sta + 0.6 * end; // dangerous if it is the middle
    myProj.Project(EC.Value3D(sta),xsta,ysta,zsta);
    
    //-- les rejections sont faites dans l intersecteur a moindre frais 
    //-- puisque la surface sera chargee
    HLRAlgo::InitMinMax(Precision::Infinite(), TotMin, TotMax);
    HLRAlgo::UpdateMinMax(xsta,ysta,zsta, TotMin, TotMax);
    HLRAlgo::EnlargeMinMax(tol, TotMin, TotMax);
    REJECT1(myDeca, TotMin, TotMax, mySurD, VertMin, VertMax);

    HLRAlgo::EncodeMinMax(VertMin, VertMax, MinMaxVert);
    /*
#ifdef OCCT_DEBUG
	{
	  Standard_Integer qwe,qwep8,q,q1,q2;
	  printf("\n E:%d -------\n",E);
	  for(qwe=0; qwe<8; qwe++) {
	    q1 = (((Standard_Integer*)iFaceMinMax)[qwe   ]) & 0x0000FFFF;
	    q2 = (((Standard_Integer*)iFaceMinMax)[qwe+8]) & 0x0000FFFF; 
	    printf("\nFace: %3d    %6d  ->  %6d    delta : %6d ",qwe,q1,q2,q2-q1);
	    
	    q1 = (((Standard_Integer*)MinMaxVert)[qwe   ]) & 0x0000FFFF;
	    q2 = (((Standard_Integer*)MinMaxVert)[qwe+8]) & 0x0000FFFF; 
	    printf("  |  Vtx: %3d    %6d  ->  %6d    delta : %6d ",qwe,q1,q2,q2-q1);
	    
	    q1 = ((((Standard_Integer*)iFaceMinMax)[qwe  ])>>16) & 0x0000FFFF;
	    q2 = ((((Standard_Integer*)iFaceMinMax)[qwe+8])>>16) & 0x0000FFFF; 
	    printf("\nFace: %3d    %6d  ->  %6d    delta : %6d ",qwe,q1,q2,q2-q1);
	    
	    q1 = ((((Standard_Integer*)MinMaxVert)[qwe  ])>>16) & 0x0000FFFF;
	    q2 = ((((Standard_Integer*)MinMaxVert)[qwe+8])>>16) & 0x0000FFFF; 
	    printf("  |  Vtx: %3d    %6d  ->  %6d    delta : %6d ",qwe,q1,q2,q2-q1);
	  }
	  printf("\n");


	  for(qwe=0,qwep8=8; qwe<8; qwe++,qwep8++) { 
	    q = ((Standard_Integer*)iFaceMinMax)[qwep8]- ((Standard_Integer*)MinMaxVert)[qwe];
	    q1 = q>>16;
	    q2 = (q& 0x0000FFFF);
	    printf("\nmot: %3d    q1 = %+10d    q2=%+10d    Mask : %d",qwe,(q1>32768)? (32768-q1) : q1,(q2>32768)? (32768-q2) : q2,q&0x80008000);
	  }
	  for(qwe=0,qwep8=8; qwe<8; qwe++,qwep8++) { 
	    q = ((Standard_Integer*)MinMaxVert)[qwep8]- ((Standard_Integer*)iFaceMinMax)[qwe];
	    q1 = q>>16;
	    q2 = (q& 0x0000FFFF);
	    printf("\nmot: %3d    q1 = %+10d    q2=%+10d    Mask : %d",qwe+8,(q1>32768)? (32768-q1) : q1,(q2>32768)? (32768-q2) : q2,q&0x80008000);
	  }
	  cout<<endl;
	}
 #endif
    */

    if (((iFaceMinMax->Max[0] - MinMaxVert.Min[0]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[0] - iFaceMinMax->Min[0]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[1] - MinMaxVert.Min[1]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[1] - iFaceMinMax->Min[1]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[2] - MinMaxVert.Min[2]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[2] - iFaceMinMax->Min[2]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[3] - MinMaxVert.Min[3]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[3] - iFaceMinMax->Min[3]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[4] - MinMaxVert.Min[4]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[4] - iFaceMinMax->Min[4]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[5] - MinMaxVert.Min[5]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[5] - iFaceMinMax->Min[5]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[6] - MinMaxVert.Min[6]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[6] - iFaceMinMax->Min[6]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[7] - MinMaxVert.Min[7]) & 0x80008000) != 0) { //-- Rejection en Z 
      return state;
    }
  }

  nbCal3Intersection++;
  gp_Pnt   PLim;
  gp_Pnt2d Psta;
  Psta = EC.Value  (sta);
  PLim = EC.Value3D(sta);
  

  static int aff=0;
  if(aff) {
    static Standard_Integer nump1=0;
    printf("\npoint PNR%d  %g %g %g",++nump1,PLim.X(),PLim.Y(),PLim.Z());
  }
  
  gp_Lin L = myProj.Shoot(Psta.X(),Psta.Y());
  Standard_Real wLim = ElCLib::Parameter(L,PLim);
  myIntersector.Perform(L,wLim);
  if (myIntersector.IsDone()) {
    Standard_Integer nbPoints = myIntersector.NbPoints();
    if (nbPoints > 0) {
      Standard_Real TolZ = myBigSize * 0.000001;
      if (iFaceTest) {
	if (!myLEOutLine && !myLEInternal) TolZ = myBigSize * 0.001;
	else                               TolZ = myBigSize * 0.01;
      }
      wLim -= TolZ;
      Standard_Real PeriodU,PeriodV,UMin =0.,UMax =0.,VMin =0.,VMax =0.;
      if (((HLRBRep_Surface*)iFaceGeom)->IsUPeriodic()) { 
	PeriodU = ((HLRBRep_Surface*)iFaceGeom)->UPeriod();
	UMin = ((HLRBRep_Surface*)iFaceGeom)->FirstUParameter();
	UMax = ((HLRBRep_Surface*)iFaceGeom)->LastUParameter();
      }
      else 
	PeriodU = 0.;
      if (((HLRBRep_Surface*)iFaceGeom)->IsVPeriodic()) { 
	PeriodV = ((HLRBRep_Surface*)iFaceGeom)->VPeriod();
	VMin = ((HLRBRep_Surface*)iFaceGeom)->FirstVParameter();
	VMax = ((HLRBRep_Surface*)iFaceGeom)->LastVParameter();
      }
      else 
	PeriodV = 0;
      gp_Pnt PInter;
      Standard_Real u,v,w;
      IntCurveSurface_TransitionOnCurve Tr;
      
      for (i = 1; i <= nbPoints; i++) {
	myIntersector.CSPoint(i).Values(PInter,u,v,w,Tr);
	if (w < wLim) {
          Standard_Real aDummyShift;
          if (PeriodU > 0.)
            GeomInt::AdjustPeriodic(u, UMin, UMax, PeriodU, u, aDummyShift);
          if (PeriodV > 0.)
            GeomInt::AdjustPeriodic(v, VMin, VMax, PeriodV, v, aDummyShift);
	  
          gp_Pnt2d pnt2d(u, v);
          if (myClassifier->Classify(pnt2d, Precision::PConfusion())
            != TopAbs_OUT)
          {
            state = TopAbs_IN;
            Level++;
            if (!LevelFlag) {
              return state;
            }
          }
	}
      }
    }
  }
  return state;
}


//=======================================================================
//function : SimplClassify
//purpose  : 
//=======================================================================

TopAbs_State HLRBRep_Data::SimplClassify (const Standard_Integer /*E*/,
					  const HLRBRep_EdgeData& ED,
					  const Standard_Integer Nbp,
					  const Standard_Real p1,
					  const Standard_Real p2)
{
  nbClassification++;
  HLRAlgo_EdgesBlock::MinMaxIndices VertMin, VertMax, MinMaxVert;
  Standard_Real TotMin[16],TotMax[16];
  
  Standard_Integer i;
  TopAbs_State state = TopAbs_IN;
//  Standard_Boolean rej = Standard_False;
  const HLRBRep_Curve& EC = ED.Geometry();
  Standard_Real sta,xsta,ysta,zsta, dp;
  Standard_Real tol = (Standard_Real)(ED.Tolerance());

  dp = (p2 - p1)/(Nbp+1);

  for(sta = p1+dp,i = 1; i <= Nbp; ++i, sta += dp) {
    myProj.Project(EC.Value3D(sta),xsta,ysta,zsta);
    
    //-- les rejections sont faites dans l intersecteur a moindre frais 
    //-- puisque la surface sera chargee
    HLRAlgo::InitMinMax(Precision::Infinite(), TotMin, TotMax);
    HLRAlgo::UpdateMinMax(xsta,ysta,zsta, TotMin, TotMax);
    HLRAlgo::EnlargeMinMax(tol, TotMin, TotMax);
    REJECT1(myDeca, TotMin, TotMax, mySurD, VertMin, VertMax);

    HLRAlgo::EncodeMinMax(VertMin, VertMax, MinMaxVert);
    if (((iFaceMinMax->Max[0] - MinMaxVert.Min[0]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[0] - iFaceMinMax->Min[0]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[1] - MinMaxVert.Min[1]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[1] - iFaceMinMax->Min[1]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[2] - MinMaxVert.Min[2]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[2] - iFaceMinMax->Min[2]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[3] - MinMaxVert.Min[3]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[3] - iFaceMinMax->Min[3]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[4] - MinMaxVert.Min[4]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[4] - iFaceMinMax->Min[4]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[5] - MinMaxVert.Min[5]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[5] - iFaceMinMax->Min[5]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[6] - MinMaxVert.Min[6]) & 0x80008000) != 0 ||
	((MinMaxVert.Max[6] - iFaceMinMax->Min[6]) & 0x80008000) != 0 ||
	((iFaceMinMax->Max[7] - MinMaxVert.Min[7]) & 0x80008000) != 0) { //-- Rejection en Z 
      return TopAbs_OUT;
    }
  }
  return state;
}

//=======================================================================
//function : RejectedPoint
//purpose  : build an interference if non Rejected intersection point
//=======================================================================

Standard_Boolean 
HLRBRep_Data::RejectedPoint (const IntRes2d_IntersectionPoint& PInter,
			     const TopAbs_Orientation BoundOri,
			     const Standard_Integer NumSeg)
{
  Standard_Integer Ind = 0;
  Standard_Integer decal;
  Standard_Real p1,p2,dz;
  Standard_ShortReal t1,t2;
  TopAbs_State st;
  TopAbs_Orientation Orie =TopAbs_FORWARD ;
  TopAbs_Orientation Or2 = TopAbs_INTERNAL;
  Standard_Boolean inverted = Standard_False;
  const IntRes2d_Transition* Tr1;
  const IntRes2d_Transition* Tr2;
  Standard_Real TolZ = myBigSize * 0.00001;

  p1 = ((HLRBRep_Curve*)myLEGeom)->Parameter3d(PInter.ParamOnFirst ());
  p2 = ((HLRBRep_Curve*)myFEGeom)->Parameter3d(PInter.ParamOnSecond());
  dz = ((HLRBRep_Curve*)myLEGeom)->Z(p1)-((HLRBRep_Curve*)myFEGeom)->Z(p2);

  if (myLE == myFE) {            // auto intersection can be inverted
    if (dz >=  TolZ) {
      inverted = Standard_True;
      Standard_Real p = p1;
      p1 = p2;
      p2 = p;
      dz = -dz;
    }
  }

  if (dz >=  TolZ) {
    myAboveIntf = Standard_True;
    return Standard_True;
  }
  myAboveIntf = Standard_False;
  st = (dz <= -TolZ) ? TopAbs_IN : TopAbs_ON;

  if (inverted) {
    Tr1 = &(PInter.TransitionOfSecond());
    Tr2 = &(PInter.TransitionOfFirst ());
  }
  else {
    Tr1 = &(PInter.TransitionOfFirst ());
    Tr2 = &(PInter.TransitionOfSecond());
  }

  if (iFaceTest) {
    if (myLE == myFE) {
      if (st == TopAbs_IN)
	((HLRBRep_EdgeData*)myLEData)->Simple(Standard_False);
    }
    else {
      if (mySameVertex) {
	if ((st == TopAbs_ON)                           ||
	    (Tr1->PositionOnCurve() != IntRes2d_Middle) ||
	    (Tr2->PositionOnCurve() != IntRes2d_Middle))
	  return Standard_True;
      }
    }
    if (st == TopAbs_IN) iFaceSmpl = Standard_False;
  }

  switch (Tr1->TransitionType()) {                 // compute the transition
  case IntRes2d_In :
    Orie = (myFEOri == TopAbs_REVERSED ? TopAbs_REVERSED : TopAbs_FORWARD);
    break;
  case IntRes2d_Out :
    Orie = (myFEOri == TopAbs_REVERSED ? TopAbs_FORWARD : TopAbs_REVERSED);
    break;
  case IntRes2d_Touch :
    switch (Tr1->Situation()) {
    case IntRes2d_Inside :
      Orie = (myFEOri == TopAbs_REVERSED ? TopAbs_EXTERNAL : TopAbs_INTERNAL);
      break;
    case IntRes2d_Outside :
      Orie = (myFEOri == TopAbs_REVERSED ? TopAbs_INTERNAL : TopAbs_EXTERNAL);
      break;
    case IntRes2d_Unknown :
      return Standard_True;
    }
    break;
  case IntRes2d_Undecided :
    return Standard_True;
  }

  if (iFaceBack) Orie = TopAbs::Complement(Orie);  // change the transition
  TopAbs_Orientation Ori = TopAbs_FORWARD;
  switch (Tr1->PositionOnCurve()) {
  case IntRes2d_Head   : Ori = TopAbs_FORWARD ; break;
  case IntRes2d_Middle : Ori = TopAbs_INTERNAL; break;
  case IntRes2d_End    : Ori = TopAbs_REVERSED; break;
  }

  if (st != TopAbs_OUT) {
    if (Tr2->PositionOnCurve() != IntRes2d_Middle) { // correction de la transition  sur myFE
      if (mySameVertex) return Standard_True;        // si intersection a une extremite verticale !

      Standard_Boolean douteux = Standard_False;
      Standard_Real psav = p2;
      gp_Pnt2d Ptsav;
      gp_Vec2d Tgsav,Nmsav;
      if (Tr2->PositionOnCurve() == IntRes2d_Head) {
	Ind = ((HLRBRep_EdgeData*)myFEData)->VSta();
	Or2 = TopAbs_FORWARD ;
	AdjustParameter((HLRBRep_EdgeData*)myFEData,Standard_True ,p2,t2);
	if (((HLRBRep_EdgeData*)myFEData)->VerAtSta()) {
	  douteux = Standard_True;
	  ((HLRBRep_Curve*)myFEGeom)->D2(psav,Ptsav,Tgsav,Nmsav);
	  if (Tgsav.SquareMagnitude() <= DERIVEE_PREMIERE_NULLE)
	    Tgsav = Nmsav;
	}
      }
      else {
	Ind = ((HLRBRep_EdgeData*)myFEData)->VEnd();
	Or2 = TopAbs_REVERSED;
	AdjustParameter((HLRBRep_EdgeData*)myFEData,Standard_False,p2,t2);
	if (((HLRBRep_EdgeData*)myFEData)->VerAtEnd()) {
	  douteux = Standard_True;
	  ((HLRBRep_Curve*)myFEGeom)->D2(psav,Ptsav,Tgsav,Nmsav);
	  if (Tgsav.SquareMagnitude() <= DERIVEE_PREMIERE_NULLE)
	    Tgsav = Nmsav;
	}
      }
      gp_Vec2d TgFE;
      ((HLRBRep_Curve*)myFEGeom)->D1(p2,Ptsav,TgFE);
      if (douteux) {
	if (TgFE.XY().Dot(Tgsav.XY()) < 0.0) {
	  if      (Orie == TopAbs_FORWARD ) Orie = TopAbs_REVERSED;
	  else if (Orie == TopAbs_REVERSED) Orie = TopAbs_FORWARD ;
	}
      }
      myIntf.ChangeBoundary().Set2D(myFE,p2);
    }
    if (Ori != TopAbs_INTERNAL) {                 // correction de la transition  sur myLE
      Standard_Boolean douteux = Standard_False;  // si intersection a une extremite verticale !
      Standard_Real psav = p1;
      gp_Pnt2d Ptsav;
      gp_Vec2d Tgsav,Nmsav;
      if (Ori == TopAbs_FORWARD) {
	AdjustParameter((HLRBRep_EdgeData*)myLEData,Standard_True ,p1,t1);
	if (((HLRBRep_EdgeData*)myLEData)->VerAtSta()) {
	  douteux = Standard_True;
	  ((HLRBRep_Curve*)myLEGeom)->D2(psav,Ptsav,Tgsav,Nmsav);
	  if (Tgsav.SquareMagnitude() <= DERIVEE_PREMIERE_NULLE)
	    Tgsav=Nmsav;
	}
      }
      else {
	AdjustParameter((HLRBRep_EdgeData*)myLEData,Standard_False,p1,t1);
	if (((HLRBRep_EdgeData*)myLEData)->VerAtEnd()) {
	  douteux = Standard_True;
	  ((HLRBRep_Curve*)myLEGeom)->D2(psav,Ptsav,Tgsav,Nmsav);
	  if (Tgsav.SquareMagnitude() <= DERIVEE_PREMIERE_NULLE)
	    Tgsav=Nmsav;
	}
      }
      if (douteux) {
	gp_Vec2d TgLE;
	((HLRBRep_Curve*)myLEGeom)->D1(p1,Ptsav,TgLE);
	if (TgLE.XY().Dot(Tgsav.XY()) < 0.0) {
	  if      (Orie == TopAbs_FORWARD ) Orie = TopAbs_REVERSED;
	  else if (Orie == TopAbs_REVERSED) Orie = TopAbs_FORWARD ;
	}
      }
    }
    if (st == TopAbs_ON) {
      TopAbs_State stbef,staft;
      EdgeState(p1,p2,stbef,staft);
      myIntf.ChangeBoundary().SetState3D(stbef,staft);
    }
  }

  if (myFEInternal) {
    decal = 2;
  }
  else {
    decal = 1;
    if (st == TopAbs_IN &&
	Ori == TopAbs_FORWARD &&
	Orie == TopAbs_FORWARD)
      decal = 0;
  }
  HLRAlgo_Intersection& inter = myIntf.ChangeIntersection();
  inter.Orientation(Ori);
  inter.Level(decal);
  inter.SegIndex(NumSeg);
  inter.Index(Ind);
  inter.Parameter(p1);
  inter.Tolerance(myLETol);
  inter.State(st);
  myIntf.Orientation(Or2);
  myIntf.Transition(Orie);
  myIntf.BoundaryTransition(BoundOri);
  myIntf.ChangeBoundary().Set2D(myFE,p2);
  return Standard_False;
}

//=======================================================================
//function : SameVertex
//purpose  : 
//=======================================================================

Standard_Boolean 
HLRBRep_Data::SameVertex (const Standard_Boolean h1,
			  const Standard_Boolean h2)
{
  Standard_Integer v1,v2;
  if (h1) v1 = ((HLRBRep_EdgeData*)myLEData)->VSta();
  else    v1 = ((HLRBRep_EdgeData*)myLEData)->VEnd();
  if (h2) v2 = ((HLRBRep_EdgeData*)myFEData)->VSta();
  else    v2 = ((HLRBRep_EdgeData*)myFEData)->VEnd();
  Standard_Boolean SameV = v1 == v2;
  if (SameV) {
    myIntersected = Standard_True; // compute the intersections
    if ((myLEType == GeomAbs_Line    ||
	 myLEType == GeomAbs_Circle  ||
	 myLEType == GeomAbs_Ellipse ) &&
	(myFEType == GeomAbs_Line    ||
	 myFEType == GeomAbs_Circle  ||
	 myFEType == GeomAbs_Ellipse ))
      myIntersected = Standard_False;    // no other intersection

    Standard_Boolean otherCase = Standard_True;

    if (( h1 && ((HLRBRep_EdgeData*)myLEData)->OutLVSta()) ||
	(!h1 && ((HLRBRep_EdgeData*)myLEData)->OutLVEnd())) {
      if (iFaceTest || myLEInternal)
	otherCase = Standard_False;
    }
    else if (iFaceTest)
      otherCase = Standard_False;

    if (otherCase) {
      if (( h1 && ((HLRBRep_EdgeData*)myLEData)->CutAtSta()) ||
	  (!h1 && ((HLRBRep_EdgeData*)myLEData)->CutAtEnd())) {
	myIntersected = Standard_False; // two connected OutLines do not
      }                                 // intersect themselves.
    }
  }
  return SameV;
}

//=======================================================================
//function : IsBadFace
//purpose  : 
//=======================================================================

Standard_Boolean HLRBRep_Data::IsBadFace() const
{
  if (iFaceGeom)
  {
    // check for garbage data - if periodic then bounds must not exceed period
    HLRBRep_Surface *pGeom = (HLRBRep_Surface*)iFaceGeom;
    if (pGeom->IsUPeriodic())
    {
      Standard_Real aPeriod = pGeom->UPeriod();
      Standard_Real aMin = pGeom->FirstUParameter();
      Standard_Real aMax = pGeom->LastUParameter();
      if (aPeriod * 2 < aMax - aMin)
        return Standard_True;
    }
    if (pGeom->IsVPeriodic())
    {
      Standard_Real aPeriod = pGeom->VPeriod();
      Standard_Real aMin = pGeom->FirstVParameter();
      Standard_Real aMax = pGeom->LastVParameter();
      if (aPeriod * 2 < aMax - aMin)
        return Standard_True;
    }
  }
  return Standard_False;
}
