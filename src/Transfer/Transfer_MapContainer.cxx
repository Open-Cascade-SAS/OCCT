#include <Transfer_MapContainer.ixx>

Transfer_MapContainer::Transfer_MapContainer()
{
}

 void Transfer_MapContainer::SetMapObjects(TColStd_DataMapOfTransientTransient& theMapObjects) 
{
   myMapObj= theMapObjects;
}

 TColStd_DataMapOfTransientTransient& Transfer_MapContainer::GetMapObjects() 
{
  return myMapObj;
}
