#include <PrsMgr_Presentation.ixx>

PrsMgr_Presentation::PrsMgr_Presentation 
(const Handle(PrsMgr_PresentationManager)& aPresentationManager)
:myPresentationManager(aPresentationManager),myMustBeUpdated(Standard_False) {}

void PrsMgr_Presentation::Destroy () {
}
