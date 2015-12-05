#include "stdafx.h"
#include "MyUtility.hpp"



namespace MyUtils
{
#if 0
	// ダイアログアイテムテキストと整数値の相互変換 API は Win32 で GetDlgItemInt() として用意されているが、実数値の変換はないので自作。
	// アイテムのテキスト文字列が数でない場合("aaa"など)、ゼロとして変換される。
	// strtod や scanf の %f, %lf は指数形式もスキャン可能。
	// HACK: 変換に失敗した場合は NaN を返すようにしてもいいかもしれない。
	// MFC であれば最初から DDV を使えばよい。

	bool GetDlgItemReal(HWND hWnd, int ItemID, double& Real)
	{
		TCHAR pEditStr[256] = {};
		if (!GetDlgItemText(hWnd, ItemID, pEditStr, ARRAYSIZE(pEditStr) - 1)) return false;
		Real = _tcstod(pEditStr, nullptr);
		return true;
	}

	bool GetDlgItemReal(HWND hWnd, int ItemID, float& Real)
	{
		TCHAR pEditStr[256] = {};
		if (!GetDlgItemText(hWnd, ItemID, pEditStr, ARRAYSIZE(pEditStr) - 1)) return false;
		Real = static_cast<float>(_tcstod(pEditStr, nullptr));
		return true;
	}

	bool SetDlgItemReal(HWND hWnd, int ItemID, double Real)
	{
		TCHAR pEditStr[256] = {};
		_stprintf_s(pEditStr, _T("%.15f"), Real); // HACK: 小数点以下に15桁をとることが最大精度とは限らない。%g を使ったほうがいい。
		return !!::SetDlgItemText(hWnd, ItemID, pEditStr);
	}

	bool SetDlgItemReal(HWND hWnd, int ItemID, float Real)
	{
		TCHAR pEditStr[256] = {};
		_stprintf_s(pEditStr, _T("%f"), Real);
		return !!::SetDlgItemText(hWnd, ItemID, pEditStr);
	}

	bool GetFloatValuesFromEditBox(HWND hWnd, const int EntryNum, const int pIdTable[], float pData[])
	{
		for (int i = 0; i < EntryNum; i++)
		{
			// エディットボックスが一つでも空の場合、処理を中断
			if (!GetDlgItemReal(hWnd, pIdTable[i], pData[i]))
				return false;
		}
		return true;
	}
#endif


	bool GetClipboardText(CString& outString)
	{
#ifdef UNICODE
		const UINT format = CF_UNICODETEXT;
#else
		const UINT format = CF_TEXT;
#endif
		outString.Empty();
		if (!::OpenClipboard(nullptr))
		{
			return false;
		}
		bool isSuccess = false;
		auto hClipboardText = ::GetClipboardData(format);
		if (hClipboardText != nullptr)
		{
			auto pText = static_cast<LPCTSTR>(::GlobalLock(hClipboardText));
			if (pText != nullptr)
			{
				// HACK: クリップボード内のテキストが膨大だった場合、メモリ不足に陥ってアロケートに失敗する可能性も否定できない。
				outString = pText;
				isSuccess = true;
			}
			::GlobalUnlock(hClipboardText);
		}
		::CloseClipboard();
		return isSuccess;
	}

	// クリップボードへ文字列を書き込む。
	bool SetClipboardText(HWND hWndNewOwner, LPCTSTR pSrcStr)
	{
		const int textLength = lstrlen(pSrcStr); // 終端 NUL を含めない長さ
		HGLOBAL hGlobal = ::GlobalAlloc(GHND, sizeof(TCHAR) * (textLength + 1)); // 終端 NUL 含める
		if (hGlobal == nullptr)
		{
			return false;
		}
		auto pText = static_cast<LPTSTR>(::GlobalLock(hGlobal)); // 作成したメモリブロックへのポインタを取得
		if (pText == nullptr)
		{
			::GlobalFree(hGlobal);
			return false;
		}
		lstrcpyn(pText, pSrcStr, textLength + 1); // 終端 NUL 含める
		::GlobalUnlock(hGlobal);
		if (!::OpenClipboard(hWndNewOwner))
		{
			::GlobalFree(hGlobal);
			return false;
		}
		// クリップボード内の既存データを解放。
		if (!::EmptyClipboard())
		{
			::GlobalFree(hGlobal);
			return false;
		}
		// MBCS の場合は CF_TEXT、
		// Unicode (UTF-16LE) の場合は CF_UNICODETEXT を使うらしい。
		// DLL を Unicode ビルドして Unicode API を使った場合、EXE が MBCS でも DLL 側の UI 上では Unicode をまともに扱える？
		// Visual C++ ランタイムを動的リンクして CRT のバージョンが衝突するようなことがなければ OK のはずだが……
		// TODO: MBCS アプリ上でテストしてみる。
#ifdef UNICODE
		const UINT format = CF_UNICODETEXT;
#else
		const UINT format = CF_TEXT;
#endif
		const bool isSuccess = (::SetClipboardData(format, hGlobal) != nullptr);
		// SetClipboardData() 関数を呼び出すと、グローバルメモリブロックは
		// Windows の管理となるので、自分で GlobalFree() してはいけないらしい。
		::CloseClipboard();
		return isSuccess;
	}


	// 実行プログラム（EXE）が自分自身のバージョン情報を知りたい場合、pModuleFilePath に __argv[0] あるいは __wargv[0] を渡す。
	bool VersionInfoStringPack::QueryVersionInfoAsStrings(LPCTSTR pModuleFilePath)
	{
		if (!::PathFileExists(pModuleFilePath))
		{
			return false;
		}
		DWORD handle = 0;
		const DWORD size = ::GetFileVersionInfoSize(pModuleFilePath, &handle);
		std::vector<BYTE> buf;
		buf.resize(size);
		if (!::GetFileVersionInfo(pModuleFilePath, handle, static_cast<DWORD>(buf.size()), &buf[0]))
		{
			return false;
		}
		LPVOID pItem = nullptr;
		UINT itemSize = 0;
		// 言語は1つしかないという前提のもとで。
		if (!::VerQueryValue(&buf[0], _T("\\VarFileInfo\\Translation"), &pItem, &itemSize))
		{
			return false;
		}
		const auto langInfo = *(static_cast<const DWORD*>(pItem));
		CString strSubBlockBase;
		// 言語に応じた文字列テーブルから取得。
		strSubBlockBase.Format(_T("\\StringFileInfo\\%04X%04X\\"),
			LOWORD(langInfo), HIWORD(langInfo));

		LPCTSTR ppInLabels[] =
		{
			_T("Comments"),
			_T("CompanyName"),
			_T("FileDescription"),
			_T("FileVersion"),
			_T("InternalName"),
			_T("LegalCopyright"),
			_T("LegalTrademarks"),
			_T("OriginalFilename"),
			_T("PrivateBuild"),
			_T("ProductName"),
			_T("ProductVersion"),
			_T("SpecialBuild"),
			nullptr // Sentinel
		};
		CString* ppOutStrings[] =
		{
			&this->Comments,
			&this->CompanyName,
			&this->FileDescription,
			&this->FileVersion,
			&this->InternalName,
			&this->LegalCopyright,
			&this->LegalTrademarks,
			&this->OriginalFilename,
			&this->PrivateBuild,
			&this->ProductName,
			&this->ProductVersion,
			&this->SpecialBuild,
			nullptr // Sentinel
		};
		static_assert(ARRAYSIZE(ppInLabels) == ARRAYSIZE(ppOutStrings), "Not same size!!");

		CString strSubBlock;
		for (int i = 0; ppInLabels[i]; ++i)
		{
			strSubBlock = strSubBlockBase + ppInLabels[i];
			if (::VerQueryValue(&buf[0], strSubBlock, &pItem, &itemSize))
			{
				(*ppOutStrings[i]) = static_cast<LPCTSTR>(pItem);
			}
			else
			{
				ppOutStrings[i]->Empty();
			}
		}
		return true;
	}


}// end of namespace

