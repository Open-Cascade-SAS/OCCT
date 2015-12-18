// OCC_BaseDoc.h: interface for the OCC_BaseDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCC_BASEDOC_H__2E048CC8_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
#define AFX_OCC_BASEDOC_H__2E048CC8_38F9_11D7_8611_0060B0EE281E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

// Base class for all documents in the sample.
// Declares base functionlaity and interface for the sample documents.
// Common properties:
// - interactive context
// - 3d viewer
// Common methods:
// - base events
// - export into one of supported export formats.
class AFX_EXT_CLASS OCC_BaseDoc : public CDocument
{
public:

  OCC_BaseDoc();

  virtual ~OCC_BaseDoc();

  Handle(AIS_InteractiveContext)& GetAISContext() { return myAISContext; }

  Handle(V3d_Viewer) GetViewer() { return myViewer; }

  // Returns string with supported export pixel and vector images formats.
  const CString SupportedImageFormats() const;

  // Exports the given view into of the supported formats.
  void ExportView (const Handle(V3d_View)& theView) const;

  //Events
  virtual void DragEvent (const Standard_Integer /*theMouseX*/,
                          const Standard_Integer /*theMouseY*/,
                          const Standard_Integer /*theState*/,
                          const Handle(V3d_View)& /*theView*/) {}

  virtual void InputEvent (const Standard_Integer /*theMouseX*/,
                           const Standard_Integer /*theMouseY*/,
                           const Handle(V3d_View)& /*theView*/) {}

  virtual void MoveEvent (const Standard_Integer /*theMouseX*/,
                          const Standard_Integer /*theMouseY*/,
                          const Handle(V3d_View)& /*theView*/) {}

  virtual void ShiftMoveEvent (const Standard_Integer /*theMouseX*/,
                               const Standard_Integer /*theMouseY*/,
                               const Handle(V3d_View)& /*theView*/) {}

  virtual void ShiftDragEvent (const Standard_Integer /*theMouseX*/,
                               const Standard_Integer /*theMouseY*/,
                               const Standard_Integer /*theState*/,
                               const Handle(V3d_View)& /*theView*/) {}

  virtual void ShiftInputEvent(const Standard_Integer /*theMouseX*/,
                               const Standard_Integer /*theMouseY*/,
                               const Handle(V3d_View)& /*theView*/) {}

  virtual void Popup (const Standard_Integer /*theMouseX*/,
                      const Standard_Integer /*theMouseY*/,
                      const Handle(V3d_View)& /*theView*/) {}

  void ResetDocumentViews (CDocTemplate* theTemplate);

protected:

  Handle(V3d_Viewer) myViewer;
  Handle(AIS_InteractiveContext) myAISContext;
};

#endif // !defined(AFX_OCC_BASEDOC_H__2E048CC8_38F9_11D7_8611_0060B0EE281E__INCLUDED_)
