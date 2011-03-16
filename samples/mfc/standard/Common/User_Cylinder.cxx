#include <stdafx.h>

#include <User_Cylinder.hxx>

// Implementation of Handle and type mgt
//
IMPLEMENT_STANDARD_HANDLE(User_Cylinder,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(User_Cylinder,AIS_InteractiveObject)

#include "ColoredMeshDlg.h"

#include <Graphic3d_StructureManager.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <BRepMesh.hxx>
#include <StdPrs_ToolShadedShape.hxx>
#include <Poly_Connect.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <GProp_PGProps.hxx>
#include <Graphic3d_Array1OfVertexNC.hxx>
#include <Aspect_Array1OfEdge.hxx>
#include <Quantity_Color.hxx>

#include <AIS_GraphicTool.hxx>

// Constructors implementation
//

User_Cylinder::User_Cylinder(const Standard_Real R, const Standard_Real H) :
AIS_InteractiveObject(PrsMgr_TOP_ProjectorDependant)
{
	BRepPrimAPI_MakeCylinder S(R,H);
	myShape = S.Shape();
	SetHilightMode(0);
	SetSelectionMode(0);
	myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
	myPlanarFaceColor = Quantity_NOC_FIREBRICK3;
	myCylindricalFaceColor = Quantity_NOC_GRAY;
}

User_Cylinder::User_Cylinder(const gp_Ax2 CylAx2, const Standard_Real R, const Standard_Real H) :
AIS_InteractiveObject(PrsMgr_TOP_ProjectorDependant)

{
	BRepPrimAPI_MakeCylinder S(CylAx2,R,H);
	BRepBuilderAPI_NurbsConvert aNurbsConvert(S.Shape());
	myShape = aNurbsConvert.Shape();
	SetHilightMode(0);
	SetSelectionMode(0);
	myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
	myPlanarFaceColor = Quantity_NOC_FIREBRICK3;
	myCylindricalFaceColor = Quantity_NOC_KHAKI4;
}

void User_Cylinder::Compute(const Handle_PrsMgr_PresentationManager3d& aPresentationManager,
							const Handle_Prs3d_Presentation& aPresentation,
							const Standard_Integer aMode ) 
{
//	Quantity_NameOfColor aColor;
	switch (aMode) {
	case 0:
//		aColor = AIS_InteractiveObject::Color();
//		SetColor(aColor);
//		myDrawer->WireAspect()->SetColor (aColor);
		StdPrs_WFDeflectionShape::Add(aPresentation,myShape, myDrawer );
		break;
	case 1:
	{
/*
		myDrawer->ShadingAspect()->SetMaterial(Graphic3d_NOM_BRASS);
		myDrawer->SetShadingAspectGlobal(Standard_False);
		TopExp_Explorer Ex;
		Handle(Geom_Surface) Surface; 
		for (Ex.Init(myShape,TopAbs_FACE); Ex.More(); Ex.Next())
		{
			Surface = BRep_Tool::Surface(TopoDS::Face(Ex.Current()));
			if (Surface->IsKind(STANDARD_TYPE(Geom_Plane)))
				myDrawer->ShadingAspect()->SetColor(myPlanarFaceColor);
			else
				myDrawer->ShadingAspect()->SetColor(myCylindricalFaceColor);
			StdPrs_ShadedShape::Add(aPresentation,Ex.Current(), myDrawer);
		}
		
		break;
*/
		
		Standard_Real aTransparency = Transparency();
		Graphic3d_NameOfMaterial aMaterial = Material();
//		aColor = AIS_InteractiveObject::Color();
				

		myDrawer->SetShadingAspectGlobal(Standard_False);
		TopExp_Explorer Ex;
		Handle(Geom_Surface) Surface; 
		
		for (Ex.Init(myShape,TopAbs_FACE); Ex.More(); Ex.Next())
		{
			
			Surface = BRep_Tool::Surface(TopoDS::Face(Ex.Current()));
			myDrawer->ShadingAspect()->SetMaterial(aMaterial);	
			if (Surface->IsKind(STANDARD_TYPE(Geom_Plane)))
				myDrawer->ShadingAspect()->SetColor(myPlanarFaceColor);
			else
				myDrawer->ShadingAspect()->SetColor(myCylindricalFaceColor);
			
			
			myDrawer->ShadingAspect()->SetTransparency (aTransparency);
			StdPrs_ShadedShape::Add(aPresentation,Ex.Current(), myDrawer);
		}
//		myDrawer->ShadingAspect()->SetMaterial(aMaterial);	
		//myDrawer->ShadingAspect()->SetTransparency (aTransparency);
//		if(HasColor())
//			myDrawer->ShadingAspect()->SetColor (aColor);
//		StdPrs_ShadedShape::Add(aPresentation,myShape, myDrawer);
		
		break;

	}
    case 6:  //color
	{
		BRepTools::Clean(myShape);
		BRepTools::Update(myShape);

		Handle(Graphic3d_StructureManager) aStrucMana = GetContext()->MainPrsMgr()->StructureManager();

		Handle(Graphic3d_Group) mygroup = Prs3d_Root::CurrentGroup(aPresentation);
		myAspect = (new Prs3d_ShadingAspect())->Aspect();
		Graphic3d_MaterialAspect material = myAspect->FrontMaterial();
		material.SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
		material.SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
		material.SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
		material.SetReflectionModeOff(Graphic3d_TOR_EMISSION);
		myAspect->SetFrontMaterial(material);

		mygroup->SetPrimitivesAspect(myAspect);	
		myAspect->SetEdgeOn();

		myDeflection = AIS_Shape::GetDeflection(myShape,myDrawer);
		BRepMesh::Mesh(myShape,myDeflection);

		myX1OnOff = Standard_False;
		myXBlueOnOff = Standard_False;
		myXGreenOnOff =Standard_False;
		myXRedOnOff = Standard_False;
		myY1OnOff = Standard_False;
		myYBlueOnOff = Standard_False;
		myYGreenOnOff = Standard_False;
		myYRedOnOff = Standard_False;
		myZ1OnOff = Standard_False;
		myZBlueOnOff =Standard_False;
		myZGreenOnOff = Standard_False;
		myZRedOnOff = Standard_False;

		CColoredMeshDlg Dlg(NULL);
		Dlg.DoModal();

		myX1OnOff = Dlg.X1OnOff;

		myXBlueOnOff = Dlg.m_CheckXBlueOnOff;
		myXGreenOnOff = Dlg.m_CheckXGreenOnOff;
		myXRedOnOff = Dlg.m_CheckXRedOnOff;

		myY1OnOff = Dlg.Y1OnOff;

		myYBlueOnOff = Dlg.m_CheckYBlueOnOff;
		myYGreenOnOff = Dlg.m_CheckYGreenOnOff;
		myYRedOnOff = Dlg.m_CheckYRedOnOff;

		myZ1OnOff = Dlg.Z1OnOff;

		myZBlueOnOff = Dlg.m_CheckZBlueOnOff;
		myZGreenOnOff = Dlg.m_CheckZGreenOnOff;
		myZRedOnOff = Dlg.m_CheckZRedOnOff;

	// Adds a triangulation of the shape myShape to its topological data structure.
	// This triangulation is computed with the deflection myDeflection.

#ifdef DEBUG
	cout <<"Deflection =  " << myDeflection << "\n" << endl;
#endif

		StdPrs_ToolShadedShape SST;

		Standard_Integer NumFace;
		TopExp_Explorer ExpFace;

//khr -->

		gp_Pnt H (0,0,0);
		gp_Pnt B (0,0,1000000000);
		for( NumFace=0,ExpFace.Init(myShape,TopAbs_FACE); ExpFace.More(); ExpFace.Next(),NumFace++ )
		{
			TopoDS_Face		myFace		= TopoDS::Face(ExpFace.Current());
			TopLoc_Location myLocation	= myFace.Location();

#ifdef DEBUG
	    cout << "J\'explore actuellement la face " << NumFace << "\n" << endl;
#endif
			Handle(Poly_Triangulation) myT = BRep_Tool::Triangulation(myFace, myLocation);
	    // Returns  the Triangulation of  the  face. It  is a null handle if there is no triangulation.

			if (myT.IsNull())
			{
#ifdef DEBUG
//		cout << "Triangulation of the face "<< i <<" is null \n"<< endl;
#endif
				return;
			}
			
			const TColgp_Array1OfPnt&  Nodes= myT->Nodes();
			
			const Poly_Array1OfTriangle& triangles = myT->Triangles();
						
			Standard_Integer nnn = myT->NbTriangles();					// nnn : nombre de triangles
			Standard_Integer nt, n1, n2, n3 = 0;						// nt  : triangle courant
	    // ni  : sommet i du triangle courant


//recherche du pt "haut" et du pt "bas

			for (nt = 1; nt <= nnn; nt++)
			{
//				triangles(nt).Get(n1,n2,n3);						// le triangle est n1,n2,n3

				if (SST.Orientation(myFace) == TopAbs_REVERSED)			// si la face est "reversed"
					triangles(nt).Get(n1,n3,n2);						// le triangle est n1,n3,n2
				else 
					triangles(nt).Get(n1,n2,n3);						// le triangle est n1,n2,n3
			
				if (TriangleIsValid (Nodes(n1),Nodes(n2),Nodes(n3)) )
				{	// Associates a vertexNT to each node
					gp_Pnt p = Nodes(n1).Transformed(myLocation.Transformation());
					gp_Pnt q = Nodes(n2).Transformed(myLocation.Transformation());
					gp_Pnt r = Nodes(n3).Transformed(myLocation.Transformation());

					if (p.Z() > H.Z()) H=p;						
					if (q.Z() > H.Z()) H=q;						
					if (r.Z() > H.Z()) H=r;
					if (p.Z() < B.Z()) B=p;
					if (q.Z() < B.Z()) B=q;
					if (r.Z() < B.Z()) B=r;
				}
			}	
		}
	
//khr <--


		for( NumFace=0,ExpFace.Init(myShape,TopAbs_FACE); ExpFace.More(); ExpFace.Next(),NumFace++ )
		{
			TopoDS_Face		myFace		= TopoDS::Face(ExpFace.Current());
			TopLoc_Location myLocation	= myFace.Location();

#ifdef DEBUG
	    cout << "J\'explore actuellement la face " << NumFace << "\n" << endl;
#endif
			Handle(Poly_Triangulation) myT = BRep_Tool::Triangulation(myFace, myLocation);
	    // Returns  the Triangulation of  the  face. It  is a null handle if there is no triangulation.

			if (myT.IsNull())
			{
#ifdef DEBUG
		//cout << "Triangulation of the face "<< i <<" is null \n"<< endl;
#endif
				return;
			}
			Poly_Connect pc(myT);	
			const TColgp_Array1OfPnt&  Nodes= myT->Nodes();
			BAR = GProp_PGProps::Barycentre(Nodes);


			const TColgp_Array1OfPnt2d& UVNodes = myT->UVNodes();
			const Poly_Array1OfTriangle& triangles = myT->Triangles();
			TColgp_Array1OfDir myNormal(Nodes.Lower(), Nodes.Upper());
			
			SST.Normal(myFace, pc, myNormal);
			BRepTools::UVBounds(myFace,Umin, Umax, Vmin, Vmax);
			dUmax = (Umax - Umin);
			dVmax = (Vmax - Vmin);

			Standard_Integer nnn = myT->NbTriangles();					// nnn : nombre de triangles
			Standard_Integer nt, n1, n2, n3 = 0;						// nt  : triangle courant
	    // ni  : sommet i du triangle courant


//recherche du pt "haut" et du pt "bas
//			gp_Pnt H (0,0,0);
//			gp_Pnt B (0,0,1000000000);

			for (nt = 1; nt <= nnn; nt++)
			{
//				triangles(nt).Get(n1,n2,n3);						// le triangle est n1,n2,n3

				if (SST.Orientation(myFace) == TopAbs_REVERSED)			// si la face est "reversed"
					triangles(nt).Get(n1,n3,n2);						// le triangle est n1,n3,n2
				else 
					triangles(nt).Get(n1,n2,n3);						// le triangle est n1,n2,n3
			
				if (TriangleIsValid (Nodes(n1),Nodes(n2),Nodes(n3)) )
				{	// Associates a vertexNT to each node
					gp_Pnt p = Nodes(n1).Transformed(myLocation.Transformation());
					gp_Pnt q = Nodes(n2).Transformed(myLocation.Transformation());
					gp_Pnt r = Nodes(n3).Transformed(myLocation.Transformation());
/*
					if (p.Z() > H.Z()) H=p;						
					if (q.Z() > H.Z()) H=q;						
					if (r.Z() > H.Z()) H=r;
					if (p.Z() < B.Z()) B=p;
					if (q.Z() < B.Z()) B=q;
					if (r.Z() < B.Z()) B=r;
*/
				}
			}	

			for (nt = 1; nt <= nnn; nt++)					
			{
#ifdef DEBUG
		cout << "On traite actuellement le triangle : "<< nt <<"\n";
#endif						
				if (SST.Orientation(myFace) == TopAbs_REVERSED)			// si la face est "reversed"
					triangles(nt).Get(n1,n3,n2);						// le triangle est n1,n3,n2
				else 
					triangles(nt).Get(n1,n2,n3);						// le triangle est n1,n2,n3

				if (TriangleIsValid (Nodes(n1),Nodes(n2),Nodes(n3)) )
				{	// Associates a vertexNT to each node
					Graphic3d_Array1OfVertexNC Points(1,3);
					Aspect_Array1OfEdge aretes(1,3);

		
					mygroup->BeginPrimitives();
					{
						gp_Pnt p = Nodes(n1).Transformed(myLocation.Transformation());
						gp_Pnt q = Nodes(n2).Transformed(myLocation.Transformation());
						gp_Pnt r = Nodes(n3).Transformed(myLocation.Transformation());

						Points(1).SetCoord(p.X(), p.Y(), p.Z());
						Points(2).SetCoord(q.X(), q.Y(), q.Z());
						Points(3).SetCoord(r.X(), r.Y(), r.Z());

				
						Points(1).SetNormal(myNormal(n1).X(), myNormal(n1).Y(), myNormal(n1).Z());
						Points(2).SetNormal(myNormal(n2).X(), myNormal(n2).Y(), myNormal(n2).Z());
						Points(3).SetNormal(myNormal(n3).X(), myNormal(n3).Y(), myNormal(n3).Z());
//ICI!!!
/*						
						Points(1).SetColor(Color(p,abs(int(B.Z())),abs(int(H.Z())),Dlg.Colorization));
						Points(2).SetColor(Color(q,abs(int(B.Z())),abs(int(H.Z())),Dlg.Colorization));
						Points(3).SetColor(Color(r,abs(int(B.Z())),abs(int(H.Z())),Dlg.Colorization));
*/
						Points(1).SetColor(Color(p,B.Z(),H.Z(),Dlg.Colorization));
						Points(2).SetColor(Color(q,B.Z(),H.Z(),Dlg.Colorization));
						Points(3).SetColor(Color(r,B.Z(),H.Z(),Dlg.Colorization));

				
			/*	Points(1).SetColor(Altitude(p,abs(B.Z()),abs(H.Z())));
				Points(2).SetColor(Altitude(q,abs(B.Z()),abs(H.Z())));
				Points(3).SetColor(Altitude(r,abs(B.Z()),abs(H.Z())));*/


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
	}
	}
}

void User_Cylinder::Compute(const Handle_Prs3d_Projector& aProjector,
							const Handle_Prs3d_Presentation& aPresentation)
{
	myDrawer->EnableDrawHiddenLine();
	StdPrs_HLRPolyShape::Add(aPresentation,myShape,myDrawer,aProjector);
}

void User_Cylinder::ComputeSelection(const Handle_SelectMgr_Selection& aSelection,
									 const Standard_Integer aMode)
{
	switch(aMode){
	case 0:
		StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_SHAPE, 0, 0);
		break;
	case 4:
		StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_FACE, 0, 0);
		break;
	}
}

Standard_Integer User_Cylinder::NbPossibleSelection() const
{
	return 2;
}

Standard_Boolean User_Cylinder::AcceptShapeDecomposition() const
{
	return Standard_True;
}

void User_Cylinder::SetPlanarFaceColor(const Quantity_Color acolor)
{
	myPlanarFaceColor = acolor;
}

void User_Cylinder::SetCylindricalFaceColor(const Quantity_Color acolor)
{
	myCylindricalFaceColor = acolor;
}

Standard_Boolean User_Cylinder::TriangleIsValid(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3) const
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

Quantity_Color User_Cylinder::Color(gp_Pnt& thePoint,Standard_Real AltMin,Standard_Real AltMax,
									const Standard_Integer ColorizationMode) 
{
	red =1;   //initializing colors parameters
	green=1;
	blue =1;
	switch ( ColorizationMode)
	{
		case 0 : //normal, vert/maron
		{
			Standard_Real Alt= thePoint.Z();

			Standard_Real AltDelta;

			AltDelta = AltMax-AltMin;

			red = 0.5- ((0.5*(AltMax-Alt))/(AltDelta));
		//	red = 1/(((1000/(AltMax-AltMin))*Alt)+1000*(1-(AltMin/(AltMax-AltMin))));
		
		//	green = (3*AltMax-AltMin)/(3*AltMax-8*AltMin + 7*Alt);
		//	green = 1/(((7/(3*AltMax-AltMin))*Alt) + 1-(7*AltMin/(3*AltMax-AltMin)));
			Standard_Real A = 7*Alt-7*AltMin;
			green = (3*AltMax-AltMin)/(3*AltMax-AltMin+(7*Alt-7*AltMin));
//			AfxMessageBox(green);
		//	green =(0.30- (((0.3-1)*(AltMax-Alt))/(AltMax-AltMin)));
		
			blue = 0 ;
/*
			red = 0.5;
			green = 0.2222;
			blue = 0;
*/	
			Quantity_Color color;
			color.SetValues(red,green,blue, Quantity_TOC_RGB);
			return color;
			break;
		}//end case 0

		case 1 :	//mer-neige
		{		
			Standard_Real Alt= thePoint.Z();

			Standard_Real b =AltMax-AltMin;
			Standard_Real a= AltMax-thePoint.Z();

			red =1;
			green=1;
			blue =1;
			if (0<a && a <= (b/5))
			{
				red = 1;
				green = 1;
				blue = 1;
			}
			else if ((b/5)<a  && a <= (2*b/5))
			{
				 red = .55;
				 green = 0.3;
				 blue = 0;
			}
			else if ((2*b/5)<a  && a <= (18*b/20))
			{
				 green =1/(((7/(3*AltMax-AltMin))*Alt)+(1-(7*AltMin/(3*AltMax-AltMin))));
				 red = 1/(((1000/(AltMax-AltMin))*Alt)+1000*(1-(AltMin/(AltMax-AltMin))));
				 blue = 0;
			}
			else if ((18*b/20)<a  && a <= (18.5*b/20))
			{
				 red = 0.75;
				 green = 0.66;
				 blue = 0;
			}

			else if ((18.5*b/20)<a  && a <= b)
			{
				red = 0.25;
				green = .66;
				blue = 1;
			}
			Quantity_Color color;
			color.SetValues(red,green,blue, Quantity_TOC_RGB);
			return color;
			break;
		}//end case 1

		case 2 :
		{
			gp_Pnt P (85.,0.,-105.);
			gp_Vec TheVect ( P, thePoint);
			Standard_Real CoordX;
			Standard_Real CoordY;
			Standard_Real CoordZ;

			CoordX = TheVect.X();
			CoordY = TheVect.Y();
			CoordZ = TheVect.Z();
/*
			Standard_Real maxixy = Max(fabs(CoordX),fabs(CoordY));
			Standard_Real maxiyz = Max(fabs(CoordY),fabs(CoordZ));
			Standard_Real Maxi = Max(maxixy,maxiyz);

*/
			Standard_Real Distance = BAR.Distance ( P);

			//red = (abs(CoordX))/(1*Distance) ;
			//green =(abs(CoordY))/(1*Distance)	;
			//blue = (abs(CoordZ))/(1*Distance)	;

			Standard_Real a =fabs(CoordX);
			Standard_Real b =fabs(CoordY);
			Standard_Real c =fabs(CoordZ);

Standard_Real xx = a / Max(Distance,a); //(Max (Distance, Maxi));
Standard_Real yy = b / Max(Distance,b); //(Max (Distance, Maxi));	
Standard_Real zz = c / Max(Distance,c); //(Max (Distance, Maxi));			
			
	
			if (myXRedOnOff)
			red = xx;
			else if (myXGreenOnOff)
			green =xx;
			else if (myXBlueOnOff)
			blue=xx;

			if (myYRedOnOff)
			red = yy;
			else if (myYGreenOnOff)
			green = yy;
			else if (myYBlueOnOff)
			blue = yy;
			
			if (myZRedOnOff)
			red = zz;
			else if (myZGreenOnOff)
			green = zz;
			else if (myZBlueOnOff)
			blue = zz;

		/*	if (myX1OnOff)
			if (myY1OnOff)
			if (myZ1OnOff)*/


			Quantity_Color color;
			color.SetValues(red,green,blue, Quantity_TOC_RGB);
			return color;
			break;
		}//end case 2
	}//end switch

	Quantity_Color c;
	return c;
}

void User_Cylinder::SetColor(const Quantity_Color &aColor)
{
	AIS_InteractiveObject::SetColor(aColor);
	SetPlanarFaceColor(aColor);
	SetCylindricalFaceColor(aColor);
}
