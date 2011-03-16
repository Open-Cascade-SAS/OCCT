// File:	BRepMesh_PluginMacro.hxx
// Created:	Fri Apr 11 09:11:08 2008
// Author:	Peter KURNEV
//		<pkv@irinox>


#ifndef BRepMesh_PluginMacro_HeaderFile
#define BRepMesh_PluginMacro_HeaderFile

#define DISCRETPLUGIN(name) \
extern "C" {Standard_EXPORT Standard_Integer DISCRETALGO(const TopoDS_Shape& ,\
							 const Standard_Real,\
							 const Standard_Real,\
							 BRepMesh_PDiscretRoot& );} \
Standard_Integer DISCRETALGO(const TopoDS_Shape& theShape,\
			     const Standard_Real    theDeflection,\
			     const Standard_Real    theAngle,\
			     BRepMesh_PDiscretRoot&  theAlgo) { \
  return name::Discret(theShape,\
		       theDeflection,\
		       theAngle,\
		       theAlgo);} \
\


#endif
