#include <StepShape_FacetedBrepAndBrepWithVoids.ixx>

#include <StepShape_FacetedBrep.hxx>

#include <StepShape_BrepWithVoids.hxx>


StepShape_FacetedBrepAndBrepWithVoids::StepShape_FacetedBrepAndBrepWithVoids ()  {}

void StepShape_FacetedBrepAndBrepWithVoids::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(StepShape_ClosedShell)& aOuter)
{
  
  StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}

void StepShape_FacetedBrepAndBrepWithVoids::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(StepShape_ClosedShell)& aOuter,
 const Handle(StepShape_FacetedBrep)& aFacetedBrep,
 const Handle(StepShape_BrepWithVoids)& aBrepWithVoids)
{
  // --- class own fields ---
  facetedBrep = aFacetedBrep;
  brepWithVoids = aBrepWithVoids;
  // --- class inherited fields ---
  StepShape_ManifoldSolidBrep::Init(aName, aOuter);
}


void StepShape_FacetedBrepAndBrepWithVoids::Init
(const Handle(TCollection_HAsciiString)& aName,
 const Handle(StepShape_ClosedShell)& aOuter,
 const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	// --- class inherited fields ---

	StepShape_ManifoldSolidBrep::Init(aName, aOuter);
	
	// --- ANDOR componant fields ---
	
	brepWithVoids = new StepShape_BrepWithVoids();
	brepWithVoids->Init(aName, aOuter, aVoids);
	
	// --- ANDOR componant fields ---
	
	facetedBrep = new StepShape_FacetedBrep();
	facetedBrep->Init(aName, aOuter);
}


void StepShape_FacetedBrepAndBrepWithVoids::SetFacetedBrep(const Handle(StepShape_FacetedBrep)& aFacetedBrep)
{
	facetedBrep = aFacetedBrep;
}

Handle(StepShape_FacetedBrep) StepShape_FacetedBrepAndBrepWithVoids::FacetedBrep() const
{
	return facetedBrep;
}

void StepShape_FacetedBrepAndBrepWithVoids::SetBrepWithVoids(const Handle(StepShape_BrepWithVoids)& aBrepWithVoids)
{
	brepWithVoids = aBrepWithVoids;
}

Handle(StepShape_BrepWithVoids) StepShape_FacetedBrepAndBrepWithVoids::BrepWithVoids() const
{
	return brepWithVoids;
}

	//--- Specific Methods for AND class field access ---


void StepShape_FacetedBrepAndBrepWithVoids::SetVoids(const Handle(StepShape_HArray1OfOrientedClosedShell)& aVoids)
{
	brepWithVoids->SetVoids(aVoids);
}

Handle(StepShape_HArray1OfOrientedClosedShell) StepShape_FacetedBrepAndBrepWithVoids::Voids() const
{
	return brepWithVoids->Voids();
}

Handle(StepShape_OrientedClosedShell) StepShape_FacetedBrepAndBrepWithVoids::VoidsValue(const Standard_Integer num) const
{
	return brepWithVoids->VoidsValue(num);
}

Standard_Integer StepShape_FacetedBrepAndBrepWithVoids::NbVoids () const
{
	return brepWithVoids->NbVoids();
}

	//--- Specific Methods for AND class field access ---

