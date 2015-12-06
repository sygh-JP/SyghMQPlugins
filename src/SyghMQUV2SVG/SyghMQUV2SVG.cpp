///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 sygh.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SyghMQUV2SVG.h"
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

#pragma region // CSyghMQUV2SVGApp

BEGIN_MESSAGE_MAP(CSyghMQUV2SVGApp, CWinApp)
END_MESSAGE_MAP()


CSyghMQUV2SVGApp::CSyghMQUV2SVGApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CSyghMQUV2SVGApp オブジェクトです。

CSyghMQUV2SVGApp g_theApp;


BOOL CSyghMQUV2SVGApp::InitInstance()
{
	CWinApp::InitInstance();

	//::InitCommonControls();
	TRACE(_T("Plugin-DLL instance initialization finished.\n"));

	return TRUE;
}

#pragma endregion

//---------------------------------------------------------------------------


static const DWORD DEVELOPER_ID = (MY_PRODUCT_ID);
static const DWORD THIS_PLUGIN_ID = (MY_PLUGIN_ID_BASE + 3);

// メニュー名は一応マルチバイト文字も使えるが、OS のロケール（ANSI コードページ）に依存するので使わない。
static LPCSTR MENU_NAME = "Sygh: UV to SVG";


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
		ShowMainDialog(doc);
	}
	// 呼び出し側（Metasequoia）に true を返すと、ドキュメントに変更があった、と判断される。
	// SVG 出力はドキュメントに一切変更を加えないので常に false を返す。
	return false;
}


namespace MyPluginCoreFuncs
{
	// ドキュメントのカレントオブジェクトかつカレントマテリアルの全 UV リストを作成。
	void BuildUVList(MQCDocument* pDoc, TCoord2DArray& uv2DArray)
	{
		ASSERT(pDoc != nullptr);

		uv2DArray.clear();
		const int curmi = pDoc->GetCurrentMaterialIndex();
		if (curmi == -1)
		{
			// カレントマテリアルが無効＝マテリアルがひとつも無い。すなわちすべてが未着色面。
			throw CString(_T("マテリアルが存在しません。"));
		}
		auto* pCurObj = pDoc->GetObject(pDoc->GetCurrentObjectIndex());
		if (pCurObj == nullptr)
		{
			// カレントオブジェクトが無効＝オブジェクトがひとつも無い。
			throw CString(_T("オブジェクトが存在しません。"));
		}
		// 一応検証。
		auto* pCurMat = pDoc->GetMaterial(curmi);
		if (!pCurMat)
		{
			throw CString(_T("カレントマテリアルがドキュメント内に存在しません。"));
		}
		// UV マッピングされている必要がある。
		if (pCurMat->GetMappingType() != MQMATERIAL_PROJECTION_UV)
		{
			throw CString(_T("カレントマテリアルのマッピング方式が UV ではありません。"));
		}

		const int faceCount = pCurObj->GetFaceCount();
		for (int j = 0; j < faceCount; ++j)
		{
			// カレントマテリアルでない場合、処理をスキップ。
			if (pCurObj->GetFaceMaterial(j) != curmi)
			{
				continue;
			}
			// 三角形でも四角形でもない場合、処理をスキップ。つまり線分は無視。
			// HACK: Metasequoia 4 では多角形面がサポートされるようになったため、5 以上が返ってくることがありえる。
			// 多角形にも対応するかどうかは TBD。
			const auto fpc = pCurObj->GetFacePointCount(j);
#if 0
			if (fpc != 3 && fpc != 4)
#else
			if (fpc < 3)
#endif
			{
				continue;
			}
			TCoordArray uvArray(fpc);
			pCurObj->GetFaceCoordinateArray(j, &uvArray[0]);
			uv2DArray.push_back(uvArray);
		}
		//return true;
	}

	// 指定したファイルパスに SVG 出力。
	void SaveSVG(LPCTSTR pFilename, const TCoord2DArray& uv2DArray, const SVGOption& inOption)
	{
		// 簡単な XML を出力するだけなので、MSXML などは使わない。

		// HACK: SVG はレイヤーの代わりにグループを扱える。オブジェクト×マテリアルの数だけグループを作成する形でもよい。
		// 名前の属性を付加する場合はエスケープ（エンティティ参照化）に注意。
		// 表示状態の属性も付加できるはず。

		FILE* fp = nullptr;
		// SVG では Shift_JIS は無理らしい。UTF-8 を使用する。
		// .NET と違い、MSVC CRT では BOM なしの UTF-8 を直接出力できない。
		// ASCII のみのテキストであればそのまま BOM なし UTF-8 と同義だが、
		// マルチバイトのオブジェクト名・マテリアル名をグループ名に使うことを見据えて、
		// あえて BOM は付けておく。BOM をまともに扱えないツールはサポートしない。
		// そのほか、DTD はいかにも時代遅れな感じだが、SVG 2.0 は2015年9月時点でもまだドラフトらしい。
		if (_tfopen_s(&fp, pFilename, _T("w, ccs=UTF-8")) != 0)
		{
			throw CString(_T("出力ファイルが書込モードで開けません。"));
		}
		_ASSERTE(fp != nullptr);

		_ftprintf(fp,
			_T("<?xml version=\"%s\" encoding=\"%s\" standalone=\"%s\"?>\n")
			_T("<!DOCTYPE svg PUBLIC \"%s\"\n")
			_T("\t")_T("\"%s\">\n")
			_T("\n")
			_T("<svg width=\"%ld\" height=\"%ld\" version=\"%s\"\n")
			_T("\t")_T("viewBox=\"0 0 %ld %ld\"\n")
			_T("\t")_T("xmlns=\"%s\">\n"),
			_T("1.0"),
			_T("UTF-8"),
			_T("no"),
			_T("-//W3C//DTD SVG 1.1//EN"),
			_T("http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd"),
			inOption.CanvasSize.cx, inOption.CanvasSize.cy,
			_T("1.1"),
			inOption.CanvasSize.cx, inOption.CanvasSize.cy,
			_T("http://www.w3.org/2000/svg")
			);

		_ftprintf(fp,
			_T("\t")
			_T("<g transform=\"translate(%ld,%ld) scale(%ld,%ld)\"")
			_T(" style=\"fill:%s;stroke:%s;stroke-width:%f;stroke-linejoin:%s;\">\n"),
			0L, 0L, inOption.CanvasSize.cx, inOption.CanvasSize.cy,
			_T("none"), inOption.ColorName.GetString(), inOption.PenWidth, _T("round"));

		// ガイド正方形（正規化された正方形）の出力。
		if (inOption.IsGuideNeeded)
		{
			_ftprintf(fp, _T("\t\t")_T("<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" />\n"),
				0, 0, 1, 1);
		}

		// 各面を polygon タグとして出力する。
		for (const auto& uvArray : uv2DArray)
		{
			_ftprintf(fp, _T("\t\t")_T("<polygon points=\""));

			const auto uvCount = uvArray.size();
			for (size_t j = 0; j < uvCount; ++j)
			{
				// 頂点 UV を出力。
				_ftprintf(fp, _T("%f,%f"), uvArray[j].u, uvArray[j].v);
				if (j + 1 != uvCount)
				{
					_ftprintf(fp, _T(" "));
				}
			}
			_ftprintf(fp, _T("\" />\n"));
		}
		_ftprintf(fp, _T("\t")_T("</g>\n"));
		_ftprintf(fp, _T("</svg>\n"));

		fclose(fp);
		fp = nullptr;
	}
}
