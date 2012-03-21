// Created on: 2000-03-16
// Created by: Peter KURNEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <BOPTest.ixx>

#include <stdio.h>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

#include <TCollection_AsciiString.hxx>

#include <IntTools_FaceFace.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_Curve.hxx>

#include <Geom_Curve.hxx>
#include <DrawTrSurf.hxx>
#include <DBRep.hxx>
#include <IntTools_Tools.hxx>


static Standard_Integer bopcurves (Draw_Interpretor&, Standard_Integer, const char** );
static Standard_Integer bcurtolerance (Draw_Interpretor& di, Standard_Integer n, const char** a);

//=======================================================================
//function : BOPTest::CurveCommands
//purpose  : 
//=======================================================================
  void  BOPTest::CurveCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "CCR commands";

  theCommands.Add("bopcurves"     , "Use  bopcurves> F1 F2", __FILE__, bopcurves, g);
  theCommands.Add("bcurtolerance" , " use >bcurtolerance C3Dtrim, Tol\n", __FILE__, bcurtolerance, g);
}

//=======================================================================
//function : bopcurves
//purpose  : 
//=======================================================================
Standard_Integer bopcurves (Draw_Interpretor& di, 
			    Standard_Integer n, 
			    const char** a)
{
  if (n<3) {
    di << " Use bopcurves> F1 F2\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  TopAbs_ShapeEnum aType;

  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes is not allowed \n";
    return 1;
  }

  aType=S1.ShapeType();
  if (aType != TopAbs_FACE) {
    di << " Type mismatch F1\n";
    return 1;
  }
  aType=S2.ShapeType();
  if (aType != TopAbs_FACE) {
    di << " Type mismatch F2\n";
    return 1;
  }


  const TopoDS_Face& aF1=TopoDS::Face(S1);
  const TopoDS_Face& aF2=TopoDS::Face(S2);

  Standard_Boolean aToApproxC3d, aToApproxC2dOnS1, aToApproxC2dOnS2, anIsDone;
  Standard_Boolean bToSplit;
  Standard_Integer i, aNbCurves;
  Standard_Real anAppTol, aTolR;
  TCollection_AsciiString aNm("c_");
  //
  bToSplit=Standard_False;
  aToApproxC3d=Standard_True;
  aToApproxC2dOnS1=Standard_False;
  aToApproxC2dOnS2=Standard_False;
  anAppTol=0.0000001;


  IntTools_FaceFace aFF;
  
  aFF.SetParameters (aToApproxC3d,
		     aToApproxC2dOnS1,
		     aToApproxC2dOnS2,
		     anAppTol);
  
  aFF.Perform (aF1, aF2);
  
  anIsDone=aFF.IsDone();
  if (!anIsDone) {
    //printf(" anIsDone=%d\n", anIsDone);
    di << " anIsDone=" << (Standard_Integer) anIsDone << "\n";
    return 1;
  }

  aFF.PrepareLines3D(bToSplit);
  const IntTools_SequenceOfCurves& aSCs=aFF.Lines();

  //
  aTolR=aFF.TolReached3d();
  di << "Tolerance Reached=" << aTolR << "\n";

  aNbCurves=aSCs.Length();
  if (!aNbCurves) {
    di << " has no 3d curve\n";
    return 1;
  }

  for (i=1; i<=aNbCurves; i++) {
    const IntTools_Curve& anIC=aSCs(i);

    Handle (Geom_Curve) aC3D=anIC.Curve();

    if (aC3D.IsNull()) {
      di << " has Null 3d curve# " << i << "%d\n";
      continue;
    }

    TCollection_AsciiString anIndx(i), aNmx;
    aNmx=aNm+anIndx;
    Standard_CString name= aNmx.ToCString();
    DrawTrSurf::Set(name, aC3D);
    di << name << " ";
  }

  di << "\n";
  
  return 0;
}

//=======================================================================
//function : bcurtolerance
//purpose  : 
//=======================================================================
 Standard_Integer bcurtolerance (Draw_Interpretor& di, 
				 Standard_Integer n, 
				 const char** a)
{
  if(n < 2) {
    di << " use >bcurtolerance C3D [Tol=1.e-7]\n";
    return 1;
  }

  Handle(Geom_Curve) aC3D = DrawTrSurf::GetCurve(a[1]);
  if ( aC3D.IsNull()) {
    di << " Null Curve is not allowed here\n";
    return 1;
  }
  //
  Standard_Real  aTolMax, aTol;
  
  aTol=1.e-7;
  if (n>2) {
    aTol=atof(a[2]);
    if (aTol<=0.) {
      aTol=1.e-7;
    }
  }
  //
  aTolMax=IntTools_Tools::CurveTolerance(aC3D, aTol);
  //printf(" aTolMax=%16.11f\n", aTolMax);
  di << " aTolMax=" << aTolMax << "\n";
  
  return 0;
}
