// Validate_Presentation.cpp: implementation of the Validate_Presentation class.
// Checking validity of shapes
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Validate_Presentation.h"

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Validate_Presentation;

// Initialization of array of samples
Standard_CString Validate_Presentation::myFileNames[] =
{
  "wedge_ok.brep",
  "wedge_bad.brep",
  "shape2_bad.brep",
  0
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Validate_Presentation::Validate_Presentation()
{
  FitMode=true;
  for (myNbSamples = 0; myFileNames[myNbSamples]; myNbSamples++)
    ;
  setName ("Checking validity of shapes");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Validate_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    sample (myFileNames[myIndex]);
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

void Validate_Presentation::checkShape(const TopoDS_Shape& aShape)
{
  setResultTitle ("Validate shape");
  TCollection_AsciiString aText;

  aText = aText +
    "  // Input shape" EOL
    "  TopoDS_Shape aShape;" EOL
    "  // Initializing aShape" EOL
    "  // ...." EOL EOL

    "  // Checking" EOL
    "  BRepCheck_Analyzer aChecker(aShape);" EOL
    "  if (!aChecker.IsValid())" EOL
    "  {" EOL
    "    // Initialize sequence of subshapes" EOL
    "    TopTools_SequenceOfShape aShapes;" EOL
    "    TopTools_MapOfShape aMap;" EOL
    "    TopAbs_ShapeEnum aTypes[] = {TopAbs_VERTEX, TopAbs_EDGE, " EOL
    "      TopAbs_WIRE, TopAbs_FACE, TopAbs_SHELL};" EOL
    "    TopExp_Explorer ex;" EOL
    "    for (int i=0; i < 5; i++)" EOL
    "      for (ex.Init(aShape, aTypes[i]); ex.More(); ex.Next())" EOL
    "        if (!aMap.Contains(ex.Current()))" EOL
    "        {" EOL
    "          aShapes.Append(ex.Current());" EOL
    "          aMap.Add(ex.Current());" EOL
    "        }" EOL EOL

    "    for (i=1; i <= aShapes.Length(); i++)" EOL
    "    {" EOL
    "      const TopoDS_Shape& aSubShape = aShapes(i);" EOL
    "      Handle(BRepCheck_Result) aResult = aChecker.Result(aSubShape);" EOL
    "      if (!aResult.IsNull())" EOL
    "      {" EOL
    "        BRepCheck_ListOfStatus aLstStatus;" EOL
    "        // get statuses for a subshape" EOL
    "        aLstStatus = aResult->Status();" EOL
    "        // get statuses for a subshape in context" EOL
    "        aResult->InitContextIterator();" EOL
    "        while (aResult->MoreShapeInContext())" EOL
    "        {" EOL
    "          BRepCheck_ListOfStatus aLst1;" EOL
    "          aLst1 = aResult->StatusOnShape();" EOL
    "          aLstStatus.Append(aLst1);" EOL
    "          aResult->NextShapeInContext();" EOL
    "        }" EOL
    "        BRepCheck_ListIteratorOfListOfStatus itl(aLstStatus);" EOL
    "        for (; itl.More(); itl.Next())" EOL
    "        {" EOL
    "          BRepCheck_Status aStatus = itl.Value();" EOL
    "          if (aStatus == BRepCheck_NoError)" EOL
    "            continue;" EOL EOL

    "          Standard_CString aError;" EOL
    "          switch (aStatus)" EOL
    "          {" EOL
    "            // for vertices" EOL
    "          case BRepCheck_InvalidPointOnCurve: aError = \"InvalidPointOnCurve\"; break;" EOL
    "          case BRepCheck_InvalidPointOnCurveOnSurface: aError = \"InvalidPointOnCurveOnSurface\"; break;" EOL
    "          case BRepCheck_InvalidPointOnSurface: aError = \"InvalidPointOnSurface\"; break;" EOL
    "            // for edges" EOL
    "          case BRepCheck_No3DCurve: aError = \"No3DCurve\"; break;" EOL
    "          case BRepCheck_Multiple3DCurve: aError = \"Multiple3DCurve\"; break;" EOL
    "          case BRepCheck_Invalid3DCurve: aError = \"Invalid3DCurve\"; break;" EOL
    "          case BRepCheck_NoCurveOnSurface: aError = \"NoCurveOnSurface\"; break;" EOL
    "          case BRepCheck_InvalidCurveOnSurface: aError = \"InvalidCurveOnSurface\"; break;" EOL
    "          case BRepCheck_InvalidCurveOnClosedSurface: aError = \"InvalidCurveOnClosedSurface\"; break;" EOL
    "          case BRepCheck_InvalidSameRangeFlag: aError = \"InvalidSameRangeFlag\"; break;" EOL
    "          case BRepCheck_InvalidSameParameterFlag: aError = \"InvalidSameParameterFlag\"; break;" EOL
    "          case BRepCheck_InvalidDegeneratedFlag: aError = \"InvalidDegeneratedFlag\"; break;" EOL
    "          case BRepCheck_FreeEdge: aError = \"FreeEdge\"; break;" EOL
    "          case BRepCheck_InvalidMultiConnexity: aError = \"InvalidMultiConnexity\"; break;" EOL
    "          case BRepCheck_InvalidRange: aError = \"InvalidRange\"; break;" EOL
    "            // for wires" EOL
    "          case BRepCheck_EmptyWire: aError = \"EmptyWire\"; break;" EOL
    "          case BRepCheck_RedundantEdge: aError = \"RedundantEdge\"; break;" EOL
    "          case BRepCheck_SelfIntersectingWire: aError = \"SelfIntersectingWire\"; break;" EOL
    "            // for faces" EOL
    "          case BRepCheck_NoSurface: aError = \"NoSurface\"; break;" EOL
    "          case BRepCheck_InvalidWire: aError = \"InvalidWire\"; break;" EOL
    "          case BRepCheck_RedundantWire: aError = \"RedundantWire\"; break;" EOL
    "          case BRepCheck_IntersectingWires: aError = \"IntersectingWires\"; break;" EOL
    "          case BRepCheck_InvalidImbricationOfWires: aError = \"InvalidImbricationOfWires\"; break;" EOL
    "            // for shells" EOL
    "          case BRepCheck_EmptyShell: aError = \"EmptyShell\"; break;" EOL
    "          case BRepCheck_RedundantFace: aError = \"RedundantFace\"; break;" EOL
    "            // for shapes" EOL
    "          case BRepCheck_UnorientableShape: aError = \"UnorientableShape\"; break;" EOL
    "          case BRepCheck_NotClosed: aError = \"NotClosed\"; break;" EOL
    "          case BRepCheck_NotConnected: aError = \"NotConnected\"; break;" EOL
    "          case BRepCheck_SubshapeNotInShape: aError = \"SubshapeNotInShape\"; break;" EOL
    "          case BRepCheck_BadOrientation: aError = \"BadOrientation\"; break;" EOL
    "          case BRepCheck_BadOrientationOfSubshape: aError = \"BadOrientationOfSubshape\"; break;" EOL
    "          case BRepCheck_InvalidToleranceValue: aError = \"InvalidToleranceValue\"; break;" EOL
    "          default : aError = \"Undefined error\";" EOL
    "          }" EOL
    "        }" EOL
    "      }" EOL
    "    }" EOL
    "  }" EOL
    "  else" EOL
    "  {" EOL
    "    // aShape is a valid shape" EOL
    "  }" EOL EOL

    "//==========================================" EOL EOL;
  setResultText (aText.ToCString());
  getAISContext()->IsoOnPlane(Standard_False);
  drawShape (aShape);
  if (WAIT_A_SECOND) return;

  // Checking
  BRepCheck_Analyzer aChecker(aShape);
  if (!aChecker.IsValid())
  {
    // Initialize sequence of subshapes
    TopTools_SequenceOfShape aShapes;
    TopTools_MapOfShape aMap;
    TopAbs_ShapeEnum aTypes[] = {TopAbs_VERTEX, TopAbs_EDGE, 
      TopAbs_WIRE, TopAbs_FACE, TopAbs_SHELL};
    TopExp_Explorer ex;
    for (int i=0; i < 5; i++)
      for (ex.Init(aShape, aTypes[i]); ex.More(); ex.Next())
        if (!aMap.Contains(ex.Current()))
        {
          aShapes.Append(ex.Current());
          aMap.Add(ex.Current());
        }

    for (i=1; i <= aShapes.Length(); i++)
    {
      const TopoDS_Shape& aSubShape = aShapes(i);
      Handle(BRepCheck_Result) aResult = aChecker.Result(aSubShape);
      if (!aResult.IsNull())
      {
        BRepCheck_ListOfStatus aLstStatus;
        // get statuses for a subshape
        aLstStatus = aResult->Status();
        // get statuses for a subshape in context
        aResult->InitContextIterator();
        while (aResult->MoreShapeInContext())
        {
          BRepCheck_ListOfStatus aLst1;
          aLst1 = aResult->StatusOnShape();
          aLstStatus.Append(aLst1);
          aResult->NextShapeInContext();
        }
        BRepCheck_ListIteratorOfListOfStatus itl(aLstStatus);
        for (; itl.More(); itl.Next())
        {
          BRepCheck_Status aStatus = itl.Value();
          // use aStatus as you need
          // ...

          if (aStatus == BRepCheck_NoError)
            continue;

          TopAbs_ShapeEnum aType = aSubShape.ShapeType();
          Standard_CString aTypeName;
          switch (aType)
          {
          case TopAbs_SHELL: aTypeName = "SHELL"; break;
          case TopAbs_FACE: aTypeName = "FACE"; break;
          case TopAbs_WIRE: aTypeName = "WIRE"; break;
          case TopAbs_EDGE: aTypeName = "EDGE"; break;
          case TopAbs_VERTEX: aTypeName = "VERTEX"; break;
          }

          Standard_CString aError;
          switch (aStatus)
          {
            // for vertices
          case BRepCheck_InvalidPointOnCurve: aError = "InvalidPointOnCurve"; break;
          case BRepCheck_InvalidPointOnCurveOnSurface: aError = "InvalidPointOnCurveOnSurface"; break;
          case BRepCheck_InvalidPointOnSurface: aError = "InvalidPointOnSurface"; break;
            // for edges
          case BRepCheck_No3DCurve: aError = "No3DCurve"; break;
          case BRepCheck_Multiple3DCurve: aError = "Multiple3DCurve"; break;
          case BRepCheck_Invalid3DCurve: aError = "Invalid3DCurve"; break;
          case BRepCheck_NoCurveOnSurface: aError = "NoCurveOnSurface"; break;
          case BRepCheck_InvalidCurveOnSurface: aError = "InvalidCurveOnSurface"; break;
          case BRepCheck_InvalidCurveOnClosedSurface: aError = "InvalidCurveOnClosedSurface"; break;
          case BRepCheck_InvalidSameRangeFlag: aError = "InvalidSameRangeFlag"; break;
          case BRepCheck_InvalidSameParameterFlag: aError = "InvalidSameParameterFlag"; break;
          case BRepCheck_InvalidDegeneratedFlag: aError = "InvalidDegeneratedFlag"; break;
          case BRepCheck_FreeEdge: aError = "FreeEdge"; break;
          case BRepCheck_InvalidMultiConnexity: aError = "InvalidMultiConnexity"; break;
          case BRepCheck_InvalidRange: aError = "InvalidRange"; break;
            // for wires
          case BRepCheck_EmptyWire: aError = "EmptyWire"; break;
          case BRepCheck_RedundantEdge: aError = "RedundantEdge"; break;
          case BRepCheck_SelfIntersectingWire: aError = "SelfIntersectingWire"; break;
            // for faces
          case BRepCheck_NoSurface: aError = "NoSurface"; break;
          case BRepCheck_InvalidWire: aError = "InvalidWire"; break;
          case BRepCheck_RedundantWire: aError = "RedundantWire"; break;
          case BRepCheck_IntersectingWires: aError = "IntersectingWires"; break;
          case BRepCheck_InvalidImbricationOfWires: aError = "InvalidImbricationOfWires"; break;
            // for shells
          case BRepCheck_EmptyShell: aError = "EmptyShell"; break;
          case BRepCheck_RedundantFace: aError = "RedundantFace"; break;
            // for shapes
          case BRepCheck_UnorientableShape: aError = "UnorientableShape"; break;
          case BRepCheck_NotClosed: aError = "NotClosed"; break;
          case BRepCheck_NotConnected: aError = "NotConnected"; break;
          case BRepCheck_SubshapeNotInShape: aError = "SubshapeNotInShape"; break;
          case BRepCheck_BadOrientation: aError = "BadOrientation"; break;
          case BRepCheck_BadOrientationOfSubshape: aError = "BadOrientationOfSubshape"; break;
          case BRepCheck_InvalidToleranceValue: aError = "InvalidToleranceValue"; break;
          default : aError = "Undefined error";
          }

          aText = aText + aTypeName + " : " + aError + EOL;
          setResultText (aText.ToCString());
          getAISContext()->IsoOnPlane(Standard_True);
          Handle(AIS_InteractiveObject) obj = drawShape (aSubShape, Quantity_NOC_RED, Standard_False);
          getAISContext()->SetDisplayMode(obj,AIS_WireFrame);
          getAISContext()->Display(obj);
          if (WAIT_A_SECOND) return;
          getAISContext()->IsoOnPlane(Standard_False);
          getAISContext()->Erase (obj);
        }
      }
    }
  }
  else
  {
    aText = aText + "No errors in shape" + EOL;
  }

  setResultText (aText.ToCString());
}

void Validate_Presentation::sample(const Standard_CString aFileName)
{
  TCollection_AsciiString aPath(GetDataDir());
  aPath = aPath + "\\" + aFileName;

  TopoDS_Shape aShape;
  BRep_Builder aBld;
  Standard_Boolean isRead = BRepTools::Read (aShape, aPath.ToCString(), aBld);
  if (!isRead)
  {
    aPath += " was not found.  The sample can not be shown.";
    setResultText(aPath.ToCString());
    return;
  }

  checkShape (aShape);
}
