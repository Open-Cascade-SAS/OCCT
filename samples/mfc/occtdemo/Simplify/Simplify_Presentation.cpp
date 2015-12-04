// Simplify_Presentation.cpp: implementation of the Simplify_Presentation class.
// Simplify shape -> create a new shape based on triangulation of a given shape
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Simplify_Presentation.h"

#include <GeomPlate_Surface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <ShapeFix_Shape.hxx>


#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Simplify_Presentation;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Simplify_Presentation::Simplify_Presentation()
{
  setName ("Simplify shape");
}

//=========================================================================================
// Sample execution
//=========================================================================================

void Simplify_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  sample("shell1.brep");
}


//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////
//================================================================
// Function : Simplify_Presentation::simplify
// Purpose  : 
//================================================================
void Simplify_Presentation::simplify(const TopoDS_Shape& aShape)
{
  setResultTitle("Simplify Face");
  setResultText(
    "  TopoDS_Shape aShape;" EOL
    "" EOL
    "  // initialize aShape" EOL
    "  //aShape = ..." EOL
    "" EOL
    "  // define parameter triangulation" EOL
    "  Standard_Real aDeflection = 0.1;" EOL
    "  " EOL
    "  // removes all the triangulations of the faces ," EOL
    "  //and all the polygons on the triangulations of the edges" EOL
    "  BRepTools::Clean(aShape);" EOL
    "  // adds a triangulation of the shape aShape with the deflection aDeflection" EOL
    "  BRepMesh::Mesh(aShape,aDeflection);" EOL
    "" EOL
    "  Standard_Integer aIndex = 1, nbNodes = 0;" EOL
    "  " EOL
    "  // define two sequence of points" EOL
    "  TColgp_SequenceOfPnt aPoints, aPoints1;" EOL
    " " EOL
    "  // triangulation" EOL
    "  for(TopExp_Explorer aExpFace(aShape,TopAbs_FACE); aExpFace.More(); aExpFace.Next())" EOL
    "  {  " EOL
    "    TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());" EOL
    "    TopLoc_Location aLocation;" EOL
    "" EOL
    "    // takes the triangulation of the face aFace" EOL
    "    Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace,aLocation);" EOL
    "" EOL
    "    if(!aTr.IsNull())" EOL
    "    { " EOL
    "      // takes the array of nodes for this triangulation" EOL
    "      const TColgp_Array1OfPnt& aNodes = aTr->Nodes();    " EOL
    "      nbNodes = aNodes.Length();" EOL
    "" EOL
    "      for( Standard_Integer i = 1; i <= nbNodes; i++)" EOL
    "      {" EOL
    "        // create seguence of node points in absolute coordinate system" EOL
    "        gp_Pnt aPnt = aNodes(i).Transformed(aLocation);" EOL
    "        aPoints.Append(aPnt);" EOL
    "        " EOL
    "      }" EOL
    "    }" EOL
    "  }" EOL
    " " EOL
    "  // remove double points" EOL
    "  nbNodes = aPoints.Length();" EOL
    "  for( Standard_Integer i = 1; i <= nbNodes; i++)" EOL
    "  {" EOL
    "    gp_Pnt aPi = aPoints(i);" EOL
    "    for( Standard_Integer j = i + 1; j < nbNodes; j++)" EOL
    "    {" EOL
    "      gp_Pnt aPj = aPoints(j);" EOL
    "      if(!aPi.IsEqual(aPj,0.9))" EOL
    "        aIndex++;" EOL
    "    }" EOL
    "    if(aIndex == j - 1)" EOL
    "      aPoints1.Append(aPi);" EOL
    "" EOL
    "    aIndex = i + 1;" EOL
    "  }" EOL
    "" EOL
    "  // find max point" EOL
    "  aIndex = 0;" EOL
    "  gp_Pnt aPntMax = aPoints1(1);" EOL
    "  nbNodes = aPoints1.Length();" EOL
    "  for(i = 2; i <= nbNodes; i++)" EOL
    "  {" EOL
    "    if(aPoints1(i).X() > aPntMax.X())" EOL
    "    {" EOL
    "      aIndex = i;" EOL
    "      aPntMax = aPoints1(aIndex);      " EOL
    "    } " EOL
    "  }" EOL
    "" EOL
    "  // clear seguence" EOL
    "  aPoints.Clear();" EOL
    "" EOL
    "  Standard_Integer nbLeftNodes = nbNodes;" EOL
    "" EOL
    "  // ascending sort - fill aPoints with ascending " EOL
    "  // by X coordinate points from aPoints1" EOL
    "  for(i = 1; i < nbNodes; i++)" EOL
    "  {" EOL
    "    Standard_Real aMin = aPntMax.X();" EOL
    "    aIndex = 1;" EOL
    "    for( Standard_Integer j = 1; j <= nbLeftNodes; j++)" EOL
    "    {" EOL
    "      if(aPoints1(j).X() < aMin)" EOL
    "      {" EOL
    "        aMin = aPoints1(j).X();" EOL
    "        aIndex = j;" EOL
    "      } " EOL
    "    }" EOL
    "    aPoints.Append(aPoints1(aIndex));" EOL
    "    aPoints1.Remove(aIndex);" EOL
    "    nbLeftNodes = aPoints1.Length();" EOL
    "  }" EOL
    "  aPoints.Append(aPntMax);" EOL
    "" EOL
    "  // define parameters GeomPlate_BuildPlateSurface" EOL
    "  Standard_Integer Degree = 3;" EOL
    "  Standard_Integer NbPtsOnCur = 10;" EOL
    "  Standard_Integer NbIter = 3;" EOL
    "  Standard_Integer Order = 0;" EOL
    "  Standard_Integer MaxSeg = 9;" EOL
    "  Standard_Integer MaxDegree = 5;" EOL
    "  Standard_Real dmax, anApproxTol = 0.001;" EOL
    "  Standard_Real aConstrTol = Precision::Confusion();" EOL
    "  " EOL
    "  // define object BuildPlateSurface" EOL
    "  GeomPlate_BuildPlateSurface BPSurf(Degree,NbPtsOnCur,NbIter);" EOL
    "  " EOL
    "  // add point constraints to GeomPlate_BuildPlateSurface object" EOL
    "  nbNodes = aPoints.Length();" EOL
    "  for (i = 1; i <= nbNodes; i++)" EOL
    "    BPSurf.Add(new GeomPlate_PointConstraint(aPoints(i), Order, aConstrTol));" EOL
    "" EOL
    "  BPSurf.Perform();" EOL
    "" EOL
    "  // make PlateSurface" EOL
    "  Handle(GeomPlate_Surface) PSurf;" EOL
    "  Handle(Geom_Surface) aSurf;" EOL
    "  " EOL
    "  if (BPSurf.IsDone())" EOL
    "  {" EOL
    "    PSurf = BPSurf.Surface();" EOL
    "" EOL
    "    // define parameter approximation" EOL
    "    dmax = Max(0.01,10*BPSurf.G0Error());" EOL
    "" EOL
    "    // make approximation" EOL
    "    GeomPlate_MakeApprox Mapp(PSurf,anApproxTol, MaxSeg,MaxDegree,dmax);" EOL
    "    aSurf = Mapp.Surface();" EOL
    "  }" EOL
    "  else " EOL
    "    return;" EOL
    "" EOL
    "  ShapeAnalysis_FreeBounds aFreeBounds(aShape, Standard_False, Standard_True);" EOL
    "  TopoDS_Compound aClosedWires = aFreeBounds.GetClosedWires();" EOL
    "  TopTools_IndexedMapOfShape aWires;" EOL
    "  TopExp::MapShapes(aClosedWires, TopAbs_WIRE, aWires);" EOL
    "  TopoDS_Wire aWire;" EOL
    "  Standard_Integer nbWires = aWires.Extent();" EOL
    "  if (nbWires) " EOL
    "    aWire = TopoDS::Wire(aWires(1));" EOL
    "  else " EOL
    "    return;" EOL
    "" EOL
    "  BRep_Builder B;" EOL
    "  TopoDS_Face aFace;" EOL
    "  B.MakeFace(aFace, aSurf, Precision::Confusion());" EOL
    "  B.Add(aFace, aWire);" EOL
    "  Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape(aFace);" EOL
    "  sfs->Perform();" EOL
    "  TopoDS_Shape aFixedFace = sfs->Shape();" EOL
    "  if (aFixedFace.IsNull()) " EOL
    "    return;" EOL);

  // define parameter triangulation
  Standard_Real aDeflection = 0.1;
  
  // removes all the triangulations of the faces ,
  //and all the polygons on the triangulations of the edges
  BRepTools::Clean(aShape);
  // adds a triangulation of the shape aShape with the deflection aDeflection
  BRepMesh::Mesh(aShape,aDeflection);

  Standard_Integer aIndex = 1, nbNodes = 0;
  
  // define two sequence of points
  TColgp_SequenceOfPnt aPoints, aPoints1;
 
  // triangulation
  for(TopExp_Explorer aExpFace(aShape,TopAbs_FACE); aExpFace.More(); aExpFace.Next())
  {  
    TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
    TopLoc_Location aLocation;

    // takes the triangulation of the face aFace
    Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace,aLocation);

    if(!aTr.IsNull())
    { 
      // takes the array of nodes for this triangulation
      const TColgp_Array1OfPnt& aNodes = aTr->Nodes();    
      nbNodes = aNodes.Length();

      for( Standard_Integer i = 1; i <= nbNodes; i++)
      {
        // create seguence of node points in absolute coordinate system
        gp_Pnt aPnt = aNodes(i).Transformed(aLocation);
        aPoints.Append(aPnt);
        
      }
    }
  }
 
  // remove double points
  nbNodes = aPoints.Length();
  for( Standard_Integer i = 1; i <= nbNodes; i++)
  {
    gp_Pnt aPi = aPoints(i);
    for( Standard_Integer j = i + 1; j < nbNodes; j++)
    {
      gp_Pnt aPj = aPoints(j);
      if(!aPi.IsEqual(aPj,0.9))
        aIndex++;
    }
    if(aIndex == j - 1)
      aPoints1.Append(aPi);

    aIndex = i + 1;
  }

  // find max point
  aIndex = 0;
  gp_Pnt aPntMax = aPoints1(1);
  nbNodes = aPoints1.Length();
  for(i = 2; i <= nbNodes; i++)
  {
    if(aPoints1(i).X() > aPntMax.X())
    {
      aIndex = i;
      aPntMax = aPoints1(aIndex);      
    } 
  }

  // clear seguence
  aPoints.Clear();

  Standard_Integer nbLeftNodes = nbNodes;

  // ascending sort - fill aPoints with ascending 
  // by X coordinate points from aPoints1
  for(i = 1; i < nbNodes; i++)
  {
    Standard_Real aMin = aPntMax.X();
    aIndex = 1;
    for( Standard_Integer j = 1; j <= nbLeftNodes; j++)
    {
      if(aPoints1(j).X() < aMin)
      {
        aMin = aPoints1(j).X();
        aIndex = j;
      } 
    }
    aPoints.Append(aPoints1(aIndex));
    aPoints1.Remove(aIndex);
    nbLeftNodes = aPoints1.Length();
  }
  aPoints.Append(aPntMax);

  // define parameters GeomPlate_BuildPlateSurface
  Standard_Integer Degree = 3;
  Standard_Integer NbPtsOnCur = 10;
  Standard_Integer NbIter = 3;
  Standard_Integer Order = 0;
  Standard_Integer MaxSeg = 9;
  Standard_Integer MaxDegree = 5;
  Standard_Real dmax, anApproxTol = 0.001;
  Standard_Real aConstrTol = Precision::Confusion();
  
  // define object BuildPlateSurface
  GeomPlate_BuildPlateSurface BPSurf(Degree,NbPtsOnCur,NbIter);
  
  // add point constraints to GeomPlate_BuildPlateSurface object
  nbNodes = aPoints.Length();
  for (i = 1; i <= nbNodes; i++)
    BPSurf.Add(new GeomPlate_PointConstraint(aPoints(i), Order, aConstrTol));

  BPSurf.Perform();

  // make PlateSurface
  Handle(GeomPlate_Surface) PSurf;
  Handle(Geom_Surface) aSurf;
  
  if (BPSurf.IsDone())
  {
    PSurf = BPSurf.Surface();

    // define parameter approximation
    dmax = Max(0.01,10*BPSurf.G0Error());

    // make approximation
    GeomPlate_MakeApprox Mapp(PSurf,anApproxTol, MaxSeg,MaxDegree,dmax);
    aSurf = Mapp.Surface();
  }
  else 
    return;

  ShapeAnalysis_FreeBounds aFreeBounds(aShape, Standard_False, Standard_True);
  TopoDS_Compound aClosedWires = aFreeBounds.GetClosedWires();
  TopTools_IndexedMapOfShape aWires;
  TopExp::MapShapes(aClosedWires, TopAbs_WIRE, aWires);
  TopoDS_Wire aWire;
  Standard_Integer nbWires = aWires.Extent();
  if (nbWires) 
    aWire = TopoDS::Wire(aWires(1));
  else 
    return;

  BRep_Builder B;
  TopoDS_Face aFace;
  B.MakeFace(aFace, aSurf, Precision::Confusion());
  B.Add(aFace, aWire);
  Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape(aFace);
  sfs->Perform();
  TopoDS_Shape aFixedFace = sfs->Shape();
  if (aFixedFace.IsNull()) 
    return;

  // output surface, make it half transparent
  Handle(AIS_InteractiveObject) aSurfIO = drawSurface(
    aSurf, Quantity_NOC_LEMONCHIFFON3, Standard_False);
  aSurfIO->SetTransparency(0.5);
  getAISContext()->Display(aSurfIO, Standard_False);
  COCCDemoDoc::Fit();

  if(WAIT_A_LITTLE) return;

  // output points
  for(i = 1; i <= nbNodes; i++)
    drawPoint(aPoints(i));

  if(WAIT_A_LITTLE) return;
  
  // output resulting face
  drawShape(aFixedFace);
}



//================================================================
// Function : Simplify_Presentation::sample
// Purpose  : 
//================================================================
void Simplify_Presentation::sample(const Standard_CString aFileName)
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

  simplify(aShape);
}
