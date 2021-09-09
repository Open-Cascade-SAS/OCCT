Inspector  {#occt_user_guides__inspector}
===============================

@tableofcontents
 
@section occt_inspector_1 Introduction

This manual explains how to use the Inspector.

@subsection occt_inspector_1_1 Overview
Inspector is a Qt-based library that provides functionality to interactively inspect low-level content of the OCAF data model, OCCT viewer and Modeling Data.
This component is aimed to assist the developers of OCCT-based applications to debug the problematic situations that occur in their applications.

Inspector has a plugin-oriented architecture. The current release contains the following plugins:

| Plugin | OCCT component | Root class of OCCT investigated component |
| :----- | :----- | :----- |
| @ref occt_inspector_2_2 "DFBrowser"| OCAF | *TDocStd_Application* |
| @ref occt_inspector_2_3 "VInspector"| Visualization | *AIS_InteractiveContext* |
| @ref occt_inspector_2_4 "ShapeView"| Modeling Data | *TopoDS_Shape* |
| @ref occt_inspector_2_5 "MessageView"| Modeling Data | *Message_Report* |


Each plugin implements logic of a corresponding OCCT component.

Each of the listed plugins is embedded in the common framework, thus it is possible to manage, which plugins should be loaded by the Inspector, and to extend their number by implementing a new plugin.


@subsection occt_inspector_1_3 Getting started

There are two launch modes:
1. Launch **TInspectorEXE** executable sample. For more details see @ref occt_inspector_4_1 "TInspectorEXE" section;
2. Launch DRAW, load plugin INSPECTOR, and use *tinspector* command.
   For more details, see @ref occt_inspector_4_2 "Launch in DRAW Test Harness" section.


**Note**. If you have no Inspector library in your build directory, make sure that OCCT is compiled with *BUILD_Inspector* 
option ON. For more details see @ref occt_inspector_5 "Build procedure".


@section occt_inspector_2 Inspector Plugins

@subsection occt_inspector_2_1 Overview

Inspector consists of the following components:
  * <b>buttons</b> to activate the corresponding plugin;
  * <b>view area</b> to visualize the plugin content.

@figure{tinspector_elements.svg,"Plugins placement in Inspector",360}

@subsection occt_inspector_2_2 DFBrowser Plugin

@subsubsection occt_inspector_2_2_1 Overview

@figure{dfbrowser.png, "DFBrowser"}

This plugin visualizes the content of *TDocStd_Application* in a tree view. It shows application documents, 
the hierarchy of *TDF_Labels*, the content of *TDF_Attributes* and interconnection between attributes (e.g. references).
Additionally there is a 3D view to visualize *TopoDS_Shape* elements stored in the document.

@subsubsection occt_inspector_2_2_2 Elements

@figure{dfbrowser_elements.svg, "DFBrowser Elements",360}

<b>OCAF tree view</b>

Each OCAF element has own tree view item:

| Type | Tree item | Text | Description |
| :----- | :----- | :----- | :----- |
| *TDocStd_Application* | Application | *TDocStd_Application* | The root of tree view. Its children are documents.|
| *TDocStd_Document* | Document | entry : name | A child of *Application* item. Its children are *Label* and *Attribute* items.<br> Text view is an entry of the root label and the value of *TDataStd_Name* attribute for the label if it exists. |
| *TDF_Label* | Label | entry : name | A child of a *Document* or another *Label* item. Its children and text view are the same as for Document item. |
| *TDF_Attribute* | Attribute | attribute type [additional information] | A child of a *Label*. It has no children. <br> Text view is the attribute type *(DynamicType()->Name()* of *TDF_Attribute*) and additional information (a combination of attribute values). |


Additional information about TDF_Attributes:

| Type | Text |
| :----- | :----- |
| *TDocStd_Owner* | [storage format] |
| *TDataStd_AsciiString*,<br> *TDataStd_Name*,<br> *TDataStd_Real*,<br> other *Simple* type attributes | [value] |
| *TDataStd_BooleanList*,<br> *TDataStd_ExtStringList*,<br> other *List* attributes | [value_1 ... value_n] |
| *TDataStd_BooleanArray*,<br> *TDataStd_ByteArray*,<br> other *Array* type attributes | [value_1 ... value_n] |
| *TDataStd_TreeNode* | [tree node ID  ==> Father()->Label()] (if it has a father) or <br> [tree node ID <== First()->Label()] (if it has NO father)|
| *TDataStd_TreeNode(XDE)* | [XDE tree node ID  ==> Father()->Label()] (if it has a father), <br> [XDE tree Node ID <== label_1, ..., label_n] (if it has NO father)|
| *TNaming_NamedShape* | [shape type : evolution] |
| *TNaming_UsedShapes* | [map extent] |


Custom color of items:

| OCAF element Type | Color |
| :----- | :----- |
| *TDF_Label* | <b>dark green</b>, if the label has *TDataStd_Name* attribute, <br><b>light grey</b> if the label is empty (has no attributes on all levels of  hierarchy),<br> <b>black</b> otherwise. |
| *TNaming_NamedShape* | <b>dark gray</b> for *TopAbs_FORWARD* orientation of *TopoDS_Shape*, <br> <b>gray</b> for *TopAbs_REVERSED* orientation of *TopoDS_Shape*, <br> <b>black</b> for other orientation. |


Context pop-up menu:
| Action | Functionality |
| :----- | :----- |
| Expand | Expands the next two levels under the selected item. |
| Expand All | Expands the whole tree of the selected item. |
| Collapse All | Collapses the whole tree of the selected item. |

<b>Property Panel</b>

Property panel is used to display the result of <b>TDF_Attribute::Dump()</b> or <b>TDF_Label::Dump()</b> of the selected tree view item.
The information is shown in one table.

@figure{property_panel.png,"PropertyPanel",360}

<b>Property Panel (custom)</b>

Property panel (custom) is used to display the content of *Label* or *Attribute* tree view items or Search result view.
The information is usually shown in one or several tables.

*TDF_Attribute* has the following content in the Property Panel:

<table>
<tr><th>Type</th><th>Description</th><th>Content</th></tr>
<tr><td><i>TDF_Label</i></td>
    <td> a table of [entry or attribute name, value]</td>
    <td>@figure{property_panel_custom_label.png, "",140}</td></tr>
<tr><td><i>TDocStd_Owner</i>,<br> Simple type attributes, <br> List type attributes</td>
    <td>a table of [method name, value]</td>
    <td>@figure{property_panel_custom_simple_type.png, "",140}</td></tr>
<tr><td><i>TDataStd_BooleanArray</i>,<br> <i>TDataStd_ByteArray</i>,<br> other Array type attributes</td>
    <td>2 controls: <br> - a table of [array bound, value], <br> - a table of [method name, value] </td>
    <td>@figure{property_panel_custom_array.png, "",140}</td></tr>
<tr><td><i>TDataStd_TreeNode</i></td>
    <td>2 controls: <br> - a table of [Tree ID, value] (visible only if Tree ID() != ID() ), <br> - a tree view of tree nodes starting from *Root()* of the tree node. The current tree node has <b>dark blue</b> text.</td>
    <td>@figure{property_panel_custom_tree_node.png, "",140} </td></tr>
<tr><td><i>TDataStd_NamedData</i></td>
    <td>tab bar of attribute elements, each tab has a table of [name, value]</td>
    <td>@figure{property_panel_custom_named_data.png, "",140}</td></tr>
<tr><td><i>TNaming_UsedShapes</i></td>
    <td>a table of all shapes handled by the framework</td>
    <td>@figure{property_panel_custom_tnaming_used_shapes.png, "",140}</td></tr>
<tr><td><i>TNaming_NamedShape</i></td>
    <td>2 controls: <br> - a table of [method name, value] including CurrentShape/OriginalShape methods result of <i>TNaming_Tools</i>, <br> - an evolution table. <br> Tables contain buttons for @ref occt_shape_export "TopoDS_Shape export".</td>
    <td>@figure{property_panel_custom_tnaming_named_shape.png, "",140}</td></tr>
<tr><td><i>TNaming_Naming</i></td>
    <td>2 controls: <br> - a table of <i>TNaming_Name</i> values,<br> - a table of [method name, value]</td>
    <td>@figure{property_panel_custom_tnaming_naming.png, "",140}</td></tr>
</table>


<b>Dump view</b>

@figure{dump_attribute.png, "Dump of TDF_Attribute",200}

Dump view shows the result of <b>TDF_Attribute::Dump()</b> or <b>TDF_Label::Dump()</b> of the selected tree view item.

<b>3D view</b>

3D View visualizes *TopoDS_Shape* elements of OCAF attribute via AIS facilities.

DFBrowser creates two kinds of presentations depending on the selection place:

<table>
<tr><th>Kind</th><th>Source object</th><th>Visualization properties</th><th>View</th></tr>
<tr><td>Main presentation</td>
    <td>Tree view item:<br> *TPrsStd_AISPresentation*,<br> *TNaming_NamedShape*,<br> *TNaming_Naming*</td>
    <td>Color: a default color for shape type of the current *TopoDS_Shape*.</td>
    <td>@figure{display_main_presentation.png, "",100}</td></tr>
<tr><td>Additional presentation</td>
    <td>References in Property panel</td>
    <td>Color: white</td>
    <td>@figure{display_additional_presentation.png, "",100}</td></tr>
</table>



<b>Tree Navigation</b>

Tree Navigation shows a path to the item selected in the tree view.
The path is a sequence of label entries and attribute type names.
Each element in the path is selectable - simply click on it to select the corresponding tree view item.

Navigation control has buttons to go to the previous and the next selected tree view items.


<b>Update Button</b>

Update button synchronizes content of tree view to the current content of OCAF document that could be modified outside.

<b>Search</b>

The user can search OCAF element by typing:
  * *TDF_Label* entry,
  * *TDF_Attribute* name,
  * *TDataStd_Name* and *TDataStd_Comment* attributes value.

@figure{search.png,"Search",360}

As soon as the user confirms the typed criteria, the Property panel is filled by all satisfied values.
The user can click a value to highlight the corresponding tree view item. By double click the item will be selected.


@subsubsection occt_inspector_2_2_3 Elements cooperation

<b>Tree item selection</b>

Selection of tree view item updates content of the following controls:
  * Navigation line;
  * Property Panel;
  * 3D View (if it is possible to create an interactive presentation);
  * Dump View.

@figure{dfbrowser_selection_in_tree_view.svg,"",360}

<b>Property Panel (custom) item selection </b>

If the property panel (custom) shows content of *TDF_Label*:
  * selection of the table row highlights the corresponding item in the tree view,
  * double click on the table row selects this item in the tree view.

If the property panel (custom) shows content of *TDF_Attribute* that has reference to another attribute, selection of this reference:
  * highlights the referenced item in the tree view,
  * displays additional presentation in the 3D view if it can be created.

@figure{property_panel_custom_item_selection.svg,"",360}

Attributes having references:

| Type | Reference | Additional presentation
| :----- | :----- | :----- |
| *TDF_Reference* | *TDF_Label* | |
| *TDataStd_ReferenceArray*, <br> *TDataStd_ReferenceList*, <br> *TNaming_Naming* | One or several *TDF_Label* in a container. | |
| *TDataStd_TreeNode* | *TDF_Label* | |
| *TNaming_NamedShape* | *TDF_Label* in Evolution table |  *TopoDS_Shapes* selected in the property panel tables. |
| *TNaming_UsedShapes* | one or several *TNaming_NamedShape* | *TopoDS_Shapes* of the selected *TNaming_NamedShape*. |


@subsubsection occt_shape_export TopoDS_Shape export

Property panel of *TNaming_NamedShape* attribute has controls to export *TopoDS_Shape* to:
  * BREP. **Save file** dialog is open to enter the result file name,
  * @ref occt_inspector_2_4 "ShapeView" plugin. The dialog for exporting element to ShapeView allows activating this plugin immediately.


@subsection occt_inspector_2_3 VInspector Plugin

@subsubsection occt_inspector_2_3_1 Overview

@figure{vinspector.png, "VInspector",360}

This plugin visualizes interactive objects displayed in *AIS_InteractiveContext* in a tree view with computed selection
components for each presentation. It shows the selected elements in the context and allows selecting these elements.

@subsubsection occt_inspector_2_3_2 Elements

@figure{vinspector_elements.svg,"VInspector Elements",360}

<b>Presentations tree view</b>

This view shows presentations and selection computed on them. Also, the view has columns with information about the state of visualization elements.

VInspector tree items.

| Type | Description |
| :----- | :----- |
| *AIS_InteractiveContext* | The root of tree view. Its children are interactive objects obtained by *DisplayedObjects* and *ErasedObjects* methods.|
| *AIS_InteractiveObject* | A child of *AIS_InteractiveContext* item. Its children are *SelectMgr_Selection* obtained by iteration on *CurrentSelection*.  |
| *SelectMgr_Selection* | A child of *AIS_InteractiveObject*. Its children are *SelectMgr_SensitiveEntity* obtaining by iteration on *Sensitive*. |
| *SelectMgr_SensitiveEntity* | A child of *SelectMgr_Selection*. Its children are *SelectMgr_SensitiveEntity* obtaining by iteration on *OwnerId*. |
| *SelectBasics_EntityOwner* | A child of *SelectMgr_SensitiveEntity*. It has no children. |


Custom color of tree view items:

| OCAF element Type | Column | What | Color |
| :----- | :----- | :----- | :----- |
| *AIS_InteractiveObject* | 0 | Text | <b>dark gray</b> in *ErasedObjects* list of *AIS_InteractiveContext*,<br> <b>black</b> otherwise |
| *AIS_InteractiveObject*, <br> *SelectMgr_SensitiveEntity*, <br> *SelectBasics_EntityOwner*| 1 | Background | <b>dark blue</b>, if there is a selected owner under the item, <br> <b>black</b> otherwise |
| *SelectMgr_Selection*,<br> *SelectMgr_SensitiveEntity*,<br> *electBasics_EntityOwner* | all | Text | <b>dark gray</b>, if *SelectionState* of *SelectMgr_Selection* is not *SelectMgr_SOS_Activated*,<br> <b>black</b> otherwise |


Context popup menu in tree view:
| Action | Item | Functionality |
| :----- | :----- | :----- |
| Export to ShapeView | *AIS_InteractiveObject* | Exports *TopoDS_Shape* of the *AIS_Interactive* presentation to ShapeView plugin. <br> It should be *AIS_Shape* presentation and ShapeView plugin should be registered in Inspector<br> Dialog about exporting element to ShapeView is shown with a possibility to activate this plugin immediately. |
| Show | *AIS_InteractiveObject* | Displays presentation in *AIS_InteractiveContext*. |
| Hide | *AIS_InteractiveObject* | Erases presentation from *AIS_InteractiveContext*. |

<b>Update</b>

This button synchronizes the plugin content with the current state of *AIS_InteractiveContext* and updates the presence of items and their current selection.

@subsubsection occt_inspector_2_3_3 Elements cooperation

*VInspector* marks the presentations currently selected in *AIS_InteractiveContext* with a blue background in tree items. Use **Update** button to synchronize VInspector selected items state to the context.

It is also possible to perform selection in the context using "Selection controls" VInspector feature. However, this operation should be performed carefully as
it clears the current selection in *AIS_InteractiveContext*.

Selection change:
| From | To | Action | Result |
| :----- | :----- | :----- | :----- |
| *AIS_InteractiveContext* | VInspector | Performs selection in *AIS_InteractiveContext*. | Click **Update** button in VInspector and check **Selection** column: <br> *AIS_InteractiveContext* item contains some selected objects, <br> the value of some *AIS_InteractiveObject* is filled if they are selected for this presentation or its entity owner. |
| VInspector | *AIS_InteractiveContext* | Activates one of Selection controls and selects one or several elements in the tree view. | The objects become selected in *AIS_InteractiveContext*. |

@subsubsection occt_inspector_2_3_4 VInspector tree view columns

Use context pop-up menu on the tree view header to select, which columns should be displayed.
@figure{vinspector_tree_columns.png, "Vinspector tree header context menu",360}

Use the setting Lights (position, color) in the view.
@figure{vinspector_light_setting.png, "Vinspector light setting",360}

@subsubsection occt_inspector_2_3_5 VInspector property panel

Property panel shows the result of <b>AIS_InteractiveContext::Dump()</b> or <b>AIS_InteractiveObject::Dump()</b>.
@figure{vinspector_property_panel.png, "Vinspector property panel",360}

@subsection occt_inspector_2_4 ShapeView Plugin

@subsubsection occt_inspector_2_4_1 Overview

@figure{shapeview.png, "ShapeView",360}

This plugin visualizes content of *TopoDS_Shape* in a tree view.

@subsubsection occt_inspector_2_4_2 Property panel

Property panel shows properties for TopoDS_Shape based on DumpJson.

@figure{shapeview_property_panel.png, "ShapeView Property panel",360}

@subsubsection occt_inspector_2_4_3 Elements

@figure{shapeview_elements.svg,"ShapeView Elements",360}

<b>TopoDS_Shape View</b>

The view elements are *TopoDS_Shape* objects.
The shape is exploded into sub-shapes using *TopoDS_Iterator* of the *TopoDS_Shape*.
Children sub-shapes are presented in the view as children of the initial shape.
By iterating recursively through all shapes we obtain a tree view of items shown in the ShapeView.

The columns of the View show some information about *TopoDS_Shape* of the item.
The first column allows changing the visibility of the item shape in the 3D view.

Context pop-up menu in tree view:
| Action | Functionality |
| :----- | :----- |
| Load BREP file | Opens the selected file and appends the resulting *TopoDS_Shape* into the tree view. |
| Remove all shape items | Clears tree view. |
| BREP view | Shows the text view with BREP content of the selected item. Creates the BREP file in a temporary directory of the plugin. |
| Close All BREP views | Closes all opened text views. |
| BREP directory | Displays the folder, where temporary BREP files have been stored. |

@subsubsection occt_inspector_2_4_4 Elements cooperation

Selection of one or several items in *TopoDS_Shape* View creates its *AIS_Shape* presentation and displays it in the 3D View.

@subsubsection occt_inspector_2_4_5 ShapeView tree view columns

Use context pop-up menu on the tree view header to select, which columns should be displayed.
@figure{shapeview_tree_columns.png, "ShapeView tree header context menu",360}

@subsection occt_inspector_2_5 MessageView Plugin

MessageView plugin is used to display content of Message_Report.

@subsubsection occt_inspector_2_5_1 Message report tree view

Message report tree view shows the content of the Message_Report.

Context pop-up menu in message report tree view:
| Action | Functionality |
| :----- | :----- |
| Export Report | Exports the report as json file. |
| WallClock Metric statistic | Creates the table  that sums the number of calls and the time spent on the functionality inside the value and shows it in Property panel (custom). It's necessary to activate "WallClock metric". |
| Preview children presentations | Displays presentations of children items of selected items if found. |
| Deactivate | Deactivates all types of metrics for the current report. |
| Activate | Appends items to activate report metrics. |
| Clear | Clears message report. |
| Activate metric | Switches active state in report for clicked type of metric. |
| Test metric | Sends several alerts to check metric of message-alert-tool mechanism. |
| Test Message_Messenger | Sends several alerts to check property panel/presentations of messenger-alert-tool mechanism. |
| Test Tree of messages | Sends several alerts to check tree of alerts. |

@figure{messageview_pop_up_menu.png, "MessageView pop-up menu",360}

@subsubsection occt_inspector_2_5_2 3D View

3D View shows the selected item (TopoDS_Shape) in message report tree view.
@figure{messageview_view.png, "MessageView 3D View",360}

@subsubsection occt_inspector_2_5_3 Dump panel

Shows Dump() information of the selected item if the item has Dump().
@figure{messageview_dump_panel.png, "MessageView 3D View",360}

@subsubsection occt_inspector_2_5_4 Property panel (custom)

Shows the table for WallClock Metric statistic option.
@figure{messageview_property_panel_custom.png, "MessageView 3D View",360}

@subsubsection occt_inspector_2_5_5 Elements
@figure{messageview_elements.svg, "MessageView elements",360}

@section occt_inspector_3 Common controls

@subsection occt_inspector_3_1 Tree View

This control shows presentation hierarchy of the investigated OCCT element, e.g. *TDocStd_Application* for DFBrowser, see @ref occt_inspector_1_1 "Overview".
The first column contains the name, other columns are informative.

The tree view has a context menu with plugin-specific actions.

@subsubsection occt_inspector_3_1_1 Tree View preferences

It is possible to define visibility and width of columns.
This option is available in a view that contains more than one column,
 e.g. @ref occt_inspector_2_3_4 "VInspector tree view columns"
 and @ref occt_inspector_2_4_4 "ShapeView tree view columns".

@figure{treeview_preferences.svg, "Preferences schema",360}


@subsection occt_inspector_3_2 3D View

@subsubsection occt_inspector_3_2_1 Overview

@figure{3DView.png, "3D View",360}

This control for OCCT 3D viewer creates visualization view components and allows performing some user actions in the view.


@subsubsection occt_inspector_3_2_2 Elements

@figure{3DView_elements.svg,"3DView Elements",360}

3D View contains the following elements:
| Element | Functionality |
| :----- | :----- |
| 3D view | V3d viewer with mouse events processing. |
| Context | Allows choosing another context that should be used in the plugin. The following contexts are available:<br> **Own** - the context of this view, <br> **External** - the context of the @ref occt_inspector_4_3 "external application", which initializes the plugin, <br> **None** - the visualization is not performed at all (useful if the presentation is too complex). |
| Multi/Single | The buttons define what to do with the previously displayed objects: <br> **Multi** displays new presentations together with already displayed ones, <br> **Single** removes all previously displayed presentations. |
| Clean | Removes all displayed presentations. |
| Trihedron display | Shows the trihedron. |
| View cube display | Shows the view cube. |
| Fit All | Scene manipulation actions<br> (Fit All is checkable. If checked(by double click), display/hide of new objects will perform **Fit All** of the scene.) |
| Display Mode | Sets *AIS_Shading* or *AIS_WireFrame* display mode for all presentations. |

Context popup menu:
| Action | Functionality |
| :----- | :----- |
| Set View Orientation | Shows the list of available *V3d_View* projections. Selection of an item with change the view. |

@figure{3DView_set_orientation.png,"Set view orientation",360}

@subsubsection occt_inspector_3_2_3 3D View preferences.
View preferences store the current view orientation.

@subsection occt_inspector_3_3 Preferences context menu

@figure{preferences.png,"Plugin preferences",360}

Context menu contains:
| Element | Functionality |
| :----- | :----- |
| Tree Level Line,<br> PropertyPanel,<br> PropertyPanel (custom),<br> Dump, <br> View| Names of dock widgets in the active plugin. If the button is checked, dock widget is visible. |
| Store Preferences | Creates ".tinspector.xml" preferences file with the current settings for each plugin.<br> This file is created in TEMP/TMP directory (by default) or in a user-defined directory. |
| Remove Preferences | Removes preferences file. After the Inspector is restarted, default values will be applied. |


The following controls have store/restore preferences:
| Element | Preferences |
| :----- | :----- |
| Geometry| Inspector window size and position. <br>State of dockable widgets : visibility, position, size.|
| @ref occt_inspector_3_1_1 "Tree View preferences"| Columns visible in the tree view and their width. |
| @ref occt_inspector_3_2_3 "3D View preferences"| 3D view camera direction. |

@section occt_inspector_4 Getting Started

@subsection occt_inspector_4_1 TInspectorEXE sample

This sample allows trying Inspector functionality.

Use *inspector.bat* script file placed in a binary directory of OCCT to launch it.

This script accepts the names of plugin's DLL that should be loaded. By default it loads all plugins described above.


@figure{TStandaloneEXE.png, "TStandaloneEXE",360}

Click on the Open button shows the dialog to select a file.
@figure{TStandaloneEXE_open.png, "",360}

Depending on the active plugin, it is possible to select the following files in the dialog:<br>
- DFBRowser: OCAF document or STEP files;
- VInspector: BREP files;
- ShapeView: BREP files.

Click the file name in the proposed directory and enter it manually or using **Browse** button.

By default, TInspectorEXE opens the following files for plugins:
| Plugin DLL library name | Files |
| :----- | :----- |
| TKDFBrowser | step/screw.step |
| TKVInspector | occ/hammer.brep |
| TKShapeView | occ/face1.brep, <br> occ/face2.brep |

These files are found relatively to *CSF_OCCTDataPath*.

@subsubsection occt_inspector_4_1_1 TInspectorEXE preferences
The application stores recently loaded files. On the application start, the last file is activated.
**Open file** dialog contains recently loaded files.
Selection of a new file updates the container of recently loaded files and rewrites preferences.

Source code of *TIspectorEXE* is a good sample for @ref occt_inspector_4_3 "using the Inspector in a custom application".

@subsection occt_inspector_4_2 How to launch the Inspector in DRAW Test Harness

*TKToolsDraw* plugin provides DRAW commands for Qt tools. Use *INSPECTOR* parameter of @ref occt_draw_1_3_3 "pload" 
command to download the commands of this library. It contains *tinspector* command to start Inspector under DRAW.
See more detailed description of the @ref occt_draw_13_1 "tinspector" command.

The simple code to start Inspector with all plugins loaded:

~~~~
pload INSPECTOR
tinspector
~~~~

@figure{drawexe_tinspector.png,"tinspector",360}

This command does the following:
- all available Plugins are presented in the Inspector. These are @ref occt_inspector_2_2 "DFBrowser", @ref occt_inspector_2_3 "VInspector", @ref occt_inspector_2_4 "ShapeView" and  @ref occt_inspector_2_5 "MessageView";
- DFBrowser is the active plugin;
- OCAF tree is empty.

After this, we should create objects in DRAW and update *tinspector*.
The examples of using Inspector in DRAW can be found in OCCT source directory /tests/tools.

@subsection occt_inspector_4_3 How to use the Inspector in a custom application

The example of using the Inspector in a custom application is presented in OCCT qt sample - <b>FuncDemo</b>.
For building qt samples, switch on *BUILD_SAMPLES_QT* variable in @ref build_cmake_conf "Configuration process".

In general, the following steps should be taken:
* Set dependencies to OCCT and Qt in the application (Header and Link);
* Create an instance of *TInspector_Communicator*;
* Register the plugins of interest in the communicator by DLL library name;
* Initialize the communicator with objects that will be investigated;
* Set visible true for the communicator.



Here is an example of C++ implementation:
~~~~{.cpp}

#include <inspector/TInspector_Communicator.hxx>

static TInspector_Communicator* MyTCommunicator;

void CreateInspector()
{
  NCollection_List<Handle(Standard_Transient)> aParameters;
  //... append parameters in the list

  if (!MyTCommunicator)
  {
    MyTCommunicator = new TInspector_Communicator();

    MyTCommunicator->RegisterPlugin ("TKDFBrowser");
    MyTCommunicator->RegisterPlugin ("TKVInspector");
    MyTCommunicator->RegisterPlugin ("TKShapeView");
    MyTCommunicator->RegisterPlugin ("TKMessageView");

    MyTCommunicator->Init (aParameters);
    MyTCommunicator->Activate ("TKDFBrowser");
  }
  MyTCommunicator->SetVisible (true);
}
~~~~

Give one the following objects for a plugin using a container of parameters:

| Plugin | to be initialized by |
| :----- | :----- |
| *TKDFBrowser* | *TDocStd_Application* |
| *TKVInspector* | *AIS_InteractiveContext* |
| *TKShapeView* | *TopoDS_TShape* |
| *TKMessageView* | *Message_Report* |


@section occt_inspector_5 Build procedure


@subsection occt_inspector_5_1 Building with CMake within OCCT

By default the Inspector compilation is off.
To compile it, set the *BUILD_Inspector* flag to "ON". See @ref build_cmake_conf "Configuration process".

When this option is switched ON, MS Visual Studio project has an additional tree of folders:

@figure{VStudio_projects.png,"Inspector packages in MS Visual Studio",160}


@section occt_inspector_6 Sources and packaging

OCCT sources are extended by the /tools directory.

Distribution of plugin packages :
| Source packages | Plugin |
| :----- | :----- |
| *DFBrowser*, <br> *DFBrowserPane*, <br> *DFBrowserPaneXDE*, <br> *TKDFBrowser* | DFBrowser |
| *VInspector*, <br> *TKVInspector*  | VInspector |
| *ShapeView*, <br> *TKShapeView* | ShapeView |
| *MessageView*, <br> *TKMessageView* | MessageView |

Other packages:
| Source packages| Used in |
| :----- | :----- |
| *TInspectorAPI*, <br> *TKInspectorAPI* | Interface for connection to plugin. |
| *ViewControl*, <br> *TKTreeModel* | Classes for property view, table, table model. |
| *TreeModel*, <br> *TKTreeView* | Items-oriented model to simplify work with GUI tree control. |
| *View*, <br> *TKView* | 3D View component. |
| *TInspector*, <br> *TKTInspector*  | Inspector window, where plugins are placed. |
| *ToolsDraw*, <br> *TKToolsDraw* | Plugin for DRAW to start Inspector. |


In MSVC studio, a separate folder contains Inspector projects.

@section occt_inspector_7 Glossary
* **Component** -- a part of OCCT , e.g. OCAF, VISUALIZATION, MODELING and others. 
* **Plugin** -- a library that is loaded in some executable/library. Here, the plugins are:
  * DFBrowser,
  * ShapeView,
  * VInspector.
