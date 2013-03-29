#ifndef _ISession2D_Shape_HeaderFile
#define _ISession2D_Shape_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>
#include "TopoDS_Shape.hxx"

#include "SelectMgr_SelectableObject.hxx"  
#include "Graphic3d_ArrayOfPolylines.hxx"  
#include "AIS_InteractiveObject.hxx"  

#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_PolyAlgo.hxx>

DEFINE_STANDARD_HANDLE(ISession2D_Shape,AIS_InteractiveObject)
class ISession2D_Shape : public AIS_InteractiveObject {

public:

 // Methods PUBLIC
 // 
Standard_EXPORT ISession2D_Shape ();
void Standard_EXPORT Add(const TopoDS_Shape& aShape);

DEFINE_STANDARD_RTTI(ISession2D_Shape)


//           myProjector
HLRAlgo_Projector& Projector() { return myProjector;};
Standard_EXPORT void SetProjector(HLRAlgo_Projector& aProjector);

private:
Standard_Integer myNbIsos;
public :
Standard_Integer& NbIsos() { return myNbIsos;};
Standard_EXPORT void SetNbIsos(Standard_Integer& aNbIsos) ;

Standard_Boolean AcceptShapeDecomposition() {return Standard_True;};

virtual Standard_Boolean  AcceptSelectionMode(const Standard_Integer aMode) const
{return Standard_True; } 


private: 

 // Methods PRIVATE
 // 
void BuildAlgo();
void BuildPolyAlgo();

void DrawCompound(const Handle(Prs3d_Presentation)& thePresentation,const TopoDS_Shape& theCompound, const Handle(Prs3d_LineAspect) theAspect);


Standard_EXPORT virtual  void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,const Handle(Prs3d_Presentation)& thePresentation,const Standard_Integer theMode = 0) ;

virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,const Standard_Integer aMode) ;

private :
TopTools_ListOfShape myListOfShape;
HLRAlgo_Projector myProjector;
Handle(HLRBRep_Algo) myAlgo;
Handle(HLRBRep_PolyAlgo) myPolyAlgo;


};


// other inCurve functions and methods (like "C++: function call" methods)
//



#endif
