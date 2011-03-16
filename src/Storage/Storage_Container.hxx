#ifndef _Storage_Container_HeaderFile
#define _Storage_Container_HeaderFile

class Handle_Standard_Type;
const Handle_Standard_Type& Storage_Container_Type_();
class Storage_BaseDriver;

typedef Storage_BaseDriver* Storage_Container;

Storage_Container& Storage_CurrentContainer();

#endif

