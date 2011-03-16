
#include <RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext.ixx>
#include <StepGeom_GeometricRepresentationContext.hxx>
#include <StepRepr_GlobalUnitAssignedContext.hxx>
#include <StepBasic_HArray1OfNamedUnit.hxx>
#include <StepBasic_NamedUnit.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext.hxx>


RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext::RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext () {}

void RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num0,
	 Handle(Interface_Check)& ach,
	 const Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)& ent) const
{

	Standard_Integer num = num0;


	// --- Instance of plex componant GeometricRepresentationContext ---

	if (!data->CheckNbParams(num,1,ach,"geometric_representation_context")) return;

	// --- field : coordinateSpaceDimension ---

	Standard_Integer aCoordinateSpaceDimension;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadInteger (num,1,"coordinate_space_dimension",ach,aCoordinateSpaceDimension);

	num = data->NextForComplex(num);

	// --- Instance of plex componant GlobalUnitAssignedContext ---

	if (!data->CheckNbParams(num,1,ach,"global_unit_assigned_context")) return;

	// --- field : units ---

	Handle(StepBasic_HArray1OfNamedUnit) aUnits;
	Handle(StepBasic_NamedUnit) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,1,"units",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aUnits = new StepBasic_HArray1OfNamedUnit (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"unit", ach, STANDARD_TYPE(StepBasic_NamedUnit), anent2))
	      aUnits->SetValue(i2, anent2);
	  }
	}

	num = data->NextForComplex(num);

	// --- Instance of common supertype RepresentationContext ---

	if (!data->CheckNbParams(num,2,ach,"representation_context")) return;
	// --- field : contextIdentifier ---


	Handle(TCollection_HAsciiString) aContextIdentifier;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
	data->ReadString (num,1,"context_identifier",ach,aContextIdentifier);
	// --- field : contextType ---


	Handle(TCollection_HAsciiString) aContextType;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadString (num,2,"context_type",ach,aContextType);

	//--- Initialisation of the red entity ---

	ent->Init(aContextIdentifier,aContextType,aCoordinateSpaceDimension,aUnits);
}


void RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)& ent) const
{

	// --- Instance of plex componant GeometricRepresentationContext ---

	SW.StartEntity("GEOMETRIC_REPRESENTATION_CONTEXT");
	// --- field : coordinateSpaceDimension ---

	SW.Send(ent->CoordinateSpaceDimension());

	// --- Instance of plex componant GlobalUnitAssignedContext ---

	SW.StartEntity("GLOBAL_UNIT_ASSIGNED_CONTEXT");
	// --- field : units ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbUnits();  i2 ++) {
	  SW.Send(ent->UnitsValue(i2));
	}
	SW.CloseSub();

	// --- Instance of common supertype RepresentationContext ---

	SW.StartEntity("REPRESENTATION_CONTEXT");
	// --- field : contextIdentifier ---

	SW.Send(ent->ContextIdentifier());
	// --- field : contextType ---

	SW.Send(ent->ContextType());
}


void RWStepGeom_RWGeometricRepresentationContextAndGlobalUnitAssignedContext::Share(const Handle(StepGeom_GeometricRepresentationContextAndGlobalUnitAssignedContext)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbUnits();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->UnitsValue(is1));
	}

}

