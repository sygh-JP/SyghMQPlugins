// MainDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SyghUVBBox.h"
#include "MainDlg.h"
#include "../_MyUtils/MyMQUtils.hpp"


// CMainDlg ダイアログ

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
	, m_ddvLeftTopU(0)
	, m_ddvLeftTopV(0)
	, m_ddvRightBottomU(0)
	, m_ddvRightBottomV(0)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LEFT_TOP_U, m_ddvLeftTopU);
	DDX_Text(pDX, IDC_EDIT_LEFT_TOP_V, m_ddvLeftTopV);
	DDX_Text(pDX, IDC_EDIT_RIGHT_BOTTOM_U, m_ddvRightBottomU);
	DDX_Text(pDX, IDC_EDIT_RIGHT_BOTTOM_V, m_ddvRightBottomV);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_GET_CLIPBOARD, &CMainDlg::OnBnClickedButtonGetClipboard)
	ON_BN_CLICKED(IDC_BUTTON_SET_CLIPBOARD, &CMainDlg::OnBnClickedButtonSetClipboard)
END_MESSAGE_MAP()


// CMainDlg メッセージ ハンドラ

void CMainDlg::OnOK()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	ASSERT(this->m_pDocument != nullptr);

	// ダイアログから DDXCtrl・DDValue へ設定
	if (!this->UpdateData(true))
	{
		// 確定しない。
		return;
	}

	try
	{
		const MQCoordinate inputBBoxMin(m_ddvLeftTopU, m_ddvLeftTopV);
		const MQCoordinate inputBBoxMax(m_ddvRightBottomU, m_ddvRightBottomV);
		MyPluginCoreFuncs::FitBox(m_pDocument, m_originalBBoxMin, m_originalBBoxMax, inputBBoxMin, inputBBoxMax);
	}
	catch (tstring& err)
	{
		AfxMessageBox(err.c_str(), MB_OK | MB_ICONERROR);
		return;
	}
	catch (...)
	{
		AfxMessageBox(_T("不明なエラーが発生しました。"), MB_OK | MB_ICONERROR);
		return;
	}

	__super::OnOK();
}

BOOL CMainDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  ここに初期化を追加してください

	ASSERT(this->m_pDocument != nullptr);

	if (!MyMQUtils::CalcMinMaxOfUVSelectedVertices(m_pDocument, m_originalBBoxMin, m_originalBBoxMax))
	{
		AfxMessageBox(_T("頂点が UV 選択されていません。"), MB_OK | MB_ICONWARNING);
		this->OnCancel();
		return true;
	}

	m_ddvLeftTopU = m_originalBBoxMin.u;
	m_ddvLeftTopV = m_originalBBoxMin.v;
	m_ddvRightBottomU = m_originalBBoxMax.u;
	m_ddvRightBottomV = m_originalBBoxMax.v;

	// DDXCtrl・DDValue からダイアログへ設定
	this->UpdateData(false);

	TRACE(_T("Plugin-DLL dialog initialization finished.\n"));

	return true;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CMainDlg::OnBnClickedButtonGetClipboard()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	CString strTemp;
	if (!MyUtils::GetClipboardText(strTemp))
	{
		AfxMessageBox(_T("クリップボードにテキストが格納されていません。"), MB_OK | MB_ICONERROR);
		return;
	}

	const size_t MaxFieldsCount = 4;
	const auto paramArray = MyUtils::SplitStringIntoArray<float>(strTemp, MaxFieldsCount);
	if (paramArray.size() != MaxFieldsCount)
	{
		AfxMessageBox(_T("クリップボードに 4 数値フィールドを含むテキストが格納されていません。"), MB_OK | MB_ICONERROR);
		return;
	}

	m_ddvLeftTopU = paramArray[0];
	m_ddvLeftTopV = paramArray[1];
	m_ddvRightBottomU = paramArray[2];
	m_ddvRightBottomV = paramArray[3];

	// DDXCtrl・DDValue からダイアログへ設定
	this->UpdateData(false);
}


void CMainDlg::OnBnClickedButtonSetClipboard()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	// ダイアログから DDXCtrl・DDValue へ設定
	if (!this->UpdateData(true))
	{
		// 確定しない。
		return;
	}

	const auto strTemp = MyUtils::ConvertArrayToString<float>(
	{
		m_ddvLeftTopU,
		m_ddvLeftTopV,
		m_ddvRightBottomU,
		m_ddvRightBottomV,
	}, _T("\n"));

	if (!MyUtils::SetClipboardText(this->GetSafeHwnd(), strTemp))
	{
		AfxMessageBox(_T("クリップボードへの書き込みに失敗しました。"), MB_OK | MB_ICONERROR);
	}
}
