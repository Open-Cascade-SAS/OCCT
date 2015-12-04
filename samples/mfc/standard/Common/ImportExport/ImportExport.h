// ImportExport.h: interface for the CImportExport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMPORTEXPORT_H__BAA02E6B_A948_11D1_8DA6_0800369C8A03__INCLUDED_)
#define AFX_IMPORTEXPORT_H__BAA02E6B_A948_11D1_8DA6_0800369C8A03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Storage_Error.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Quantity_HArray1OfColor.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <Standard_Macro.hxx>

class Standard_EXPORT CImportExport  
{
public:

	CImportExport() {};
	virtual ~CImportExport(){};

private :
    static Handle(TopTools_HSequenceOfShape) 
        BuildSequenceFromContext(const Handle(AIS_InteractiveContext)& anInteractiveContext,
                                 Handle(Quantity_HArray1OfColor)&      anArrayOfColors,
                                 Handle(TColStd_HArray1OfReal)&        anArrayOfTransparencies);

public :

    //======================================================================
    // return is HSequence to be able to select a set of files
    static int ReadBREP(const Handle(AIS_InteractiveContext)& anInteractiveContext);
    static Handle(TopTools_HSequenceOfShape) ReadBREP();
    static Standard_Boolean ReadBREP(CString      aFileName,
                                    TopoDS_Shape& aShape);

    //----------------------------------------------------------------------
    static void SaveBREP(const Handle(AIS_InteractiveContext)& anInteractiveContext);
    static Standard_Boolean SaveBREP(const TopoDS_Shape& aShape);
    static Standard_Boolean SaveBREP(CString aFileName,
                                    const TopoDS_Shape& aShape);

    //======================================================================

    static void ReadIGES(const Handle(AIS_InteractiveContext)& anInteractiveContext);
	static Handle(TopTools_HSequenceOfShape) ReadIGES(); // not by reference --> the sequence is created here !!
    static Standard_Integer ReadIGES(const Standard_CString& aFileName,
                                     Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape);
    //----------------------------------------------------------------------
    static void SaveIGES(const Handle(AIS_InteractiveContext)& anInteractiveContext);
    static Standard_Boolean SaveIGES(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape);
    static Standard_Boolean SaveIGES(const Standard_CString& aFileName,
                                     const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape);

    //======================================================================

    static void ReadSTEP(const Handle(AIS_InteractiveContext)& anInteractiveContext);
	static Handle(TopTools_HSequenceOfShape) ReadSTEP(); // not by reference --> the sequence is created here !!
    static IFSelect_ReturnStatus ReadSTEP(const Standard_CString& aFileName,
                                          Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape);
    //----------------------------------------------------------------------
    static void SaveSTEP(const Handle(AIS_InteractiveContext)& anInteractiveContext);
    static IFSelect_ReturnStatus SaveSTEP(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape);
    static IFSelect_ReturnStatus SaveSTEP(const Standard_CString& aFileName,
                                          const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,

                                          const STEPControl_StepModelType aValue = STEPControl_AsIs);

    static void ReadSAT(const Handle(AIS_InteractiveContext)& anInteractiveContext);
	static Handle(TopTools_HSequenceOfShape) ReadSAT(); // not by reference --> the sequence is created here !!
	static IFSelect_ReturnStatus ReadSAT(const Standard_CString& aFileName,
                                         Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape);	
    //----------------------------------------------------------------------
	static Standard_Boolean SaveSTL(const Standard_CString& aFileName,
                                          const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,
                                          TCollection_AsciiString& ReturnMessage);
	static Standard_Boolean SaveSTL(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape);
	static void SaveSTL(const Handle(AIS_InteractiveContext)& anInteractiveContext);
    //----------------------------------------------------------------------
	static Standard_Boolean SaveVRML(const Standard_CString& aFileName,
                                          const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,
                                          const Handle(Quantity_HArray1OfColor)&   anArrayOfColors,
                                          const Handle(TColStd_HArray1OfReal)&     anArrayOfTransparencies,
                                          TCollection_AsciiString& ReturnMessage);
	static Standard_Boolean SaveVRML(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape,
                                     const Handle(Quantity_HArray1OfColor)&   anArrayOfColors,
                                     const Handle(TColStd_HArray1OfReal)&     anArrayOfTransparencies);
	static void SaveVRML(const Handle(AIS_InteractiveContext)& anInteractiveContext);

};

#endif // !defined(AFX_IMPORTEXPORT_H__BAA02E6B_A948_11D1_8DA6_0800369C8A03__INCLUDED_)
