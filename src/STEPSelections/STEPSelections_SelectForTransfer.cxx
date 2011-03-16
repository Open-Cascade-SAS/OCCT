#include <STEPSelections_SelectForTransfer.ixx>
#include <Interface_EntityIterator.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
//=======================================================================
//function : STEPSelections_SelectForTransfer
//purpose  : 
//=======================================================================

STEPSelections_SelectForTransfer::STEPSelections_SelectForTransfer()
{
  
}
//=======================================================================
//function : STEPSelections_SelectForTransfer
//purpose  : 
//=======================================================================

STEPSelections_SelectForTransfer::STEPSelections_SelectForTransfer(const Handle(XSControl_TransferReader)& TR)
{
  SetReader(TR);
}

//=======================================================================
//function : RootResult
//purpose  : 
//=======================================================================

 Interface_EntityIterator STEPSelections_SelectForTransfer::RootResult(const Interface_Graph& /*G*/) const
{
  Interface_EntityIterator iter;
  Handle(TColStd_HSequenceOfTransient) roots = Reader()->TransientProcess()->RootsForTransfer();
  Standard_Integer nb = roots->Length();
    for(Standard_Integer i = 1; i <= nb ; i++) 
      iter.GetOneItem(roots->Value(i));
  return iter;
}

