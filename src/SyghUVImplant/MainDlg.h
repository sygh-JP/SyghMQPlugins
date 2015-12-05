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
	//! @brief  UV 移植元のオブジェクトのリスト。<br>
	CComboBox m_ddxcComboObjSrc;
	//! @brief  UV 移植先のオブジェクトのリスト。<br>
	CComboBox m_ddxcComboObjDst;
};
