
#ifndef _StdObjMgt_TransientPersistentMap_HeaderFile
#define _StdObjMgt_TransientPersistentMap_HeaderFile

#include <NCollection_DataMap.hxx>

class Standard_Transient;
class StdObjMgt_Persistent;

typedef NCollection_DataMap<Handle(Standard_Transient), Handle(StdObjMgt_Persistent)> StdObjMgt_TransientPersistentMap;

#endif // _StdObjMgt_TransientPersistentMap_HeaderFile
