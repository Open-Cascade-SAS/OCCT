// File:	BOPTest_WSplitCommands.cxx
// Created:	10:45:01 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTest.ixx>

#include <stdio.h>

#include <BOPTest_DrawableShape.hxx>

#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>

#include <Draw.hxx>
#include <Draw_Color.hxx>

#include <TCollection_AsciiString.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <BOP_WireSplitter.hxx>

#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>

#include <TopoDS_Shell.hxx>
#include <BOP_ShellSplitter.hxx>

#include <BOPTColStd_ListOfListOfShape.hxx>
#include <BOPTColStd_ListIteratorOfListOfListOfShape.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>




static   Standard_Integer bwsplit  (Draw_Interpretor&, Standard_Integer, const char** );
static   Standard_Integer bssplit  (Draw_Interpretor&, Standard_Integer, const char** );


//=======================================================================
//function : WSplitCommands
//purpose  : 
//=======================================================================
  void  BOPTest::WSplitCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) 
    return;

  done = Standard_True;
  // Chapter's name
  const char* g = "CCR commands";

  theCommands.Add("bwsplit"   , "Use bwsplit> Face"   , __FILE__, bwsplit  , g);
  theCommands.Add("bssplit"   , "Use bssplit> Shell"  , __FILE__, bssplit  , g);
}

//=======================================================================
//function : bssplit
//purpose  : 
//=======================================================================
Standard_Integer bssplit (Draw_Interpretor& di, 
			  Standard_Integer n, 
			  const char** a)
{
  if (n!=2) {
    di << " Use bssplit> Shell\n";
    return 1;
  }
  
  TopoDS_Shape S = DBRep::Get(a[1]);
  
  if (S.IsNull()) {
    di << " Null shape is not allowed \n";
    return 1;
  }

  Standard_Boolean anIsDone, anIsNothingToDo ;

  TopAbs_ShapeEnum aT;
  aT=S.ShapeType();
  if (aT!=TopAbs_SHELL) {
    di << " Shape Type must be SHELL\n";
    return 1;
  }

  const TopoDS_Shell& aSh = TopoDS::Shell(S);

  BOP_ShellSplitter aShellSplitter;
  aShellSplitter.SetShell(aSh);
  aShellSplitter.DoWithShell ();
  anIsDone=aShellSplitter.IsDone();
  di << " aShellSplitter.IsDone=" << (Standard_Integer) anIsDone << "\n";
  
  anIsNothingToDo=aShellSplitter.IsNothingToDo();
  di << " IsNothingToDo=" << (Standard_Integer) anIsNothingToDo << "\n";

  if (!anIsDone) { 
    return 0;
  }
  if (anIsNothingToDo) {
    return 0;
  }
  //
  Standard_Integer i=1;
  BRep_Builder BB;
  const BOPTColStd_ListOfListOfShape& aSSS=aShellSplitter.Shapes();
  BOPTColStd_ListIteratorOfListOfListOfShape aWireIt(aSSS);
  for (; aWireIt.More(); aWireIt.Next(), ++i) {
    TopoDS_Compound aCompound;
    BB.MakeCompound(aCompound);

    const TopTools_ListOfShape& aListEd=aWireIt.Value();

    TopTools_ListIteratorOfListOfShape anIt(aListEd);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aE=anIt.Value();
      BB.Add(aCompound, aE);
    }
    TCollection_AsciiString aName(a[1]), aDef("_"), anI(i), aRName;
    aRName=aName;
    aRName=aRName+aDef;
    aRName=aRName+anI;

    DBRep::Set (aRName.ToCString(), aCompound);
    di << aRName.ToCString() << "\n";
  }

  return 0;
}

//=======================================================================
//function : bwsplit
//purpose  : 
//=======================================================================
Standard_Integer bwsplit (Draw_Interpretor& di, 
			  Standard_Integer n, 
			  const char** a)
{
  if (n!=2) {
    di << " Use bwsplit> Face\n";
    return 1;
  }
  
  TopoDS_Shape S = DBRep::Get(a[1]);
  
  if (S.IsNull()) {
    di << " Null shape is not allowed \n";
    return 1;
  }

  Standard_Boolean anIsDone, anIsNothingToDo ;

  TopAbs_ShapeEnum aT;
  aT=S.ShapeType();
  if (aT!=TopAbs_FACE) {
    di << " Shape Type must be FACE\n";
    return 1;
  }

  const TopoDS_Face& aF = TopoDS::Face(S);

  BOP_WireSplitter aWireSplitter;
  aWireSplitter.SetFace(aF);
  aWireSplitter.DoWithFace ();
  anIsDone=aWireSplitter.IsDone();
  di << " aWireSplitter.IsDone=" << (Standard_Integer) anIsDone << "\n";
  
  anIsNothingToDo=aWireSplitter.IsNothingToDo();
  di << " IsNothingToDo=" << (Standard_Integer) anIsNothingToDo << "\n";

  if (!anIsDone) { 
    return 0;
  }
  if (anIsNothingToDo) {
    return 0;
  }
  //
  Standard_Integer i=1;
  BRep_Builder BB;
  const BOPTColStd_ListOfListOfShape& aSSS=aWireSplitter.Shapes();
  BOPTColStd_ListIteratorOfListOfListOfShape aWireIt(aSSS);
  for (; aWireIt.More(); aWireIt.Next(), ++i) {
    TopoDS_Compound aCompound;
    BB.MakeCompound(aCompound);

    const TopTools_ListOfShape& aListEd=aWireIt.Value();

    TopTools_ListIteratorOfListOfShape anIt(aListEd);
    for (; anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aE=anIt.Value();
      BB.Add(aCompound, aE);
    }
    TCollection_AsciiString aName(a[1]), aDef("_"), anI(i), aRName;
    aRName=aName;
    aRName=aRName+aDef;
    aRName=aRName+anI;

    DBRep::Set (aRName.ToCString(), aCompound);
    di << aRName.ToCString() << "\n";
  }
  return 0;
}


