// Copyright (c) 1995-1999 Matra Datavision
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

//  Modified by skv - Thu Jul  7 14:37:05 2005 OCC9134

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtElC.hxx>
#include <Extrema_ExtElCS.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Extrema_ExtPElS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Vec.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_ListOfInteger.hxx>

Extrema_ExtElCS::Extrema_ExtElCS() 
{
  myDone = Standard_False;
  myIsPar = Standard_False;
  myNbExt = 0;
}


Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Pln& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Lin& C,
			      const gp_Pln& S)
{
  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;

  if (C.Direction().IsNormal(S.Axis().Direction(),
                            Precision::Angular()))
  {
    mySqDist = new TColStd_HArray1OfReal(1, 1);
    mySqDist->SetValue(1, S.SquareDistance(C));
    myIsPar = Standard_True;
    myNbExt = 1;
  }
}


Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Cylinder& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Lin& C,
                              const gp_Cylinder& S)
{
  myDone = Standard_False;
  myNbExt = 0;
  myIsPar = Standard_False;

  gp_Ax3 Pos = S.Position();

  Standard_Boolean isParallel = Standard_False;

  Standard_Real radius = S.Radius();
  Extrema_ExtElC Extrem(gp_Lin(Pos.Axis()), C, Precision::Angular());
  if (Extrem.IsParallel())
  {
    isParallel = Standard_True;
  }
  else
  {
    Standard_Integer i, aStartIdx = 0;

    Extrema_POnCurv myPOnC1, myPOnC2;
    Extrem.Points(1, myPOnC1, myPOnC2);
    gp_Pnt PonAxis = myPOnC1.Value();
    gp_Pnt PC = myPOnC2.Value();

    // line intersects the cylinder
    if (radius - PonAxis.Distance(PC) > Precision::PConfusion())
    {
      IntAna_Quadric theQuadric(S);
      IntAna_IntConicQuad Inters(C, theQuadric);
      if (Inters.IsDone() && Inters.IsInQuadric())
      {
        isParallel = Standard_True;
      }
      else if (Inters.IsDone())
      {
        myNbExt = Inters.NbPoints();
        aStartIdx = myNbExt;
        if (myNbExt > 0)
        {
          // Not more than 2 additional points from perpendiculars.
          mySqDist = new TColStd_HArray1OfReal(1, myNbExt + 2);
          myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt + 2);
          myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt + 2);
          Standard_Real u, v, w;
          for (i = 1; i <= myNbExt; i++)
          {
            mySqDist->SetValue(i, 0.);
            gp_Pnt P_int = Inters.Point(i);
            w = Inters.ParamOnConic(i);
            Extrema_POnCurv PonC(w, P_int);
            myPoint1->SetValue(i, PonC);
            ElSLib::CylinderParameters(Pos, radius, P_int, u, v);
            Extrema_POnSurf PonS(u, v, P_int);
            myPoint2->SetValue(i, PonS);
          }
        }
      }
    }
    else
    {
      // line is tangent or outside of the cylinder
      Extrema_ExtPElS ExPS(PC, S, Precision::Confusion());
      if (ExPS.IsDone())
      {
        if (aStartIdx == 0)
        {
          myNbExt = ExPS.NbExt();
          mySqDist = new TColStd_HArray1OfReal(1, myNbExt);
          myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
          myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);
        }
        else
          myNbExt += ExPS.NbExt();

        for (i = aStartIdx + 1; i <= myNbExt; i++)
        {
          myPoint1->SetValue(i, myPOnC2);
          myPoint2->SetValue(i, ExPS.Point(i - aStartIdx));
          mySqDist->SetValue(i, (myPOnC2.Value()).SquareDistance(ExPS.Point(i - aStartIdx).Value()));
        }
      }
    }
    
    myDone = Standard_True;
  }

  if (isParallel)
  {
    // Line direction is similar to cylinder axis of rotation.
    // The case is possible when either extrema returned parallel status
    // or Intersection tool returned infinite number of solutions.
    // This is possible due to Intersection algorithm uses more precise
    // characteristics to consider given geometries parallel.
    // In the latter case there may be several extremas, thus we look for
    // the one with the lowest distance and use it as a final solution.
    mySqDist = new TColStd_HArray1OfReal(1, 1);
    Standard_Real aDist = Extrem.SquareDistance(1);
    const Standard_Integer aNbExt = Extrem.NbExt();
    for (Standard_Integer i = 2; i <= aNbExt; i++)
    {
      const Standard_Real aD = Extrem.SquareDistance(i);
      if (aD < aDist)
      {
        aDist = aD;
      }
    }
      
    aDist = sqrt(aDist) - radius;
    mySqDist->SetValue(1, aDist * aDist);
    myDone = Standard_True;
    myIsPar = Standard_True;
    myNbExt = 1;
  }
}



Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Cone& S)
{  Perform(C, S);}



//void Extrema_ExtElCS::Perform(const gp_Lin& C,
//			      const gp_Cone& S)
void Extrema_ExtElCS::Perform(const gp_Lin& ,
			      const gp_Cone& )
{
  throw Standard_NotImplemented();

}



Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Sphere& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Lin& C,
                              const gp_Sphere& S)
{
  // In case of intersection - return four points:
  // 2 intersection points and 2 perpendicular.
  // No intersection - only min and max.

  myDone = Standard_False;
  myNbExt = 0;
  myIsPar = Standard_False;
  Standard_Integer aStartIdx = 0;

  gp_Pnt aCenter = S.Location();

  Extrema_ExtPElC Extrem(aCenter, C, Precision::Angular(), RealFirst(), RealLast());

  Standard_Integer i;
  if (Extrem.IsDone() &&
      Extrem.NbExt() > 0)
  {
    Extrema_POnCurv myPOnC1 =  Extrem.Point(1);
    if (myPOnC1.Value().Distance(aCenter) <= S.Radius())
    {
      IntAna_IntConicQuad aLinSphere(C, S);
      if (aLinSphere.IsDone())
      {
        myNbExt = aLinSphere.NbPoints();
        aStartIdx = myNbExt;
        // Not more than 2 additional points from perpendiculars.
        mySqDist = new TColStd_HArray1OfReal(1, myNbExt + 2);
        myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt + 2);
        myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt + 2);

        for (i = 1; i <= myNbExt; i++)
        {
          Extrema_POnCurv aCPnt(aLinSphere.ParamOnConic(i), aLinSphere.Point(i));

          Standard_Real u,v;
          ElSLib::Parameters(S, aLinSphere.Point(i), u, v);
          Extrema_POnSurf aSPnt(u, v, aLinSphere.Point(i));

          myPoint1->SetValue(i, aCPnt);
          myPoint2->SetValue(i, aSPnt);
          mySqDist->SetValue(i,(aCPnt.Value()).SquareDistance(aSPnt.Value()));
        }
      }
    }

    Extrema_ExtPElS ExPS(myPOnC1.Value(), S, Precision::Confusion());
    if (ExPS.IsDone())
    {
      if (aStartIdx == 0)
      {
        myNbExt = ExPS.NbExt();
        mySqDist = new TColStd_HArray1OfReal(1, myNbExt);
        myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
        myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);
      }
      else
        myNbExt += ExPS.NbExt();

      for (i = aStartIdx + 1; i <= myNbExt; i++)
      {
        myPoint1->SetValue(i, myPOnC1);
        myPoint2->SetValue(i, ExPS.Point(i - aStartIdx));
        mySqDist->SetValue(i,(myPOnC1.Value()).SquareDistance(ExPS.Point(i - aStartIdx).Value()));
      }
    }
  }
  myDone = Standard_True;
}


Extrema_ExtElCS::Extrema_ExtElCS(const gp_Lin& C,
				 const gp_Torus& S)
{  Perform(C, S);}



//void Extrema_ExtElCS::Perform(const gp_Lin& C,
//			      const gp_Torus& S)
void Extrema_ExtElCS::Perform(const gp_Lin& ,
			      const gp_Torus& )
{
  throw Standard_NotImplemented();

}


//        Circle-?

Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Pln& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Circ& C,
                              const gp_Pln& S)
{
  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;

  gp_Ax2 Pos = C.Position();
  gp_Dir NCirc = Pos.Direction();
  gp_Dir NPln = S.Axis().Direction();

  Standard_Boolean isParallel = Standard_False;

  if (NCirc.IsParallel(NPln, Precision::Angular())) {
    isParallel = Standard_True;
  }
  else {

    gp_Dir ExtLine = NCirc ^ NPln;
    ExtLine = ExtLine ^ NCirc;
    //
    gp_Dir XDir = Pos.XDirection();
    Standard_Real T[2];
    T[0] = XDir.AngleWithRef(ExtLine, NCirc);
    if(T[0] < 0.)
    {
      //Put in period
      T[0] += M_PI;
    }
    T[1] = T[0] + M_PI;
    //
    myNbExt = 2;
    //Check intersection 
    IntAna_IntConicQuad anInter(C, S,
                                Precision::Angular(),
                                Precision::Confusion());

    if (anInter.IsDone() && anInter.IsInQuadric())
    {
      isParallel = Standard_True;
    }
    else if (anInter.IsDone())
    {
      if(anInter.NbPoints() > 1)
      {
        myNbExt += anInter.NbPoints();
      }
    }

    if (!isParallel)
    {
      myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
      mySqDist = new TColStd_HArray1OfReal(1, myNbExt);
      myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);

      Standard_Integer i;
      gp_Pnt PC, PP;
      Standard_Real U, V;
      Extrema_POnCurv POnC;
      Extrema_POnSurf POnS;
      for (i = 0; i < 2; ++i)
      {
        PC = ElCLib::CircleValue(T[i], C.Position(), C.Radius());
        POnC.SetValues(T[i], PC);
        myPoint1->SetValue(i + 1, POnC);
        ElSLib::PlaneParameters(S.Position(), PC, U, V);
        PP = ElSLib::PlaneValue(U, V, S.Position());
        POnS.SetParameters(U, V, PP);
        myPoint2->SetValue(i + 1, POnS);
        mySqDist->SetValue(i + 1, PC.SquareDistance(PP));
      }
      //
      if (myNbExt > 2)
      {
        //Add intersection points
        for (i = 1; i <= anInter.NbPoints(); ++i)
        {
          Standard_Real t = anInter.ParamOnConic(i);
          PC = ElCLib::CircleValue(t, C.Position(), C.Radius());
          POnC.SetValues(t, PC);
          myPoint1->SetValue(i + 2, POnC);
          ElSLib::PlaneParameters(S.Position(), PC, U, V);
          PP = ElSLib::PlaneValue(U, V, S.Position());
          POnS.SetParameters(U, V, PP);
          myPoint2->SetValue(i + 2, POnS);
          mySqDist->SetValue(i + 2, PC.SquareDistance(PP));
        }
      }
    }
  }
  
  if (isParallel)
  {
    mySqDist = new TColStd_HArray1OfReal(1, 1);
    mySqDist->SetValue(1, S.SquareDistance(C.Location()));
    myIsPar = Standard_True;
    myNbExt = 1;
  }
}

Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Cylinder& S)
{
  Perform(C, S);
}



//  Modified by skv - Thu Jul  7 14:37:05 2005 OCC9134 Begin
// Implementation of the method.
void Extrema_ExtElCS::Perform(const gp_Circ& C,
			      const gp_Cylinder& S)
{
  myDone  = Standard_False;
  myIsPar = Standard_False;
  myNbExt = 0;

  // Get an axis line of the cylinder.
  gp_Lin anAxis(S.Axis());

  // Compute extrema between the circle and the line.
  Extrema_ExtElC anExtC(anAxis, C, 0.);

  if (!anExtC.IsDone())
    return;

  Standard_Boolean isParallel = Standard_False;

  if (anExtC.IsParallel()) {
    isParallel = Standard_True;
  } else {
    Standard_Integer aNbExt   = anExtC.NbExt();
    Standard_Integer i;
    Standard_Integer aCurI    = 1;
    constexpr Standard_Real aTolConf = Precision::Confusion();
    Standard_Real    aCylRad  = S.Radius();

    // Check whether two objects have intersection points
    IntAna_Quadric aCylQuad(S);
    IntAna_IntConicQuad aCircCylInter(C, aCylQuad);
    Standard_Integer aNbInter = 0;
    if (aCircCylInter.IsDone() && aCircCylInter.IsInQuadric())
    {
      isParallel = Standard_True;
    }
    else if (aCircCylInter.IsDone())
    {
      aNbInter = aCircCylInter.NbPoints();
    }

    if (!isParallel)
    {
      // Compute the extremas.
      myNbExt = 2 * aNbExt + aNbInter;
      mySqDist = new TColStd_HArray1OfReal(1, myNbExt);
      myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
      myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);

      for (i = 1; i <= aNbExt; i++) {
        Extrema_POnCurv aPOnAxis;
        Extrema_POnCurv aPOnCirc;
        Standard_Real   aSqDist = anExtC.SquareDistance(i);
        Standard_Real   aDist = sqrt(aSqDist);

        anExtC.Points(i, aPOnAxis, aPOnCirc);

        if (aSqDist <= (aTolConf * aTolConf)) {
          myNbExt -= 2;
          continue;
        }

        gp_Dir aDir(aPOnAxis.Value().XYZ().Subtracted(aPOnCirc.Value().XYZ()));
        Standard_Real aShift[2] = {aDist + aCylRad, aDist - aCylRad};
        Standard_Integer j;

        for (j = 0; j < 2; j++) {
          gp_Vec aVec(aDir);
          gp_Pnt aPntOnCyl;

          aVec.Multiply(aShift[j]);
          aPntOnCyl = aPOnCirc.Value().Translated(aVec);

          Standard_Real aU;
          Standard_Real aV;

          ElSLib::Parameters(S, aPntOnCyl, aU, aV);

          Extrema_POnSurf aPOnSurf(aU, aV, aPntOnCyl);

          myPoint1->SetValue(aCurI, aPOnCirc);
          myPoint2->SetValue(aCurI, aPOnSurf);
          mySqDist->SetValue(aCurI++, aShift[j] * aShift[j]);
        }
      }

      // Adding intersection points to the list of extremas
      for (i=1; i<=aNbInter; i++)
      {
        Standard_Real aU;
        Standard_Real aV;

        gp_Pnt aInterPnt = aCircCylInter.Point(i);

        aU = ElCLib::Parameter(C, aInterPnt);
        Extrema_POnCurv aPOnCirc(aU, aInterPnt);

        ElSLib::Parameters(S, aInterPnt, aU, aV);
        Extrema_POnSurf aPOnCyl(aU, aV, aInterPnt);
        myPoint1->SetValue(aCurI, aPOnCirc);
        myPoint2->SetValue(aCurI, aPOnCyl);
        mySqDist->SetValue(aCurI++, 0.0);
      }
    }
  }

  myDone = Standard_True;

  if (isParallel)
  {
    // The case is possible when either extrema returned parallel status
    // or Intersection tool returned infinite number of solutions.
    // This is possible due to Intersection algorithm uses more precise
    // characteristics to consider given geometries parallel.
    // In the latter case there may be several extremas, thus we look for
    // the one with the lowest distance and use it as a final solution.

    myIsPar = Standard_True;
    myNbExt = 1;
    mySqDist = new TColStd_HArray1OfReal(1, 1);
    Standard_Real aDist = anExtC.SquareDistance(1);

    const Standard_Integer aNbExt = anExtC.NbExt();
    for (Standard_Integer i = 2; i <= aNbExt; i++)
    {
      const Standard_Real aD = anExtC.SquareDistance(i);
      if (aD < aDist)
      {
        aDist = aD;
      }
    }

    aDist = sqrt(aDist) - S.Radius();
    mySqDist->SetValue(1, aDist * aDist);
  }
}
//  Modified by skv - Thu Jul  7 14:37:05 2005 OCC9134 End



Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Cone& S)
{  Perform(C, S);}



//void Extrema_ExtElCS::Perform(const gp_Circ& C,
//			 const gp_Cone& S)
void Extrema_ExtElCS::Perform(const gp_Circ& ,
			 const gp_Cone& )
{
  throw Standard_NotImplemented();

}



//=======================================================================
//function : Extrema_ExtElCS
//purpose  : Circle/Sphere
//=======================================================================
Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
                                 const gp_Sphere& S)
{
  Perform(C, S);
}

//=======================================================================
//function : Perform
//purpose  : Circle/Sphere
//=======================================================================
void Extrema_ExtElCS::Perform(const gp_Circ& C,
                              const gp_Sphere& S)
{
  myDone = Standard_False;
  myIsPar = Standard_False;
  myNbExt = 0;

  if (gp_Lin(C.Axis()).SquareDistance(S.Location()) < Precision::SquareConfusion())
  {
    // Circle and sphere are parallel
    myIsPar = Standard_True;
    myDone = Standard_True;
    myNbExt = 1;

    // Compute distance from circle to the sphere
    Standard_Real aSqDistLoc = C.Location().SquareDistance(S.Location());
    Standard_Real aSqDist = aSqDistLoc + C.Radius() * C.Radius();
    Standard_Real aDist = sqrt(aSqDist) - S.Radius();
    mySqDist = new TColStd_HArray1OfReal(1, 1);
    mySqDist->SetValue(1, aDist * aDist);
    return;
  }

  // Intersect sphere with circle's plane
  gp_Pln CPln(C.Location(), C.Axis().Direction());
  IntAna_QuadQuadGeo anInter(CPln, S);
  if (!anInter.IsDone())
    // not done
    return;

  if (anInter.TypeInter() != IntAna_Circle)
  {
    // Intersection is empty or just a point.
    // The parallel case has already been considered,
    // thus, here we have to find only one minimal solution
    myNbExt = 1;
    myDone = Standard_True;

    mySqDist = new TColStd_HArray1OfReal(1, 1);
    myPoint1 = new Extrema_HArray1OfPOnCurv(1, 1);
    myPoint2 = new Extrema_HArray1OfPOnSurf(1, 1);

    // Compute parameter on circle
    const Standard_Real aT = ElCLib::Parameter(C, S.Location());
    // Compute point on circle
    gp_Pnt aPOnC = ElCLib::Value(aT, C);

    // Compute parameters on sphere
    Standard_Real aU, aV;
    ElSLib::Parameters(S, aPOnC, aU, aV);
    // Compute point on sphere
    gp_Pnt aPOnS = ElSLib::Value(aU, aV, S);

    // Save solution
    myPoint1->SetValue(1, Extrema_POnCurv(aT, aPOnC));
    myPoint2->SetValue(1, Extrema_POnSurf(aU, aV, aPOnS));
    mySqDist->SetValue(1, aPOnC.SquareDistance(aPOnS));
    return;
  }

  // Here, the intersection is a circle

  // Intersection circle
  gp_Circ aCInt = anInter.Circle(1);

  // Perform intersection of the input circle with the intersection circle
  Extrema_ExtElC anExtC(C, aCInt);
  Standard_Boolean isExtremaCircCircValid =  anExtC.IsDone() // Check if intersection is done
                                          && !anExtC.IsParallel() // Parallel case has already been considered
                                          && anExtC.NbExt() > 0; // Check that some solutions have been found
  if (!isExtremaCircCircValid)
    // not done
    return;

  myDone = Standard_True;

  // Few solutions
  Standard_Real aNbExt = anExtC.NbExt();
  // Find the minimal distance
  Standard_Real aMinSqDist = ::RealLast();
  for (Standard_Integer i = 1; i <= aNbExt; ++i)
  {
    Standard_Real aSqDist = anExtC.SquareDistance(i);
    if (aSqDist < aMinSqDist)
      aMinSqDist = aSqDist;
  }

  // Collect all solutions close to the minimal one
  TColStd_ListOfInteger aSols;
  for (Standard_Integer i = 1; i <= aNbExt; ++i)
  {
    Standard_Real aDiff = anExtC.SquareDistance(i) - aMinSqDist;
    if (aDiff < Precision::SquareConfusion())
      aSols.Append(i);
  }

  // Save all minimal solutions
  myNbExt = aSols.Extent();

  mySqDist = new TColStd_HArray1OfReal(1, myNbExt);
  myPoint1 = new Extrema_HArray1OfPOnCurv(1, myNbExt);
  myPoint2 = new Extrema_HArray1OfPOnSurf(1, myNbExt);

  TColStd_ListIteratorOfListOfInteger it(aSols);
  for (Standard_Integer iSol = 1; it.More(); it.Next(), ++iSol)
  {
    Extrema_POnCurv P1, P2;
    anExtC.Points(it.Value(), P1, P2);

    // Compute parameters on sphere
    Standard_Real aU, aV;
    ElSLib::Parameters(S, P1.Value(), aU, aV);
    // Compute point on sphere
    gp_Pnt aPOnS = ElSLib::Value(aU, aV, S);

    // Save solution
    myPoint1->SetValue(iSol, P1);
    myPoint2->SetValue(iSol, Extrema_POnSurf(aU, aV, aPOnS));
    mySqDist->SetValue(iSol, P1.Value().SquareDistance(aPOnS));
  }
}

Extrema_ExtElCS::Extrema_ExtElCS(const gp_Circ& C,
				 const gp_Torus& S)
{  Perform(C, S);}



//void Extrema_ExtElCS::Perform(const gp_Circ& C,
//			      const gp_Torus& S)
void Extrema_ExtElCS::Perform(const gp_Circ& ,
			      const gp_Torus& )
{
  throw Standard_NotImplemented();

}

Extrema_ExtElCS::Extrema_ExtElCS(const gp_Hypr& C,
				 const gp_Pln& S)
{
  Perform(C, S);
}



void Extrema_ExtElCS::Perform(const gp_Hypr& C,
			      const gp_Pln& S)
{
  myDone = Standard_True;
  myIsPar = Standard_False;
  myNbExt = 0;

  gp_Ax2 Pos = C.Position();
  gp_Dir NHypr = Pos.Direction();
  gp_Dir NPln = S.Axis().Direction();

  if (NHypr.IsParallel(NPln, Precision::Angular())) {

    mySqDist = new TColStd_HArray1OfReal(1, 1);
    mySqDist->SetValue(1, S.SquareDistance(C.Location()));
    myIsPar = Standard_True;
    myNbExt = 1;
  }
  else {

    gp_Dir XDir = Pos.XDirection();
    gp_Dir YDir = Pos.YDirection();

    Standard_Real A = C.MinorRadius()*(NPln.Dot(YDir)); 
    Standard_Real B = C.MajorRadius()*(NPln.Dot(XDir)); 

    if(Abs(B) > Abs(A)) {
      Standard_Real T = -0.5 * Log((A+B)/(B-A));
      gp_Pnt Ph = ElCLib::HyperbolaValue(T, Pos, C.MajorRadius(), C.MinorRadius());
      Extrema_POnCurv PC(T, Ph);
      myPoint1 = new Extrema_HArray1OfPOnCurv(1,1);
      myPoint1->SetValue(1, PC);

      mySqDist = new TColStd_HArray1OfReal(1, 1);
      mySqDist->SetValue(1, S.SquareDistance(Ph));

      Standard_Real U, V;
      ElSLib::PlaneParameters(S.Position(), Ph, U, V);
      gp_Pnt Pp = ElSLib::PlaneValue(U, V, S.Position());
      Extrema_POnSurf PS(U, V, Pp);
      myPoint2 = new Extrema_HArray1OfPOnSurf(1,1);
      myPoint2->SetValue(1, PS);

      myNbExt = 1;
    }
  }
}


Standard_Boolean Extrema_ExtElCS::IsDone() const
{
  return myDone;
}


Standard_Integer Extrema_ExtElCS::NbExt() const
{
  if (!IsDone()) throw StdFail_NotDone();
  return myNbExt;
}

Standard_Real Extrema_ExtElCS::SquareDistance(const Standard_Integer N) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  return mySqDist->Value(N);
}


void Extrema_ExtElCS::Points(const Standard_Integer N,
			     Extrema_POnCurv& P1,
			     Extrema_POnSurf& P2) const
{
  if (N < 1 || N > NbExt())
  {
    throw Standard_OutOfRange();
  }

  P1 = myPoint1->Value(N);
  P2 = myPoint2->Value(N);
}


Standard_Boolean Extrema_ExtElCS::IsParallel() const
{
  if (!IsDone())
  {
    throw StdFail_NotDone();
  }
  return myIsPar;
}
