#pragma once

#ifndef __AFXWIN_H__
#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h" // メイン シンボル

#include "../_MyUtils/MyUtility.hpp"


class CSyghMQUVBBoxApp : public CWinApp
{
public:
	CSyghMQUVBBoxApp();

	// オーバーライド
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};

namespace MyPluginCoreFuncs
{
	extern bool FitBox(MQCDocument* doc, MQCoordinate min1, MQCoordinate max1, MQCoordinate min2, MQCoordinate max2);
}
