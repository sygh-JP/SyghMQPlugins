#pragma once

#ifndef __AFXWIN_H__
#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h" // メイン シンボル

#include "../_MyUtils/MyUtility.hpp"


class CSyghUVSelectUtilApp : public CWinApp
{
public:
	CSyghUVSelectUtilApp();

	// オーバーライド
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};

namespace MyPluginCoreFuncs
{
	extern void SelectFace2UV(MQCDocument* doc);
	extern void SelectUV2Face(MQCDocument* doc);
	extern void ClearUVSelect(MQCDocument* doc);
}
