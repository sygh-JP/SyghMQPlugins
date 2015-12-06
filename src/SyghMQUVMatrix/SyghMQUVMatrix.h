#pragma once

#ifndef __AFXWIN_H__
#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h" // メイン シンボル

#include "../_MyUtils/MyUtility.hpp"


class CSyghMQUVMatrixApp : public CWinApp
{
public:
	CSyghMQUVMatrixApp();

	// オーバーライド
public:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};


class SRTMatrixParam
{
	// MQCoordinate は怠惰なデフォルトコンストラクタを持つので、コンポジションする場合はきちんと初期化しておく。
public:
	MQCoordinate Cs = { 0, 0 };
	MQCoordinate Cr = { 0, 0 };
	MQCoordinate Scale = { 0, 0 };
	MQCoordinate Transl = { 0, 0 };
	float RotAngleDegrees = 0;
public:
	SRTMatrixParam() {}
public:
	void InitializeByCenter(MQCoordinate center)
	{
		this->Cs = this->Cr = center;
		this->RotAngleDegrees = 0.0f;
		this->Scale.u = this->Scale.v = 1.0f;
		this->Transl.u = this->Transl.v = 0.0f;
	}
	void InitializeByMinMax(MQCoordinate minVal, MQCoordinate maxVal)
	{
		// 中点で初期化。
		const MQCoordinate center = 0.5f * (minVal + maxVal);
		this->InitializeByCenter(center);
	}
};

namespace MyPluginCoreFuncs
{
	extern bool ApplyUVSRTMatrix(MQCDocument* doc, const SRTMatrixParam& inParam);
}
