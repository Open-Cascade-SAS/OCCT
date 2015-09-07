// Created on: 1996-07-15
// Created by: Laurent BUCHARD
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

//  Modified by skv - Thu Sep  4 11:22:05 2003 OCC578

#include <BRep_Tool.hxx>
#include <BRepClass3d_Intersector3d.hxx>
#include <BRepClass3d_SClassifier.hxx>
#include <BRepClass3d_SolidExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElCLib.hxx>
#include <Geom_Surface.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntCurvesFace_Intersector.hxx>
#include <math_BullardGenerator.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <vector>

// modified by NIZHNY-MKK  Mon Jun 21 15:13:40 2004
static
  Standard_Boolean FaceNormal (const TopoDS_Face& aF,
                               const Standard_Real U,
                               const Standard_Real V,
                               gp_Dir& aDN);

static 
  Standard_Real GetAddToParam(const gp_Lin& L,const Standard_Real P,const Bnd_Box& B);



//=======================================================================
//function : BRepClass3d_SClassifier
//purpose  : 
//=======================================================================
BRepClass3d_SClassifier::BRepClass3d_SClassifier() 
{ 
}


//=======================================================================
//function : BRepClass3d_SClassifier
//purpose  : 
//=======================================================================
BRepClass3d_SClassifier::BRepClass3d_SClassifier(BRepClass3d_SolidExplorer& S,
						 const gp_Pnt&  P,
						 const Standard_Real Tol) { 
  if(S.Reject(P)) { 
    myState=3; //-- in ds solid case without face 
  }
  else { 
    Perform(S,P,Tol);
  }
}


//=======================================================================
//function : PerformInfinitePoint
//purpose  : 
//=======================================================================
void BRepClass3d_SClassifier::PerformInfinitePoint(BRepClass3d_SolidExplorer& aSE,
						   const Standard_Real /*Tol*/) {

  //Take a normal to the first extracted face in its random inner point
  //and intersect this reversed normal with the faces of the solid.
  //If the min.par.-intersection point is
  // a) inner point of a face
  // b) transition is not TANGENT
  //    (the line does not touch the face but pierces it)
  //then set <myState> to IN or OUT according to transition
  //else take the next random point inside the min.par.-intersected face
  //and continue
  
  if(aSE.Reject(gp_Pnt(0,0,0))) { 
    myState=3; //-- in ds solid case without face 
    return;
  }
  //
  //------------------------------------------------------------
  // 1
  Standard_Boolean bFound;
  Standard_Real aParam, aU = 0., aV = 0.;
  gp_Pnt aPoint;
  gp_Dir aDN;

  math_BullardGenerator aRandomGenerator;
  myFace.Nullify();
  myState=2;

  // Collect faces in sequence to iterate
  std::vector<TopoDS_Face> aFaces;
  for (aSE.InitShell(); aSE.MoreShell(); aSE.NextShell())
  {
    for (aSE.InitFace(); aSE.MoreFace(); aSE.NextFace())
    {
      aFaces.push_back (aSE.CurrentFace());
    }
  }

  // iteratively try up to 10 probing points from each face
  const int NB_MAX_POINTS_PER_FACE = 10;
  for (int itry = 0; itry < NB_MAX_POINTS_PER_FACE; itry++)
  {
    for (std::vector<TopoDS_Face>::iterator iFace = aFaces.begin(); iFace != aFaces.end(); ++iFace)
    {
      TopoDS_Face aF = *iFace;

      TopAbs_State aState = TopAbs_OUT;
      IntCurveSurface_TransitionOnCurve aTransition = IntCurveSurface_Tangent;

      aParam = 0.1 + 0.8 * aRandomGenerator.NextReal(); // random number in range [0.1, 0.9]
      bFound = aSE.FindAPointInTheFace(aF, aPoint, aU, aV, aParam);
      if (!bFound || !FaceNormal(aF, aU, aV, aDN))
        continue;

      gp_Lin aLin(aPoint, -aDN);
      Standard_Real parmin = RealLast();
      for (aSE.InitShell();aSE.MoreShell();aSE.NextShell()) { 
        if (aSE.RejectShell(aLin) == Standard_False) { 
          for (aSE.InitFace();aSE.MoreFace(); aSE.NextFace()) {
            if (aSE.RejectFace(aLin) == Standard_False) { 
              TopoDS_Shape aLocalShape = aSE.CurrentFace();
              TopoDS_Face CurFace = TopoDS::Face(aLocalShape);
              IntCurvesFace_Intersector& Intersector3d = aSE.Intersector(CurFace);
              Intersector3d.Perform(aLin,-RealLast(),parmin); 

              if(Intersector3d.IsDone()) {
                if(Intersector3d.NbPnt()) { 
                  Standard_Integer imin = 1;
                  for (Standard_Integer i = 2; i <= Intersector3d.NbPnt(); i++)
                    if (Intersector3d.WParameter(i) < Intersector3d.WParameter(imin))
                      imin = i;
                  parmin = Intersector3d.WParameter(imin);
                  aState = Intersector3d.State(imin);
                  aTransition = Intersector3d.Transition(imin);
                }
              }
            }
          }
        }
        else
          myState = 1;
      } //end of loop on the whole solid
        
      if (aState == TopAbs_IN)
      {
        if (aTransition == IntCurveSurface_Out) { 
          //-- The line is going from inside the solid to outside 
          //-- the solid.
          myState = 3; //-- IN --
          return;
        }
        else if (aTransition == IntCurveSurface_In) { 
          myState = 4; //-- OUT --
          return;
        }
      }
    } // iteration by faces
  } // iteration by points
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepClass3d_SClassifier::Perform(BRepClass3d_SolidExplorer& SolidExplorer,
				      const gp_Pnt&  P,
				      const Standard_Real Tol) 
{ 


  if(SolidExplorer.Reject(P)) { 
    myState=3; //-- in ds solid case without face 
    return;
  }


  myFace.Nullify();
  myState = 0;
  if(SolidExplorer.Reject(P) == Standard_False) { 
    gp_Lin L;
    Standard_Real Par;
    //-- We compute the intersection betwwen the line builded in the Solid Explorer
    //-- and the shape.

    //-- --------------------------------------------------------------------------------
    //-- Calculate intersection with the face closest to the direction of bounding boxes 
    //-- by priority so that to have the smallest possible parmin. 
    //-- optimization to produce as much as possible rejections with other faces. 
    Standard_Integer iFlag;
    //

    //  Modified by skv - Thu Sep  4 11:22:05 2003 OCC578 Begin
    //  If found line passes through a bound of any face, it means that the line
    //  is not found properly and it is necessary to repeat whole procedure.
    //  That's why the main loop while is added.
    Standard_Boolean isFaultyLine = Standard_True;
    Standard_Integer anIndFace    = 0;
    Standard_Real    parmin = 0.;

    while (isFaultyLine) {
      if (anIndFace == 0) {
	iFlag = SolidExplorer.Segment(P,L,Par);
      } else {
	iFlag = SolidExplorer.OtherSegment(P,L,Par);
      }

      Standard_Integer aCurInd = SolidExplorer.GetFaceSegmentIndex();

      if (aCurInd > anIndFace) {
	anIndFace = aCurInd;
      } else {
	myState = 1;

	return;
      }
      //  Modified by skv - Thu Sep  4 11:22:10 2003 OCC578 End

      if (iFlag==1) {
	// IsOnFace
	// iFlag==1 i.e face is Infinite
	myState=2; 

	return;
      }
      //SolidExplorer.Segment(P,L,Par);
      //
      //process results from uncorrected shells
      //
      //if(Par > 1.e+100 && L.Direction().IsParallel(gp_Dir(0.,0.,1.),1.e-8)) {
      if (iFlag==2) {
	myState = 4;
	return;
      }
      //-- BRepClass3d_Intersector3d Intersector3d;
    
      //  Modified by skv - Thu Sep  4 13:48:27 2003 OCC578 Begin
      // Check if the point is ON surface but OUT of the face. 
      // Just skip this face because it is bad for classification.
      if (iFlag == 3)
	continue;

      isFaultyLine = Standard_False;
//       Standard_Real parmin = RealLast();

//       for(SolidExplorer.InitShell();
// 	  SolidExplorer.MoreShell();
// 	  SolidExplorer.NextShell()) { 
      parmin = RealLast();

      for(SolidExplorer.InitShell();
	  SolidExplorer.MoreShell() && !isFaultyLine;
	  SolidExplorer.NextShell()) { 
//  Modified by skv - Thu Sep  4 13:48:27 2003 OCC578 End

	if(SolidExplorer.RejectShell(L) == Standard_False) { 

//  Modified by skv - Thu Sep  4 13:48:27 2003 OCC578 Begin
// 	  for(SolidExplorer.InitFace(); 
// 	      SolidExplorer.MoreFace(); 
// 	      SolidExplorer.NextFace()) {
	  for(SolidExplorer.InitFace(); 
	      SolidExplorer.MoreFace() && !isFaultyLine; 
	      SolidExplorer.NextFace()) {
//  Modified by skv - Thu Sep  4 13:48:27 2003 OCC578 End
	  
	    if(SolidExplorer.RejectFace(L) == Standard_False) { 
	    
	      //-- Intersector3d.Perform(L,Par,Tol,SolidExplorer.CurrentFace());
	      TopoDS_Shape aLocalShape = SolidExplorer.CurrentFace();
	      TopoDS_Face f = TopoDS::Face(aLocalShape);
	      //	    TopoDS_Face f = TopoDS::Face(SolidExplorer.CurrentFace());
	      IntCurvesFace_Intersector& Intersector3d = SolidExplorer.Intersector(f);

	      // MSV Oct 25, 2001: prolong segment, since there are cases when
	      // the intersector does not find intersection points with the original
	      // segment due to rough triangulation of a parametrized surface
	      Standard_Real addW = Max(10*Tol, 0.01*Par);
              Standard_Real AddW = addW;

	      Bnd_Box aBoxF = Intersector3d.Bounding();

              // MSV 23.09.2004: the box must be finite in order to
              // correctly prolong the segment to its bounds
              if (!aBoxF.IsVoid() && !aBoxF.IsWhole()) {
                Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
                aBoxF.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

                Standard_Real boxaddW = GetAddToParam(L,Par,aBoxF);
                addW = Max(addW,boxaddW);
              }

	      Standard_Real minW = -AddW;//-addW;
	      Standard_Real maxW = Min(Par*10,Par+addW);//Par+addW;
              //cout << "range [" << minW << "," << maxW << "]" << endl << endl;
	      Intersector3d.Perform(L,minW,maxW);
	      //Intersector3d.Perform(L,-Tol,Par+10.0*Tol);
	      if(Intersector3d.IsDone()) { 
		Standard_Integer i;
		for (i=1; i <= Intersector3d.NbPnt(); i++) { 
		  if(Abs(Intersector3d.WParameter(i)) < Abs(parmin) - Precision::PConfusion()) {
 
		    parmin = Intersector3d.WParameter(i);
		    //  Modified by skv - Thu Sep  4 12:46:32 2003 OCC578 Begin
		    TopAbs_State aState = Intersector3d.State(i);
		    //  Modified by skv - Thu Sep  4 12:46:33 2003 OCC578 End
		    if(Abs(parmin)<=Tol) { 
		      myState = 2;
		      myFace  = f;
		    }
		    //  Modified by skv - Thu Sep  4 12:46:32 2003 OCC578 Begin
		    //  Treatment of case TopAbs_ON separately.

		    else if(aState==TopAbs_IN) { 
		    //  Modified by skv - Thu Sep  4 12:46:32 2003 OCC578 End

		      //-- The intersection point between the line and a face F 
		      // -- of the solid is in the face F 

		      IntCurveSurface_TransitionOnCurve tran = Intersector3d.Transition(i);
		      if (tran == IntCurveSurface_Tangent) {
#ifdef OCCT_DEBUG
			cout<<"*Problem ds BRepClass3d_SClassifier.cxx"<<endl;
#endif
			continue; // ignore this point
		      }
		      // if parmin is negative we should reverse transition
		      if (parmin < 0)
			tran = (tran == IntCurveSurface_Out 
				? IntCurveSurface_In : IntCurveSurface_Out);
		      if(tran == IntCurveSurface_Out) { 
			//-- The line is going from inside the solid to outside 
			//-- the solid.
			myState = 3; //-- IN --
		      }
		      else /* if(tran == IntCurveSurface_In) */ { 
			myState = 4; //-- OUT --
		      }
		      myFace  = f;
		    }
		    //  Modified by skv - Thu Sep  4 12:48:50 2003 OCC578 Begin
		    // If the state is TopAbs_ON, it is necessary to chose
		    // another line and to repeat the whole procedure.
		    else if(aState==TopAbs_ON) {
		      isFaultyLine = Standard_True;

		      break;
		    }
		    //  Modified by skv - Thu Sep  4 12:48:50 2003 OCC578 End
		  }
		  else { 
		    //-- No point has been found by the Intersector3d.
		    //-- Or a Point has been found with a greater parameter.
		  }
		} //-- loop by intersection points
	      } //-- Face has not been rejected
	      else { 
		myState = 1;
	      }
	    }
	  } //-- Exploration of the faces
	} //-- Shell has not been rejected
	else { 
	  myState=1; 
	}
      } //-- Exploration of the shells

      //  Modified by skv - Thu Sep  4 11:42:03 2003 OCC578 Begin
      // The end of main loop.
    }
    //  Modified by skv - Thu Sep  4 11:42:03 2003 OCC578 End

#ifdef OCCT_DEBUG
    //#################################################
    SolidExplorer.DumpSegment(P,L,parmin,State());
    //#################################################
#endif

  } //-- Solid has not been rejected
  else { 
    myState = 1;
  }
}


TopAbs_State BRepClass3d_SClassifier::State() const { 
  if(myState==2)  return(TopAbs_ON);
  if(myState==4)        return(TopAbs_OUT);          //--
  else if(myState==3)   return(TopAbs_IN);           //-- 
  return(TopAbs_OUT);             
}

TopoDS_Face BRepClass3d_SClassifier::Face() const {  
  return(myFace);
}

Standard_Boolean BRepClass3d_SClassifier::Rejected() const { 
  return(myState==1); 
}

  
Standard_Boolean BRepClass3d_SClassifier::IsOnAFace() const { 
  return(myState==2);
}


void BRepClass3d_SClassifier::ForceIn() {
  myState=3;
}

void BRepClass3d_SClassifier::ForceOut() { 
  myState=4;
}

Standard_Real GetAddToParam(const gp_Lin&       L,
                            const Standard_Real P,
                            const Bnd_Box&      B)
{
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  Standard_Real x[2] = {aXmin,aXmax}, y[2] = {aYmin,aYmax}, z[2] = {aZmin,aZmax};
  Standard_Integer i = 0, j = 0, k = 0;
  Standard_Real Par = P;
  for(i = 0 ; i < 2; i++) {
    for(j = 0; j < 2; j++) {
      for(k = 0; k < 2; k++) {
        Standard_Real X = fabs(x[i]-L.Location().X());
        Standard_Real Y = fabs(y[j]-L.Location().Y());
        Standard_Real Z = fabs(z[k]-L.Location().Z());
        if(X < 1.e+20 && Y < 1.e+20 && Z < 1.e+20) {
          gp_Pnt aP(x[i],y[j],z[k]);
          Standard_Real par = ElCLib::Parameter(L,aP);
          if(par > Par)
            Par = par;
        }
        else
          return 1.e+20;
      }
    }
  }
  return Par - P;
}
//=======================================================================
//function : FaceNormal
//purpose  : 
//=======================================================================
Standard_Boolean FaceNormal (const TopoDS_Face& aF,
                             const Standard_Real U,
                             const Standard_Real V,
                             gp_Dir& aDN)
{
  gp_Pnt aPnt ;
  gp_Vec aD1U, aD1V, aN;
  Handle(Geom_Surface) aS;

  aS=BRep_Tool::Surface(aF);
  aS->D1 (U, V, aPnt, aD1U, aD1V);
  aN=aD1U.Crossed(aD1V);
  if (aN.Magnitude() <= gp::Resolution())
    return Standard_False;
  
  aN.Normalize();  
  aDN.SetXYZ(aN.XYZ());
  if (aF.Orientation() == TopAbs_REVERSED){
    aDN.Reverse();
  }
  return Standard_True;
}
