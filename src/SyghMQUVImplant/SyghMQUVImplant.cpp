///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 sygh.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SyghMQUVImplant.h"
#include "MainDlg.h"
#include "../_MyUtils/MyPluginID.hpp"


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

#pragma region // CSyghMQUVImplantApp

BEGIN_MESSAGE_MAP(CSyghMQUVImplantApp, CWinApp)
END_MESSAGE_MAP()


CSyghMQUVImplantApp::CSyghMQUVImplantApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CSyghMQUVImplantApp オブジェクトです。

CSyghMQUVImplantApp g_theApp;


BOOL CSyghMQUVImplantApp::InitInstance()
{
	CWinApp::InitInstance();

	TRACE(_T("Plugin-DLL instance initialization finished.\n"));

	return TRUE;
}

#pragma endregion

//---------------------------------------------------------------------------


static const DWORD DEVELOPER_ID = (MY_PRODUCT_ID);
static const DWORD THIS_PLUGIN_ID = (MY_PLUGIN_ID_BASE + 4);

// メニュー名は一応マルチバイト文字も使えるが、OS のロケール（ANSI コードページ）に依存するので使わない。
static LPCSTR MENU_NAME = "Sygh: UV Implant";


//---------------------------------------------------------------------------
//  MQGetPlugInID
//    プラグインIDを返す。
//    この関数は起動時に呼び出される。
//---------------------------------------------------------------------------
MQPLUGIN_EXPORT void MQGetPlugInID(DWORD* Product, DWORD* ID)
{
	// TODO: プロダクト名(制作者名)とIDを、全部で64bitの値として返す
	// 値は他と重複しないようなランダムなもので良い
	*Product = DEVELOPER_ID;
	*ID = THIS_PLUGIN_ID;
	// バージョンアップしてもプラグインIDは変更はしない。
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
	return MQPLUGIN_TYPE_OBJECT;
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
//  MQModifyObject
//    メニューから選択されたときに呼び出される。
//---------------------------------------------------------------------------
MQPLUGIN_EXPORT BOOL MQModifyObject(int index, MQCDocument* doc)
{
	if (index == 0)
	{
		return ShowMainDialog(doc);
	}
	// 呼び出し側（Metasequoia）に true を返すと、ドキュメントに変更があった、と判断される。
	return false;
}


namespace MyPluginCoreFuncs
{

	// src から dst に UV を移植。
	void ImplantUV(MQCDocument* pDoc, int srcIndex, int dstIndex)
	{
		ASSERT(pDoc != nullptr);
		ASSERT(pDoc->GetObjectCount() > srcIndex);
		ASSERT(pDoc->GetObjectCount() > dstIndex);

		if (srcIndex == dstIndex)
		{
			throw CString(_T("移植元・移植先が同じオブジェクトです。"));
		}

		const int curmi = pDoc->GetCurrentMaterialIndex();
		if (curmi == -1)
		{
			// カレントマテリアルが無効＝マテリアルがひとつも無い。すなわちすべてが未着色面。
			throw CString(_T("ドキュメント内にマテリアルが存在しません。"));
		}

		auto* pCurObj = pDoc->GetObject(pDoc->GetCurrentObjectIndex());
		if (pCurObj == nullptr)
		{
			// カレントオブジェクトが無効＝オブジェクトがひとつも無い。
			throw CString(_T("ドキュメント内にオブジェクトが存在しません。"));
		}

		auto* pSrcObj = pDoc->GetObject(srcIndex);
		auto* pDstObj = pDoc->GetObject(dstIndex);
		ASSERT(pSrcObj != nullptr);
		ASSERT(pDstObj != nullptr);

		const CString notEqTopologyMsg(_T("移植元・移植先のオブジェクトのトポロジーが異なります。"));

		// トポロジーが全く同一であることを検証する。
		const int srcFaceCount = pSrcObj->GetFaceCount();
		const int dstFaceCount = pDstObj->GetFaceCount();
		if (srcFaceCount != dstFaceCount)
		{
			throw notEqTopologyMsg;
		}

		for (int j = 0; j < srcFaceCount; ++j)
		{
			const int srcMatid = pSrcObj->GetFaceMaterial(j);
			if (srcMatid == -1)
			{
				throw CString(_T("移植元オブジェクトの面にマテリアルが割り当てられていません。"));
			}
			const int dstMatid = pDstObj->GetFaceMaterial(j);
			if (dstMatid == -1)
			{
				throw CString(_T("移植先オブジェクトの面にマテリアルが割り当てられていません。"));
			}

			// 一応検証。
			auto* pSrcMat = pDoc->GetMaterial(srcMatid);
			if (!pSrcMat)
			{
				throw CString(_T("移植元オブジェクトの面のマテリアルがドキュメント内に存在しません。"));
			}
			auto* pDstMat = pDoc->GetMaterial(dstMatid);
			if (!pDstMat)
			{
				throw CString(_T("移植先オブジェクトの面のマテリアルがドキュメント内に存在しません。"));
			}

			// 移植元・移植先でマテリアルが異なっていても構わないが、UV マッピングされている必要がある。
			if (pSrcMat->GetMappingType() != MQMATERIAL_PROJECTION_UV)
			{
				throw CString(_T("移植元オブジェクトの面のマッピング方式が UV ではありません。"));
			}
			if (pDstMat->GetMappingType() != MQMATERIAL_PROJECTION_UV)
			{
				throw CString(_T("移植先オブジェクトの面のマッピング方式が UV ではありません。"));
			}

			// 面の頂点数の検証。
			const auto srcfpc = pSrcObj->GetFacePointCount(j);
			const auto dstfpc = pDstObj->GetFacePointCount(j);

			if (srcfpc != dstfpc)
			{
				throw notEqTopologyMsg;
			}
		}

		// チェック後、改めてループを回す。

		//MQCoordinate uvArray[4];
		std::vector<MQCoordinate> uvArray(4);
		for (int j = 0; j < srcFaceCount; ++j)
		{
			const auto srcfpc = pSrcObj->GetFacePointCount(j);
			// 三角形でも四角形でもない場合、処理をスキップ。つまり線分は無視。
			// HACK: Metasequoia 4 では多角形面がサポートされるようになったため、5 以上が返ってくることがありえる。
			// 多角形にも対応するかどうかは TBD。
#if 0
			if (srcfpc != 3 && srcfpc != 4)
#else
			if (srcfpc < 3)
#endif
			{
				continue;
			}
			uvArray.resize(srcfpc);

			// UV の移植。
			pSrcObj->GetFaceCoordinateArray(j, &uvArray[0]);
			pDstObj->SetFaceCoordinateArray(j, &uvArray[0]);
		}
	}

}
