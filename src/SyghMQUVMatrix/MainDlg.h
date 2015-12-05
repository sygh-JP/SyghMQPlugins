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
	float m_ddvScalingCenterU;
	float m_ddvScalingCenterV;
	float m_ddvScalingU;
	float m_ddvScalingV;
	float m_ddvRotCenterU;
	float m_ddvRotCenterV;
	float m_ddvRotAngleDegrees;
	float m_ddvTranslationU;
	float m_ddvTranslationV;
private:
	//SRTMatrixParam m_matrixParam;
public:
	afx_msg void OnBnClickedButtonGetClipboard();
	afx_msg void OnBnClickedButtonSetClipboard();
};
