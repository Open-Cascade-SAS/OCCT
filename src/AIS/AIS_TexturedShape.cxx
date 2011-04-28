// File:	AIS_TexturedShape.cdl
// Created:	Mon Jul  2 11:32:59 2001
// Author:	Mathias BOSSHARD
//		<mbd@pomalox.paris1.matra-dtv.fr>
// Copyright:	 Matra Datavision 2001
//
// Modified:
//
//
//
////////////////////////////////////////////////////////////////////////


#include <AIS_TexturedShape.ixx>
#include <Standard_ErrorHandler.hxx>

#include <BRepTools.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Root.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Prs3d_Presentation.hxx>
#include <TopExp_Explorer.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <Graphic3d_Group.hxx>
#include <AIS_Drawer.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_ToolShadedShape.hxx>
#include <Precision.hxx>
#include <BRepMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Connect.hxx>
#include <Graphic3d_Array1OfVertexNT.hxx>
#include <Aspect_Array1OfEdge.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )

//////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////////////////

//=======================================================================
//function : AIS_TexturedShape
//purpose  : 
//=======================================================================

AIS_TexturedShape::AIS_TexturedShape(const TopoDS_Shape& ashape):AIS_Shape(ashape),
                                     myPredefTexture(Graphic3d_NameOfTexture2D(0)),
				     myTextureFile(""),
				     DoRepeat(Standard_True),
				     myURepeat(1.0),
				     myVRepeat(1.0),
				     DoMapTexture(Standard_True),
				     DoSetTextureOrigin(Standard_True),
				     myUOrigin(0.0),
				     myVOrigin(0.0),
				     DoSetTextureScale(Standard_True),
				     myScaleU(1.0),
				     myScaleV(1.0),
				     DoShowTriangles(Standard_False),
                                     myModulate(Standard_True)
{
}

//////////////////////////////////////////////////////////////////////
// TEXTURE MAPPING MANAGEMENT METHODS
//////////////////////////////////////////////////////////////////////

//=======================================================================
//function : SetTextureFileName
//purpose  : 
//=======================================================================

void AIS_TexturedShape::SetTextureFileName(const TCollection_AsciiString& TextureFileName)
{
  if (TextureFileName.IsIntegerValue())
    {
      if(TextureFileName.IntegerValue()<Graphic3d_Texture2D::NumberOfTextures() && TextureFileName.IntegerValue()>=0)
	myPredefTexture = (Graphic3d_NameOfTexture2D)(TextureFileName.IntegerValue());
      else
	{
	  cout << "Texture "<<TextureFileName<<" doesn't exist \n"<< endl;
	  cout << "Using Texture 0 instead ...\n"<< endl;
	  myPredefTexture = (Graphic3d_NameOfTexture2D)(0);
	}
      myTextureFile = "";
    }
  else
    {
      myTextureFile = TextureFileName;
      myPredefTexture = (Graphic3d_NameOfTexture2D)(-1);
    }
}

//=======================================================================
//function : SetTextureRepeat
//purpose  : 
//=======================================================================

void AIS_TexturedShape::SetTextureRepeat(const Standard_Boolean RepeatYN,
                                         const Standard_Real URepeat,
                                         const Standard_Real VRepeat)
{
  DoRepeat = RepeatYN;
  myURepeat = URepeat;
  myVRepeat = VRepeat;
}

//=======================================================================
//function : SetTextureMapOn
//purpose  : 
//=======================================================================

void AIS_TexturedShape::SetTextureMapOn()
{
  DoMapTexture = Standard_True;
}

//=======================================================================
//function : SetTextureMapOff
//purpose  : 
//=======================================================================

void AIS_TexturedShape::SetTextureMapOff()
{
  DoMapTexture = Standard_False;
}

//=======================================================================
//function : SetTextureOrigin
//purpose  : 
//=======================================================================

void AIS_TexturedShape::SetTextureOrigin(const Standard_Boolean SetTextureOriginYN, const Standard_Real UOrigin, const Standard_Real VOrigin)
{
  DoSetTextureOrigin = SetTextureOriginYN;
  myUOrigin = UOrigin;
  myVOrigin = VOrigin;
}

//=======================================================================
//function : SetTextureScale
//purpose  : 
//=======================================================================

void AIS_TexturedShape::SetTextureScale(const Standard_Boolean SetTextureScaleYN, const Standard_Real ScaleU, const Standard_Real ScaleV)
{
  DoSetTextureScale = SetTextureScaleYN;
  myScaleU = ScaleU;
  myScaleV = ScaleV;
}

//=======================================================================
//function : TriangleIsValid
//purpose  : 
//=======================================================================

Standard_Boolean AIS_TexturedShape::TriangleIsValid(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3) const
{ 
  gp_Vec V1(P1,P2);								// V1=(P1,P2)
  gp_Vec V2(P2,P3);								// V2=(P2,P3)
  gp_Vec V3(P3,P1);								// V3=(P3,P1)
  
  if ((V1.SquareMagnitude() > 1.e-10) && (V2.SquareMagnitude() > 1.e-10) && (V3.SquareMagnitude() > 1.e-10))
    {
      V1.Cross(V2);								// V1 = Normal	
      if (V1.SquareMagnitude() > 1.e-10)
	return Standard_True;
      else
	return Standard_False;
    }
  else
    return Standard_False;
  
}

//=======================================================================
//function : ShowTriangles
//purpose  : 
//=======================================================================

void AIS_TexturedShape::ShowTriangles(const Standard_Boolean ShowTrianglesYN)
{
  DoShowTriangles = ShowTrianglesYN;
}

//=======================================================================
//function : EnableTextureModulate
//purpose  : 
//=======================================================================

void AIS_TexturedShape::EnableTextureModulate()
{
  myModulate = Standard_True;
}

//=======================================================================
//function : DisableTextureModulate
//purpose  : 
//=======================================================================

void AIS_TexturedShape::DisableTextureModulate()
{
  myModulate = Standard_False;
}

//=======================================================================
//function : UpdateAttributes
//purpose  : 
//=======================================================================

void AIS_TexturedShape::UpdateAttributes()
{
  Handle(Graphic3d_StructureManager) aStrucMana = GetContext()->MainPrsMgr()->StructureManager();
  myAspect = (new Prs3d_ShadingAspect())->Aspect();
  Handle(Prs3d_Presentation) aPrs = Presentation();
  if (!DoMapTexture)
    {
      myAspect->SetTextureMapOff();
      return;
    }
  
  if(myPredefTexture!=-1)
    mytexture = new Graphic3d_Texture2Dmanual(aStrucMana, myPredefTexture);
  else
    mytexture = new Graphic3d_Texture2Dmanual(aStrucMana, myTextureFile.ToCString());
  
  myAspect->SetTextureMapOn();
  
  myAspect->SetTextureMap(mytexture);
  if (!mytexture->IsDone())
    {
      cout << "An error occured while building texture \n" <<endl;
      return;
    }
  
  if (DoShowTriangles)
    myAspect->SetEdgeOn();
  else
    myAspect->SetEdgeOff();
  
  Prs3d_Root::CurrentGroup(aPrs)->SetGroupPrimitivesAspect(myAspect);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_TexturedShape::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
				const Handle(Prs3d_Presentation)& aPrs,
				const Standard_Integer aMode)
{
  aPrs->Clear();
  
  if(myshape.IsNull()) return;
  
  Standard_Integer TheType;
  TheType = (Standard_Integer) myshape.ShapeType();
  if(TheType>4 && TheType<8)
    {
      aPrs->SetVisual(Graphic3d_TOS_ALL);
      aPrs->SetDisplayPriority(TheType+2);
    }

  if (myshape.ShapeType() == TopAbs_COMPOUND)
    {
      TopExp_Explorer anExplor (myshape, TopAbs_VERTEX);
      if (!anExplor.More()) {return;}
    }

  if (IsInfinite())
    aPrs->SetInfiniteState(Standard_True);

  switch (aMode)
    { 

    case 0: // Wireframe
      StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);
      break;

    case 1: // Shading)
      {
	Standard_Real prevangle ;
	Standard_Real newangle  ; 
	Standard_Real prevcoeff ;
	Standard_Real newcoeff  ; 
	
	if (OwnDeviationAngle(newangle,prevangle) || OwnDeviationCoefficient(newcoeff,prevcoeff))
	  if (Abs (newangle - prevangle) > Precision::Angular() || Abs (newcoeff - prevcoeff) > Precision::Confusion()  ) 
	    { 
	      BRepTools::Clean(myshape);
	    }
	if ((Standard_Integer) myshape.ShapeType()>4)
	  StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);
	else
	  {
	    myDrawer->SetShadingAspectGlobal(Standard_False);
	    if (IsInfinite())
	      StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);
	    else
	      {
		try
		  {
		  OCC_CATCH_SIGNALS
		    StdPrs_ShadedShape::Add(aPrs,myshape,myDrawer);
		  }
		catch (Standard_Failure)
		  {
		    cout <<"AIS_TexturedShape::Compute() in ShadingMode failed \n" <<endl;
		    StdPrs_WFShape::Add(aPrs,myshape,myDrawer);
		  }
	      }
	  }
	break;
      }

    case 2: // Bounding box
      {
	if (IsInfinite())
	  {
	    StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer);
	  }
	else
	  {
	    AIS_Shape::DisplayBox(aPrs,BoundingBox(),myDrawer);
	  }
	break;
      }

    case 3: // texture mapping on triangulation 
      {
	BRepTools::Clean(myshape);
	BRepTools::Update(myshape);

	Handle(Graphic3d_StructureManager) aStrucMana = GetContext()->MainPrsMgr()->StructureManager();
	  {  
	    Handle(Prs3d_ShadingAspect) aPrs3d_ShadingAspect = new Prs3d_ShadingAspect;
	    myAspect = aPrs3d_ShadingAspect->Aspect();
	  }
	if (!DoMapTexture)
	  {
	    myAspect->SetTextureMapOff();
	    return;
	  }
	myAspect->SetTextureMapOn();
	
	if(myPredefTexture!=-1)
	  mytexture = new Graphic3d_Texture2Dmanual(aStrucMana, myPredefTexture);
	else
	  mytexture = new Graphic3d_Texture2Dmanual(aStrucMana, myTextureFile.ToCString());

	if (!mytexture->IsDone())
	  {
	    cout <<"An error occured while building texture \n" <<endl;
	    return;
	  }
	if (myModulate)
	  mytexture->EnableModulate();
	else
	  mytexture->DisableModulate();

	myAspect->SetTextureMap(mytexture);
	if (DoShowTriangles)
	  myAspect->SetEdgeOn();
	else
	  myAspect->SetEdgeOff();

	if (DoRepeat)
	  mytexture->EnableRepeat();
	else
	  mytexture->DisableRepeat();

	myDeflection = AIS_Shape::GetDeflection(myshape,myDrawer);
	BRepMesh::Mesh(myshape,myDeflection);
	// Adds a triangulation of the shape myshape to its topological data structure.
	// This triangulation is computed with the deflection myDeflection.

#ifdef DEBUG
	cout <<"Deflection =  " << myDeflection << "\n" << endl;
#endif

	StdPrs_ToolShadedShape SST;

	Standard_Integer NumFace;
	TopExp_Explorer ExpFace;

	for( NumFace=0,ExpFace.Init(myshape,TopAbs_FACE); ExpFace.More(); ExpFace.Next(),NumFace++ )
	  {
	    TopoDS_Face		myFace		= TopoDS::Face(ExpFace.Current());
	    TopLoc_Location aLocation	= myFace.Location();

#ifdef DEBUG
	    cout << "J\'explore actuellement la face " << NumFace << "\n" << endl;
#endif
	    Handle(Poly_Triangulation) myT = BRep_Tool::Triangulation(myFace, aLocation);
	    // Returns  the Triangulation of  the  face. It  is a null handle if there is no triangulation.

	    if (myT.IsNull())
	      {
#ifdef DEBUG
		cout << "Triangulation of the face "<< i <<" is null \n"<< endl;
#endif
		return;
	      }
	    Poly_Connect pc(myT);	
	    const TColgp_Array1OfPnt& Nodes = myT->Nodes();
	    const TColgp_Array1OfPnt2d& UVNodes = myT->UVNodes();
	    const Poly_Array1OfTriangle& triangles = myT->Triangles();
	    TColgp_Array1OfDir myNormal(Nodes.Lower(), Nodes.Upper());
	    
	    SST.Normal(myFace, pc, myNormal);
	    BRepTools::UVBounds(myFace,Umin, Umax, Vmin, Vmax);
	    dUmax = (Umax - Umin);
	    dVmax = (Vmax - Vmin);
	    Handle(Graphic3d_Group) mygroup = Prs3d_Root::CurrentGroup(aPrs);

	    Standard_Integer nnn = myT->NbTriangles();					// nnn : number of triangles
	    Standard_Integer nt, n1, n2, n3 = 0;						// nt  : current triangle
	    // ni  : top i of the current triangle
	    for (nt = 1; nt <= nnn; nt++)					
	      {
#ifdef DEBUG
		cout << "On traite actuellement le triangle : "<< nt <<"\n";
#endif						
		if (SST.Orientation(myFace) == TopAbs_REVERSED)			// if the face is "reversed"
		  triangles(nt).Get(n1,n3,n2);						// the triangle is n1,n3,n2
		else 
		  triangles(nt).Get(n1,n2,n3);						// the triangle is n1,n2,n3

		if (TriangleIsValid (Nodes(n1),Nodes(n2),Nodes(n3)) )
		  {	// Associates vertexNT to each node
		    Graphic3d_Array1OfVertexNT Points(1,3);
		    Aspect_Array1OfEdge aretes(1,3);

		    mygroup->BeginPrimitives();
		    {
		      gp_Pnt p = Nodes(n1).Transformed(aLocation.Transformation());
		      gp_Pnt q = Nodes(n2).Transformed(aLocation.Transformation());
		      gp_Pnt r = Nodes(n3).Transformed(aLocation.Transformation());

		      Points(1).SetCoord(p.X(), p.Y(), p.Z());
		      Points(2).SetCoord(q.X(), q.Y(), q.Z());
		      Points(3).SetCoord(r.X(), r.Y(), r.Z());

		      Points(1).SetNormal(myNormal(n1).X(), myNormal(n1).Y(), myNormal(n1).Z());
		      Points(2).SetNormal(myNormal(n2).X(), myNormal(n2).Y(), myNormal(n2).Z());
		      Points(3).SetNormal(myNormal(n3).X(), myNormal(n3).Y(), myNormal(n3).Z());

		      Points(1).SetTextureCoordinate((-myUOrigin+(myURepeat*(UVNodes(n1).X()-Umin))/dUmax)/myScaleU,
						     (-myVOrigin+(myVRepeat*(UVNodes(n1).Y()-Vmin))/dVmax)/myScaleV);
		      Points(2).SetTextureCoordinate((-myUOrigin+(myURepeat*(UVNodes(n2).X()-Umin))/dUmax)/myScaleU,
						     (-myVOrigin+(myVRepeat*(UVNodes(n2).Y()-Vmin))/dVmax)/myScaleV);
		      Points(3).SetTextureCoordinate((-myUOrigin+(myURepeat*(UVNodes(n3).X()-Umin))/dUmax)/myScaleU,
						     (-myVOrigin+(myVRepeat*(UVNodes(n3).Y()-Vmin))/dVmax)/myScaleV);
		      
		      aretes(1).SetValues(1, 2, Aspect_TOE_INVISIBLE);
		      aretes(2).SetValues(2, 3, Aspect_TOE_INVISIBLE);
		      aretes(3).SetValues(3, 1, Aspect_TOE_INVISIBLE);
		    }
		    mygroup->EndPrimitives();
		    mygroup->TriangleSet(Points, aretes, Standard_True);	

		  } // end of "if the triangle is valid
	      } // end of the "parcours" of the triangles
	    mygroup->SetGroupPrimitivesAspect(myAspect);
	  }// end of the exploration of the shape in faces
	break;
      }// end case 3

    } // end switch
  //  aPrs->ReCompute(); // for hidden line recomputation if necessary...
}





/////////////////////////////////////////////////////////
//		QUERY METHODS
/////////////////////////////////////////////////////////



Standard_Boolean AIS_TexturedShape::TextureMapState() const
{
  return DoMapTexture;
}

Standard_Real AIS_TexturedShape::URepeat() const
{
  return myURepeat;
}

Standard_Boolean AIS_TexturedShape::TextureRepeat() const
{
  return DoRepeat;
}

Standard_Real AIS_TexturedShape::Deflection() const
{
  return myDeflection;
}

Standard_CString AIS_TexturedShape::TextureFile() const
{
  return myTextureFile.ToCString();
}

Standard_Real AIS_TexturedShape::VRepeat() const
{
  return myVRepeat;
}
Standard_Boolean AIS_TexturedShape::ShowTriangles() const
{
  return DoShowTriangles;
}
Standard_Real AIS_TexturedShape::TextureUOrigin() const
{
  return myUOrigin;
}
Standard_Real AIS_TexturedShape::TextureVOrigin() const
{
  return myVOrigin;
}
Standard_Real AIS_TexturedShape::TextureScaleU() const
{
  return myScaleU;
}
Standard_Real AIS_TexturedShape::TextureScaleV() const
{
  return myScaleV;
}
Standard_Boolean	AIS_TexturedShape::TextureScale() const
{
  return DoSetTextureScale;
}
Standard_Boolean	AIS_TexturedShape::TextureOrigin() const
{
  return DoSetTextureOrigin;
}
Standard_Boolean	AIS_TexturedShape::TextureModulate() const
{
  return myModulate;
}
