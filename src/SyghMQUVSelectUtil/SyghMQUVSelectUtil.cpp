
#include "stdafx.h"
#include "SyghMQUVSelectUtil.h"
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

#pragma region // CSyghMQUVSelectUtilApp

BEGIN_MESSAGE_MAP(CSyghMQUVSelectUtilApp, CWinApp)
END_MESSAGE_MAP()


CSyghMQUVSelectUtilApp::CSyghMQUVSelectUtilApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CSyghMQUVSelectUtilApp オブジェクトです。

CSyghMQUVSelectUtilApp g_theApp;


BOOL CSyghMQUVSelectUtilApp::InitInstance()
{
	CWinApp::InitInstance();

	//::InitCommonControls();
	TRACE(_T("Plugin-DLL instance initialization finished.\n"));

	return TRUE;
}

#pragma endregion

//---------------------------------------------------------------------------


static const DWORD DEVELOPER_ID = (MY_PRODUCT_ID);
static const DWORD THIS_PLUGIN_ID = (MY_PLUGIN_ID_BASE + 2);

// メニュー名は一応マルチバイト文字も使えるが、OS のロケール（ANSI コードページ）に依存するので使わない。
static const LPCSTR MENU_NAME = "Sygh: UV Select Utility";


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
		// HACK: 選択状態を変更するだけのプラグインなので、常に false を返せばいいのでは？
		// ここで true を返しても、実際にはデータの変更がないので、ダーティ フラグは立たないみたいだが……
		return ShowMainDialog(doc);
	}
	return false;
}

namespace
{
	// 参考：カレント・非カレントに関係なくすべての UV 選択された頂点をスキャンするループ。
	void TestFunc(MQCDocument* doc)
	{
		for (int i = 0; i < doc->GetObjectCount(); ++i)
		{
			auto* obj = doc->GetObject(i);
			if (obj == nullptr)
			{
				continue;
			}
			for (int j = 0; j < obj->GetFaceCount(); ++j)
			{
				for (int k = 0; k < obj->GetFacePointCount(j); ++k)
				{
					if (doc->IsSelectUVVertex(i, j, k))
					{
					}
				}
			}
		}
	}
}

namespace MyPluginCoreFuncs
{

	// 選択面に含まれる頂点を UV 選択。
	void SelectFace2UV(MQCDocument* doc)
	{
		MyMQUtils::ScanAllFaces(doc, [doc](int objIndex, int faceIndex, int facePointCount) {
			if (doc->IsSelectFace(objIndex, faceIndex))
			{
				for (int k = 0; k < facePointCount; ++k)
				{
					doc->AddSelectUVVertex(objIndex, faceIndex, k);
				}
			}
		});
	}

	// UV 選択された頂点が属する面を選択。
	void SelectUV2Face(MQCDocument* doc)
	{
		MyMQUtils::ScanAllFaces(doc, [doc](int objIndex, int faceIndex, int facePointCount) {
			for (int k = 0; k < facePointCount; ++k)
			{
				if (doc->IsSelectUVVertex(objIndex, faceIndex, k))
				{
					doc->AddSelectFace(objIndex, faceIndex);
					break; // 次の面を調べる。
				}
			}
		});
	}

	// 全ての頂点に対して UV 選択を解除。
	void ClearUVSelect(MQCDocument* doc)
	{
		MyMQUtils::ScanAllFaces(doc, [doc](int objIndex, int faceIndex, int facePointCount) {
			for (int k = 0; k < facePointCount; ++k)
			{
				doc->DeleteSelectUVVertex(objIndex, faceIndex, k);
			}
		});
	}

}
