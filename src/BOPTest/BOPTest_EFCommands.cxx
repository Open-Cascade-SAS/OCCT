// Created on: 2001-02-26
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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

#include <Draw_Interpretor.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <DBRep.hxx>

#include <IntTools_EdgeFace.hxx>
#include <BRep_Tool.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>

//=======================================================================
//function :bedge
//purpose  : 
//=======================================================================
static Standard_Integer bef   (Draw_Interpretor& di, 
			       Standard_Integer n, 
			       const char** a)
{
  if (n<3) {
    di << " Use bef> Edge Face Discr[33] Tol[1.e-7] Deflection[0.01]\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  
  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes is not allowed \n";
    return 1;
  }

  if (S1.ShapeType()!=TopAbs_EDGE && S2.ShapeType()!=TopAbs_FACE) {
    di << " Shapes must be EDGE and  FACE type\n";
    return 1;
  }

  TopoDS_Edge E=TopoDS::Edge(S1);
  TopoDS_Face F=TopoDS::Face(S2);

  Standard_Integer aDiscr=33;
  Standard_Real aTol=.0000001, aDeflection=.01, aFrom, aTo;
  
  if (n>=4) {  
    aDiscr=atoi (a[3]);
  }

  if (n>=5) {  
    aTol=atof (a[4]);
  }

  if (n>=6) {  
    aDeflection=atof (a[5]);
  }

  di << " bef: ===Begin===\n";
  di << " bef: Discretize=" << aDiscr << "%d\n";
  di << " bef: Tolerance =" << aTol << "\n";
  di << " bef: Deflection=" << aDeflection << "\n";
  
  IntTools_EdgeFace aEF;
  
  aEF.SetEdge(E); 
  aEF.SetFace(F); 
  aEF.SetTolE(aTol);
  aEF.SetTolF(aTol);

  aEF.SetDiscretize(aDiscr);
  aEF.SetDeflection(aDeflection);

  BRep_Tool::Range(E, aFrom, aTo);
  aEF.SetRange (aFrom, aTo);

  aEF.Perform();
  //
  // Results treatment
  Standard_Boolean anIsDone=aEF.IsDone();
  
  di << "\n aEF.IsDone()=" << (Standard_Integer) anIsDone << ", aEF.ErrorStatus()=" << aEF.ErrorStatus() << "\n";
  
  if (anIsDone) {
    Standard_Integer i, aNb;
    Standard_Real tF, tL;

    const IntTools_SequenceOfCommonPrts& aCPs=aEF.CommonParts();
    aNb=aCPs.Length();
    di << " aNb Common Parts=" << aNb << "\n";
    for (i=1; i<=aNb; i++) {
      const IntTools_CommonPrt& aCP=aCPs(i);
      TopAbs_ShapeEnum aType=aCP.Type();
      if (aType==TopAbs_VERTEX)
	di << " aCP => Type=Vertex\n";
      else if (aType==TopAbs_EDGE)
	di << " aCP => Type=Edge\n";
      else 
	di << " aCP => Type=Unknown\n";

      aCP.Range1(tF, tL);
      di << " Part " << i << " Edge#1 (" << tF << ", " << tL << ")\n";
    }
  }

  di << " bef: ===End===\n";
  return 0;
}

//=======================================================================
//function : EFCommands
//purpose  : 
//=======================================================================
  void  BOPTest::EFCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "Mtest commands";

  theCommands.Add("bef", "Use bef> Edge Face Discr[33] Tol[1.e-7] Deflection[0.01]", __FILE__, bef, g);
}
