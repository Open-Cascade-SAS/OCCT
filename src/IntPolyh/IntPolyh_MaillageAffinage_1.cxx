// File:	IntPolyh_MaillageAffinage.cxx
// Created:	Fri Mar  5 01:52:52 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>

//  modified by Edward AGAPOV (eap) Tue Jan 22 2002 (bug occ53)
//  - improve SectionLine table management (avoid memory reallocation)
//  - some protection against arrays overflow

//  modified by Edward AGAPOV (eap) Thu Feb 14 2002 (occ139)
//  - make Section Line parts rightly connected (prepend 2nd part to the 1st)
//  - TriangleShape() for debugging purpose

//  Modified by skv - Thu Sep 25 17:42:42 2003 OCC567
//  modified by ofv Thu Apr  8 14:58:13 2004 fip

//#ifndef _maillIso_HeaderFile
//#define _maillIso_HeaderFile

//#endif
#include <Standard_Stream.hxx>

#include <stdio.h>

#include <Precision.hxx>
#include <IntPolyh_MaillageAffinage.ixx>
#include <IntPolyh_Edge.hxx>
#include <IntPolyh_Couple.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <Bnd_HArray1OfBox.hxx> 
#include <gp_Pnt.hxx>
#include <gp.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>
#include <IntPolyh_ArrayOfCouples.hxx>

# ifdef DEB
#include <TopoDS_Shape.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <BRep_TFace.hxx>
#include <TopoDS_Face.hxx>
#ifdef DRAW
#include <DBRep.hxx>
#endif
# endif

# ifdef DEB
  # define MYDEBUG DEB
Standard_Integer MYDRAW1 = 0;
# else
  # define MYDEBUG 0
# endif

Standard_Integer MYPRINTma1 = 0;

#define MyTolerance 10.0e-7
#define MyConfusionPrecision 10.0e-12
#define SquareMyConfusionPrecision 10.0e-24




//=======================================================================
//function : FillArrayOfPnt
//purpose  : Compute points on one surface and fill an array of points
//=======================================================================
void IntPolyh_MaillageAffinage::FillArrayOfPnt(const Standard_Integer SurfID, 
					       const TColStd_Array1OfReal& Upars,
					       const TColStd_Array1OfReal& Vpars)
{
  Handle(Adaptor3d_HSurface) MaSurface=(SurfID==1)? MaSurface1:MaSurface2;
  IntPolyh_ArrayOfPoints &TPoints=(SurfID==1)? TPoints1:TPoints2;
  Standard_Integer NbSamplesU=(SurfID==1)? NbSamplesU1:NbSamplesU2;
  Standard_Integer NbSamplesV=(SurfID==1)? NbSamplesV1:NbSamplesV2;
  Bnd_Box *PtrBox = (SurfID==1) ? (&MyBox1) : (&MyBox2);
  Standard_Integer CpteurTabPnt=0;
  Standard_Real Tol;

  //cout << "Nb : " << NbSamplesU << " " << NbSamplesV << endl;

  Standard_Real u0 = Upars(1);
  Standard_Real v0 = Vpars(1);
  Standard_Real u1 = Upars(NbSamplesU);
  Standard_Real v1 = Vpars(NbSamplesV);
  IntCurveSurface_ThePolyhedronOfHInter polyhedron(MaSurface, Upars, Vpars);
  Tol=polyhedron.DeflectionOverEstimation();

  for(Standard_Integer BoucleU=1; BoucleU<=NbSamplesU; BoucleU++){
    Standard_Real U = Upars(BoucleU);
    for(Standard_Integer BoucleV=1; BoucleV<=NbSamplesV; BoucleV++){
      Standard_Real V = Vpars(BoucleV);
      gp_Pnt PtXYZ = (MaSurface)->Value(U,V);
      (TPoints[CpteurTabPnt]).Set(PtXYZ.X(), PtXYZ.Y(), PtXYZ.Z(), U, V);
      CpteurTabPnt++;
      PtrBox->Add(PtXYZ);
    }
  }
    


  TPoints.SetNbPoints(CpteurTabPnt);

  Tol*=1.2;

  Standard_Real a1,a2,a3,b1,b2,b3;
  PtrBox->Get(a1,a2,a3,b1,b2,b3);
  PtrBox->Update(a1-Tol,a2-Tol,a3-Tol,b1+Tol,b2+Tol,b3+Tol);
  PtrBox->Enlarge(MyTolerance);
  //cout << "End Fill Array 1" << endl;
}

//=======================================================================
//function : FillArrayOfPnt
//purpose  : Compute points on one surface and fill an array of points
//           REMPLISSAGE DU TABLEAU DE POINTS
//=======================================================================
void IntPolyh_MaillageAffinage::FillArrayOfPnt(const Standard_Integer SurfID,
					       const Standard_Boolean isShiftFwd,
					       const TColStd_Array1OfReal& Upars,
					       const TColStd_Array1OfReal& Vpars)
{

  Handle(Adaptor3d_HSurface) MaSurface=(SurfID==1)? MaSurface1:MaSurface2;
  IntPolyh_ArrayOfPoints &TPoints=(SurfID==1)? TPoints1:TPoints2;
  Standard_Integer NbSamplesU=(SurfID==1)? NbSamplesU1:NbSamplesU2;
  Standard_Integer NbSamplesV=(SurfID==1)? NbSamplesV1:NbSamplesV2;
  Bnd_Box *PtrBox = (SurfID==1) ? (&MyBox1) : (&MyBox2);
  Standard_Integer CpteurTabPnt=0;
  Standard_Real Tol;

  //cout << "FillArrayOfPnt 2" << endl;
  //cout << "??????????????" << endl;
  Standard_Real resol = gp::Resolution();
  Standard_Real u0 = Upars(1);
  Standard_Real v0 = Vpars(1);
  Standard_Real u1 = Upars(NbSamplesU);
  Standard_Real v1 = Vpars(NbSamplesV);
  IntCurveSurface_ThePolyhedronOfHInter polyhedron(MaSurface, Upars, Vpars);
  Tol=polyhedron.DeflectionOverEstimation();

  for(Standard_Integer BoucleU=1; BoucleU<=NbSamplesU; BoucleU++){
    Standard_Real U = Upars(BoucleU);
    for(Standard_Integer BoucleV=1; BoucleV<=NbSamplesV; BoucleV++){
      Standard_Real V = Vpars(BoucleV);

      gp_Pnt PtXYZ;
      gp_Vec aDU;
      gp_Vec aDV;
      gp_Vec aNorm;

      MaSurface->D1(U, V, PtXYZ, aDU, aDV);
      
      aNorm = aDU.Crossed(aDV);
      Standard_Real aMag = aNorm.Magnitude();
      if (aMag > resol) {
	aNorm /= aMag;
	aNorm.Multiply(Tol*1.5);
	
	if (isShiftFwd)
	  PtXYZ.Translate(aNorm);
	else
	  PtXYZ.Translate(aNorm.Reversed());
      }

      (TPoints[CpteurTabPnt]).Set(PtXYZ.X(), PtXYZ.Y(), PtXYZ.Z(), U, V);
      CpteurTabPnt++;
      PtrBox->Add(PtXYZ);
      }
  }

  TPoints.SetNbPoints(CpteurTabPnt);

  Tol*=1.2;

  Standard_Real a1,a2,a3,b1,b2,b3;
  PtrBox->Get(a1,a2,a3,b1,b2,b3);
  PtrBox->Update(a1-Tol,a2-Tol,a3-Tol,b1+Tol,b2+Tol,b3+Tol);
  PtrBox->Enlarge(MyTolerance);
}//fin FillArrayOfPnt

