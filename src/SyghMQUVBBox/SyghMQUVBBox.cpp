///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 sygh.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SyghMQUVBBox.h"
#include "MainDlg.h"
#include "../_MyUtils/MyPluginID.hpp"
#include "../_MyUtils/MyMQUtils.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされたどの関数も
//		関数の最初に追加される AFX_MANAGE_STATE マクロを
//		持たなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな 
//		らないことを意味します、コンストラクタが MFC
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

#pragma region // CSyghMQUVBBoxApp

BEGIN_MESSAGE_MAP(CSyghMQUVBBoxApp, CWinApp)
END_MESSAGE_MAP()


CSyghMQUVBBoxApp::CSyghMQUVBBoxApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CSyghMQUVBBoxApp オブジェクトです。

CSyghMQUVBBoxApp g_theApp;


BOOL CSyghMQUVBBoxApp::InitInstance()
{
	CWinApp::InitInstance();

	//::InitCommonControls();
	TRACE(_T("Plugin-DLL instance initialization finished.\n"));

	return TRUE;
}

#pragma endregion

//---------------------------------------------------------------------------


static const DWORD DEVELOPER_ID = (MY_PRODUCT_ID);
static const DWORD THIS_PLUGIN_ID = (MY_PLUGIN_ID_BASE + 0);

// メニュー名は一応マルチバイト文字も使えるが、OS のロケール（ANSI コードページ）に依存するので使わない。
static const LPCSTR MENU_NAME = "Sygh: UV Bounding Box";


//---------------------------------------------------------------------------
//  MQGetPlugInID
//    プラグインIDを返す。
//    この関数は起動時に呼び出される。
//---------------------------------------------------------------------------
MQPLUGIN_EXPORT void MQGetPlugInID(DWORD* Product, DWORD* ID)
{
	// プロダクト名(制作者名)とIDを、全部で64bitの値として返す
	// 値は他と重複しないようなランダムなもので良い
	*Product = DEVELOPER_ID;
	*ID = THIS_PLUGIN_ID;
	// バージョンアップしてもプラグインIDは変更はしないことにする。
}

//---------------------------------------------------------------------------
//  MQGetPlugInName
//    プラグイン名を返す。
//    この関数は[プラグインについて]表示時に呼び出される。
//---------------------------------------------------------------------------
MQPLUGIN_EXPORT const char* MQGetPlugInName(void)
{
	TCHAR dllFilePath[MAX_PATH] = {};
	::GetModuleFileName(AfxGetInstanceHandle(), dllFilePath, MAX_PATH);

	MyUtils::VersionInfoStringPack verinfo;
	verinfo.QueryVersionInfoAsStrings(dllFilePath);

	const auto pluginFullName = verinfo.ProductName + _T("  Ver.") + verinfo.ProductVersion + _T("    ") + verinfo.LegalCopyright;

	// 複数のスレッドから同時に呼び出されることは決してないという前提。
	static CStringA name;
	name = CStringA(pluginFullName);
	return name;
}

//---------------------------------------------------------------------------
//  MQGetPlugInType
//    プラグインのタイプを返す。
//    この関数は起動時に呼び出される。
//---------------------------------------------------------------------------
MQPLUGIN_EXPORT int MQGetPlugInType(void)
{
	// 選択部変形用プラグインである
	return MQPLUGIN_TYPE_SELECT;
}

//---------------------------------------------------------------------------
//  MQEnumString
//    ポップアップメニューに表示される文字列を返す。
//    この関数は起動時に呼び出される。
//---------------------------------------------------------------------------
MQPLUGIN_EXPORT const char* MQEnumString(int index)
{
	switch (index)
	{
	case 0:
		return MENU_NAME;
	}
	return nullptr;
}

namespace
{
	bool ShowMainDialog(MQCDocument* doc)
	{
		// ダイアログを表示
		CMainDlg dlg;
		dlg.SetDocument(doc);
		const INT_PTR result = dlg.DoModal();
		TRACE(_T("Plugin-DLL command execution finished.\n"));
		return result == IDOK;
	}
}

//---------------------------------------------------------------------------
//  MQModifySelect
//    メニューから選択されたときに呼び出される。
//---------------------------------------------------------------------------
MQPLUGIN_EXPORT BOOL MQModifySelect(int index, MQCDocument* doc)
{
	if (index == 0)
	{
		return ShowMainDialog(doc);
	}
	return false;
}


namespace
{
	// 拡縮率の計算。
	MQCoordinate CalcScalingRate(MQCoordinate min1, MQCoordinate max1, MQCoordinate min2, MQCoordinate max2)
	{
		const float EPS = (1.0e-10f);
		const MQCoordinate diff1 = max1 - min1;
		const MQCoordinate diff2 = max2 - min2;
		// ゼロ割による発散防止。
		// 分母の絶対値が十分小さい場合、拡縮はしない。
		const float tempU = (fabs(diff1.u) < EPS) ? (1.0f) : (diff2.u / diff1.u);
		const float tempV = (fabs(diff1.v) < EPS) ? (1.0f) : (diff2.v / diff1.v);
		return MQCoordinate(tempU, tempV);
	}

	// 各頂点 UV に対する処理。
	inline MQCoordinate ScaleAndTranslate(MQCoordinate texCoord, MQCoordinate scale, MQCoordinate min1, MQCoordinate min2)
	{
		return scale * (texCoord - min1) + min2;
	}

	bool FitBoundingBoxImpl(MQCDocument* doc, MQCoordinate scale, MQCoordinate min1, MQCoordinate min2)
	{
		return MyMQUtils::CommonUVScanLoopImpl<true>(doc,
			[&](MQCoordinate& uv)
		{
			uv = ScaleAndTranslate(uv, scale, min1, min2);
		});
	}
}

namespace MyPluginCoreFuncs
{
	bool FitUVBoundingBox(MQCDocument* doc, MQCoordinate min1, MQCoordinate max1, MQCoordinate min2, MQCoordinate max2)
	{
		const MQCoordinate scale = CalcScalingRate(min1, max1, min2, max2);
		return FitBoundingBoxImpl(doc, scale, min1, min2);
	}
}
