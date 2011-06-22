#define G005

#include <Sphere_Sphere.ixx>

#include <BRepPrimAPI_MakeSphere.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Drawer.hxx>

#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_LineAspect.hxx>

#include <StdPrs_WFDeflectionShape.hxx>

#include <Precision.hxx>

#include <Aspect_Array1OfEdge.hxx>

#include <Graphic3d_Structure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Array1OfVertexN.hxx>
#include <Graphic3d_Array1OfVertexC.hxx>
#include <Graphic3d_Array1OfVertexNC.hxx>
#include <Graphic3d_Array1OfVertexNT.hxx>
#ifdef G005
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_ArrayOfQuadrangles.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#endif

#include <Graphic3d_AspectText3d.hxx>

Standard_CString stText="Test";
Standard_Integer stTextHeight=10;

static void ShadeWithVertex(const Handle(Graphic3d_Group) aGroup,
			    const gp_Pnt aPosition, 
			    const Standard_Real aRadius,
			    const Standard_Real aDeflection,
				const Standard_Boolean isText,
				const Standard_Boolean isOptText) 
{
      Standard_Real R1,R2;
      Standard_Real X1,Y1,Z1;
      Standard_Real X2,Y2,Z2;
      Standard_Real X3,Y3,Z3;
      Standard_Real X4,Y4,Z4;
      Standard_Real Xc,Yc,Zc;
      Standard_Real Beta = 0.;
      Standard_Real Alpha = 0.;
      Standard_Integer nbpanes = Sphere_Sphere::NbPanes(aRadius,aDeflection);
      Standard_Real Dbeta = 2.*Standard_PI/nbpanes;
      Standard_Real Dalpha = 2.*Standard_PI/nbpanes;
      Standard_Integer i,j;

      aPosition.Coord (Xc, Yc, Zc);
  	  if(isText) {
		Handle(Graphic3d_AspectText3d) Text3d = new Graphic3d_AspectText3d();
	 	Text3d->SetTextureMappedFont(isOptText);
		aGroup->SetPrimitivesAspect(Text3d);
	  }

#ifdef G005
      if( Graphic3d_ArrayOfPrimitives::IsEnable() ) {
        Handle(Graphic3d_ArrayOfTriangles) ptri =
                new Graphic3d_ArrayOfTriangles(3*nbpanes*2,0,
                Standard_False,Standard_False,Standard_False,Standard_False);
        Handle(Graphic3d_ArrayOfQuadrangles) pquad =
                new Graphic3d_ArrayOfQuadrangles(4*nbpanes*(nbpanes/2-2),0,
                Standard_False,Standard_False,Standard_False,Standard_False);
        aGroup->BeginPrimitives();
        for( j=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
            if( j == 0 ) {
              ptri->AddVertex(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              ptri->AddVertex(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              ptri->AddVertex(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
            } else if( j == nbpanes/2-1 ) {
              ptri->AddVertex(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              ptri->AddVertex(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              ptri->AddVertex(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            } else {
              pquad->AddVertex(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              pquad->AddVertex(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              pquad->AddVertex(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
              pquad->AddVertex(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->AddPrimitiveArray(ptri);
        aGroup->AddPrimitiveArray(pquad);
        aGroup->EndPrimitives();
      } else
#endif
      {
        aGroup->BeginPrimitives();
        Graphic3d_Array1OfVertex TriVertex(1,3*nbpanes*2);
        Graphic3d_Array1OfVertex QuadVertex(1,4*nbpanes*(nbpanes/2-2));
        Aspect_Array1OfEdge TriEdge(1,3*nbpanes*2);
        Aspect_Array1OfEdge QuadEdge(1,4*nbpanes*(nbpanes/2-2));
        Standard_Integer nt,nq;
        for( j=nt=nq=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
            if( j == 0 ) {
              nt++;
              TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
              TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else if( j == nbpanes/2-1 ) {
              nt++;
              TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
              TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else {
              nq++;
              QuadVertex(nq).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
              QuadEdge(nq).SetValues(nq,nq-3,Aspect_TOE_VISIBLE);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->TriangleSet(TriVertex,TriEdge);
        aGroup->QuadrangleSet(QuadVertex,QuadEdge);
        aGroup->EndPrimitives();
      }
}

static void ShadeWithVertexN(const Handle(Graphic3d_Group) aGroup,
			    const gp_Pnt aPosition, 
			    const Standard_Real aRadius,
			    const Standard_Real aDeflection,
				const Standard_Boolean isText,
				const Standard_Boolean isOptText) 
{
      Standard_Real R1,R2;
      Standard_Real X1,Y1,Z1;
      Standard_Real X2,Y2,Z2;
      Standard_Real X3,Y3,Z3;
      Standard_Real X4,Y4,Z4;
      Standard_Real Xc,Yc,Zc;
      Standard_Real Beta = 0.;
      Standard_Real Alpha = 0.;
      Standard_Integer nbpanes = Sphere_Sphere::NbPanes(aRadius,aDeflection);
      Standard_Real Dbeta = 2.*Standard_PI/nbpanes;
      Standard_Real Dalpha = 2.*Standard_PI/nbpanes;
      Standard_Integer i,j;

      aPosition.Coord (Xc, Yc, Zc);
	  if(isText) {
		Handle(Graphic3d_AspectText3d) Text3d = new Graphic3d_AspectText3d();
 		Text3d->SetTextureMappedFont(isOptText);
		aGroup->SetPrimitivesAspect(Text3d);
	  }
#ifdef G005
      if( Graphic3d_ArrayOfPrimitives::IsEnable() ) {
        Handle(Graphic3d_ArrayOfTriangles) ptri =
                new Graphic3d_ArrayOfTriangles(3*nbpanes*2,0,
                Standard_True,Standard_False,Standard_False,Standard_False);
        Handle(Graphic3d_ArrayOfQuadrangles) pquad =
                new Graphic3d_ArrayOfQuadrangles(4*nbpanes*(nbpanes/2-2),0,
                Standard_True,Standard_False,Standard_False,Standard_False);
        aGroup->BeginPrimitives();
        for( j=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
            if( j == 0 ) {
				ptri->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
				gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
				ptri->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
				gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
				ptri->AddVertex(gp_Pnt(X3+Xc,Y3+Yc,Z3+Zc),
				gp_Vec(X3/aRadius,Y3/aRadius,Z3/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
            } else if( j == nbpanes/2-1 ) {
				ptri->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
				gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
				ptri->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
				gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
				ptri->AddVertex(gp_Pnt(X4+Xc,Y4+Yc,Z4+Zc),
				gp_Vec(X4/aRadius,Y4/aRadius,Z4/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            } else {
				pquad->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
				gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
				pquad->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
				gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
				pquad->AddVertex(gp_Pnt(X3+Xc,Y3+Yc,Z3+Zc),
				gp_Vec(X3/aRadius,Y3/aRadius,Z3/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
				pquad->AddVertex(gp_Pnt(X4+Xc,Y4+Yc,Z4+Zc),
				gp_Vec(X4/aRadius,Y4/aRadius,Z4/aRadius));
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->AddPrimitiveArray(ptri);
        aGroup->AddPrimitiveArray(pquad);
        aGroup->EndPrimitives();
      } else
#endif
      {
        aGroup->BeginPrimitives();
        Graphic3d_Array1OfVertexN TriVertex(1,3*nbpanes*2);
        Graphic3d_Array1OfVertexN QuadVertex(1,4*nbpanes*(nbpanes/2-2));
        Aspect_Array1OfEdge TriEdge(1,3*nbpanes*2);
        Aspect_Array1OfEdge QuadEdge(1,4*nbpanes*(nbpanes/2-2));
        Standard_Integer nt,nq;
        for( j=nt=nq=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
            if( j == 0 ) {
				nt++;
				TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
				TriVertex(nt).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
				TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
				nt++;
				TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
				TriVertex(nt).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
				TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
				nt++;
				TriVertex(nt).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
				TriVertex(nt).SetNormal(X3/aRadius,Y3/aRadius,Z3/aRadius);
				TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else if( j == nbpanes/2-1 ) {
				nt++;
				TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
				TriVertex(nt).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
				TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
				nt++;
				TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
				TriVertex(nt).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
				TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
				nt++;
				TriVertex(nt).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
				TriVertex(nt).SetNormal(X4/aRadius,Y4/aRadius,Z4/aRadius);
				TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else {
				nq++;
				QuadVertex(nq).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
				QuadVertex(nq).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
				QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
				nq++;
				QuadVertex(nq).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
				QuadVertex(nq).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
				QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
				nq++;
				QuadVertex(nq).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
				QuadVertex(nq).SetNormal(X3/aRadius,Y3/aRadius,Z3/aRadius);
				QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
				nq++;
				QuadVertex(nq).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
				if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
				QuadVertex(nq).SetNormal(X4/aRadius,Y4/aRadius,Z4/aRadius);
				QuadEdge(nq).SetValues(nq,nq-3,Aspect_TOE_VISIBLE);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->TriangleSet(TriVertex,TriEdge);
        aGroup->QuadrangleSet(QuadVertex,QuadEdge);
        aGroup->EndPrimitives();
      }
}

static void ShadeWithVertexC(const Handle(Graphic3d_Group) aGroup,
			    const gp_Pnt aPosition, 
			    const Standard_Real aRadius,
			    const Standard_Real aDeflection,
				const Standard_Boolean isText,
				const Standard_Boolean isOptText) 
{
      Standard_Real R1,R2;
      Standard_Real X1,Y1,Z1;
      Standard_Real X2,Y2,Z2;
      Standard_Real X3,Y3,Z3;
      Standard_Real X4,Y4,Z4;
      Standard_Real Xc,Yc,Zc;
      Standard_Real Beta = 0.;
      Standard_Real Alpha = 0.;
      Standard_Integer nbpanes = Sphere_Sphere::NbPanes(aRadius,aDeflection);
      Standard_Real Dbeta = 2.*Standard_PI/nbpanes;
      Standard_Real Dalpha = 2.*Standard_PI/nbpanes;
      Quantity_Color C1,C2,C3,C4;
      Standard_Integer i,j;

      aPosition.Coord (Xc, Yc, Zc);
	  if(isText) {
		Handle(Graphic3d_AspectText3d) Text3d = new Graphic3d_AspectText3d();
 		Text3d->SetTextureMappedFont(isOptText);
		aGroup->SetPrimitivesAspect(Text3d);
	  }
#ifdef G005
      if( Graphic3d_ArrayOfPrimitives::IsEnable() ) {
        Handle(Graphic3d_ArrayOfTriangles) ptri =
                new Graphic3d_ArrayOfTriangles(3*nbpanes*2,0,
                Standard_False,Standard_True,Standard_False,Standard_False);
        Handle(Graphic3d_ArrayOfQuadrangles) pquad =
                new Graphic3d_ArrayOfQuadrangles(4*nbpanes*(nbpanes/2-2),0,
                Standard_False,Standard_True,Standard_False,Standard_False);
        aGroup->BeginPrimitives();
        for( j=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
	    C1.SetValues(0.5+X1/aRadius/2.,
		0.5+Y1/aRadius/2.,0.5+Z1/aRadius/2.,Quantity_TOC_RGB);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
	    C2.SetValues(0.5+X2/aRadius/2.,
		0.5+Y2/aRadius/2.,0.5+Z2/aRadius/2.,Quantity_TOC_RGB);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
	    C3.SetValues(0.5+X3/aRadius/2.,
		0.5+Y3/aRadius/2.,0.5+Z3/aRadius/2.,Quantity_TOC_RGB);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
	    C4.SetValues(0.5+X4/aRadius/2.,
		0.5+Y4/aRadius/2.,0.5+Z4/aRadius/2.,Quantity_TOC_RGB);
            if( j == 0 ) {
              ptri->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),C1);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),C2);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X3+Xc,Y3+Yc,Z3+Zc),C3);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
            } else if( j == nbpanes/2-1 ) {
              ptri->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),C1);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),C2);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X4+Xc,Y4+Yc,Z4+Zc),C4);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            } else {
              pquad->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),C1);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),C2);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X3+Xc,Y3+Yc,Z3+Zc),C3);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X4+Xc,Y4+Yc,Z4+Zc),C4);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->AddPrimitiveArray(ptri);
        aGroup->AddPrimitiveArray(pquad);
        aGroup->EndPrimitives();
      } else
#endif
      {
        aGroup->BeginPrimitives();
        Graphic3d_Array1OfVertexC TriVertex(1,3*nbpanes*2);
        Graphic3d_Array1OfVertexC QuadVertex(1,4*nbpanes*(nbpanes/2-2));
        Aspect_Array1OfEdge TriEdge(1,3*nbpanes*2);
        Aspect_Array1OfEdge QuadEdge(1,4*nbpanes*(nbpanes/2-2));
        Standard_Integer nt,nq;
        for( j=nt=nq=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
	    C1.SetValues(0.5+X1/aRadius/2.,
		0.5+Y1/aRadius/2.,0.5+Z1/aRadius/2.,Quantity_TOC_RGB);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
	    C2.SetValues(0.5+X2/aRadius/2.,
		0.5+Y2/aRadius/2.,0.5+Z2/aRadius/2.,Quantity_TOC_RGB);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
	    C3.SetValues(0.5+X3/aRadius/2.,
		0.5+Y3/aRadius/2.,0.5+Z3/aRadius/2.,Quantity_TOC_RGB);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
	    C4.SetValues(0.5+X4/aRadius/2.,
		0.5+Y4/aRadius/2.,0.5+Z4/aRadius/2.,Quantity_TOC_RGB);
            if( j == 0 ) {
              nt++;
              TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      TriVertex(nt).SetColor(C1);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      TriVertex(nt).SetColor(C2);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
	      TriVertex(nt).SetColor(C3);
              TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else if( j == nbpanes/2-1 ) {
              nt++;
              TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      TriVertex(nt).SetColor(C1);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      TriVertex(nt).SetColor(C2);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
	      TriVertex(nt).SetColor(C4);
              TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else {
              nq++;
              QuadVertex(nq).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      QuadVertex(nq).SetColor(C1);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      QuadVertex(nq).SetColor(C2);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
	      QuadVertex(nq).SetColor(C3);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
	      QuadVertex(nq).SetColor(C4);
              QuadEdge(nq).SetValues(nq,nq-3,Aspect_TOE_VISIBLE);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->TriangleSet(TriVertex,TriEdge);
        aGroup->QuadrangleSet(QuadVertex,QuadEdge);
        aGroup->EndPrimitives();
      }
}

static void ShadeWithVertexNC(const Handle(Graphic3d_Group) aGroup,
			    const gp_Pnt aPosition, 
			    const Standard_Real aRadius,
			    const Standard_Real aDeflection,
				const Standard_Boolean isText,
				const Standard_Boolean isOptText) 
{
      Standard_Real R1,R2;
      Standard_Real X1,Y1,Z1;
      Standard_Real X2,Y2,Z2;
      Standard_Real X3,Y3,Z3;
      Standard_Real X4,Y4,Z4;
      Standard_Real Xc,Yc,Zc;
      Standard_Real Beta = 0.;
      Standard_Real Alpha = 0.;
      Standard_Integer nbpanes = Sphere_Sphere::NbPanes(aRadius,aDeflection);
      Standard_Real Dbeta = 2.*Standard_PI/nbpanes;
      Standard_Real Dalpha = 2.*Standard_PI/nbpanes;
      Quantity_Color C1,C2,C3,C4;
      Standard_Integer i,j;

      aPosition.Coord (Xc, Yc, Zc);
	  if(isText) {
		Handle(Graphic3d_AspectText3d) Text3d = new Graphic3d_AspectText3d();
 		Text3d->SetTextureMappedFont(isOptText);
		aGroup->SetPrimitivesAspect(Text3d);
	  }
#ifdef G005
      if( Graphic3d_ArrayOfPrimitives::IsEnable() ) {
        Handle(Graphic3d_ArrayOfTriangles) ptri =
                new Graphic3d_ArrayOfTriangles(3*nbpanes*2,0,
                Standard_True,Standard_True,Standard_False,Standard_False);
        Handle(Graphic3d_ArrayOfQuadrangles) pquad =
                new Graphic3d_ArrayOfQuadrangles(4*nbpanes*(nbpanes/2-2),0,
                Standard_True,Standard_True,Standard_False,Standard_False);
        aGroup->BeginPrimitives();
        for( j=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
	    C1.SetValues(0.5+X1/aRadius/2.,
		0.5+Y1/aRadius/2.,0.5+Z1/aRadius/2.,Quantity_TOC_RGB);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
	    C2.SetValues(0.5+X2/aRadius/2.,
		0.5+Y2/aRadius/2.,0.5+Z2/aRadius/2.,Quantity_TOC_RGB);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
	    C3.SetValues(0.5+X3/aRadius/2.,
		0.5+Y3/aRadius/2.,0.5+Z3/aRadius/2.,Quantity_TOC_RGB);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
	    C4.SetValues(0.5+X4/aRadius/2.,
		0.5+Y4/aRadius/2.,0.5+Z4/aRadius/2.,Quantity_TOC_RGB);
            if( j == 0 ) {
              ptri->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
			gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius),C1);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
			gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius),C2);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X3+Xc,Y3+Yc,Z3+Zc),
			gp_Vec(X3/aRadius,Y3/aRadius,Z3/aRadius),C3);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
            } else if( j == nbpanes/2-1 ) {
              ptri->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
			gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius),C1);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
			gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius),C2);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X4+Xc,Y4+Yc,Z4+Zc),
			gp_Vec(X4/aRadius,Y4/aRadius,Z4/aRadius),C4);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            } else {
              pquad->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
			gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius),C1);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
			gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius),C2);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X3+Xc,Y3+Yc,Z3+Zc),
			gp_Vec(X3/aRadius,Y3/aRadius,Z3/aRadius),C3);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X4+Xc,Y4+Yc,Z4+Zc),
			gp_Vec(X4/aRadius,Y4/aRadius,Z4/aRadius),C4);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->AddPrimitiveArray(ptri);
        aGroup->AddPrimitiveArray(pquad);
        aGroup->EndPrimitives();
      } else
#endif
      {
        aGroup->BeginPrimitives();
        Graphic3d_Array1OfVertexNC TriVertex(1,3*nbpanes*2);
        Graphic3d_Array1OfVertexNC QuadVertex(1,4*nbpanes*(nbpanes/2-2));
        Aspect_Array1OfEdge TriEdge(1,3*nbpanes*2);
        Aspect_Array1OfEdge QuadEdge(1,4*nbpanes*(nbpanes/2-2));
        Standard_Integer nt,nq;
        for( j=nt=nq=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
	    C1.SetValues(0.5+X1/aRadius/2.,
		0.5+Y1/aRadius/2.,0.5+Z1/aRadius/2.,Quantity_TOC_RGB);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
	    C2.SetValues(0.5+X2/aRadius/2.,
		0.5+Y2/aRadius/2.,0.5+Z2/aRadius/2.,Quantity_TOC_RGB);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
	    C3.SetValues(0.5+X3/aRadius/2.,
		0.5+Y3/aRadius/2.,0.5+Z3/aRadius/2.,Quantity_TOC_RGB);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
	    C4.SetValues(0.5+X4/aRadius/2.,
		0.5+Y4/aRadius/2.,0.5+Z4/aRadius/2.,Quantity_TOC_RGB);
            if( j == 0 ) {
              nt++;
              TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
	      TriVertex(nt).SetColor(C1);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
	      TriVertex(nt).SetColor(C2);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X3/aRadius,Y3/aRadius,Z3/aRadius);
	      TriVertex(nt).SetColor(C3);
              TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else if( j == nbpanes/2-1 ) {
              nt++;
              TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
	      TriVertex(nt).SetColor(C1);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
	      TriVertex(nt).SetColor(C2);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X4/aRadius,Y4/aRadius,Z4/aRadius);
	      TriVertex(nt).SetColor(C4);
              TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else {
              nq++;
              QuadVertex(nq).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      QuadVertex(nq).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
	      QuadVertex(nq).SetColor(C1);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      QuadVertex(nq).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
	      QuadVertex(nq).SetColor(C2);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
	      QuadVertex(nq).SetNormal(X3/aRadius,Y3/aRadius,Z3/aRadius);
	      QuadVertex(nq).SetColor(C3);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
	      QuadVertex(nq).SetNormal(X4/aRadius,Y4/aRadius,Z4/aRadius);
	      QuadVertex(nq).SetColor(C4);
              QuadEdge(nq).SetValues(nq,nq-3,Aspect_TOE_VISIBLE);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->TriangleSet(TriVertex,TriEdge);
        aGroup->QuadrangleSet(QuadVertex,QuadEdge);
        aGroup->EndPrimitives();
      }
}

static void ShadeWithVertexNT(const Handle(Graphic3d_Group) aGroup,
			    const gp_Pnt aPosition, 
			    const Standard_Real aRadius,
			    const Standard_Real aDeflection,
				const Standard_Boolean isText,
				const Standard_Boolean isOptText) 
{
      Standard_Real R1,R2;
      Standard_Real X1,Y1,Z1;
      Standard_Real X2,Y2,Z2;
      Standard_Real X3,Y3,Z3;
      Standard_Real X4,Y4,Z4;
      Standard_Real Xc,Yc,Zc;
      Standard_Real Beta = 0.;
      Standard_Real Alpha = 0.;
      Standard_Integer nbpanes = Sphere_Sphere::NbPanes(aRadius,aDeflection);
      Standard_Real Dbeta = 2.*Standard_PI/nbpanes;
      Standard_Real Dalpha = 2.*Standard_PI/nbpanes;
      Standard_Real Tx1,Ty1,Tx2,Ty2;
      Standard_Integer i,j;

      aPosition.Coord (Xc, Yc, Zc);
	  if(isText) {
		Handle(Graphic3d_AspectText3d) Text3d = new Graphic3d_AspectText3d();
 		Text3d->SetTextureMappedFont(isOptText);
		aGroup->SetPrimitivesAspect(Text3d);
	  }
#ifdef G005
      if( Graphic3d_ArrayOfPrimitives::IsEnable() ) {
        Handle(Graphic3d_ArrayOfTriangles) ptri =
                new Graphic3d_ArrayOfTriangles(3*nbpanes*2,0,
                Standard_True,Standard_False,Standard_True,Standard_False);
        Handle(Graphic3d_ArrayOfQuadrangles) pquad =
                new Graphic3d_ArrayOfQuadrangles(4*nbpanes*(nbpanes/2-2),0,
                Standard_True,Standard_False,Standard_True,Standard_False);
        aGroup->BeginPrimitives();
        for( j=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          Ty1 = Standard_Real(j)/nbpanes/2.;
          Ty2 = Standard_Real(j+1)/nbpanes/2.;
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
            Tx1 = Standard_Real(i)/nbpanes;
            Tx2 = Standard_Real(i+1)/nbpanes;
            if( j == 0 ) {
              ptri->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
			gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius),
			gp_Pnt2d(Tx1,Ty1));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
			gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius),
			gp_Pnt2d(Tx1,Ty2));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X3+Xc,Y3+Yc,Z3+Zc),
			gp_Vec(X3/aRadius,Y3/aRadius,Z3/aRadius),
			gp_Pnt2d(Tx2,Ty2));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
            } else if( j == nbpanes/2-1 ) {
              ptri->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
			gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius),
			gp_Pnt2d(Tx1,Ty1));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
			gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius),
			gp_Pnt2d(Tx1,Ty2));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              ptri->AddVertex(gp_Pnt(X4+Xc,Y4+Yc,Z4+Zc),
			gp_Vec(X4/aRadius,Y4/aRadius,Z4/aRadius),
			gp_Pnt2d(Tx2,Ty1));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            } else {
              pquad->AddVertex(gp_Pnt(X1+Xc,Y1+Yc,Z1+Zc),
			gp_Vec(X1/aRadius,Y1/aRadius,Z1/aRadius),
			gp_Pnt2d(Tx1,Ty1));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X2+Xc,Y2+Yc,Z2+Zc),
			gp_Vec(X2/aRadius,Y2/aRadius,Z2/aRadius),
			gp_Pnt2d(Tx1,Ty2));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X3+Xc,Y3+Yc,Z3+Zc),
			gp_Vec(X3/aRadius,Y3/aRadius,Z3/aRadius),
			gp_Pnt2d(Tx2,Ty2));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
              pquad->AddVertex(gp_Pnt(X4+Xc,Y4+Yc,Z4+Zc),
			gp_Vec(X4/aRadius,Y4/aRadius,Z4/aRadius),
			gp_Pnt2d(Tx2,Ty1));
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->AddPrimitiveArray(ptri);
        aGroup->AddPrimitiveArray(pquad);
        aGroup->EndPrimitives();
      } else
#endif
      {
        aGroup->BeginPrimitives();
        Graphic3d_Array1OfVertexNT TriVertex(1,3*nbpanes*2);
        Graphic3d_Array1OfVertexNT QuadVertex(1,4*nbpanes*(nbpanes/2-2));
        Aspect_Array1OfEdge TriEdge(1,3*nbpanes*2);
        Aspect_Array1OfEdge QuadEdge(1,4*nbpanes*(nbpanes/2-2));
        Standard_Integer nt,nq;
        for( j=nt=nq=0; j<nbpanes/2; j++ ) {
          Alpha = 0.;
          R1 = aRadius*sin(Beta);
          Z1 = Z4 = aRadius*cos(Beta);
          Beta += Dbeta;
          R2 = aRadius*sin(Beta);
          Z2 = Z3 = aRadius*cos(Beta);
          Ty1 = Standard_Real(j)/nbpanes/2.;
          Ty2 = Standard_Real(j+1)/nbpanes/2.;
          for( i=0; i<nbpanes; i++ ) {
            X1 = R1*cos(Alpha);
            Y1 = R1*sin(Alpha);
            X2 = R2*cos(Alpha);
            Y2 = R2*sin(Alpha);
            Alpha += Dalpha;
            X3 = R2*cos(Alpha);
            Y3 = R2*sin(Alpha);
            X4 = R1*cos(Alpha);
            Y4 = R1*sin(Alpha);
            Tx1 = Standard_Real(i)/nbpanes;
            Tx2 = Standard_Real(i+1)/nbpanes;
            if( j == 0 ) {
              nt++;
              TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
	      TriVertex(nt).SetTextureCoordinate(Tx1,Ty1);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
	      TriVertex(nt).SetTextureCoordinate(Tx1,Ty2);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X3/aRadius,Y3/aRadius,Z3/aRadius);
	      TriVertex(nt).SetTextureCoordinate(Tx2,Ty2);
              TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else if( j == nbpanes/2-1 ) {
              nt++;
              TriVertex(nt).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
	      TriVertex(nt).SetTextureCoordinate(Tx1,Ty1);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
	      TriVertex(nt).SetTextureCoordinate(Tx1,Ty2);
              TriEdge(nt).SetValues(nt,nt+1,Aspect_TOE_VISIBLE);
              nt++;
              TriVertex(nt).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
	      TriVertex(nt).SetNormal(X4/aRadius,Y4/aRadius,Z4/aRadius);
	      TriVertex(nt).SetTextureCoordinate(Tx2,Ty1);
              TriEdge(nt).SetValues(nt,nt-2,Aspect_TOE_VISIBLE);
            } else {
              nq++;
              QuadVertex(nq).SetCoord(X1+Xc,Y1+Yc,Z1+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X1+Xc,Y1+Yc,Z1+Zc), stTextHeight);
	      QuadVertex(nq).SetNormal(X1/aRadius,Y1/aRadius,Z1/aRadius);
	      QuadVertex(nq).SetTextureCoordinate(Tx1,Ty1);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X2+Xc,Y2+Yc,Z2+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X2+Xc,Y2+Yc,Z2+Zc), stTextHeight);
	      QuadVertex(nq).SetNormal(X2/aRadius,Y2/aRadius,Z2/aRadius);
	      QuadVertex(nq).SetTextureCoordinate(Tx1,Ty2);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X3+Xc,Y3+Yc,Z3+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X3+Xc,Y3+Yc,Z3+Zc), stTextHeight);
	      QuadVertex(nq).SetNormal(X3/aRadius,Y3/aRadius,Z3/aRadius);
	      QuadVertex(nq).SetTextureCoordinate(Tx2,Ty2);
              QuadEdge(nq).SetValues(nq,nq+1,Aspect_TOE_VISIBLE);
              nq++;
              QuadVertex(nq).SetCoord(X4+Xc,Y4+Yc,Z4+Zc);
			  if(isText) aGroup->Text(stText,Graphic3d_Vertex(X4+Xc,Y4+Yc,Z4+Zc), stTextHeight);
	      QuadVertex(nq).SetNormal(X4/aRadius,Y4/aRadius,Z4/aRadius);
	      QuadVertex(nq).SetTextureCoordinate(Tx2,Ty1);
              QuadEdge(nq).SetValues(nq,nq-3,Aspect_TOE_VISIBLE);
            }
          }
        }
//        aGroup->BeginPrimitives();
        aGroup->TriangleSet(TriVertex,TriEdge);
        aGroup->QuadrangleSet(QuadVertex,QuadEdge);
        aGroup->EndPrimitives();
      }
}

//==================================================
// Function: 
// Purpose :
//==================================================

Sphere_Sphere::Sphere_Sphere(const gp_Pnt& aPosition,
			 const Standard_Real aRadius,
			 const Standard_Real aDeflection,
			 const Standard_Boolean hasVNormals,
			 const Standard_Boolean hasVColors,
			 const Standard_Boolean hasVTexels,
			 const Standard_Boolean hasText,
			 const Standard_Boolean isOptText
) :
Sphere_BasicShape(BRepPrimAPI_MakeSphere(aRadius).Shape(),
aDeflection,hasVNormals,hasVColors,hasVTexels),
myRadius(aRadius),
myText(hasText),
myOptText(isOptText)
{
  gp_Trsf trsf;
  trsf.SetTranslation(gp_Vec(aPosition.X(),aPosition.Y(),aPosition.Z()));
  TopLoc_Location location(trsf);
  myshape.Location(location);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void Sphere_Sphere::Compute(
			const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
			const Handle(Prs3d_Presentation)& aPrs,
			const Standard_Integer aMode)
{  
  aPrs->Clear();
  if(myshape.IsNull()) return;

  // wire,edge,vertex -> pas de HLR + priorite display superieure
  Standard_Integer TheType = (Standard_Integer) myshape.ShapeType();
  if(TheType>4 && TheType<8) {
    aPrs->SetVisual(Graphic3d_TOS_ALL);
    aPrs->SetDisplayPriority(TheType+2);
  }
  
  switch (aMode) {
  case 0:{
    try { StdPrs_WFDeflectionShape::Add(aPrs,myshape,myDrawer); }
    catch (Standard_Failure) { 
#ifdef DEB
      cout << "Sphere_Sphere::Compute()  failed"<< endl;
#endif
      cout << "a Shape should be incorrect : No Compute can be maked on it  "<< endl;     
    }
    break;
  }
  case 1:
    { 
      Handle(Graphic3d_Group) group = Prs3d_Root::CurrentGroup(aPrs);
      Handle(Graphic3d_AspectFillArea3d) aspect = myDrawer->ShadingAspect()->Aspect();
      aspect->SuppressBackFace();
      group->SetPrimitivesAspect(aspect);
      gp_Pnt position = Position();
      Standard_Integer shadeAspect(0);
      if( myVNormalsFlag ) shadeAspect |= 1;
      if( myVColorsFlag ) shadeAspect |= 2;
      if( myVTexelsFlag ) shadeAspect |= 4;
      switch( shadeAspect ) {
	default:
        case 0:
          ShadeWithVertex(group,position,myRadius,myDeflection,myText, myOptText);
	  break;
	case 1:
          ShadeWithVertexN(group,position,myRadius,myDeflection,myText, myOptText);
	  break;
	case 2:
	  {
            Graphic3d_MaterialAspect material = aspect->FrontMaterial();
            material.SetReflectionModeOff(Graphic3d_TOR_AMBIENT);
            material.SetReflectionModeOff(Graphic3d_TOR_DIFFUSE);
            material.SetReflectionModeOff(Graphic3d_TOR_SPECULAR);
            material.SetReflectionModeOff(Graphic3d_TOR_EMISSION);
            aspect->SetFrontMaterial(material);
            group->SetPrimitivesAspect(aspect);
	  }
          ShadeWithVertexC(group,position,myRadius,myDeflection,myText, myOptText);
	  break;
	case 3:
          ShadeWithVertexNC(group,position,myRadius,myDeflection,myText, myOptText);
	  break;
	case 4:
//          ShadeWithVertexT(group,position,myRadius,myDeflection,myText, myOptText);
	  break;
	case 5:
          ShadeWithVertexNT(group,position,myRadius,myDeflection,myText, myOptText);
	  break;
	case 6:
//          ShadeWithVertexCT(group,position,myRadius,myDeflection,myText, myOptText);
	  break;
	case 7:
//          ShadeWithVertexNCT(group,position,myRadius,myDeflection,myText, myOptText);
	  break;
      }
      Standard_Real value = Transparency() ;
      if( value > 0. ) {
	SetTransparency( value );
      }
      break;
    }
  case 2:
    {
    }
  }
  aPrs->ReCompute(); // for hidden line recomputation if necessary...
}

void Sphere_Sphere::SetRadius(const Standard_Real aRadius) {
  myRadius = aRadius;
}

Standard_Real Sphere_Sphere::Radius() const {
  return myRadius;
}

Standard_Integer Sphere_Sphere::NbPanes(const Standard_Real aRadius,
				const Standard_Real aDeflection) {
  if( aRadius <= 0. ) return 0;
  Standard_Real h = aRadius-Abs(aDeflection);
  Standard_Real da = 2.*ACos(h/aRadius);
  Standard_Integer nbpanes = Standard_Integer(2.*Standard_PI/da);
  nbpanes = nbpanes/2*2;
  return nbpanes;
}

Standard_Integer Sphere_Sphere::NbPanes(const Standard_Integer nbItems) {
  return Standard_Integer(Sqrt(2*nbItems));
}

Standard_Integer Sphere_Sphere::NbItems(const Standard_Integer nbPanes) {
  return nbPanes*nbPanes/2;
}

Standard_Real Sphere_Sphere::Deflection(const Standard_Real aRadius,
                                const Standard_Integer nbPanes) {
  Standard_Real da = 2.*Standard_PI/nbPanes;
  Standard_Real h = aRadius*Cos(da/2.);
  return (aRadius-h);
}
