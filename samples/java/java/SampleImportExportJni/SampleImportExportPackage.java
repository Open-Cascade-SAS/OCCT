// Java Native Class from Cas.Cade
//                     Copyright (C) 1991,1999 by
//  
//                      MATRA DATAVISION, FRANCE
//  
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of the above copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any other person. No title to an ownership of the
// software is hereby transferred.
//  
// At the termination of the contract, the software and all copies of this
// software must be deleted.
//


package SampleImportExportJni;

import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_ExtString;
import CASCADESamplesJni.V3d_View;
import jcas.Standard_Integer;
import jcas.Standard_Boolean;
import jcas.Standard_CString;
import CASCADESamplesJni.AIS_InteractiveContext;
import CASCADESamplesJni.TCollection_AsciiString;
import SampleImportExportJni.MgtBRep_TriangleMode;
import jcas.Standard_Short;
import SampleImportExportJni.IFSelect_ReturnStatus;
import SampleImportExportJni.STEPControl_StepModelType;


public class SampleImportExportPackage {

 static {
    System.loadLibrary("SampleImportExportJni");
 }
            
native public static V3d_Viewer CreateViewer3d(String aName);
native public static void SetWindow3d(V3d_View aView,int hiwin,int lowin);
native public static boolean ReadBREP(Standard_CString aFileName,AIS_InteractiveContext aContext);
native public static boolean SaveBREP(Standard_CString aFileName,AIS_InteractiveContext aContext);
native public static boolean ReadCSFDB(Standard_CString aFileName,AIS_InteractiveContext aContext,TCollection_AsciiString ReturnMessage);
native public static boolean SaveCSFDB(Standard_CString aFileName,AIS_InteractiveContext aContext,TCollection_AsciiString ReturnMessage,short aTriangleMode);
native public static short ReadSTEP(Standard_CString aFileName,AIS_InteractiveContext aContext);
native public static short SaveSTEP(Standard_CString aFileName,AIS_InteractiveContext aContext,short aValue);
native public static int ReadIGES(Standard_CString aFileName,AIS_InteractiveContext aContext);
native public static boolean SaveIGES(Standard_CString aFileName,AIS_InteractiveContext aContext);
native public static boolean SaveImage(Standard_CString aFileName,Standard_CString aFormat,V3d_View aView);



}
