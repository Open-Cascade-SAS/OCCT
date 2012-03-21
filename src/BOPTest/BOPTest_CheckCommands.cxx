// Created on: 2002-08-05
// Created by: Peter KURNEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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
#include <TCollection_AsciiString.hxx>

#include <gp_Pnt.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>

#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_Checker.hxx>
// modified by NIZHNY-MKK  Fri Sep  3 17:50:06 2004
#include <BOP_CheckStatus.hxx>
#include <BOP_Operation.hxx>
#include <BOP_ArgumentAnalyzer.hxx>
#include <BOP_CheckResult.hxx>
#include <BOP_ListOfCheckResult.hxx>
#include <BOP_ListIteratorOfListOfCheckResult.hxx>

#include <BOPTools_CheckResult.hxx>
#include <BOPTools_ListOfCheckResults.hxx>
#include <BOPTools_ListIteratorOfListOfCheckResults.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <DBRep.hxx>

#include <Geom_Geometry.hxx>
#include <Geom_CartesianPoint.hxx>

#include <DrawTrSurf.hxx>


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
  theCommands.Add("bopcheck" ,  "Use >bopcheck Shape",  __FILE__, bopcheck, g);
  theCommands.Add("bopargcheck" ,  "Use bopargcheck without parameters to get ",  __FILE__, bopargcheck, g);
}

//=======================================================================
//function : bopcheck
//purpose  : 
//=======================================================================
Standard_Integer bopcheck (Draw_Interpretor& di, Standard_Integer n,  const char** a )
{
  
  if (n<2) {
    di << " Use >bopcheck Shape" << "\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);
  //
  BOPTools_Checker aChecker(aS);

  aChecker.Perform();
  
  if (aChecker.HasFaulty()) {
    const BOPTools_ListOfCheckResults& aResultList = aChecker.GetCheckResult();
//     const TopoDS_Shape& aSF1=aChecker.FaultyShape1();
//     const TopoDS_Shape& aSF2=aChecker.FaultyShape2();
    Standard_Integer shapeit = 1;
    BOPTools_ListIteratorOfListOfCheckResults anIt(aResultList);

    for(; anIt.More(); anIt.Next()) {
      const BOPTools_CheckResult& aCheckResult = anIt.Value();
      switch(aCheckResult.GetCheckStatus()) {
	case BOPTools_CHKUNKNOWN: {
	  di << "Unknown status" << "\n";
	  break;
	}
	case BOPTools_VERTEXVERTEX: {
	  di << "Self-intersection near vertices" << "\n";
	  break;
	}
	case BOPTools_VERTEXEDGE: {
	  di << "Self-intersection near vertex and edge" << "\n";
	  break;
	}
	case BOPTools_VERTEXFACE: {
	  di << "Self-intersection near vertex and face" << "\n";
	  break;
	}
	case BOPTools_EDGEEDGE: {
	  di << "Self-intersection near edges" << "\n";
	  break;
	}
	case BOPTools_EDGEEDGECOMBLK: {
	  di << "Coincidance of edges" << "\n";
	  break;
	}
	case BOPTools_EDGEFACE: {
	  di << "Self-intersection near edge and face" << "\n";
	  break;
	}
	case BOPTools_EDGEFACECOMBLK: {
	  di << "Edge belong to face not topologically but geometrically" << "\n";
	  break;
	}
	case BOPTools_FACEFACE: {
	  di << "Self-intersection near faces" << "\n";
	  break;
	}
	case BOPTools_BADSHRANKRANGE: {
	  di << "Can not obtain shrunk range: too small edge" << "\n";
	  break;
	}
	case BOPTools_NULLSRANKRANGE: {
	  di << "Shrunk range is empty: too small edge" << "\n";
	  break;
	}
	default: {
	  di << "Program error" << "\n";
	  break;
	}
      }

      TopTools_ListIteratorOfListOfShape anIt2(aCheckResult.GetShapes());
      TCollection_AsciiString aBaseName("x");
      TopoDS_Shape aSF1;

      for(; anIt2.More(); anIt2.Next()) {
	TopoDS_Shape aSF2 = anIt2.Value();
	Standard_Boolean bPrint = Standard_False;

	if(aSF1.IsNull()) {
	  bPrint = Standard_True;
	} 
	else if (!aSF2.IsSame(aSF1)) {
	  bPrint = Standard_True;
	}
	
	if(bPrint) {
	  TCollection_AsciiString anumbername(shapeit);
	  TCollection_AsciiString aXName = aBaseName + anumbername;
	  //char* aname = aXName.ToCString();
	  Standard_CString aname=aXName.ToCString();
	  DBRep::Set (aname, aSF2);
	  //printf(aname);
	  di << aname << " ";
	  //printf(" ");
	  shapeit++;
	}
	aSF1 = aSF2;
      }
      //printf("\n");
      di << "\n";
      //
      const Handle(Geom_Geometry)& aGG=aCheckResult.GetInterferenceGeometry();

      if (!aGG.IsNull()) {
      
	TCollection_AsciiString aNmx("x0");
	Standard_CString name= aNmx.ToCString();
	//
	Handle (Geom_CartesianPoint) aGCP=
	Handle (Geom_CartesianPoint)::DownCast(aGG);
	//
	if (!aGCP.IsNull()) {
	  const gp_Pnt& aP3D=aGCP->Pnt();
	  DrawTrSurf::Set(name, aP3D);
	}
	else {
	  DrawTrSurf::Set(name, aGG);
	}
      }
    }
  }
  //
  else {
    di << " This shape seems to be OK." << "\n";
  }
 
  return 0;
}

static void MakeShapeForFullOutput(const TCollection_AsciiString & aBaseName,
                                   const Standard_Integer          aIndex,
                                   const TopTools_ListOfShape &    aList,
                                   Standard_Integer&               aCount,
				   Draw_Interpretor&               di)
{
  TCollection_AsciiString aNum(aIndex);
  TCollection_AsciiString aName = aBaseName + aNum;
  Standard_CString name = aName.ToCString();

  TopoDS_Compound cmp;
  BRep_Builder BB;
  BB.MakeCompound(cmp);

  TopTools_ListIteratorOfListOfShape anIt(aList);
  for(; anIt.More(); anIt.Next()) {
    const TopoDS_Shape & aS = anIt.Value();
    BB.Add(cmp, aS);
    aCount++;
  }
  di << "Made faulty shape: " << name << "\n";
  DBRep::Set(name, cmp);
}
                                   

Standard_Integer bopargcheck (Draw_Interpretor& di, Standard_Integer n,  const char** a )
{
  if (n<2) {
    di << "\n";
//     cout << " Use >bopargcheck Shape1 [[Shape2] [-F/O/C/T/S/U] [+R|F|T|V|E|I|P]] [#BF]" << endl << endl;
    di << " Use >bopargcheck Shape1 [[Shape2] [-F/O/C/T/S/U] [/R|F|T|V|E|I|P]] [#BF]" << "\n" << "\n";
    di << " -<Boolean Operation>" << "\n";
    di << " F (fuse)" << "\n";
    di << " O (common)" << "\n";
    di << " C (cut)" << "\n";
    di << " T (cut21)" << "\n";
    di << " S (section)" << "\n";
    di << " U (unknown)" << "\n";
    di << " For example: \"bopargcheck s1 s2 -F\" enables checking for Fuse operation" << "\n";
    di << " default - section" << "\n" << "\n";
//     cout << " +<Test Options>" << endl;
    di << " /<Test Options>" << "\n";
//     cout << " R (enable small edges (shrank range) test)" << endl;
    di << " R (disable small edges (shrank range) test)" << "\n";
//     cout << " F (enable faces verification test)" << endl;
    di << " F (disable faces verification test)" << "\n";
//     cout << " T (enable tangent faces searching test)" << endl;
    di << " T (disable tangent faces searching test)" << "\n";
//     cout << " V (test possibility to merge vertices)" << endl;
    di << " V (disable test possibility to merge vertices)" << "\n";
//     cout << " E (test possibility to merge edges)" << endl;
    di << " E (disable test possibility to merge edges)" << "\n";
    di << " I (disable self-interference test)" << "\n";
    di << " P (disable shape type test)" << "\n";
    di << " For example: \"bopargcheck s1 s2 /RI\" disables small edge detection and self-intersection detection" << "\n";
    di << " default - all options are enabled" << "\n" << "\n";
    di << " #<Additional Test Options>" << "\n";
    di << " B (stop test on first faulty found); default OFF" << "\n";
    di << " F (full output for faulty shapes); default - output in a short format" << "\n" << "\n";
    di << " NOTE: <Boolean Operation> and <Test Options> are used only for couple" << "\n";
    di << "       of argument shapes, except I and P options that are always used for" << "\n";
    di << "       couple of shapes as well as for single shape test." << "\n";
    return 1;
  }

  TopoDS_Shape aS1 = DBRep::Get(a[1]);

  if(aS1.IsNull()) {
    di << "Error: null shape not allowed!" << "\n";
    di << "Type bopargcheck without arguments for more information" << "\n";
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
  TopoDS_Shape aS2;
  if(isS2) {
    if(indxS2 != 2) {
      di << "Error: second shape should follow the first one!" << "\n";
      di << "Type bopargcheck without arguments for more information" << "\n";
      return 1;
    }
    else {
      aS2 = DBRep::Get(a[2]);
      if(aS2.IsNull()) {
        di << "Error: second shape is null!" << "\n";
        di << "Type bopargcheck without arguments for more information" << "\n";
        return 1;
      }
    }
  }

  // init checker
  BOP_ArgumentAnalyzer aChecker;
  aChecker.SetShape1(aS1);

  // set default options (always tested!) for single and couple shapes
  aChecker.ArgumentTypeMode() = Standard_True;
  aChecker.SelfInterMode()    = Standard_True;

  // test & set options and operation for two shapes
  if(!aS2.IsNull()) {
    aChecker.SetShape2(aS2);
    // set operation (default - Section)
    if(isBO) {
      if(a[indxBO][1] == 'F' || a[indxBO][1] == 'f') {
        aChecker.OperationType() = BOP_FUSE;
      }
      else if(a[indxBO][1] == 'O' || a[indxBO][1] == 'o') {
        aChecker.OperationType() = BOP_COMMON;
      }
      else if(a[indxBO][1] == 'C' || a[indxBO][1] == 'c') {
        aChecker.OperationType() = BOP_CUT;
      }
      else if(a[indxBO][1] == 'T' || a[indxBO][1] == 't') {
         aChecker.OperationType() = BOP_CUT21;
      }
      else if(a[indxBO][1] == 'S' || a[indxBO][1] == 's') {
         aChecker.OperationType() = BOP_SECTION;
      }
      else if(a[indxBO][1] == 'U' || a[indxBO][1] == 'u') {
        aChecker.OperationType() = BOP_UNKNOWN;
      }
      else {
        di << "Error: invalid boolean operation type!" << "\n";
        di << "Type bopargcheck without arguments for more information" << "\n";
        return 1;
      }
    }
    else
      aChecker.OperationType() = BOP_SECTION;

    aChecker.SmallEdgeMode()   = Standard_True;
    aChecker.RebuildFaceMode() = Standard_True;
    aChecker.TangentMode()     = Standard_True;
    aChecker.MergeVertexMode() = Standard_True;
    aChecker.MergeEdgeMode()   = Standard_True;

    // set options (default - all ON)
    if(isOP) {
      Standard_Integer ind = 1;
      while(a[indxOP][ind] != 0) {
        if(a[indxOP][ind] == 'R' || a[indxOP][ind] == 'r') {
          //aChecker.SmallEdgeMode() = Standard_True;
	  aChecker.SmallEdgeMode() = Standard_False;
        }
        else if(a[indxOP][ind] == 'F' || a[indxOP][ind] == 'f') {
          //aChecker.RebuildFaceMode() = Standard_True;
	  aChecker.RebuildFaceMode() = Standard_False;
        }
        else if(a[indxOP][ind] == 'T' || a[indxOP][ind] == 't') {
          //aChecker.TangentMode() = Standard_True;
	  aChecker.TangentMode() = Standard_False;
        }
        else if(a[indxOP][ind] == 'V' || a[indxOP][ind] == 'v') {
          //aChecker.MergeVertexMode() = Standard_True;
	  aChecker.MergeVertexMode() = Standard_False;
        }
        else if(a[indxOP][ind] == 'E' || a[indxOP][ind] == 'e') {
          //aChecker.MergeEdgeMode() = Standard_True;
	  aChecker.MergeEdgeMode() = Standard_False;
        }
        else if(a[indxOP][ind] == 'I' || a[indxOP][ind] == 'i') {
          aChecker.SelfInterMode() = Standard_False;
        }
        else if(a[indxOP][ind] == 'P' || a[indxOP][ind] == 'p') {
          aChecker.ArgumentTypeMode() = Standard_False;
        }
        else {
          di << "Error: invalid test option(s)!" << "\n";
          di << "Type bopargcheck without arguments for more information" << "\n";
          return 1;
        }
        ind++;
      }
    }
    else {
      // default test mode (all - ON)
      aChecker.SmallEdgeMode()   = Standard_True;
      aChecker.RebuildFaceMode() = Standard_True;
      aChecker.TangentMode()     = Standard_True;
      aChecker.MergeVertexMode() = Standard_True;
      aChecker.MergeEdgeMode()   = Standard_True;
    }
  }
  else {
    // check type and self-interference mode for single shape test
    // also check small edges and check faces
      aChecker.SmallEdgeMode()   = Standard_True;
      aChecker.RebuildFaceMode() = Standard_True;

     if(isOP) {
      Standard_Integer ind = 1;
      while(a[indxOP][ind] != 0) {
        if(a[indxOP][ind] == 'R' || a[indxOP][ind] == 'r') {
        }
        else if(a[indxOP][ind] == 'F' || a[indxOP][ind] == 'f') {
        }
        else if(a[indxOP][ind] == 'T' || a[indxOP][ind] == 't') {
        }
        else if(a[indxOP][ind] == 'V' || a[indxOP][ind] == 'v') {
        }
        else if(a[indxOP][ind] == 'E' || a[indxOP][ind] == 'e') {
        }
        else if(a[indxOP][ind] == 'I' || a[indxOP][ind] == 'i') {
          aChecker.SelfInterMode() = Standard_False;
        }
        else if(a[indxOP][ind] == 'P' || a[indxOP][ind] == 'p') {
          aChecker.ArgumentTypeMode() = Standard_False;
        }
        else {
          di << "Error: invalid test option(s)!" << "\n";
          di << "Type bopargcheck without arguments for more information" << "\n";
          return 1;
        }
        ind++;
      }
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
      const BOP_ListOfCheckResult& aResultList = aChecker.GetCheckResult();
      BOP_ListIteratorOfListOfCheckResult anIt(aResultList);

      Standard_Integer S1_BadType = 0, S1_SelfInt = 0, S1_SmalE = 0, S1_BadF = 0, S1_BadV = 0, S1_BadE = 0;
      Standard_Integer S1_SelfIntAll = 0, S1_SmalEAll = 0, S1_BadFAll = 0, S1_BadVAll = 0, S1_BadEAll = 0;
      Standard_Integer S2_BadType = 0, S2_SelfInt = 0, S2_SmalE = 0, S2_BadF = 0, S2_BadV = 0, S2_BadE = 0;
      Standard_Integer S2_SelfIntAll = 0, S2_SmalEAll = 0, S2_BadFAll = 0, S2_BadVAll = 0, S2_BadEAll = 0;
      Standard_Boolean hasUnknown = Standard_False;

      TCollection_AsciiString aS1SIBaseName("s1si_");
      TCollection_AsciiString aS1SEBaseName("s1se_");
      TCollection_AsciiString aS1BFBaseName("s1bf_");
      TCollection_AsciiString aS1BVBaseName("s1bv_");
      TCollection_AsciiString aS1BEBaseName("s1be_");
      TCollection_AsciiString aS2SIBaseName("s2si_");
      TCollection_AsciiString aS2SEBaseName("s2se_");
      TCollection_AsciiString aS2BFBaseName("s2bf_");
      TCollection_AsciiString aS2BVBaseName("s2bv_");
      TCollection_AsciiString aS2BEBaseName("s2be_");

      for(; anIt.More(); anIt.Next()) {
        const BOP_CheckResult& aResult = anIt.Value();
        const TopoDS_Shape & aSS1 = aResult.GetShape1();
        const TopoDS_Shape & aSS2 = aResult.GetShape2();
        const TopTools_ListOfShape & aLS1 = aResult.GetFaultyShapes1();
        const TopTools_ListOfShape & aLS2 = aResult.GetFaultyShapes2();
        Standard_Boolean isL1 = !aLS1.IsEmpty();
        Standard_Boolean isL2 = !aLS2.IsEmpty();

        switch(aResult.GetCheckStatus()) {
        case BOP_BadType: {
          if(!aSS1.IsNull()) S1_BadType++;
          if(!aSS2.IsNull()) S2_BadType++;
        }
	break;
        case BOP_SelfIntersect: {
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
        case BOP_TooSmallEdge: {
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
        case BOP_NonRecoverableFace: {
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
        case BOP_IncompatibilityOfVertex: {
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
        case BOP_IncompatibilityOfEdge: {
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
        case BOP_IncompatibilityOfFace: {
          // not yet implemented
        }
	break;
        case BOP_CheckUnknown:
        default: {
          hasUnknown = Standard_True;
        }
	break;
        } // switch
      }// faulties

      Standard_Integer FS1 = S1_SelfInt + S1_SmalE + S1_BadF + S1_BadV + S1_BadE;
      FS1 += (S1_BadType != 0) ? 1 : 0;
      Standard_Integer FS2 = S2_SelfInt + S2_SmalE + S2_BadF + S2_BadV + S2_BadE;
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

        // warning
        di << "\n";
        if(hasUnknown)
          di << "WARNING: The unexpected test break occurs!" << "\n";
      }
    } // full output
  } // has faulties

  return 0;
}
