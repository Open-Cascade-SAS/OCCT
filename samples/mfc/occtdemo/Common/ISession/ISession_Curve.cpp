// ISession_Curve.cpp: implementation of the ISession_Curve class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ISession_Curve.h"
#include <Prs3d_LineAspect.hxx>
#include <StdPrs_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ArrowAspect.hxx>


IMPLEMENT_STANDARD_HANDLE(ISession_Curve,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTI(ISession_Curve,AIS_InteractiveObject)
//
// Foreach ancestors, we add a IMPLEMENT_STANDARD_SUPERTYPE and 
// a IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY macro.
// We must respect the order: from the direct ancestor class
// to the base class.
//
IMPLEMENT_STANDARD_TYPE(ISession_Curve)
        IMPLEMENT_STANDARD_SUPERTYPE(AIS_InteractiveObject)
        IMPLEMENT_STANDARD_SUPERTYPE(SelectMgr_SelectableObject)
        IMPLEMENT_STANDARD_SUPERTYPE(PrsMgr_PresentableObject)
        IMPLEMENT_STANDARD_SUPERTYPE(MMgt_TShared)
        IMPLEMENT_STANDARD_SUPERTYPE(Standard_Transient)
        IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
                IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(AIS_InteractiveObject)
                IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(SelectMgr_SelectableObject)
                IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(PrsMgr_PresentableObject)
                IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(MMgt_TShared)
                IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(Standard_Transient)
        IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(ISession_Curve)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ISession_Curve::ISession_Curve(const Handle(Geom_Curve)& aCurve)
:AIS_InteractiveObject(),myCurve(aCurve)
{
}

ISession_Curve::~ISession_Curve()
{

}
void ISession_Curve::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                             const Handle(Prs3d_Presentation)& aPresentation,
                             const Standard_Integer /*aMode*/)
{
  GeomAdaptor_Curve anAdaptorCurve(myCurve);
  if (hasOwnColor)
    myDrawer->LineAspect()->SetColor(myOwnColor);
  myDrawer->Link()->SetDiscretisation(100);
  myDrawer->Link()->SetMaximalParameterValue(500);

  StdPrs_Curve::Add (aPresentation, anAdaptorCurve, myDrawer);
}

void ISession_Curve::Compute(const Handle(Prs3d_Projector)& aProjector,
                             const Handle(Prs3d_Presentation)& aPresentation) 
 {

 }

void ISession_Curve::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
				      const Standard_Integer aMode) 
{ 
}


