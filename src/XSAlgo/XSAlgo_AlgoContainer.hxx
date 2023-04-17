// Created on: 2000-01-19
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _XSAlgo_AlgoContainer_HeaderFile
#define _XSAlgo_AlgoContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Message_ProgressRange.hxx>

class ShapeBuild_ReShape;
class XSAlgo_ToolContainer;
class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Face;
class Transfer_TransientProcess;
class Transfer_FinderProcess;

class XSAlgo_AlgoContainer;
DEFINE_STANDARD_HANDLE(XSAlgo_AlgoContainer, Standard_Transient)


class XSAlgo_AlgoContainer : public Standard_Transient
{

public:

  
  //! Empty constructor
  Standard_EXPORT XSAlgo_AlgoContainer();
  
  //! Sets ToolContainer
    void SetToolContainer (const Handle(XSAlgo_ToolContainer)& TC);
  
  //! Returns ToolContainer
    Handle(XSAlgo_ToolContainer) ToolContainer() const;
  
  //! Performs actions necessary for preparing environment
  //! for transfer. Empty in Open version.
  Standard_EXPORT virtual void PrepareForTransfer() const;
  
  //! Does shape processing with specified tolerances
  //! @param[in] theShape shape to process
  //! @param[in] thePrec basic precision and tolerance
  //! @param[in] theMaxTol maximum allowed tolerance
  //! @param[in] thePrscfile name of the resource file
  //! @param[in] thePseq name of the sequence of operators defined in the resource file for Shape Processing
  //! @param[out] theInfo information to be recorded in the translation map
  //! @param[in] theProgress progress indicator
  //! @param[in] theNonManifold flag to proceed with non-manifold topology
  //! @return the processed shape
  Standard_EXPORT virtual TopoDS_Shape ProcessShape (const TopoDS_Shape&          theShape,
                                                     const Standard_Real          thePrec,
                                                     const Standard_Real          theMaxTol,
                                                     const Standard_CString       thePrscfile,
                                                     const Standard_CString       thePseq,
                                                     Handle(Standard_Transient)&  theInfo,
                                                     const Message_ProgressRange& theProgress = Message_ProgressRange(),
                                                     const Standard_Boolean       theNonManifold = Standard_False) const;

  //! Does shape processing with specified tolerances
  //! @param[in] theShape shape to process
  //! @param[in] thePrec basic precision and tolerance
  //! @param[in] theMaxTol maximum allowed tolerance
  //! @param[in] thePrscfile name of the resource file
  //! @param[in] thePseq name of the sequence of operators defined in the resource file for Shape Processing
  //! @param[out] theInfo information to be recorded in the translation map
  //! @param[in] theReShape tool to record the modifications of input shape
  //! @param[in] theProgress progress indicator
  //! @param[in] theNonManifold flag to proceed with non-manifold topology
  //! @return the processed shape
  Standard_EXPORT virtual TopoDS_Shape ProcessShape(const TopoDS_Shape&               theShape,
                                                    const Standard_Real               thePrec,
                                                    const Standard_Real               theMaxTol,
                                                    const Standard_CString            thePrscfile,
                                                    const Standard_CString            thePseq,
                                                    Handle(Standard_Transient)&       theInfo,
                                                    const Handle(ShapeBuild_ReShape)& theReShape,
                                                    const Message_ProgressRange&      theProgress = Message_ProgressRange(),
                                                    const Standard_Boolean            theNonManifold = Standard_False) const;
  
  //! Checks quality of pcurve of the edge on the given face,
  //! and corrects it if necessary.
  Standard_EXPORT virtual Standard_Boolean CheckPCurve (const TopoDS_Edge& edge, const TopoDS_Face& face, const Standard_Real preci, const Standard_Boolean isSeam) const;
  
  Standard_EXPORT virtual void MergeTransferInfo (const Handle(Transfer_TransientProcess)& TP, const Handle(Standard_Transient)& info, const Standard_Integer startTPitem = 1) const;
  
  //! Updates translation map (TP or FP) with information
  //! resulting from ShapeProcessing
  //! Parameter startTPitem can be used for optimisation, to
  //! restrict modifications to entities stored in TP starting
  //! from item startTPitem
  Standard_EXPORT virtual void MergeTransferInfo (const Handle(Transfer_FinderProcess)& FP, const Handle(Standard_Transient)& info) const;




  DEFINE_STANDARD_RTTIEXT(XSAlgo_AlgoContainer,Standard_Transient)

protected:




private:


  Handle(XSAlgo_ToolContainer) myTC;


};


#include <XSAlgo_AlgoContainer.lxx>





#endif // _XSAlgo_AlgoContainer_HeaderFile
