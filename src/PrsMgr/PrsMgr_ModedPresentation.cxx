#include <PrsMgr_ModedPresentation.ixx>

PrsMgr_ModedPresentation::PrsMgr_ModedPresentation() {}

PrsMgr_ModedPresentation::PrsMgr_ModedPresentation(const Handle(PrsMgr_Presentation)& aPresentation, const Standard_Integer aMode): myPresentation(aPresentation), myMode(aMode) {
}

Handle(PrsMgr_Presentation) PrsMgr_ModedPresentation::Presentation () const {

  return myPresentation;

}

Standard_Integer PrsMgr_ModedPresentation::Mode () const {
  return myMode;
}
