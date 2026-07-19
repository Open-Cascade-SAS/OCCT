Upgrade from older OCCT versions  {#occt__upgrade}
================================

@tableofcontents

@section upgrade_intro Introduction

This document provides technical details on changes made in particular versions of OCCT. It can help to upgrade user applications based on previous versions of OCCT to newer ones.

@ref upgrade_occt800 "SEEK TO THE LAST CHAPTER (UPGRADE TO 8.0.0)"

@subsection upgrade_intro_precautions Precautions

Back-up your code before the upgrade.
We strongly recommend using version control system during the upgrade process and saving one or several commits at each step of upgrade, until the overall result is verified.
This will facilitate identification and correction of possible problems that can occur at the intermediate steps of upgrade.
It is advisable to document each step carefully to be able to repeat it if necessary.

@subsection upgrade_intro_disclaim Disclaimer

This document describes known issues that have been encountered during porting of OCCT and some applications and approaches that have helped to resolve these issues in known cases.
It does not pretend to cover all possible migration issues that can appear in your application.
Take this document with discretion; apply your expertise and knowledge of your application to ensure the correct result. 

The automatic upgrade tool is provided as is, without warranty of any kind, and we explicitly disclaim any liability for possible errors that may appear due to use of this tool. 
It is your responsibility to ensure that the changes you made in your code are correct. 
When you upgrade the code by an automatic script, make sure to carefully review the introduced changes at each step before committing them.

@section upgrade_pre_7_1 Upgrades pre-7.1.0

Detailed upgrade notes for OCCT versions 6.5.0 through 7.0.0 are available in the versioned documentation archives, e.g. https://dev.opencascade.org/doc/occt-7.0.0/overview/html/index.html. Refer to the [OCCT GitHub releases](https://github.com/Open-Cascade-SAS/OCCT/releases) for per-version change logs.


@section upgrade_occt710 Upgrade to OCCT 7.1.0

@subsection upgrade_710_aspects Presentation attributes

This section should be considered if application defines custom presentations, i.e. inherited from *AIS_InteractiveObject*.
The previous versions of OCCT have three levels for defining presentation properties (e.g. colors, materials, etc.):

1. For the entire structure - *Graphic3d_Structure* / *Prs3d_Presentation*.
2. For a specific group of primitives - *Graphic3d_Group::SetGroupPrimitivesAspect()* overriding structure aspects.
3. For a specific primitive array within the graphic group - *Graphic3d_Group::SetPrimitivesAspect()*.

The structure level has de facto not been used for a long time since OCCT presentations always define aspects at the graphic group level (overriding any structure aspects).
Within this OCCT release, structure level of aspects has been completely removed. In most cases the application code should just remove missing methods. In those rare cases, when this functionality was intentionally used, the application should explicitly define aspects to the  appropriate graphic groups.

Note that defining several different aspects within the same graphic group should also be avoided in the application code since it is a deprecated functionality which can be removed in further releases.
*Graphic3d_Group::SetGroupPrimitivesAspect()* should be the main method defining presentation attributes.

The implementation of *Graphic3d_Group::SetGroupPrimitivesAspect()* has been changed from copying aspect values to keeping the passed object.
Although it was not documented, previously it was possible to modify a single aspect instance, like *Graphic3d_AspectFillArea3d* and set it to multiple groups.
Now such code would produce an unexpected result and therefore should be updated to create the dedicated aspect instance.

@subsection upgrade_710_types Typedefs

The following type definitions in OCCT has been modified to use C++11 types:
- *Standard_Boolean* is now *bool* (previously *unsigned int*).
- *Standard_ExtCharacter* is now *char16_t* (previously *short*).
- *Standard_ExtString;* is now *const char16_t* (previously *const short*).
- *Standard_Utf16Char* is now *char16_t* (previously *uint16_t* for compatibility with old compilers).
- *Standard_Utf32Char* is now *char32_t* (previously *uint32_t* for compatibility with old compilers).

For most applications this change should be transparent on the level of source code. Binary compatibility is not maintained, as *bool* has a different size in comparison with *unsigned int*.

@subsection upgrade_710_ffp Programmable Pipeline

Fixed-function pipeline has been already deprecated since OCCT 7.0.0.
Release 7.1.0 disables this functionality by default in favor of Programmable Pipeline (based on GLSL programs).

Method *V3d_View::Export()*, based on *gl2ps* library, requires fixed pipeline and will return error if used with default settings.
Applications should explicitly enable fixed pipeline by setting *OpenGl_Caps::ffpEnable* flag to TRUE within *OpenGl_GraphicDriver::ChangeOptions()* before creating the viewer to use *V3d_View::Export()*.
This method is declared as deprecated and will be removed in one of the next OCCT releases.
The recommended way to generate a vector image of a 3D model or scene is to use an application-level solution independent from OpenGL.

@subsection upgrade_710_trsfpers Transformation persistence

The behavior of transformation persistence flags *Graphic3d_TMF_ZoomPers* and *Graphic3d_TMF_TriedronPers* has been changed for consistency with a textured fixed-size 2D text.
An object with these flags is considered as defined in pixel units, and the presentation is no more scaled depending on the view height.
The applications that need to scale such objects depending on viewport size should update them manually.

Flags *Graphic3d_TMF_PanPers* and *Graphic3d_TMF_FullPers* have been removed.
*Graphic3d_TMF_TriedronPers* or *Graphic3d_TMF_2d* can be used instead depending on the context.

*Graphic3d_TransModeFlags* is not an integer bitmask anymore - enumeration values should be specified instead.
Several transformation persistence methods in *PrsMgr_PresentableObject* have been marked deprecated.
Transformation persistence should be defined using  *Graphic3d_TransformPers* constructor directly and passed by a handle, not value.

@subsection upgrade_710_selprops Dynamic highlight and selection properties

Release 7.1.0 introduces *Graphic3d_HighlightStyle* - an entity that allows flexible customization of highlighting parameters (such as highlighting method, color, and transparency). Therefore, the signatures of the following methods related to highlighting:
- *AIS_InteractiveContext::Hilight()*;
- *AIS_InteractiveContext::HilightWithColor()*;
- *PrsMgr_PresentationManager::Color()*;
- *SelectMgr_EntityOwner::HilightWithColor()*;
have been changed to receive *Graphic3d_HighlightStyle* instead of *Quantity_Color*.

Method *AIS_InteractiveContext::Hilight* is now deprecated and highlights the interactive object with selection style.

A group of methods *AIS_InteractiveContext::IsHilighted* has changed its behavior - now they only check highlight flags of the object or the owner in the global status. If the highlight color is required on the application level, it is necessary to use overloaded methods *AIS_InteractiveContext::HighlightStyle* for the owner and the object.

The following methods have been replaced in *AIS_InteractiveContext* class:
- *HilightColor* and *SetHilightColor* by *HighlightStyle* and *SetHighlightStyle*;
- *SelectionColor* setter and getter by *SelectionStyle* and *SetSelectionStyle*.

The API of *Prs3d_Drawer* has been extended to allow setting up styles for both dynamic selection and highlighting. Therefore, it is possible to change the highlight style of a particular object on the application level via *SelectMgr_SelectableObject::HilightAttributes()* and process it in the entity owner.

@subsection upgrade_occt710_correction_of_TObj_Model Correction in TObj_Model class

Methods *TObj_Model::SaveAs* and *TObj_Model::Load* now receive *TCollection_ExtendedString* filename arguments instead of char*. UTF-16 encoding can be used to pass file names containing Unicode symbols.

@subsection upgrade_710_env Redundant environment variables

The following environment variables have become redundant:

* *CSF_UnitsLexicon* and *CSF_UnitsDefinition* are no more used.  Units definition (*UnitsAPI/Lexi_Expr.dat* and *UnitsAPI/Units.dat*) is now embedded into source code.
* *CSF_XSMessage* and *CSF_XHMessage* are now optional.
  English messages (XSMessage/\*XSTEP.us* and SHMessage/\*SHAPE.us*) are now embedded into source code
  and automatically loaded when environment variables are not set.
* *CSF_ShadersDirectory* is not required any more, though it still can be used to load custom shaders.
  Mandatory GLSL resources are now embedded into source code.
* *CSF_PluginDefaults* and other variables pointing to OCAF plugin resources (*CSF_StandardDefaults*, *CSF_XCAFDefaults*, *CSF_StandardLiteDefaults* and *CSF_XmlOcafResource*) are not necessary if method *TDocStd_Application::DefineFormat()* is used to enable persistence of OCAF documents.

Other environment variables still can be used to customize behavior of relevant algorithms but are not necessary any more (all required resources are embedded).

@subsection upgrade_710_removed Removed features

The following obsolete features have been removed:
* Anti-aliasing API *V3d_View::SetAntialiasingOn()*. This method was intended to activate deprecated OpenGL functionality *GL_POLYGON_SMOOTH, GL_LINE_SMOOTH* and *GL_POINT_SMOOTH*.
  Instead of the old API, the application should request MSAA buffers for anti-aliasing by assigning *Graphic3d_RenderingParams::NbMsaaSamples* property of the structure returned by *V3d_View::ChangeRenderingParams()*.
* *Prs3d_Drawer::ShadingAspectGlobal()* flag has been removed as not used. The corresponding calls can be removed safely from the application code.
* The methods managing ZClipping planes and ZCueing: *V3d_View::SetZClippingType()*, *V3d_View::SetZCueingOn()*, etc. have been removed.  ZClipping planes can be replaced by general-purpose clipping planes (the application should update plane definition manually).
* The 3D viewer printing API *V3d_View::Print()* has been removed. This functionality was available on Windows platforms only. The applications should use the general image dump API *V3d_View::ToPixMap()* and manage printing using a platform-specific API at the application level.
  Text resolution can be managed by rendering parameter *Graphic3d_RenderingParams::Resolution*, returned by *V3d_View::ChangeRenderingParams()*.
* Methods *PrsMgr_PresentationManager::BoundBox*, *PrsMgr_PresentationManager::Hilight* and *SelectMgr_EntityOwner::Hilight* have been removed as not used. The corresponding method in custom implementations of *SelectMgr_EntityOwner* can be removed safely. *PrsMgr_PresentationManager::Color* with the corresponding style must be used instead.
* Package *SortTools* has been removed. The code that used the tools provided by that package should be corrected manually. The recommended approach is to use sorting algorithms provided by STL (std::sort).

* Package *Dico*. The code that used the tools provided by that package should be corrected manually.  The recommended approach is to use *NCollection_DataMap* and *NCollection_IndexedDataMap* classes.

@subsection upgrade_710_changed_methods Other changes

The following classes have been changed:

* *BVH_Sorter* class has become abstract. The list of arguments of both  *Perform* methods has been changed and the methods became pure virtual.
* *Extrema_FuncExtPS* has been renamed to *Extrema_FuncPSNorm*.
* The default constructor and the constructor taking a point and a surface have been removed from class *Extrema_GenLocateExtPS*. Now the only constructor takes the surface and optional tolerances in U and V directions. The new method *Perform* takes the point with the start solution and processes it. The class has become not assignable and not copy-constructable.
* Constructors with arguments *(const gp_Ax22d& D, const gp_Pnt2d& F)* have been removed from *GCE2d_MakeParabola*, *gce_MakeParab2d* and *gp_Parab2d*.  The objects created with some constructors of class *gp_Parab2d*  may differ from the previous version (see the comments in *gp_Parab2d.hxx*). The result returned by *gp_Parab2d::Directrix()* method has an opposite direction in comparison with the previous OCCT versions.
* *BRepTools_Modifier* class now has two modes of work. They are defined by the boolean parameter *MutableInput*, which is turned off by default. This means that the algorithm always makes a copy of a sub-shape (e.g. vertex) if its tolerance is to be increased in the output shape. The old mode corresponds to *MutableInput* turned on. This change may impact an application if it implements a class derived from *BRepTools_Modifier*.
* The second parameter *theIsOuterWire* of method *ShapeAnalysis_Wire::CheckSmallArea* has been removed.
* In class *GeomPlate_CurveConstraint*, two constructors taking boundary curves of different types have been replaced with one constructor taking the curve of an abstract type.
* The last optional argument *RemoveInvalidFaces* has been removed from the constructor of class  *BRepOffset_MakeOffset* and method *Initialize*.
* The public method *BOPDS_DS::VerticesOnIn* has been renamed into *SubShapesOnIn* and the new output parameter *theCommonPB* has been added.

@section upgrade_occt720 Upgrade to OCCT 7.2.0

@subsection upgrade_720_removed Removed features

The following obsolete features have been removed:
* *AIS_InteractiveContext::PreSelectionColor()*, *DefaultColor()*, *WasCurrentTouched()*, *ZDetection()*.
  These properties were unused, and therefore application should remove occurrences of these methods.
* *AIS_InteractiveObject::SelectionPriority()*.
  These property was not implemented.
* The class *LocOpe_HBuilder* has been removed as obsolete.
* The package *TestTopOpe* has been removed;
* The package *TestTopOpeDraw* has been removed;
* The package *TestTopOpeTools* has been removed.
* The packages *QANewModTopOpe*, *QANewBRepNaming* and *QANewDBRepNaming* have been removed as containing obsolete features.
* The following methods of the *IntPolyh_Triangle* class have been removed as unused:
  - *CheckCommonEdge*
  - *SetEdgeandOrientation*
  - *MultipleMiddleRefinement2*.
* The method *IntPolyh_Triangle::TriangleDeflection* has been renamed to *IntPolyh_Triangle::ComputeDeflection*.
* The following methods of the *IntPolyh_MaillageAffinage* class have been removed as unused:
  - *LinkEdges2Triangles*;
  - *TriangleEdgeContact2*;
  - *StartingPointsResearch2*;
  - *NextStartingPointsResearch2*;
  - *TriangleComparePSP*;
  - *StartPointsCalcul*.
* The method PerformAdvanced of the *ShapeConstruct_ProjectCurveOnSurface* class has been removed as unused.
* The method Perform of the *ShapeConstruct_ProjectCurveOnSurface* class is modified:
  - input arguments *continuity*, *maxdeg*, *nbinterval* have been removed as unused;
  - input arguments *TolFirst*, *TolLast* have been added at the end of arguments' list.
* Typedefs Quantity_Factor, Quantity_Parameter, Quantity_Ratio, Quantity_Coefficient, Quantity_PlaneAngle, Quantity_Length, V3d_Parameter and V3d_Coordinate have been removed; Standard_Real should be used instead.

@subsection upgrade_occt720_reshape_oriented_removed Corrections in BRepOffset API

In classes *BRepTools_ReShape* and *ShapeBuild_ReShape*, the possibility to process shapes different only by orientation in different ways has been removed.
Thus methods *Remove()* and *Replace()* do not have any more the last argument 'oriented'; they work always as if *Standard_False* was passed before (default behavior).
Methods *ModeConsiderLo()* and *Apply()* with three arguments have been removed.

@subsection upgrade_occt720_correction_of_Offset_API Corrections in BRepOffset API

Class *BRepOffsetAPI_MakeOffsetShape*:
* *BRepOffsetAPI_MakeOffsetShape::BRepOffsetAPI_MakeOffsetShape()* - constructor with parameters has been deleted.
* *BRepOffsetAPI_MakeOffsetShape::PerformByJoin()* - method has been added. This method is old algorithm behaviour. 

The code below shows new calling procedure:
~~~~{.cpp}
    BRepOffsetAPI_MakeOffsetShape anOffsetMaker;
    anOffsetMaker.PerformByJoin(Shape, OffsetValue, Tolerance);
    NewShape = anOffsetMaker.Shape();
~~~~

Class *BRepOffsetAPI_MakeThickSolid*:
* *BRepOffsetAPI_MakeThickSolid::BRepOffsetAPI_MakeThickSolid()* - constructor with parameters has been deleted.
* *BRepOffsetAPI_MakeThickSolid::MakeThickSolidByJoin()* - method has been added. This method is old algorithm behaviour. 

The code below shows new calling procedure:
~~~~{.cpp}
    BRepOffsetAPI_MakeThickSolid aBodyMaker;
    aBodyMaker.MakeThickSolidByJoin(myBody, facesToRemove, -myThickness / 50, 1.e-3);
    myBody = aBodyMaker.Shape();
~~~~

@subsection upgrade_720_highlight Highlight style

Management of highlight attributes has been revised and might require modifications from application side:
* New class *Graphic3d_PresentationAttributes* defining basic presentation attributes has been introduced.
  It's definition includes properties previously defined by class Graphic3d_HighlightStyle (*Color*, *Transparency*),
  and new properties (*Display mode*, *ZLayer*, optional *FillArea aspect*).
* Class *Prs3d_Drawer* now inherits class *Graphic3d_PresentationAttributes*.
  So that overall presentation attributes are now split into two parts - Basic attributes and Detailed attributes.
* Class *Graphic3d_HighlightStyle* has been dropped.
  It is now defined as a typedef to *Prs3d_Drawer*.
  Therefore, highlight style now also includes not only Basic presentation attributes, but also Detailed attributes
  which can be used by custom presentation builders.
* Highlighting style defined by class *Graphic3d_PresentationAttributes* now provides more options:
  - *Graphic3d_PresentationAttributes::BasicFillAreaAspect()* property providing complete Material definition.
    This option, when defined, can be used instead of the pair Object Material + Highlight Color.
  - *Graphic3d_PresentationAttributes::ZLayer()* property specifying the Layer where highlighted presentation should be shown.
    This property can be set to Graphic3d_ZLayerId_UNKNOWN, which means that ZLayer of main presentation should be used instead.
  - *Graphic3d_PresentationAttributes::DisplayMode()* property specifying Display Mode for highlight presentation.
* Since Highlight and Selection styles within *AIS_InteractiveContext* are now defined by *Prs3d_Drawer* inheriting from *Graphic3d_PresentationAttributes*,
  it is now possible to customize default highlight attributes like *Display Mode* and *ZLayer*, which previously could be defined only on Object level.
* Properties *Prs3d_Drawer::HighlightStyle()* and *Prs3d_Drawer::SelectionStyle()* have been removed.
  Instead, *AIS_InteractiveObject* now defines *DynamicHilightAttributes()* for dynamic highlighting in addition to *HilightAttributes()* used for highlighting in selected state.
  Note that *AIS_InteractiveObject::HilightAttributes()* and *AIS_InteractiveObject::DynamicHilightAttributes()* override highlighting properties for both - entire object and for part coming from decomposition.
  This includes Z-layer settings, which will be the same when overriding properties through AIS_InteractiveObject, while *AIS_InteractiveContext::HighlightStyle()* allows customizing properties for local and global selection independently
  (with Graphic3d_ZLayerId_Top used for dynamic highlighting of entire object and Graphic3d_ZLayerId_Topmost for dynamic highlighting of object part by default).
* The following protected fields have been removed from class *AIS_InteractiveObject*:
  - *myOwnColor*, replaced by *myDrawer->Color()*
  - *myTransparency*, replaced by *myDrawer->Transparency()*
  - *myZLayer*, replaced by *myDrawer->ZLayer()*
* The method *PrsMgr_PresentationManager::Unhighlight()* taking Display Mode as an argument has been marked deprecated.
  Implementation now performs unhighlighting of all highlighted presentation mode.
* The methods taking/returning *Quantity_NameOfColor* (predefined list of colors) and duplicating methods operating with *Quantity_Color* (definition of arbitrary RGB color) in AIS have been removed.
  *Quantity_Color* should be now used instead.

@subsection upgrade_720_implicit_viewer_update Elimination of implicit 3D Viewer updates

Most AIS_InteractiveContext methods are defined with a flag to update viewer immediately or not.
Within previous version of OCCT, this argument had default value TRUE.
While immediate viewer updates are useful for beginners (the result is displayed as soon as possible),
this approach is inefficient for batch viewer updates, and having default value as TRUE
lead to non-intended accidental updates which are difficult to find.

To avoid such issues, the interface has been modified and default value has been removed.
Therefore, old application code should be updated to set the flag theToUpdateViewer explicitly
to desired value (TRUE to preserve old previous behavior), if it was not already set.

The following AIS_InteractiveContext methods have been changed:
  Display, Erase, EraseAll, DisplayAll, EraseSelected, DisplaySelected, ClearPrs, Remove, RemoveAll, Hilight,
  HilightWithColor, Unhilight, Redisplay, RecomputePrsOnly, Update, SetDisplayMode, UnsetDisplayMode, SetColor,
  UnsetColor, SetWidth, UnsetWidth, SetMaterial, UnsetMaterial, SetTransparency, UnsetTransparency,
  SetLocalAttributes, UnsetLocalAttributes, SetPolygonOffsets, SetTrihedronSize, SetPlaneSize, SetPlaneSize,
  SetDeviationCoefficient, SetDeviationAngle, SetAngleAndDeviation, SetHLRDeviationCoefficient,
  SetHLRDeviationAngle, SetHLRAngleAndDeviation, SetSelectedAspect, MoveTo, Select, ShiftSelect, SetSelected,
  UpdateSelected, AddOrRemoveSelected, HilightSelected, UnhilightSelected, ClearSelected, ResetOriginalState,
  SubIntensityOn, SubIntensityOff, FitSelected, EraseGlobal, ClearGlobal, ClearGlobalPrs.

In addition, the API for immediate viewer update has been removed from V3d_View and Graphic3d_StructureManager classes
(enumerations *Aspect_TypeOfUpdate* and *V3d_TypeOfUpdate*):
  V3d::SetUpdateMode(), V3d::UpdateMode(), Graphic3d_StructureManager::SetUpdateMode(), Graphic3d_StructureManager::UpdateMode().

The argument theUpdateMode has been removed from methods Graphic3d_CView::Display(), Erase(), Update().
Method Graphic3d_CView::Update() does not redraw the view and does not re-compute structures anymore.

The following Grid management methods within class V3d_Viewer do not implicitly redraw the viewer:
  ActivateGrid, DeactivateGrid, SetRectangularGridValues, SetCircularGridValues,
  RectangularGridGraphicValues, CircularGridGraphicValues, SetPrivilegedPlane, DisplayPrivilegedPlane.

@subsection upgrade_720_v3d_colorname Elimination of Quantity_NameOfColor from TKV3d interface classes

The duplicating interface methods accepting *Quantity_NameOfColor* (in addition to methods accepting *Quantity_Color*) of TKV3d toolkit have been removed.
In most cases this change should be transparent, however applications implementing such interface methods should also remove them
(compiler will automatically highlight this issue for methods marked with `override` keyword).

@subsection upgrade_720_Result_Of_BOP_On_Containers Result of Boolean operations on containers

* The result of Boolean operations on arguments of collection types (WIRE/SHELL/COMPSOLID) is now filtered from duplicating containers.

@subsection upgrade_720_changes_methods Other changes

* *MMgt_TShared* class definition has been removed - Standard_Transient should be used instead (MMgt_TShared is marked as deprecated typedef of Standard_Transient for smooth migration).
* Class GeomPlate_BuildPlateSurface accepts base class Adaptor3d_HCurve (instead of inherited Adaptor3d_HCurveOnSurface accepted earlier).
* Types GeomPlate_Array1OfHCurveOnSurface and GeomPlate_HArray1OfHCurveOnSurface have been replaced with GeomPlate_Array1OfHCurve and GeomPlate_HArray1OfHCurve correspondingly (accept base class Adaptor3d_HCurve instead of Adaptor3d_HCurveOnSurface).
* Enumeration *Image_PixMap::ImgFormat*, previously declared as nested enumeration within class *Image_PixMap*, has been moved to global namespace as *Image_Format* following OCCT coding rules.
  The enumeration values have suffix Image_Format_ and preserve previous name scheme for easy renaming of old values - e.g. Image_PixMap::ImgGray become Image_Format_Gray.
  Old definitions are preserved as deprecated aliases to the new ones;
* Methods *Image_PixMap::PixelColor()* and *Image_PixMap::SetPixelColor()* now take/return Quantity_ColorRGBA instead of Quantity_Color/NCollection_Vec4.
* The method BOPAlgo_Builder::Origins() returns BOPCol_DataMapOfShapeListOfShape instead of BOPCol_DataMapOfShapeShape.
* The methods BOPDS_DS::IsToSort(const occ::handle<BOPDS_CommonBlock>&, Standard_Integer&) and BOPDS_DS::SortPaveBlocks(const occ::handle<BOPDS_CommonBlock>&) have been removed. The sorting is now performed during the addition of the Pave Blocks into Common Block.
* The methods BOPAlgo_Tools::MakeBlocks() and BOPAlgo_Tools::MakeBlocksCnx() have been replaced with the single template method BOPAlgo_Tools::MakeBlocks(). The chains of connected elements are now stored into the list of list instead of data map.
* The methods BOPAlgo_Tools::FillMap() have been replaced with the single template method BOPAlgo_Tools::FillMap().
* Package BVH now uses opencascade::handle instead of NCollection_Handle (for classes BVH_Properties, BVH_Builder, BVH_Tree, BVH_Object).
  Application code using BVH package directly should be updated accordingly.
* AIS_Shape now computes UV texture coordinates for AIS_Shaded presentation in case if texture mapping is enabled within Shaded Attributes.
  Therefore, redundant class *AIS_TexturedShape is now deprecated* - applications can use *AIS_Shape* directly (texture mapping should be defined through AIS_Shape::Attributes()).
* Methods for managing active texture within OpenGl_Workspace class (ActiveTexture(), DisableTexture(), EnableTexture()) have been moved to *OpenGl_Context::BindTextures()*.

@subsection upgrade_720_BOP_DataStructure BOP - Pairs of interfering indices

* The classes *BOPDS_PassKey* and *BOPDS_PassKeyBoolean* are too excessive and not used any more in Boolean Operations. To replace them the new *BOPDS_Pair* class has been implemented. Thus:
  - The method *BOPDS_DS::Interferences()* now returns the *BOPDS_MapOfPair*;
  - The method *BOPDS_Iterator::Value()* takes now only two parameters - the indices of interfering sub-shapes.

@subsection upgrade_720_Removal_Of_Old_Boolean_Operations_Draw Removal of the Draw commands based on old Boolean operations

* The commands *fubl* and *cubl* have been removed. The alternative for these commands are the commands *bfuseblend* and *bcutblend* respectively.
* The command *ksection* has been removed. The alternative for this command is the command *bsection*.

@subsection upgrade_720_Change_Of_FaceFace_Intersection Change of Face/Face intersection in Boolean operations

* Previously, the intersection tolerance for all section curves between pair of faces has been calculated as the maximal tolerance among all curves.
  Now, each curve has its own valid tolerance calculated as the maximal deviation of the 3D curve from its 2D curves or surfaces in case there are no 2D curves.
* The methods *IntTools_FaceFace::TolReached3d()*, *IntTools_FaceFace::TolReal()* and *IntTools_FaceFace::TolReached2d()* have been removed.
* Intersection tolerances of the curve can be obtained from the curve itself:
  - *IntTools_Curve::Tolerance()* - returns the valid tolerance for the curve;
  - *IntTools_Curve::TangentialTolerance()* - returns the tangential tolerance, which reflects the size of the common between faces.
* 2d tolerance (*IntTools_FaceFace::TolReached2d()*) has been completely removed from the algorithm as unused.


@subsection upgrade_720_persistence Restore OCCT 6.9.1 persistence

The capability of reading / writing files in old format using *Storage_ShapeSchema* functionality from OCCT 6.9.1 has been restored in OCCT 7.2.0.

One can use this functionality in two ways:
- invoke DRAW Test Harness commands *fsdread / fsdwrite* for shapes;
- call *StdStorage* class *Read / Write* functions in custom code.

The code example below demonstrates how to read shapes from a storage driver using *StdStorage* class.

~~~~{.cpp}
// aDriver should be created and opened for reading
occ::handle<StdStorage_Data> aData;

// Read data from the driver
// StdStorage::Read creates aData instance automatically if it is null
Storage_Error anError = StdStorage::Read(*aDriver, aData);
if (anError != Storage_VSOk)
{
  // Error processing
}

// Get root objects
occ::handle<StdStorage_RootData> aRootData = aData->RootData();
occ::handle<NCollection_HSequence<occ::handle<StdStorage_Root>>> aRoots = aRootData->Roots();
if (!aRoots.IsNull())
{
  // Iterator over the sequence of root objects
  for (NCollection_HSequence<occ::handle<StdStorage_Root>>::Iterator anIt(*aRoots); anIt.More(); anIt.Next())
  {
    occ::handle<StdStorage_Root>& aRoot = anIt.ChangeValue();
	// Get a persistent root's object
    occ::handle<StdObjMgt_Persistent> aPObject = aRoot->Object();
    if (!aPObject.IsNull())
    {
      occ::handle<ShapePersistent_TopoDS::HShape> aHShape = occ::down_cast<ShapePersistent_TopoDS::HShape>(aPObject);
      if (aHShape) // Downcast to an expected type to import transient data
      {
        TopoDS_Shape aShape = aHShape->Import();
        shapes.Append(aShape);
      }
    }
  }
}
~~~~

The following code demonstrates how to write shapes in OCCT 7.2.0 using *StdStorage* class.

~~~~{.cpp}
// Create a file driver
NCollection_Handle<Storage_BaseDriver> aFileDriver(new FSD_File());

// Try to open the file driver for writing
try
{
  OCC_CATCH_SIGNALS
  PCDM_ReadWriter::Open (*aFileDriver, TCollection_ExtendedString(theFilename), Storage_VSWrite);
}
catch (Standard_Failure& e)
{
  // Error processing
}

// Create a storage data instance
occ::handle<StdStorage_Data> aData = new StdStorage_Data();
// Set an axiliary application name (optional)
aData->HeaderData()->SetApplicationName(TCollection_ExtendedString("Application"));

// Provide a map to track sharing
NCollection_DataMap<occ::handle<Standard_Transient>, occ::handle<StdObjMgt_Persistent>> aMap;
// Iterator over a collection of shapes
for (int i = 1; i <= shapes.Length(); ++i)
{
  TopoDS_Shape aShape = shapes.Value(i);
  // Translate a shape to a persistent object
  occ::handle<ShapePersistent_TopoDS::HShape> aPShape =
    ShapePersistent_TopoDS::Translate(aShape, aMap, ShapePersistent_WithTriangle);
  if (aPShape.IsNull())
  {
    // Error processing
  }

  // Construct a root name
  TCollection_AsciiString aName = TCollection_AsciiString("Shape_") + i;

  // Add a root to storage data
  occ::handle<StdStorage_Root> aRoot = new StdStorage_Root(aName, aPShape);
  aData->RootData()->AddRoot(aRoot);
}

// Write storage data to the driver
Storage_Error anError = StdStorage::Write(*aFileDriver, aData);
if (anError != Storage_VSOk)
{
  // Error processing
}
~~~~

@subsection upgrade_720_Change_In_BRepLib_MakeFace_Algo Change in BRepLib_MakeFace algorithm

Previously, *BRepLib_MakeFace* algorithm changed orientation of the source wire in order to avoid creation of face as a hole (i.e. it is impossible to create the entire face as a hole; the hole can be created in context of another face only). New algorithm does not reverse the wire if it is open. Material of the face for the open wire will be located on the left side from the source wire.

@subsection upgrade_720_Change_In_BRepFill_OffsetWire Change in BRepFill_OffsetWire algorithm

From now on, the offset  will always be directed to the outer region in case of positive offset value and to the inner region in case of negative offset value.
Inner/Outer region for an open wire is defined by the following rule:
when we go along the wire (taking into account edges orientation) the outer region will be on the right side, the inner region will be on the left side.
In case of a closed wire, the inner region will always be inside the wire (at that, the edges orientation is not taken into account).

@subsection upgrade_720_Change_In_GeomAdaptor_Curve Change in Geom(2d)Adaptor_Curve::IsPeriodic

Since 7.2.0 version, method *IsPeriodic()* returns the corresponding status of periodicity of the basis curve regardless of closure status of the adaptor curve (see method *IsClosed()*).
Method *IsClosed()* for adaptor can return false even on periodic curve, in the case if its parametric range is not full period, e.g. for adaptor on circle in range [0, @f$ \pi @f$].
In previous versions, *IsPeriodic()* always returned false if *IsClosed()* returned false.

@subsection upgrade_720_UnifySameDomain Change in algorithm ShapeUpgrade_UnifySameDomain

The history of the changing of the initial shape was corrected:
* all shapes created by the algorithm are considered as modified shapes instead of generated ones;
* method Generated was removed and its calls should be replaced by calls of method History()->Modified.

@subsection upgrade_720_Change_In_RWStl Changes in STL Reader / Writer

Class RWStl now uses class Poly_Triangulation for storing triangular mesh instead of StlMesh data classes; the latter have been removed.

@subsection upgrade_720_New_Error_Warning_system_in_BOA Refactoring of the Error/Warning reporting system in Boolean Component

The Error/Warning reporting system of the algorithms in Boolean Component (in all BOPAlgo_* and BRepAlgoAPI_* algorithms) has been refactored.
The methods returning the status of errors and warnings of the algorithms (ErrorStatus() and WarningStatus()) have been removed.
Instead use methods HasErrors() and HasWarnings() to check for presence of errors and warnings, respectively.
The full list of errors and warnings, with associated data such as problematic sub-shapes, can be obtained by method GetReport().

@section upgrade_occt721 Upgrade to OCCT 7.2.1

@subsection upgrade_721_Changes_In_USD Changes in ShapeUpgrade_UnifySameDomain

The following public methods in the class ShapeUpgrade_UnifySameDomain became protected:
* *UnifyFaces*
* *UnifyEdges*

The following public method has been removed:
* *UnifyFacesAndEdges*

@subsection upgrade_721_Move_BuildPCurveForEdgeOnPlane Moving BuildPCurveForEdgeOnPlane from BOPTools_AlgoTools2D to BRepLib

The methods BuildPCurveForEdgeOnPlane and BuildPCurveForEdgesOnPlane have been moved from the class BOPTools_AlgoTools2D
to the more lower level class BRepLib.

@subsection upgrade_721_removed Removed features

The following obsolete features have been removed:
* The package BOPCol has been fully removed:
  - *BOPCol_BaseAllocator* is replaced with *occ::handle<NCollection_BaseAllocator>*;
  - *BOPCol_BoxBndTree* is replaced with *BOPTools_BoxBndTree*;
  - *BOPCol_Box2DBndTree* is removed as unused;
  - *BOPCol_DataMapOfIntegerInteger* is replaced with *TColStd_DataMapOfIntegerInteger*;
  - *BOPCol_DataMapOfIntegerListOfInteger* is replaced with *TColStd_DataMapOfIntegerListOfInteger*;
  - *BOPCol_DataMapOfIntegerListOfShape* is replaced with *TopTools_DataMapOfIntegerListOfShape*;
  - *BOPCol_DataMapOfIntegerMapOfInteger.hxx* is removed as unused;
  - *BOPCol_DataMapOfIntegerReal* is replaced with *TColStd_DataMapOfIntegerReal*;
  - *BOPCol_DataMapOfIntegerShape* is replaced with *TopTools_DataMapOfIntegerShape*;
  - *BOPCol_DataMapOfShapeBox* is replaced with *TopTools_DataMapOfShapeBox*;
  - *BOPCol_DataMapOfShapeInteger* is replaced with *TopTools_DataMapOfShapeInteger*;
  - *BOPCol_DataMapOfShapeListOfShape* is replaced with *TopTools_DataMapOfShapeListOfShape*;
  - *BOPCol_DataMapOfShapeReal* is replaced with *TopTools_DataMapOfShapeReal*;
  - *BOPCol_DataMapOfShapeShape* is replaced with *TopTools_DataMapOfShapeShape*;
  - *BOPCol_DataMapOfTransientAddress* is removed as unused;
  - *BOPCol_IndexedDataMapOfIntegerListOfInteger* is removed as unused;
  - *BOPCol_IndexedDataMapOfShapeBox* is removed as unused;
  - *BOPCol_IndexedDataMapOfShapeInteger* is removed as unused;
  - *BOPCol_IndexedDataMapOfShapeListOfShape* is replaced with *TopTools_IndexedDataMapOfShapeListOfShape*;
  - *BOPCol_IndexedDataMapOfShapeReal* is removed as unused;
  - *BOPCol_IndexedDataMapOfShapeShape* is replaced with *TopTools_IndexedDataMapOfShapeShape*;
  - *BOPCol_IndexedMapOfInteger* is replaced with *TColStd_IndexedMapOfInteger*;
  - *BOPCol_IndexedMapOfOrientedShape* is replaced with *TopTools_IndexedMapOfOrientedShape*;
  - *BOPCol_IndexedMapOfShape* is replaced with *TopTools_IndexedMapOfShape*;
  - *BOPCol_ListOfInteger* is replaced with *TColStd_ListOfInteger*;
  - *BOPCol_ListOfListOfShape* is replaced with *TopTools_ListOfListOfShape*;
  - *BOPCol_ListOfShape* is replaced with *TopTools_ListOfShape*;
  - *BOPCol_MapOfInteger* is replaced with *TColStd_MapOfInteger*;
  - *BOPCol_MapOfOrientedShape* is replaced with *TopTools_MapOfOrientedShape*;
  - *BOPCol_MapOfShape* is replaced with *TopTools_MapOfShape*;
  - *BOPCol_PListOfInteger* is removed as unused;
  - *BOPCol_PInteger* is removed as unused
  - *BOPCol_SequenceOfPnt2d* is replaced with *TColgp_SequenceOfPnt2d*;
  - *BOPCol_SequenceOfReal* is replaced with *TColStd_SequenceOfReal*;
  - *BOPCol_SequenceOfShape* is replaced with *TopTools_SequenceOfShape*;
  - *BOPCol_Parallel* is replaced with *BOPTools_Parallel*;
  - *BOPCol_NCVector* is replaced with *NCollection_Vector*;
* The class *BOPDS_PassKey* and containers for it have been removed as unused.
* The unused containers from *IntTools* package have been removed:
  - *IntTools_DataMapOfShapeAddress* is removed as unused;
  - *IntTools_IndexedDataMapOfTransientAddress* is removed as unused;
* The container *BiTgte_DataMapOfShapeBox* is replaced with *TopTools_DataMapOfShapeBox*;
* The class *BOPTools* has been removed as duplicate of the class *TopExp*;
* The method *BOPAlgo_Builder::Splits()* has been removed as excessive. The method *BOPAlgo_Builder::Images()* can be used instead.
* The method *BOPTools_AlgoTools::CheckSameGeom()* has been removed as excessive. The method *BOPTools_AlgoTools::AreFacesSameDomain()* can be used instead.

@section upgrade_occt730 Upgrade to OCCT 7.3.0

@subsection upgrade_730_lights Light sources

Multiple changes have been applied to lights management within *TKV3d* and *TKOpenGl*:
* *V3d_Light* class is now an alias to *Graphic3d_CLight*.
  *Graphic3d_CLight* is now a Handle class with refactored methods for managing light source parameters.
  Most methods of *V3d_Light* sub-classes have been preserved to simplify porting.
* Obsolete debugging functionality for drawing a light source has been removed from *V3d_Light*.
  Methods and constructors that take parameters for debug display and do not affect the light definition itself have also been removed.
* Light constructors taking *V3d_Viewer* have been marked as deprecated.
  Use method *AddLight()* of the class *V3d_Viewer* or *V3d_View* to add new light sources to a scene or a single view, respectively.
* The upper limit of 8 light sources has been removed.
* The classes for specific light source types: *V3d_AmbientLight, V3d_DirectionalLight, V3d_PositionalLight* and *V3d_SpotLight* have been preserved, but it is now possible to define the light of any type by creating base class *Graphic3d_CLight* directly. The specific classes only hide unrelated light properties depending on the type of light source.
* It is no more required to call *V3d_Viewer::UpdateLights()* after modifying the properties of light sources (color, position, etc.)

@subsection upgrade_730_shadingmodels Shading Models

*Graphic3d_AspectFillArea3d* has been extended by a new property *ShadingModel()*, which previously has been defined globally for the entire View.

Previously, a triangle array without normal vertex attributes was implicitly considered as unshaded,
but now such array will be shaded using *Graphic3d_TOSM_FACET* model (e.g. by computing per-triangle normals).
Therefore, *Graphic3d_TOSM_UNLIT* should be explicitly specified to disable shading of triangles array.
Alternatively, a material without reflectance properties can be used to disable shading (as before).

@subsection upgrade_730_tkopengl Custom low-level OpenGL elements

The following API changes should be considered while porting custom *OpenGl_Element* objects:
* *OpenGl_ShaderManager::BindFaceProgram()*, *BindLineProgram()*, *BindMarkerProgram()* now take enumeration arguments instead of Boolean flags.

@subsection upgrade_730_BOPAlgo_Section Changes in BOPAlgo_Section

The public method *BuildSection()* in the class *BOPAlgo_Section* has become protected. The methods *Perform()* or *PerformWithFiller()* should be called for construction of the result of SECTION operation.

@subsection upgrade_730_BRepAdaptor_CompCurve Changes in BRepAdaptor_CompCurve

The method *BRepAdaptor_CompCurve::SetPeriodic* has been eliminated.
Since the new version, the method *BRepAdaptor_CompCurve::IsPeriodic()* will always return FALSE. Earlier, it could return TRUE in case if the wire contained only one edge based on a periodic curve. 

@subsection upgrade_730_removed Removed features
* The methods *SetDeflection*, *SetEpsilonT*, *SetDiscretize* of the class *IntTools_EdgeFace* have been removed as redundant.
* Deprecated functionality *V3d_View::Export()*, related enumerations Graphic3d_ExportFormat, Graphic3d_SortType
  as well as optional dependency from gl2ps library have been removed.

@subsection upgrade_730_BuilderSolid Boolean Operations - Solid Builder algorithm

Previously, the unclassified faces of *BOPAlgo_BuilderSolid* algorithm (i.e. the faces not used for solids creation and located outside of all created solids) were used to form an additional (not closed) solid with INTERNAL orientation.
Since the new version, these unclassified faces are no longer added into the resulting solids. Instead, the @ref specification__boolean_ers "warning" with a list of these faces appears.

The following public methods of the *BOPAlgo_BuilderSolid* class have been removed as redundant:
* *void SetSolid(const TopoDS_Solid& theSolid);*
* *const TopoDS_Solid& Solid() const;*

@subsection upgrade_730_BRepAlgoBO Boolean Operation classes in BRepAlgo are deprecated

The API classes in the package BRepAlgo providing access to old Boolean operations are marked as deprecated:
* BRepAlgo_Fuse
* BRepAlgo_Common
* BRepAlgo_Cut
* BRepAlgo_Section
Corresponding classes from the package BRepAlgoAPI should be used instead.

@subsection upgrade_730_replace_CDM_MessageDriver_interface_by_Message_Messenger Unification of the Error/Warning reporting system of Application Framework

Class *CDM_MessageDriver* and its descendants have been removed; class *Message_Messenger* is used instead in all OCAF packages.
By default, messenger returned by *Message::DefaultMessenger()* is used, thus all messages generated by OCAF are directed in the common message queue of OCCT.

In classes implementing OCAF persistence for custom attributes (those inheriting from *BinMDF_ADriver*, *XmlMDF_ADriver*), uses of method *WriteMessage()* should be replaced by call to method *Send()* of the inherited field *myMessageDriver*. Note that this method takes additional argument indicating the gravity of the message (Trace, Info, Warning, Alarm, or Fail).

Class *Message_PrinterOStream* can be used instead of *CDM_COutMessageDriver* to direct all messages to a stream.
If custom driver class is used in the application, that class shall be reimplemented inheriting from *Message_Printer* instead of *CDM_MessageDriver*.
Method *Send()* should be redefined instead of method *Write()* of *CDM_MessageDriver*.
To use the custom printer in OCAF, it can be either added to default messenger or set into the custom *Message_Messenger* object created in the method *MessageDriver()* of a class inheriting *CDF_Application*.

@section upgrade_occt740 Upgrade to OCCT 7.4.0

@subsection upgrade_740_BRepPrimAPI_MakeRevol Changes in BRepPrimAPI_MakeRevol algorithm
Previously the algorithm could create a shape with the same degenerated edge shared between some faces. Now it is prevented. The algorithm creates the different copy of this edge for each face. The method *Generated(...)* has been changed in order to apply restriction to the input shape: input shape can be only of type VERTEX, EDGE, FACE or SOLID. For input shape of another type the method always returns empty list.

@subsection upgrade_740_removed Removed features
* The following methods of the class *BRepAlgoAPI_BooleanOperation* have been removed as obsolete or replaced:
  - *BuilderCanWork* can be replaced with *IsDone* or *HasErrors* method.
  - *FuseEdges* removed as obsolete.
  - *RefineEdges* replaced with new method *SimplifyResult*.
* The method *ImagesResult* of the class *BOPAlgo_BuilderShape* has been removed as unused. The functionality of this method can be completely replaced by the history methods *Modified* and *IsDeleted*.
* The method *TrackHistory* of the classes *BOPAlgo_RemoveFeatures* and *BRepAlgoAPI_Defeaturing* has been renamed to *SetToFillHistory*.
* The method *GetHistory* of the class *BRepAlgoAPI_Defeaturing* has been renamed to *History*.
* The classes *BRepAlgo_BooleanOperations* and *BRepAlgo_DSAccess* have been removed as obsolete. Please use the BRepAlgoAPI_* classes to perform Boolean operations.
* *BRepAlgo_DataMapOfShapeBoolean* has been removed as unused.
* *BRepAlgo_DataMapOfShapeInterference* has been removed as unused.
* *BRepAlgo_EdgeConnector* has been removed as unused.
* *BRepAlgo_SequenceOfSequenceOfInteger* has been removed as unused.

@subsection upgrade_740_localcontext Local Context removal

Previously deprecated Local Context functionality has been removed from AIS package,
so that related methods have been removed from AIS_InteractiveContext interface:
*HasOpenedContext()*, *HighestIndex()*, *LocalContext()*, *LocalSelector()*, *OpenLocalContext()*, *CloseLocalContext()*,
*IndexOfCurrentLocal()*, *CloseAllContexts()*, *ResetOriginalState()*, *ClearLocalContext()*, *UseDisplayedObjects()*, *NotUseDisplayedObjects()*,
*SetShapeDecomposition()*, *SetTemporaryAttributes()*, *ActivateStandardMode()*, *DeactivateStandardMode()*, *KeepTemporary()*,
*SubIntensityOn()*, *SubIntensityOff()*, *ActivatedStandardModes()*, *IsInLocal()*, *AddOrRemoveSelected()* taking TopoDS_Shape.

A set of deprecated methods previously related to Local Context and now redirecting to other methods has been preserved to simplify porting; they will be removed in next release.

@subsection upgrade_740_geomconvert Changes in behavior of Convert algorithms

Now methods *GeomConvert::ConcatG1*, *GeomConvert::ConcatC1*, *Geom2dConvert::ConcatG1*, *Geom2dConvert::ConcatC1* modify the input argument representing the flag of closedness.

@subsection upgrade_740_selection Changes in selection API and picked point calculation algorithm.

*SelectBasics_PickResult* structure has been extended, so that it now defines a 3D point on the detected entity in addition to Depth value along picking ray.
*SelectMgr_SelectingVolumeManager::Overlap()* methods have been corrected to fill in *SelectBasics_PickResult* structure (depth and 3D point) instead of only depth value, so that custom *Select3D_SensitiveEntity* implementation should be updated accordingly (including *Select3D_SensitiveSet* subclasses).

@subsection upgrade_740_ocafpersistence Document format version management improvement.

Previously Document format version restored by *DocumentRetrievalDriver* was propagated using static methods of the corresponding units (like *MDataStd* or *MNaming*) to static variables of these units and after that became accessible to Drivers of these units.
Now Document format version is available to drivers via *RelocationTable*. The Relocation table now keeps *HeaderData* of the document and a format version can be extracted in the following way: *theRelocTable.GetHeaderData()->StorageVersion()*.
Obsolete methods: *static void SetDocumentVersion (const Standard_Integer DocVersion)* and *static Standard_Integer DocumentVersion()* have been removed from *BinMDataStd*, *BinMNaming*, *XmlMDataStd* and *XmlMNaming*.

@subsection upgrade_740_changed_api_of_brepmesh BRepMesh - revision of the data model

The entire structure of *BRepMesh* component has been revised and separated into several logically connected classes.

In new version, deflection is controlled more accurately, this may be necessary to tune parameters of call of the BRepMesh algorithm on the application side to obtain the same quality of presentation and/or performance as before.

*BRepMesh_FastDiscret* and *BRepMesh_FastDiscretFace* classes have been removed.

The following changes have been introduced in the API of *BRepMesh_IncrementalMesh*, component entry point:
* Due to revised logic, *adaptiveMin* parameter of the constructor has been removed as meaningless;
* *BRepMesh_FastDiscret::Parameters* has been moved to a separate structure called *IMeshTools_Parameters*; the signatures of related methods have been changed correspondingly.

* Interface of *BRepMesh_Delaun* class has been changed.

Example of usage:
Case 1 (explicit parameters):
~~~~{.cpp}
#include <IMeshData_Status.hxx>
#include <IMeshTools_Parameters.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

bool meshing_explicit_parameters()
{
  BRepMesh_IncrementalMesh aMesher (aShape, 0.1, false, 0.5, true);
  const int aStatus = aMesher.GetStatusFlags();
  return !aStatus;
}

bool meshing_new()
{
  IMeshTools_Parameters aMeshParams;
  aMeshParams.Deflection               = 0.1;
  aMeshParams.Angle                    = 0.5;
  aMeshParams.Relative                 = false;
  aMeshParams.InParallel               = true;
  aMeshParams.MinSize                  = Precision::Confusion();
  aMeshParams.InternalVerticesMode     = true;
  aMeshParams.ControlSurfaceDeflection = true;

  BRepMesh_IncrementalMesh aMesher (aShape, aMeshParams);
  const int aStatus = aMesher.GetStatusFlags();
  return !aStatus;
}
~~~~

@subsection upgrade_740_chamfer Changes in API of Chamfer algorithms

Some public methods of the class BRepFilletAPI_MakeChamfer are released from excess arguments:
- method Add for symmetric chamfer now takes only 2 arguments: distance and edge;
- method GetDistAngle now takes only 3 arguments: index of contour, distance and angle.

@subsection upgrade_740_aspects Aspects unification

Fill Area, Line and Marker aspects (classes *Graphic3d_AspectFillArea3d*, *Graphic3d_AspectLine3d*, *Graphic3d_AspectMarker3d* and *Graphic3d_AspectText3d*)
have been merged into new class *Graphic3d_Aspects* providing a single state for rendering primitives of any type.
The old per-primitive type aspect classes have been preserved as sub-classes of *Graphic3d_Aspects* with default values close to the previous behavior.
All aspects except Graphic3d_AspectFillArea3d define Graphic3d_TOSM_UNLIT shading model.

The previous approach with dedicated aspects per primitive type was handy in simplified case, but lead to confusion otherwise.
In fact, drawing points or lines with lighting applied is a valid use case, but only *Graphic3d_AspectFillArea3d* previously defined necessary material properties.

As aspects for different primitive types have been merged, Graphic3d_Group does no more provide per-type aspect properties.
Existing code relying on old behavior and putting interleaved per-type aspects into single Graphic3d_Group should be updated.
For example, the following pseudo-code will not work anymore, because all *SetGroupPrimitivesAspect* calls will setup the same property:
~~~~{.cpp}
occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
aGroup->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
aGroup->SetGroupPrimitivesAspect (myDrawer->LineAspect()->Aspect());    //!< overrides previous aspect

occ::handle<Graphic3d_ArrayOfSegments> aLines = new Graphic3d_ArrayOfSegments (2);
occ::handle<Graphic3d_ArrayOfTriangles> aTris = new Graphic3d_ArrayOfTriangles (3);
aGroup->AddPrimitiveArray (aLines); //!< both arrays will use the same aspect
aGroup->AddPrimitiveArray (aTris);
~~~~

To solve the problem, the code should be modified to either put primitives into dedicated groups (preferred approach), or using *SetPrimitivesAspect* in proper order:
~~~~{.cpp}
occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();

aGroup->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
occ::handle<Graphic3d_ArrayOfTriangles> aTris = new Graphic3d_ArrayOfTriangles (3);
aGroup->AddPrimitiveArray (aTris);

occ::handle<Graphic3d_ArrayOfSegments> aLines = new Graphic3d_ArrayOfSegments (2);
aGroup->SetPrimitivesAspect (myDrawer->LineAspect()->Aspect()); //!< next array will use the new aspect
aGroup->AddPrimitiveArray (aLines);
~~~~

@subsection upgrade_740_materials Material definition

Decomposition of Ambient, Diffuse, Specular and Emissive properties has been eliminated within *Graphic3d_MaterialAspect* definition.
As result, the following methods of *Graphic3d_MaterialAspect* class have been removed: SetReflectionMode(), SetReflectionModeOn(), Ambient(), Diffuse(), Emissive(), Specular(), SetAmbient(), SetDiffuse(), SetSpecular(), SetEmissive().

Previously, computation of final value required the following code:
~~~~{.cpp}
Graphic3d_MaterialAspect theMaterial; Quantity_Color theInteriorColor;
NCollection_Vec3<float> anAmbient (0.0f);
if (theMaterial.ReflectionMode (Graphic3d_TOR_AMBIENT))
{
  anAmbient = theMaterial.MaterialType (Graphic3d_MATERIAL_ASPECT)
            ? (NCollection_Vec3<float> )theInteriorColor           * theMaterial.Ambient()
            : (NCollection_Vec3<float> )theMaterial.AmbientColor() * theMaterial.Ambient();
}
~~~~

New code looks like this:
~~~~{.cpp}
Graphic3d_MaterialAspect theMaterial; Quantity_Color theInteriorColor;
NCollection_Vec3<float> anAmbient = theMaterial.AmbientColor();
if (theMaterial.MaterialType (Graphic3d_MATERIAL_ASPECT)) { anAmbient *= (NCollection_Vec3<float> )theInteriorColor; }
~~~~

Existing code should be updated to:
- Replace Graphic3d_MaterialAspect::SetReflectionModeOff() with setting black color; SetReflectionModeOn() calls can be simply removed.
  R.g. theMaterial.SetAmbientColor(Quantity_NOC_BLACK).
- Replace Graphic3d_MaterialAspect::Ambient(), SetAmbient(), Diffuse(), SetDiffuse(), Specular(), SetSpecular(), Emissive(), SetEmissive() with methods working with pre-multiplied color.
  E.g. theMaterial.SetAmbientColor(Graphic3d_Vec3 (1.0f, 0.0f, 0.0f) * 0.2f).
- Avoid using Graphic3d_MaterialAspect::Color() and SetColor() with non-physical materials (Graphic3d_MATERIAL_ASPECT).
  These materials do not include color definition, because it is taken from Graphic3d_Aspects::InteriorColor() - this has not been changed.
  However, previously it was possible storing the color with SetColor() call and then fetching it with Color() by application code (the rendering ignored this value);
  now SetColor() explicitly ignores call for Graphic3d_MATERIAL_ASPECT materials and Color() returns DiffuseColor() multiplication coefficients.

@subsection upgrade_740_text Changes in Graphic3d_Text and OpenGl_Text API

Parameters of *Text* in *Graphic3d_Group* are moved into a new *Graphic3d_Text* class. *AddText* of *Graphic3d_Group* should be used instead of the previous *Text*.

The previous code:
~~~~{.cpp}
double x, y, z;
theAttachmentPoint.Coord(x,y,z);
theGroup->Text (theText,
                Graphic3d_Vertex(x,y,z),
                theAspect->Height(),
                theAspect->Angle(),
                theAspect->Orientation(),
                theAspect->HorizontalJustification(),
                theAspect->VerticalJustification());
~~~~
should be replaced by the new code:
~~~~{.cpp}
occ::handle<Graphic3d_Text> aText = new Graphic3d_Text (theAspect->Height());
aText->SetText (theText.ToExtString());
aText->SetPosition (theAttachmentPoint);
aText->SetHorizontalAlignment (theAspect->HorizontalJustification());
aText->SetVerticalAlignment (theAspect->VerticalJustification());
theGroup->AddText (aText);
~~~~

*OpenGl_Text* contains *Graphic3d_Text* field.

*OpenGl_TextParam* struct is removed. Constructor and *Init* of *OpenGl_Text* with *OpenGl_TextParam* are also removed.
Instead of using them, change *OpenGl_Text*.

Please, note, that after modifying *OpenGl_Text*, *Reset* of *OpenGl_Text* should be called.

*FormatParams* of *OpenGl_Text* is replaced by *Text*.

@subsection upgrade_740_prsupdate Presentation invalidation

Historically AIS_InteractiveObject provided two independent mechanisms invalidating presentation (asking presentation manager to recompute specific display mode or all modes):

1. *AIS_InteractiveObject::SetToUpdate()*, marking existing presentation for update.
   This is main invalidation API, which is expected to be followed by *AIS_InteractiveContext::Update()* call.
2. *AIS_InteractiveObject::myToRecomputeModes* + *myRecomputeEveryPrs*.
   This is auxiliary invalidation API, used internally by AIS_InteractiveContext::SetColor()/UnsetColor() and similar modification methods.

The latter one has been removed to avoid confusion and unexpected behavior.
In addition, two methods *AIS_InteractiveObject::Update()* have been deprecated in favor of new *AIS_InteractiveObject::UpdatePresentations()* recomputing only invalidated presentations.

Custom presentations implementing interface methods *AIS_InteractiveObject::SetColor()* and others should be revised to use *AIS_InteractiveObject::SetToUpdate()*
or updating presentation without recomputation (see *AIS_InteractiveObject::SynchronizeAspects()* and *AIS_InteractiveObject::replaceAspects()*).

@subsection upgrade_740_interiorstyles Interior styles

* *Aspect_IS_HOLLOW* is now an alias to *Aspect_IS_EMPTY* and does not implicitly enables drawing mesh edges anymore.
  Specify Graphic3d_AspectFillArea3d::SetDrawEdges(true) with Graphic3d_AspectFillArea3d::SetInteriorStyle(Aspect_IS_EMPTY) to get previous behavior of Aspect_IS_HOLLOW style.
* *Aspect_IS_HIDDENLINE* does not implicitly enables drawing mesh edges anymore.
  Specify Graphic3d_AspectFillArea3d::SetDrawEdges(true) with Graphic3d_AspectFillArea3d::SetInteriorStyle(Aspect_IS_HIDDENLINE) to get previous behavior of Aspect_IS_HIDDENLINE style.

@subsection upgrade_740_modedprs PrsMgr and SelectMgr hierarchy clean up

Proxy classes *Prs3d_Presentation*, *PrsMgr_ModedPresentation* and *PrsMgr_Prs* have been removed.
Code iterating through the list of low-level structures AIS_InteractiveObject::Presentations() should be updated to access PrsMgr_Presentation directly.
Forward declarations of *Prs3d_Presentation* should be corrected, since it is now a typedef to *Graphic3d_Structure*.

Proxy classes *SelectBasics_SensitiveEntity* and *SelectBasics_EntityOwner* have been removed - *Select3D_SensitiveEntity* and *SelectMgr_EntityOwner* should be now used directly instead.

@subsection upgrade_740_offset Polygon offset defaults

*Graphic3d_PolygonOffset* default constructor has been corrected to define Units=1 instead of Units=0.
Default polygon offset settings Mode=Aspect_POM_Fill + Factor=1 + Units=1 are intended to push triangulation
(Shaded presentation) a little bit behind of lines (Wireframe and Face Edges)
for reducing z-fighting effect of Shaded+Wireframe combination.
The change in defaults (Units changed from 0 to 1) is intended to cover scenario when camera direction is perpendicular to model plane (like 2D view).

Application observing unexpected visual difference on this change should consider customizing this property within AIS_InteractiveContext default attributes
or on per-presentation basis via *Graphic3d_Aspects::SetPolygonOffset()* methods.

@subsection upgrade_740_zlayer Adding ZLayers in given position

Interface of insertion ZLayer in the viewer has been improved with ability to insert new layer before or after existing one.
Previously undocumented behavior of *V3d_Viewer::AddZlayer()* method has been corrected to insert new layer before *Graphic3d_ZLayerId_Top*.
Applications might need revising their custom layers creation code and specify precisely their order with new methods *V3d_Viewer::InsertLayerBefore()* and *V3d_Viewer::InsertLayerAfter()*.

@subsection upgrade_740_enum_changed Modified enumerations

Applications using integer values of the following enumerations in persistence
should be corrected as these enumerations have been modified:

| Name |
| :----- |
| AIS_TypeOfAttribute |
| Aspect_InteriorStyle |
| Font_FontAspect |

@subsection upgrade_740_geproj Custom defines within env.bat

*env.bat* produced by Visual Studio project generator *genproj.bat* has been modified so that *%CSF_DEFINES%* variable is reset to initial state.
Custom building environment relying on old behavior and setting extra macros within *%CSF_DEFINES%* before env.bat should be updated
to either modify custom.bat or setup new variable *%CSF_DEFINES_EXTRA%* instead.

@subsection upgrade_740_BVH_in_BOP Switching Boolean Operations algorithm to use BVH tree instead of UB tree

Since OCCT 7.4.0 Boolean Operations algorithm uses BVH tree instead of UBTree to find the pairs of entities with interfering bounding boxes.
The following API changes have been made:
* BOPTools_BoxBndTree and BOPTools_BoxBndTreeSelector have been removed. Use the BOPTools_BoxTree and BOPTools_BoxTreeSelector instead.
* BOPTools_BoxSelector::SetBox() method now accepts the BVH_Box instead of Bnd_Box.
* Methods BOPTools_BoxSelector::Reject and BOPTools_BoxSelector::Accept have been removed as unused.
* The RunParallel flag has been removed from the list of parameters of BOPAlgo_Tools::IntersectVertices method. Earlier, it performed selection from the UB tree in parallel mode. Now all interfering pairs are found in one pass, using pair traverse of the same BVH tree.

@subsection upgrade_740_stdnamespace Standard_Stream.hxx no more has "using std::" statements
*Standard_Stream.hxx* header, commonly included by other OCCT header files, does no more add entities from *std namespace* related to streams (like *std::cout*, *std::istream* and others) into global namespace.
The application code relying on this matter should be updated to either specify std namespace explicitly (like std::cout) or add "using std::" statements locally.

@section upgrade_occt750 Upgrade to OCCT 7.5.0

@subsection upgrade_750_srgb_color RGB color definition

OCCT 3D Viewer has been improved to properly perform lighting using in linear RGB color space and then convert result into non-linear gamma-shifted sRGB color space before displaying on display.
This change affects texture mapping, material definition and color definition.

Previously *Quantity_Color* definition was provided with unspecified RGB color space.
In practice, mixed color spaces have been actually used, with non-linear sRGB prevailing in general.
Since OCCT 7.5.0, *Quantity_Color* now specifies that components are defined in linear RGB color space.

This change affects following parts:
* Standard colors defined by *Quantity_NameOfColor* enumeration have been converted into linear RGB values within Quantity_Color construction.
* Application may use new enumeration value *Quantity_TOC_sRGB* for passing/fetching colors in sRGB color space,
  which can be useful for interoperation with color picking widgets (returning 8-bit integer values within [0..255] range)
  or for porting colors constants within old application code without manual conversion.
* *Graphic3d_MaterialAspect* color components are now expected in linear RGB color space,
  and standard OCCT materials within *Graphic3d_NameOfMaterial* enumeration have been updated accordingly.
* Texture mapping now handles new *Graphic3d_TextureRoot::IsColorMap()* for interpreting content in linear RGB or sRGB color space.
  It is responsibility of user specifying this flag correctly. The flag value is TRUE by default.
* Method *Image_PixMap::PixelColor()* has been extended with a new Boolean flag for performing linearization of non-linear sRGB.
  This flag is FALSE by default; application should consider passing TRUE instead for further handling *Quantity_Color* properly as linear RGB values.

@subsection upgrade_750_aspectwindow Aspect_Window interface change

Unexpected const-ness of Aspect_Window::DoResize() method has been removed, so that application classes implementing this interface should be updated accordingly.

@subsection upgrade_750_rename Renaming of types

Enumeration BRepOffset_Type is renamed to ChFiDS_TypeOfConcavity.

@subsection upgrade_750_BRepOffset_MakeOffset change in construction of offset faces

Now by default offset faces of non-planar faces may be planar faces if their originals can be approximated by planes.

@subsection upgrade_750_tkv3d TKV3d/TKService toolkits changes

The following changes could be highlighted while porting:
* *Prs3d::GetDeflection()* has been moved to *StdPrs_ToolTriangulatedShape::GetDeflection()*.
* *Prs3d_ShapeTool* has been moved to *StdPrs_ShapeTool*.
* *StdSelect_ViewerSelector3d* has been moved to *SelectMgr_ViewerSelector3d*.
* *Font_BRepFont* has been moved to *StdPrs_BRepFont*.
* Visualization classes now use *TopLoc_Datum3D* (from *TKMath*) instead of *Geom_Transformation* (from *TKG3d*) as smart pointer to *gp_Trsf*.
  This is rather an internal change, but some applications might need to be updated.

@subsection upgrade_750_hlrangle Prs3d_Drawer deviation angle

Properties Prs3d_Drawer::HLRAngle() and Prs3d_Drawer::HLRDeviationCoefficient() have been removed from classes *Prs3d_Drawer*, *AIS_Shape* and *AIS_InteractiveContext*.
Prs3d_Drawer::DeviationAngle() should be now used instead of Prs3d_Drawer::HLRAngle() and Prs3d_Drawer::DeviationCoefficient() instead of Prs3d_Drawer::HLRDeviationCoefficient().
The default value of Prs3d_Drawer::DeviationAngle() property has been changed from 12 to 20 degrees to match removed Prs3d_Drawer::HLRAngle(), previously used as input for triangulation algorithm.

@subsection upgrade_750_hlrprs Changes in HLR presentation API

Methods computing HLR presentation within *PrsMgr_PresentableObject::Compute()* have been renamed to *PrsMgr_PresentableObject::computeHLR()*
and now accept *Graphic3d_Camera* object instead of removed *Prs3d_Projector*.

@subsection upgrade_750_dimensions Dimension and Relation presentations moved from AIS to PrsDim

Presentation classes displaying Dimensions and Relations have been moved from *AIS* package to *PrsDim*.
Corresponding classes should be renamed in application code (like *AIS_LengthDimension* -> *PrsDim_LengthDimension*).

@subsection upgrade_750_sensitiveEntity Select3D_SensitiveEntity interface change

The method Select3D_SensitiveEntity::NbSubElements() has been changed to be constant. Select3D_SensitiveEntity subclasses at application level should be updated accordingly.


@subsection upgrade_750_Booleans Changes in Boolean operations algorithm

* TreatCompound method has been moved from *BOPAlgo_Tools* to *BOPTools_AlgoTools*. Additionally, the map parameter became optional:
~~~~{.cpp}
void BOPTools_AlgoTools::TreatCompound (const TopoDS_Shape& theS,
                                        NCollection_List<TopoDS_Shape>& theLS,
                                        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>* theMap = nullptr);
~~~~

@subsection upgrade_750_Adaptor2d_OffsetCurve  Offset direction change

Offset direction, which used in class Adaptor2d_OffsetCurve for evaluating values and derivatives of offset curve is unified for offset direction used in class Geom2d_OffsetCurve: now offset direction points to outer ("right") side of base curve instead of the previously used inner ("left") side. Old usage of class in any application should be changed something like that:

Adaptor2d_OffsetCurve aOC(BaseCurve, Offset) --> Adaptor2d_OffsetCurve aOC(BaseCurve, -Offset)

@subsection upgrade_750_ProgressIndicator Change of progress indication API

The progress indication mechanism has been revised to eliminate its weak points in 
previous design (leading to implementation mistakes).
Redesign also allows using progress indicator in multi-threaded algorithms 
in more straight-forward way with minimal overhead.
Note however, that multi-threaded algorithm should pre-allocate per-task 
progress ranges in advance to ensure thread-safety - 
see examples in documentation of class Message_ProgressScope for details.

Classes Message_ProgressSentry and Message_ProgressScale have been removed.
New classes Message_ProgressScope and Message_ProgressRange should be used as main 
API classes to organize progress indication in the algorithms.
Instances of the class Message_ProgressRange are used to pass the progress capability to
nested levels of the algorithm, and an instance of the class Message_ProgressScope is to
be created (preferably as local variable) to manage progress at each level of the algorithm.
The instance of Message_ProgressIndicator is not passed anymore to sub-algorithms.
See documentation of the class Message_ProgressScope for more details and examples.

Methods to deal with progress scopes and to advance progress are removed from class 
Message_ProgressIndicator; now it only provides interface to the application-level progress indicator.
Virtual method Message_ProgressIndicator::Show() has changed its signature and should be 
updated accordingly in descendants of Message_ProgressIndicator.
The scope passed as argument to this method can be used to obtain information on context 
of the current process (instead of calling method GetScope() in previous implementation).
Methods Show(), UserBreak(), and Reset() are made protected in class Message_ProgressIndicator; 
methods More() or UserBreak() of classes Message_ProgressScope or Message_ProgressRange should 
be used to know if the cancel event has come.
See documentation of the class Message_ProgressIndicator for more details and implementation 
of Draw_ProgressIndicator for an example.

Let's take a look onto typical algorithm using an old API:
@code
class MyAlgo
{
public:
  //! Algorithm entry point taking an optional Progress Indicator.
  bool Perform (const TCollection_AsciiString& theFileName,
                const occ::handle<Message_ProgressIndicator>& theProgress = occ::handle<Message_ProgressIndicator>())
  {
    Message_ProgressSentry aPSentry (theProgress, (TCollection_AsciiString("Processing ") + theFileName).ToCString(), 2);
    {
      Message_ProgressSentry aPSentry1 (theProgress, "Stage 1", 0, 153, 1);
      for (int anIter = 0; anIter < 153; ++anIter, aPSentry1.Next())
      { 
        if (!aPSentry1.More()) { return false; } 
        // do some job here...
      }
    }
    aPSentry.Next();
    {
      perform2 (theProgress);
    }
    aPSentry.Next();
    bool wasAborted = !theProgress.IsNull() && theProgress->UserBreak();
    return !wasAborted;
  }

private:
  //! Nested sub-algorithm taking Progress Indicator.
  bool perform2 (const occ::handle<Message_ProgressIndicator>& theProgress)
  {
    Message_ProgressSentry aPSentry2 (theProgress, "Stage 2", 0, 100, 1);
    for (int anIter = 0; anIter < 100 && aPSentry2.More(); ++anIter, aPSentry2.Next()) {}
    return !aPSentry2.UserBreak();
  }
};

// application executing an algorithm
occ::handle<Message_ProgressIndicator> aProgress = new MyProgress();
MyAlgo anAlgo;
anAlgo.Perform ("FileName", aProgress);
@endcode

The following guidance can be used to update such code:
- Replace `const Handle(Message_ProgressIndicator)&` with `const Message_ProgressRange&` 
  in arguments of the methods that support progress indication.
  Message_ProgressIndicator object should be now created only at place where application starts algorithms.
- Replace `Message_ProgressSentry` with `Message_ProgressScope`.
  Take note that Message_ProgressScope has less arguments (no "minimal value").
  In other aspects, Message_ProgressScope mimics an iterator-style interface 
  (with methods More() and Next()) close to the old Message_ProgressSentry (pay attention 
  to extra functionality of Message_ProgressScope::Next() method below).
  Note that method Message_ProgressScope::Close() is equivalent of the method 
  Relieve() of Message_ProgressSentry in previous version.
  Class Message_ProgressSentry is still defined (marked as deprecated) providing
  API more close to old one, and can be still used to reduce porting efforts.
- Each Message_ProgressScope should take the next Range object to work with.
  Within old API, Message_ProgressSentry received the root Progress Indicator 
  object which maintained the sequence of ranges internally.
  Message_ProgressScope in new API takes Message_ProgressRange, which should be
  returned by Message_ProgressScope::Next() method of the parent scope.
  Do not use the same Range passed to the algorithm for all sub-Scopes like 
  it was possible in old API; each range object may be used only once.

Take a look onto ported code and compare with code above to see differences:

@code
class MyAlgo
{
public:
  //! Algorithm entry point taking an optional Progress Range.
  bool Perform (const TCollection_AsciiString& theFileName,
                const Message_ProgressRange& theProgress = Message_ProgressRange())
  {
    Message_ProgressScope aPSentry (theProgress, TCollection_AsciiString("Processing ") + theFileName, 2);
    {
      Message_ProgressScope aPSentry1 (aPSentry.Next(), "Stage 1", 153);
      for (int anIter = 0; anIter < 153; ++anIter, aPSentry1.Next())
      { 
        if (!aPSentry1.More()) { return false; };
        // do some job here...
      }
    }
    {
      perform2 (aPSentry.Next());
    }
    bool wasAborted = aPSentry.UserBreak();
    return !wasAborted;
  }

  //! Nested sub-algorithm taking Progress sub-Range.
  bool perform2 (const Message_ProgressRange& theProgress)
  {
    Message_ProgressScope aPSentry2 (theProgress, "Stage 2", 100);
    for (int anIter = 0; anIter < 100 && aPSentry2.More(); ++anIter, aPSentry2.Next()) {}
    return !aPSentry2.UserBreak();
  }
};

// application executing an algorithm
occ::handle<Message_ProgressIndicator> aProgress = new MyProgress();
MyAlgo anAlgo;
anAlgo.Perform ("FileName", aProgress->Start());
@endcode

@subsection upgrade_750_message_messenger Message_Messenger interface change

Operators << with left argument *Handle(Message_Messenger)*, used to output messages with
a stream-like interface,  have been removed.
This functionality is provided now by separate class *Message_Messenger::StreamBuffer*.
That class contains a stringstream buffer which can be filled using standard stream
operators. The string is sent to a messenger on destruction of the buffer object,
call of its method Flush(), or using operator << with one of ostream manipulators 
(*std::endl, std::flush, std::ends*). Manipulator *Message_EndLine* has been removed,
*std::endl* should be used instead.

New methods *SendFail(), SendAlarm(), SendWarning(), SendInfo()*, and *SendTrace()* are
provided in both *Message_Messenger* class and as static functions in *Message* package
(short-cuts to default messenger), returning buffer object for the output of
corresponding type of the message.

The code that used operator << for messenger, should be ported as follows.

Before the change:
~~~~{.cpp}
  occ::handle<Message_Messenger> theMessenger = ...;
  theMessenger << "Value = " << anInteger << Message_EndLine;
~~~~

After the change, single-line variant:
~~~~{.cpp}
  occ::handle<Message_Messenger> theMessenger = ...;
  theMessenger->SendInfo() << "Value = " << anInteger << std::endl;
~~~~

After the change, extended variant:
~~~~{.cpp}
  occ::handle<Message_Messenger> theMessenger = ...;
  Message_Messenger::StreamBuffer aSender = theMessenger->SendInfo();
  aSender << "Array: [ ";
  for (int i = 0; i < aNb; ++i) { aSender << anArray[i] << " "; }
  aSender << "]" << std::endl; // aSender can be used further for other messages
~~~~

@subsection upgrade_750_message_printer Message_Printer interface change

Previously, sub-classes of *Message_Printer* have to provide a triplet of *Message_Printer::Send()* methods accepting different string representations: TCollection_AsciiString, TCollection_ExtendedString and Standard_CString.
*Message_Printer* interface has been changed, so that sub-classes now have to implement only single method *Message_Printer::send()* accepting TCollection_AsciiString argument and having no Endl flag, which has been removed.
Old three Message_Printer::Send() methods remain defined virtual with unused last argument and redirecting to new send() method by default.

@subsection upgrade_750_prd3d_root Prs3d_Root deprecation

Redundant class Prs3d_Root has been marked as deprecated - Prs3d_Presentation::NewGroup() should be called directly.

@subsection upgrade_750_cdf_session Support of multiple OCAF application instances

Class *CDF_Session* has been removed.
That class was used to store global instance of OCAF application (object of class *CDM_Application* or descendant, typically *TDocStd_Application*).
Global directory of all opened OCAF documents has been removed as well; such directory is maintained now by each instance of the *CDM_Application* class.

This allows creating programs that work with different OCAF documents concurrently in parallel threads,
provided that each thread deals with its own instance of *TDocStd_Application* and documents managed by this instance.

Note that neither *TDocStd_Application* nor *TDocStd_Document* is protected from concurrent access from several threads.
Such protection, if necessary, shall be implemented on the application level.
For an example, access to labels and attributes could be protected by mutex if there is a probability that different threads access the same labels / attributes:
~~~~{.cpp}
  {
    Standard_Mutex::Sentry aSentry (myMainLabelAccess);
    TDF_Label aChildLab = aDocument->Main().NewChild();
    TDataStd_Integer::Set(aChildLab, 0);
  }
~~~~

@subsection upgrade_750_draw_hotkeys Draw Harness hotkeys

Draw Harness hotkeys **W** (Wireframe) and **S** (Shaded) have been re-mapped to **Ctrl+W** and **Ctrl+S**.
Hotkey **A** has been remapped to **Backspace**.
Hotkeys WASD and Arrays are now mapped for walk-through navigation in 3D Viewer.

@subsection upgrade_750_msgfile_utf8 Utf-8 encoding for message files

Message files (with extension .msg) are now expected to be in UTF-8 encoding (unless they have UTF-16 BOM in which case UTF-16 is expected).
This allows using arbitrary Unicode symbols for localization of messages.

Existing message files containing 8-bit characters (previously interpreted as characters from Latin-1 code block) should be converted to UTF-8.

@section upgrade_occt760 Upgrade to OCCT 7.6.0

@subsection upgrade_760_handle_adaptors Simplification of surface/curve adaptor

Interfaces *Adaptor2d_Curve2d*, *Adaptor3d_Curve* and *Adaptor3d_Surface* now inherit Standard_Transient and can be Handle-managed.
No more necessary parallel hiererchy of classes *Adaptor2d_HCurve2d*, *Adaptor3d_HCurve* and *Adaptor3d_HSurface* (generated from generic templates by WOK) has been eliminated.
Existing code using old Handle classes should be updated to:
* Rename occurrences of old names (remove H suffix); upgrade.bat could be used for that purpose.
* Replace redundant calls to previously declared methods .GetCurve2d()/.GetCurve()/.GetSurface() with the common operator->() syntax.
* Pay attention on code calling GetSurface()/GetCurve() methods of removed handle classes. Such places should be replaced with Handle dereference.

@subsection upgrade_760_extendedstring_cout Output of TCollection_ExtendedString to stream

Behavior of the method TCollection_ExtendedString::Print(Standard_OStream&) and corresponding operator << has changed.
Previously it printed all Latin-1 symbols (those in range 0x80-0xff) as '\0' (effectively losing them); symbols above 0xff were converted to hex representation (formatted like XML Numeric Character Reference).
Now all symbols are sent to stream as UTF-8 byte sequences.
Existing code relying on old behavior, if any, shall be rewritten.

@subsection upgrade_760_trimming_surface Trimming surface

Geom_RectangularTrimmedSurface sequentially trimming in U and V directions already no longer loses the first trim.
For example:
~~~~{.cpp}
  occ::handle<Geom_RectangularTrimmedSurface> ST  = new Geom_RectangularTrimmedSurface (Sbase, u1, u2, true); // trim along U
  occ::handle<Geom_RectangularTrimmedSurface> ST1 = new Geom_RectangularTrimmedSurface (ST, v1, v2, false); // trim along V
~~~~
gives different result.
In current version ST1 - surface trimmed only along V, U trim is removed;
After modification ST1 - surface trimmed along U and V, U trim is kept.

@subsection upgrade_760_storageformatversion Storage format version of OCAF document

The methods *XmlLDrivers::StorageVersion()* and *BinLDrivers::StorageVersion()* were removed.
Since now *TDocStd_Document* manipulates the storage format version of a document for both XML and binary file formats.
For this the methods *StorageFormatVersion()* and *ChangeStorageFormatVersion()* were moved from *CDM_Document* to *TDocStd_Document*.
The methods are used to get and set the storage format version of a document.
A new enumeration *TDocStd_FormatVersion* lists the storage format versions of a document. By default, the document uses the latest (current) storage format version.
In order to save a document in an older storage format version, call the method *ChangeStorageFormatVersion()* with one of the values from the enumeration.
This value will be used by storage drivers of a corresponding OCAF file format (XML or binary) and the document will be saved
following the rules of the specified storage format version (corresponding to an older version of Open CASCADE Technology).
This way an application based on an old version of Open CASCADE Technology may read documents saved by new applications (based on newer version of Open CASCADE Technology).

@subsection upgrade_760_createdocument New OCAF document

A new OCAF document may be created only by means of the method *NewDocument()* from CDF_Application (redefined in TDocStd_Application). The methods *CreateDocument()* are deleted in all retrieval drivers.

@subsection upgrade_760_changesInStorageOfShapes Changes in storage of shapes

Information about per-vertex triangulations normals is now stored in BinOCAF and XmlOCAF document,
BRep and Binary BRep Shape formats (only in case of triangulation-only Faces, with no analytical geometry to restore normals).

Versions of formats have been changed (11 for BinOCAF, 10 for XmlOCAF, 4 for BRep Shape and 3 for Binary BRep Shape).
Files written with the new version will not be readable by applications of old versions.

@subsection upgrade_760_changesBinaryFormat Changes in storage of binary document format

All kinds of binary document formats since the new version 12 saved with support of partial reading (sub-set of labels and sub-set of attributes).
For that the shapes data structures are stored with the related NamedShape attributes in the file, not in the particular section in the start of the document.
Also, size allocated for each label is stored in the file. This allows to skip big parts of document in partial reading mode if needed.

As a result, the new binary files become some smaller, but default reading and writing of documents may take some more time (depenging on the environment), up to 15 percents slower in the worse cases.
Backward compatibility (loading of old documents in the newer version) is still fully supported, as well as writing the older versions of the document.

@subsection upgrade_occt760_poly Changes in *Poly* package and *Poly_Triangulation* class

*Poly_Triangulation* does no more provide access to internal array structures: methods Nodes(), ChangeNode(), Triangles(), ChangeTriangle(), UVNodes(), ChangeUVNode(), Normals() have been removed.
Methods of *Poly_Triangulation* for accessing individual nodal properties / triangles by index and implementing copy semantics should be used instead.
The same is applicable to *Poly_PolygonOnTriangulation* interface.

@subsection upgrade_occt760_glsl Custom GLSL programs

Accessors to standard materials have been modified within *Declarations.glsl* (*occFrontMaterial_Diffuse()* -> *occMaterial_Diffuse(bool)* and similar).
Applications defining custom GLSL programs should take into account syntax changes.

@subsection upgrade_occt760_noral_colors Nodal color modulation

Nodal color vertex attribute is now modulated in the same way as a color texture - color is multiplied by material coefficients (diffuse/ambient/specular in case of a common material definition).
Existing code defining nodal colors should be updated to:
- Use *Graphic3d_TOSM_UNLIT* shading model when lighting is not needed.
- Adjust diffuse/ambient material coefficients, which have been previously ignored.
- Remove code multiplying nodal colors, intended to compensate over-brightness due to addition of specular color from material definition, as specular component is now also modulated by a vertex color.

@subsection upgrade_occt760_tkopengles TKOpenGles library

OCCT now provides two separate toolkits - *TKOpenGl* depending on desktop OpenGL and *TKOpenGles* depending on OpenGL ES.
Both libraries can be now built simultaneously on systems providing both APIs (like desktop Linux).

Existing applications depending on OpenGL ES (mobile projects first of all) should be adjusted to link against *TKOpenGles*.
Note that both *TKOpenGl* and *TKOpenGles* keep exporting classes with the same name, so applications should not attempt to link both libraries simultaneously.

@subsection upgrade_occt760_fast_access_to_labels Fast access to OCAF label

Access to an OCAF label via its entry is accelerated. In order to activate it, call *TDF_Data::SetAccessByEntries()*.
The method *TDF_Tool::Label()*, which returns a label by an entry, becomes faster for about 10 .. 20 times.
It has sense for applications, which use an entry as a unique key to access the data in OCAF tree.
Also, the method *TDF_Tool::Entry()*, which returns an entry for a label, is accelerated as well.

@subsection upgrade_occt760_bop_progress_indicator Progress indicator in Boolean operations

Method SetProgressIndicator() has been removed due to Progress indicator mechanism refactoring.
To enable progress indicator and user break in Boolean operations user has to pass progress range as a parameter to Perform or Build method.
For example:
~~~~
occ::handle<Draw_ProgressIndicator> aProgress = new Draw_ProgressIndicator(di, 1);
BRepAlgoAPI_Cut(S1, S2, aProgress->Start()); // method Start() creates range for usage in cut algorithm
~~~~

@subsection upgrade_occt760_change_check_to_adaptors Changes in BRepLib_CheckCurveOnSurface & GeomLib_CheckCurveOnSurface interfaces

Now the classes accept adaptors instead objects as input parameters.
*BRepLib_CheckCurveOnSurface* does no more provide access to curves, surface and parameters: methods PCurve(), PCurve2(), Surface() and Range() have been removed.
*BRepLib_CheckCurveOnSurface*: the default value of the *isMultiThread* parameter of the *Perform()* function has been changed from *true* to *false*
*GeomLib_CheckCurveOnSurface* does no more provide access to curve, surface and parameters: methods Curve(), Surface() and Range() have been removed.
*GeomLib_CheckCurveOnSurface*: the default value of the *isMultiThread* parameter of the *Perform()* function has been changed from *true* to *false*

The following functions in *GeomLib_CheckCurveOnSurface* have been modified:
~~~~{.cpp}
GeomLib_CheckCurveOnSurface(const occ::handle<Adaptor3d_Curve>& theCurve,
                            const double theTolRange);

void Init (const occ::handle<Adaptor3d_Curve>& theCurve, const double theTolRange);

void Perform(const occ::handle<Adaptor3d_CurveOnSurface>& theCurveOnSurface,
             const bool isMultiThread);
~~~~

@subsection upgrade_occt760_old_bop_removed Removal of old Boolean operations algorithm (BRepAlgo_BooleanOperation)

* The method *BRepAlgo_Tool::Deboucle3D* has been removed as duplicating. The corresponding method from *BRepOffset_Tool* class has to be used instead.
* The API classes from *BRepAlgo* package performing old Boolean operations algorithm have been removed:
  - *BRepAlgo_BooleanOperation*
  - *BRepAlgo_Fuse*
  - *BRepAlgo_Common*
  - *BRepAlgo_Cut*
  - *BRepAlgo_Section*
  The corresponding classes from the *BRepAlgoAPI* package have to be used instead.
  
@section upgrade_occt770 Upgrade to OCCT 7.7.0

Building OCCT now requires C++17-compliant compiler (see [Building OCCT](@ref build_upgrade__building_occt) for details).
It is recommended using Visual Studio 2019 or later (2022 preferred) for building OCCT on Windows platform.

@subsection upgrade_770_removed_features Removed features

* One of the constructors of the BRepExtrema_DistanceSS class (the one without deflection parameter) has been removed as excessive. The remaining constructor has to be used instead.

@subsection upgrade_occt770_parallel_flag_removed Removed parameter theIsParallel from Put/Compute/Perform

theIsParallel parameter has been removed from Put/Compute/Perform from the next classes:
 - BRepCheck_Analyzer
 - BRepCheck_Edge
 - BRepLib_ValidateEdge
 - GeomLib_CheckCurveOnSurface
 - BRepLib_CheckCurveOnSurface

Now, to set this flag, it is necessary to use method SetParallel()
For example:
~~~~{.cpp}
BRepLib_ValidateEdge aValidateEdge(myHCurve, ACS, SameParameter);
aValidateEdge.SetParallel(toRunParallel);
aValidateEdge.Process();
~~~~

@subsection upgrade_occt770_drawer_aspects Prs3d_Drawer aspects

`Prs3d_Drawer` getters no more implicitly create "default" aspects.
If specific property has not been set before to this drawer instance nor to linked drawer instance, then nullptr property will be returned.
Make sure to set property beforehand or to call `SetOwn*` / `SetupOwn*` methods to derive from defaults.

@subsection upgrade_occt770_opengl OpenGL functions

Applications extending OCCT 3D Viewer and calling OpenGL functions directly (like @c @::glEnable(), e.g. using global namespace) might be affected by changes in `OpenGl_GlFunctions.hxx`.
This header, as well as `OpenGl_GlCore20.hxx` and similar, no more include system OpenGL / OpenGL ES headers to define function table.
Application code calling OpenGL functions directly should be changed to either use `OpenGl_Context::core11fwd` (as designed)
or to include system OpenGL headers in advance (with help of `OpenGl_GlNative.hxx`).

@subsection upgrade_occt770_tooltriangulatedshape StdPrs_ToolTriangulatedShape

Method `StdPrs_ToolTriangulatedShape::Normal()` has been removed.
Please use `BRepLib_ToolTriangulatedShape::ComputeNormals()` to fill in normal attributes in triangulation and fetch them directly using `Poly_Triangulation::Normal()`.

@subsection upgrade_occt770_shapeproximity BRepExtrema_ShapeProximity

A new way of using the `BRepExtrema_ShapeProximity` class was provided for computing a proximity value between two shapes.
If at initialization of the `BRepExtrema_ShapeProximity` class the *theTolerance* parameter is not defined (Precision::Infinite() by default), the proximity value will be computed.

@section upgrade_occt780 Upgrade to OCCT 7.8.0

@subsection upgrade_780_recommendations New Features and Recommendations

The NCollection containers have been modernized to work with move semantics through the new `Move operator` and `Move constructor`. It is recommended to leverage this functionality in the development process.<br />
Backward compatibility with STL allocators has been implemented to use the OCCT memory manager with STL allocators (NCollection_Allocator, NCollection_OccAllocator).<br />
Additionally, utilities have been introduced to work with `shared_ptr` and `unique_ptr` using the OCCT memory manager (`Standard_MemoryUtils.hxx`).

@subsection upgrade_780_ncollection_update Change in Default Clear Behavior for Containers

NCollection container's `Clear(const bool theReleaseMemory = true)` have been changed to `Clear(const bool theReleaseMemory = false)`.<br />
Impacted classes include `IndexedMap`, `IndexedDataMap`, `Map`, `DataMap`, `DynamicArray(Vector)`, `IncAllocator`.<br />
This means that allocated memory for the container will be reused. In this case, it's necessary to be careful with `IncAllocator::Reset()` to control owners of memory blocks.

@subsection upgrade_780_hash_utils Reworked Hash Mechanism for Hash Map (NCollection's map)

The `HashCode(value, upperBound)` static method has been removed and `IsEqual(value1, value2)` is no longer used in the map.<br />
NCollection's map now operates on an STL-like hash mechanism: a struct with a public operator `size_t operator()(object&) const` and `bool operator(object&, object&) const`.<br />
The difference between STL and OCCT is that the hash struct and comparator are combined into a single struct to reduce conflicts on OCCT's user side.<br />
Hash utils have been implemented to hash objects, returning `uint32_t` and `uint64_t` depending on the template (`Standard_HashUtils.hxx`). Algorithms used are `MurmurHash` and `FNVHash`.<br />
Benefits:
* x64 using 8 bytes to store the hash instead of 4 bytes.
* OCCT classes will now be usable as elements in STL `unordered_map` and `unordered_set`.

The migration problem will occur at compile time. Make sure that `int HashCode` has been changed anywhere to `size operator` and `bool IsEqual` to `bool operator`.

@subsection upgrade_780_removed_files Removed Hash Specialization Classes

The majority of include files containing only specialized hashes have been removed.
Their functionality has been consolidated into the hashed object include file (in the "std" namespace).<br />
It is guaranteed that each removed hash class has been transferred to the native hash mechanism of the hashed class.

The migration problem may arise at compile time. Ensure that you remove any files that have been deprecated.

@subsection upgrade_780_tk_rework Reorganized DE TK

DE TK components have been combined or separated based on specific CAD formats to support plug-in ability.
* Components now have a "TKDE" prefix. The available list includes `TKDESTEP`, `TKDEOBJ`, `TKDEIGES`, `TKDEGLTF`, `TKDEVRML`, `TKDEPLY`, `TKDESTL`.
* The DE DRAW TK has been updated in a similar way: DRAW components now have a "TKXSDRAW" prefix. The available list includes `TKXSDRAWSTEP`, `TKXSDRAWOBJ`, `TKXSDRAWIGES`, `TKXSDRAWGLTF`, `TKXSDRAWVRML`, `TKXSDRAWPLY`, `TKXSDRAWSTL`.

Migration problems may occur during configuration time or compile time. Ensure that you update your project configuration accordingly.

@subsection upgrade_780_step_thread_safety Implemented STEP Thread-safety Interface

The STEP interface now uses Static_Interface to extract exchange settings.<br />
A new ability has been implemented to determine parameters in STEP, avoiding Static_Interface.
* For reading, use an additional argument with STEP's parameters in `ReadFile` or `Perform`.
* For writing, use an additional argument with STEP's parameters in `Transfer` or `Perform`.

@subsection upgrade_780_new_memory_manager New Memory Management Functionality

`Standard.hxx` has a new method `AllocateOptimal` for allocating without post-processing (cleaning).<br />
New profiles to allocate memory (defined at configuration time):
* `Native` - allocates with standard `malloc` and `calloc` functionality, performance depends on the OS.
* `TBB` - allocates with TBB's `scalable` allocator functionality.
* `JeMalloc` - allocates with `jemalloc` functions.
* `Flexible` - old-way allocation which defines allocation method in real-time by environment variables.<br />

The most recommended manager is `JeMalloc`. To use it with a plugin system, like `DRAW`, please ensure that JeMalloc was built with the `--disable-initial-exec-tls` flag. For more details, visit [JeMalloc](https://jemalloc.net/).

@subsection upgrade_780_optimization_profiles New CMake Variable for Optimization Profiles

`BUILD_OPT_PROFILE` is a new variable to define optimization level. Available profiles:
* `Default` - specializes only in quality-dependent parameters for the compiler.
* `Production` - specializes in performance and quality-dependent parameters for the compiler and linker.

@section upgrade_occt790 Upgrade to OCCT 7.9.0

@subsection upgrade_790_code_formatting Code Formatting update

The entire code base has been formatted with `clang-format` 18.1.8 (Windows) using settings available in the root of the repository.
Most custom patches on top of previous releases will likely have merge conflicts.
When encountering merge conflicts, it is recommended to use `clang-format` to format the code.
To maintain patches, it is recommended to merge them into the main repository as part of a contribution. See [Get Involved](https://dev.opencascade.org/get_involved) and [Contribution Guide](https://github.com/Open-Cascade-SAS/OCCT/discussions/36).

@subsection upgrade_790_migration Migration to GitHub

The OCCT repository has been migrated to GitHub. The new repository is available at [GitHub](https://github.com/Open-Cascade-SAS/OCCT).
The old repository will be available for some time, but it is recommended to use the new repository for all new changes.
Contribution to the new repository is available through the GitHub interface - see [Get Involved](https://dev.opencascade.org/get_involved) and [Contribution Guide](https://github.com/Open-Cascade-SAS/OCCT/discussions/36).

@subsection upgrade_790_configuration GenProj no longer supported

The `GenProj` tool is no longer supported. It is recommended to use CMake for building OCCT.
In case of problems, please refer to the [CMake Guide](https://dev.opencascade.org/doc/overview/html/build_upgrade__building_occt.html).

@subsection upgrade_790_modeling_scale_exception Disabling exception for transformation with scale

The exception for transformation with scale has been disabled by default.
These exceptions were enabled in OCCT 7.6.0 for all cases of applying a transformation on a `TopoDS_Shape` with scale or negative determinant.
Now the exceptions are disabled by default but can be enabled by changing the parameter in the method which applies the transformation on `TopoDS_Shape`.

@subsection upgrade_790_de_wrapper Migration of DE_Wrapper classes

The DE Wrapper classes have been reorganized to follow a single style throughout the OCCT open source and commercial code.
All DE formats starting from 7.8.0 were grouped into their own TKs with the `TKDE` prefix.
Now all DE Wrapper interfaces have moved to their own package with the `DE` prefix.
DE Wrapper classes follow the pattern: `DE<Format>_Parameters`, `DE<Format>_Provider`, and `DE<Format>_ConfigurationNode`.
Example: `DESTEP_Parameters`, `DESTEP_Provider`, `DESTEP_ConfigurationNode`.

@subsection upgrade_790_de_shape_healing Migration of shape healing parameters

The shape healing parameters have migrated from the resource file to the DE interface.
The previous implementation was based on the resource file or `Interface_Static`.
Now the parameters are stored in the `DE_ShapeFixParameters` structure with the option to use a string-string map to store extra parameters.
To use the previous interface, use code similar to:

~~~~{.cpp}
  STEPControl_Reader aReader;
  XSAlgo_ShapeProcessor::ProcessingData aProcessingData =
    XSAlgo_ShapeProcessor::ReadProcessingData("read.step.resource.name", "read.step.sequence");
  aReader.SetShapeFixParameters(std::move(aProcessingData.first));
  aReader.SetShapeProcessFlags(aProcessingData.second);
~~~~

It is recommended to use the new interface to store parameters in the `DE_ShapeFixParameters` structure directly.

@subsection upgrade_790_de_interface_static Migration of DE parameters from Interface_Static

During transfer operations, all parameters that were stored in `Interface_Static` have moved to their own DE structure.
The parameters are read only once during initialization and stored in the model.
Parameters are now available as part of the DE Wrapper interface, for example: `DESTEP_Parameters`, `DEIGES_Parameters`.
Code samples showing how to set the parameters can be found in `DESTEP_Provider` and `DEIGES_Provider`.

@subsection upgrade_790_general_handle_types Deprecated Handle types

The `Handle_*` type names have been deprecated in favor of directly using the macro.
The `Handle_*` type names are still available, but it is recommended to use the macro directly.
Example:

~~~~{.cpp}
  occ::handle<TDocStd_Application> anApp = new TDocStd_Application(); // recommended
  Handle_TDocStd_Application anApp = new TDocStd_Application(); // deprecated
~~~~

@subsection upgrade_790_general_map NCollection_Map algorithm method migration

The `NCollection_Map` class has been reorganized to migrate extra methods to the `NCollection_MapAlgo` class.
Boolean operations on maps are now available in the `NCollection_MapAlgo` class.

@section upgrade_occt800 Upgrade to OCCT 8.0.0

OCCT 8.0.0 is a major release with broad, source-incompatible changes across the foundation, modeling-data, and modeling-algorithm layers. The bulk of the per-line changes (handle macro, `Standard_*` typedefs, `Standard_OVERRIDE` etc., `TColStd_*`/`TopTools_*` typedefs, `DEFINE_HARRAY*`, `Standard_Failure::Raise`) can be migrated automatically. A smaller set of changes — exception handling, removed/renamed APIs, reworked evaluation hierarchies, and packages that have been removed outright — require manual review.

@subsection upgrade_800_minimums Minimum requirements and project layout

OCCT 8.0.0 raises the supported toolchain baseline and reorganizes the source tree.

- **C++17 is now required**. Application code and CMake `CXX_STANDARD` must be raised to 17 or higher. Compiler minimums: VS 2019 (2022 preferred), GCC 8.0, Clang 7.0, Apple Clang 11.0, MinGW-w64 7.3.
- **CMake 3.10 or later** is enforced (3.16+ recommended; required when `BUILD_USE_PCH=ON` for precompiled headers).
- **Source-tree layout** has been reorganized from a flat `src/<Package>/` layout to `src/<Module>/<Toolkit>/<Package>/`. Build scripts that hard-coded the old layout, copied OCCT into a vendored tree, or referenced specific source paths must be updated.
- **Resource files** are now installed under `share/<OCCT_PROJECT_NAME>/resources/` (vcpkg-compliant layout). The CSF_* environment variables (`CSF_OCCTResourcePath`, `CSF_IGESDefaults`, `CSF_STEPDefaults`, `CSF_PluginDefaults`, `CSF_XCAFDefaults`, etc.) are the supported way to point OCCT at the resources at runtime; `CASROOT` remains supported for compatibility.
- **`BUILD_PATCH` CMake option is removed**. There is no replacement; apply patches at the source level or via your build system.
- **`OCCT_PROJECT_NAME`** is a new CMake cache variable that controls the install layout's project name (defaults to `opencascade`); it is the supported customization hook for vcpkg and embedded builds.
- **Inspector** and **ExpToCas** have been moved out of the main repository to their own GitHub repositories. If your build relied on them being shipped in-tree, switch to the upstream repositories.
- The `-symbolic` Unix linker flag is no longer applied. RTTI behavior with `dlopen`-style plugins should be re-validated.

@subsection upgrade_800_migration_toolkit Automated migration toolkit

A 12-phase Python migration suite is shipped with OCCT under `adm/scripts/migration_800/`. It requires Python 3.6+ with no external dependencies. Each phase supports `--dry-run` to preview changes.

For external projects, the recommended entry point is the wrapper script:

~~~~{.bash}
# Linux/macOS
./adm/scripts/migration_800/run_migration.sh /path/to/your/src --dry-run
./adm/scripts/migration_800/run_migration.sh /path/to/your/src

# Windows
adm\scripts\migration_800\run_migration.bat /path/to/your/src --dry-run
~~~~

The phases run in order and cover, in summary:

1. `migrate_handles.py`- `Handle(T)` to `occ::handle<T>`, `Handle(T)::DownCast()` to `occ::down_cast<T>()`.
2. `migrate_standard_types.py`- `Standard_Boolean/Integer/Real/...` to `bool/int/double/...`, `Standard_True/False` to `true/false`.
3. `migrate_macros.py`- `Standard_OVERRIDE` to `override`, `Standard_NODISCARD` to `[[nodiscard]]`, `Standard_FALLTHROUGH` to `[[fallthrough]];`, etc.
4. `cleanup_define_handle.py`- removes redundant `DEFINE_STANDARD_HANDLE` macros.
5. `cleanup_deprecated_typedefs.py`- removes deprecated typedef/using declarations and replaces usages.
6. `collect_typedefs.py`- collects `NCollection` typedef mappings (analysis phase).
7. `replace_typedefs.py`- replaces `TColStd_*`/`TopTools_*` etc. with `NCollection_*<T>`.
8. `remove_typedef_headers.py`- removes typedef-only headers and updates `FILES.cmake`.
9. `cleanup_forwarding_headers.py`- cleans up forwarding/include-only headers.
10. `cleanup_unused_typedefs.py`- removes unused typedef declarations.
11. `cleanup_access_specifiers.py`- removes redundant access specifiers.
12. `migrate_raise_to_throw.py`- converts `Standard_Failure::Raise(...)` to `throw Standard_Failure(...)`.

The accompanying `migrate_hcollections.py` script converts `DEFINE_HARRAY1`/`DEFINE_HARRAY2`/`DEFINE_HSEQUENCE` macros to the `NCollection_HArray1`/`HArray2`/`HSequence` template classes.

After all phases, `verify_migration.py` reports any remaining legacy patterns:

~~~~{.bash}
python3 adm/scripts/migration_800/verify_migration.py --verbose /path/to/your/src
~~~~

Pre-generated mapping files (`collected_typedefs.json`, `collected_deprecated_typedefs.json`) are bundled in `adm/scripts/migration_800/`; external projects do not need to re-scan the OCCT source. The migration scripts also support `.h`/`.c`/`.hpp`/`.cpp` files and can automatically add or replace `#include` directives when replacing typedefs.

`Standard_UNUSED` is an exception and requires manual migration to `[[maybe_unused]]` due to stricter placement rules of the C++ attribute.

@subsection upgrade_800_handle Handle macro and Standard_* primitive types

The `Handle()` macro and `::DownCast` are replaced with explicit `occ::handle<>` / `occ::down_cast<>` templates:

~~~~{.cpp}
// Before                                                     // After
Handle(Geom_Circle) aCircle;                                   occ::handle<Geom_Circle> aCircle;
Handle(Geom_Circle)::DownCast(aCurve);                         occ::down_cast<Geom_Circle>(aCurve);
~~~~

OCCT's primitive typedefs are migrated to standard C++ types:

| Deprecated typedef       | Replacement       |
| ------------------------ | ----------------- |
| `Standard_Boolean`       | `bool`            |
| `Standard_True/False`    | `true/false`      |
| `Standard_Integer`       | `int`             |
| `Standard_Real`          | `double`          |
| `Standard_ShortReal`     | `float`           |
| `Standard_Byte`          | `uint8_t`         |
| `Standard_Size`          | `size_t`          |
| `Standard_Address`       | `void*`           |
| `Standard_CString`       | `const char*`     |
| `Standard_Character`     | `char`            |
| `Standard_ExtCharacter`  | `char16_t`        |
| `Standard_Time`          | `std::time_t`     |

The `Standard_*` typedefs are still defined for source-level compatibility, but their use is discouraged in new code and the migration scripts rewrite them to the native C++ types.

@subsection upgrade_800_macros Standard_* macros

Legacy attribute macros are replaced with the standard C++ attributes and keywords:

| Deprecated macro       | Replacement        |
| ---------------------- | ------------------ |
| `Standard_OVERRIDE`    | `override`         |
| `Standard_NODISCARD`   | `[[nodiscard]]`    |
| `Standard_FALLTHROUGH` | `[[fallthrough]];` |
| `Standard_Noexcept`    | `noexcept`         |
| `Standard_DELETE`      | `= delete`         |
| `Standard_THREADLOCAL` | `thread_local`     |
| `Standard_ATOMIC(T)`   | `std::atomic<T>`   |

@subsection upgrade_800_exceptions Exception handling

`Standard_Failure` now inherits from `std::exception`. OCCT exceptions can be caught by standard `catch (const std::exception&)` blocks, the message is exposed through `what()`, and `ExceptionType()` returns the exception class name.

Static `Raise()`, `Instance()`, and `Throw()` methods on `Standard_Failure` and its subclasses are removed. Use the C++ `throw` statement directly:

~~~~{.cpp}
// Before (removed)                                            // After
Standard_Failure::Raise("error");                              throw Standard_Failure("error");
Standard_OutOfRange::Raise("index");                           throw Standard_OutOfRange("index");
~~~~

`GetMessageString()` is deprecated- use `what()`. The error-handler stack used by `OCC_CATCH_SIGNALS` is now `thread_local` instead of being protected by a global mutex; threads no longer contend on error-handler state. `Catches()` and `LastCaughtError()` static methods on `Standard_ErrorHandler` have been removed; `OCC_CATCH_SIGNALS` exposes a new `Raise()` re-throw method.

If you previously combined OCCT exceptions with custom catch logic that relied on `Catches()` / `LastCaughtError()`, restructure the handler around the standard `try`/`catch` flow.

@subsection upgrade_800_ncollection NCollection API changes

`Seek()` and `ChangeSeek()` are removed from `NCollection_Map`. Use the unified `Contained()` API, which returns `std::optional<std::reference_wrapper<T>>`:

~~~~{.cpp}
// Before (removed)
const KeyType* pKey = aMap.Seek(aKey);

// After
auto anOpt = aMap.Contained(aKey);
if (anOpt.has_value()) { const KeyType& aFoundKey = anOpt->get(); }
~~~~

The non-throwing `TryEmplace`/`TryBind` methods are now available on all map types and avoid the throwing two-step "find then add" pattern:

~~~~{.cpp}
if (auto* pValue = aMap.TryBind(key, defaultValue)) { /* use pValue */ }
aMap.Emplace(key, constructorArgs...);  // in-place construction, no copy/move
~~~~

C++17 structured bindings are supported via `Items()` / `IndexedItems()` views:

~~~~{.cpp}
for (auto [aKey, aValue] : aMap.Items()) { ... }
~~~~

New collection types are available in 8.0.0 and may be preferred over their predecessors in new code:

- `NCollection_FlatMap` / `NCollection_FlatDataMap`- cache-friendly open-addressing hash containers with Robin Hood hashing.
- `NCollection_OrderedMap` / `NCollection_OrderedDataMap`- insertion-order-preserving maps with O(1) lookup, append, and removal.
- `NCollection_KDTree`- header-only static balanced KD-Tree for spatial queries (`gp_Pnt`, `gp_Pnt2d`, `gp_XYZ`, `gp_XY`).
- `NCollection_ForwardRange`- lightweight forward-range adapter for non-owning index spans.

`Standard_Mutex` is deprecated in favor of `std::mutex`.

@subsection upgrade_800_collections_typedefs Package collection typedefs

The package-specific collection typedefs (`TColStd_*`, `TopTools_*`, `TColgp_*`, etc.) are kept as deprecated aliases of the underlying `NCollection_*<T>` template instantiations and emit deprecation warnings on use. The migration scripts rewrite call sites to use the templates directly:

~~~~{.cpp}
// Before                                                     // After
TColStd_ListOfInteger aList;                                   NCollection_List<int> aList;
TopTools_MapOfShape aMap;                                      NCollection_Map<TopoDS_Shape> aMap;
TColStd_Array1OfReal anArr;                                    NCollection_Array1<double> anArr;
TColgp_SequenceOfPnt aSeq;                                     NCollection_Sequence<gp_Pnt> aSeq;
~~~~

Two package typedef wrappers have been removed entirely (no backward-compatible aliases): `TColGeom` and `TColGeom2d`. Replace with `NCollection_Array1<occ::handle<Geom_Curve>>` and the equivalent `Geom2d_*` form:

~~~~{.cpp}
// Before (removed)                                           // After
#include <TColGeom_Array1OfCurve.hxx>                          #include <NCollection_Array1.hxx>
TColGeom_Array1OfCurve aCurves;                                #include <Geom_Curve.hxx>
                                                               NCollection_Array1<occ::handle<Geom_Curve>> aCurves;
~~~~

@subsection upgrade_800_bspline BSpline/Bezier weights and accessors

BSpline and Bezier curves and surfaces store poles, weights, and knots as direct array value-members rather than `NCollection_HArray*` handles. The previously nullable `Weights()` accessor is replaced by `WeightsArray()`, which always returns a valid reference- non-rational geometry exposes a non-owning view over a static unit-weights buffer with zero allocation:

~~~~{.cpp}
// Before (nullable)
const NCollection_Array1<double>* pWeights = aCurve->Weights();
if (pWeights != nullptr) { /* use weights */ }

// After (always valid)
const NCollection_Array1<double>& aWeights = aCurve->WeightsArray();
~~~~

Copy-out accessor overloads (those taking an output `NCollection_Array1` to fill) are deprecated in favour of zero-copy const-reference returning versions:

~~~~{.cpp}
// Before (deprecated, copies data)
NCollection_Array1<gp_Pnt> aPoles;
aCurve->Poles(aPoles);

// After (zero-copy)
const NCollection_Array1<gp_Pnt>& aPoles = aCurve->Poles();
~~~~

The same deprecation applies to single-element accessors throughout the `Convert` package- prefer the batch const-reference accessors (`Poles()`, `Weights()`, `Knots()`, `Multiplicities()`).

@subsection upgrade_800_eval Evaluation hierarchy: EvalD0/D1/D2/D3/DN

The geometry evaluation dispatch hierarchy has been redesigned across all 32 `Geom`/`Geom2d` curve and surface classes and across `Adaptor3d_Curve`, `Adaptor3d_Surface`, and their subclasses.

The new primary virtual dispatch points are `EvalD0`, `EvalD1`, `EvalD2`, `EvalD3`, and `EvalDN`. They return POD result structs (`Geom_CurveD1` / `D2` / `D3`, `Geom_SurfD1` / `D2` / `D3`, and the `Geom2d_*` equivalents) and use exception-based error handling on the hot path.

The old `Value()` and `D0` / `D1` / `D2` / `D3` / `DN` methods are kept as non-virtual inline backward-compatible wrappers that delegate to `EvalD*`. **If your code overrides `D0`/`D1`/`D2`/`D3`/`DN` on a `Geom*` class or an `Adaptor3d_Curve`/`Adaptor3d_Surface` subclass, you must override `EvalD0`/`EvalD1`/`EvalD2`/`EvalD3`/`EvalDN` instead.** Existing call-site code that consumes `Value()` / `D0..DN` continues to work unchanged.

The new `EvalRep` descriptor system additionally allows alternate evaluation paths to be attached per-object via the `Set`/`Get`/`Clear` EvalRep API (full, derivative-bounded, and parameter-mapped descriptors are supported). This is the mechanism behind, for example, the offset-surface fast path that bypasses the expensive offset evaluation when an equivalent non-offset surface is available.

Adaptor classes for elementary geometry now store `gp_*` primitives directly in `std::variant` and dispatch via switch/enum to `ElCLib`/`ElSLib` static methods, eliminating virtual calls for elementary geometry on the evaluation hot path.

@subsection upgrade_800_lprop_gprop LProp consolidation and GProp deprecations

LProp packages are consolidated and several `GProp` classes are deprecated.

- `Geom2dLProp` and `LProp3d` packages are removed and consolidated into the unified `GeomLProp` template classes (`GeomLProp_CLPropsBase`, `GeomLProp_SLProps`). Backward-compatible type aliases for the previous class names are preserved, so most call sites continue to compile unchanged.
- `LProp_AnalyticCurInf` is removed (dead code).
- The `GProp_EquaType` enum is removed (no consumers). Dimensionality analysis is now exposed via `PointSetLib_Equation`.

The point-cloud property classes `GProp_PGProps`, `GProp_PEquation`, `GProp_CelGProps`, `GProp_SelGProps`, `GProp_VelGProps` are marked with `Standard_HEADER_DEPRECATED("Deprecated since OCCT 8.0.0. Use PointSetLib_Equation instead")` (and equivalent messages) in favour of the new `PointSetLib` package (`PointSetLib_Props`, `PointSetLib_Equation`), which is provided in `TKMath` without inheritance from `GProp_GProps`.

@subsection upgrade_800_bndlib Bounding-box computation: GeomBndLib

A new `GeomBndLib` package provides geometry-aware bounding boxes for all standard `Geom` curve and surface types via `std::variant`-based per-type dispatch, with analytical solutions for conics and quadrics and PSO+Powell-based tight `BoxOptimal` results. `BndLib_Add3dCurve`, `BndLib_AddSurface`, and `BndLib_Add2dCurve` now delegate to `GeomBndLib`. The public `BndLib` API is preserved unchanged.

@subsection upgrade_800_extremapc Point-to-curve extrema: ExtremaPC

The new `ExtremaPC` package provides point-to-curve extrema computation with per-geometry specialized evaluators (`ExtremaPC_Line`, `ExtremaPC_Circle`, `ExtremaPC_Ellipse`, `ExtremaPC_Hyperbola`, `ExtremaPC_BezierCurve`, `ExtremaPC_BSplineCurve`, `ExtremaPC_OffsetCurve`) and an `ExtremaPC_Curve` aggregator dispatcher.

@subsection upgrade_800_brepgraph BRepGraph- graph-based BRep representation

OCCT 8.0.0 introduces a new graph-based representation of topology and BRep geometry as an alternative to the linked `TopoDS_Shape` data structure. The foundation is split across two layers:

- `BRepGraph` (public)- typed `BRepGraph_NodeId` identifiers, multiple Views (`TopoView`, `RefsView`, `CacheView`, `BuilderView`), bidirectional parent/child explorers (`BRepGraph_ChildExplorer`, `BRepGraph_ParentExplorer`, `BRepGraph_WireExplorer`), iterators (`BRepGraph_DefsIterator`, `BRepGraph_RefsIterator`, `BRepGraph_RelatedIterator`, `BRepGraph_CacheKindIterator`, `BRepGraph_LayerIterator`), extensible Layer/Cache system, mutation guards (`BRepGraph_MutGuard`), history tracking (`BRepGraph_History`), deduplication (`BRepGraph_Deduplicate`), compaction (`BRepGraph_Compact`), deep copy (`BRepGraph_Copy`), and validation (`BRepGraph_Validate`).
- `BRepGraphInc` (internal)- incidence-table storage with `BRepGraphInc_Populate` (`TopoDS_Shape` -> graph) and `BRepGraphInc_Reconstruct` (graph -> `TopoDS_Shape`) roundtrip conversion.

`BRepGraph_Tool` is the centralized geometry-access API (analogue of `BRep_Tool`). `BRepGraph_Builder` allows programmatic graph construction without an input `TopoDS_Shape`.

The graph representation is additive in 8.0.0- existing `TopoDS_Shape` code continues to work unchanged. Application code that wants to opt in can use `BRepGraphInc_Populate` and `BRepGraphInc_Reconstruct` to convert in either direction.

@subsection upgrade_800_topods TopoDS_TShape redesign

The `TopoDS_TShape` hierarchy has been reworked at the state and dispatch layer:

- `ShapeType()` is non-virtual and embedded in a compact `uint16_t` bit-packed state field on `TopoDS_TShape`. Subclasses no longer override `ShapeType()`; the shape type is stored alongside the orientation/lock/checked/closed/infinite/convex/modified/free flags in the same field.
- `TopAbs::Compose()`, `Reverse()`, and `Complement()` are inlined `noexcept` static functions in the `TopAbs.hxx` header (constexpr lookup tables) rather than out-of-line functions in `TopAbs.cxx`.

This is a binary-incompatible change. Source compatibility is preserved for code that uses the public `TopoDS_*` and `TopExp*` APIs. Custom `TShape`-derived classes that previously overrode `ShapeType()` must drop the override and rely on the base-class state field.

Child storage on `TopoDS_TShape` remains `NCollection_List<TopoDS_Shape>` and `TopoDS_Iterator` continues to iterate the list - callers that touch `myShapes` directly do not need to change.

@subsection upgrade_800_handle_outparams Handle out-parameter overloads deprecated

Methods that previously returned `occ::handle<>` values via output parameters are deprecated in favour of return-by-value overloads in `ApplicationFramework`, `DataExchange`, `ModelingAlgorithms`, `ModelingData`, and `Visualization`:

~~~~{.cpp}
// Before (deprecated)
occ::handle<TDocStd_Document> aDoc;
anApp.GetDocument(aLabel, aDoc);

// After
occ::handle<TDocStd_Document> aDoc = anApp.GetDocument(aLabel);
~~~~

Many handle-returning APIs are now `[[nodiscard]]`.

@subsection upgrade_800_mesh_factory Mesh: registry-based discrete-algorithm factory

The legacy `DISCRETPLUGIN` / `DISCRETALGO` symbol-based plugin system is replaced with a registry-based factory. The headers `BRepMesh_PluginMacro.hxx`, `BRepMesh_PluginEntryType.hxx`, and `BRepMesh_FactoryError.hxx` are removed, as are the Draw commands `mpsetfunctionname`, `mpgetfunctionname`, `mperror`.

Migrate plugin discovery to the new factory:

~~~~{.cpp}
// Before (removed)
BRepMesh_PluginEntryType aFunc = BRepMesh::PluginEntry("DISCRETPLUGIN");

// After
occ::handle<BRepMesh_DiscretAlgoFactory> aFactory =
    BRepMesh_DiscretAlgoFactory::FindFactory("FastDiscret");
if (!aFactory.IsNull())
{
  occ::handle<BRepMesh_DiscretAlgo> anAlgo = aFactory->Create();
}
~~~~

`BRepMesh_IncrementalMeshFactory` is registered for the "FastDiscret" algorithm and `XBRepMesh_Factory` for the "XBRepMesh" extended meshing algorithm. Loading both `TKMesh` and `TKXMesh` no longer causes symbol collisions.

@subsection upgrade_800_plib PLib refactoring- value types and removed classes

The `PLib_Base` abstract handle class has been removed. `PLib_JacobiPolynomial` and `PLib_HermitJacobi` are now value types (no longer derived from `Standard_Transient`); their methods are non-virtual and `const`. Construct them on the stack or as direct members:

~~~~{.cpp}
// Before
occ::handle<PLib_JacobiPolynomial> aPoly = new PLib_JacobiPolynomial(theDegree, theNivConstr);
aPoly->Points(...);

// After
PLib_JacobiPolynomial aPoly(theDegree, theNivConstr);
aPoly.Points(...);
~~~~

`PLib_DoubleJacobiPolynomial` is removed entirely. There is no direct in-tree replacement- application code that depended on it must be ported manually to a combination of `PLib_JacobiPolynomial` and `PLib_HermitJacobi`, or rewritten against the new `Geom2dProp` / `GeomProp` differential-property packages where applicable.

@subsection upgrade_800_std_math Standard math wrappers deprecated

The OCCT-specific math wrapper functions are deprecated in favour of the `<cmath>` / `<algorithm>` standard library equivalents:

| Deprecated | Replacement |
| ---------- | ----------- |
| `ACos`, `ASin`, `ATan`, `Cos`, `Sin`, `Tan`, `Cosh`, `Sinh`, `Tanh` | `std::acos`, `std::asin`, ... |
| `Exp`, `Log`, `Pow`, `Sqrt` | `std::exp`, `std::log`, `std::pow`, `std::sqrt` |
| `Abs`, `Sign` | `std::abs`, `std::copysign` (or `<` comparison) |
| `Floor`, `Ceiling`, `Round`, `IntegerPart` | `std::floor`, `std::ceil`, `std::round`, `std::trunc` |
| `Min`, `Max` | `std::min`, `std::max` |
| `NextAfter` | `std::nextafter` |

@subsection upgrade_800_step_signature StepData_ReadWriteModule::StepType signature

The pure virtual `StepType()` method on `StepData_ReadWriteModule` now returns `const std::string_view&` instead of `const TCollection_AsciiString&`. All subclasses must update their override to match. If your code stored or compared the result against `TCollection_AsciiString` instances, convert through `TCollection_AsciiString(theType.data(), int(theType.size()))` or compare against string literals directly.

@subsection upgrade_800_ais_immediate AIS_InteractiveContext immediate-mode rendering deprecated

The legacy immediate-mode rendering methods on `AIS_InteractiveContext` are deprecated. New code should use the retained-mode pipeline through `Display()` / `Redisplay()` and presentation updates. Specific deprecated entry points emit compiler warnings on use.

@subsection upgrade_800_mutex Standard_Mutex superseded by std::mutex

`Standard_Mutex` and its `Standard_Mutex::Sentry` RAII helper are deprecated. Use the standard library:

~~~~{.cpp}
// Before
static Standard_Mutex theMutex;
{
  Standard_Mutex::Sentry aLock(theMutex);
  ...
}

// After
static std::mutex theMutex;
{
  std::lock_guard<std::mutex> aLock(theMutex);
  ...
}
~~~~

`TopTools_MutexForShapeProvider` is removed. Where shape-level locking is needed, allocate a `std::mutex` directly. Several previously mutex-guarded internals in `BRepCheck_*` and Foundation globals have been converted to `thread_local` storage in 8.0.0, which may obviate the lock entirely.

@subsection upgrade_800_visualization_unlit Visualization: implicit UNLIT shading removed

The implicit optimization in `OpenGl_Aspects` that forced UNLIT shading when a material had no reflection properties has been removed. This was breaking PBR materials, interior color handling, and texture modulation. If your application relied on zero-material properties to obtain UNLIT shading, set the shading model explicitly:

~~~~{.cpp}
// After
anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
~~~~

@subsection upgrade_800_bnd_intersected Bnd: IntersectStatus enum

`Bnd_Range::IsIntersected()` previously returned a magic integer; it now returns the strongly-typed `Bnd_Range::IntersectStatus` enum:

~~~~{.cpp}
// Before
int aResult = aRange.IsIntersected(...);

// After
Bnd_Range::IntersectStatus aResult = aRange.IsIntersected(...);
~~~~

Several `Bnd_*` accessors (`Center()`, `Min()`, `Max()`, `Get()`) now return `std::optional`. `Bnd_*` classes have been annotated with `[[nodiscard]]` and `noexcept` where appropriate; `Contains()` and `Intersects()` wrappers have been added.

@subsection upgrade_800_geomadaptor_transformed Transformed adaptor changes

`GeomAdaptor_TransformedSurface::GeomSurface()` is deprecated. Use the new original/transformed surface accessors instead. `GeomAdaptor_TransformedCurve` has been introduced as a new base class for `BRepAdaptor_Curve` that wraps a `GeomAdaptor_Curve` (or `Adaptor3d_CurveOnSurface`) with an applied `gp_Trsf`. `GeomAdaptor_Surface` exposes new `ToleranceU()` / `ToleranceV()` accessors.

@subsection upgrade_800_geomhash GeomHash configurable tolerances

Geometry hash classes now expose `CompTolerance` and `HashTolerance` fields and constructors instead of the previously hard-coded `1e-12` constant. Code that depended on the old constant should explicitly request the previous values via the constructors.

@subsection upgrade_800_isclosed IsClosed/IsPeriodic tolerance change

`IsClosed()` on curves and surfaces previously used `gp::Resolution()` (~1e-290), making the check practically unusable. It now uses `Precision::Computational()` (~`DBL_EPSILON`). Trimmed curves and surfaces now also detect integer-period spans when reporting `IsClosed()` / `IsPeriodic()`. Application code that relied on the previous near-zero tolerance must adjust its inputs or its expectations accordingly.

@subsection upgrade_800_build Build and configuration

- `USE_VTK` is now `OFF` by default. Builds that need VIS must opt in explicitly (`-DUSE_VTK=ON`).
- ARM64 architecture is supported; CMake auto-detection and Windows ARM64 CI are added.
- **glTF read/write (`USE_RAPIDJSON`)** is now an explicit dependency: glTF support is disabled when RapidJSON is not available. Set `-DUSE_RAPIDJSON=ON` (and ensure RapidJSON is discoverable, or use `BUILD_USE_VCPKG=ON`) to keep glTF.
- **Documentation build is CMake-driven**. The legacy Tcl-based documentation entrypoints have been removed; use the `Overview`, `RefMan`, and `doc` CMake targets.
- **`vcpkg` integration**- OCCT ships a vcpkg manifest at `adm/vcpkg/ports/opencascade/vcpkg.json` (referenced via `VCPKG_MANIFEST_DIR`). Configure with `-DBUILD_USE_VCPKG=ON` and the desired `USE_*` flags; the toolchain file is detected automatically from `VCPKG_ROOT`. CMake config files are now installed under `share/<OCCT_PROJECT_NAME>/` for vcpkg compliance.
- **Plugin discovery**- Data Exchange plugin registration is centralized through new `Register` / `UnRegister` static methods on configuration nodes, with `DE_MultiPluginHolder` for projects that load multiple providers.
- Standard exception throw replaces legacy `Standard_Failure::Raise` throughout the OCCT source tree- any custom build system that grepped for `Raise(` should be updated.

@subsection upgrade_800_removed Removed and deprecated- summary

Removed (no backward-compatible alias):

| Item | Replacement |
| ---- | ----------- |
| `Standard_Failure::Raise()`, `Instance()`, `Throw()` static methods | `throw Standard_Failure(...)` |
| `Standard_ErrorHandler::Catches()` / `LastCaughtError()` | Implicit via execution flow / variant in handler |
| `NCollection_Map::Seek()` / `ChangeSeek()` | `Contained()` returning `std::optional` |
| `BRepMesh_PluginMacro.hxx`, `BRepMesh_PluginEntryType.hxx`, `BRepMesh_FactoryError.hxx` | `BRepMesh_DiscretAlgoFactory` |
| Draw commands `mpsetfunctionname`, `mpgetfunctionname`, `mperror` | Not needed with factory pattern |
| `TColGeom`, `TColGeom2d` packages | `NCollection_Array1<occ::handle<Geom*_Curve>>` |
| `Geom2dLProp`, `LProp3d` packages | `GeomLProp` template classes (aliases preserved) |
| `LProp_AnalyticCurInf` |- |
| `GProp_EquaType` enum | `PointSetLib_Equation` |
| `BUC60720` Draw command, `QABugs_PresentableObject` |- |
| `PLib_Base` abstract class | `PLib_JacobiPolynomial` / `PLib_HermitJacobi` as value types |
| `PLib_DoubleJacobiPolynomial` |- (port manually) |
| `TopTools_MutexForShapeProvider` | `std::mutex` |
| `OSD_MAllocHook` class, `mallochook` Draw command | Platform tools (Valgrind, AddressSanitizer) |
| `QANCollection` test package | GTest-based tests under `src/.../GTests/` |
| `BUILD_PATCH` CMake option |- (apply patches at source level) |
| Legacy Tcl documentation entrypoints | `Overview`, `RefMan`, `doc` CMake targets |
| Inspector, ExpToCas (in-tree) | Separate GitHub repositories |

Deprecated (still compiles with warnings):

| Item | Replacement |
| ---- | ----------- |
| `Standard_Failure::GetMessageString()` | `what()` |
| Package collection typedefs (`TColStd_*`, `TopTools_*`, ...) | `NCollection_*<T>` |
| BSpline / Bezier copy-out accessor overloads | Const-reference returning versions |
| Convert package single-element accessors | Batch const-reference accessors |
| Nullable `Weights()` | `WeightsArray()` |
| `GProp_PGProps`, `GProp_PEquation`, `GProp_CelGProps`, `GProp_SelGProps`, `GProp_VelGProps` | `PointSetLib_Props`, `PointSetLib_Equation` |
| `GeomAdaptor_TransformedSurface::GeomSurface()` | Original/transformed surface accessors |
| Handle out-parameter overloads | Return-by-value overloads |
| `Standard_Mutex` (and `Standard_Mutex::Sentry`) | `std::mutex` (with `std::lock_guard`) |
| OCCT math wrappers (`ACos`, `Sin`, `Sqrt`, `Min`, `Max`, ...) | `std::` equivalents in `<cmath>` / `<algorithm>` |
| `Transfer_TransferDeadLoop` exception | Status-flag-based dead-loop detection |
| `AIS_InteractiveContext` immediate-mode rendering methods | Retained-mode pipeline |
