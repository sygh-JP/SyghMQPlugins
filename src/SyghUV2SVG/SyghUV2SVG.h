#pragma once

#ifndef __AFXWIN_H__
#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h" // メイン シンボル

#include "../_MyUtils/MyUtility.hpp"


class CSyghUV2SVGApp : public CWinApp
{
public:
	CSyghUV2SVGApp();

	// オーバーライド
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};


using TCoordArray = std::vector<MQCoordinate>;
using TCoord2DArray = std::vector<TCoordArray>;


class SVGOption
{
public:
	CSize CanvasSize;
	float PenWidth = 0;
	bool IsGuideNeeded = false;
	CString ColorName;
public:
	SVGOption() {}
	SVGOption(int canvasSizeX, int canvasSizeY, float penWidth, bool isGuideNeeded, CString colorName)
		: CanvasSize(canvasSizeX, canvasSizeY)
		, PenWidth(penWidth)
		, IsGuideNeeded(isGuideNeeded)
		, ColorName(colorName)
	{}
};

namespace MyPluginCoreFuncs
{
	extern void BuildUVList(MQCDocument* pDoc, TCoord2DArray& uv2DArray);
	extern void SaveSVG(LPCTSTR pFilename, const TCoord2DArray& uv2DArray, const SVGOption& Option);
}

// MFC 標準 DLL では自動的かつ暗黙的に DllMain() が実装される。
// 一応明示的にカスタム実装する方法はあるが、推奨されないらしい。
// モジュールのインスタンスハンドルが欲しい場合は、
// 通例 AfxGetInstanceHandle() もしくは CWinApp::m_hInstance を使えばよい。
// 
// ちなみに非 MFC EXE でホストされている DLL から AfxGetAppName() を呼ぶと、EXE ではなく DLL の名前が返ってくる模様。
// AfxMessageBox() で表示されるダイアログのタイトルも同様。
// MFC EXE でホストされている場合は変わってくるかもしれない。

// 16bit 時代や 32bit 時代の古い Web ページではダイアログプロシージャーやメッセージプロシージャーのシグネチャを
// INT CALLBACK Proc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam);
// や
// BOOL CALLBACK Proc(HWND hWnd, UINT uMsg, UINT wParam, LONG lParam);
// と間違って説明しているものがある。Windows API 直叩き環境だけでなく、MFC 環境でも同様のミスをしているものがある。
// 決して真似しないように。正しくは下記。
// INT_PTR CALLBACK Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
