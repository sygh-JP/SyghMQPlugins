#pragma once
#include <afxwin.h>

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

private:
	MQCDocument* m_pDocument = nullptr;
public:
	void SetDocument(MQCDocument* pDoc)
	{
		this->m_pDocument = pDoc;
	}
private:
	float m_ddvLeftTopU;
	float m_ddvLeftTopV;
	float m_ddvRightBottomU;
	float m_ddvRightBottomV;
private:
	MQCoordinate m_originalBBoxMin = { 0, 0 };
	MQCoordinate m_originalBBoxMax = { 0, 0 };
public:
	afx_msg void OnBnClickedButtonGetClipboard();
	afx_msg void OnBnClickedButtonSetClipboard();
};
