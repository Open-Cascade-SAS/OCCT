Contribution Workflow {#occt_dev_guides__contribution_workflow}
====================================
@tableofcontents 

@section occt_contribution_workflow_1 Introduction

The purpose of this document is to describe standard workflow for processing contributions to certified version of OCCT. 

@subsection occt_contribution_workflow_1_1 Use of issue tracker system

Each contribution should have corresponding issue (bug, or feature, or integration request) 
registered in the MantisBT issue tracker system accessible by URL 
http://tracker.dev.opencascade.org. 
The issue is processed further according to the described workflow.

@subsection occt_contribution_workflow_1_2 Access Levels 

  Access level defines the permissions of the user to view, 
  register and modify issues in a Mantis bugtracker. 
  The correspondence of access level and user permissions 
  is defined in accordance with the table below.

| Access level  | Granted to       | Permissions       | Can set statuses         |
|:------------- | :--------- | :-------------- | :----------------------- |
| Viewer        | Everyone (anonymous access)   | View public issues only    | No     |
| Reporter | Users registered on dev.opencascade.com  | View, report, and comment issues  | New, Resolved   |
| Updater       | Users of dev.opencascade.com in publicly visible projects | View  and comment issues | New, Resolved  |
| Developer     | OCC developers and external contributors who signed the CLA  | View, report, modify, and handle issues | New, Assigned, Resolved, Reviewed  |
| Tester        | OCC engineer devoted to certification testing | View, report, modify, and handle issues | Assigned, Tested         |
| Manager       | Person responsible for a project or OCCT component | View, report, modify, and handle issues | New, Resolved, Reviewed, Tested, Closed  |

According to his access level, the user can participate in the issue handling process under different roles, as described below.

@section occt_contribution_workflow_2 Typical workflow for an issue

@subsection occt_contribution_workflow_2_1 General scheme

@image html OCCT_ContributionWorkflow_V3_image001.png "Standard life cycle of an issue"
@image latex OCCT_ContributionWorkflow_V3_image001.png "Standard life cycle of an issue"
  
@subsection occt_contribution_workflow_2_2 Issue registration

An issue is registered in Mantis bugtracker by the Reporter with definition of the necessary attributes. 
The definition of the following attributes is obligatory:

  * **Category** - indicates component of OCCT to which the issue relates. If in doubt, assign OCCT:Foundation Classes.
  * **Reproducibility** 
  * **Severity** 
  * **Priority**
  * **Profile** - allows defining the platform on which the problem was detected   from the list of predefined platforms. If a platform is absent in the list of predefined platforms it is possible to use Or Fill In option to define the platform manually.
    * **Platform**
    * **OS**
    * **OS Version**
  * **Products Version** - defines the version of Open CASCADE on which the problem has been detected.
  * **Summary** - a short, one sentence description of the issue. It has a limit of 128 characters. It should be informative and useful for the developers. It is advisable to avoid vague or misleading phrases, such as "it doesn't work" or "it crashed". It is not allowed to mention the issue originator, and in particular the customer, in the name of the registered issue.
  * **Description** - should contain a detailed definition of the nature of the registered issue depending on its type. For a bug it is required to submit a detailed description of the incorrect behavior, including the indication of the cause of the problem (if possible at this stage) or any inputs from the originator. For a feature or integration request it is recommended to describe the proposed feature in details (as much as possible at that stage), including the changes required for its implementation and the main features of the new functionality. Filling the bug description is obligatory. 
  
For example:
  
~~~~~
3rd-party library New_Image has been implemented in OCCT. This library provides build-in support of popular image processing formats, such as JPEG, PNG, GIF or BMP. It should replace all obsolete code for image load/dump in the future. Currently it is used to save images from Window / OpenGL context. 
~~~~~
  
  * **Steps To Reproduce** - in this field it is possible to describe in detail how to reproduce the issue.  This field considerably helps to find the cause of the problem, to eliminate it and to create the test case.
  * **Additional information and documentation updates** - this field should be filled in case when it is  specifically required to single out the changes to be considered when porting from the previous version of OCCT or when the corresponding GIT commit messages are missing or misleading. 
  * **Upload File** field allows attaching the shapes, scripts or modified source files of OCCT. It is recommended to attach a prototype test case in form of a Tcl script for DRAW,   using either existing DRAW commands, or a C++ code which can be organized in DRAW commands,   as well as sample shapes or other input data (if applicable), immediately after the issue registration. 
  
  The newly registered issue gets status **NEW** and is assigned to the developer responsible for the OCCT component indicated in the Category field (Maintainer). 

@subsection occt_contribution_workflow_2_3 Assigning the issue

  The description of the new issue is checked by the **Maintainer** and if it is feasible, 
  he may assign the issue to a **Developer**. Alternatively, any user with **Developer** access level  
  or higher can assign the issue to himself if he wants to provide a solution. 
  
  The recommended way to handle contributions is that the **Reporter** assigns the issue to himself and provides a solution.

  The **Maintainer, Technical Project Manager,** or **Bugmaster** can close or reassign the issue 
  (in **FEEDBACK** state) to the **Reporter** after it has been registered, if its description does not contain sufficient details to reproduce the bug or explain the purpose of the new feature. 
  That decision shall be documented in the comments to the issue in the Bugtracker.

  The assigned issue should have state **ASSIGNED**.

@subsection occt_contribution_workflow_2_4 Resolving the issue

  The **Developer** responsible for the issue assigned to him provides a solution 
  as a change on the version of OCCT indicated in the issue attributes, or the last development version. 

@subsubsection  occt_contribution_workflow_2_4_1 Creation of the branch
  
  The modification of sources should take place in the dedicated branch of the official OCCT Git repository.
   
  The branch should be based on the recent version of the master branch (not later than the commit tagged as the last OCCT release).
  
  The branch name should be composed of letters **CR** followed by the issue ID number (without leading zeros). It is possible to add an optional suffix to the branch name after the issue ID, e.g. to distinguish between several versions of the fix.
  
  The name of the branch, where the fix is submitted, should be given in the note to the Mantis issue    (providing the direct link to relevant branch view in GitWeb is encouraged).
   
  Please, see the details in the @ref occt_dev_guides__git_guide "Guide to using GIT".
  
@subsubsection  occt_contribution_workflow_2_4_2 Requirements to the code modification.
 
   The amount of code affected by the change should be limited 
   to the changes required for the bug fix or improvement. 
   Change of layout or re-formatting of the existing code is allowed 
   only in the parts where meaningful changes related to the issue have been made.  

   Every developer providing a contribution to the source code of OCC should make the appropriate comments in the code  to improve its legibility and maintainability.

  Making the appropriate comments is mandatory in the following cases:
  * Development of a new package / class / method / enumeration;
  * Modification of an existing package / class / method / enumeration that changes its behavior;
  * Modification / new development impacts other packages / classes / methods / enumerations, the documentation which of should be modified correspondingly.  
  
  The only case when the comments may be not required is 
  a modification that does not change the existing behavior in any noticeable way
  or brings the behavior in accordance with the existing description.
  
  The comments must be in good English, containing as much relevant 
  information and as clear as possible. 
  
    
@subsubsection  occt_contribution_workflow_2_4_3 Requirements to the commit message. 

  The commit message posted in GIT constitutes an integral part of both the fix and the release documentation. 
  
  The first line of the first commit message should contain the Summary of the issue (starting with its ID followed by colon, e.g. "0022943: Bug TDataXtd_PatternStd"). 
   
  The next lines should contain a mandatory description of changes. The contents and the recommended structure of the description depend on the nature of the bug. 
  
  In a general case, the following elements should be present:
	* **Problem** – a description of the unwanted behavior;
	* **Change** – a description of the implemented changes, including the name of class/method/enumeration that has been modified or implemented anew;
	* **Result** – a description of the current behavior (after the implementation).

For example:

~~~~~
The exception in method BRepFill_Cone::Truncate caused by hard-coded number of symbols in the parameter for truncation operation has been fixed. Now the parameter provided by method BRepFill_Cone::Parameter is stored in a variable, which is used for truncation.
~~~~~

  \note Please, note that such phrases as "the bug was fixed" or "the regression was eliminated" do not describe the fix, because they provide no information about the **Change**.
    
Other cases may require a more detailed description, or vice versa, the Problem or the Result may not be worth indicating. It is not advisable to go too deep into implementation details, however, any reader should be able to understand, what has been fixed and where.   

Let us see some typical cases:

#### Implementation of a new method:

~~~~~
New method CheckFace::Point has been implemented to check if the point is IN the face. 
~~~~~

#### Improvement of an existing algorithm: 

~~~~~
The concatenation algorithm in class BSpline_Surface has been fixed to work with periodic Bspline surfaces.
~~~~~

#### Removal of an obsolete method:

~~~~~
Class BOPCol_Array1 has been removed. Instead, new method Ncollection_BaseVector::SetIncrement allows setting the size of increment dynamically.  Classes from package BOPDS have been modified accordingly.
~~~~~

#### Description of large-scale development 

The implementation of new functionalities or functional overhauls can contain numerous modifications concerning numerous packages or numerous classes in one package. The description of such implementation should include two elements:
* General description or Summary; 
* List of modifications with necessary comments.

According to the requirements of chapter @ref occt_contribution_workflow_2_2 "Issue Registration" in the present document, the purpose of an improvement should be already given in the Bug Description. However, if the Bug Description is missing or if it describes the problems rather then their solution or if some other important information is missing, it should be given in the commit or better, if there were many different commits, backtracking and overwriting each other, in **Additional information and documentation updates** field. 

Here is the example of information about numerous changes in connection with the bug that should be provided: 

~~~~~
The following modifications have been implemented to improve the stereoscopic output:
	- The target FBO is passed as parameter in method *OpenGl_View::Render()*.
	- Read/Write buffers management logic has been revised in class *OpenGl_Context* taking into account FBOs.
	- New DRAWEXE command *dumpstereo* allows dumping the contents of 2D viewer.
	- New methods *LProjection* and *RProjection* have been added in class Graphic3d_Camera::UpdateProjection() for off-screen rendering.
~~~~~

For more examples of bug descriptions, please, refer to the previous versions of OCCT Release Notes.

@subsubsection  occt_contribution_workflow_2_4_4 Submitting the contributions 

  In some cases (if Git is not accessible for the contributor), 
  external contributions can be submitted as patch (diff) files or sources 
  attached to the Mantis issue, with indication of OCCT version on which the fix is made. 
  Such contributions should be put to Git for processing by someone else,
  and hence they have less priority in processing than the ones submitted directly through Git.

  To submit the modified sources for review and testing, the issue, for which the solution is provided, should be switched to **RESOLVED** state and assigned to the developer who is expected to make a code review (the **Reviewer**; by default, can be set to the **Maintainer** of the component).

@subsection occt_contribution_workflow_2_5 Code review

  The **Reviewer** analyzes the proposed solution for applicability in accordance with OCCT Code reviewing rules and examines all changes in the sources to detect obvious and possible errors, misprints, conformity to coding style.
  
    * If Reviewer detects some problems, he can either:
      * Fix these issues and provide new solution, reassigning the issue (in **RESOLVED** state) to the **Developer**, who then becomes a **Reviewer**. 
       Possible disagreements should be resolved through discussion, which is done normally within issue notes (or on the OCCT developer’s forum if necessary).
      * Reassign the issue back to the **Developer**,  providing detailed list of remarks. The issue then gets status **ASSIGNED** and a new solution should be provided.
    * If Reviewer does not detect any problems, he changes status to **REVIEWED**.

@subsection occt_contribution_workflow_2_6 Testing

  The issues that are in **REVIEWED** state are subject of certification (non-regression) testing. 
  The issue is assigned to OCC **Tester** when he starts processing it.
  The results of tests are checked by the **Tester**: 
    * If the **Tester** detects build problems or regressions, he changes the status to **ASSIGNED** and reassigns the issue to the **Developer** with a detailed description of the problem. The **Developer** should produce a new solution.
    * If the **Tester** does not detect build problems or regressions, he changes the status to **TESTED** for further integration.

@subsection occt_contribution_workflow_2_7 Integration of a solution

  Before integration into the master branch of the repository the **Integrator** checks the following conditions:
    * the change has been reviewed;
    * the change has been tested without regressions (or with regressions treated properly);
    * the test case has been created for this issue (when applicable), and the change has been rechecked on this test case;
    * the change does not conflict with other changes integrated previously.
  
  If the result of check is successful the Integrator integrates solution 
  into the master branch of the repository. Each change is integrated into the master branch 
  as a single commit without preserving the history of changes made in the branch 
  (by rebase, squashing all intermediate commits), however, preserving the author when possible. 
  This is done to have the master branch history plain and clean. 
  The following picture illustrates the process:
  
@image html OCCT_ContributionWorkflow_V3_image002.png "Integration of several branches"
@image latex OCCT_ContributionWorkflow_V3_image002.png "Integration of several branches"
  
  The new master branch is tested against possible regressions that might appear due to interference between separate changes. When the tests are Ok, the new master is pushed to the official repository 
  and the original branches are removed from it.
  The issue status is set then to **VERIFIED** and is assigned to the **Reporter** so that he could check the fix as-integrated.

@subsection occt_contribution_workflow_2_8 Closing a bug

  The **Bugmaster** closes the issue after regular OCCT Release provided that the issue status is **VERIFIED** and that issue was really solved in that release, by rechecking the corresponding test case. The final issue state is **CLOSED**.

@subsection occt_contribution_workflow_2_9 Reopening a bug

  If a regression is detected, the **Bugmaster** may reopen and reassign the **CLOSED** issue to the appropriate developer with comprehensive comments about the reason of reopening. The issue then becomes **ASSIGNED** again. 

@section occt_contribution_workflow_3 Issue attributes

@subsection occt_contribution_workflow_3_1 Severity

  Severity shows at which extent the issue affects the product. 
  The list of used severities is given in the table below in the descending order.
  
  | Severity    |                  Description                      | Weight for Bug Score |
  | :---------- | :------------------------------------------------ | :------------------: |
  | crash       | Crash of the application or OS, loss of data      |          5           |
  | block       | Regression corresponding to the previously  delivered official version. Impossible  operation of a function on any data with no work-around. Missing function previously requested in software requirements specification.   Destroyed data.     |        4            |
  | major       | Impossible operation of a function with existing work-around. Incorrect operation of a function on a particular dataset. Impossible operation of a function after intentional input of incorrect data. Incorrect behavior of a function   after intentional input of incorrect data.  | 3 |
  | minor       | Incorrect behavior of a function corresponding  to the description in software requirements specification. Insufficient performance  of a function.          |   2     |
  | tweak       | Ergonomic inconvenience, need of light updates.   |          1           |
  | text        | Inconsistence of program code to the Coding Standard. Errors in source text  (e.g.  unnecessary variable declarations, missing   comments, grammatical errors in user manuals). | 1 |
  | trivial     | Cosmetic bugs.                                    |          1           |
  | feature     | Bug fix, new feature, improvement that requires workload estimation and validation. | 1 |   
  | integration request | Requested integration of an existing feature into the product.               |          0           |
  | Just a question       | A question to be processed, without need   of any changes in the product.        |         0                |

@subsection occt_contribution_workflow_3_2 Statuses of issues

  The bug statuses that can be applied to the issues are listed in the table below. 
  
  | Status               |  Description                                                        |
  | :------------------- | :----------------------------------------- |
  | New                  | New just registered issue. Testing case should be created by Reporter. |
  | Feedback             | The issue requires more information; the original posters should pay attention. |
  | Assigned             | Assigned to a developer.  |
  | Resolved + a resolution  |    The issue has been fixed, and now is waiting for revision.  |
  |Revised  + a resolution   | The issue has been revised, and now is waiting for testing. |
  | Tested               | The fix has been internally tested by the tester with success on the full non-regression database or its part and a test case has been created for this issue. |
  | Verified             | The fix has been integrated into the master of the corresponding repository |
  | Closed               | The fix has been integrated to the master. The corresponding test case has been executed successfully. The issue is no longer reproduced. |

@subsection occt_contribution_workflow_3_3 Resolutions

  **Resolution** is set when the bug is resolved. "Reopen" resolution is added automatically when the bug is reopened.

  | Resolution            |                               Description                                    |
  |:--------------------- | :--------------------------------------------------------------------------- |
  | Open                  | The issue is being processed.                                                |
  | Fixed                 | The issue has been successfully fixed.                                       |
  | Reopened              | The bug has been reopened because of insufficient fix or regression.         |
  | Unable to reproduce   | The bug is not reproduced.                                |
  | Not fixable           | The bug cannot be fixed because it is a bug of third party software, or because it requires more workload than it can be allowed.        |
  | Duplicate             | The bug for the same issue already exists in the tracker.                    |
  | Not a bug             | It is a normal behavior in accordance with the specification of the product  |
  | No change required    | The issue didn’t require any change of the product, such as a question issue |
  | Suspended             | This resolution is set for Acknowledged status only. It means that the issue is waiting for fix until a special administrative decision is taken (e.g. a budget is not yet set in accordance with the contract)       | 
  | Documentation updated | The issue was a normal behavior of the product, but the actions of the user  were wrong. The specification and the user manual have been updated  to reflect this issue.    |
  | Won’t fix             | An administrative/contractual decision has been taken to not fix the bug     |


  

 
