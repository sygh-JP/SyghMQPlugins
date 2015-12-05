#pragma once
#include <afxcmn.h>


using TStringToColorRefMap = std::map<CString, COLORREF>;


// CMainDlg ダイアログ

class CMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	explicit CMainDlg(CWnd* pParent = nullptr);   // 標準コンストラクタ
	virtual ~CMainDlg();

	// ダイアログ データ
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	afx_msg void OnBnClickedButtonSvgFilePathRef();
private:
	CSpinButtonCtrl m_ddxcSpinWidth;
	CSpinButtonCtrl m_ddxcSpinHeight;
	CComboBoxEx m_ddxcComboExLineColor;
private:
	CString m_ddvOutputSvgFilePath;
	BOOL m_ddvOutputsGuide;
private:
	MQCDocument* m_pDocument = nullptr;
	CImageList m_imageList;
	TStringToColorRefMap m_svgColors;
private:
	void SetupCmbExLineColor();
public:
	void SetDocument(MQCDocument* pDoc)
	{
		this->m_pDocument = pDoc;
	}
};
