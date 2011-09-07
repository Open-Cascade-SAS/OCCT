#ifndef _BRepMesh_PluginEntryType_HeaderFile
#define _BRepMesh_PluginEntryType_HeaderFile

#ifndef _Standard_Real_HeaderFile
  #include <Standard_Real.hxx>
#endif

class TopoDS_Shape;
class BRepMesh_DiscretRoot;

//! Type definition for plugin exported function
typedef Standard_Integer (*BRepMesh_PluginEntryType) (const TopoDS_Shape&    theShape,
                                                      const Standard_Real    theDeflection,
                                                      const Standard_Real    theAngle,
                                                      BRepMesh_DiscretRoot*& theMeshAlgoInstance);

#endif //_BRepMesh_PluginEntryType_HeaderFile
