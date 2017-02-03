// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_ShadedShape.hxx>
#include <Vrml_Normal.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <Poly_Connect.hxx>
#include <TColgp_Array1OfDir.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Vrml_IndexedFaceSet.hxx>
#include <Vrml_Coordinate3.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <math.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Geom_Surface.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <CSLib_NormalStatus.hxx>
#include <CSLib.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Precision.hxx>
#include <Vrml_Material.hxx>
#include <VrmlConverter_ShadingAspect.hxx>
#include <Vrml_ShapeHints.hxx>
#include <Vrml_MaterialBindingAndNormalBinding.hxx>
#include <Vrml_NormalBinding.hxx>
#include <Vrml_Separator.hxx>
#include <Vrml_NormalBinding.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

//=========================================================================
// function: Add
// purpose
//=========================================================================
void VrmlConverter_ShadedShape::Add( Standard_OStream& anOStream, 
                                     const TopoDS_Shape& aShape,
                                     const Handle(VrmlConverter_Drawer)& aDrawer )
  {
  Handle(Poly_Triangulation) T;
  TopLoc_Location theLocation;
  Standard_Integer i, j, k, decal, nnv, EI;
  
  Standard_Integer t[3], n[3];
  gp_Pnt p;
  TopExp_Explorer ex;

  // counting phasis. This phasis will count the valid triangle
  // and the vertices to allocate the correct size for the arrays: 
  
  Standard_Integer nbTriangles = 0, nbVertices = 0;
  
  Standard_Integer nt, nnn, n1, n2, n3;
    
  // iterating on each face of the shape:
  for (ex.Init(aShape, TopAbs_FACE); ex.More(); ex.Next()) {  
    // getting the face:
    const TopoDS_Face& F = TopoDS::Face(ex.Current());
    // getting the triangulation of the face. The triangulation may not exist:
    T = BRep_Tool::Triangulation(F, theLocation);
      // number of triangles:
    if (T.IsNull()) continue; //smh 
      nnn = T->NbTriangles();            
    
    const TColgp_Array1OfPnt& Nodes = T->Nodes(); 
    // getting a triangle. It is  a triplet of indices in the node table:	
    const Poly_Array1OfTriangle& triangles = T->Triangles(); 
    
    // Taking the nodes of the triangle, taking into account the orientation
    // of the triangle.
    for (nt = 1; nt <= nnn; nt++) {
      if (F.Orientation() == TopAbs_REVERSED) 
	triangles(nt).Get(n1,n3,n2);
      else 
	triangles(nt).Get(n1,n2,n3);
      
      const gp_Pnt& P1 = Nodes(n1);
      const gp_Pnt& P2 = Nodes(n2);
      const gp_Pnt& P3 = Nodes(n3);
      // controlling whether the triangle correct from a 3d point of 
      // view: (the triangle may exist in the UV space but the
      // in the 3d space a dimension is null for example)
      gp_Vec V1(P1,P2);
      if (V1.SquareMagnitude() > 1.e-10) {
	gp_Vec V2(P2,P3);
	if (V2.SquareMagnitude() > 1.e-10) {
	  gp_Vec V3(P3,P1);
	  if (V3.SquareMagnitude() > 1.e-10) {
	    V1.Normalize();
	    V2.Normalize();
	    V1.Cross(V2);
	    if (V1.SquareMagnitude() > 1.e-10) {
	      nbTriangles++;
	      }
	  }
	}
      }
    }
    nbVertices += T->NbNodes();
  }      

//     cout << "nbTriangles = "<< nbTriangles << endl;
//     cout  << "nbVertices = "<< nbVertices << endl << endl;  

//----------------------------
  // now we are going to iterate again to build graphic data from the triangle.   
  if (nbVertices > 2 && nbTriangles > 0) {
    // allocating the graphic arrays.

     Handle(VrmlConverter_ShadingAspect) SA = new VrmlConverter_ShadingAspect;
     SA = aDrawer->ShadingAspect();

     Handle(TColgp_HArray1OfVec) HAV1 = new TColgp_HArray1OfVec(1, nbVertices);
     Handle(TColgp_HArray1OfVec) HAV2 = new TColgp_HArray1OfVec(1, nbVertices);
     
     gp_Vec V, VV;

     Handle(TColStd_HArray1OfInteger) HAI1 = new TColStd_HArray1OfInteger(1,4*nbTriangles);
     Handle(TColStd_HArray1OfInteger) HAI3 = new TColStd_HArray1OfInteger(1,(nbVertices/3*4+nbVertices%3));
     Handle(TColStd_HArray1OfInteger) HAI2 = new TColStd_HArray1OfInteger(1,1);
     Handle(TColStd_HArray1OfInteger) HAI4 = new TColStd_HArray1OfInteger(1,1);

	HAI2->SetValue (1,-1);
	HAI4->SetValue (1,-1);

// !! Specialize HAI2 -  materialIndex  HAI4 - textureCoordinateIndex

    EI = 1;
    nnv = 1;

    for (ex.Init(aShape, TopAbs_FACE); ex.More(); ex.Next()) {
      const TopoDS_Face& F = TopoDS::Face(ex.Current());
      T = BRep_Tool::Triangulation(F, theLocation);
      if (!T.IsNull()) {
	Poly_Connect pc(T);
	
	// 1 -  Building HAV1 -  array of all XYZ of nodes for Vrml_Coordinate3 from the triangles
	//            and HAV2 - array of all normals of nodes for Vrml_Normal
	
	const TColgp_Array1OfPnt& Nodes = T->Nodes();
	TColgp_Array1OfDir NORMAL(Nodes.Lower(), Nodes.Upper());

	  decal = nnv-1;
	
	for (j= Nodes.Lower(); j<= Nodes.Upper(); j++) {
	  p = Nodes(j).Transformed(theLocation.Transformation());

          V.SetX(p.X()); V.SetY(p.Y()); V.SetZ(p.Z());
          HAV1->SetValue(nnv,V);

	  if(SA->HasNormals())
	    {
		// to compute the normal.
	      ComputeNormal(F, pc, NORMAL);

	      VV.SetX(NORMAL(j).X());  VV.SetY(NORMAL(j).Y());  VV.SetZ(NORMAL(j).Z());
	      HAV2->SetValue(nnv,VV);
	    }
          nnv++;
	}
	
	// 2 -   Building HAI1 - array of indexes of all triangles and
	//        HAI3 - array of indexes of all normales  for Vrml_IndexedFaceSet
        nbTriangles = T->NbTriangles();
	const Poly_Array1OfTriangle& triangles = T->Triangles();	
        for (i = 1; i <= nbTriangles; i++) {
          pc.Triangles(i,t[0],t[1],t[2]);
          if (F.Orientation() == TopAbs_REVERSED) 
            triangles(i).Get(n[0],n[2],n[1]);
          else 
            triangles(i).Get(n[0],n[1],n[2]);
          const gp_Pnt& P1 = Nodes(n[0]);
          const gp_Pnt& P2 = Nodes(n[1]);
          const gp_Pnt& P3 = Nodes(n[2]);
          gp_Vec V1(P1,P2);
          if (V1.SquareMagnitude() > 1.e-10) {
            gp_Vec V2(P2,P3);
            if (V2.SquareMagnitude() > 1.e-10) {
              gp_Vec V3(P3,P1);
              if (V3.SquareMagnitude() > 1.e-10) {
        	V1.Normalize();
        	V2.Normalize();
        	V1.Cross(V2);
        	if (V1.SquareMagnitude() > 1.e-10) {
		  for (j = 0; j < 3; j++) {
		   
		    HAI1->SetValue(EI, n[j]+decal-1);	// array of indexes of all triangles
		    EI++;
		  }

		  HAI1->SetValue(EI, -1);		    
		  EI++;		  
		}
	      }
	    }
	  }
	}
      }
    }
  

     if(SA->HasNormals())
       {
	 j=1;
	 for (i=HAI3->Lower(); i <= HAI3->Upper(); i++)
	   {
	     k = i % 4;
	     if (k == 0)
	       {
		 HAI3->SetValue(i, -1);
		 j++;
	       }
	     else
	       {
		 HAI3->SetValue(i, i-j);   
	       }
	   }
       }

//-----------------------------
/*
  cout  << " ******************** " << endl;  
     cout  << " Array HAV1 - Coordinare3 " << endl;  

     for ( i=HAV1->Lower(); i <= HAV1->Upper(); i++ )
       {
	 cout << HAV1->Value(i).X() << " " << HAV1->Value(i).Y()<< " " << HAV1->Value(i).Z() << endl; 
       }

     if(SA->HasNormals())
       {

	 cout  << " ******************** " << endl;         
	 cout  << " Array HAV2 - Normals " << endl;  

	 for ( i=HAV2->Lower(); i <= HAV2->Upper(); i++ )
	   {
	     cout << HAV2->Value(i).X() << " " << HAV2->Value(i).Y()<< " " << HAV2->Value(i).Z() << endl; 
	   }

	 cout  << " ******************** " << endl;  
	 cout  << " Array HAI3 - normalIndex " << endl;  

	 for ( i=HAI3->Lower(); i <= HAI3->Upper(); i++ )
	   {
	     cout << HAI3->Value(i) << endl;
	   }

       }

     cout  << " ******************** " << endl;         
     cout  << " Array HAI1 - coordIndex " << endl;  
       
     for ( i=HAI1->Lower(); i <= HAI1->Upper(); i++ )
       {
	 cout << HAI1->Value(i) << endl;
       }
   
     cout  << " ******************** " << endl;       
*/
//----------------------------

// creation of Vrml objects

     Vrml_ShapeHints  SH; 
     SH = SA->ShapeHints();

     if(SA->HasNormals())
       {
// Separator 1 {
     Vrml_Separator SE1;
     SE1.Print(anOStream);
// Material
  if (SA->HasMaterial()){

     Handle(Vrml_Material) M;
     M = SA->FrontMaterial();

     M->Print(anOStream);
   }

// Coordinate3
     Handle(Vrml_Coordinate3)  C3 = new Vrml_Coordinate3(HAV1);
     C3->Print(anOStream);
// ShapeHints
     SH.Print(anOStream);
// NormalBinding
     Vrml_MaterialBindingAndNormalBinding MBNB1 = Vrml_PER_VERTEX_INDEXED;
     Vrml_NormalBinding   NB(MBNB1);
     NB.Print(anOStream);
// Separator 2 {
     Vrml_Separator SE2;
     SE2.Print(anOStream);
// Normal
     Vrml_Normal  N(HAV2);
     N.Print(anOStream);
// IndexedFaceSet
     Vrml_IndexedFaceSet  IFS;
     IFS.SetCoordIndex(HAI1);
     IFS.SetNormalIndex(HAI3);
     IFS.Print(anOStream);
// Separator 2 }     
     SE2.Print(anOStream);
// Separator 1 }
     SE1.Print(anOStream);
        }
     else 
       { 
// Separator 1 {
     Vrml_Separator SE1;
     SE1.Print(anOStream);
// Material
  if (SA->HasMaterial()){

     Handle(Vrml_Material) M;
     M = SA->FrontMaterial();

     M->Print(anOStream);
   }
// Coordinate3
     Handle(Vrml_Coordinate3)  C3 = new Vrml_Coordinate3(HAV1);
     C3->Print(anOStream);
// ShapeHints
     SH.Print(anOStream);
// IndexedFaceSet
     Vrml_IndexedFaceSet  IFS;
     IFS.SetCoordIndex(HAI1);
     IFS.Print(anOStream);
// Separator 1 }
     SE1.Print(anOStream);
       }
  }
}


//--------- Notes -------------

// the necessary of calculation of Normals and Textures must be define in Drawer
// likes tolerance

//---------- End on notes ------------



//----------------------------
// Computing the normal
//-----------------------------

void VrmlConverter_ShadedShape::ComputeNormal(const TopoDS_Face& aFace, 
					      Poly_Connect& pc, 
					      TColgp_Array1OfDir& Nor)
{
  const Handle(Poly_Triangulation)& T = pc.Triangulation();
  BRepAdaptor_Surface S;
  Standard_Boolean hasUV = T->HasUVNodes();
  Standard_Integer i;
  TopLoc_Location l;
  Handle(Geom_Surface) GS = BRep_Tool::Surface(aFace, l);

  if (hasUV && !GS.IsNull()) {
    Standard_Boolean OK = Standard_True;
    gp_Vec D1U,D1V;
    gp_Vec D2U,D2V,D2UV;
    gp_Pnt P;
    Standard_Real U, V;
    CSLib_DerivativeStatus aStatus;
    CSLib_NormalStatus NStat;
    S.Initialize(aFace);
    const TColgp_Array1OfPnt2d& UVNodes = T->UVNodes();
    for (i = UVNodes.Lower(); i <= UVNodes.Upper(); i++) {
      U = UVNodes(i).X();
      V = UVNodes(i).Y();
      S.D1(U,V,P,D1U,D1V);
      CSLib::Normal(D1U,D1V,Precision::Angular(),aStatus,Nor(i));
      if (aStatus != CSLib_Done) {
	S.D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
	CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,Precision::Angular(),OK,NStat,Nor(i));
      }
      if (aFace.Orientation() == TopAbs_REVERSED) (Nor(i)).Reverse();
    }
  }
  else {
    const TColgp_Array1OfPnt& Nodes = T->Nodes();
    Standard_Integer n[3];
    const Poly_Array1OfTriangle& triangles = T->Triangles();

    for (i = Nodes.Lower(); i <= Nodes.Upper(); i++) {
      gp_XYZ eqPlan(0, 0, 0);
      for (pc.Initialize(i);  pc.More(); pc.Next()) {
	triangles(pc.Value()).Get(n[0], n[1], n[2]);
	gp_XYZ v1(Nodes(n[1]).Coord()-Nodes(n[0]).Coord());
	gp_XYZ v2(Nodes(n[2]).Coord()-Nodes(n[1]).Coord());
	eqPlan += (v1^v2).Normalized();
      }
      Nor(i) = gp_Dir(eqPlan);
      if (aFace.Orientation() == TopAbs_REVERSED) (Nor(i)).Reverse();
    }
  }
}


