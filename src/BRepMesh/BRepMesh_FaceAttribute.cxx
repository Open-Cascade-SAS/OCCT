// File:	BRepMesh_FaceAttribute.cxx
// Author:	Ekaterina SMIRNOVA
// Copyright: Open CASCADE SAS 2008

#include <BRepMesh_FaceAttribute.ixx>

//=======================================================================
//function : BRepMesh_FaceAttribute
//purpose  : 
//=======================================================================
BRepMesh_FaceAttribute::BRepMesh_FaceAttribute():
 mydefface(0.), myumin(0.), myumax(0.), myvmin(0.), myvmax(0.),
 mydeltaX(1.), mydeltaY(1.), myminX(0.), myminY(0.)
{
}
