// MainDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SyghMQUVMatrix.h"
#include "MainDlg.h"
#include "../_MyUtils/MyMQUtils.hpp"


// CMainDlg ダイアログ

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
	, m_ddvScalingCenterU(0)
	, m_ddvScalingCenterV(0)
	, m_ddvScalingU(0)
	, m_ddvScalingV(0)
	, m_ddvRotCenterU(0)
	, m_ddvRotCenterV(0)
	, m_ddvRotAngleDegrees(0)
	, m_ddvTranslationU(0)
	, m_ddvTranslationV(0)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCALING_CENTER_U, m_ddvScalingCenterU);
	DDX_Text(pDX, IDC_EDIT_SCALING_CENTER_V, m_ddvScalingCenterV);
	DDX_Text(pDX, IDC_EDIT_SCALING_U, m_ddvScalingU);
	DDX_Text(pDX, IDC_EDIT_SCALING_V, m_ddvScalingV);
	DDX_Text(pDX, IDC_EDIT_ROT_CENTER_U, m_ddvRotCenterU);
	DDX_Text(pDX, IDC_EDIT_ROT_CENTER_V, m_ddvRotCenterV);
	DDX_Text(pDX, IDC_EDIT_ROT_ANGLE_DEGREES, m_ddvRotAngleDegrees);
	DDX_Text(pDX, IDC_EDIT_TRANSLATION_U, m_ddvTranslationU);
	DDX_Text(pDX, IDC_EDIT_TRANSLATION_V, m_ddvTranslationV);
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

	// ダイアログ上の情報を使って DDValue を更新。
	if (!this->UpdateData(true))
	{
		// 確定しない。
		return;
	}

	try
	{
		SRTMatrixParam matrixParam;
		matrixParam.ScalingCenter.u = m_ddvScalingCenterU;
		matrixParam.ScalingCenter.v = m_ddvScalingCenterV;
		matrixParam.Scaling.u = m_ddvScalingU;
		matrixParam.Scaling.v = m_ddvScalingV;
		matrixParam.RotCenter.u = m_ddvRotCenterU;
		matrixParam.RotCenter.v = m_ddvRotCenterV;
		matrixParam.RotAngleDegrees = m_ddvRotAngleDegrees;
		matrixParam.Translation.u = m_ddvTranslationU;
		matrixParam.Translation.v = m_ddvTranslationV;

		MyPluginCoreFuncs::ApplyUVSRTMatrix(m_pDocument, matrixParam);
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

	MQCoordinate originalBBoxMin, originalBBoxMax;
	if (!MyMQUtils::CalcMinMaxOfUVSelectedVertices(m_pDocument, originalBBoxMin, originalBBoxMax))
	{
		AfxMessageBox(_T("頂点が UV 選択されていません。"), MB_OK | MB_ICONWARNING);
		this->OnCancel();
		return true;
	}

	SRTMatrixParam matrixParam;
	matrixParam.InitializeByMinMax(originalBBoxMin, originalBBoxMax);

	m_ddvScalingCenterU = matrixParam.ScalingCenter.u;
	m_ddvScalingCenterV = matrixParam.ScalingCenter.v;
	m_ddvScalingU = matrixParam.Scaling.u;
	m_ddvScalingV = matrixParam.Scaling.v;
	m_ddvRotCenterU = matrixParam.RotCenter.u;
	m_ddvRotCenterV = matrixParam.RotCenter.v;
	m_ddvRotAngleDegrees = matrixParam.RotAngleDegrees;
	m_ddvTranslationU = matrixParam.Translation.u;
	m_ddvTranslationV = matrixParam.Translation.v;

	// DDValue を使ってダイアログ上の情報を更新。
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

	const size_t MaxFieldsCount = 9;
	const auto paramArray = MyUtils::SplitStringIntoArray<float>(strTemp, MaxFieldsCount);
	if (paramArray.size() != MaxFieldsCount)
	{
		AfxMessageBox(_T("クリップボードに 9 個の数値フィールドを含むテキストが格納されていません。"), MB_OK | MB_ICONERROR);
		return;
	}

	m_ddvScalingCenterU = paramArray[0];
	m_ddvScalingCenterV = paramArray[1];
	m_ddvScalingU = paramArray[2];
	m_ddvScalingV = paramArray[3];
	m_ddvRotCenterU = paramArray[4];
	m_ddvRotCenterV = paramArray[5];
	m_ddvRotAngleDegrees = paramArray[6];
	m_ddvTranslationU = paramArray[7];
	m_ddvTranslationV = paramArray[8];

	// DDValue を使ってダイアログ上の情報を更新。
	this->UpdateData(false);
}


void CMainDlg::OnBnClickedButtonSetClipboard()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	// ダイアログ上の情報を使って DDValue を更新。
	if (!this->UpdateData(true))
	{
		// 確定しない。
		return;
	}

	// 無印 VC 2015 (MFC 14.0) にはおかしなバグがある模様。 
	// UpdateData(true) を呼び出すと、特定の float/double 型 DDV 変数の数値が「5」になってしまう。
	// どうやら CRT のバグ起因らしい。
	// Update 1 以降でバグ修正されるかもしれないが、
	// VC 2015 の CRT には他にもバグがあるので（例えば putwchar で非 ASCII 文字が正常に出力できないとか）、
	// UWP アプリや DirectX 12 アプリをどうしても開発したいなどの目的がないかぎり、
	// 当面は VC 2013 より前の処理系を使ったほうがよさそう。
	// ちなみに Visual C++ 2015 では[構成プロパティ]→[リンカー]→[デバッグ]→[デバッグ情報の生成]の設定に互換性のない仕様変更が加わっている模様。
	// MSBuild もしくは IDE 側のバグかもしれない。プラットフォーム ツールセットを 2013 に戻す場合は要注意。

	const auto strTemp = MyUtils::ConvertArrayToString<float>(
	{
		m_ddvScalingCenterU,
		m_ddvScalingCenterV,
		m_ddvScalingU,
		m_ddvScalingV,
		m_ddvRotCenterU,
		m_ddvRotCenterV,
		m_ddvRotAngleDegrees,
		m_ddvTranslationU,
		m_ddvTranslationV,
	}, _T("\n"));

	if (!MyUtils::SetClipboardText(this->GetSafeHwnd(), strTemp))
	{
		AfxMessageBox(_T("クリップボードへの書き込みに失敗しました。"), MB_OK | MB_ICONERROR);
	}
}
