// Created on: 1993-02-03
// Created by: Laurent BUCHARD
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


#include <Adaptor3d_HSurface.hxx>
#include <Bnd_Array1OfBox.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <IntPatch_HInterTool.hxx>
#include <IntPatch_Polyhedron.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <stdio.h>
#define MSG_DEBUG                   0

#define LONGUEUR_MINI_EDGE_TRIANGLE 1e-14
#define DEFLECTION_COEFF            1.1
#define NBMAXUV                     30

//================================================================================
static Standard_Integer NbPOnU (const Handle(Adaptor3d_HSurface)& S)
{
  const Standard_Real u0 = S->FirstUParameter();
  const Standard_Real u1 = S->LastUParameter();
  const Standard_Integer nbpu = IntPatch_HInterTool::NbSamplesU(S,u0,u1);
  return (nbpu>NBMAXUV? NBMAXUV : nbpu);
}
//================================================================================
static Standard_Integer NbPOnV (const Handle(Adaptor3d_HSurface)& S)
{
  const Standard_Real v0 = S->FirstVParameter();
  const Standard_Real v1 = S->LastVParameter();
  const Standard_Integer nbpv = IntPatch_HInterTool::NbSamplesV(S,v0,v1);
  return (nbpv>NBMAXUV? NBMAXUV : nbpv);
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void IntPatch_Polyhedron::Destroy()
{
  gp_Pnt *CMyPnts     = (gp_Pnt *)C_MyPnts;       if(C_MyPnts) delete [] CMyPnts;
  Standard_Real *CMyU = (Standard_Real *)C_MyU;   if(C_MyU)    delete [] CMyU;
  Standard_Real *CMyV = (Standard_Real *)C_MyV;   if(C_MyV)    delete [] CMyV;
  C_MyPnts=C_MyU=C_MyV=NULL;
}

//=======================================================================
//function : IntPatch_Polyhedron
//purpose  : 
//=======================================================================
IntPatch_Polyhedron::IntPatch_Polyhedron (const Handle(Adaptor3d_HSurface)& Surface)
     : TheDeflection(Epsilon(100.)),
       nbdeltaU(NbPOnU(Surface)),
       nbdeltaV(NbPOnV(Surface)),
       C_MyPnts(NULL),C_MyU(NULL),C_MyV(NULL),
       UMinSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
       UMaxSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
       VMinSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
       VMaxSingular(IntPatch_HInterTool::SingularOnVMin(Surface))
{ 
  const Standard_Integer t = (nbdeltaU+1)*(nbdeltaV+1)+1;
  gp_Pnt *CMyPnts     = new gp_Pnt[t];
  Standard_Real *CMyU = new Standard_Real[t];
  Standard_Real *CMyV = new Standard_Real[t];
  C_MyPnts = CMyPnts;
  C_MyU    = CMyU;
  C_MyV    = CMyV;
  
  const Standard_Real u0 = Surface->FirstUParameter();
  const Standard_Real u1 = Surface->LastUParameter();
  const Standard_Real v0 = Surface->FirstVParameter();
  const Standard_Real v1 = Surface->LastVParameter();

  const Standard_Real U1mU0sNbdeltaU = (u1-u0)/(Standard_Real)nbdeltaU;
  const Standard_Real V1mV0sNbdeltaV = (v1-v0)/(Standard_Real)nbdeltaV;

  gp_Pnt TP;
  Standard_Real    U,V;
  Standard_Integer i1, i2, Index=1;
  for (i1 = 0, U = u0; i1 <= nbdeltaU; i1++, U+= U1mU0sNbdeltaU) {
    for (i2 = 0, V = v0; i2 <= nbdeltaV; i2++, V+= V1mV0sNbdeltaV ) {
      Surface->D0(U,V,TP);
      CMyPnts[Index] = TP;
      CMyU[Index]    = U;
      CMyV[Index]    = V;
      TheBnd.Add(TP);
      Index++;
    }
  }

  Standard_Real tol=0.0;
  const Standard_Integer nbtriangles = NbTriangles();
  for (i1=1; i1<=nbtriangles; i1++) {
    const Standard_Real tol1 = DeflectionOnTriangle(Surface,i1);
    if(tol1>tol) tol=tol1;
  }

  tol*=DEFLECTION_COEFF;

  DeflectionOverEstimation(tol);
  FillBounding();
}

//=======================================================================
//function : IntPatch_Polyhedron
//purpose  : 
//=======================================================================
IntPatch_Polyhedron::IntPatch_Polyhedron (const Handle(Adaptor3d_HSurface)& Surface,
                                          const Standard_Integer nbu,
                                          const Standard_Integer nbv)
: TheDeflection(Epsilon(100.)),
  nbdeltaU(nbu),
  nbdeltaV(nbv),
  C_MyPnts(NULL),C_MyU(NULL),C_MyV(NULL),
  UMinSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
  UMaxSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
  VMinSingular(IntPatch_HInterTool::SingularOnVMin(Surface)),
  VMaxSingular(IntPatch_HInterTool::SingularOnVMin(Surface))
{ 
  const Standard_Integer t = (nbdeltaU+1)*(nbdeltaV+1)+1;
  gp_Pnt *CMyPnts     = new gp_Pnt[t];
  Standard_Real *CMyU = new Standard_Real[t];
  Standard_Real *CMyV = new Standard_Real[t];
  C_MyPnts = CMyPnts;
  C_MyU    = CMyU;
  C_MyV    = CMyV;
  
  const Standard_Real u0 = Surface->FirstUParameter();
  const Standard_Real u1 = Surface->LastUParameter();
  const Standard_Real v0 = Surface->FirstVParameter();
  const Standard_Real v1 = Surface->LastVParameter();

  const Standard_Real U1mU0sNbdeltaU = (u1-u0)/(Standard_Real)nbdeltaU;
  const Standard_Real V1mV0sNbdeltaV = (v1-v0)/(Standard_Real)nbdeltaV;

  gp_Pnt TP;
  Standard_Real U,V;
  Standard_Integer i1, i2, Index=1;
  for (i1 = 0, U = u0; i1 <= nbdeltaU; i1++, U+= U1mU0sNbdeltaU) {
    for (i2 = 0, V = v0; i2 <= nbdeltaV; i2++, V+= V1mV0sNbdeltaV ) {
      Surface->D0(U,V,TP);
      CMyPnts[Index] = TP;
      CMyU[Index]    = U;
      CMyV[Index]    = V;
      TheBnd.Add(TP);
      Index++;      
    }
  }

  Standard_Real tol=0.0;
  const Standard_Integer nbtriangles = NbTriangles();
  for (i1=1; i1<=nbtriangles; i1++) {
    const Standard_Real tol1 = DeflectionOnTriangle(Surface,i1);
    if(tol1>tol) tol=tol1;
  }
  
  tol*=DEFLECTION_COEFF;

  DeflectionOverEstimation(tol);
  FillBounding();
}


//=======================================================================
//function : DeflectionOnTriangle
//purpose  : 
//=======================================================================

Standard_Real IntPatch_Polyhedron::DeflectionOnTriangle
  (const Handle(Adaptor3d_HSurface)& Surface,
   const Standard_Integer Triang) const 
{
  Standard_Integer i1,i2,i3;    

  Triangle(Triang,i1,i2,i3);
  //-- Calcul de l eqution du plan
  Standard_Real u1,v1,u2,v2,u3,v3;
  gp_Pnt P1,P2,P3;
  P1 = Point(i1,u1,v1);
  P2 = Point(i2,u2,v2);
  P3 = Point(i3,u3,v3);
  if(P1.SquareDistance(P2)<=LONGUEUR_MINI_EDGE_TRIANGLE) return(0);
  if(P1.SquareDistance(P3)<=LONGUEUR_MINI_EDGE_TRIANGLE) return(0);
  if(P2.SquareDistance(P3)<=LONGUEUR_MINI_EDGE_TRIANGLE) return(0);
  gp_XYZ XYZ1=P2.XYZ()-P1.XYZ();
  gp_XYZ XYZ2=P3.XYZ()-P2.XYZ();
  gp_XYZ XYZ3=P1.XYZ()-P3.XYZ();
  gp_Vec NormalVector((XYZ1^XYZ2)+(XYZ2^XYZ3)+(XYZ3^XYZ1));
  Standard_Real aNormLen = NormalVector.Magnitude();
  if (aNormLen < gp::Resolution()) {
    return 0.;
  }
  //
  NormalVector.Divide(aNormLen);
  //-- Calcul du point u,v  au centre du triangle
  Standard_Real u = (u1+u2+u3)/3.0;
  Standard_Real v = (v1+v2+v3)/3.0;
  gp_Vec P1P(P1,Surface->Value(u,v));
  return(Abs(P1P.Dot(NormalVector)));
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
void IntPatch_Polyhedron::Parameters( const Standard_Integer Index
                                     ,Standard_Real &U
                                     ,Standard_Real &V) const 
{
  U = ((Standard_Real *)C_MyU)[Index];
  V = ((Standard_Real *)C_MyV)[Index];
}

//=======================================================================
//function : DeflectionOverEstimation
//purpose  : 
//=======================================================================
void IntPatch_Polyhedron::DeflectionOverEstimation(const Standard_Real flec)
{
  if(flec<0.0001) {  
    TheDeflection=0.0001;
    TheBnd.Enlarge(0.0001);
  }
  else { 
    TheDeflection=flec;
    TheBnd.Enlarge(flec);
  }
}

//=======================================================================
//function : DeflectionOverEstimation
//purpose  :
//=======================================================================
Standard_Real IntPatch_Polyhedron::DeflectionOverEstimation() const
{
  return TheDeflection;
}

//=======================================================================
//function : Bounding
//purpose  : 
//=======================================================================
const Bnd_Box& IntPatch_Polyhedron::Bounding() const
{
  return TheBnd;
}

//=======================================================================
//function : FillBounding
//purpose  : 
//=======================================================================
void IntPatch_Polyhedron::FillBounding()
{
  TheComponentsBnd=new Bnd_HArray1OfBox(1, NbTriangles());
  Bnd_Box Boite;
  Standard_Integer p1, p2, p3;
  Standard_Integer nbtriangles = NbTriangles();
  for (Standard_Integer iTri=1; iTri<=nbtriangles; iTri++) {
    Triangle(iTri, p1, p2, p3);
    Boite.SetVoid();
    const gp_Pnt& P1 = Point(p1);
    const gp_Pnt& P2 = Point(p2);
    const gp_Pnt& P3 = Point(p3);
    if(P1.SquareDistance(P2)>LONGUEUR_MINI_EDGE_TRIANGLE) {
      if(P1.SquareDistance(P3)>LONGUEUR_MINI_EDGE_TRIANGLE) {
	if(P2.SquareDistance(P3)>LONGUEUR_MINI_EDGE_TRIANGLE) {
	  Boite.Add(P1);
	  Boite.Add(P2);
	  Boite.Add(P3);
	}  
      }
    }
    Boite.Enlarge(TheDeflection);
    TheComponentsBnd->SetValue(iTri,Boite);  
  }
}

//=======================================================================
//function : ComponentsBounding
//purpose  : 
//=======================================================================
const Handle(Bnd_HArray1OfBox)& IntPatch_Polyhedron::ComponentsBounding () const
{
  return TheComponentsBnd;
}

//=======================================================================
//function : NbTriangles
//purpose  : 
//=======================================================================
Standard_Integer IntPatch_Polyhedron::NbTriangles () const
{
  return nbdeltaU*nbdeltaV*2;
}

//=======================================================================
//function : NbPoints
//purpose  : 
//=======================================================================
Standard_Integer IntPatch_Polyhedron::NbPoints () const
{
  return (nbdeltaU+1)*(nbdeltaV+1);
}

//=======================================================================
//function : TriConnex
//purpose  : 
//=======================================================================
Standard_Integer IntPatch_Polyhedron::TriConnex (const Standard_Integer Triang,
						 const Standard_Integer Pivot,
						 const Standard_Integer Pedge,
						 Standard_Integer&      TriCon,
						 Standard_Integer&      OtherP)   const {

  Standard_Integer Pivotm1    = Pivot-1;
  Standard_Integer nbdeltaVp1 = nbdeltaV+1;
  Standard_Integer nbdeltaVm2 = nbdeltaV + nbdeltaV;

// Pivot position in the MaTriangle :
  Standard_Integer ligP = Pivotm1/nbdeltaVp1;
  Standard_Integer colP = Pivotm1 - ligP * nbdeltaVp1;

// Point sur Edge position in the MaTriangle and edge typ :
  Standard_Integer ligE = 0, colE = 0, typE = 0;
  if (Pedge!=0) {
    ligE= (Pedge-1)/nbdeltaVp1;
    colE= (Pedge-1) - (ligE * nbdeltaVp1);
  // Horizontal
    if      (ligP==ligE) typE=1;
  // Vertical
    else if (colP==colE) typE=2;
  // Oblique
    else                 typE=3;
  }
  else {
    typE=0;
  }

// Triangle position General case :
  Standard_Integer linT = 0, colT = 0;
  Standard_Integer linO = 0, colO = 0;
  Standard_Integer t,tt;
  if (Triang!=0) {
    t = (Triang-1)/(nbdeltaVm2);
    tt= (Triang-1)-t*nbdeltaVm2;
    linT= 1+t;
    colT= 1+tt;
    if (typE==0) {
      if (ligP==linT) {
	ligE=ligP-1;
	colE=colP-1;
	typE=3;
      }
      else {
	if (colT==ligP+ligP) {
	  ligE=ligP;
	  colE=colP-1;
	  typE=1;
	}
	else {
	  ligE=ligP+1;
	  colE=colP+1;
	  typE=3;
	}
      }
    }
    switch (typE) {
    case 1:  // Horizontal
      if (linT==ligP) {
	linT++;
	linO=ligP+1;
	colO=(colP>colE)? colP : colE; //--colO=Max(colP, colE);
      }
      else {
	linT--;
	linO=ligP-1;
	colO=(colP<colE)? colP : colE;	//--colO=Min(colP, colE);
      }
      break;
    case 2:  // Vertical
      if (colT==(colP+colP)) {
        colT++;
	linO=(ligP>ligE)? ligP : ligE; 	//--linO=Max(ligP, ligE);
	colO=colP+1;;
      }
      else {
        colT--;
	linO=(ligP<ligE)? ligP : ligE; 	//--linO=Min(ligP, ligE);
	colO=colP-1;;
      }
      break;
    case 3:  // Oblique
      if ((colT&1)==0) {
	colT--;
	linO=(ligP>ligE)? ligP : ligE;	//--linO=Max(ligP, ligE);
	colO=(colP<colE)? colP : colE;	//--colO=Min(colP, colE);
      }
      else {
	colT++;
	linO=(ligP<ligE)? ligP : ligE;	//--linO=Min(ligP, ligE);
	colO=(colP>colE)? colP : colE;	//--colO=Max(colP, colE);
      }
      break;
    }
  }
  else {
    // Unknown Triangle position :
    if (Pedge==0) {
      // Unknown edge :
      linT=(1>ligP)? 1 : ligP;      //--linT=Max(1, ligP);
      colT=(1>(colP+colP))? 1 : (colP+colP);       //--colT=Max(1, colP+colP);
      if (ligP==0) linO=ligP+1;
      else         linO=ligP-1;
      colO=colP;
    }
    else {
      // Known edge We take the left or down connectivity :
      switch (typE) {
      case 1:  // Horizontal
	linT=ligP+1;
	colT=(colP>colE)? colP : colE; 	//--colT=Max(colP,colE);
	colT+=colT;
	linO=ligP+1;
	colO=(colP>colE)? colP : colE;	//--colO=Max(colP,colE);
	break;
      case 2:  // Vertical
	linT=(ligP>ligE)? ligP : ligE;	//--linT=Max(ligP, ligE);
	colT=colP+colP;
	linO=(ligP<ligE)? ligP : ligE; 	//--linO=Min(ligP, ligE);
	colO=colP-1;
	break;
      case 3:  // Oblique
	linT=(ligP>ligE)? ligP : ligE; 	//--linT=Max(ligP, ligE);
	colT=colP+colE;
	linO=(ligP>ligE)? ligP : ligE;	//--linO=Max(ligP, ligE);
	colO=(colP<colE)? colP : colE;	//--colO=Min(colP, colE);
	break;
      }
    }
  }

  TriCon=(linT-1)*nbdeltaVm2 + colT;

  if (linT<1) {
    linO=0;
    colO=colP+colP-colE;
    if (colO<0) {colO=0;linO=1;}
    else if (colO>nbdeltaV) {colO=nbdeltaV;linO=1;}
    TriCon=0;
  }
  else if (linT>nbdeltaU) {
    linO=nbdeltaU;
    colO=colP+colP-colE;
    if (colO<0) {colO=0;linO=nbdeltaU-1;}
    else if (colO>nbdeltaV) {colO=nbdeltaV;linO=nbdeltaU-1;}
    TriCon=0;
  }

  if (colT<1) {
    colO=0;
    linO=ligP+ligP-ligE;
    if (linO<0) {linO=0;colO=1;}
    else if (linO>nbdeltaU) {linO=nbdeltaU;colO=1;}
    TriCon=0;
  }
  else if (colT>nbdeltaV) {
    colO=nbdeltaV;
    linO=ligP+ligP-ligE;
    if (linO<0) {linO=0;colO=nbdeltaV-1;}
    else if (linO>nbdeltaU) {linO=nbdeltaU;colO=nbdeltaV-1;}
    TriCon=0;
  }

  OtherP=linO*nbdeltaVp1 + colO+1;


  //----------------------------------------------------
  //-- Detection des cas ou le triangle retourne est
  //-- invalide. Dans ce cas, on retourne le triangle
  //-- suivant par un nouvel appel a TriConnex.
  //-- 
  //-- Si En entree : Point(Pivot)==Point(Pedge) 
  //-- Alors on retourne OtherP a 0 
  //-- et Tricon = Triangle
  //--
  if(Point(Pivot).SquareDistance(Point(Pedge))<=LONGUEUR_MINI_EDGE_TRIANGLE) { 
    OtherP=0;
    TriCon=Triang;
#if MSG_DEBUG
    cout<<" Probleme ds IntCurveSurface_Polyhedron : Pivot et PEdge Confondus "<<endl;
#endif
    return(TriCon);
  }
  if(Point(OtherP).SquareDistance(Point(Pedge))<=LONGUEUR_MINI_EDGE_TRIANGLE) { 
#if MSG_DEBUG
    cout<<" Probleme ds IntCurveSurface_Polyhedron : OtherP et PEdge Confondus "<<endl;
#endif
    return(0); //-- BUG NON CORRIGE ( a revoir le role de nbdeltaU et nbdeltaV)
//    Standard_Integer TempTri,TempOtherP;
//    TempTri = TriCon;
//    TempOtherP = OtherP;
//    return(TriConnex(TempTri,Pivot,TempOtherP,TriCon,OtherP));
  }
  return TriCon;
}



//=======================================================================
//function : PlaneEquation
//purpose  : 
//=======================================================================

void IntPatch_Polyhedron::PlaneEquation (const Standard_Integer Triang,
					 gp_XYZ&        NormalVector,
					 Standard_Real& PolarDistance)  const
{
  Standard_Integer i1,i2,i3;
  Triangle(Triang,i1,i2,i3);

  gp_XYZ Pointi1(Point(i1).XYZ());
  gp_XYZ Pointi2(Point(i2).XYZ());
  gp_XYZ Pointi3(Point(i3).XYZ());
  

  gp_XYZ v1= Pointi2 - Pointi1;
  gp_XYZ v2= Pointi3 - Pointi2;
  gp_XYZ v3= Pointi1 - Pointi3;

  if(v1.SquareModulus()<=LONGUEUR_MINI_EDGE_TRIANGLE) { NormalVector.SetCoord(1.0,0.0,0.0); return; } 
  if(v2.SquareModulus()<=LONGUEUR_MINI_EDGE_TRIANGLE) { NormalVector.SetCoord(1.0,0.0,0.0); return; } 
  if(v3.SquareModulus()<=LONGUEUR_MINI_EDGE_TRIANGLE) { NormalVector.SetCoord(1.0,0.0,0.0); return; } 

  NormalVector= (v1^v2)+(v2^v3)+(v3^v1);
  Standard_Real aNormLen = NormalVector.Modulus();
  if (aNormLen < gp::Resolution()) {
    PolarDistance = 0.;
  }
  else {
    NormalVector.Divide(aNormLen);
    PolarDistance = NormalVector * Point(i1).XYZ();
  }
}
//=======================================================================
//function : Contain
//purpose  : 
//=======================================================================
Standard_Boolean IntPatch_Polyhedron::Contain (const Standard_Integer Triang,
					       const gp_Pnt& ThePnt) const
{  
  Standard_Integer i1,i2,i3;
  Triangle(Triang,i1,i2,i3);
  gp_XYZ Pointi1(Point(i1).XYZ());
  gp_XYZ Pointi2(Point(i2).XYZ());
  gp_XYZ Pointi3(Point(i3).XYZ());
  
  gp_XYZ v1=(Pointi2-Pointi1)^(ThePnt.XYZ()-Pointi1);
  gp_XYZ v2=(Pointi3-Pointi2)^(ThePnt.XYZ()-Pointi2);
  gp_XYZ v3=(Pointi1-Pointi3)^(ThePnt.XYZ()-Pointi3);
  if (v1*v2 >= 0. && v2*v3 >= 0. && v3*v1>=0.) 
    return Standard_True;
  else 
    return Standard_False;
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void IntPatch_Polyhedron::Dump()const
{
}
//=======================================================================
//function : Size
//purpose  : 
//=======================================================================
void IntPatch_Polyhedron::Size(Standard_Integer& nbdu,
			       Standard_Integer& nbdv) const
{
  nbdu=nbdeltaU;
  nbdv=nbdeltaV;
}
//=======================================================================
//function : Triangle
//purpose  : 
//=======================================================================
void IntPatch_Polyhedron::Triangle (const Standard_Integer Index,
				    Standard_Integer& P1,
				    Standard_Integer& P2,
				    Standard_Integer& P3) const
{
  Standard_Integer line=1+((Index-1)/(nbdeltaV*2));
  Standard_Integer colon=1+((Index-1)%(nbdeltaV*2));
  Standard_Integer colpnt=(colon+1)/2;

// General formula = (line-1)*(nbdeltaV+1)+colpnt
  
//  Position of P1 = MesXYZ(line,colpnt);
  P1= (line-1)*(nbdeltaV+1) + colpnt;

//  Position of P2= MesXYZ(line+1,colpnt+((colon-1)%2));
  P2= line*(nbdeltaV+1) + colpnt+((colon-1)%2);

//  Position of P3= MesXYZ(line+(colon%2),colpnt+1);
  P3= (line-1+(colon%2))*(nbdeltaV+1) + colpnt + 1;
  //-- printf("\nTriangle %4d    P1:%4d   P2:%4d   P3:%4d",Index,P1,P2,P3);
}
//=======================================================================
//function : Point
//=======================================================================
const gp_Pnt& IntPatch_Polyhedron::Point( const Standard_Integer Index
					 ,Standard_Real& U
					 ,Standard_Real& V) const 
{
  gp_Pnt *CMyPnts     = (gp_Pnt *)C_MyPnts;
  Standard_Real *CMyU = (Standard_Real *)C_MyU;
  Standard_Real *CMyV = (Standard_Real *)C_MyV;
  U=CMyU[Index];
  V=CMyV[Index];
  return CMyPnts[Index];
}
//=======================================================================
//function : Point
//=======================================================================
const gp_Pnt& IntPatch_Polyhedron::Point(const Standard_Integer Index) const {
  gp_Pnt *CMyPnts     = (gp_Pnt *)C_MyPnts;
  return CMyPnts[Index];
}

//=======================================================================
//function : Point
//=======================================================================
void IntPatch_Polyhedron::Point (const gp_Pnt& /*p*/, 
				 const Standard_Integer /*lig*/,
				 const Standard_Integer /*col*/,
				 const Standard_Real /*u*/,
				 const Standard_Real /*v*/) 
{
  //printf("\n IntPatch_Polyhedron::Point : Ne dois pas etre appelle\n");
}

//=======================================================================
//function : Point
//=======================================================================
void IntPatch_Polyhedron::Point (const Standard_Integer Index, gp_Pnt& P) const
{
  gp_Pnt *CMyPnts = (gp_Pnt *)C_MyPnts;
  P = CMyPnts[Index];
}
//=======================================================================
