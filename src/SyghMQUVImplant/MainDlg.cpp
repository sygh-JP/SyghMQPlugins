// MainDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SyghMQUVImplant.h"
#include "MainDlg.h"
#include "../_MyUtils/MyMQUtils.hpp"


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
	DDX_Control(pDX, IDC_COMBO_OBJ_SRC, m_ddxcComboObjSrc);
	DDX_Control(pDX, IDC_COMBO_OBJ_DST, m_ddxcComboObjDst);
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
		MyPluginCoreFuncs::ImplantUV(this->m_pDocument, m_ddxcComboObjSrc.GetCurSel(), m_ddxcComboObjDst.GetCurSel());
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

	const int objCount = MyMQUtils::GetAliveObjectsCount(m_pDocument);
	if (objCount <= 1)
	{
		AfxMessageBox(_T("ドキュメント内のオブジェクト数が 1 以下です。UV 移植できません。"), MB_OK | MB_ICONWARNING);
		this->OnCancel();
		return true;
	}
	const int matCount = MyMQUtils::GetAliveMaterialsCount(m_pDocument);
	if (matCount <= 0)
	{
		AfxMessageBox(_T("ドキュメント内にマテリアルが存在しません。UV 移植できません。"), MB_OK | MB_ICONWARNING);
		this->OnCancel();
		return true;
	}

	// コンボボックスにオブジェクト名を追加
	const int MaxNameLength = 256;
	char objName[MaxNameLength] = {};
	for (int i = 0; i < objCount; ++i)
	{
		auto* obj = m_pDocument->GetObject(i);
		if (obj == nullptr)
		{
			continue;
		}
		// Metasequoia 4 でも内部コードは ANSI のままらしい。
		obj->GetName(objName, MaxNameLength - 1);
		const CString strObjName(objName);
		const auto& addStringToCombo = [](CComboBox& combo, const CString& str)
		{
			const int errCode = combo.AddString(str);
			// 成功したときの戻り値はインデックスとなる。CB_OKAY ではない。
			return errCode != CB_ERR && errCode != CB_ERRSPACE;
		};
		if (!addStringToCombo(m_ddxcComboObjSrc, strObjName))
		{
			AfxMessageBox(_T("プラグイン ダイアログの初期化中にエラーが発生しました。"), MB_OK | MB_ICONERROR);
			this->OnCancel();
			return true;
		}
		if (!addStringToCombo(m_ddxcComboObjDst, strObjName))
		{
			AfxMessageBox(_T("プラグイン ダイアログの初期化中にエラーが発生しました。"), MB_OK | MB_ICONERROR);
			this->OnCancel();
			return true;
		}
	}
	m_ddxcComboObjSrc.SetCurSel(0);
	m_ddxcComboObjDst.SetCurSel(0);

	TRACE(_T("Plugin-DLL dialog initialization finished.\n"));

	return true;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
