// File:	QAOCC_OCC749Prs.cxx
// Created:	Fri Sep 20 16:33:40 2002
// Author:	Michael KUZMITCHEV
//		<mkv@russox>


#include <QAOCC_OCC749Prs.ixx>

#include <Prs3d_Root.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Aspect_Array1OfEdge.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_Array1OfVertexC.hxx>
#include <Aspect_Edge.hxx>

QAOCC_OCC749Prs::QAOCC_OCC749Prs( const Standard_Boolean reflection, 
		    const Quantity_Color& color,
		    const Quantity_Color& color1 ) : 
myReflection( reflection ), myColor1( color ), myColor2( color1 )
{
}

void QAOCC_OCC749Prs::Compute(const Handle(PrsMgr_PresentationManager3d)& aPrsMgr,
		       const Handle(Prs3d_Presentation)& aPrs,
		       const Standard_Integer aMode )
{
  aPrs->Clear();
  Handle(Graphic3d_Group) group = Prs3d_Root::NewGroup( aPrs );

  Handle_Graphic3d_AspectFillArea3d CTX=new Graphic3d_AspectFillArea3d();
  CTX->SetInteriorStyle(Aspect_IS_SOLID);
  group->SetGroupPrimitivesAspect(CTX);

  Graphic3d_MaterialAspect material = CTX->FrontMaterial();
  if ( !myReflection ) {
    material.SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
    material.SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
    material.SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
    material.SetReflectionModeOff(Graphic3d_TOR_EMISSION);
  }
  CTX->SetFrontMaterial(material);
  group->SetPrimitivesAspect(CTX);

  Graphic3d_Array1OfVertexC anArray( 1,4 );
  anArray(1).SetCoord( 0.,0.,0 );
  anArray(1).SetColor( myColor1 );
  anArray(2).SetCoord( 50., 0., 0 );
  anArray(2).SetColor( myColor1 );
  anArray(3).SetCoord( 50., 50, 0 );
  anArray(3).SetColor( myColor2 );
  anArray(4).SetCoord( 0., 50., 0 );
  anArray(4).SetColor( myColor2 );
  Aspect_Edge aE1(1,2,Aspect_TOE_VISIBLE);
  Aspect_Edge aE2(2,3,Aspect_TOE_VISIBLE);
  Aspect_Edge aE3(3,4,Aspect_TOE_VISIBLE);
  Aspect_Edge aE4(4,1,Aspect_TOE_VISIBLE);
  Aspect_Array1OfEdge anEdges(1,4);
  anEdges.SetValue(1,aE1);
  anEdges.SetValue(2,aE2);
  anEdges.SetValue(3,aE3);
  anEdges.SetValue(4,aE4);
  group->QuadrangleSet(anArray,anEdges);
}

void QAOCC_OCC749Prs::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
				const Standard_Integer aMode)
{
}

void QAOCC_OCC749Prs::SetReflection( const Standard_Boolean reflection )
{
  myReflection = reflection;
}
    
void QAOCC_OCC749Prs::SetColor( const Quantity_Color& color,
			 const Quantity_Color& color1 )
{
  myColor1 = color;
  myColor2 = color1;
}
