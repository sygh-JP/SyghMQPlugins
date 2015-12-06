// MainDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SyghMQUVSelectUtil.h"
#include "MainDlg.h"


// CMainDlg ダイアログ

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
END_MESSAGE_MAP()


// CMainDlg メッセージ ハンドラ

void CMainDlg::OnOK()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	ASSERT(this->m_pDocument != nullptr);

	// ダイアログ上の情報を使って DDValue を更新。
	this->UpdateData(true);

	try
	{
		const auto retId = this->GetCheckedRadioButton(IDC_RADIO_ADD_SELECT_FACE2UV, IDC_RADIO_CLEAR_ALL_UV_SELECT);
		switch (retId)
		{
		case IDC_RADIO_ADD_SELECT_FACE2UV:
			MyPluginCoreFuncs::SelectFace2UV(m_pDocument);
			break;
		case IDC_RADIO_ADD_SELECT_UV2FACE:
			MyPluginCoreFuncs::SelectUV2Face(m_pDocument);
			break;
		case IDC_RADIO_CLEAR_ALL_UV_SELECT:
			MyPluginCoreFuncs::ClearUVSelect(m_pDocument);
			break;
		default:
			ASSERT(false);
			break;
		}
	}
	catch (const CString& err)
	{
		AfxMessageBox(err, MB_OK | MB_ICONERROR);
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

	// HACK: ドキュメント中にオブジェクトが含まれること、また UV マッピング設定のマテリアルが含まれることなどを事前確認するとよい。

	// ラジオボタンの選択の初期化。
	this->CheckDlgButton(IDC_RADIO_ADD_SELECT_FACE2UV, BST_CHECKED);

	TRACE(_T("Plugin-DLL dialog initialization finished.\n"));

	return true;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
