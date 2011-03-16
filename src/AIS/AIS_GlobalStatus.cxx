// File:	AIS_GlobalStatus.cxx
// Created:	Fri Jan 24 16:54:46 1997
// Author:	Robert COUBLANC
//		<rob@robox.paris1.matra-dtv.fr>


#include <AIS_GlobalStatus.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>



static TColStd_ListIteratorOfListOfInteger It;


AIS_GlobalStatus::AIS_GlobalStatus():
myStatus(AIS_DS_None),
myLayerIndex(0),
myIsHilit(Standard_False),
myHiCol(Quantity_NOC_WHITE),
mySubInt(Standard_False)
{  
}

AIS_GlobalStatus::AIS_GlobalStatus(const AIS_DisplayStatus DS,
				   const Standard_Integer DMode,
				   const Standard_Integer SMode,
				   const Standard_Boolean ishilighted,
				   const Quantity_NameOfColor TheHiCol,
				   const Standard_Integer Layer):
myStatus(DS),
myLayerIndex(Layer),
myIsHilit(Standard_False),
myHiCol(TheHiCol),
mySubInt(Standard_False)
{
  myDispModes.Append(DMode);
  mySelModes.Append(SMode);
}


void AIS_GlobalStatus::RemoveDisplayMode(const Standard_Integer aMode)
{
  for(It.Initialize(myDispModes);It.More();It.Next()){
    if(It.Value()==aMode){ myDispModes.Remove(It);
			   return;}
  }
}

void AIS_GlobalStatus::RemoveSelectionMode(const Standard_Integer aMode)
{
  for(It.Initialize(mySelModes);It.More();It.Next()){
    if(It.Value()==aMode){ mySelModes.Remove(It);
			   return;}
  }
}
void AIS_GlobalStatus::ClearSelectionModes()
{
  mySelModes.Clear();
}

Standard_Boolean AIS_GlobalStatus::IsDModeIn(const Standard_Integer aMode) const 
{
  for(It.Initialize(myDispModes);It.More();It.Next())
    if(It.Value()==aMode) return Standard_True;
  return Standard_False;
  
}

Standard_Boolean AIS_GlobalStatus::IsSModeIn(const Standard_Integer aMode) const 
{
  for(It.Initialize(mySelModes);It.More();It.Next())
    if(It.Value()==aMode) return Standard_True;
  return Standard_False;
}

