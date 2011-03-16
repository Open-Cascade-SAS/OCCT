// File:	BOPTest_ProjectionCommands.cxx
// Created:	Thu May 18 10:45:01 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTest.ixx>

#include <stdio.h>

#include <Draw_Interpretor.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_CommonPrt.hxx>
#include <BRep_Tool.hxx>

//=======================================================================
//function :bbedge
//purpose  : 
//=======================================================================
static Standard_Integer bbedge (Draw_Interpretor& di,
				Standard_Integer n, 
				const char** a)
{
  if (n<7) {
    di << " Use bbedge> Edge1 Edge2 f1 l1 f2 l2\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  
  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes is not allowed \n";
    return 1;
  }

  if (S1.ShapeType()!=TopAbs_EDGE && S2.ShapeType()!=TopAbs_EDGE) {
    di << " Shapes must be EDGE type\n";
    return 1;
  }

  TopoDS_Edge E1=TopoDS::Edge(S1);
  TopoDS_Edge E2=TopoDS::Edge(S2);

  Standard_Integer aDiscr=30;
  Standard_Real aTol=.0000001, aDeflection=.01;
  Standard_Real f1, l1, f2, l2;

  f1=atof (a[3]);
  l1=atof (a[4]);
  f2=atof (a[5]);
  l2=atof (a[6]);

  IntTools_Range aR1, aR2;
  aR1.SetFirst(f1);  aR1.SetLast(l1);
  aR2.SetFirst(f2);  aR2.SetLast(l2);
  
  di << " bbedge: ===Begin===\n";
  di << " bbedge: Discretize=" << aDiscr << "\n";
  di << " bbedge: Tolerance =" << aTol << "\n";
  di << " bbedge: Deflection=" << aDeflection << "\n";

  IntTools_EdgeEdge aEE;
  
  
  aEE.SetEdge1(E1); 
  aEE.SetEdge2(E2); 
  aEE.SetTolerance1(aTol);
  aEE.SetTolerance2(aTol);

  aEE.SetDiscretize(aDiscr);
  aEE.SetDeflection(aDeflection);

  aEE.SetRange1(aR1);
  aEE.SetRange2(aR2);

  aEE.Perform();
  //
  // Results treatment
  Standard_Boolean anIsDone=aEE.IsDone();
  di << "\n aEE.IsDone()=" << (Standard_Integer) anIsDone << ", aEE.ErrorStatus()=" << aEE.ErrorStatus() << "\n";
  
  if (anIsDone) {
    const IntTools_SequenceOfCommonPrts& aSCp=aEE.CommonParts ();

    Standard_Integer i,j, aNbRanges, aNbRanges2;
    Standard_Real    t1, t2;

    aNbRanges=aSCp.Length();
    for (i=1; i<=aNbRanges; i++) {
      const IntTools_CommonPrt& aCommonPrt=aSCp(i);
      
      di << " CommonPrt#" << i << " , Type=" << aCommonPrt.Type() << "\n";
      const IntTools_Range& aRange1=aCommonPrt.Range1();
      aRange1.Range(t1, t2);
      di << "Range 1: [" << t1 << ", " << t2 << "]\n"; 

      const IntTools_SequenceOfRanges& aSeqOfRanges2=aCommonPrt.Ranges2();
      aNbRanges2=aSeqOfRanges2.Length();
      for (j=1; j<=aNbRanges2; j++) {
	const IntTools_Range& aRange2=aSeqOfRanges2(j);
	aRange2.Range(t1, t2);
	di << "Range 2: [" << t1 << ", " << t2 << "] (# " << j << ")\n"; 
      }
    } 
  }
  

  di << " bedge: ===End===\n";
  return 0;
}

//=======================================================================
//function :bedge
//purpose  : 
//=======================================================================
static Standard_Integer bedge (Draw_Interpretor& di, 
			       Standard_Integer n, 
			       const char** a)
{
  if (n<3) {
    di << " Use bedge> Edge1 Edge2 Discr[30] Tol[1.e-7] Deflection[0.01]\n";
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  
  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes is not allowed \n";
    return 1;
  }

  if (S1.ShapeType()!=TopAbs_EDGE && S2.ShapeType()!=TopAbs_EDGE) {
    di << " Shapes must be EDGE type\n";
    return 1;
  }

  TopoDS_Edge E1=TopoDS::Edge(S1);
  TopoDS_Edge E2=TopoDS::Edge(S2);

  Standard_Integer aDiscr=30;
  Standard_Real aTol=.0000001, aDeflection=.01;
  Standard_Real f1, l1, f2, l2;

  BRep_Tool::Range(E1, f1, l1);
  BRep_Tool::Range(E2, f2, l2);

  IntTools_Range aR1, aR2;
  aR1.SetFirst(f1);  aR1.SetLast(l1);
  aR2.SetFirst(f2);  aR2.SetLast(l2);

  if (n>=4) {  
    aDiscr=atoi (a[3]);
  }

  if (n>=5) {  
    aTol=atof (a[4]);
  }

  if (n>=6) {  
    aDeflection=atof (a[5]);
  }

  di << " bedge: ===Begin===\n";
  di << " bedge: Discretize=" << aDiscr << "\n";
  di << " bedge: Tolerance =" << aTol << "\n";
  di << " bedge: Deflection=" << aDeflection << "\n";

  IntTools_EdgeEdge aEE;
  
  
  aEE.SetEdge1(E1); 
  aEE.SetEdge2(E2); 
  aEE.SetTolerance1(aTol);
  aEE.SetTolerance2(aTol);

  aEE.SetDiscretize(aDiscr);
  aEE.SetDeflection(aDeflection);

  aEE.SetRange1(aR1);
  aEE.SetRange2(aR2);

  aEE.Perform();
  //
  // Results treatment
  Standard_Boolean anIsDone=aEE.IsDone();
  di << "\n aEE.IsDone()=" << (Standard_Integer) anIsDone << ", aEE.ErrorStatus()=" << aEE.ErrorStatus() << "\n";
  
  if (anIsDone) {
    const IntTools_SequenceOfCommonPrts& aSCp=aEE.CommonParts ();

    Standard_Integer i,j, aNbRanges, aNbRanges2;
    Standard_Real    t1, t2;

    aNbRanges=aSCp.Length();
    for (i=1; i<=aNbRanges; i++) {
      const IntTools_CommonPrt& aCommonPrt=aSCp(i);
      
      di << " CommonPrt#" << i << " , Type=" << aCommonPrt.Type() << "\n";
      const IntTools_Range& aRange1=aCommonPrt.Range1();
      aRange1.Range(t1, t2);
      di << "Range 1: [" << t1 << ", " << t2 << "]\n"; 

      const IntTools_SequenceOfRanges& aSeqOfRanges2=aCommonPrt.Ranges2();
      aNbRanges2=aSeqOfRanges2.Length();
      for (j=1; j<=aNbRanges2; j++) {
	const IntTools_Range& aRange2=aSeqOfRanges2(j);
	aRange2.Range(t1, t2);
	di << "Range 2: [" << t1 << ", " << t2 << "] (# " << j << ")\n"; 
      }
    } 
  }
  

  di << " bedge: ===End===\n";
  return 0;
}

//=======================================================================
//function : MTestCommands
//purpose  : 
//=======================================================================
  void  BOPTest::MTestCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "Mtest commands";

  theCommands.Add("bedge" , "Use bedge> Edge1 Edge2 Discr[30] Tol[1.e-7] Deflection[0.01]", __FILE__, bedge,g);
  theCommands.Add("bbedge", "Use bbedge> Edge1 Edge2 f1 l1 f2 l2", __FILE__, bbedge,g);
}
