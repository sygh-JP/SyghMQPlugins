// MainDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SyghMQUV2SVG.h"
#include "MainDlg.h"


// CMainDlg ダイアログ

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
	, m_ddvOutputSvgFilePath(_T(""))
	, m_ddvOutputsGuide(FALSE)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SVG_FILE_PATH, m_ddvOutputSvgFilePath);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_ddxcSpinWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_ddxcSpinHeight);
	DDX_Check(pDX, IDC_CHECK_OUTPUT_GUIDE, m_ddvOutputsGuide);
	DDX_Control(pDX, IDC_COMBOEX_LINE_COLOR, m_ddxcComboExLineColor);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SVG_FILE_PATH_REF, &CMainDlg::OnBnClickedButtonSvgFilePathRef)
END_MESSAGE_MAP()


namespace
{
	const LPCTSTR DefaultSvgColorName = _T("black");
}

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

	if (m_ddvOutputSvgFilePath.IsEmpty())
	{
		AfxMessageBox(_T("出力ファイル名が空です。"), MB_OK | MB_ICONWARNING);
		return;
	}

	try
	{
		TCoord2DArray uv2DArray;
		// UV 解析
		MyPluginCoreFuncs::BuildUVList(this->m_pDocument, uv2DArray);

		CString colorName = DefaultSvgColorName; // 万が一選択されていない場合、デフォルト色を設定。
		const int curIndex = m_ddxcComboExLineColor.GetCurSel();
		if (curIndex != CB_ERR)
		{
			m_ddxcComboExLineColor.GetLBText(curIndex, colorName);
		}

		// HACK: 線幅の変更も実装する？

		const SVGOption option{ m_ddxcSpinWidth.GetPos32(), m_ddxcSpinHeight.GetPos32(), 0.5f, !!m_ddvOutputsGuide, colorName };

		// SVG 出力
		MyPluginCoreFuncs::SaveSVG(m_ddvOutputSvgFilePath, uv2DArray, option);
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

void CMainDlg::OnBnClickedButtonSvgFilePathRef()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	CFileDialog saveDlg(false, _T(".svg"), nullptr, OFN_OVERWRITEPROMPT,
		_T("Scalable Vector Graphics (*.SVG)|*.svg|All Files (*.*)|*.*||"), this, 0, true);

	if (saveDlg.DoModal() == IDOK)
	{
		// All Files (*.*) を選択したとき、ピリオド＋デフォルト拡張子が余計に追加されてしまう。
		// HACK: CFileDialog を継承して OnTypeChange をオーバーライドしたクラスを作ったほうがいいかも。
		m_ddvOutputSvgFilePath = saveDlg.GetPathName();
		// DDXCtrl・DDValue からダイアログへ設定
		this->UpdateData(false);
	}
}

namespace
{
	void MakeSolidBitmap(CWnd* pWnd, CBitmap& bmp, COLORREF color, int width, int height)
	{
		// MFC は Win32 API の薄いラッパーでしかないので、Borland の VCL に比べてかなり分かりづらい。

		auto* pWndDC = pWnd->GetDC();
		CDC dc;

		dc.CreateCompatibleDC(pWndDC);

		bmp.CreateCompatibleBitmap(pWndDC, width, height);

		auto* pOldBmp = dc.SelectObject(&bmp);

		// 塗りつぶすだけ。
		dc.FillSolidRect(0, 0, width, height, color);

		dc.SelectObject(pOldBmp);

		pWnd->ReleaseDC(pWndDC);
	}

	void CreateSvgColorMap(TStringToColorRefMap& outMap)
	{
		outMap.clear();

		// COLORREF の色の並びは各 8bit で RGBX の順。RGBQUAD の BGRX とは異なる。
		// RGB() マクロを使ったほうが楽。

		outMap[_T("red")] = RGB(255, 0, 0);
		outMap[_T("lime")] = RGB(0, 255, 0);
		outMap[_T("blue")] = RGB(0, 0, 255);
		outMap[_T("cyan")] = RGB(0, 255, 255);
		outMap[_T("magenta")] = RGB(255, 0, 255);
		outMap[_T("yellow")] = RGB(255, 255, 0);
		outMap[_T("black")] = RGB(0, 0, 0);
		outMap[_T("white")] = RGB(255, 255, 255);

		// 要素は first (key) の辞書順でソートされる。
	}
}

void CMainDlg::SetupCmbExLineColor()
{
	CreateSvgColorMap(m_svgColors);

	// Bitmap は ImageList に追加した後に破棄してかまわないが、
	// ImageList は ComboBoxEx に設定した後に破棄してはいけない。

	// イメージリストの作成
	m_imageList.Create(16, 16, ILC_COLOR24, static_cast<int>(m_svgColors.size()), 0);
	for (auto& scPair : m_svgColors)
	{
		CBitmap bmp;
		MakeSolidBitmap(this, bmp, scPair.second, 16, 16);
		m_imageList.Add(&bmp, COLORREF());
	}

	m_ddxcComboExLineColor.SetImageList(&m_imageList);

	// 拡張コンボボックスに項目を設定
	COMBOBOXEXITEM item = {};

	item.mask = CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

	int index = 0;
	for (auto& scPair : m_svgColors)
	{
		item.iItem = index;
		item.iImage = index;
		item.iSelectedImage = index;
		item.pszText = const_cast<LPTSTR>(scPair.first.GetString());
		m_ddxcComboExLineColor.InsertItem(&item);
		++index;
	}
#if 0
	// CComboBoxEx では使えない。
	const int bkIndex = m_ddxcComboExLineColor.SelectString(-1, DefaultSvgColorName);
	m_ddxcComboExLineColor.SetCurSel((CB_ERR != bkIndex) ? bkIndex : 0);
#else
	CString str;
	const int comboElemCount = m_ddxcComboExLineColor.GetCount();
	for (int i = 0; i < comboElemCount; ++i)
	{
		// 特定のテキストが設定されている項目を選択する。
		m_ddxcComboExLineColor.GetLBText(i, str);
		if (str == DefaultSvgColorName)
		{
			m_ddxcComboExLineColor.SetCurSel(i);
			break;
		}
	}
#endif
}


BOOL CMainDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  ここに初期化を追加してください

	ASSERT(this->m_pDocument != nullptr);

	this->SetupCmbExLineColor();

	// CSpinButtonCtrl::SetRange32() の上下限値は、VC6 (MFC 4.2) では 16bit 仕様の SetRange() と同じく
	// UD_MAXVAL, UD_MINVAL（それぞれ 0x7fff, -0x7fff）となっている。(commctrl.h)
	// VC9 (MFC 9) では改善されている。MFC 7, 7.1, 8 は未確認。
	m_ddxcSpinWidth.SetRange32(1, 99999);
	m_ddxcSpinHeight.SetRange32(1, 99999);
	m_ddxcSpinWidth.SetPos32(256);
	m_ddxcSpinHeight.SetPos32(256);
	m_ddvOutputsGuide = true;

	// DDXCtrl・DDValue からダイアログへ設定
	this->UpdateData(false);

	TRACE(_T("Plugin-DLL dialog initialization finished.\n"));

	return true;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
