#pragma once

#ifndef __AFXWIN_H__
#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h" // メイン シンボル

#include "../_MyUtils/MyUtility.hpp"


class CSyghMQUVImplantApp : public CWinApp
{
public:
	CSyghMQUVImplantApp();

	// オーバーライド
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};

namespace MyPluginCoreFuncs
{
	extern void ImplantUV(MQCDocument* pDoc, int srcIndex, int dstIndex);
}
