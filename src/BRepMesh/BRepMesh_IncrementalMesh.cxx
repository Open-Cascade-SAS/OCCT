// File:	BRepMesh_IncrementalMesh.cxx
// Created:	Tue Jun 20 10:34:51 1995
// Author:	Stagiaire Alain JOURDAIN
//		<ajo@phobox>


#include <BRepMesh_IncrementalMesh.ixx>

#include <BRepMesh.hxx>
#include <BRepMesh_Edge.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_FastDiscretFace.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopTools_HArray1OfShape.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>

#include <vector>

// NOTE: to be replaced by more correct check
// #if defined(WNT) || defined(LIN)
// #define HAVE_TBB 1
// #endif

// paralleling with Intel TBB
#ifdef HAVE_TBB
#include <tbb/parallel_for_each.h>
#endif

//=======================================================================
//function : BRepMesh_IncrementalMesh
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::BRepMesh_IncrementalMesh() 
: myRelative(Standard_False),
  myModified(Standard_False),
  myStatus(0)
{
  mymapedge.Clear();
  myancestors.Clear();
}

//=======================================================================
//function : BRepMesh_IncrementalMesh
//purpose  : 
//=======================================================================

BRepMesh_IncrementalMesh::
BRepMesh_IncrementalMesh(const TopoDS_Shape& S,
			 const Standard_Real D,
			 const Standard_Boolean Rel,
			 const Standard_Real Ang) :
			 myRelative(Rel),
			 myModified(Standard_False),
                         myStatus(0)
{
  mymapedge.Clear();
  myancestors.Clear();
  myDeflection = D;
  myAngle = Ang;
  myShape = S;
  
  //
  Perform(); 
}

//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BRepMesh_IncrementalMesh::~BRepMesh_IncrementalMesh() 
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::Init() 
{
  myModified=Standard_False;
  mymapedge.Clear();
  myancestors.Clear();
}

//=======================================================================
//function : SetRelative
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::SetRelative(const Standard_Boolean theFlag)
{
  myRelative=theFlag;
}

//=======================================================================
//function : Relative
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_IncrementalMesh::Relative()const
{
  return myRelative;
}

//=======================================================================
//function : IsModified
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_IncrementalMesh::IsModified() const
{
  return myModified;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepMesh_IncrementalMesh::Perform()
{
  Bnd_Box aBox;
  //
  SetDone();
  //
  Init(); 
  //
  BRepBndLib::Add(myShape, aBox);
  myBox = aBox;
  //
  if (!myMesh.IsNull()) {
    myMesh.Nullify();
  }
  //
  myMesh = new BRepMesh_FastDiscret(myDeflection,
				    myAngle,
				    aBox,
				    Standard_True,
				    Standard_True,
				    myRelative,
				    Standard_True);
  //
  Update(myShape);
}

//=======================================================================
//function : GetStatus
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_IncrementalMesh::GetStatusFlags() const
{
  return myStatus;
}

//=======================================================================
//function : Update(shape)
//purpose  : Builds the incremental mesh of the shape
//=======================================================================
void BRepMesh_IncrementalMesh::Update(const TopoDS_Shape& S)
				  
{
  myModified = Standard_False;
  TopExp_Explorer ex;

  Standard_Real TXmin, TYmin, TZmin, TXmax, TYmax, TZmax;
  Standard_Real dx, dy, dz;

  //AGV 080407: Since version 6.2.0 there would be exception without this check
  if (myBox.IsVoid())
    return;
  
  myBox.Get(TXmin, TYmin, TZmin, TXmax, TYmax, TZmax);
  dx = TXmax-TXmin;
  dy = TYmax-TYmin;
  dz = TZmax-TZmin;
  mydtotale = dx;
  if (dy > mydtotale) mydtotale = dy;
  if (dz > mydtotale) mydtotale = dz;
  
  for (ex.Init(S, TopAbs_EDGE); ex.More(); ex.Next()) {
    if(BRep_Tool::IsGeometric(TopoDS::Edge(ex.Current()))) {
      Update(TopoDS::Edge(ex.Current()));
    }
  }

  // get list of faces
  TopTools_ListOfShape LF;
  BRepLib::ReverseSortFaces(S,LF);

  // make array of faces suitable for processing (excluding faces without surface)
  std::vector<TopoDS_Face> aFaces;
  for (TopTools_ListIteratorOfListOfShape it(LF); it.More(); it.Next()) 
  {
    TopoDS_Face F = TopoDS::Face(it.Value());

    TopLoc_Location L1;
    const Handle(Geom_Surface)& Surf = BRep_Tool::Surface(F, L1);
    if(Surf.IsNull())
      continue;
    
    Update (F);
    aFaces.push_back (F);
  }

  // mesh faces in parallel threads using TBB
#ifdef HAVE_TBB
  if (Standard::IsReentrant())
    tbb::parallel_for_each (aFaces.begin(), aFaces.end(), *myMesh.operator->());
  else
#endif
  for (std::vector<TopoDS_Face>::iterator it(aFaces.begin()); it != aFaces.end(); it++)
    myMesh->Process (*it);

  // maillage des edges non contenues dans les faces :
  Standard_Real f, l, defedge;
  Standard_Integer i, nbNodes;
  TopLoc_Location L;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  ex.Init(S ,TopAbs_EDGE, TopAbs_FACE);

  while (ex.More()) {
    const TopoDS_Edge& E = TopoDS::Edge(ex.Current());

    if(!BRep_Tool::IsGeometric(E)) {
      ex.Next();
      continue;
    }

    if (myRelative) {
      Bnd_Box B;
      BRepBndLib::Add(E, B);
      B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      dx = aXmax-aXmin;
      dy = aYmax-aYmin;
      dz = aZmax-aZmin;
      defedge = dx;
      if (defedge < dy) defedge = dy;
      if (defedge < dz) defedge = dz;
      // ajustement par rapport a la taille totale:
      Standard_Real cdef = mydtotale/(2*defedge);
      if (cdef < 0.5) cdef = 0.5;
      if (cdef > 2.) cdef = 2.;
      defedge = cdef * defedge * myDeflection;
    }
    else defedge = myDeflection;
    
    Handle(Poly_Polygon3D) P3D = BRep_Tool::Polygon3D(E, L);
    Standard_Boolean maill = Standard_False;
    if (P3D.IsNull()) {
      maill = Standard_True;
    }
    else if (P3D->Deflection() > 1.1*defedge) {
      maill = Standard_True;
    }
    if (maill) {
      BRepAdaptor_Curve C(E);
      f = C.FirstParameter();
      l = C.LastParameter();
      
      GCPnts_TangentialDeflection TD(C, f, l, myAngle, defedge, 2);
      nbNodes = TD.NbPoints();
      
      TColgp_Array1OfPnt Nodes(1, nbNodes);
      TColStd_Array1OfReal UVNodes(1, nbNodes);
      for ( i = 1; i <= nbNodes; i++) {
	Nodes(i) = TD.Value(i);
	UVNodes(i) = TD.Parameter(i);
      }
      
      BRep_Builder B;
      Handle(Poly_Polygon3D) P = new Poly_Polygon3D(Nodes, UVNodes);
      P->Deflection(myDeflection);
      B.UpdateEdge(E, P);
    }

    ex.Next();
  }
}    
//=======================================================================
//function : Update(edge)
//purpose  : Locate a correct discretisation if it exists
//           Set no one otherwise
//=======================================================================
void BRepMesh_IncrementalMesh::Update(const TopoDS_Edge& E)
{
  TopLoc_Location l;
  Standard_Integer i = 1;
  Handle(Poly_Triangulation) T, TNull;
  Handle(Poly_PolygonOnTriangulation) Poly, NullPoly;
  Standard_Boolean found = Standard_False;
  Standard_Real defedge;
  BRep_Builder B;
  Standard_Boolean defined = Standard_False;
  
  do {
    BRep_Tool::PolygonOnTriangulation(E, Poly, T, l, i);
    i++;
    if (!T.IsNull() && !Poly.IsNull()) {
      if (!defined) {
	if (myRelative) {
	  Bnd_Box aBox;
	  BRepBndLib::Add(E, aBox);
	  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dx, dy, dz;
	  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	  dx = aXmax-aXmin;
	  dy = aYmax-aYmin;
	  dz = aZmax-aZmin;
	  defedge = dx;
	  if (defedge < dy) defedge = dy;
	  if (defedge < dz) defedge = dz;
	  // ajustement par rapport a la taille totale:
	  Standard_Real cdef = mydtotale/(2*defedge);
	  if (cdef < 0.5) cdef = 0.5;
	  if (cdef > 2.) cdef = 2.;
	  defedge = cdef * defedge * myDeflection;
	}
	else defedge = myDeflection;
	mymapedge.Bind(E, defedge);
	defined = Standard_True;
      }
      if ((!myRelative && Poly->Deflection() <= 1.1*defedge) ||
	  (myRelative && Poly->Deflection() <= 1.1*defedge)) 
	found = Standard_True;
      else {
	myModified = Standard_True;
	B.UpdateEdge(E, NullPoly, T, l);
      }
    }
  } while (!Poly.IsNull());

  if (!found) myMap.Add(E);
}


//=======================================================================
//function : Update(face)
//purpose  : If the face is not correctly triangulated, or if one of its
//           edges is to be discretisated correctly, the triangulation
//           of this face is built.
//=======================================================================
void  BRepMesh_IncrementalMesh::Update(const TopoDS_Face& F)
{
  TopLoc_Location l;
  Handle(Geom_Surface) SS = BRep_Tool::Surface(F, l);
  if (SS.IsNull()) return;

  //Standard_Integer i;
  Standard_Boolean WillBeTriangulated = Standard_False;
  Handle(Poly_Triangulation) T, TNull;
  T = BRep_Tool::Triangulation(F, l);
  Handle(Poly_PolygonOnTriangulation) Poly, NullPoly;

  BRep_Builder B;
  TopExp_Explorer ex;
  
  Standard_Real defedge, defface;
  Standard_Integer nbEdge = 0;
  if (myRelative) {
    defface = 0.;
    
    for (ex.Init(F, TopAbs_EDGE); ex.More(); ex.Next()) {
      const TopoDS_Edge& edge = TopoDS::Edge(ex.Current());
      nbEdge++;
      if (mymapedge.IsBound(edge)) {
	defedge = mymapedge(edge);
      }
      else {
	Bnd_Box aBox;
	BRepBndLib::Add(edge, aBox);
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dx, dy, dz;
	aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	dx = aXmax-aXmin;
	dy = aYmax-aYmin;
	dz = aZmax-aZmin;
	defedge = dx;
	if (defedge < dy) defedge = dy;
	if (defedge < dz) defedge = dz;
	// ajustement par rapport a la taille totale:
	Standard_Real cdef = mydtotale/(2*defedge);
	if (cdef < 0.5) cdef = 0.5;
	if (cdef > 2.) cdef = 2.;
	defedge = cdef * defedge * myDeflection;
      }
      defface = defface + defedge;
    }
    if (nbEdge != 0) defface = defface / nbEdge;
    else             defface = myDeflection;
  }
  else
    defface = myDeflection;

  if (!T.IsNull()) {
    if ((!myRelative && T->Deflection() <= 1.1*defface) ||
	(myRelative && T->Deflection() <= 1.1*defface)) {
      for (ex.Init(F, TopAbs_EDGE);
	   ex.More();
	   ex.Next()) {
	const TopoDS_Shape& E = ex.Current();
	Poly = BRep_Tool::PolygonOnTriangulation(TopoDS::Edge(E), T, l);
	if (Poly.IsNull() || myMap.Contains(E)) {
	  WillBeTriangulated = Standard_True;
	  // cas un peu special. la triangulation est bonne, mais
	  // l'edge n'a pas de representation polygonalisee sur celle-ci.
	  break;
	}
      }
    } 
    else WillBeTriangulated = Standard_True;
  }

  if (WillBeTriangulated || T.IsNull()) {
    myModified = Standard_True;
    if (!T.IsNull()) {
      for (ex.Init(F, TopAbs_EDGE); ex.More(); ex.Next()) {
	B.UpdateEdge(TopoDS::Edge(ex.Current()), NullPoly, T, l);
	myMap.Remove(ex.Current());
      }
      B.UpdateFace(F, TNull);
    }
    myMesh->Add(F);
    myStatus |= (Standard_Integer)(myMesh->CurrentFaceStatus());
    if (myMesh->CurrentFaceStatus() == BRepMesh_ReMesh) {
#ifdef DEB_MESH
      cout << " face remaillee + finement que prevu."<< endl;
#endif

      Standard_Integer index;
      if( myancestors.Extent() < 1 )
        TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,myancestors);

      TopTools_MapOfShape MShape;
      MShape.Add(F);

      TopoDS_Iterator ex(F),ex2;
      for (; ex.More(); ex.Next()) {
        const TopoDS_Shape& aWire = ex.Value();
        if (aWire.ShapeType() != TopAbs_WIRE)
          continue;
        TopoDS_Iterator exW(aWire);
        for(; exW.More(); exW.Next()) {
          const TopoDS_Edge& edge = TopoDS::Edge(exW.Value());
          index = myancestors.FindIndex(edge);
          if (index != 0) {
            const TopTools_ListOfShape& L = myancestors.FindFromKey(edge);
	  
            TopTools_ListIteratorOfListOfShape it(L);
	  
            for (; it.More(); it.Next()) {
              TopoDS_Face F2 = TopoDS::Face(it.Value());
              if (!MShape.Contains(F2)) {
                MShape.Add(F2);
                T = BRep_Tool::Triangulation(F2, l);	  
                if (!T.IsNull()) {
#ifdef DEB_MESH
                  cout <<"triangulation a refaire" <<endl;
#endif
                  for (ex2.Initialize(F2); ex2.More(); ex2.Next()) {
                    const TopoDS_Shape& aWire2 = ex2.Value();
                    if (aWire2.ShapeType() != TopAbs_WIRE)
                      continue;
                    TopoDS_Iterator exW2(aWire2);
                    for(; exW2.More(); exW2.Next()) {
                      TopoDS_Edge E2 = TopoDS::Edge(exW2.Value());
                      B.UpdateEdge(E2, NullPoly, T, l);
                    }
                  }
                  B.UpdateFace(F2, TNull);
                  myMesh->Add(F2);
                }
              }
            }
          }
        }
      }      
    }
  }
}
