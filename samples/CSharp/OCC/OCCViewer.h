#pragma once
#include "stdafx.h"


class OCCViewer
{
public:
	__declspec(dllexport) OCCViewer(void);
	__declspec(dllexport) ~OCCViewer(void);
private:
	Handle_V3d_Viewer myViewer;
	Handle_V3d_View myView;
	Handle_AIS_InteractiveContext myAISContext;
	Handle_Graphic3d_WNTGraphicDevice myGraphicDevice;
public:
	__declspec(dllexport) bool InitViewer(void* wnd);
	__declspec(dllexport) bool ImportBRep(char* filename);
	__declspec(dllexport) void UpdateView(void);
	__declspec(dllexport) void RedrawView(void);
	__declspec(dllexport) void SetDegenerateModeOn(void);
	__declspec(dllexport) void SetDegenerateModeOff(void);
	__declspec(dllexport) void WindowFitAll(int Xmin, int Ymin, int Xmax, int Ymax);
	__declspec(dllexport) void Place(int x, int y, float zoomFactor);
	__declspec(dllexport) void Zoom(int x1, int y1, int x2, int y2);
	__declspec(dllexport) void Pan(int x, int y);
	__declspec(dllexport) void Rotation(int x, int y);
	__declspec(dllexport) void StartRotation(int x, int y);
	__declspec(dllexport) void Select(int x1, int y1, int x2, int y2);
	__declspec(dllexport) void Select(void);
	__declspec(dllexport) void MoveTo(int x, int y);
	__declspec(dllexport) void ShiftSelect(int x1, int y1, int x2, int y2);
	__declspec(dllexport) void ShiftSelect(void);
	__declspec(dllexport) void BackgroundColor(int& r, int& g, int& b);
	__declspec(dllexport) void UpdateCurrentViewer(void);
	__declspec(dllexport) void FrontView(void);
	__declspec(dllexport) void TopView(void);
	__declspec(dllexport) void LeftView(void);
	__declspec(dllexport) void BackView(void);
	__declspec(dllexport) void RightView(void);
	__declspec(dllexport) void BottomView(void);
	__declspec(dllexport) void AxoView(void);
	__declspec(dllexport) void ZoomAllView(void);
	__declspec(dllexport) float Scale(void);
	__declspec(dllexport) void PanGloView(void);
	__declspec(dllexport) void ResetView(void);
	__declspec(dllexport) void SetDisplayMode(int aMode);
	__declspec(dllexport) void SetColor(int r, int g, int b);
	__declspec(dllexport) void ObjectColor(int& r, int& g, int& b);
	__declspec(dllexport) void SetBackgroundColor(int r, int g, int b);
	__declspec(dllexport) void EraseObjects(void);
	__declspec(dllexport) float GetVersion(void);
	__declspec(dllexport) void SetMaterial(int theMaterial);
	__declspec(dllexport) void SetTransparency(int theTrans);
	__declspec(dllexport) bool ImportCsfdb(char* filename);
	__declspec(dllexport) bool ImportIges(char* filename);
	__declspec(dllexport) bool ImportStep(char* filename);
	__declspec(dllexport) bool ExportBRep(char* filename);
	__declspec(dllexport) bool ExportIges(char* filename);
	__declspec(dllexport) bool ExpotStep(char* filename);
	__declspec(dllexport) bool ExportStl(char* filename);
	__declspec(dllexport) bool ExportVrml(char* filename);
	__declspec(dllexport) bool Dump(char* filename);
	__declspec(dllexport) bool IsObjectSelected(void);
	__declspec(dllexport) int DisplayMode(void);
	__declspec(dllexport) void CreateNewView(void* wnd);
	__declspec(dllexport) bool SetAISContext(OCCViewer* Viewer);
	Handle_AIS_InteractiveContext GetAISContext(void);
	__declspec(dllexport) int CharToInt(char symbol);
};
