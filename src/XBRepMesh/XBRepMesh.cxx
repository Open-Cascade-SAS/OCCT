// File:	XBRepMesh.cxx
// Created:	Fri Apr 11 09:00:48 2008
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <XBRepMesh.ixx>
#include <BRepMesh_PluginMacro.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

//=======================================================================
//function : Discret
//purpose  : 
//=======================================================================
Standard_Integer XBRepMesh::Discret(const TopoDS_Shape& theShape,
				    const Standard_Real theDeflection,
				    const Standard_Real theAngle,
				    BRepMesh_PDiscretRoot& theAlgo)
{
  Standard_Integer iErr;
  //
  iErr=0;
  theAlgo=new BRepMesh_IncrementalMesh;
  theAlgo->SetDeflection(theDeflection);
  theAlgo->SetAngle(theAngle);
  theAlgo->SetShape(theShape);

  return iErr;
}
DISCRETPLUGIN(XBRepMesh)
