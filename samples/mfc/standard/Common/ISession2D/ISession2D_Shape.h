#ifndef _ISession2D_Shape_HeaderFile
#define _ISession2D_Shape_HeaderFile

#include "AIS_InteractiveObject.hxx"
#include "Graphic3d_ArrayOfPolylines.hxx"
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <NCollection_List.hxx>
#include "SelectMgr_SelectableObject.hxx"
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include "TopoDS_Shape.hxx"
class ISession2D_Shape;
DEFINE_STANDARD_HANDLE(ISession2D_Shape,AIS_InteractiveObject)

class ISession2D_Shape : public AIS_InteractiveObject
{
public:
  Standard_EXPORT ISession2D_Shape ();

  // Adds shape to the list of topological shapes
  void Standard_EXPORT Add (const TopoDS_Shape& aShape);

  // Removes shape from the list of shapes.
  // It is used in case of shapes erasing.
  void Standard_EXPORT Remove (const TopoDS_Shape& theShape);
  // Returns myProjector
  HLRAlgo_Projector& Projector() { return myProjector;};

  Standard_EXPORT void SetProjector (HLRAlgo_Projector& aProjector);


  Standard_Integer& NbIsos() { return myNbIsos;};

  Standard_EXPORT void SetNbIsos (Standard_Integer& aNbIsos);

  Standard_Boolean AcceptShapeDecomposition() {return Standard_True;}

  virtual Standard_Boolean AcceptSelectionMode (const Standard_Integer /*aMode*/) const
  { return Standard_True; }

public:
  DEFINE_STANDARD_RTTIEXT(ISession2D_Shape,AIS_InteractiveObject)

private:
  void BuildAlgo();
  void BuildPolyAlgo();

  void DrawCompound (const Handle(Prs3d_Presentation)& thePresentation,
                     const TopoDS_Shape& theCompound,
                     const Handle(Prs3d_LineAspect) theAspect);

  Standard_EXPORT virtual  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                         const Handle(Prs3d_Presentation)& thePresentation,
                                         const Standard_Integer theMode = 0);

  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection,
                                 const Standard_Integer aMode);

private:

  Standard_Integer myNbIsos;
  NCollection_List<TopoDS_Shape> myListOfShape;
  HLRAlgo_Projector myProjector;
  Handle(HLRBRep_Algo) myAlgo;
  Handle(HLRBRep_PolyAlgo) myPolyAlgo;
};
// other inCurve functions and methods (like "C++: function call" methods)
//



#endif
