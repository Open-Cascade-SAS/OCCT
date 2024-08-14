iOS: 3D Viewer (Objective-C++|UIKit) {#occt_samples_ios_uikit}
================== 

UIKitSample consists of the Open CASCADE 3D Viewer which provides import of STEP files and toolbar with three buttons.
The sample could be found within OCCT repository in folder `/samples/ios/UIKitSample/`.

The first and second buttons serve for import hardcoded STEP files. The third button displays "About" dialog.

The viewer supports zoom, pan and rotate actions. The viewer supports selection of solids as well.

<img src="image/sample_ios_uikit.png" alt="">

Installation and configuration:
    1. Make sure you are running Mac OS version 10.12.1 or above and properly installed XCode version 8.1 or above.
    2. Install Open CASCADE Technology (OCCT) and build static libraries for desired device or/and simulator on your workstation.
    3. Build or download Freetype2 static library for desired device or/and simulator.
    4. Open UIKitSample in XCode.
    5. Select the UIKitSample project and add the OCCT static libraries and Freetype2 static library.
    6. Select the UIKitSample and go to the "Build Settings" tab. After go to the section "Search Paths" and in the field "Header Search Paths" specify a path to the OCCT inc folder. Next in the field "Library Search Paths" specify a path/paths to the OCCT static libraries and Freetype2 static library folders.
    7. Connect device and build sample for device or choose simulator as a target and build for simulator.
    8. Run sample.
