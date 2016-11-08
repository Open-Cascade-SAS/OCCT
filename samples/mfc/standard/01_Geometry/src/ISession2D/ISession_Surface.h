// ISession_Surface.h: interface for the ISession_Surface class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

class ISession_Surface : public AIS_InteractiveObject  
{
  DEFINE_STANDARD_RTTIEXT(ISession_Surface,AIS_InteractiveObject)
public:

  ISession_Surface (const Handle(Geom_Surface)& theSurface) : mySurface (theSurface) {}
  virtual ~ISession_Surface() {}

private:

  Standard_EXPORT virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer aMode = 0);
  virtual void Compute (const Handle(Prs3d_Projector)& ,const Handle(Prs3d_Presentation)& ) {}
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& ,const Standard_Integer ) {}

private:

  Handle(Geom_Surface) mySurface;

};

DEFINE_STANDARD_HANDLE(ISession_Surface, AIS_InteractiveObject)
