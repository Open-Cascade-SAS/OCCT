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

#include <DBRep.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_IncAllocator.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_BOP.hxx>
#include <BOPDS_DS.hxx>
#include <BOPTest_DrawableShape.hxx>
#include <BOPCol_ListOfShape.hxx>

#include <TCollection_AsciiString.hxx>
#include <IntTools_FaceFace.hxx>
#include <IntTools_Curve.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Color.hxx>
#include <Draw.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Section.hxx>

//
static BOPAlgo_PaveFiller* pPF=NULL;
//

static
  Standard_Integer bopsmt(Draw_Interpretor& di,
                          Standard_Integer n,
                          const char** a,
                          const BOPAlgo_Operation aOp);
static
  Standard_Integer bsmt (Draw_Interpretor& di, 
                       Standard_Integer n, 
                       const char** a,
                       const BOPAlgo_Operation aOp);
//
static Standard_Integer bop       (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopsection(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer boptuc    (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopcut    (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopfuse   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopcommon (Draw_Interpretor&, Standard_Integer, const char**);
//
static Standard_Integer bsection  (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer btuc      (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcut      (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bfuse     (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bcommon   (Draw_Interpretor&, Standard_Integer, const char**);
//
static Standard_Integer bopcurves (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopnews   (Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : BOPCommands
//purpose  : 
//=======================================================================
  void BOPTest::BOPCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOP commands";
  // Commands
  
  theCommands.Add("bop"       , "use bop s1 s2"   , __FILE__, bop, g);
  theCommands.Add("bopcommon" , "use bopcommon r" , __FILE__, bopcommon, g);
  theCommands.Add("bopfuse"   , "use bopfuse r"   , __FILE__,bopfuse, g);
  theCommands.Add("bopcut"    , "use bopcut"      , __FILE__,bopcut, g);
  theCommands.Add("boptuc"    , "use boptuc"      , __FILE__,boptuc, g);
  theCommands.Add("bopsection", "use bopsection"  , __FILE__,bopsection, g);
  //
  theCommands.Add("bcommon" , "use bcommon r s1 s2" , __FILE__,bcommon, g);
  theCommands.Add("bfuse"   , "use bfuse r s1 s2"   , __FILE__,bfuse, g);
  theCommands.Add("bcut"    , "use bcut r s1 s2"    , __FILE__,bcut, g);
  theCommands.Add("btuc"    , "use btuc r s1 s2"    , __FILE__,btuc, g);
  theCommands.Add("bsection", "Use >bsection r s1 s2 [-n2d/-n2d1/-n2d2] [-na]", 
                                                      __FILE__, bsection, g);
  //
  theCommands.Add("bopcurves", "use  bopcurves F1 F2", __FILE__, bopcurves, g);
  theCommands.Add("bopnews", "use  bopnews -v[e,f]"  , __FILE__, bopnews, g);
}

//=======================================================================
//function : bop
//purpose  : 
//=======================================================================
Standard_Integer bop (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  char buf[32];
  Standard_Integer iErr;
  TopoDS_Shape aS1, aS2;
  BOPCol_ListOfShape aLC;
  //
  if (n!=3) {
    di << " use bop Shape1 Shape2\n";
    return 1;
  }
  //
  aS1=DBRep::Get(a[1]);
  aS2=DBRep::Get(a[2]);
  //
  if (aS1.IsNull() || aS2.IsNull()) {
    di << " null shapes are not allowed \n";
    return 1;
  }
  //
  aLC.Append(aS1);
  aLC.Append(aS2);
  //
  if (pPF!=NULL) {
    delete pPF;
    pPF=NULL;
  }
  Handle(NCollection_BaseAllocator)aAL=new NCollection_IncAllocator;
  pPF=new BOPAlgo_PaveFiller(aAL);
  //
  pPF->SetArguments(aLC);
  //
  pPF->Perform();
  iErr=pPF->ErrorStatus();
  if (iErr) {
    Sprintf(buf, " ErrorStatus : %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  return 0;
}
//=======================================================================
//function : bopcommon
//purpose  : 
//=======================================================================
Standard_Integer bopcommon (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bopsmt(di, n, a, BOPAlgo_COMMON);
}
//=======================================================================
//function : bopfuse
//purpose  : 
//=======================================================================
Standard_Integer bopfuse(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bopsmt(di, n, a, BOPAlgo_FUSE);
}
//=======================================================================
//function : bopcut
//purpose  : 
//=======================================================================
Standard_Integer bopcut(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bopsmt(di, n, a, BOPAlgo_CUT);
}
//=======================================================================
//function : boptuc
//purpose  : 
//=======================================================================
Standard_Integer boptuc(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bopsmt(di, n, a, BOPAlgo_CUT21);
}
//=======================================================================
//function : bopsection
//purpose  : 
//=======================================================================
Standard_Integer bopsection(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bopsmt(di, n, a, BOPAlgo_SECTION);
}
//=======================================================================
//function : bopsmt
//purpose  : 
//=======================================================================
Standard_Integer bopsmt(Draw_Interpretor& di,
                        Standard_Integer n,
                        const char** a,
                        const BOPAlgo_Operation aOp)
{
  if (n<2) {
    di << " use bopsmt r\n";
    return 0;
  }
  //
  if (!pPF) {
    di << " prepare PaveFiller first\n";
    return 0;
  }
  //
  if (pPF->ErrorStatus()) {
    di << " PaveFiller has not been done\n";
    return 0;
  }
  //
  char buf[64];
  Standard_Integer aNb, iErr;
  BOPAlgo_BOP aBOP;
  //
  const BOPCol_ListOfShape& aLC=pPF->Arguments();
  aNb=aLC.Extent();
  if (aNb!=2) {
    Sprintf (buf, " wrong number of arguments %s\n", aNb);
    di << buf;
    return 0;
  }
  //
  const TopoDS_Shape& aS1=aLC.First();
  const TopoDS_Shape& aS2=aLC.Last();
  //
  aBOP.AddArgument(aS1);
  aBOP.AddTool(aS2);
  aBOP.SetOperation(aOp);
  //
  aBOP.PerformWithFiller(*pPF);
  iErr=aBOP.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " ErrorStatus : %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  const TopoDS_Shape& aR=aBOP.Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  //
  DBRep::Set(a[1], aR);
  return 0;
}
//=======================================================================
//function : bcommon
//purpose  : 
//=======================================================================
Standard_Integer bcommon (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bsmt(di, n, a, BOPAlgo_COMMON);
}
//=======================================================================
//function : bfuse
//purpose  : 
//=======================================================================
Standard_Integer bfuse (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bsmt(di, n, a, BOPAlgo_FUSE);
}
//=======================================================================
//function : bcut
//purpose  : 
//=======================================================================
Standard_Integer bcut (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bsmt(di, n, a, BOPAlgo_CUT);
}
//=======================================================================
//function : btuc
//purpose  : 
//=======================================================================
Standard_Integer btuc (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  return bsmt(di, n, a, BOPAlgo_CUT21);
}
//=======================================================================
//function : bsection
//purpose  : 
//=======================================================================
Standard_Integer  bsection(Draw_Interpretor& di, 
                           Standard_Integer n, 
                           const char** a)
{
  const char* usage = " Usage: bsection Result s1 s2 [-n2d/-n2d1/-n2d2] [-na]\n";
  if (n < 4) {
    di << usage;
    return 1;
  }

  TopoDS_Shape aS1 = DBRep::Get(a[2]);
  TopoDS_Shape aS2 = DBRep::Get(a[3]);
  
  if (aS1.IsNull() || aS2.IsNull()) {
    di << " Null shapes are not allowed \n";
    return 1;
  }

  Standard_Boolean bApp, bPC1, bPC2;
  //
  bApp = Standard_True;
  bPC1 = Standard_True;
  bPC2 = Standard_True;
  
  Standard_Boolean isbadparameter = Standard_False;
  
  if(n > 4) {
    const char* key1 = a[4];
    const char* key2 = (n > 5) ? a[5] : NULL;
    const char* pcurveconf = NULL;

#ifdef WNT
    if (key1 && (!strcasecmp(key1,"-n2d") || !strcasecmp(key1,"-n2d1") || !strcasecmp(key1,"-n2d2"))) {
#else 
    if (key1 && (!strncasecmp(key1,"-n2d", 4) || !strncasecmp(key1,"-n2d1", 5) || !strncasecmp(key1,"-n2d2", 5))) {
#endif
      pcurveconf = key1;
    }
    else {
#ifdef WNT
      if (!strcasecmp(key1,"-na")) {
#else 
      if(!strncasecmp(key1,"-na", 3)) {
#endif
        bApp = Standard_False;
      }
      else {
        isbadparameter = Standard_True;
      }
    }
    if (key2) {
#ifdef WNT
      if(!strcasecmp(key2,"-na")) {
#else 
      if (!strncasecmp(key2,"-na", 3)) {
#endif
        bApp = Standard_False;
      }
      else {
        isbadparameter = Standard_True;
      }
    }

    if(!isbadparameter && pcurveconf) {      
#ifdef WNT
      if (!strcasecmp(pcurveconf, "-n2d1")) {
#else 
      if (!strncasecmp(pcurveconf, "-n2d1", 5)) {
#endif
        bPC1 = Standard_False;
      }
      else {
#ifdef WNT
        if (!strcasecmp(pcurveconf, "-n2d2")) {
#else 
        if (!strncasecmp(pcurveconf, "-n2d2", 5)) {
#endif
          bPC2 = Standard_False;
        }
        else {
#ifdef WNT
          if (!strcasecmp(pcurveconf, "-n2d")) {
#else 
          if (!strncasecmp(pcurveconf, "-n2d", 4)) {
#endif
            bPC1 = Standard_False;
            bPC2 = Standard_False;
          }
        }
      }
    }
  }
      
  if(!isbadparameter) {
    Standard_Integer iErr;
    char buf[80];
    //
    BRepAlgoAPI_Section aSec(aS1, aS2, Standard_False);
    aSec.Approximation(bApp);
    aSec.ComputePCurveOn1(bPC1);
    aSec.ComputePCurveOn2(bPC2);
    //
    aSec.Build();
    iErr=aSec.ErrorStatus();
    if (!aSec.IsDone()) {
      Sprintf(buf, " ErrorStatus : %d\n",  iErr);
      di << buf;
      return 0;
    }
    //
    const TopoDS_Shape& aR=aSec.Shape();
    if (aR.IsNull()) {
      di << " null shape\n";
      return 0;
    }
    DBRep::Set(a[1], aR);
    return 0;
  }
  else {
    di << usage;
    return 1;
  }
  return 0;
}
//=======================================================================
//function : bsmt
//purpose  : 
//=======================================================================
Standard_Integer bsmt (Draw_Interpretor& di, 
                       Standard_Integer n, 
                       const char** a,
                       const BOPAlgo_Operation aOp)
{
  char buf[32];
  Standard_Integer iErr;
  TopoDS_Shape aS1, aS2;
  BOPCol_ListOfShape aLC;
  //
  if (n!=4) {
    di << " use bx r s1 s2\n";
    return 1;
  }
  //
  aS1=DBRep::Get(a[2]);
  aS2=DBRep::Get(a[3]);
  //
  if (aS1.IsNull() || aS2.IsNull()) {
    di << " null shapes are not allowed \n";
    return 1;
  }
  aLC.Append(aS1);
  aLC.Append(aS2);
  //
  Handle(NCollection_BaseAllocator)aAL=new NCollection_IncAllocator;
  BOPAlgo_PaveFiller aPF(aAL);
  //
  aPF.SetArguments(aLC);
  //
  aPF.Perform();
  iErr=aPF.ErrorStatus();
  if (iErr) {
    Sprintf(buf, " ErrorStatus : %d\n",  iErr);
    di << buf;
    return 0;
  }
  //
  BRepAlgoAPI_BooleanOperation* pBuilder=NULL;
  // 
  if (aOp==BOPAlgo_COMMON) {
    pBuilder=new BRepAlgoAPI_Common(aS1, aS2, aPF);
  }
  else if (aOp==BOPAlgo_FUSE) {
    pBuilder=new BRepAlgoAPI_Fuse(aS1, aS2, aPF);
  }
  else if (aOp==BOPAlgo_CUT) {
    pBuilder=new BRepAlgoAPI_Cut (aS1, aS2, aPF);
  }
  else if (aOp==BOPAlgo_CUT21) {
    pBuilder=new BRepAlgoAPI_Cut(aS1, aS2, aPF, Standard_False);
  }
  //
  iErr = pBuilder->ErrorStatus();
  if (!pBuilder->IsDone()) {
    Sprintf(buf, " ErrorStatus : %d\n",  iErr);
    di << buf;
    return 0;
  }
  const TopoDS_Shape& aR=pBuilder->Shape();
  if (aR.IsNull()) {
    di << " null shape\n";
    return 0;
  }
  DBRep::Set(a[1], aR);
  return 0;
}

//=======================================================================
//function : bopnews
//purpose  : 
//=======================================================================
Standard_Integer bopnews (Draw_Interpretor& di, 
                          Standard_Integer n, 
                          const char** a)
{
  if (n!=2) {
    di << " use bopnews -v[e,f]\n";
    return 0;
  }
  //
  if (pPF==NULL) {
    di << " Prepare BOPAlgo_PaveFiller first >bop S1 S2\n";
    return 0;
  }
  //
  char buf[32];
  Standard_CString aText;
  Standard_Integer i, i1, i2, iFound;
  Draw_Color aTextColor(Draw_cyan);
  TopAbs_ShapeEnum aT;
  Handle(BOPTest_DrawableShape) aDShape;
  //
  const BOPDS_PDS& pDS=pPF->PDS();
  //
  aT=TopAbs_SHAPE;
  if (!strcmp (a[1], "-f")) {
    aT=TopAbs_FACE;
  }
  else if (!strcmp (a[1], "-e")){
    aT=TopAbs_EDGE;
  }
  else if (!strcmp (a[1], "-v")){
    aT=TopAbs_VERTEX;
  }
  else {
    di << " use bopnews -v[e,f]\n";
    return 0;
  }
  //
  iFound=0;
  i1=pDS->NbSourceShapes();
  i2=pDS->NbShapes();
  for (i=i1; i<i2; ++i) {
    const BOPDS_ShapeInfo& aSI=pDS->ShapeInfo(i);
    if (aSI.ShapeType()==aT) {
      const TopoDS_Shape& aS=aSI.Shape();
      //
      Sprintf (buf, "z%d", i);
      aText=buf;
      aDShape=new BOPTest_DrawableShape (aS, aText, aTextColor);
      Draw::Set (aText, aDShape);
      //
      Sprintf (buf, " z%d", i);
      di << buf;
      //
      iFound=1;
    }
  }
  //
  if (iFound) {
    di << "\n";
  }
  else {
    di << " not found\n";
  }
  //
  return 0;
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
    di << " use bopcurves F1 F2\n";
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


  const TopoDS_Face& aF1=*(TopoDS_Face*)(&S1);
  const TopoDS_Face& aF2=*(TopoDS_Face*)(&S2);

  Standard_Boolean aToApproxC3d, aToApproxC2dOnS1, aToApproxC2dOnS2, anIsDone;
  Standard_Integer i, aNbCurves;
  Standard_Real anAppTol, aTolR;
  TCollection_AsciiString aNm("c_");

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
    di << " anIsDone=" << (Standard_Integer) anIsDone << "\n";
    return 1;
  }

  aFF.PrepareLines3D();
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
