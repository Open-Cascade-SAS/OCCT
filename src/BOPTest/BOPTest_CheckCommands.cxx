// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#include <BOPTest.ixx>
#include <TCollection_AsciiString.hxx>

#include <gp_Pnt.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

#include <DBRep.hxx>

#include <Geom_Geometry.hxx>
#include <Geom_CartesianPoint.hxx>

#include <Draw.hxx>
#include <DrawTrSurf.hxx>

#include <BRepBuilderAPI_Copy.hxx>

#include <BOPCol_ListOfShape.hxx>

#include <BOPDS_DS.hxx>

#include <BOPAlgo_CheckerSI.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_VectorOfInterfEE.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_VectorOfInterfEF.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfInterfVZ.hxx>
#include <BOPDS_VectorOfInterfEZ.hxx>
#include <BOPDS_VectorOfInterfFZ.hxx>
#include <BOPDS_VectorOfInterfZZ.hxx>

#include <BOPAlgo_ArgumentAnalyzer.hxx>
#include <BOPAlgo_CheckResult.hxx>

static 
  Standard_Integer bopcheck (Draw_Interpretor&, Standard_Integer, const char** );

static 
  Standard_Integer bopargcheck (Draw_Interpretor&, Standard_Integer, const char** );
//

//=======================================================================
//function : CheckCommands
//purpose  : 
//=======================================================================
  void  BOPTest::CheckCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) 
    return;

  done = Standard_True;
  // Chapter's name
  const char* g = "CCR commands";
  //
  theCommands.Add("bopcheck",  
                  "Use >bopcheck Shape [level of check: 0(V/V) - 5(all)]",
                  __FILE__, bopcheck, g);
  theCommands.Add("bopargcheck" , 
                  "Use bopargcheck without parameters to get ",  
                  __FILE__, bopargcheck, g);
}

//=======================================================================
//function : bopcheck
//purpose  : 
//=======================================================================
Standard_Integer bopcheck 
  (Draw_Interpretor& di, Standard_Integer n,  const char** a )
{
  if (n<2) {
    di << " Use >bopcheck Shape [level of check: 0 - 9" << "\n";
    di << " The level of check defines "; 
    di << " which interferences will be checked:\n";
    di << " 0 - V/V only\n"; 
    di << " 1 - V/V, V/E\n";
    di << " 2 - V/V, V/E, E/E\n"; 
    di << " 3 - V/V, V/E, E/E , V/F\n"; 
    di << " 4 - V/V, V/E, E/E, V/F , E/F\n"; 
    di << " 5 - V/V, V/E, E/E, V/F, E/F, F/F;\n";
    di << " 6 - V/V, V/E, E/E, V/F, E/F, F/F, V/Z\n";
    di << " 7 - V/V, V/E, E/E, V/F, E/F, F/F, E/Z\n";
    di << " 8 - V/V, V/E, E/E, V/F, E/F, F/F, E/Z, F/Z\n";
    di << " 9 - V/V, V/E, E/E, V/F, E/F, F/F, E/Z, F/Z, Z/Z\n";
    di << " Default level is 9\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    di << "null shapes are not allowed here!";
    return 1;
  }
  //
  Standard_Integer theLevelOfCheck, aNbInterfTypes;
  //
  aNbInterfTypes=BOPDS_DS::NbInterfTypes();
  //
  theLevelOfCheck = (n==3) ? Draw::Atoi(a[2]) : aNbInterfTypes-1;
  if (theLevelOfCheck > aNbInterfTypes-1) {
    di << "Invalid level";
    return 1;
  }
  //-------------------------------------------------------------------
  char buf[256];
  char type[10][4] = {
    "V/V", "V/E", "E/E","V/F", "E/F", "F/F", "V/Z", "E/Z", "F/Z", "Z/Z"
  };
  Standard_Integer iErr, aTypeInt, i, ind, j, nI1, nI2;
  Standard_Boolean bSelfInt, bFFInt;
  //
  if (theLevelOfCheck >= 0 && theLevelOfCheck < (aNbInterfTypes-1)) {
    di << "Info:\nThe level of check is set to " 
      << type[theLevelOfCheck] << ", i.e. intersection(s)\n";
    for (i=theLevelOfCheck+1; i<aNbInterfTypes; ++i) {
      di << type[i];
      if (i<aNbInterfTypes-1) {
        di << ", ";
      }
    }
    di << " will not be checked.\n\n";
  }
  //
  BOPAlgo_CheckerSI aChecker;
  BOPCol_ListOfShape anArgs;
  //
  anArgs.Append(aS);
  aChecker.SetArguments(anArgs);
  aChecker.SetLevelOfCheck(theLevelOfCheck);
  //
  aChecker.Perform();
  iErr = aChecker.ErrorStatus();
  //
  const BOPDS_PDS& theDS = aChecker.PDS();
  BOPDS_VectorOfInterfVV& aVVs=theDS->InterfVV();
  BOPDS_VectorOfInterfVE& aVEs=theDS->InterfVE();
  BOPDS_VectorOfInterfEE& aEEs=theDS->InterfEE();
  BOPDS_VectorOfInterfVF& aVFs=theDS->InterfVF();
  BOPDS_VectorOfInterfEF& aEFs=theDS->InterfEF();
  BOPDS_VectorOfInterfFF& aFFs=theDS->InterfFF();
  BOPDS_VectorOfInterfVZ& aVZs=theDS->InterfVZ();
  BOPDS_VectorOfInterfEZ& aEZs=theDS->InterfEZ();
  BOPDS_VectorOfInterfFZ& aFZs=theDS->InterfFZ();
  BOPDS_VectorOfInterfZZ& aZZs=theDS->InterfZZ();
  //
  Standard_Integer aNb[] ={
    aVVs.Extent(), aVEs.Extent(), aEEs.Extent(), 
    aVFs.Extent(), aEFs.Extent(), aFFs.Extent(),
    aVZs.Extent(), aEZs.Extent(), aFZs.Extent(), 
    aZZs.Extent(),
  };
  //
  bSelfInt = Standard_False;
  ind = 0;
  for (aTypeInt = 0; aTypeInt < aNbInterfTypes; ++aTypeInt) {
    
    for (i = 0; i < aNb[aTypeInt]; ++i) {
      BOPDS_Interf* aInt=NULL;
      //
      switch(aTypeInt) {
      case 0:
        aInt=(BOPDS_Interf*)(&aVVs(i));
        break;
      case 1:
        aInt=(BOPDS_Interf*)(&aVEs(i));
        break;
      case 2:
        aInt=(BOPDS_Interf*)(&aEEs(i));
        break;
      case 3:
        aInt=(BOPDS_Interf*)(&aVFs(i));
        break;
      case 4:
        aInt=(BOPDS_Interf*)(&aEFs(i));
        break;
      case 5:
        aInt=(BOPDS_Interf*)(&aFFs(i));
        break;
      case 6:
        aInt=(BOPDS_Interf*)(&aVZs(i));
        break;
      case 7:
        aInt=(BOPDS_Interf*)(&aEZs(i));
        break;
      case 8:
        aInt=(BOPDS_Interf*)(&aFZs(i));
        break;
      case 9:
        aInt=(BOPDS_Interf*)(&aZZs(i));
        break;
      default:
        break;
      }
      //
      nI1 = aInt->Index1();
      nI2 = aInt->Index2();
      if (nI1 == nI2) {
        continue;
      }
      //
      if(theDS->IsNewShape(nI1) || theDS->IsNewShape(nI2)) {
        continue;
      }
      //
      if (aTypeInt == 4) {
        BOPDS_InterfEF& aEF=aEFs(i);
        if (aEF.CommonPart().Type()==TopAbs_SHAPE) {
          continue;
        }
      }
      //
      const TopoDS_Shape& aS1 = theDS->Shape(nI1);
      const TopoDS_Shape& aS2 = theDS->Shape(nI2);
      //
      if (aTypeInt == 5) {
        bFFInt = Standard_False;
        BOPDS_InterfFF& aFF = aFFs(i);
        BOPDS_VectorOfPoint& aVP=aFF.ChangePoints();
        Standard_Integer aNbP=aVP.Extent();
        BOPDS_VectorOfCurve& aVC=aFF.ChangeCurves();
        Standard_Integer aNbC=aVC.Extent();
        if (!aNbP && !aNbC) {
          continue;
        }
        for (j=0; j<aNbC; ++j) {
          BOPDS_Curve& aNC=aVC(j);
          BOPDS_ListOfPaveBlock& aLPBC=aNC.ChangePaveBlocks();
          if (aLPBC.Extent()) {
            bFFInt = Standard_True;
            break;
          }
        }
        if (!bFFInt) {
          continue;
        }
      }
      //
      di << type[aTypeInt] << ":";
      //
      TCollection_AsciiString aBaseName("x");
      TCollection_AsciiString anumbername(ind);
      TCollection_AsciiString aXName = aBaseName + anumbername;
      Standard_CString aname=aXName.ToCString();
      DBRep::Set (aname, aS1);
      ++ind;
      TCollection_AsciiString anumbername1(ind);
      TCollection_AsciiString aXName1 = aBaseName + anumbername1;
      Standard_CString aname1=aXName1.ToCString();
      DBRep::Set (aname1, aS2);
      ++ind;
      //
      Sprintf(buf, "%s, %s \n", aname, aname1);
      di << buf;
      bSelfInt = Standard_True;
    }
  }
  //
  if (iErr) {
    di << "There were errors during the operation, ";
    di << "so the list may be incomplete." << "\n";
  }
  //
  if (!bSelfInt) {
    di << " This shape seems to be OK." << "\n";
  }
  return 0;
}

//=======================================================================
//function : MakeShapeForFullOutput
//purpose  : 
//=======================================================================
static void MakeShapeForFullOutput
  (const TCollection_AsciiString & aBaseName,
   const Standard_Integer          aIndex,
   const BOPCol_ListOfShape &    aList,
   Standard_Integer&               aCount,
   Draw_Interpretor&               di)
{
  TCollection_AsciiString aNum(aIndex);
  TCollection_AsciiString aName = aBaseName + aNum;
  Standard_CString name = aName.ToCString();

  TopoDS_Compound cmp;
  BRep_Builder BB;
  BB.MakeCompound(cmp);

  BOPCol_ListIteratorOfListOfShape anIt(aList);
  for(; anIt.More(); anIt.Next()) {
    const TopoDS_Shape & aS = anIt.Value();
    BB.Add(cmp, aS);
    aCount++;
  }
  di << "Made faulty shape: " << name << "\n";
  DBRep::Set(name, cmp);
}


//=======================================================================
//function : bopargcheck
//purpose  : 
//=======================================================================
Standard_Integer bopargcheck 
  (Draw_Interpretor& di, Standard_Integer n,  const char** a )
{
  if (n<2) {
    di << "\n";
    di << " Use >bopargcheck Shape1 [[Shape2] ";
    di << "[-F/O/C/T/S/U] [/R|F|T|V|E|I|P]] [#BF]" << "\n" << "\n";
    di << " -<Boolean Operation>" << "\n";
    di << " F (fuse)" << "\n";
    di << " O (common)" << "\n";
    di << " C (cut)" << "\n";
    di << " T (cut21)" << "\n";
    di << " S (section)" << "\n";
    di << " U (unknown)" << "\n";
    di << " For example: \"bopargcheck s1 s2 -F\" enables" ;
    di << "checking for Fuse operation" << "\n";
    di << " default - section" << "\n" << "\n";
    di << " /<Test Options>" << "\n";
    di << " R (disable small edges (shrank range) test)" << "\n";
    di << " F (disable faces verification test)" << "\n";
    di << " T (disable tangent faces searching test)" << "\n";
    di << " V (disable test possibility to merge vertices)" << "\n";
    di << " E (disable test possibility to merge edges)" << "\n";
    di << " I (disable self-interference test)" << "\n";
    di << " P (disable shape type test)" << "\n";
    di << " C (disable test for shape continuity)" << "\n";
    di << " For example: \"bopargcheck s1 s2 /RI\" disables ";
    di << "small edge detection and self-intersection detection" << "\n";
    di << " default - all options are enabled" << "\n" << "\n";
    di << " #<Additional Test Options>" << "\n";
    di << " B (stop test on first faulty found); default OFF" << "\n";
    di << " F (full output for faulty shapes); default - output ";
    di << "in a short format" << "\n" << "\n";
    di << " NOTE: <Boolean Operation> and <Test Options> are ";
    di <<  "used only for couple" << "\n";
    di << "       of argument shapes, except I and P options ";
    di << "that are always used for" << "\n";
    di << "       couple of shapes as well as for ";
    di <<"single shape test." << "\n";
    return 1;
  }

  TopoDS_Shape aS1 = DBRep::Get(a[1]);

  if(aS1.IsNull()) {
    di << "Error: null shape not allowed!" << "\n";
    di << "Type bopargcheck without arguments for more ";
    di <<"information" << "\n";
    return 1;
  }

  Standard_Boolean isBO = Standard_False;
  Standard_Integer indxBO = 0;
  Standard_Boolean isOP = Standard_False;
  Standard_Integer indxOP = 0;
  Standard_Boolean isAD = Standard_False;
  Standard_Integer indxAD = 0;
  Standard_Boolean isS2 = Standard_False;
  Standard_Integer indxS2 = 0;

  if(n >= 3) {
    Standard_Integer iIndex = 0;
    for(iIndex = 2; iIndex < n; iIndex++) {
      if(a[iIndex][0] == '-')
      {
        isBO = Standard_True;
        indxBO = iIndex;
      }
      //else if(a[iIndex][0] == '+')
      else if(a[iIndex][0] == '/')
      {
        isOP = Standard_True;
        indxOP = iIndex;
      }
      else if(a[iIndex][0] == '#')
      {
        isAD = Standard_True;
        indxAD = iIndex;
      }
      else {
        isS2 = Standard_True;
        indxS2 = iIndex;
      }
    }
  }

  // set & test second shape
  TopoDS_Shape aS22, aS2;
  if(isS2) {
    if(indxS2 != 2) {
      di << "Error: second shape should follow the first one!" << "\n";
      di << "Type bopargcheck without arguments for more information" << "\n";
      return 1;
    }
    else {
      aS22 = DBRep::Get(a[2]);
      if(aS22.IsNull()) {
        di << "Error: second shape is null!" << "\n";
        di << "Type bopargcheck without arguments for more information" << "\n";
        return 1;
      }
    }
  }

  // init checker
  BOPAlgo_ArgumentAnalyzer aChecker;
  aChecker.SetShape1(aS1);

  // set default options (always tested!) for single and couple shapes
  aChecker.ArgumentTypeMode() = Standard_True;
  aChecker.SelfInterMode()    = Standard_True;
  aChecker.SmallEdgeMode()    = Standard_True;
  aChecker.RebuildFaceMode()  = Standard_True;
  aChecker.ContinuityMode()   = Standard_True;

  // test & set options and operation for two shapes
  if(!aS22.IsNull()) {
    aS2 = BRepBuilderAPI_Copy(aS22).Shape();
    aChecker.SetShape2(aS2);
    // set operation (default - Section)
    if(isBO) {
      if(a[indxBO][1] == 'F' || a[indxBO][1] == 'f') {
        aChecker.OperationType() = BOPAlgo_FUSE;
      }
      else if(a[indxBO][1] == 'O' || a[indxBO][1] == 'o') {
        aChecker.OperationType() = BOPAlgo_COMMON;
      }
      else if(a[indxBO][1] == 'C' || a[indxBO][1] == 'c') {
        aChecker.OperationType() = BOPAlgo_CUT;
      }
      else if(a[indxBO][1] == 'T' || a[indxBO][1] == 't') {
         aChecker.OperationType() = BOPAlgo_CUT21;
      }
      else if(a[indxBO][1] == 'S' || a[indxBO][1] == 's') {
         aChecker.OperationType() = BOPAlgo_SECTION;
      }
      else if(a[indxBO][1] == 'U' || a[indxBO][1] == 'u') {
        aChecker.OperationType() = BOPAlgo_UNKNOWN;
      }
      else {
        di << "Error: invalid boolean operation type!" << "\n";
        di << "Type bopargcheck without arguments for more information" << "\n";
        return 1;
      }
    }
    else
      aChecker.OperationType() = BOPAlgo_SECTION;

    aChecker.TangentMode()     = Standard_True;
    aChecker.MergeVertexMode() = Standard_True;
    aChecker.MergeEdgeMode()   = Standard_True;
  }
  
  // set options (default - all ON)
  if(isOP) {
    Standard_Integer ind = 1;
    while(a[indxOP][ind] != 0) {
      if(a[indxOP][ind] == 'R' || a[indxOP][ind] == 'r') {
        aChecker.SmallEdgeMode() = Standard_False;
      }
      else if(a[indxOP][ind] == 'F' || a[indxOP][ind] == 'f') {
        aChecker.RebuildFaceMode() = Standard_False;
      }
      else if(a[indxOP][ind] == 'T' || a[indxOP][ind] == 't') {
        aChecker.TangentMode() = Standard_False;
      }
      else if(a[indxOP][ind] == 'V' || a[indxOP][ind] == 'v') {
        aChecker.MergeVertexMode() = Standard_False;
      }
      else if(a[indxOP][ind] == 'E' || a[indxOP][ind] == 'e') {
        aChecker.MergeEdgeMode() = Standard_False;
      }
      else if(a[indxOP][ind] == 'I' || a[indxOP][ind] == 'i') {
        aChecker.SelfInterMode() = Standard_False;
      }
      else if(a[indxOP][ind] == 'P' || a[indxOP][ind] == 'p') {
        aChecker.ArgumentTypeMode() = Standard_False;
      }
      else if(a[indxOP][ind] == 'C' || a[indxOP][ind] == 'c') {
        aChecker.ContinuityMode() = Standard_False;
      }
      else {
        di << "Error: invalid test option(s)!" << "\n";
        di << "Type bopargcheck without arguments for more information" << "\n";
        return 1;
      }
      ind++;
    }
  }

  // set additional options
  Standard_Boolean fullOutput = Standard_False;
  if(isAD) {
    Standard_Integer ind = 1;
    while(a[indxAD][ind] != 0) {
      if(a[indxAD][ind] == 'B' || a[indxAD][ind] == 'b') {
        aChecker.StopOnFirstFaulty() = Standard_True;
      }
      else if(a[indxAD][ind] == 'F' || a[indxAD][ind] == 'f') {
        fullOutput = Standard_True;
      }
      else {
        di << "Error: invalid additional test option(s)!" << "\n";
        di << "Type bopargcheck without arguments for more information" << "\n";
        return 1;
      }
      ind++;
    }
  }

  // run checker
  aChecker.Perform();

  // process result of checking
  if(!aChecker.HasFaulty()) {
    di << "Shape(s) seem(s) to be valid for BOP."  << "\n";
  }
  else {
    if(!fullOutput) {
      di << "Faulties, that can not be treated by BOP, are detected." << "\n"; 
    }
    else {
      const BOPAlgo_ListOfCheckResult& aResultList = aChecker.GetCheckResult();
      BOPAlgo_ListIteratorOfListOfCheckResult anIt(aResultList);

      Standard_Integer S1_BadType = 0, S1_SelfInt = 0, S1_SmalE = 0, S1_BadF = 0, S1_BadV = 0, S1_BadE = 0;
      Standard_Integer S1_SelfIntAll = 0, S1_SmalEAll = 0, S1_BadFAll = 0, S1_BadVAll = 0, S1_BadEAll = 0;
      Standard_Integer S2_BadType = 0, S2_SelfInt = 0, S2_SmalE = 0, S2_BadF = 0, S2_BadV = 0, S2_BadE = 0;
      Standard_Integer S2_SelfIntAll = 0, S2_SmalEAll = 0, S2_BadFAll = 0, S2_BadVAll = 0, S2_BadEAll = 0;
      Standard_Integer S1_OpAb = 0, S2_OpAb = 0;
      Standard_Integer S1_C0 = 0, S2_C0 = 0, S1_C0All = 0, S2_C0All = 0;
      Standard_Boolean hasUnknown = Standard_False;

      TCollection_AsciiString aS1SIBaseName("s1si_");
      TCollection_AsciiString aS1SEBaseName("s1se_");
      TCollection_AsciiString aS1BFBaseName("s1bf_");
      TCollection_AsciiString aS1BVBaseName("s1bv_");
      TCollection_AsciiString aS1BEBaseName("s1be_");
      TCollection_AsciiString aS1C0BaseName("s1C0_");
      TCollection_AsciiString aS2SIBaseName("s2si_");
      TCollection_AsciiString aS2SEBaseName("s2se_");
      TCollection_AsciiString aS2BFBaseName("s2bf_");
      TCollection_AsciiString aS2BVBaseName("s2bv_");
      TCollection_AsciiString aS2BEBaseName("s2be_");
      TCollection_AsciiString aS2C0BaseName("s2C0_");

      for(; anIt.More(); anIt.Next()) {
        const BOPAlgo_CheckResult& aResult = anIt.Value();
        const TopoDS_Shape & aSS1 = aResult.GetShape1();
        const TopoDS_Shape & aSS2 = aResult.GetShape2();
        const BOPCol_ListOfShape & aLS1 = aResult.GetFaultyShapes1();
        const BOPCol_ListOfShape & aLS2 = aResult.GetFaultyShapes2();
        Standard_Boolean isL1 = !aLS1.IsEmpty();
        Standard_Boolean isL2 = !aLS2.IsEmpty();

        switch(aResult.GetCheckStatus()) {
        case BOPAlgo_BadType: {
          if(!aSS1.IsNull()) S1_BadType++;
          if(!aSS2.IsNull()) S2_BadType++;
        }
          break;
        case BOPAlgo_SelfIntersect: {
          if(!aSS1.IsNull()) {
            S1_SelfInt++;
            if(isL1)
              MakeShapeForFullOutput(aS1SIBaseName, S1_SelfInt, aLS1, S1_SelfIntAll, di);
          }
          if(!aSS2.IsNull()) {
            S2_SelfInt++;
            if(isL2)
              MakeShapeForFullOutput(aS2SIBaseName, S2_SelfInt, aLS2, S2_SelfIntAll, di);
          }
        }
          break;
        case BOPAlgo_TooSmallEdge: {
          if(!aSS1.IsNull()) {
            S1_SmalE++;
            if(isL1)
              MakeShapeForFullOutput(aS1SEBaseName, S1_SmalE, aLS1, S1_SmalEAll, di);
          }
          if(!aSS2.IsNull()) {
            S2_SmalE++;
            if(isL2)
              MakeShapeForFullOutput(aS2SEBaseName, S2_SmalE, aLS2, S2_SmalEAll, di);
          }
        }
          break;
        case BOPAlgo_NonRecoverableFace: {
          if(!aSS1.IsNull()) {
            S1_BadF++;
            if(isL1)
              MakeShapeForFullOutput(aS1BFBaseName, S1_BadF, aLS1, S1_BadFAll, di);
          }
          if(!aSS2.IsNull()) {
            S2_BadF++;
            if(isL2)
              MakeShapeForFullOutput(aS2BFBaseName, S2_BadF, aLS2, S2_BadFAll, di);
          }
        }
          break;
        case BOPAlgo_IncompatibilityOfVertex: {
          if(!aSS1.IsNull()) {
            S1_BadV++;
            if(isL1) {
              MakeShapeForFullOutput(aS1BVBaseName, S1_BadV, aLS1, S1_BadVAll, di);
            }
          }
          if(!aSS2.IsNull()) {
            S2_BadV++;
            if(isL2){
              MakeShapeForFullOutput(aS2BVBaseName, S2_BadV, aLS2, S2_BadVAll, di);
            }
          }
        }
          break;
        case BOPAlgo_IncompatibilityOfEdge: {
          if(!aSS1.IsNull()) {
            S1_BadE++;
            if(isL1) {
              MakeShapeForFullOutput(aS1BEBaseName, S1_BadE, aLS1, S1_BadEAll, di);
            }
          }
          if(!aSS2.IsNull()) {
            S2_BadE++;
            if(isL2) {
              MakeShapeForFullOutput(aS2BEBaseName, S2_BadE, aLS2, S2_BadEAll, di);
            }
          }
        }
          break;
        case BOPAlgo_IncompatibilityOfFace: {
          // not yet implemented
        }
          break;
        case BOPAlgo_GeomAbs_C0: {
          if(!aSS1.IsNull()) {
            S1_C0++;
            if(isL1) {
              MakeShapeForFullOutput(aS1C0BaseName, S1_C0, aLS1, S1_C0All, di);
            }
          }
          if(!aSS2.IsNull()) {
            S2_C0++;
            if(isL2) {
              MakeShapeForFullOutput(aS2C0BaseName, S2_C0, aLS2, S2_C0All, di);
            }
          }
        }
          break;
        case BOPAlgo_OperationAborted: {
          if(!aSS1.IsNull()) S1_OpAb++;
          if(!aSS2.IsNull()) S2_OpAb++;
        }
          break;
        case BOPAlgo_CheckUnknown:
        default: {
          hasUnknown = Standard_True;
        }
          break;
        } // switch
      }// faulties

      Standard_Integer FS1 = S1_SelfInt + S1_SmalE + S1_BadF + S1_BadV + S1_BadE + S1_OpAb + S1_C0;
      FS1 += (S1_BadType != 0) ? 1 : 0;
      Standard_Integer FS2 = S2_SelfInt + S2_SmalE + S2_BadF + S2_BadV + S2_BadE + S2_OpAb + S2_C0;
      FS2 += (S2_BadType != 0) ? 1 : 0;
      
      // output for first shape
      di << "Faulties for FIRST  shape found : " << FS1 << "\n";
      if(FS1 != 0) {
        di << "---------------------------------" << "\n";
        Standard_CString CString1;
        if (S1_BadType != 0)
          CString1="YES";
        else
          CString1="NO";
        di << "Shapes are not suppotrted by BOP: " << CString1 << "\n";
        Standard_CString CString2;
        if (S1_SelfInt != 0)
          CString2="YES";
        else
          CString2="NO";
        di << "Self-Intersections              : " << CString2;
        if(S1_SelfInt != 0)
          di << "  Cases(" << S1_SelfInt << ")  Total shapes(" << S1_SelfIntAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString13;
        if (S1_OpAb != 0)
          CString13="YES";
        else
          CString13="NO";
        di << "Check for SI has been aborted   : " << CString13 << "\n";
        Standard_CString CString3;
        if (S1_SmalE != 0)
          CString3="YES";
        else
          CString3="NO";
        di << "Too small edges                 : " << CString3;
        if(S1_SmalE != 0)
          di << "  Cases(" << S1_SmalE << ")  Total shapes(" << S1_SmalEAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString4;
        if (S1_BadF != 0)
          CString4="YES";
        else
          CString4="NO";
        di << "Bad faces                       : " << CString4;
        if(S1_BadF != 0)
          di << "  Cases(" << S1_BadF << ")  Total shapes(" << S1_BadFAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString5;
        if (S1_BadV != 0)
          CString5="YES";
        else
          CString5="NO";
        di << "Too close vertices              : " << CString5;
        if(S1_BadV != 0)
          di << "  Cases(" << S1_BadV << ")  Total shapes(" << S1_BadVAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString6;
        if (S1_BadE != 0)
          CString6="YES";
        else
          CString6="NO";
        di << "Too close edges                 : " << CString6;
        if(S1_BadE != 0)
          di << "  Cases(" << S1_BadE << ")  Total shapes(" << S1_BadEAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString15;
        if (S1_C0 != 0)
          CString15="YES";
        else
          CString15="NO";
        di << "Shapes with Continuity C0       : " << CString15;
        if(S1_C0 != 0)
          di << "  Cases(" << S1_C0 << ")  Total shapes(" << S1_C0All << ")" << "\n";
        else
          di << "\n";
      }

      // output for second shape
      di << "\n";
      di << "Faulties for SECOND  shape found : " << FS2 << "\n";
      if(FS2 != 0) {
        di << "---------------------------------" << "\n";
        Standard_CString CString7;
        if (S2_BadType != 0)
          CString7="YES";
        else
          CString7="NO";
        di << "Shapes are not suppotrted by BOP: " << CString7 << "\n";
        Standard_CString CString8;
        if (S2_SelfInt != 0)
          CString8="YES";
        else
          CString8="NO";
        di << "Self-Intersections              : " << CString8;
        if(S2_SelfInt != 0)
          di << "  Cases(" << S2_SelfInt << ")  Total shapes(" << S2_SelfIntAll << ")" << "\n";
        else
          di << "\n";

        Standard_CString CString14;
        if (S2_OpAb != 0)
          CString14="YES";
        else
          CString14="NO";
        di << "Check for SI has been aborted   : " << CString14 << "\n";
        Standard_CString CString9;
        if (S2_SmalE != 0)
          CString9="YES";
        else
          CString9="NO";
        di << "Too small edges                 : " << CString9;
        if(S2_SmalE != 0)
          di << "  Cases(" << S2_SmalE << ")  Total shapes(" << S2_SmalEAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString10;
        if (S2_BadF != 0)
          CString10="YES";
        else
          CString10="NO";
        di << "Bad faces                       : " << CString10;
        if(S2_BadF != 0)
          di << "  Cases(" << S2_BadF << ")  Total shapes(" << S2_BadFAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString11;
        if (S2_BadV != 0)
          CString11="YES";
        else
          CString11="NO";
        di << "Too close vertices              : " << CString11;
        if(S2_BadV != 0)
          di << "  Cases(" << S2_BadV << ")  Total shapes(" << S2_BadVAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString12;
        if (S2_BadE != 0)
          CString12="YES";
        else
          CString12="NO";
        di << "Too close edges                 : " << CString12;
        if(S2_BadE != 0)
          di << "  Cases(" << S2_BadE << ")  Total shapes(" << S2_BadEAll << ")" << "\n";
        else
          di << "\n";
        Standard_CString CString16;
        if (S2_C0 != 0)
          CString16="YES";
        else
          CString16="NO";
        di << "Shapes with Continuity C0       : " << CString16;
        if(S2_C0 != 0)
          di << "  Cases(" << S2_C0 << ")  Total shapes(" << S2_C0All << ")" << "\n";
        else
          di << "\n";

        // warning
        di << "\n";
        if(hasUnknown)
          di << "WARNING: The unexpected test break occurs!" << "\n";
      }
    } // full output
  } // has faulties

  return 0;
}
