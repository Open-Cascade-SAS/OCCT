// File:	QADBMReflex_OCC749PrsUseVertexCABV.cxx
// Created:	Mon Oct  7 15:01:41 2002
// Author:	QA Admin
//		<qa@russox>


#include <QADBMReflex_OCC749PrsUseVertexCABV.ixx>

#include <Prs3d_Root.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Aspect_Array1OfEdge.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_Array1OfVertexC.hxx>
#include <Aspect_Edge.hxx>


#include <Graphic3d_Array1OfVertex.hxx>
#include <OSD_Timer.hxx>


//=======================================================================
//function : QADBMReflex_OCC749PrsUseVertex
//purpose  : 
//=======================================================================

  QADBMReflex_OCC749PrsUseVertexCABV::QADBMReflex_OCC749PrsUseVertexCABV( const Standard_Boolean Reflection,const Quantity_Color& InteriorColor,const Quantity_Color& EdgeColor,const Quantity_Color& EdgeColor2,const Standard_Integer XCount,const Standard_Integer YCount,const Standard_Integer BoxSize,const Graphic3d_MaterialAspect& MaterialAspect,const Standard_Boolean Material,const Standard_Boolean Timer ):QADBMReflex_OCC749Prs(Reflection,InteriorColor,EdgeColor,EdgeColor2,XCount,YCount,BoxSize,MaterialAspect,Material,Timer) {
}

static OSD_Timer Timer;

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void QADBMReflex_OCC749PrsUseVertexCABV::Compute(const Handle(PrsMgr_PresentationManager3d)& aPrsMgr,
					      const Handle(Prs3d_Presentation)& aPresentation,
					      const Standard_Integer aMode )
{
  Standard_Boolean BooleanTimer = GetTimer();
  if (BooleanTimer) {
    Timer.Reset ();
    Timer.Start ();
  }

  aPresentation->Clear();
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup(aPresentation);

  Graphic3d_MaterialAspect aFlatMat; 
  Standard_Boolean BooleanMaterial = GetMaterial();
  if (BooleanMaterial) {
    aFlatMat = GetMaterialAspect(); 
  }

  Standard_Boolean BooleanReflection = GetReflection();
  if (BooleanReflection) {
    aFlatMat.SetReflectionModeOn(Graphic3d_TOR_AMBIENT);
    aFlatMat.SetReflectionModeOn(Graphic3d_TOR_DIFFUSE);
    aFlatMat.SetReflectionModeOn(Graphic3d_TOR_SPECULAR);
    aFlatMat.SetReflectionModeOn(Graphic3d_TOR_EMISSION);
  } else {
    aFlatMat.SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
    aFlatMat.SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
    aFlatMat.SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
    aFlatMat.SetReflectionModeOff(Graphic3d_TOR_EMISSION);
  }

  Quantity_Color InteriorColor = GetInteriorColor();
  Quantity_Color EdgeColor     = GetEdgeColor();
  Quantity_Color EdgeColor2    = GetEdgeColor2();

  Standard_Integer XCOUNT = GetXCount();
  Standard_Integer YCOUNT = GetYCount();
  Standard_Integer BOXSIZE = GetBoxSize();

  Handle(Graphic3d_AspectFillArea3d) Fill3d;
  Fill3d = new Graphic3d_AspectFillArea3d(Aspect_IS_SOLID,
					  InteriorColor,
					  EdgeColor,
					  Aspect_TOL_SOLID,
					  1.5,
					  aFlatMat,
					  aFlatMat);

  //////////////////Fill3d->SetInteriorColor(InteriorColor);

  //Handle(Graphic3d_AspectFillArea3d) Fill3d = new Graphic3d_AspectFillArea3d();
  //Fill3d->SetInteriorStyle(Aspect_IS_SOLID);

  aGroup->SetGroupPrimitivesAspect(Fill3d);
  aGroup->SetPrimitivesAspect(Fill3d);

  aGroup->BeginPrimitives();

  Standard_Integer i,j,z1=0,z2,zmax=XCOUNT;
  if (YCOUNT > XCOUNT) zmax = YCOUNT;

  Standard_Integer HMAX = 100;
  BOXSIZE = HMAX / zmax;
  if (BOXSIZE ==0)
    BOXSIZE = 1;

  for (i=0; i < XCOUNT; i++){
    for (j=0; j < YCOUNT; j++){
      //Quantity_NameOfColor aColor = Quantity_Color::Name(1., 
      //                                                   (double)j/YCOUNT, 
      //                                                   (double)i/XCOUNT);

      Graphic3d_Array1OfVertexC aVertexes(1, 8); 
      
      //////////////////z2 = BOXSIZE +(i*zmax - j*((Standard_Integer) zmax/2));
      z2 = BOXSIZE +(i*BOXSIZE - j*BOXSIZE/2);
      aVertexes(1).SetCoord(i*BOXSIZE, j*BOXSIZE, z1);
      aVertexes(2).SetCoord(i*BOXSIZE + BOXSIZE, j*BOXSIZE, z1);
      aVertexes(3).SetCoord(i*BOXSIZE + BOXSIZE, j*BOXSIZE + BOXSIZE, z1);
      aVertexes(4).SetCoord(i*BOXSIZE, j*BOXSIZE + BOXSIZE, z1);
      aVertexes(5).SetCoord(i*BOXSIZE, j*BOXSIZE, z2);
      aVertexes(6).SetCoord(i*BOXSIZE + BOXSIZE, j*BOXSIZE, z2);
      aVertexes(7).SetCoord(i*BOXSIZE + BOXSIZE, j*BOXSIZE + BOXSIZE, z2);
      aVertexes(8).SetCoord(i*BOXSIZE, j*BOXSIZE + BOXSIZE, z2);

      //aVertexes(1).SetColor(EdgeColor);
      //aVertexes(2).SetColor(EdgeColor);
      //aVertexes(3).SetColor(EdgeColor2);
      //aVertexes(4).SetColor(EdgeColor2);
      //aVertexes(5).SetColor(EdgeColor2);
      //aVertexes(6).SetColor(EdgeColor2);
      //aVertexes(7).SetColor(EdgeColor);
      //aVertexes(8).SetColor(EdgeColor);

      //////////////////if (!(i == 0 && j ==0)) {
	aVertexes(1).SetColor(Quantity_Color(1., (double)j/YCOUNT, (double)i/XCOUNT, Quantity_TOC_RGB));
	aVertexes(2).SetColor(Quantity_Color(1., (double)j/YCOUNT, (double)i/XCOUNT, Quantity_TOC_RGB));
	aVertexes(3).SetColor(Quantity_Color(1., (double)j/YCOUNT, (double)i/XCOUNT, Quantity_TOC_RGB));
	aVertexes(4).SetColor(Quantity_Color(1., (double)j/YCOUNT, (double)i/XCOUNT, Quantity_TOC_RGB));
	aVertexes(5).SetColor(Quantity_Color(1., (double)j/YCOUNT, (double)i/XCOUNT, Quantity_TOC_RGB));
	aVertexes(6).SetColor(Quantity_Color(1., (double)j/YCOUNT, (double)i/XCOUNT, Quantity_TOC_RGB));
	aVertexes(7).SetColor(Quantity_Color(1., (double)j/YCOUNT, (double)i/XCOUNT, Quantity_TOC_RGB));
	aVertexes(8).SetColor(Quantity_Color(1., (double)j/YCOUNT, (double)i/XCOUNT, Quantity_TOC_RGB));
      //////////////////}

      Aspect_Array1OfEdge aEdges(1, 24);
      aEdges(1) = Aspect_Edge(1, 2, Aspect_TOE_VISIBLE);
      aEdges(2) = Aspect_Edge(2, 3, Aspect_TOE_VISIBLE);
      aEdges(3) = Aspect_Edge(3, 4, Aspect_TOE_VISIBLE);
      aEdges(4) = Aspect_Edge(4, 1, Aspect_TOE_VISIBLE);

      aEdges(5) = Aspect_Edge(5, 6, Aspect_TOE_VISIBLE);
      aEdges(6) = Aspect_Edge(6, 7, Aspect_TOE_VISIBLE);
      aEdges(7) = Aspect_Edge(7, 8, Aspect_TOE_VISIBLE);
      aEdges(8) = Aspect_Edge(8, 5, Aspect_TOE_VISIBLE);

      aEdges(9) = Aspect_Edge(1, 5, Aspect_TOE_VISIBLE);
      aEdges(10) = Aspect_Edge(5, 6, Aspect_TOE_VISIBLE);
      aEdges(11) = Aspect_Edge(6, 2, Aspect_TOE_VISIBLE);
      aEdges(12) = Aspect_Edge(2, 1, Aspect_TOE_VISIBLE);

      aEdges(13) = Aspect_Edge(6, 7, Aspect_TOE_VISIBLE);
      aEdges(14) = Aspect_Edge(7, 3, Aspect_TOE_VISIBLE);
      aEdges(15) = Aspect_Edge(3, 2, Aspect_TOE_VISIBLE);
      aEdges(16) = Aspect_Edge(2, 6, Aspect_TOE_VISIBLE);

      aEdges(17) = Aspect_Edge(3, 7, Aspect_TOE_VISIBLE);
      aEdges(18) = Aspect_Edge(7, 8, Aspect_TOE_VISIBLE);
      aEdges(19) = Aspect_Edge(8, 4, Aspect_TOE_VISIBLE);
      aEdges(20) = Aspect_Edge(4, 3, Aspect_TOE_VISIBLE);

      aEdges(21) = Aspect_Edge(4, 8, Aspect_TOE_VISIBLE);
      aEdges(22) = Aspect_Edge(8, 5, Aspect_TOE_VISIBLE);
      aEdges(23) = Aspect_Edge(5, 1, Aspect_TOE_VISIBLE);
      aEdges(24) = Aspect_Edge(1, 4, Aspect_TOE_VISIBLE);

      aGroup->QuadrangleSet(aVertexes, aEdges);
    }
  }
  aGroup->EndPrimitives();

  if (BooleanTimer) {
    Timer.Stop ();
    Timer.Show (cout);
  }

}
