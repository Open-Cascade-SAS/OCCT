#include <stdafx.h>

#include <User_Cylinder.hxx>

// Implementation of Handle and type mgt
//

IMPLEMENT_STANDARD_RTTIEXT(User_Cylinder,AIS_InteractiveObject)

#include "ColoredMeshDlg.h"

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <StdPrs_ToolTriangulatedShape.hxx>
#include <Poly_Connect.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <GProp_PGProps.hxx>
#include <Quantity_Color.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Prs3d.hxx>

#include <AIS_GraphicTool.hxx>

// Constructors implementation
//

User_Cylinder::User_Cylinder(const Standard_Real R, const Standard_Real H) :
AIS_InteractiveObject(PrsMgr_TOP_ProjectorDependant)
{
  BRepPrimAPI_MakeCylinder S(R,H);
  myShape = S.Shape();
  SetHilightMode(0);
  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myColor = Quantity_NOC_GRAY;
}

User_Cylinder::User_Cylinder(const gp_Ax2 CylAx2, const Standard_Real R, const Standard_Real H) :
AIS_InteractiveObject(PrsMgr_TOP_ProjectorDependant)

{
  BRepPrimAPI_MakeCylinder S(CylAx2,R,H);
  BRepBuilderAPI_NurbsConvert aNurbsConvert(S.Shape());
  myShape = aNurbsConvert.Shape();
  SetHilightMode(0);
  myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
  myColor = Quantity_NOC_KHAKI4;
}

void User_Cylinder::Compute(const Handle(PrsMgr_PresentationManager3d)& /*aPresentationManager*/,
                            const Handle(Prs3d_Presentation)& aPresentation,
                            const Standard_Integer aMode ) 
{
  switch (aMode) {
case AIS_WireFrame:
  {
    StdPrs_WFShape::Add(aPresentation,myShape, myDrawer );
    break;
  }
case AIS_Shaded:
  {
    Standard_Real aTransparency = Transparency();
    Graphic3d_NameOfMaterial aMaterial = Material();
    myDrawer->ShadingAspect()->SetMaterial(aMaterial);
    myDrawer->ShadingAspect()->SetColor(myColor);
    myDrawer->ShadingAspect()->SetTransparency (aTransparency);
    StdPrs_ShadedShape::Add(aPresentation,myShape, myDrawer);
    break;
  }
case 6: //color
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

    myDeflection = Prs3d::GetDeflection(myShape,myDrawer);
    BRepMesh_IncrementalMesh(myShape,myDeflection);

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

    myXBlueOnOff  = Dlg.m_CheckXBlueOnOff  != 0;
    myXGreenOnOff = Dlg.m_CheckXGreenOnOff != 0;
    myXRedOnOff   = Dlg.m_CheckXRedOnOff   != 0;

    myY1OnOff = Dlg.Y1OnOff;

    myYBlueOnOff  = Dlg.m_CheckYBlueOnOff  != 0;
    myYGreenOnOff = Dlg.m_CheckYGreenOnOff != 0;
    myYRedOnOff   = Dlg.m_CheckYRedOnOff   != 0;

    myZ1OnOff = Dlg.Z1OnOff;

    myZBlueOnOff  = Dlg.m_CheckZBlueOnOff  != 0;
    myZGreenOnOff = Dlg.m_CheckZGreenOnOff != 0;
    myZRedOnOff   = Dlg.m_CheckZRedOnOff   != 0;

    // Adds a triangulation of the shape myShape to its topological data structure.
    // This triangulation is computed with the deflection myDeflection.

#ifdef DEBUG
    cout <<"Deflection = " << myDeflection << "\n" << endl;
#endif

    Standard_Integer NumFace;
    TopExp_Explorer ExpFace;

    //khr -->

    gp_Pnt H (0,0,0);
    gp_Pnt B (0,0,1000000000);
    for( NumFace=0,ExpFace.Init(myShape,TopAbs_FACE); ExpFace.More(); ExpFace.Next(),NumFace++ )
    {
      TopoDS_Face myFace = TopoDS::Face(ExpFace.Current());
      TopLoc_Location myLocation = myFace.Location();

#ifdef DEBUG
      cout << "J\'explore actuellement la face " << NumFace << "\n" << endl;
#endif
      Handle(Poly_Triangulation) myT = BRep_Tool::Triangulation(myFace, myLocation);
      // Returns the Triangulation of the face. It is a null handle if there is no triangulation.

      if (myT.IsNull())
      {
#ifdef DEBUG
        // cout << "Triangulation of the face "<< i <<" is null \n"<< endl;
#endif
        return;
      }

      const TColgp_Array1OfPnt& Nodes= myT->Nodes();

      const Poly_Array1OfTriangle& triangles = myT->Triangles();

      Standard_Integer nnn = myT->NbTriangles(); // nnn : nombre de triangles
      Standard_Integer nt, n1, n2, n3 = 0;// nt : triangle courant
      // ni : sommet i du triangle courant
      //recherche du pt "haut" et du pt "bas
      for (nt = 1; nt <= nnn; nt++)
      {
        // triangles(nt).Get(n1,n2,n3); // le triangle est n1,n2,n3

        if (myFace.Orientation() == TopAbs_REVERSED) // si la face est "reversed"
          triangles(nt).Get(n1,n3,n2); // le triangle est n1,n3,n2
        else
          triangles(nt).Get(n1,n2,n3); // le triangle est n1,n2,n3

        if (TriangleIsValid (Nodes(n1),Nodes(n2),Nodes(n3)) )
        { // Associates a vertexNT to each node
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
      TopoDS_Face myFace = TopoDS::Face(ExpFace.Current());
      TopLoc_Location myLocation = myFace.Location();

#ifdef DEBUG
      cout << "J\'explore actuellement la face " << NumFace << "\n" << endl;
#endif
      Handle(Poly_Triangulation) myT = BRep_Tool::Triangulation(myFace, myLocation);
      // Returns the Triangulation of the face. It is a null handle if there is no triangulation.

      if (myT.IsNull())
      {
#ifdef DEBUG
        //cout << "Triangulation of the face "<< i <<" is null \n"<< endl;
#endif
        return;
      }
      Poly_Connect pc(myT);
      const TColgp_Array1OfPnt& Nodes= myT->Nodes();
      BAR = GProp_PGProps::Barycentre(Nodes);


      //const TColgp_Array1OfPnt2d& UVNodes = myT->UVNodes();
      const Poly_Array1OfTriangle& triangles = myT->Triangles();
      TColgp_Array1OfDir myNormal(Nodes.Lower(), Nodes.Upper());

      StdPrs_ToolTriangulatedShape::Normal(myFace, pc, myNormal);
      BRepTools::UVBounds(myFace,Umin, Umax, Vmin, Vmax);
      dUmax = (Umax - Umin);
      dVmax = (Vmax - Vmin);

      Standard_Integer nnn = myT->NbTriangles(); // nnn : nombre de triangles
      Standard_Integer nt, n1, n2, n3 = 0;// nt : triangle courant
      // ni : sommet i du triangle courant

      //recherche du pt "haut" et du pt "bas
      // gp_Pnt H (0,0,0);
      // gp_Pnt B (0,0,1000000000);

      for (nt = 1; nt <= nnn; nt++)
      {
        // triangles(nt).Get(n1,n2,n3); // le triangle est n1,n2,n3
        if (myFace.Orientation() == TopAbs_REVERSED) // si la face est "reversed"
          triangles(nt).Get(n1,n3,n2); // le triangle est n1,n3,n2
        else
          triangles(nt).Get(n1,n2,n3); // le triangle est n1,n2,n3

        if (TriangleIsValid (Nodes(n1),Nodes(n2),Nodes(n3)) )
        { // Associates a vertexNT to each node
          gp_Pnt p = Nodes(n1).Transformed(myLocation.Transformation());
          gp_Pnt q = Nodes(n2).Transformed(myLocation.Transformation());
          gp_Pnt r = Nodes(n3).Transformed(myLocation.Transformation());
        }
      }

      Handle(Graphic3d_ArrayOfTriangles) aOP = new Graphic3d_ArrayOfTriangles(3 * nnn, 0, Standard_True, Standard_True);

      for (nt = 1; nt <= nnn; nt++)
      {
#ifdef DEBUG
        cout << "On traite actuellement le triangle : "<< nt <<"\n";
#endif
        if (myFace.Orientation() == TopAbs_REVERSED) // si la face est "reversed"
          triangles(nt).Get(n1,n3,n2); // le triangle est n1,n3,n2
        else
          triangles(nt).Get(n1,n2,n3); // le triangle est n1,n2,n3

        if (TriangleIsValid (Nodes(n1),Nodes(n2),Nodes(n3)) )
        { // Associates a vertexNT to each node

          TColgp_Array1OfPnt Points(1,3);

          gp_Pnt p = Nodes(n1).Transformed(myLocation.Transformation());
          gp_Pnt q = Nodes(n2).Transformed(myLocation.Transformation());
          gp_Pnt r = Nodes(n3).Transformed(myLocation.Transformation());

          Points(1).SetCoord(p.X(), p.Y(), p.Z());
          Points(2).SetCoord(q.X(), q.Y(), q.Z());
          Points(3).SetCoord(r.X(), r.Y(), r.Z());

          aOP->AddVertex(Points(1), myNormal(n1), Color(p,B.Z(),H.Z(),Dlg.Colorization));
          aOP->AddVertex(Points(2), myNormal(n2), Color(q,B.Z(),H.Z(),Dlg.Colorization));
          aOP->AddVertex(Points(3), myNormal(n3), Color(r,B.Z(),H.Z(),Dlg.Colorization));
        } // end of "if the triangle is valid
      } // end of the "parcours" of the triangles

      Prs3d_Root::CurrentGroup (aPresentation)->AddPrimitiveArray (aOP);

      mygroup->SetGroupPrimitivesAspect(myAspect);
    }// end of the exploration of the shape in faces

    break;
  }
  }
}

void User_Cylinder::Compute(const Handle(Prs3d_Projector)& aProjector,
                            const Handle(Prs3d_Presentation)& aPresentation)
{
  Handle (Prs3d_Drawer) aDefDrawer = GetContext()->DefaultDrawer();
  if (aDefDrawer->DrawHiddenLine())
    myDrawer->EnableDrawHiddenLine();
  else
    myDrawer->DisableDrawHiddenLine();
  StdPrs_HLRPolyShape::Add(aPresentation,myShape,myDrawer,aProjector);
}

void User_Cylinder::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                    const Standard_Integer aMode)
{
  switch(aMode)
  {
  case 0:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_SHAPE, 0.01, 0.1);
    break;
  case 4:
    StdSelect_BRepSelectionTool::Load(aSelection,this,myShape,TopAbs_FACE, 0.01, 0.1);
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

Standard_Boolean User_Cylinder::TriangleIsValid(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3) const
{ 
  gp_Vec V1(P1,P2);// V1=(P1,P2)
  gp_Vec V2(P2,P3);// V2=(P2,P3)
  gp_Vec V3(P3,P1);// V3=(P3,P1)

  if ((V1.SquareMagnitude() > 1.e-10) && (V2.SquareMagnitude() > 1.e-10) && (V3.SquareMagnitude() > 1.e-10))
  {
    V1.Cross(V2);// V1 = Normal	
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
  red =1; //initializing colors parameters
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
      //Standard_Real A = 7*Alt-7*AltMin;
      green = (3*AltMax-AltMin)/(3*AltMax-AltMin+(7*Alt-7*AltMin));
      blue = 0 ;

      Quantity_Color color;
      color.SetValues(red,green,blue, Quantity_TOC_RGB);
      return color;
      break;
    }//end case 0

  case 1 : //mer-neige
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
      else if ((b/5)<a && a <= (2*b/5))
      {
        red = .55;
        green = 0.3;
        blue = 0;
      }
      else if ((2*b/5)<a && a <= (18*b/20))
      {
        green =1/(((7/(3*AltMax-AltMin))*Alt)+(1-(7*AltMin/(3*AltMax-AltMin))));
        red = 1/(((1000/(AltMax-AltMin))*Alt)+1000*(1-(AltMin/(AltMax-AltMin))));
        blue = 0;
      }
      else if ((18*b/20)<a && a <= (18.5*b/20))
      {
        red = 0.75;
        green = 0.66;
        blue = 0;
      }

      else if ((18.5*b/20)<a && a <= b)
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

      Standard_Real Distance = BAR.Distance ( P);

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
  myColor = aColor;
}
