//--- File:	SelectBasics_SensitiveEntity.cxx
//-- Created:	Mon Jan 23 11:05:19 1995
//-- Author:	Mister rmi
//--		<rmi@photon>
//---Copyright:	 Matra Datavision 1995


#include <SelectBasics_SensitiveEntity.ixx>



//==================================
//function : Initialize
//purpose  : 
//==================================
SelectBasics_SensitiveEntity
::SelectBasics_SensitiveEntity(const Handle(SelectBasics_EntityOwner)& OwnerId,
			       const Standard_ShortReal aFactor):
myOwnerId(OwnerId),
mySFactor(aFactor)
{}


void SelectBasics_SensitiveEntity
::Set (const Handle(SelectBasics_EntityOwner)& TheOwnerId) { myOwnerId = TheOwnerId;}

const Handle(SelectBasics_EntityOwner)&  SelectBasics_SensitiveEntity
::OwnerId() const {return myOwnerId;}

Standard_Real SelectBasics_SensitiveEntity::Depth() const
{return 0.0;}

