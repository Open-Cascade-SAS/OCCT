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


package SamplesTopologyJni;

import CASCADESamplesJni.V3d_Viewer;
import jcas.Standard_ExtString;
import CASCADESamplesJni.V3d_View;
import jcas.Standard_Integer;
import CASCADESamplesJni.AIS_InteractiveContext;
import CASCADESamplesJni.TCollection_AsciiString;


public class SamplesTopologyPackage {

 static {
    System.loadLibrary("SamplesTopologyJni");
 }
            
native public static V3d_Viewer CreateViewer(String aName);
native public static void SetWindow(V3d_View aView,int hiwin,int lowin);
native public static void MakeBox(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeCylinder(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeCone(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeSphere(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeTorus(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeWedge(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakePrism(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeRevol(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakePipe(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeThru(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeEvolved(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void MakeDraft(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Cut(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Fuse(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Common(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Section(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void PSection(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Blend(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void EvolvedBlend(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Chamfer(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Vertex(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Edge(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Wire(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Face(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Shell(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Compound(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Sewing(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Builder(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Geometry(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Explorer(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Validity(AIS_InteractiveContext aContext,TCollection_AsciiString Message,TCollection_AsciiString Result);
native public static void LinearProp(AIS_InteractiveContext aContext,TCollection_AsciiString Message,TCollection_AsciiString Result);
native public static void SurfaceProp(AIS_InteractiveContext aContext,TCollection_AsciiString Message,TCollection_AsciiString Result);
native public static void VolumeProp(AIS_InteractiveContext aContext,TCollection_AsciiString Message,TCollection_AsciiString Result);
native public static void Mirror(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Mirroraxis(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Rotate(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Scale(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Translation(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Displacement(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Deform(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void LocalPrism(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void LocalDPrism(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void LocalRevol(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void LocalPipe(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Rib(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Glue(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Split(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Thick(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void Offset(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void BuildMesh(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void DisplayMesh(AIS_InteractiveContext aContext,TCollection_AsciiString Message);
native public static void ClearMesh(AIS_InteractiveContext aContext,TCollection_AsciiString Message);



}
