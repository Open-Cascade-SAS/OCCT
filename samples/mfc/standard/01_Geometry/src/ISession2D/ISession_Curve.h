// ISession_Curve.h: interface for the ISession_Curve class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include "AIS_InteractiveObject.hxx"

class ISession_Curve : public AIS_InteractiveObject  
{
  DEFINE_STANDARD_RTTIEXT(ISession_Curve,AIS_InteractiveObject)
public:
  ISession_Curve(const Handle(Geom_Curve)& theCurve) : myCurve (theCurve) {}
  virtual ~ISession_Curve() {}

private:

  Standard_EXPORT virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer aMode = 0) ;
  virtual void Compute(const Handle(Prs3d_Projector)& ,const Handle(Prs3d_Presentation)& ) {}
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& ,const Standard_Integer ) {}

private:
  Handle(Geom_Curve) myCurve;
};

DEFINE_STANDARD_HANDLE(ISession_Curve,AIS_InteractiveObject)
