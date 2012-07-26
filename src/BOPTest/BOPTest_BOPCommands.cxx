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
#include <DBRep_DrawableShape.hxx>

#include <Draw.hxx>
#include <Draw_Color.hxx>

#include <TCollection_AsciiString.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Builder.hxx>

#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BooleanOperations_StateOfShape.hxx>

#include <BOPTools_SolidStateFiller.hxx>
#include <BOPTools_DSFiller.hxx>
#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_PCurveMaker.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfVVInterference.hxx>
#include <BOPTools_CArray1OfVSInterference.hxx>
#include <BOPTools_CArray1OfVEInterference.hxx>
#include <BOPTools_CArray1OfESInterference.hxx>
#include <BOPTools_CArray1OfEEInterference.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BOPTools_VEInterference.hxx>
#include <BOPTools_VSInterference.hxx>
#include <BOPTools_EEInterference.hxx>
#include <BOPTools_ESInterference.hxx>
#include <BOPTools_SSInterference.hxx>

#include <BOPTest_DrawableShape.hxx>

#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>

#include <OSD_Chronometer.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

static OSD_Chronometer DRAW_BOP_CHRONO;
static void StartChrono();
static void StopChrono(Draw_Interpretor&); 
Standard_Integer btimesum (Draw_Interpretor& , Standard_Integer n, const char** a);
//XX

//
static   Standard_Integer bop        (Draw_Interpretor&,   Standard_Integer,   const char**);
//
static   Standard_Integer bcommon    (Draw_Interpretor& ,  Standard_Integer ,  const char**);
static   Standard_Integer bfuse      (Draw_Interpretor& ,  Standard_Integer ,  const char**);
static   Standard_Integer bcut       (Draw_Interpretor& ,  Standard_Integer ,  const char**);
static   Standard_Integer bsection   (Draw_Interpretor& ,  Standard_Integer ,  const char**);

static   Standard_Integer bFillDS    (Draw_Interpretor& ,  Standard_Integer  , const char**);
//
static   Standard_Integer bopcommon  (Draw_Interpretor& ,  Standard_Integer ,  const char**);
static   Standard_Integer bopfuse    (Draw_Interpretor& ,  Standard_Integer ,  const char**);
static   Standard_Integer bopcut     (Draw_Interpretor& ,  Standard_Integer ,  const char**);
static   Standard_Integer boptuc     (Draw_Interpretor& ,  Standard_Integer ,  const char**);
static   Standard_Integer bopsection (Draw_Interpretor& ,  Standard_Integer ,  const char**);

static   Standard_Integer boperationFiller (Standard_Integer n, const char** a, const BOP_Operation anOp,
					    Draw_Interpretor&);
//
static   Standard_Integer bopstates  (Draw_Interpretor& ,  Standard_Integer ,  const char**);
//
static  Standard_Integer bopwho      (Draw_Interpretor& ,  Standard_Integer ,  const char**);
static  Standard_Integer bopsticks   (Draw_Interpretor& ,  Standard_Integer ,  const char**);


static
  void UnUsedMap(BOPTools_SequenceOfCurves& ,
		 const BOPTools_PaveSet& ,
		 TColStd_IndexedMapOfInteger& );
//=======================================================================
//function : BOPCommands
//purpose  : 
//=======================================================================
  void  BOPTest::BOPCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) 
    return;

  done = Standard_True;
  // Chapter's name
  const char* g = "CCR commands";
  //
  // Using DSFiller that is ready.
  theCommands.Add("bop"       , "Use  >bop Shape1 Shape2", __FILE__,   bop, g);
  theCommands.Add("bopcommon" , "Use  >bopcommon R" , __FILE__, bopcommon , g);
  theCommands.Add("bopfuse"   , "Use  >bopfuse R"   , __FILE__, bopfuse   , g);
  theCommands.Add("bopcut"    , "Use  >bopcut R"    , __FILE__, bopcut    , g);
  theCommands.Add("boptuc"    , "Use  >boptuc R"    , __FILE__, boptuc    , g);
  theCommands.Add("bopsection", "Use  >bopsection R", __FILE__, bopsection, g);
  //
  // States
  theCommands.Add("bopstates",  "Use  bopstates [-f] [-t] [-out]", __FILE__, bopstates, g);
  //
  theCommands.Add("bcommon" , "Use >bcommon R a b"    , __FILE__, bcommon , g);
  theCommands.Add("bfuse"   , "Use >bfuse  R a b"     , __FILE__, bfuse   , g);
  theCommands.Add("bcut"    , "Use >bcut R a b"       , __FILE__, bcut    , g);
  // 
  //  bsection
  theCommands.Add("bsection", "Use >bsection Result s1 s2 [-2d/-2d1/-2d2] [-a]"  
		  , __FILE__, bsection, g);
  //
  theCommands.Add("btimesum"   , "Use >btimesum FileName"   , __FILE__, btimesum,  g);
  theCommands.Add("bopwho"     , "Use >bopwho Index"        , __FILE__, bopwho,    g);
  theCommands.Add("bopsticks"  , "Use >bopsticks"           , __FILE__, bopsticks, g);
  //
}
//
//////////////////////////////////
//
// The one and only global variable
//
BOPTools_DSFiller *pDF;
//
//////////////////////////////////
//
//=======================================================================
//function : bop
//purpose  : 
//=======================================================================
Standard_Integer bop (Draw_Interpretor& di, 
		      Standard_Integer n, 
		      const char** a)
{
  StartChrono();

  if (n<3) {
    di << " Use >bop Shape1 Shape2\n";
    StopChrono(di);
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[1]);
  TopoDS_Shape S2 = DBRep::Get(a[2]);
  
  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes are not allowed \n";
    StopChrono(di);
    return 1;
  }
  if (pDF!=NULL) {
    delete pDF;
    pDF=NULL;
  }

  pDF=new BOPTools_DSFiller;
  
  pDF->SetShapes (S1, S2);
  if (!pDF->IsDone()) {
    di << "Check types of the arguments, please\n";
    if (pDF!=NULL) {
      delete pDF;
      pDF=NULL;
    }
    StopChrono(di);
    return 1;
  }
  
  pDF->Perform();
  //printf(" BOPTools_DSFiller is Ready to use\n");
 
  StopChrono(di);
  
  return 0;
}

//=======================================================================
//function : bFillDS
//purpose  : 
//=======================================================================
Standard_Integer bFillDS (Draw_Interpretor& di, 
			  Standard_Integer n, 
			  const char** a)
{
  if (n!=4) {
    di << " Usage: >command  Result Shape1 Shape2\n";
    return 1;
  }
  
  return bop(di, n, a+1);
}

//=======================================================================
//function : bcommon
//purpose  : 
//=======================================================================
Standard_Integer bcommon (Draw_Interpretor& di, 
			  Standard_Integer n, 
			  const char** a)
{
  Standard_Integer iFlag;

  iFlag =bFillDS (di, n, a);

  if (iFlag) {
    return iFlag;
  }

  return bopcommon(di, n, a);
}
//=======================================================================
//function : bfuse
//purpose  : 
//=======================================================================
Standard_Integer  bfuse(Draw_Interpretor& di, 
			Standard_Integer n, 
			const char** a)
{
  Standard_Integer iFlag;

  iFlag =bFillDS (di, n, a);

  if (iFlag) {
    return iFlag;
  }

  return bopfuse(di, n, a);
}

//=======================================================================
//function : bcut
//purpose  : 
//=======================================================================
Standard_Integer  bcut(Draw_Interpretor& di, 
		       Standard_Integer n, 
		       const char** a)
{
  Standard_Integer iFlag;

  iFlag =bFillDS (di, n, a);

  if (iFlag) {
    return iFlag;
  }

  return bopcut(di, n, a);
}
//=======================================================================
//function : bsection
//purpose  : 
//=======================================================================
Standard_Integer  bsection(Draw_Interpretor& di, 
			   Standard_Integer n, 
			   const char** a)
{
  const char* usage = " Usage: bsection Result s1 s2 [-2d/-2d1/-2d2] [-a]\n";
  if (n < 4) {
    di << usage;
    return 1;
  }

  TopoDS_Shape S1 = DBRep::Get(a[2]);
  TopoDS_Shape S2 = DBRep::Get(a[3]);
  
  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes are not allowed \n";
    return 1;
  }


  BRepAlgoAPI_Section aSec(S1, S2, Standard_False);
  aSec.Approximation(Standard_False);
  aSec.ComputePCurveOn1(Standard_False);
  aSec.ComputePCurveOn2(Standard_False);
  Standard_Boolean isbadparameter = Standard_False;
  
  if(n > 4) {
    const char* key1 = a[4];
    const char* key2 = (n > 5) ? a[5] : NULL;
    const char* pcurveconf = NULL;
    Standard_Boolean approx = Standard_False;

#ifdef WNT
    if (key1 && !strcasecmp(key1,"-2d")) {
#else 
    if (key1 && !strncasecmp(key1,"-2d", 3)) {
#endif
      pcurveconf = key1;
    }
    else {
      if (!strcasecmp(key1,"-a")) {
	approx = Standard_True;
      }
      else {
	isbadparameter = Standard_True;
      }
    }
#ifdef WNT
    if (key2 && !strcasecmp(key2,"-2d")) {
#else 
    if (key2 && !strncasecmp(key2,"-2d", 3)) {
#endif
      if(pcurveconf) {
	isbadparameter = Standard_True;
      }
      else {
	pcurveconf = key2;
      }
    }
    else {
      if(key2) {
	if (!strcasecmp(key2,"-a")) {
	  approx = Standard_True;
	}
	else {
	  isbadparameter = Standard_True;
	}
      }
    }
    
    if(!isbadparameter && pcurveconf) {      

      if (strcasecmp(pcurveconf, "-2d") == 0) {
	aSec.ComputePCurveOn1(Standard_True);
	aSec.ComputePCurveOn2(Standard_True);
      }
      else {
	if (strcasecmp(pcurveconf, "-2d1") == 0) {
	  aSec.ComputePCurveOn1(Standard_True);
	}
	else {
	  if (strcasecmp(pcurveconf, "-2d2") == 0) {
	    aSec.ComputePCurveOn2(Standard_True);
	  }
	}
      }
    }
    aSec.Approximation(approx);
  }
      
  if(!isbadparameter) {	
    aSec.Build();
    Standard_Boolean anIsDone = aSec.IsDone();

    if (anIsDone) {
      const TopoDS_Shape& aR = aSec.Shape();
      DBRep::Set (a[1], aR);
    }
    else {
      di << "not done ErrorStatus()="<< aSec.ErrorStatus() <<"\n";
    }
  }
  else {
    di << usage;
    return 1;
  }
  return 0;
}

//=======================================================================
//function : bopcommon
//purpose  : 
//=======================================================================
Standard_Integer bopcommon (Draw_Interpretor& di, 
			    Standard_Integer n, 
			    const char** a)
{
  return boperationFiller (n, a, BOP_COMMON, di); 
}

//=======================================================================
//function : bopfuse
//purpose  : 
//=======================================================================
Standard_Integer  bopfuse(Draw_Interpretor& di, 
			    Standard_Integer n, 
			    const char** a)
{
  return boperationFiller (n, a, BOP_FUSE, di); 
}

//=======================================================================
//function : bopcut
//purpose  : 
//=======================================================================
Standard_Integer bopcut (Draw_Interpretor& di, 
			 Standard_Integer n, 
			 const char** a)
{
  return boperationFiller (n, a, BOP_CUT, di); 
}
//=======================================================================
//function : boptuc
//purpose  : 
//=======================================================================
Standard_Integer boptuc (Draw_Interpretor& di, 
			 Standard_Integer n, 
			 const char** a)
{
  return boperationFiller (n, a, BOP_CUT21, di); 
}
//=======================================================================
//function : bopsection
//purpose  : 
//=======================================================================
Standard_Integer bopsection (Draw_Interpretor& di, 
			     Standard_Integer n, 
			     const char** a)
{
  return boperationFiller (n, a, BOP_SECTION, di); 
}

//=======================================================================
//function : boperationFiller
//purpose  : 
//=======================================================================
Standard_Integer boperationFiller (Standard_Integer n, 
				   const char** a,
				   const BOP_Operation anOp,
				   Draw_Interpretor& di)
{
  //XX
  StartChrono();
  //XX

  if (n<2) {
    di << " Use oper> Result [-mf]\n";
    StopChrono(di);
    return 1;
  }
  
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    StopChrono(di);
    return 1;
  }

  const TopoDS_Shape& S1 = pDF->Shape1();
  const TopoDS_Shape& S2 = pDF->Shape2();

  if (S1.IsNull() || S2.IsNull()) {
    di << " Null shapes are not allowed \n";
    StopChrono(di);
    return 1;
  }

  Standard_Boolean aMFlag, anIsDone;

  aMFlag=Standard_False;
  //
  BRepAlgoAPI_BooleanOperation* pBuilder=NULL;
  // 
  // SECTION
  if (anOp==BOP_SECTION) {
    pBuilder=new BRepAlgoAPI_Section(S1, S2, *pDF);
  }
  else if (anOp==BOP_COMMON) {
    pBuilder=new BRepAlgoAPI_Common(S1, S2, *pDF);
  }
  else if (anOp==BOP_FUSE) {
    pBuilder=new BRepAlgoAPI_Fuse(S1, S2, *pDF);
  }
  else if (anOp==BOP_CUT) {
    pBuilder=new BRepAlgoAPI_Cut (S1, S2, *pDF);
  }
  else if (anOp==BOP_CUT21) {
    pBuilder=new BRepAlgoAPI_Cut(S1, S2, *pDF, Standard_False);
  }
  //
  anIsDone=pBuilder->IsDone();
  if (anIsDone) {
    const TopoDS_Shape& aR=pBuilder->Shape();
    DBRep::Set (a[1], aR);
  }
  else {
    di << "not done ErrorStatus()=" << pBuilder->ErrorStatus() << "\n";
  }
  
  delete pBuilder;

  StopChrono(di);

  return 0;
}

//=======================================================================
// OTHER FUNCTIONS
//
static 
  void GetName (const BooleanOperations_StateOfShape aState,
		TCollection_AsciiString& aNm);
static  
  TopAbs_ShapeEnum ChooseShapeType(const char* a1);

static
  BooleanOperations_StateOfShape ChooseShapeState(const char* a3);

//=======================================================================
//function : bopstates
//purpose  : 
//=======================================================================
Standard_Integer  bopstates(Draw_Interpretor& di, 
			     Standard_Integer n, 
			     const char** a)
{
  if (n>4) {
    di << " Use bopstates [-f] [-t] [-out]\n";
    return 1;
  }
  
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }

  Standard_Integer i, aNbLines, aIsSmtToDraw=0;
  BooleanOperations_StateOfShape aState, aChooseState;
  aChooseState=BooleanOperations_UNKNOWN;
  TopAbs_ShapeEnum aEnumToDisplay = TopAbs_COMPOUND;
  Draw_Color aTextColor(Draw_cyan);
  //
  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();
  //
  // Choose Object or Tool or All
  i=1;
  aNbLines=aDS.NumberOfSourceShapes();
  if (n>=3) {
    if (!strcmp (a[2], "-o")) {
      aDS.ObjectRange(i, aNbLines);
    }
    if (!strcmp (a[2], "-t")) {
      aDS.ToolRange(i, aNbLines);
    }
  }
  //
  // ChooseShapeState
  if (n==4) {
    aChooseState=ChooseShapeState(a[3]);
  }
  //
  // ChooseShapeType
  if (n==1) {
    aEnumToDisplay=TopAbs_EDGE;
  }
  else if (n>1) {
    aEnumToDisplay=ChooseShapeType(a[1]);
  }
  //
  // Presentation
  for (; i<=aNbLines; ++i) {
    const TopoDS_Shape& aS=aDS.GetShape(i);
    TopAbs_ShapeEnum aCurrentType=aS.ShapeType();
    if (aCurrentType==aEnumToDisplay) {
      aState=aDS.GetState(i);
      TCollection_AsciiString aNm, aInd(i);
      GetName(aState, aNm);

      if (aState==BooleanOperations_INTERSECTED && aCurrentType==TopAbs_EDGE) {
	
	const BOPTools_SplitShapesPool& aSpPool=pDF->SplitShapesPool();
	const BOPTools_ListOfPaveBlock& aSplitsList=aSpPool(aDS.RefEdge(i));
	BOPTools_ListIteratorOfListOfPaveBlock anIt(aSplitsList);
	for (; anIt.More();anIt.Next()) {
	  const BOPTools_PaveBlock& aPB=anIt.Value();
	  Standard_Integer nSplit=aPB.Edge();
	  const TopoDS_Shape& aSplit=aDS.GetShape(nSplit);
	  aState=aDS.GetState(nSplit);
	  GetName(aState, aNm);
	  aNm=aNm+aInd;
	  TCollection_AsciiString aNmx, anUnd("_"), aIndx(nSplit);
	  aNmx=aNm+anUnd;
	  aNmx=aNmx+aIndx;
	  
	  Standard_CString aTxt=aNmx.ToCString();
	  
	  if (aChooseState!=BooleanOperations_UNKNOWN) {
	    if (aState!=aChooseState) {
	      continue;
	    }
	  }
	  aIsSmtToDraw++;
	  //printf("%s ",  aTxt);
	  di << aTxt << " ";
	  Handle(BOPTest_DrawableShape) aDSh=
	    new BOPTest_DrawableShape (aSplit, aTxt, aTextColor);
	  Draw::Set (aTxt, aDSh);
	}
	  continue;
      }

      aNm=aNm+aInd;
      Standard_CString aText=aNm.ToCString();
      
      if (aChooseState!=BooleanOperations_UNKNOWN) {
	if (aState!=aChooseState) {
	  continue;
	}
      }
      aIsSmtToDraw++;
      //printf("%s ",  aText);
      di << aText << " ";
      Handle(BOPTest_DrawableShape) aDShape=
	new BOPTest_DrawableShape (aS, aText, aTextColor);
      Draw::Set (aText, aDShape);
    }
  }

  if (!aIsSmtToDraw) {
    di << " No specified shapes\n"; 
  }
  else {
    di << "\n";
  }


  return 0;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================
void GetName (const BooleanOperations_StateOfShape aState,
	      TCollection_AsciiString& aNm)
{
  TCollection_AsciiString aNmOut("ou_"), aNmIn("in_"), 
       aNmOn("on_"), aNmUn("un_"), aNmInter("intr_");

  switch (aState) {
    case BooleanOperations_OUT:
      aNm=aNmOut;
      break;
    case  BooleanOperations_IN:
      aNm=aNmIn; 
      break;
    case BooleanOperations_ON:
      aNm=aNmOn;
      break;
    case BooleanOperations_INTERSECTED:
      aNm=aNmInter;
      break;
    default:
      aNm=aNmUn;
      break;
  }
}

//=======================================================================
//function : ChooseShapeType
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum ChooseShapeType(const char* a1)
{
  TopAbs_ShapeEnum aEnumToDisplay;

  if (!strcmp (a1, "-c")) {
    aEnumToDisplay=TopAbs_COMPOUND;
  }
  else if (!strcmp (a1, "-cs")) {
    aEnumToDisplay=TopAbs_COMPSOLID;
  }
  else if (!strcmp (a1, "-s")) {
    aEnumToDisplay=TopAbs_SOLID;
  }
  else if (!strcmp (a1, "-sh")) {
    aEnumToDisplay=TopAbs_SHELL;
  }
  else if (!strcmp (a1, "-f")) {
    aEnumToDisplay=TopAbs_FACE;
  }
  else if (!strcmp (a1, "-w")) {
    aEnumToDisplay=TopAbs_WIRE;
  }
  else if (!strcmp (a1, "-e")) {
    aEnumToDisplay=TopAbs_EDGE;
  }
  else if (!strcmp (a1, "-v")) {
    aEnumToDisplay=TopAbs_VERTEX;
  }
  else {
    aEnumToDisplay=TopAbs_EDGE;
  }
  return aEnumToDisplay;
}

//=======================================================================
//function : ChooseShapeState
//purpose  : 
//=======================================================================
BooleanOperations_StateOfShape ChooseShapeState(const char* a3)
{ 
  BooleanOperations_StateOfShape aChooseState=BooleanOperations_UNKNOWN;

  if (!strcmp (a3, "-out")) {
    aChooseState=BooleanOperations_OUT;
  }
  if (!strcmp (a3, "-in")) {
    aChooseState=BooleanOperations_IN;
  }
  if (!strcmp (a3, "-on")) {
    aChooseState=BooleanOperations_ON;
  }
  return aChooseState;
}
//XX
// CHRONOMETER
//
#include <OSD_Chronometer.hxx>
#include <TCollection_AsciiString.hxx>
#include <OSD_OpenMode.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <OSD_File.hxx>

static void SaveTimeInFile(const Standard_CString aFileName,
			   const Standard_Real aChrono,
			   Draw_Interpretor& di);

//=======================================================================
//function : StartChrono
//purpose  : 
//=======================================================================
void StartChrono() 
{
  char *xr=getenv ("BOPCHRONO");
  if (xr!=NULL){
    if (!strcmp (xr, "yes")){
      DRAW_BOP_CHRONO.Reset();
      DRAW_BOP_CHRONO.Start();
    }
  }
}

//=======================================================================
//function : StopChrono
//purpose  : 
//=======================================================================
void StopChrono(Draw_Interpretor& di) 
{ 
  char *xr=getenv ("BOPCHRONO");
  if (xr!=NULL){
    if (!strcmp (xr, "yes")) {
      Standard_Real Chrono;
      DRAW_BOP_CHRONO.Stop();
      DRAW_BOP_CHRONO.Show(Chrono);
      //
      char *aFileName=getenv("BOPCHRONOFILE");
      if (aFileName!=NULL){
	SaveTimeInFile(aFileName, Chrono, di);
      }
      else {
	di << "Tps: " << Chrono << "\n";
      }
      //
    }
  }
}
//=======================================================================
//function : SaveTimeInFile
//purpose  : 
//=======================================================================
void SaveTimeInFile(const Standard_CString aFileName,
		    const Standard_Real aChrono,
		    Draw_Interpretor& di)
{
  OSD_OpenMode aMode =  OSD_ReadWrite;
  OSD_Protection  aProtect(OSD_RW,OSD_RW,OSD_RW,OSD_RW);
  TCollection_AsciiString anASName(aFileName);
  OSD_Path aPath (anASName);
  OSD_File aFile (aPath);
  aFile.Append(aMode, aProtect);
  if (!( aFile.Exists() && aFile.IsOpen())) { 
    Standard_CString aStr=anASName.ToCString();
    di << "Can not open the file: " << aStr << "\n";
    return;
  }
  TCollection_AsciiString aASTime(aChrono), aASendl("\n");
  aASTime=aASTime+aASendl;
  aFile.Write (aASTime, aASTime.Length());
  aFile.Close();
}
//XX
//=======================================================================
//function : btimesum
//purpose  : 
//=======================================================================
Standard_Integer btimesum (Draw_Interpretor& di, 
			   Standard_Integer n, 
			   const char** a)
{
  if (n!=2) {
    di << " Use >btimesum FileName\n";
    return 1;
  }
   
  TCollection_AsciiString anASName(a[1]);
  OSD_OpenMode aMode =  OSD_ReadWrite;
  OSD_Protection  aProtect(OSD_RW,OSD_RW,OSD_RW,OSD_RW);
  OSD_Path aPath (anASName);
  OSD_File aFile (aPath);
  aFile.Open(aMode, aProtect);

  if (!( aFile.Exists() && aFile.IsOpen())) { 
    Standard_CString aStr=anASName.ToCString();
    di << "Can not open the file: " << aStr << "\n";
    return 1;
  }

  Standard_Integer aNbyteRead, aNByte=256, pos;
  Standard_Real aTC, aTime=0.;
  while (1) {
    if (aFile.IsAtEnd()) {
      break;
    }
    
    TCollection_AsciiString aStr(aNByte);
    aFile.ReadLine(aStr, aNByte, aNbyteRead);
    //Standard_CString pStr=aStr.ToCString();

    pos=aStr.Search("\n");
    if (pos>0){
      aStr.Trunc(pos-1);
    }
    
    if (aStr.IsRealValue()) {
      aTC=aStr.RealValue();
      aTime=aTime+aTC;
    }
  }
  aFile.Close();
  //
  aFile.Append(aMode, aProtect);

  TCollection_AsciiString aASLine("----------\n"), aASTime(aTime), aASendl("\n");
  aASTime=aASTime+aASendl;
  aFile.Write (aASLine, aASLine.Length());
  aFile.Write (aASTime, aASTime.Length());
  aFile.Close();
  
  return 0;
}


//=======================================================================
//
//                         ex. BOPTest_TSTCommands
//
#include <stdio.h>

#include <BOPTest_DrawableShape.hxx>

#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>

#include <Draw.hxx>
#include <Draw_Color.hxx>

#include <TCollection_AsciiString.hxx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>


#include <BooleanOperations_ShapesDataStructure.hxx>
#include <BOPTools_CArray1OfInterferenceLine.hxx>
#include <BOPTools_InterferenceLine.hxx>
#include <BOPTools_ListOfInterference.hxx>
#include <BOPTools_ListIteratorOfListOfInterference.hxx>
#include <BOPTools_Interference.hxx>
#include <BOPTools_InterferencePool.hxx>

#include <BOPTools_DSFiller.hxx>
#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>

#include <BOPTools_PavePool.hxx>
#include <BOPTools_PaveSet.hxx>
#include <BOPTools_ListOfPave.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>

#include <BOPTools_CommonBlockPool.hxx>
#include <BOPTools_ListOfCommonBlock.hxx>
#include <BOPTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_CommonBlock.hxx>
#include <BOPTools_PaveFiller.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>

#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>


static Standard_Integer bopinterf (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopds     (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopsplits (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopscts   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopsamedomain(Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopaves   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopsinf   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopsonf   (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bopnews   (Draw_Interpretor& ,Standard_Integer, const char**);

//=======================================================================
//function : TSTCommands
//purpose  : 
//=======================================================================
  void  BOPTest::TSTCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "CCR commands";
  //
  theCommands.Add("bopinterf" , "", __FILE__, bopinterf, g);
  theCommands.Add("bopds"     , "Use  bopds [-sw]", __FILE__, bopds, g);

  theCommands.Add("bopsplits" , "", __FILE__, bopsplits, g);
  theCommands.Add("bopscts"   , "", __FILE__, bopscts, g);

  theCommands.Add("bopsamedomain" , "", __FILE__, bopsamedomain, g);//SameDomain Edges
  theCommands.Add("bopaves"    , "", __FILE__, bopaves, g);
  theCommands.Add("bopnews"    , "bopnews [-f,w,e,v]", __FILE__, bopnews, g);
  //
  // All Splits of face1 IN face2  
  theCommands.Add("bopsinf" , "Use bopsinf #F1 #F2", __FILE__, bopsinf, g);
  //
  // All Splits of face1 ON face2  
  theCommands.Add("bopsonf" , "Use bopsinf #F1 #F2", __FILE__, bopsonf, g);
}


//=======================================================================
//function : bopinterf
//purpose  : 
//=======================================================================
Standard_Integer bopinterf (Draw_Interpretor& di, 
			    Standard_Integer /*n*/, 
			    const char** /*a*/)
{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }

  Standard_Integer i, aNbLines;
  //
  // InterferenceTable
  Standard_Integer aWith, anIndex, aFst, aLF;
  BooleanOperations_KindOfInterference aType;

  const BOPTools_InterferencePool& anInterferencePool=pDF->InterfPool();
  const BOPTools_CArray1OfInterferenceLine& aTableIL=anInterferencePool.InterferenceTable();
  
  aNbLines=aTableIL.Extent();
  for (i=1; i<=aNbLines; ++i) {
    aFst=0;
    aLF=0;
    const BOPTools_InterferenceLine& aIL=aTableIL(i);
    const BOPTools_ListOfInterference& aIList=aIL.List();
    BOPTools_ListIteratorOfListOfInterference anIt(aIList);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_Interference& anInterf=anIt.Value();
      aWith=anInterf.With();
      aType=anInterf.Type();
      anIndex=anInterf.Index();
      if (anIndex) {
	if (aFst==0){
	  //printf(" #%d ", i);
	  di << " #" << i << " ";
	  aFst=1;
	}
	aLF=1;
	//printf("(%d, %d, %d),", aWith, aType, anIndex);
	di << "(" << aWith << ", " << aType << ", " << anIndex << "),";
      }
    }
    if (aLF) {
      //printf("\n");
      di << "\n";
    }
  }
  return 0;
}

//=======================================================================
//function : bopds
//purpose  : 
//=======================================================================
Standard_Integer bopds (Draw_Interpretor& di, 
			Standard_Integer n, 
			const char** a)
{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }

  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();
  Standard_Integer i, aNbLines;
  
  i=1;
  aNbLines=aDS.NumberOfSourceShapes();
  
  if (n==3) {
    if (!strcmp (a[2], "-o")) {
      aDS.ObjectRange(i, aNbLines);
    }
    if (!strcmp (a[2], "-t")) {
      aDS.ToolRange(i, aNbLines);
    }
  }

  Draw_Color aTextColor(Draw_cyan);
  TCollection_AsciiString aNm("z");

  for (; i<=aNbLines; ++i) {
    
    const TopoDS_Shape& aS=aDS.GetShape(i);
    //
    // Prestentation
    TopAbs_ShapeEnum aType=aS.ShapeType();
    if (n==1) {
      if (aType==TopAbs_COMPOUND ||
	  aType==TopAbs_COMPSOLID ||
	  aType==TopAbs_SOLID ||
	  aType==TopAbs_SHELL || 
	  aType==TopAbs_FACE  || 
	  aType==TopAbs_WIRE) {
	continue;
      }
    }

    if (n>=2) {
      if (!strcmp (a[1], "-c")) {
	if (aType!=TopAbs_COMPOUND) {
	  continue;
	}
      }
      else if (!strcmp (a[1], "-cs")) {
	if (aType!=TopAbs_COMPSOLID) {
	  continue;
	}
      }
      else if (!strcmp (a[1], "-s")) {
	if (aType!=TopAbs_SOLID) {
	  continue;
	}
      }
      else if (!strcmp (a[1], "-sh")) {
	if (aType!=TopAbs_SHELL) {
	  continue;
	}
      }
      else if (!strcmp (a[1], "-f")) {
	if (aType!=TopAbs_FACE) {
	  continue;
	}
      }
      else if (!strcmp (a[1], "-w")) {
	if (aType!=TopAbs_WIRE) {
	  continue;
	}
      }
     else  if (!strcmp (a[1], "-e")) {
	if (aType!=TopAbs_EDGE) {
	  continue;
	}
      }
      else if (!strcmp (a[1], "-v")) {
	if (aType!=TopAbs_VERTEX) {
	  continue;
	}
      }
      else {
	continue;
      }
    }

    TCollection_AsciiString aInd(i), aName;
    aName=aNm+aInd;
    Standard_CString aText=aName.ToCString();

    Handle(BOPTest_DrawableShape) aDShape=
      new BOPTest_DrawableShape (aS, aText, aTextColor);
    Draw::Set (aText, aDShape);
  }
  return 0;
}

//=======================================================================
//function : bopaves
//purpose  : 
//=======================================================================
Standard_Integer  bopaves(Draw_Interpretor& di, 
			  Standard_Integer /*n*/, 
			  const char** /*a*/)
{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  } 
  Standard_Integer i, aNbLines, anIndex;
  Standard_Real aParam;

  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();
  const BOPTools_PavePool& aPavePool=pDF->PavePool();

  aNbLines=aDS.NumberOfSourceShapes();
  for (i=1; i<=aNbLines; ++i) {
    const TopoDS_Shape& aS=aDS.GetShape(i);
    if (aS.ShapeType()==TopAbs_EDGE) {
      //printf(" Edge#%d\n", i);
      di << " Edge#" << i << "\n";
      const BOPTools_PaveSet& aPaveSet=aPavePool(aDS.RefEdge(i));
      const BOPTools_ListOfPave& aLP= aPaveSet.Set();
      BOPTools_ListIteratorOfListOfPave anIt(aLP);
      for (; anIt.More(); anIt.Next()){
	const BOPTools_Pave& aPave=anIt.Value();
	anIndex=aPave.Index();
	aParam =aPave.Param();
	//printf(" VertIndex=%d, aParam=%f\n", anIndex, aParam);
	di << " VertIndex=" << anIndex << ", aParam=" << aParam << "\n";
      }
    }
  }
  return 0;
}

//=======================================================================
//function : bopsd
//purpose  : SameDomain Edges
//=======================================================================
Standard_Integer bopsamedomain (Draw_Interpretor& di, 
				Standard_Integer /*n*/, 
				const char** /*a*/)
{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  } 
  TCollection_AsciiString aNm("s");
  
  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();
  const BOPTools_CommonBlockPool& aCommonBlockPool=pDF->CommonBlockPool();

  Standard_Integer i, aNbLines, nFSD, nESD1, nESD2, nOriginal2;

  aNbLines=aDS.NumberOfSourceShapes();
  for (i=1; i<=aNbLines; ++i) {
    const TopoDS_Shape& aSE=aDS.GetShape(i);
    TopAbs_ShapeEnum aType=aSE.ShapeType();
    if (aType==TopAbs_EDGE) {
      
      const BOPTools_ListOfCommonBlock& aLCB=aCommonBlockPool(aDS.RefEdge(i));
      
      if (aLCB.Extent()) {
	//
	BOPTools_ListIteratorOfListOfCommonBlock anIt(aLCB);
	for (; anIt.More(); anIt.Next()) {
	  const BOPTools_CommonBlock& aCB=anIt.Value();
	  nFSD=aCB.Face();
	  if (nFSD) {
	    const BOPTools_PaveBlock& aPB1=aCB.PaveBlock1();
	    nESD1=aPB1.Edge();
	    
	    TCollection_AsciiString aInd(i), anUnd("_"), aName;
	    aName=aNm+aInd;
	    aName=aName+anUnd;
	    aName=aName+nESD1;
	    Standard_CString aText=aName.ToCString();
	    
	    //printf(" (EF %s  %df),", aText, nFSD);
	    di << " (EF " << aText << "  " << nFSD << "f),";
	  }

	  else {
	    const BOPTools_PaveBlock& aPB1=aCB.PaveBlock1();
	    nESD1=aPB1.Edge();

	    TCollection_AsciiString aInd(i), anUnd("_"), aName;
	    aName=aNm+aInd;
	    aName=aName+anUnd;
	    aName=aName+nESD1;
	    Standard_CString aText=aName.ToCString();

	    const BOPTools_PaveBlock& aPB2=aCB.PaveBlock2();
	    nESD2=aPB2.Edge();
	    nOriginal2=aPB2.OriginalEdge();

	    TCollection_AsciiString aInd2(nOriginal2), aName2;
	    aName2=aNm+aInd2;
	    aName2=aName2+anUnd;
	    aName2=aName2+nESD2;
	    Standard_CString aText2=aName2.ToCString();

	    //printf(" (EE %s %s ),", aText, aText2);
	    di << " (EE " << aText << " " << aText2 << " ),";
	  }
	}
	//printf("\n");
	di << "\n";
      }
    }
  }
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
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }

  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();

  Standard_Integer i, iStart, aNbLinesTotal;
  Draw_Color aTextColor(Draw_cyan);
  TCollection_AsciiString aNm("z");
  //

  TopAbs_ShapeEnum aType=TopAbs_EDGE;
  if (n>1) {
    if (!strcmp (a[1], "-f")) {
      aType=TopAbs_FACE;
    }
    else if (!strcmp (a[1], "-w")) {
      aType=TopAbs_WIRE;
    }
    else  if (!strcmp (a[1], "-e")) {
      aType=TopAbs_EDGE;
    }
    else if (!strcmp (a[1], "-v")) {
      aType=TopAbs_VERTEX;
    }
  }
  
  //
  iStart=aDS.NumberOfSourceShapes()+1;
  aNbLinesTotal =aDS.NumberOfInsertedShapes();

  for (i=iStart; i<=aNbLinesTotal; ++i) {
    const TopoDS_Shape& aS=aDS.Shape(i);
    TopAbs_ShapeEnum aTypeCurrent=aS.ShapeType();
    if (aTypeCurrent==aType) {
      TCollection_AsciiString aName, aInd(i);
      aName=aNm+aInd;
      Standard_CString aText=aName.ToCString();
      
      Handle(BOPTest_DrawableShape) aDShape=
	new BOPTest_DrawableShape (aS, aText, aTextColor);
      Draw::Set (aText, aDShape);
      //printf("%s ", aText);
      di << aText << " ";
    }
  }
  
  if (iStart>aNbLinesTotal) {
    di << " No new shapes occured";
  }

  di << "\n";
  return 0;
}

//=======================================================================
//function : bopsplits
//purpose  : 
//=======================================================================
Standard_Integer bopsplits (Draw_Interpretor& di, 
			    Standard_Integer /*n*/, 
			    const char** /*a*/)

{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }

  const BOPTools_SplitShapesPool& aSpPool=pDF->SplitShapesPool();
  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();

  Standard_Integer i, aNbLines, aNbSplits, aSplitExist=0;
  Draw_Color aTextColor(Draw_cyan);
  TCollection_AsciiString aNm("s");

  aNbLines=aDS.NumberOfSourceShapes();
  for (i=1; i<=aNbLines; ++i) {
    const TopoDS_Shape& aSE=aDS.GetShape(i);
    TopAbs_ShapeEnum aType=aSE.ShapeType();
    if (aType==TopAbs_EDGE) {
      //
      // Splits' Prestentation
      const BOPTools_ListOfPaveBlock& aSplitsList=aSpPool(aDS.RefEdge(i));
      aNbSplits=aSplitsList.Extent();
      if (aNbSplits) {
	if (!aSplitExist) {
	  di << " Splits: ";
	  aSplitExist=1;
	}

	BOPTools_ListIteratorOfListOfPaveBlock anIt(aSplitsList);
	for (; anIt.More();anIt.Next()) {
	  const BOPTools_PaveBlock& aPB=anIt.Value();
	  Standard_Integer nSplit=aPB.Edge();
	  const TopoDS_Shape& aS=aDS.GetShape(nSplit);

	  TCollection_AsciiString aInd(i), anUnd("_"), aName;
	  aName=aNm+aInd;
	  aName=aName+anUnd;
	  aName=aName+nSplit;
	  Standard_CString aText=aName.ToCString();
	  
	  Handle(BOPTest_DrawableShape) aDShape=
	    new BOPTest_DrawableShape (aS, aText, aTextColor);
	  Draw::Set (aText, aDShape);
	  di << aText << " ";
	}
      }
    }
  }
  
  if (!aSplitExist) {
    di << " No splits occured";
  }
  di << "\n";
  return 0;
}

//=======================================================================
//function : bopscts
//purpose  : 
//=======================================================================
Standard_Integer bopscts (Draw_Interpretor& di, 
			  Standard_Integer /*n*/, 
			  const char** /*a*/)

{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }
  Standard_Integer i, nF1, nF2,  aNbFFs, aNbOldSects, aSectExist=0, nSect;
  Standard_Integer j, aNbCurves, aNbPaveBlocks, nNewEdge;
  Draw_Color aTextColor(Draw_cyan);
  TCollection_AsciiString aNm("t");

  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();
  const BOPTools_InterferencePool& anInterfPool=pDF->InterfPool();
  BOPTools_InterferencePool* pInterfPool= (BOPTools_InterferencePool*) &anInterfPool;
  BOPTools_CArray1OfSSInterference& aFFs=pInterfPool->SSInterferences();
 
  aNbFFs=aFFs.Extent();
  for (i=1; i<=aNbFFs; ++i) {
    BOPTools_SSInterference& aFFi=aFFs(i);
    //
    nF1=aFFi.Index1();
    nF2=aFFi.Index2();
    TCollection_AsciiString aInd(nF1), anUnd("_");
    //
    // Old Section Edges
    const BOPTools_ListOfPaveBlock& aSectList=aFFi.PaveBlocks();
    aNbOldSects=aSectList.Extent();
    if (aNbOldSects) {
      if (!aSectExist) {
	di << " Sects: ";
	aSectExist=1;
      }

      BOPTools_ListIteratorOfListOfPaveBlock anIt(aSectList);
      for (; anIt.More();anIt.Next()) {
	const BOPTools_PaveBlock& aPB=anIt.Value();
	nSect=aPB.Edge();
	const TopoDS_Shape& aS=aDS.GetShape(nSect);
	
	TCollection_AsciiString aName;
	aName=aNm+aInd;
	aName=aName+anUnd;
	aName=aName+nSect;
	Standard_CString aText=aName.ToCString();
	
	Handle(BOPTest_DrawableShape) aDShape=
	  new BOPTest_DrawableShape (aS, aText, aTextColor);
	Draw::Set (aText, aDShape);
	di << aText << " ";
      }
    }
    //
    // New Section Edges
    BOPTools_SequenceOfCurves& aBCurves=aFFi.Curves();
    aNbCurves=aBCurves.Length();
    for (j=1; j<=aNbCurves; j++) {
      BOPTools_Curve& aBC=aBCurves(j);
      const BOPTools_ListOfPaveBlock& aSectEdges=aBC.NewPaveBlocks();
      aNbPaveBlocks=aSectEdges.Extent();
      if (aNbPaveBlocks) {
	if (!aSectExist) {
	  di << " Sects: ";
	  aSectExist=1;
	}
      }
      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSectEdges);
      for (; aPBIt.More(); aPBIt.Next()) {
	BOPTools_PaveBlock& aPB=aPBIt.Value();
	nNewEdge=aPB.Edge();
	const TopoDS_Shape& aSectEdge=aDS.GetShape(nNewEdge);

	TCollection_AsciiString aName;
	aName=aNm+aInd;
	aName=aName+anUnd;
	aName=aName+nNewEdge;
	Standard_CString aText=aName.ToCString();
	
	Handle(BOPTest_DrawableShape) aDShape=
	  new BOPTest_DrawableShape (aSectEdge, aText, aTextColor);
	Draw::Set (aText, aDShape);
	di << aText << " ";
      }
    }
  }
  
  if (!aSectExist) {
    di << " No section edges";
  }
  di << "\n";
  return 0;
}

//=======================================================================
//function : bopsinf
//purpose  : 
//=======================================================================
Standard_Integer bopsinf (Draw_Interpretor& di, 
			  Standard_Integer n, 
			  const char** a)
{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }

  if (n!=3) {
    di << " Use: bopsinf> #F1 #F2\n";
    return 0;
  }

  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();

  Standard_Integer nSp=0, nF1, nF2, anExistFlag=0;
  nF1=atoi(a[1]);
  nF2=atoi(a[2]);

  const TopoDS_Shape& aS1=aDS.GetShape(nF1);
  const TopoDS_Shape& aS2=aDS.GetShape(nF2);
  if (aS1.IsNull() || aS2.IsNull()) {
    //printf(" Null shapes #%d #%d\n", nF1, nF2 );
    di << " Null shapes #" << nF1 << " #" << nF2 << "\n";
    return 0;
  }
  
  if (aS1.ShapeType()!=TopAbs_FACE ||
      aS2.ShapeType()!=TopAbs_FACE ) {
    //printf(" Shapes #%d #%d are not faces\n", nF1, nF2 );
    di << " Shapes #" << nF1 << " #" << nF2 << " are not faces\n";
    return 0;
  }

  TColStd_ListOfInteger aSplList;
  const BOPTools_PaveFiller& aPF=pDF->PaveFiller();
  BOPTools_PaveFiller* pPF=(BOPTools_PaveFiller*)&aPF;

  pPF->SplitsInFace (nSp, nF1, nF2, aSplList);
  
  TColStd_ListIteratorOfListOfInteger anIt (aSplList);
  for (; anIt.More(); anIt.Next()) {
    anExistFlag=1;
    nSp=anIt.Value();
    //printf("%d, ", nSp);
    di << nSp << ", ";
  }
  
  if(anExistFlag) {
    di << "\n";
  }
  else {
    di << "No splits of Face" << nF1 << " IN Face " << nF2 << "\n";
  }

return 0;  
}
//=======================================================================
//function : bopsonf
//purpose  : 
//=======================================================================
Standard_Integer bopsonf (Draw_Interpretor& di, 
			  Standard_Integer n, 
			  const char** a)
{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }

  if (n!=3) {
    di << " Use: bopsonf> #F1 #F2\n";
    return 0;
  }

  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();

  Standard_Integer nSp=0, nF1, nF2, anExistFlag=0;
  nF1=atoi(a[1]);
  nF2=atoi(a[2]);

  const TopoDS_Shape& aS1=aDS.GetShape(nF1);
  const TopoDS_Shape& aS2=aDS.GetShape(nF2);
  if (aS1.IsNull() || aS2.IsNull()) {
    //printf(" Null shapes #%d #%d\n", nF1, nF2 );
    di << " Null shapes #" << nF1 << " #" << nF2 << "\n";
    return 0;
  }
  
  if (aS1.ShapeType()!=TopAbs_FACE ||
      aS2.ShapeType()!=TopAbs_FACE ) {
    //printf(" Shapes #%d #%d are not faces\n", nF1, nF2 );
    di << " Shapes #" << nF1 << " #" << nF2 << " are not faces\n";
    return 0;
  }

  TColStd_ListOfInteger aSplList;
  const BOPTools_PaveFiller& aPF=pDF->PaveFiller();
  BOPTools_PaveFiller* pPF=(BOPTools_PaveFiller*)&aPF;

  pPF->SplitsOnFace (nSp, nF1, nF2, aSplList);
  
  TColStd_ListIteratorOfListOfInteger anIt (aSplList);
  for (; anIt.More(); anIt.Next()) {
    anExistFlag=1;
    nSp=anIt.Value();
    //printf("%d, ", nSp);
    di << nSp << ", ";
  }
  
   if(anExistFlag) {
    di << "\n";
  }
  else {
    di << "No splits of Face" << nF1 << " ON Face " << nF2 << "\n";
  }

  return 0;  
}

//=======================================================================
//function : bopwho
//purpose  : 
//=======================================================================
Standard_Integer bopwho (Draw_Interpretor& di, 
			 Standard_Integer n, 
			 const char** a)
{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 0;
  }

  if (n!=2) {
    di << " Use >bopwho Index\n";
    return 1;
  }
  //
  Standard_Boolean bIsNewShape;
  Standard_Integer iNum, i, aNbLines, aNbObj, aNewShape, aWhat, aWith;
  //
  iNum=atoi(a[1]);
  if (iNum<=0) {
    di << " Shape Index must be >0 .\n";
    return 1;
  }
  //
  //printf("Shape #%d is ", iNum);
  di << "Shape #" << iNum << " is ";

  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();
  aNbLines=aDS.NumberOfSourceShapes();
  //
  //
  // Old shapes processing
  //
  if (iNum>0 && iNum<=aNbLines) {
    di << "from the ";
    // Old Shape
    aNbObj=aDS.NumberOfShapesOfTheObject();
    if (iNum<=aNbObj) {
      di << "Object ";
    }
    else {
      di << "Tool ";
    }
    di << ".\n";
    return 0;
  }
  //
  bIsNewShape=aDS.IsNewShape(iNum);
  if (bIsNewShape) {
    di << "new.\n";
  }

  else {
    di << "undefined.\n";
    return 0;
  }
  //
  // New shapes processing
  //
  const TopoDS_Shape& aShapeToFind=aDS.Shape(iNum);
  TopAbs_ShapeEnum aTypeOfShapeToFind=aShapeToFind.ShapeType();
  //
  // Interferences
  const BOPTools_InterferencePool& anInterferencePool=pDF->InterfPool();
  //
  const BOPTools_CArray1OfVVInterference&  aVVInterfs=anInterferencePool.VVInterfs();
  const BOPTools_CArray1OfVEInterference&  aVEInterfs=anInterferencePool.VEInterfs();
  const BOPTools_CArray1OfVSInterference&  aVSInterfs=anInterferencePool.VSInterfs();
  const BOPTools_CArray1OfEEInterference&  aEEInterfs=anInterferencePool.EEInterfs();
  const BOPTools_CArray1OfESInterference&  aESInterfs=anInterferencePool.ESInterfs();
  //
  // VV
  aNbLines=aVVInterfs.Extent();
  for (i=1; i<=aNbLines; ++i) {
    aNewShape=aVVInterfs(i).NewShape();
    if (aNewShape==iNum) {
      aVVInterfs(i).Indices(aWhat, aWith);
      //printf(" VV: (%d, %d)\n", aWhat, aWith);
      di << " VV: (" << aWhat << ", " << aWith << ")\n";
    }
  }
  //
  // VE
  aNbLines=aVEInterfs.Extent();
  for (i=1; i<=aNbLines; ++i) {
    aNewShape=aVEInterfs(i).NewShape();
    if (aNewShape==iNum) {
      aVEInterfs(i).Indices(aWhat, aWith);
      //printf(" VE: (%d, %d)\n", aWhat, aWith);
      di << " VE: (" << aWhat << ", " << aWith << ")\n";
    }
  }
  //
  // VF
  aNbLines=aVSInterfs.Extent();
  for (i=1; i<=aNbLines; ++i) {
    aNewShape=aVSInterfs(i).NewShape();
    if (aNewShape==iNum) {
      aVSInterfs(i).Indices(aWhat, aWith);
      //printf(" VF: (%d, %d)\n", aWhat, aWith);
      di << " VF: (" << aWhat << ", " << aWith << ")\n";
    }
  }
  //
  // EE
  aNbLines=aEEInterfs.Extent();
  for (i=1; i<=aNbLines; ++i) {
    aNewShape=aEEInterfs(i).NewShape();
    if (aNewShape==iNum) {
      aEEInterfs(i).Indices(aWhat, aWith);
      //printf(" EE: (%d, %d)\n", aWhat, aWith);
      di << " EE: (" << aWhat << ", " << aWith << ")\n";
    }
  }
  //
  // EF
  aNbLines=aESInterfs.Extent();
  for (i=1; i<=aNbLines; ++i) {
    aNewShape=aESInterfs(i).NewShape();
    if (aNewShape==iNum) {
      aESInterfs(i).Indices(aWhat, aWith);
      //printf(" EF: (%d, %d)\n", aWhat, aWith);
      di << " EF: (" << aWhat << ", " << aWith << ")\n";
    }
  }
  //
  // FF
  Standard_Integer j, aNbCurves;
  //
  BOPTools_InterferencePool* pInterPool=(BOPTools_InterferencePool*)&pDF->InterfPool();
  BOPTools_CArray1OfSSInterference&  aSSInterfs=pInterPool->SSInterferences();
  //
  aNbLines=aSSInterfs.Extent();
  for (i=1; i<=aNbLines; ++i) {
    BOPTools_SSInterference& aFF=aSSInterfs(i);
    aWhat=aFF.Index1();
    aWith=aFF.Index2();
    
    
    BOPTools_SequenceOfCurves& aSC=aFF.Curves();
    
    aNbCurves=aSC.Length();
    for (j=1; j<=aNbCurves; ++j) {
      const BOPTools_Curve& aBC=aSC(j);

      if (aTypeOfShapeToFind==TopAbs_EDGE) {
	const BOPTools_ListOfPaveBlock& aLPB=aBC.NewPaveBlocks();
	BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);
	for (; anIt.More(); anIt.Next()) {
	  const BOPTools_PaveBlock& aPB=anIt.Value();
	  aNewShape=aPB.Edge();
	  if (aNewShape==iNum) {
	    //printf(" FF: (%d, %d) [Section Edge]\n", aWhat, aWith);
	    di << " FF: (" << aWhat << ", " << aWith << ") [Section Edge]\n";
	  }
	}
      }
      //
      if (aTypeOfShapeToFind==TopAbs_VERTEX) {
	BOPTools_Curve* pBC=(BOPTools_Curve*)&aBC;
	TColStd_ListOfInteger& aTVList=pBC->TechnoVertices();
	TColStd_ListIteratorOfListOfInteger aTVIt(aTVList);
	for (; aTVIt.More(); aTVIt.Next()) {
	  aNewShape=aTVIt.Value();
	  if (aNewShape==iNum) {
	    //printf(" FF: (%d, %d) [Techno Vertex]\n", aWhat, aWith);
	    di << " FF: (" << aWhat << ", " << aWith << ") [Techno Vertex]\n";
	  }
	}
      }
    }
    //
    if (aTypeOfShapeToFind==TopAbs_VERTEX) {
      TColStd_ListOfInteger& anAVList=aFF.AloneVertices();
      TColStd_ListIteratorOfListOfInteger anAVIt(anAVList);
      for (; anAVIt.More(); anAVIt.Next()) {
	aNewShape=anAVIt.Value();
	if (aNewShape==iNum) {
	  //printf(" FF: (%d, %d) [Alone Vertex]\n", aWhat, aWith);
	  di << " FF: (" << aWhat << ", " << aWith << ") [Alone Vertex]\n";
	}
      }
    }
  }
  //
  // 
  // PaveBlocks 
  if (aTypeOfShapeToFind==TopAbs_EDGE) {
    //
    Standard_Integer aNbPaveBlocks, nE, nOriginalEdge, aNbCommonBlocks;
    Standard_Integer nFace, nE2,  nOriginalEdge2;
    //
    const BOPTools_CommonBlockPool& aCommonBlockPool=pDF->CommonBlockPool();
    const BOPTools_SplitShapesPool& aSplitShapesPool=pDF->SplitShapesPool();
    aNbLines=aSplitShapesPool.Extent();
    for (i=1; i<=aNbLines; ++i) {
      const BOPTools_ListOfPaveBlock& aSplitEdges=aSplitShapesPool(i);
      aNbPaveBlocks=aSplitEdges.Extent();
      
      BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplitEdges);
      for (; aPBIt.More(); aPBIt.Next()) {
	const BOPTools_PaveBlock& aPB=aPBIt.Value();
	nE=aPB.Edge();
	nOriginalEdge=aPB.OriginalEdge();
	if (nE==iNum) {
	  //printf("  PaveBlock [Base Edge #%d]\n", nOriginalEdge);
	  di << "  PaveBlock [Base Edge #" << nOriginalEdge << "]\n";
	  const BOPTools_ListOfCommonBlock& aLCB=
	    aCommonBlockPool(aDS.RefEdge(nOriginalEdge));
	  aNbCommonBlocks=aLCB.Extent();
	  
	  BOPTools_ListIteratorOfListOfCommonBlock aCBIt(aLCB);
	  for (; aCBIt.More(); aCBIt.Next()) {
	    BOPTools_CommonBlock& aCB=aCBIt.Value();
	    nFace=aCB.Face();
	    if (nFace) {
	      //printf("  CommonBlock with Face #%d\n", nFace);
	      di << "  CommonBlock with Face #" << nFace << "\n";
	    }
	    else {
	      BOPTools_PaveBlock& aPB2=aCB.PaveBlock2(nOriginalEdge);
	      nE2=aPB2.Edge();
	      nOriginalEdge2=aPB2.OriginalEdge();
	      //printf("  CommonBlock with Edge #%d [Base Edge #%d]\n", 
		//     nE2, nOriginalEdge2);
	      di << "  CommonBlock with Edge #" << nE2 << " [Base Edge #" << nOriginalEdge2 << "]\n"; 
	    }
	  }
	  
	}
      }
    }
    
  }
  return 0; 
}

//=======================================================================
//function : bopsticks
//purpose  : 
//=======================================================================
Standard_Integer bopsticks (Draw_Interpretor& di, 
			    Standard_Integer n, 
			    const char** )
{
  if (pDF==NULL) {
    di << " Prepare BOPTools_DSFiller first >bop S1 S2\n";
    return 1;
  }
  //
  if (n!=1) {
    di << " Use >bopsticks\n";
    return 1;
  }
  //
  Standard_Integer i, j, aNbLines, nF1, nF2, aNbVtx, nV;
  //
  const BooleanOperations_ShapesDataStructure& aDS=pDF->DS();
  const BOPTools_PaveFiller& aPF=pDF->PaveFiller();
  BOPTools_PaveFiller* pPF=(BOPTools_PaveFiller*)&aPF;
  BOPTools_InterferencePool* pInterPool=(BOPTools_InterferencePool*)&pDF->InterfPool();
  BOPTools_CArray1OfSSInterference&  aSSInterfs=pInterPool->SSInterferences();
  //
  aNbLines=aSSInterfs.Extent();
  //
  for (i=1; i<=aNbLines; ++i) {
    TColStd_IndexedMapOfInteger aMapUnUsed;
    BOPTools_PaveSet aPSF;
    //
    BOPTools_SSInterference& aFF=aSSInterfs(i);
    BOPTools_SequenceOfCurves& aSCvs=aFF.Curves();

    nF1=aFF.Index1();
    nF2=aFF.Index2();
    
    pPF->PrepareSetForFace(nF1, nF2, aPSF);
    UnUsedMap(aSCvs, aPSF, aMapUnUsed);
    aNbVtx=aMapUnUsed.Extent();
    for (j=1; j<=aNbVtx; ++j) {
      nV=aMapUnUsed(j);
      if (aDS.IsNewShape(nV)) {
	//printf(" Vertex #%d [FF:(%d, %d)]\n", nV, nF1, nF2);
	di << " Vertex #" << nV << " [FF:(" << nF1 << ", " << nF2 << ")]\n";
      }
    }
  }
  return 0;  
}

//=======================================================================
// function: UnUsedMap
// purpose: 
//=======================================================================
void UnUsedMap(BOPTools_SequenceOfCurves& aSCvs,
	       const BOPTools_PaveSet& aPSF,
	       TColStd_IndexedMapOfInteger& aMapUnUsed)
{
  //
  // What stick/non-stick vertices we used 
  TColStd_IndexedMapOfInteger aMapUsed, aMapMustBeUsed;
  Standard_Integer j, aNbCurves, aNbVtx, nV1;//, nV2;
  BOPTools_ListIteratorOfListOfPave anLPIt;

  aNbCurves=aSCvs.Length();
  for (j=1; j<=aNbCurves; ++j) {
    BOPTools_Curve& aBC=aSCvs(j);
    //const IntTools_Curve& aC= aBC.Curve();// Wng in Gcc 3.0
	
    const BOPTools_PaveSet& aPaveSet=aBC.Set();
    const BOPTools_ListOfPave& aLPAlreadyUsed=aPaveSet.Set();
    anLPIt.Initialize(aLPAlreadyUsed);
    for (; anLPIt.More(); anLPIt.Next()) {
      const BOPTools_Pave& aPave=anLPIt.Value();
      nV1=aPave.Index();
      aMapUsed.Add(nV1);
    }
  }
  // 
  // 2. Stick vertices that must be used
  const BOPTools_ListOfPave& aLPMustUsed=aPSF.Set();
  anLPIt.Initialize(aLPMustUsed);
  for (; anLPIt.More(); anLPIt.Next()) {
    const BOPTools_Pave& aPave=anLPIt.Value();
    nV1=aPave.Index();
    aMapMustBeUsed.Add(nV1);
  }
  //
  // 3.Unused Stick vertices .
  aNbVtx=aMapMustBeUsed.Extent();
  for (j=1; j<=aNbVtx; ++j) {
    nV1=aMapMustBeUsed(j);
    if (!aMapUsed.Contains(nV1)) {
      aMapUnUsed.Add(nV1);
    }
  }
  // 
}
