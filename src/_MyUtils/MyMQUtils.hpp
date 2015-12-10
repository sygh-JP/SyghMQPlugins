#pragma once

namespace MyMQUtils
{
	inline void UpdateMinOrMax(float value, float& inoutMin, float& inoutMax)
	{
		if (inoutMin > value)
		{
			inoutMin = value;
		}
		else if (inoutMax < value)
		{
			inoutMax = value;
		}
	}

	template<typename TFunctor> void ScanAllFaces(MQCDocument* doc, const TFunctor& func)
	{
		const int objCount = doc->GetObjectCount();
		for (int objIndex = 0; objIndex < objCount; ++objIndex)
		{
			auto* obj = doc->GetObject(objIndex);
			if (obj == nullptr)
			{
				continue;
			}
			const int faceCount = obj->GetFaceCount();
			for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex)
			{
				const int facePointCount = obj->GetFacePointCount(faceIndex);
				func(objIndex, faceIndex, facePointCount);
			}
		}
	}

	inline int GetAliveObjectsCount(MQCDocument* doc)
	{
		const int totalCount = doc->GetObjectCount();
		int aliveCount = 0;
		for (int i = 0; i < totalCount; ++i)
		{
			auto* obj = doc->GetObject(i);
			if (obj != nullptr)
			{
				++aliveCount;
			}
		}
		return aliveCount;
	}

	inline int GetAliveMaterialsCount(MQCDocument* doc)
	{
		// GetMaterialCount() は単に配列のサイズを返すだけらしい。
		// マテリアルを追加した後、マテリアルをすべて削除した状態でも、Compaction が実行されるまで非ゼロが返ってくる。
		// 有効なマテリアルの数を調べるには、全要素に対して NULL チェックが必要。
		const int totalCount = doc->GetMaterialCount();
		int aliveCount = 0;
		for (int i = 0; i < totalCount; ++i)
		{
			auto* mat = doc->GetMaterial(i);
			if (mat != nullptr)
			{
				++aliveCount;
			}
		}
		return aliveCount;
	}

	//! @brief  カレントオブジェクトかつカレントマテリアルに対して、UV 選択されている頂点に対する共通ループ操作を定義する。<br>
	//! @return  ループの最後まで到達し、スキャンが成功したか否か。<br>
	//! @tparam  writebackUV  UV 座標の変更（書き戻し）を行なうか否か。<br>
	template<bool writebackUV, typename TFunctor> bool CommonUVScanLoopImpl(MQCDocument* doc, const TFunctor& func)
	{
		// 1つのオブジェクトに異なる2つ以上のマテリアルが割り当てられている可能性に注意してスキャンする。
		const int currentObjIndex = doc->GetCurrentObjectIndex();
		const int currentMatIndex = doc->GetCurrentMaterialIndex();
		if (currentMatIndex == -1)
		{
			// カレントマテリアルが無効＝マテリアルがひとつも無い。すなわちすべてが未着色面。
			// 本来は「カレントマテリアルが無効＝マテリアルが存在しない」とはかぎらないが、
			// Metasequoia では一応それが成り立つ仕様になっているはず。
			return false;
		}
		auto* obj = doc->GetObject(currentObjIndex);
		if (obj == nullptr)
		{
			return false;
		}
		// Metasequoia 3 までは四角形面までしか対応していなかったが、Metasequoia 4 からは五角形以上もサポートされるようになった。
		// したがって固定長配列では対応不能。
		// 通例、面数は 3 or 4 が一般的であることから、可変長配列を4要素で初期化しておき、後は resize() による自動伸長に任せる。
		std::vector<MQCoordinate> uvArray(4);
		const int faceCount = obj->GetFaceCount();
		for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex)
		{
			if (obj->GetFaceMaterial(faceIndex) != currentMatIndex)
			{
				// カレントマテリアルでない場合。
				continue;
			}
			const auto facePointCount = obj->GetFacePointCount(faceIndex);
			if (facePointCount < 1)
			{
				continue;
			}
			uvArray.resize(facePointCount);
#if 1
			obj->GetFaceCoordinateArray(faceIndex, &uvArray[0]);
			for (int k = 0; k < facePointCount; ++k)
			{
				if (doc->IsSelectUVVertex(currentObjIndex, faceIndex, k))
				{
					func(uvArray[k]);
				}
			}
			// テンプレート非型引数による分岐。静的分岐に帰結するので、コンパイル時の展開最適化に期待する。
			if (writebackUV)
			{
				obj->SetFaceCoordinateArray(faceIndex, &uvArray[0]);
			}
#else
			// こちらだと処理をカスタマイズできる領域が大きくなるが、カスタマイズする側は
			// (MQCObject* obj, int currentObjIndex, int faceIndex, int facePointCount, MQCoordinate uvArray[])
			// を引数に持つ関数オブジェクトを渡さないといけない。引数が多いと分かりづらくなる。
			func(obj, currentObjIndex, faceIndex, facePointCount, &uvArray[0]);
#endif
		}
		return true;
	}

	// @brief  カレントオブジェクトかつカレントマテリアルに対して、UV 選択された頂点の最小・最大 UV 値を取得する。<br>
	// @return  UV 選択された頂点が少なくとも1つ以上存在するか否か。<br>
	inline bool CalcMinMaxOfUVSelectedVertices(MQCDocument* doc, MQCoordinate& minVal, MQCoordinate& maxVal)
	{
		bool isFirstValue = true;
		return CommonUVScanLoopImpl<false>(doc,
			[&](MQCoordinate& uv)
		{
			if (isFirstValue)
			{
				isFirstValue = false;
				minVal = maxVal = uv;
			}
			else
			{
				UpdateMinOrMax(uv.u, minVal.u, maxVal.u);
				UpdateMinOrMax(uv.v, minVal.v, maxVal.v);
			}
		}) && !isFirstValue;
	}
}
