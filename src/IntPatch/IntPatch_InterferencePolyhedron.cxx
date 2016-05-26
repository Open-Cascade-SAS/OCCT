// Created on: 1992-11-09
// Created by: Didier PIFFAULT
// Copyright (c) 1992-1999 Matra Datavision
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


#include <Bnd_BoundSortBox.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Intf.hxx>
#include <Intf_SectionLine.hxx>
#include <Intf_SectionPoint.hxx>
#include <Intf_SeqOfSectionLine.hxx>
#include <Intf_SeqOfSectionPoint.hxx>
#include <Intf_SeqOfTangentZone.hxx>
#include <Intf_TangentZone.hxx>
#include <IntPatch_InterferencePolyhedron.hxx>
#include <IntPatch_Polyhedron.hxx>
#include <IntPatch_PolyhedronTool.hxx>
#include <NCollection_LocalArray.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>

static const int Pourcent3[9] = {0, 1, 2, 0, 1, 2, 0, 1, 2};

//=======================================================================
//function : IntPatch_InterferencePolyhedron
//purpose  : Empty constructor.
//=======================================================================

IntPatch_InterferencePolyhedron::IntPatch_InterferencePolyhedron  ()
: Intf_Interference(Standard_False)
{}

//=======================================================================
//function : IntPatch_InterferencePolyhedron
//purpose  : 
//=======================================================================

IntPatch_InterferencePolyhedron::IntPatch_InterferencePolyhedron 
  (const IntPatch_Polyhedron& FirstPol, const IntPatch_Polyhedron& SeconPol)
: Intf_Interference(Standard_False)
{
  if (!IntPatch_PolyhedronTool::Bounding(FirstPol).IsOut
      (IntPatch_PolyhedronTool::Bounding(SeconPol))) {
    Tolerance=IntPatch_PolyhedronTool::DeflectionOverEstimation(FirstPol)+
              IntPatch_PolyhedronTool::DeflectionOverEstimation(SeconPol);
    if (Tolerance==0.)
      Tolerance=Epsilon(1000.);
    Interference(FirstPol, SeconPol);
  }
}

//=======================================================================
//function : IntPatch_InterferencePolyhedron
//purpose  : 
//=======================================================================

IntPatch_InterferencePolyhedron::IntPatch_InterferencePolyhedron 
  (const IntPatch_Polyhedron& Objet)
: Intf_Interference(Standard_True)
{
  Tolerance=IntPatch_PolyhedronTool::DeflectionOverEstimation(Objet)*2;
  if (Tolerance==0.)
    Tolerance=Epsilon(1000.);
  Interference(Objet,Objet); //-- lbr le 5 juillet 96
 }


//=======================================================================
//function : Perform 
//purpose  : 
//=======================================================================

void IntPatch_InterferencePolyhedron::Perform 
  (const IntPatch_Polyhedron& FirstPol, const IntPatch_Polyhedron& SeconPol)
{
  SelfInterference(Standard_False);
  if (!IntPatch_PolyhedronTool::Bounding(FirstPol).IsOut
      (IntPatch_PolyhedronTool::Bounding(SeconPol))) {
    Tolerance=IntPatch_PolyhedronTool::DeflectionOverEstimation(FirstPol)+
              IntPatch_PolyhedronTool::DeflectionOverEstimation(SeconPol);
    if (Tolerance==0.)
      Tolerance=Epsilon(1000.);
    Interference(FirstPol, SeconPol);
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void IntPatch_InterferencePolyhedron::Perform  
  (const IntPatch_Polyhedron& Objet)
{
  SelfInterference(Standard_True);
  Tolerance=IntPatch_PolyhedronTool::DeflectionOverEstimation(Objet)*2;
  if (Tolerance==0.)
    Tolerance=Epsilon(1000.);
  Interference(Objet);
}


//=======================================================================
//function : Interference
//purpose  : 
//=======================================================================

void IntPatch_InterferencePolyhedron::Interference 
  (const IntPatch_Polyhedron&)
{}

void IntPatch_InterferencePolyhedron::Interference 
  (const IntPatch_Polyhedron& FirstPol, const IntPatch_Polyhedron& SeconPol)
{
  Standard_Boolean  gridOnFirst=Standard_True;
  Standard_Integer  NbTrianglesFirstPol  = IntPatch_PolyhedronTool::NbTriangles(FirstPol);
  Standard_Integer  NbTrianglesSecondPol = IntPatch_PolyhedronTool::NbTriangles(SeconPol);  
  Standard_Integer iFirst, iSecon;

  //------------------------------------------------------------------------------------------
  //-- the same number of triangles it is necessary to test better on
  //-- the size of boxes.
  //--
  //-- the second is chosen if nbTri1 > 2*nbTri2   or   if VolBoit1 > 2*VolBoit2
  //--
  //--if (!SelfIntf && NbTrianglesFirstPol>NbTrianglesSecondPol)
  //--  gridOnFirst=Standard_False;
  
  if(!SelfIntf) { 
    if(NbTrianglesFirstPol > NbTrianglesSecondPol+NbTrianglesSecondPol) gridOnFirst=Standard_False;

    Standard_Real vol1,vol2,Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    IntPatch_PolyhedronTool::Bounding(FirstPol).Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    vol1 = (Xmax-Xmin)*(Ymax-Ymin)*(Zmax-Zmin);

    IntPatch_PolyhedronTool::Bounding(SeconPol).Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    vol2 = (Xmax-Xmin)*(Ymax-Ymin)*(Zmax-Zmin);
    
    if(vol1> 8.0*vol2)  gridOnFirst=Standard_False;
  }


  if (gridOnFirst) {
    Bnd_BoundSortBox TheGridFirst;
    TheGridFirst.Initialize(IntPatch_PolyhedronTool::Bounding(FirstPol),
			    IntPatch_PolyhedronTool::ComponentsBounding(FirstPol));

    for (iSecon=1; iSecon<=NbTrianglesSecondPol; iSecon++) {

      TColStd_ListIteratorOfListOfInteger iLoI(TheGridFirst.Compare
	(IntPatch_PolyhedronTool::ComponentsBounding(SeconPol)->Value(iSecon)));
      while (iLoI.More()) {
	iFirst=iLoI.Value();
	if (SelfIntf) {
	  if (iFirst<iSecon)
	    Intersect(iFirst, FirstPol, iSecon, SeconPol);
	}
	else
	  Intersect(iFirst, FirstPol, iSecon, SeconPol);
	iLoI.Next();
      }
    }
  }

  else {
    Bnd_BoundSortBox TheGridSecond;
    TheGridSecond.Initialize(IntPatch_PolyhedronTool::Bounding(SeconPol), 
			     IntPatch_PolyhedronTool::ComponentsBounding(SeconPol));

    for (iFirst=1; iFirst<=NbTrianglesFirstPol; iFirst++) {
      TColStd_ListIteratorOfListOfInteger
	iLoI(TheGridSecond.Compare
	     (IntPatch_PolyhedronTool::ComponentsBounding(FirstPol)->Value(iFirst)));
      
      while (iLoI.More()) {
	iSecon=iLoI.Value();
	if (SelfIntf) {
	  if (iFirst<iSecon)
	    Intersect(iFirst, FirstPol, iSecon, SeconPol);
	}
	else
	  Intersect(iFirst, FirstPol, iSecon, SeconPol);
	iLoI.Next();
      }
    }
  }
}


//=======================================================================
//function : Intersect
//purpose  : Intersection of two triangles issue from two Polyhedron.
//=======================================================================

void IntPatch_InterferencePolyhedron::Intersect 
(const Standard_Integer Tri1, const IntPatch_Polyhedron& FirstPol,
 const Standard_Integer Tri2, const IntPatch_Polyhedron& SeconPol)
{
  IntPatch_PolyhedronTool::Triangle(FirstPol, Tri1,OI[0],OI[1],OI[2]);
  IntPatch_PolyhedronTool::Triangle(SeconPol, Tri2,TI[0],TI[1],TI[2]);
  
  // If there is an intersection of a polyhedron with itself, the
  // intersections are excluded 
  // from a triangle with connected triangles :
  
  if (SelfIntf) {
    if (OI[0]==TI[0] || OI[0]==TI[1] || OI[0]==TI[2] ||
	OI[1]==TI[0] || OI[1]==TI[1] || OI[1]==TI[2] ||
	OI[2]==TI[0] || OI[2]==TI[1] || OI[2]==TI[2] ) return;
  }
  
  // The precision of intersections includes two values ;
  
  // - Tolerance :This value allows detecting potential 
  //              intersections in all cases.  The value should be the
    //            sum of upper bounds of tops pof two polyhedrons.
  
  // - floatGap : This value is the actual precision of calculation 
  //              of line of section.Its value is very small, it
  //              allows having the same behaviour for
  //              geometry tests as for the values used.
  
  Standard_Real floatGap=1e-13 ; //-- Epsilon(1000.);
  
  
  // Equation of the triangle plane of the objet 
  gp_XYZ ONor; // Normal vector.
  Standard_Real Odp;    // Polar Distance.
  Intf::PlaneEquation(IntPatch_PolyhedronTool::Point(FirstPol, OI[0]),
		      IntPatch_PolyhedronTool::Point(FirstPol, OI[1]),
		      IntPatch_PolyhedronTool::Point(FirstPol, OI[2]),
		      ONor, Odp);

  
// Equation of the triangle plane of the tool
  gp_XYZ TNor; // Normal vector.
  Standard_Real Tdp;    // Polar distance.
  Intf::PlaneEquation(IntPatch_PolyhedronTool::Point(SeconPol, TI[0]),
		      IntPatch_PolyhedronTool::Point(SeconPol, TI[1]),
		      IntPatch_PolyhedronTool::Point(SeconPol, TI[2]),
		      TNor, Tdp);


// Scalar product of two normalized vectors -> cosinus of the angle
  Incidence= Abs(TNor*ONor);

// Distance of the plane of the triangle from the object by three points of SeconPol
  Standard_Real dfOpT[3];
  dfOpT[0]=ONor*(IntPatch_PolyhedronTool::Point(SeconPol, TI[0]).XYZ())-Odp;
  dfOpT[1]=ONor*(IntPatch_PolyhedronTool::Point(SeconPol, TI[1]).XYZ())-Odp;
  dfOpT[2]=ONor*(IntPatch_PolyhedronTool::Point(SeconPol, TI[2]).XYZ())-Odp;

// Distance of the plane of the triangle from the tool by three points of FirstPol
  Standard_Real dpOfT[3];
  dpOfT[0]=TNor*(IntPatch_PolyhedronTool::Point(FirstPol, OI[0]).XYZ())-Tdp;
  dpOfT[1]=TNor*(IntPatch_PolyhedronTool::Point(FirstPol, OI[1]).XYZ())-Tdp;
  dpOfT[2]=TNor*(IntPatch_PolyhedronTool::Point(FirstPol, OI[2]).XYZ())-Tdp;

// Values defining the couple of triangle dpOpT, dpOeT, deOpT
  CoupleCharacteristics(FirstPol, SeconPol);

// If three  points  of the triangle of <SeconPol> are in the plane of the
// triangle of <Obje> within <Tolerance> the eventual tangency zone is found.

  Intf_TangentZone TheTZ;
  if ((Abs(dfOpT[0])<=Tolerance && 
       Abs(dfOpT[1])<=Tolerance &&
       Abs(dfOpT[2])<=Tolerance)  &&
      (Abs(dpOfT[0])<=Tolerance && 
       Abs(dpOfT[1])<=Tolerance &&
       Abs(dpOfT[2])<=Tolerance) &&
      (Abs(dfOpT[0]+dfOpT[1]+dfOpT[2])!=
       Abs(dfOpT[0])+Abs(dfOpT[1])+Abs(dfOpT[2])) &&
      (Abs(dpOfT[0]+dpOfT[1]+dpOfT[2])!=
       Abs(dpOfT[0])+Abs(dpOfT[1])+Abs(dpOfT[2]))){

    if (TangentZoneValue(TheTZ, FirstPol, Tri1, SeconPol, Tri2))
    {
      if (!Insert(TheTZ)) myTZones.Append(TheTZ);
    }
  }

// Otherwise line of section is calculated:
  else {
    Standard_Integer iObj, iToo;

    // Zone de stockage des resultats :
    Standard_Integer nbpiOT=0;
    Standard_Integer nbpiO=0;
    Standard_Integer nbpiT=0;
    Intf_SeqOfSectionPoint piOT;
    Standard_Real parO[3];
    Standard_Real parT[3];

    // Indicateurs d arete touchee 
    Standard_Integer edOT[3];
    Standard_Integer edTT[3];

    // Initializations
    //--for (iObj=0; iObj<3; iObj++) {
    //  parO[iObj]=parT[iObj]=-1.;
    //  edOT[iObj]=edTT[iObj]=1;
    //--}
    parO[0]=parT[0]=parO[1]=parT[1]=parO[2]=parT[2]=-1.0;
    edOT[0]=edTT[0]=edOT[1]=edTT[1]=edOT[2]=edTT[2]= 1;

    // Singularite VERTEX VERTEX
    //for (iObj=0; iObj<3; iObj++) {
    //  for (iToo=0; iToo<3; iToo++) {
    //	if (dpOpT[iObj][iToo] <= floatGap) {
    //	  piOT.Append(Intf_SectionPoint(IntPatch_PolyhedronTool::Point(FirstPol, OI[iObj]),
    //					Intf_VERTEX, OI[iObj], 0, 0.,
    //					Intf_VERTEX, TI[iToo], 0, 0.,
    //					Incidence));
    //	  parO[iObj]=0.; 
    //	  parT[iToo]=0.; 
    //	  edOT[Pourcent3[iObj+2]]=0; edOT[iObj]=0;
    //	  edTT[Pourcent3[iToo+2]]=0; edTT[iToo]=0;
    //	  nbpiOT++; nbpiO++; nbpiT++;
    //	}
    // }
    //}
    //---------------------------->
    for (iObj=0; iObj<3; iObj++) {
      for (iToo=0; iToo<3; iToo++) {
	if (dpOpT[iObj][iToo] <= floatGap) {
	  piOT.Append(Intf_SectionPoint(IntPatch_PolyhedronTool::Point(FirstPol, OI[iObj]),
					Intf_VERTEX, OI[iObj], 0, 0.,
					Intf_VERTEX, TI[iToo], 0, 0.,
					Incidence));
	  parO[iObj]=parT[iToo]=0.0;
	  edOT[Pourcent3[iObj+2]]=edOT[iObj]=edTT[Pourcent3[iToo+2]]=edTT[iToo]=0;
	  nbpiOT++; nbpiO++; nbpiT++;
	}
      }
    }
      
      
      // Singularite VERTEX EDGE
    Standard_Integer inext, jnext;
    for (iObj=0; iObj<3; iObj++) {
      if (parO[iObj]==-1.) {
	for (iToo=0; iToo<3; iToo++) {
	  inext=Pourcent3[iToo+1];
	  if (edTT[iToo]==1) {
	    if (dpOeT[iObj][iToo] <= floatGap && dpOeT[iObj][iToo]>=-floatGap ) {
	      if ((dpOpT[iObj][iToo]+dpOpT[iObj][inext])<vtt[iToo].Modulus()) {
		parT[iToo]=dpOpT[iObj][iToo]/(dpOpT[iObj][iToo]+
					      dpOpT[iObj][inext]);
		if (TI[iToo]>TI[inext]) parT[iToo]=1.-parT[iToo];
		piOT.Append(Intf_SectionPoint
			    (IntPatch_PolyhedronTool::Point(FirstPol, OI[iObj]),
			     Intf_VERTEX, OI[iObj], 0, 0.,
			     Intf_EDGE, Min(TI[iToo],TI[inext]),
			                Max(TI[iToo],TI[inext]), parT[iToo],
			     Incidence));
		parO[iObj]=0.; 
		edOT[Pourcent3[iObj+2]]=0; edOT[iObj]=0;
		edTT[iToo]=0;
		nbpiOT++;  nbpiO++; nbpiT++;
	      }
	    }
	  }
	}
      }
    }

    // Singularite EDGE VERTEX
    for (iToo=0; iToo<3; iToo++) {
      if (parT[iToo]==-1.) {
	for (iObj=0; iObj<3; iObj++) {
	  inext=Pourcent3[iObj+1];
	  if (edOT[iObj]==1) {
	    if (Abs(deOpT[iObj][iToo]) <= floatGap) {
	      if ((dpOpT[iObj][iToo]+dpOpT[inext][iToo])<voo[iObj].Modulus()){
		parO[iObj]=dpOpT[iObj][iToo]/(dpOpT[iObj][iToo]+
					      dpOpT[inext][iToo]); 
		if (OI[iObj]>OI[inext]) parO[iObj]=1.-parO[iObj];
		piOT.Append(Intf_SectionPoint
			    (IntPatch_PolyhedronTool::Point(SeconPol, TI[iToo]),
			     Intf_EDGE, Min(OI[iObj],OI[inext]),
			                Max(OI[iObj],OI[inext]), parO[iObj],
			     Intf_VERTEX, TI[iToo], 0, 0.,
			     Incidence));
		parT[iToo]=0.; 
		edOT[iObj]=edTT[Pourcent3[iToo+2]]=edTT[iToo]=0;
		nbpiOT++;  nbpiO++; nbpiT++;
	      }
	    }
	  }
	}
      }
    }

    // Singularite FACE VERTEX
    for (iToo=0; iToo<3; iToo++) {
      if (parT[iToo]!=0.) {
	if (Abs(dfOpT[iToo]) <= floatGap) {
	  piOT.Append(Intf_SectionPoint(IntPatch_PolyhedronTool::Point(SeconPol, TI[iToo]),
					Intf_FACE,   Tri1,  0, 0.,
					Intf_VERTEX, TI[iToo], 0, 0.,
					Incidence));
	  parT[iToo]=0.;
	  edTT[Pourcent3[iToo+2]]=edTT[iToo]=0;
	  nbpiOT++; nbpiT++;
	}
      }
    }

    // Singularite VERTEX FACE
    for (iObj=0; iObj<3; iObj++) {
      if (parO[iObj]!=0.) {
	if (Abs(dpOfT[iObj]) <= floatGap) {
	  piOT.Append(Intf_SectionPoint(IntPatch_PolyhedronTool::Point(FirstPol, OI[iObj]),
					Intf_VERTEX, OI[iObj], 0, 0.,
					Intf_FACE,   Tri2,  0, 0.,
					Incidence));
	  parO[iObj]=0.;
	  edOT[Pourcent3[iObj+2]]=edOT[iObj]=0;
	  nbpiOT++; nbpiO++;
	}
      }
    }
    
    // Singularite EDGE EDGE
    gp_Pnt piO;
    gp_XYZ piT;
    Standard_Real lg;
    for (iObj=0; iObj<3; iObj++) {
      inext=Pourcent3[iObj+1];
      if (edOT[iObj]==1 && (dpOfT[iObj]*dpOfT[inext])<0.) {
	lg=dpOfT[iObj]/(dpOfT[iObj]-dpOfT[inext]);
	if (lg>0. && lg<1.) {
	  for (iToo=0; iToo<3; iToo++) {
	    jnext=Pourcent3[iToo+1];
	    if (edTT[iToo]==1 && (dfOpT[iToo]*dfOpT[jnext])<0.) {
	      lg=dfOpT[iToo]/(dfOpT[iToo]-dfOpT[jnext]);
	      if (lg>0. && lg<1.) {
		Standard_Boolean Pb=Standard_False;
		if (OI[iObj]>OI[inext]) {
		  Standard_Real div=(dpOeT[inext][iToo]-dpOeT[iObj][iToo]);
		  if(div>floatGap || div<-floatGap) { 
		    parO[iObj]=dpOeT[inext][iToo]/div;
		    piO=(IntPatch_PolyhedronTool::Point(FirstPol,OI[inext]).XYZ()) +
		      (voo[iObj].Reversed()*parO[iObj]);
		  }
		  else 
		    Pb=Standard_True;
		}
		else {
		  Standard_Real div = dpOeT[iObj][iToo]-dpOeT[inext][iToo];
		  if(div>floatGap || div<-floatGap) { 
		    parO[iObj]=dpOeT[iObj][iToo]/
		      (dpOeT[iObj][iToo]-dpOeT[inext][iToo]);;
		    piO=(IntPatch_PolyhedronTool::Point(FirstPol,OI[iObj]).XYZ()) +
		      (voo[iObj]*parO[iObj]);
		  }
		  else 
		    Pb=Standard_True;
		}
		if (TI[iToo]>TI[jnext]) {
		  Standard_Real div=(deOpT[iObj][jnext]-deOpT[iObj][iToo]);
		  if(div>floatGap || div<-floatGap) { 
		    parT[iToo]=deOpT[iObj][jnext]/div;
		    piT=(IntPatch_PolyhedronTool::Point(SeconPol,TI[jnext]).XYZ()) +
		      (vtt[iToo].Reversed()*parT[iToo]);
		  }
		  else 
		    Pb=Standard_True;
		}
		else {
		  Standard_Real div=(deOpT[iObj][iToo]-deOpT[iObj][jnext]);
		  if(div>floatGap || div<-floatGap) {  
		    parT[iToo]=deOpT[iObj][iToo]/div;		    
		    piT=(IntPatch_PolyhedronTool::Point(SeconPol,TI[iToo]).XYZ()) +
		      (vtt[iToo]*parT[iToo]);
		  }
		  else 
		    Pb=Standard_True;
		}
		if(Pb==Standard_False) { 
		  piT-=piO.XYZ();
		  lg=piT.Modulus();
		  if (lg <= floatGap){
		    piOT.Append(Intf_SectionPoint
				(piO,
				 Intf_EDGE, Min(OI[iObj],OI[inext]),
				 Max(OI[iObj],OI[inext]), parO[iObj],
				 Intf_EDGE, Min(TI[iToo],TI[jnext]), 
				 Max(TI[iToo],TI[jnext]), parT[iToo],
				 Incidence));
		    edOT[iObj]=edTT[iToo]=0;
		    nbpiOT++;  nbpiO++; nbpiT++;
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    // Intersection EDGE FACE
    for (iObj=0; iObj<3; iObj++) {
      inext=Pourcent3[iObj+1];
      if (edOT[iObj]==1 && (dpOfT[iObj]*dpOfT[inext])<0.) {
	lg=dpOfT[iObj]/(dpOfT[iObj]-dpOfT[inext]);
	if (lg>0. && lg<1.) {
	  parO[iObj]=lg;
	  piO=(IntPatch_PolyhedronTool::Point(FirstPol, OI[iObj]).XYZ())+
	    (voo[iObj]*parO[iObj]);
	  if (OI[iObj]>OI[inext]) parO[iObj]=1.-parO[iObj];
	  piOT.Append(
	    Intf_SectionPoint (piO,
			       Intf_EDGE, Min(OI[iObj],OI[inext]),
			                  Max(OI[iObj],OI[inext]), parO[iObj],
			       Intf_FACE, Tri2, 0, 0., Incidence));
	  nbpiOT++; nbpiO++;
	}
      }
    }

    // Intersection FACE EDGE
    for (iToo=0; iToo<3; iToo++) {
      jnext=Pourcent3[iToo+1];
      if (edTT[iToo]==1 && (dfOpT[iToo]*dfOpT[jnext])<0.) {
	lg=dfOpT[iToo]/(dfOpT[iToo]-dfOpT[jnext]);
	if (lg>0. && lg<1.) {
	  parT[iToo]=lg;
	  piO=(IntPatch_PolyhedronTool::Point(SeconPol, TI[iToo]).XYZ())+
	       (vtt[iToo]*parT[iToo]);
	  if (TI[iToo]>TI[jnext]) parT[iToo]=1.-parT[iToo];
	  piOT.Append(Intf_SectionPoint
	    (piO,
	     Intf_FACE, Tri1, 0, 0.,
	     Intf_EDGE, Min(TI[iToo],TI[jnext]),
	                Max(TI[iToo],TI[jnext]), parT[iToo],
	     Incidence));
	  nbpiOT++; nbpiT++;
	}
      }
    }
    
    NCollection_LocalArray <Standard_Integer> id(Max(nbpiOT, 4));

    Standard_Integer ideb=-1;
    Standard_Integer ifin=-2;

    if (nbpiOT>1) {

// Sort the <nbpiOT> sections points along the intersection beetween the
// two triangles :

      gp_XYZ dir=ONor^TNor;
      NCollection_LocalArray <Standard_Real> d(nbpiOT);
      Standard_Integer iPi, iPs;
      for (iPi=0; iPi<nbpiOT; iPi++) {
	d[iPi]=dir*piOT(iPi+1).Pnt().XYZ();
      }

      Standard_Integer di;
      id[0]=0;
      for (iPi=1; iPi<nbpiOT; iPi++) {
	id[iPi]=iPi;
	for (iPs=iPi-1; iPs>=0; iPs--) {
	  if (d[id[iPs]] > d[id[iPs+1]]) {
	    di=id[iPs+1];
	    id[iPs+1]=id[iPs];
	    id[iPs]=di;
	  }
	  else break;
	}
      }
    }

// Possibility of line of section :

    if (nbpiO==2 && nbpiT==2) {

      // In the case when an edge is in the plane of the other triangle
      // it is necessary to check if it has not been already processed
      // on a connected triangle :

    // Pour l objet :
      Standard_Integer pivo=-1;
      Standard_Integer pedg=-1;
      if (parO[0]==0.) {
	pivo=0;
	if      (parO[1]==0.) pedg=1;
	else if (parO[2]==0.) pedg=2;
      }
      else if (parO[1]==0.) {
	pivo=1;
	if (parO[2]==0.) pedg=2;
      }
      if (pivo>=0 && pedg>=0) {
	IntPatch_PolyhedronTool::TriConnex(FirstPol, Tri1,OI[pivo],OI[pedg],pivo,pedg);
	if (pivo > Tri1) { 
	  nbpiOT=0;
	  ideb=-1;        // On a deja trouve celle ci
	  ifin=-2;
	}
      }

    // For the tool :
      pivo=-1;
      pedg=-1;
      if (parT[0]==0.) {
	pivo=0;
	if      (parT[1]==0.) pedg=1;
	else if (parT[2]==0.) pedg=2;
      }
      else if (parT[1]==0.) {
	pivo=1;
	if (parT[2]==0.) pedg=2;
      }
      if (pivo>=0 && pedg>=0) {
	IntPatch_PolyhedronTool::TriConnex(SeconPol, Tri2,TI[pivo],TI[pedg],pivo,pedg);
	if (pivo > Tri2) {
	  nbpiOT=0;
	  ideb=-1;        // It has been already found
	  ifin=-2;
	}
      }

      if (nbpiOT>0) {

// If there is a covering up : insert the section  line in  the existent
// list or create a new section line :

	if (piOT(id[0]+1).TypeOnFirst()==Intf_FACE) {
	  if (piOT(id[1]+1).TypeOnFirst()==Intf_FACE) {
	    ideb=-id[0]-1;        // No line of section possible
	    ifin=-id[1]-1;        // 
	  }
	  else if (piOT(id[1]+1).TypeOnSecond()!=Intf_FACE) {
	    ideb=id[1];     // No line of section possible
	    ifin=id[1];     // only a pointersec
	  }
	  else if (nbpiOT>=3) {
	    ideb=id[1];     // Retrieve 2 segments of section
	    ifin=id[2];     //
	  }
	  else {
	    ideb=-999;        // No line of section possible
	    ifin=-999;
	  }
	}
	else  if (piOT(id[0]+1).TypeOnSecond()==Intf_FACE) {
	  if (piOT(id[1]+1).TypeOnSecond()==Intf_FACE) {
	    ideb=-id[0]-1;        // No line of section possible
	    ifin=-id[1]-1;        // 
	  }
	  else if (piOT(id[1]+1).TypeOnFirst()!=Intf_FACE) {
	    ideb=id[1];     // No line of section possible
	    ifin=id[1];     // only a pointersec
	  }
	  else if (nbpiOT>=3) {
	    ideb=id[1];     // Recouvrement des 2 segments de section
	    ifin=id[2];     //
	  }
	  else {
	    ideb=-999;        // No line of section possible
	    ifin=-999;
	  }
	}

	else { // Singularity on the first point there is only two or
	  ideb=id[0];   // three pointersec, so the first is a solution
	  ifin=id[1];   // and the second too.
	}
      }


// Insertion of the segment found in the existing section lines :

      if(ideb<0) { 
	if(ifin<0) {
	  if(ideb!=-999) { 
	    //static unsigned nisp=0;
	    Standard_Real d=piOT(-ideb).Pnt().Distance(piOT(-ifin).Pnt());	  
	    if(d<Tolerance) { 
	      Insert(piOT(-ideb), piOT(-ifin));	      
	      //-- cout<<"Insertion Point IntPatch_InterferencePolyhedron 1,2 d="<<d<<" Tol="<<Tolerance<<" num:"<<++nisp<<endl;
	      //-- piOT(-ideb).Dump(1);  piOT(-ifin).Dump(0);
	      //-- cout<<"point p"<<++nisp<<" "<<piOT(-ideb).Pnt().X()<<" "<<piOT(-ideb).Pnt().Y()<<" "<<piOT(-ideb).Pnt().Z()<<endl;
	    }
	    else { 
	      //-- cout<<"Insertion Point IntPatch_InterferencePolyhedron 1,2 d="<<d<<" Tol="<<Tolerance<<" NON INSERE "<<endl;
	    }
	  }
	}
      }
      else if (ideb>=0) {
	if (ideb!=ifin) { 
	  Insert(piOT(ideb+1), piOT(ifin+1));
	  
	  //	else
	  //	 un pointersec : It is necessary to check if it has not been already found 
	  //                       and if not insert it in the list.     
	  //                       Attention! It is necessary to check
	  //                       for each new segment if a point is in the list
	  //                       and in this case remove it from the list.
	}
      }
    }
  }
}


//=======================================================================
//function : TangentZoneValue
//purpose  : 
//=======================================================================

Standard_Boolean IntPatch_InterferencePolyhedron::TangentZoneValue
  (Intf_TangentZone& TheTZ,
   const IntPatch_Polyhedron& FirstPol,
   const Standard_Integer Tri1,
   const IntPatch_Polyhedron& SeconPol,
   const Standard_Integer Tri2) const
{
  // Potential tangent Zone !
  // ------------------------

  Standard_Boolean finished=Standard_False;
  Standard_Integer nob, nou, nob2, nou2;
  Standard_Real par;

  Intf_PIType tOP[3];
  Intf_PIType tTP[3];
  for (nou=0; nou<3; nou++) {
    tOP[nou]= Intf_EXTERNAL;
    tTP[nou]= Intf_EXTERNAL;
  }

  Standard_Integer nbpInt=0;
  Intf_SeqOfSectionPoint Tpi;

  // Compute the positions of the points of <Tri1> in the triangle <Tri2>.
  for (nob=0; nob<=2; nob++) {
    nob2=Pourcent3[nob+1];
    for (nou=0; nou<=2; nou++) {
      nou2=Pourcent3[nou+1];
      if (dpOpT[nob][nou]<=Tolerance) {
	Tpi.Append(Intf_SectionPoint(IntPatch_PolyhedronTool::Point(FirstPol, OI[nob]), 
				     Intf_VERTEX, OI[nob], 0, 0., 
				     Intf_VERTEX, TI[nou], 0, 0.,
				     1.));
	tOP[nob]=Intf_VERTEX;
	tTP[nou]=Intf_VERTEX;
	nbpInt++;
	break;
      }
      else if (Abs(dpOeT[nob][nou])<=Tolerance) {
	if (dpOpT[nob][nou]+dpOpT[nob][nou2]<vtt[nou].Modulus()) {
	  par=dpOpT[nob][nou]/(dpOpT[nob][nou]+dpOpT[nob][nou2]); 
	  if (TI[nou]>TI[nou2]) par=1.-par;
	  Tpi.Append(Intf_SectionPoint (IntPatch_PolyhedronTool::Point(FirstPol, OI[nob]), 
					Intf_VERTEX, OI[nob], 0, 0., 
					Intf_EDGE, Min(TI[nou], TI[nou2]),
					Max(TI[nou], TI[nou2]), par,
					1.));
	  tOP[nob]=Intf_EDGE;
	  nbpInt++;
	  break;
	}
      }
    }
    if (tOP[nob]==Intf_EXTERNAL) {
      if (Intf::Contain(IntPatch_PolyhedronTool::Point(SeconPol, TI[0]),
			IntPatch_PolyhedronTool::Point(SeconPol, TI[1]),
			IntPatch_PolyhedronTool::Point(SeconPol, TI[2]),
			IntPatch_PolyhedronTool::Point(FirstPol, OI[nob]))) {
	Tpi.Append(Intf_SectionPoint(IntPatch_PolyhedronTool::Point(FirstPol, OI[nob]),
				     Intf_VERTEX, OI[nob],  0, 0., 
				     Intf_FACE,   Tri2, 0, 0.,
				     1.));
	tOP[nob]=Intf_FACE;
	nbpInt++;
      }
    }
  }

  // If the three points of <Tri1> are in <Tri2> the triangle Tri1 is 
  // itself the tangent zone else compute the positions of the points
  // of <Tri2> in <Tri1>.
  if (nbpInt < 3) {
    for (nou=0; nou<=2; nou++) {
      nou2=Pourcent3[nou+1];
      if (tTP[nou]==Intf_EXTERNAL) {
	for (nob=0; nob<=2; nob++) {
	  nob2=Pourcent3[nob+1];
	  if (Abs(deOpT[nob][nou])<=Tolerance) {
	    if (dpOpT[nob][nou]+dpOpT[nob2][nou]<voo[nob].Modulus()) {
	      par=dpOpT[nob][nou]/(dpOpT[nob][nou]+dpOpT[nob2][nou]); 
	      if (OI[nob]>OI[nob2]) par=1.-par;
	      Tpi.Append(Intf_SectionPoint(IntPatch_PolyhedronTool::Point(SeconPol,TI[nou]), 
					   Intf_EDGE, Min(OI[nob], OI[nob2]),
					   Max(OI[nob], OI[nob2]), par, 
					   Intf_VERTEX, TI[nou], 0, 0., 1.));
	      tTP[nou]=Intf_EDGE;
	      nbpInt++;
	      break;
	    }
	  }
	}
	if (tTP[nou]==Intf_EXTERNAL) {
	  if (Intf::Contain(IntPatch_PolyhedronTool::Point(FirstPol, OI[0]),
			    IntPatch_PolyhedronTool::Point(FirstPol, OI[1]),
			    IntPatch_PolyhedronTool::Point(FirstPol, OI[2]),
			    IntPatch_PolyhedronTool::Point(SeconPol, TI[nou]))) {
	    Tpi.Append(Intf_SectionPoint(IntPatch_PolyhedronTool::Point(SeconPol, TI[nou]),
					 Intf_FACE,   Tri1, 0, 0.,
					 Intf_VERTEX, TI[nou], 0, 0., 
					 1.));
	    tTP[nou]=Intf_FACE;
	    nbpInt++;
	  }
	}
      }
    }
    if (tTP[0]!=Intf_EXTERNAL && 
	tTP[1]!=Intf_EXTERNAL && 
	tTP[2]!=Intf_EXTERNAL)
      finished=Standard_True;
  }
  else
    finished=Standard_True;

  // Insertion of the points of intersection in the zone of tangency :
  for (nob=1; nob<=nbpInt; nob++) 
    TheTZ.Append(Tpi(nob));

  if (!finished) {
    // If one of the triangles is not in the zone of tangency, it is necessary to find
    // the points of intersection edge/edge :

    // Last indexes are not used.
    // Arrays are increased to eliminate gcc warning.
    Standard_Real parO[10], parT[10];
    Standard_Integer nbNoInserted=0;
    Standard_Integer piToInsert[10];

    for (nob=0; nob<3; nob++) {
      //processing of the object segment P[nob], P[nob+1]
      nob2=Pourcent3[nob+1];

      for (nou=0; nou<3; nou++) {
	//processing of the segment of the tool P[nou], P[nou+1]
	nou2=Pourcent3[nou+1];
	
	if (dpOeT[nob][nou]*dpOeT[nob2][nou]<0. &&
	    deOpT[nob][nou]*deOpT[nob][nou2]<0.) {

	  if (nbpInt>=5) {
	    break;
	  }
	  else {
	    parO[nbpInt]=dpOeT[nob][nou]/(dpOeT[nob][nou]-dpOeT[nob2][nou]);
	    parT[nbpInt]=deOpT[nob][nou]/(deOpT[nob][nou]-deOpT[nob][nou2]);
	    gp_Pnt lepi=IntPatch_PolyhedronTool::Point(SeconPol, TI[nou]).Translated
	      (gp_Vec(vtt[nou]*parT[nbpInt]));
	    if (OI[nob]>OI[nob2]) parO[nbpInt]=1.-parO[nbpInt];
	    if (TI[nou]>TI[nou2]) parT[nbpInt]=1.-parT[nbpInt];
	    Tpi.Append(Intf_SectionPoint(lepi,
					 Intf_EDGE, Min(OI[nob],OI[nob2]),
					 Max(OI[nob],OI[nob2]), parO[nbpInt],
					 Intf_EDGE, Min(TI[nou],TI[nou2]),
					 Max(TI[nou],TI[nou2]), parT[nbpInt],
					 Incidence));
	    nbpInt++;
	    if (!TheTZ.Insert(Tpi(nbpInt))) {
	      piToInsert[nbNoInserted]=nbpInt;
	      nbNoInserted++;
	    }
	  }
	}
      }
      if (nbpInt>=5) break; // Number of pi passed in TZ !
    }
    nob=nbNoInserted-1;
    while (nob>=0) {
      while (!TheTZ.Insert(Tpi(piToInsert[nob]))) {
	nob--;
	if (nob<0) break;
      }
      if (nob>=0) {
	while (nob<nbNoInserted) {
	  piToInsert[nob]=piToInsert[nob+1];
	  nob++;
	}
	nbNoInserted--;
	nob=nbNoInserted-1;
      }
    }
    if (nbNoInserted>0) {
      nob=nbNoInserted-1;
      while (nob>=0) {
	Tpi(piToInsert[nob--]).Dump(4);
      }
    }
  }
  if (nbpInt<3) nbpInt=0;
  return nbpInt>0;
}


//=======================================================================
//function : CoupleCharacteristics
//purpose  : 
//=======================================================================

void IntPatch_InterferencePolyhedron::CoupleCharacteristics (const IntPatch_Polyhedron& FirstPol,
                                                         const IntPatch_Polyhedron& SeconPol)
{
  Standard_Integer n1, n2;
  Standard_Real lg;

  for (n1=0; n1<3; n1++) {
    n2=Pourcent3[n1+1];
    voo[n1]=IntPatch_PolyhedronTool::Point(FirstPol, OI[n2]).XYZ()-
            IntPatch_PolyhedronTool::Point(FirstPol, OI[n1]).XYZ();
    vtt[n1]=IntPatch_PolyhedronTool::Point(SeconPol, TI[n2]).XYZ()-
            IntPatch_PolyhedronTool::Point(SeconPol, TI[n1]).XYZ();
  }

  gp_XYZ vvec=(voo[0]^voo[1])+(voo[1]^voo[2])+(voo[2]^voo[0]);
  gp_XYZ vnorT=(vtt[0]^vtt[1])+(vtt[1]^vtt[2])+(vtt[2]^vtt[0]);
  if (vnorT.Modulus()>vvec.Modulus())
    vvec=vnorT;

  for (n1=0; n1<3; n1++) {

    for (n2=0; n2<3; n2++) {

      gp_XYZ vto=IntPatch_PolyhedronTool::Point(FirstPol, OI[n1]).XYZ()-
                 IntPatch_PolyhedronTool::Point(SeconPol, TI[n2]).XYZ();
      dpOpT[n1][n2]=vto.Modulus();

      lg=vtt[n2].Modulus();
      if (lg > 1e-16) { //-- RealEpsilon()
	gp_XYZ vv=vto^vtt[n2];
	lg=(vvec*vv)>0.0 ? lg : -lg;
        dpOeT[n1][n2]=vv.Modulus()/lg;
      }
      else
	dpOeT[n1][n2]=dpOpT[n1][n2];

      lg=voo[n1].Modulus();
      if (lg > 1e-16) { //-- RealEpsilon())
	gp_XYZ vv=vto^voo[n1];
	lg=(vvec*vv)>0.0 ? -lg : lg;
	deOpT[n1][n2]=vv.Modulus()/lg;
      }
      else
	deOpT[n1][n2]=dpOpT[n1][n2];
    }
  }
}
