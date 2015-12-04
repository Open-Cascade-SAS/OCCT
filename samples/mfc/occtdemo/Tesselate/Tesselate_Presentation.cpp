// Tesselate_Presentation.cpp: implementation of the Tesselate_Presentation class.
// Tesselate shapes.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tesselate_Presentation.h" 

#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <Poly_Triangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <gp_Pnt.hxx>


#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Tesselate_Presentation;

// Initialization of array of samples
Standard_CString Tesselate_Presentation::myFileNames[] =
{
  "wedge_ok.brep",
  "shell1.brep",
  "Pump_Nut.brep",
  "Pump_TopCover.brep",
  0
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Tesselate_Presentation::Tesselate_Presentation()
{
  for (myNbSamples = 0; myFileNames[myNbSamples]; myNbSamples++);
  setName ("Tesselate shapes");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Tesselate_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    sample (myFileNames[myIndex]);
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////
//================================================================

inline Standard_Integer _key(Standard_Integer n1,Standard_Integer n2)
{

  Standard_Integer key = 
    (n2>n1)?(n1<<16)+n2:(n2<<16)+n1;
  return key;
}

//DATA : [myIndex][{Deflection,NumberOfFace,NumberOfEdge}]
static const Standard_Real DATA [][3] = 
{
  {0.2,1,2},{0.5,6,2},{0.7,16,2},{1,1,2}
};


void Tesselate_Presentation::tesselateShape(const TopoDS_Shape& aShape)
{
  setResultTitle("Tesselate shape");
  TCollection_AsciiString aText = (
    "/////////////////////////////////////////////////////////////////" EOL
    "// Tesselate shape." EOL
    "/////////////////////////////////////////////////////////////////" EOL EOL
    ) ;

  Standard_Real aDeflection = DATA[myIndex][0];
  Standard_Integer aNumOfFace = (Standard_Integer)DATA[myIndex][1];
  Standard_Integer aNumOfEdge = (Standard_Integer)DATA[myIndex][2];

  aText +=
    "Standard_Real aDeflection;" EOL
    "// aDeflection = ... ;" EOL EOL

    "// removes all the triangulations of the faces ," EOL
    "//and all the polygons on the triangulations of the edges:" EOL
    "BRepTools::Clean(aShape);" EOL EOL

    "// adds a triangulation of the shape aShape with the deflection aDeflection:" EOL
    "BRepMesh::Mesh(aShape,aDeflection);" EOL EOL

    "TopExp_Explorer aExpFace,aExpEdge;" EOL
    "for(aExpFace.Init(aShape,TopAbs_FACE);aExpFace.More();aExpFace.Next())" EOL
    "{  " EOL
    "  TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());" EOL
    "  TopLoc_Location aLocation;" EOL EOL
    
    "  // takes the triangulation of the face aFace:" EOL
    "  Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace,aLocation);" EOL EOL
    
    "  if(!aTr.IsNull()) // if this triangulation is not NULL" EOL
    "  { " EOL
    "    // takes the array of nodes for this triangulation:" EOL
    "    const TColgp_Array1OfPnt& aNodes = aTr->Nodes();" EOL
    "    // takes the array of triangles for this triangulation:" EOL
    "    const Poly_Array1OfTriangle& triangles = aTr->Triangles();" EOL EOL
    
    "    // create array of node points in absolute coordinate system" EOL
    "    TColgp_Array1OfPnt aPoints(1, aNodes.Length());" EOL
    "    for( Standard_Integer i = 1; i < aNodes.Length()+1; i++)" EOL
    "      aPoints(i) = aNodes(i).Transformed(aLocation);" EOL EOL

    "    // Takes the node points of each triangle of this triangulation." EOL
    "    // takes a number of triangles:" EOL
    "    Standard_Integer nnn = aTr->NbTriangles();" EOL
    "    Standard_Integer nt,n1,n2,n3;" EOL
    "    for( nt = 1 ; nt < nnn+1 ; nt++)" EOL
    "    {" EOL
    "      // takes the node indices of each triangle in n1,n2,n3:" EOL
    "      triangles(nt).Get(n1,n2,n3);" EOL
    "      // takes the node points:" EOL
    "      gp_Pnt aPnt1 = aPoints(n1);" EOL
    "      gp_Pnt aPnt2 = aPoints(n2);" EOL
    "      gp_Pnt aPnt3 = aPoints(n3);" EOL
    "    } " EOL EOL
    
    "    // Takes the polygon associated to an edge." EOL
    "    aExpEdge.Init(aFace,TopAbs_EDGE);" EOL
    "    TopoDS_Edge aEdge;" EOL
    "    // for example,working with the first edge:" EOL
    "    if(aExpEdge.More())" EOL
    "      aEdge = TopoDS::Edge(aExpEdge.Current());" EOL EOL
    
    "    if(!aEdge.IsNull()) // if this edge is not NULL" EOL
    "    {" EOL
    "      // takes the polygon associated to the edge aEdge:" EOL
    "      Handle(Poly_PolygonOnTriangulation) aPol = " EOL
    "        BRep_Tool::PolygonOnTriangulation(aEdge,aTr,aEdge.Location());" EOL EOL
    
    "      if(!aPol.IsNull()) // if this polygon is not NULL" EOL
    "        // takes the array of nodes for this polygon" EOL
    "        // (indexes in the array of nodes for triangulation of theFace):" EOL
    "        const TColStd_Array1OfInteger& aNodesOfPol = aPol->Nodes();" EOL
    "    }" EOL
    "  }" EOL
    "}" EOL EOL
    
    "//==================================================" EOL EOL
    
      ;
   aText += "  Result with deflection = ";
   aText += TCollection_AsciiString(aDeflection);
   aText += " :" EOL;

   setResultText(aText.ToCString());  

//==========================================================================

  BRepTools::Clean(aShape);
  BRepMesh::Mesh(aShape,aDeflection);

  BRep_Builder aBuilder,aBuild1,aBuild2;
  TopoDS_Compound aCompound,aComp1,aComp2;
  aBuilder.MakeCompound(aCompound);
  aBuild1.MakeCompound(aComp1);
  aBuild2.MakeCompound(aComp2);

  TopTools_SequenceOfShape aVertices;
  Standard_Integer aCount = 0;
  Standard_Integer aNumOfNodes = 0;
  Standard_Integer aNumOfTriangles = 0;
  
  Handle(AIS_InteractiveObject) aShowEdge,aShowFace,aShowShape;
  
  TopExp_Explorer aExpFace,aExpEdge;

  for(aExpFace.Init(aShape,TopAbs_FACE);aExpFace.More();aExpFace.Next())
  {  
    aCount++;
  
    TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
    TopLoc_Location aLocation;

    Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace,aLocation);

    if(!aTr.IsNull())
    { 
      const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
      aNumOfNodes += aTr->NbNodes();
      Standard_Integer aLower = aNodes.Lower();
      Standard_Integer anUpper = aNodes.Upper();
      const Poly_Array1OfTriangle& triangles = aTr->Triangles();
      aNumOfTriangles += aTr->NbTriangles();

      if(aCount == aNumOfFace)
      {
        Standard_Integer aNbOfNodesOfFace = aTr->NbNodes();
        Standard_Integer aNbOfTrianglesOfFace = aTr->NbTriangles();
        aExpEdge.Init(aFace,TopAbs_EDGE);

        TopoDS_Edge aEdge;

        for( Standard_Integer i = 0; aExpEdge.More() && i < aNumOfEdge ; aExpEdge.Next(), i++)
          aEdge = TopoDS::Edge(aExpEdge.Current());

        if(!aEdge.IsNull())
        {
          Handle(Poly_PolygonOnTriangulation) aPol = 
            BRep_Tool::PolygonOnTriangulation(aEdge,aTr,aEdge.Location());

          if(!aPol.IsNull())
          {
            const TColStd_Array1OfInteger& aNodesOfPol = aPol->Nodes();
            Standard_Integer aNbOfNodesOfEdge = aPol->NbNodes();

            aText += "Number of nodes of the edge = ";
            aText += TCollection_AsciiString(aNbOfNodesOfEdge) + EOL;
            aText += "Number of nodes of the face = ";
            aText += TCollection_AsciiString(aNbOfNodesOfFace) + EOL;
            aText += "Number of triangles of the face = ";
            aText += TCollection_AsciiString(aNbOfTrianglesOfFace) + EOL;
            setResultText(aText.ToCString());  

            Standard_Integer aLower = aNodesOfPol.Lower(), anUpper = aNodesOfPol.Upper();
            for( i = aLower; i < anUpper ; i++)
            {
              gp_Pnt aPnt1 = aNodes(aNodesOfPol(i)).Transformed(aLocation);
              gp_Pnt aPnt2 = aNodes(aNodesOfPol(i+1)).Transformed(aLocation);
              TopoDS_Vertex aVertex1 = BRepBuilderAPI_MakeVertex (aPnt1);
              TopoDS_Vertex aVertex2 = BRepBuilderAPI_MakeVertex (aPnt2);

              if(!aVertex1.IsNull() && !aVertex2.IsNull() && // if vertices are "alive"
                !BRep_Tool::Pnt(aVertex1).IsEqual(
                BRep_Tool::Pnt(aVertex2),Precision::Confusion())) // if they are different
              {
                aEdge = BRepBuilderAPI_MakeEdge (aVertex1,aVertex2);
                aBuild2.Add(aComp2,aVertex1);
                if(!aEdge.IsNull())
                  aBuild2.Add(aComp2,aEdge);
                if(i == anUpper-1)
                  aBuild2.Add(aComp2,aVertex2);
              }
            }
      
            getAISContext()->EraseAll();
            aShowShape = drawShape(aShape);
            if(WAIT_A_SECOND) return;
            aShowEdge = drawShape(aComp2,Quantity_NOC_GREEN);
            getAISContext()->Erase(aShowShape);
            if(WAIT_A_SECOND) return;
          }
        }
      }
    

      TopTools_DataMapOfIntegerShape aEdges;
      TopTools_SequenceOfShape aVertices;

      for( Standard_Integer i = 1; i < aNodes.Length()+1; i++)
      {
        gp_Pnt aPnt = aNodes(i).Transformed(aLocation);
        TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(aPnt);

        if(!aVertex.IsNull())
        {
          aBuilder.Add(aCompound,aVertex);
          if(aCount == aNumOfFace ) 
            aBuild1.Add(aComp1,aVertex);
          aVertices.Append(aVertex);
        }
      }

      Standard_Integer nnn = aTr->NbTriangles();
      Standard_Integer nt,n1,n2,n3;

      for( nt = 1 ; nt < nnn+1 ; nt++)
      {     
        triangles(nt).Get(n1,n2,n3);

        Standard_Integer key[3];
        
        TopoDS_Vertex aV1,aV2;
        key[0] = _key(n1, n2);
        if(!aEdges.IsBound(key[0]))
        {
          aV1 = TopoDS::Vertex(aVertices(n1));
          aV2 = TopoDS::Vertex(aVertices(n2));
          if(!aV1.IsNull() && !aV2.IsNull() &&
            !BRep_Tool::Pnt(aV1).IsEqual(BRep_Tool::Pnt(aV2),Precision::Confusion()))
          {
            TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge (aV1,aV2);  
            if(!aEdge.IsNull())
            {
              aEdges.Bind(key[0], aEdge);
              aBuilder.Add(aCompound,aEdges(key[0]));
              if(aCount == aNumOfFace)
                aBuild1.Add(aComp1,aEdges(key[0]));
            } 
          }
        }
        
        key[1] = _key(n2,n3);
        if(!aEdges.IsBound(key[1])) 
        { 
          aV1 = TopoDS::Vertex(aVertices(n2));
          aV2 = TopoDS::Vertex(aVertices(n3));
          if(!aV1.IsNull() && !aV2.IsNull() &&
            !BRep_Tool::Pnt(aV1).IsEqual(BRep_Tool::Pnt(aV2),Precision::Confusion()))
          {
            TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge (aV1,aV2);  
            if(!aEdge.IsNull())
            {
              aEdges.Bind(key[1],aEdge);
              aBuilder.Add(aCompound,aEdges(key[1]));
              if(aCount == aNumOfFace) 
                aBuild1.Add(aComp1,aEdges(key[1]));
            } 
          } 
        } 
 
        key[2] = _key(n3,n1);
        if(!aEdges.IsBound(key[2])) 
        { 
          aV1 = TopoDS::Vertex(aVertices(n3));
          aV2 = TopoDS::Vertex(aVertices(n1));
          if(!aV1.IsNull() && !aV2.IsNull() &&
            !BRep_Tool::Pnt(aV1).IsEqual(BRep_Tool::Pnt(aV2),Precision::Confusion()))
          { 
            TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge (aV1,aV2);  
            if(!aEdge.IsNull())
            { 
              aEdges.Bind(key[2],aEdge);
              aBuilder.Add(aCompound,aEdges(key[2]));
              if(aCount == aNumOfFace) 
                aBuild1.Add(aComp1,aEdges(key[2]));
            } 
          } 
        } 
      } 
      
      if(aCount == aNumOfFace)
      {
        aShowFace = drawShape(aComp1,Quantity_NOC_GREEN);
        getAISContext()->Erase(aShowEdge);
      }
    }
    else
    {
      aText += "Can't compute a triangulation on face ";
      aText += TCollection_AsciiString(aCount) + EOL;
      setResultText(aText.ToCString());
    }
  }
  
  aText += "Number of nodes of the shape = ";
  aText += TCollection_AsciiString(aNumOfNodes) + EOL;
  aText += "Number of triangles of the shape = ";
  aText += TCollection_AsciiString(aNumOfTriangles) + EOL EOL;
  setResultText(aText.ToCString());

  if(WAIT_A_SECOND) return;
  drawShape(aCompound,Quantity_NOC_GREEN);
  getAISContext()->Erase(aShowFace);
  
}


void Tesselate_Presentation::sample(const Standard_CString aFileName)
{
  TCollection_AsciiString aPath(GetDataDir());
  aPath = aPath + "\\" + aFileName;

  ResetView();
  
  if (aFileName == "wedge_ok.brep"){
	SetViewCenter(6.3639597574916, 4.4907309380832);
	SetViewScale(52.722555157077); 
  }

  if (aFileName == "shell1.brep"){
	SetViewCenter(60.457553053711, -20.351208944076);
	SetViewScale(26.857478563027); 
  }

  if (aFileName == "Pump_Nut.brep"){
	SetViewCenter(248.77723166710, 77.249633819945);
	SetViewScale(12.371719671833); 
  }

  if (aFileName == "Pump_TopCover.brep"){
	SetViewCenter(408.72474423160, 169.38361094986);
	SetViewScale(2.1932732873087); 
  }
 
  TopoDS_Shape aShape;
  BRep_Builder aBld;
  Standard_Boolean isRead = BRepTools::Read (aShape, aPath.ToCString(), aBld);
  if (!isRead)
  {
    aPath += " was not found.  The sample can not be shown.";
    setResultText(aPath.ToCString());
    return;
  }

  tesselateShape (aShape);
}
