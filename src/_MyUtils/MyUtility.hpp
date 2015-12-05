#pragma once

#if 0
#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string  tstring;
#endif
#endif

#pragma comment(lib, "version.lib") // VerQueryValue, GetFileVersionInfo


namespace MyUtils
{

	template<class Type> inline void Swap(Type& a, Type& b)
	{
		Type c = a;
		a = b;
		b = c;
	}

	template<class Type> inline const Type& Clamp(const Type& a, const Type& Min, const Type& Max)
	{
		if (a < Min) return Min;
		if (a > Max) return Max;
		return a;
	}

#if 0
	extern bool GetDlgItemReal(HWND hWnd, int ItemID, double& Real);
	extern bool GetDlgItemReal(HWND hWnd, int ItemID, float& Real);
	extern bool SetDlgItemReal(HWND hWnd, int ItemID, double Real);
	extern bool SetDlgItemReal(HWND hWnd, int ItemID, float Real);

	extern bool GetFloatValuesFromEditBox(HWND hWnd, const int EntryNum, const int pIdTable[], float pData[]);
#endif

	extern bool GetClipboardText(CString& outString);
	extern bool SetClipboardText(HWND hWndNewOwner, LPCTSTR pSrcStr);

	//using TFloatArray = std::vector<float>;

	//! @brief  ホワイトスペースで区切られた文字列の各フィールドをそれぞれ浮動小数点数に変換。<br>
	//! ホワイトスペースとは、ASCII の半角スペース 0x20 やタブ文字 0x09、改行 0x0a など。<br>
	template<typename T> std::vector<T> SplitStringIntoArray(LPCTSTR inString, size_t maxFieldsCount)
	{
		// C++ のストリームは型安全性やバッファ安全性が高い一方で処理速度がとんでもなく遅いので、大量にデータを処理するのには向かない。
		std::vector<T> outArray;
		std::basic_istringstream<TCHAR> inStream(inString);
		while (true)
		{
			T temp;
			inStream >> temp;
			if (inStream.fail())
			{
				break;
			}
			else
			{
				outArray.push_back(temp);
			}
			if (outArray.size() >= maxFieldsCount)
			{
				break;
			}
		}
		return outArray;
	}

	template<typename T> CString ConvertArrayToString(const std::vector<T>& inArray, LPCTSTR separator = _T(" "))
	{
		std::basic_ostringstream<TCHAR> outStream;
		for (const auto& val : inArray)
		{
			outStream << val;
			outStream << separator;
		}
		return outStream.str().c_str();
	}


	class VersionInfoStringPack
	{
	public:
		CString
			Comments,
			CompanyName,
			FileDescription,
			FileVersion,
			InternalName,
			LegalCopyright,
			LegalTrademarks,
			OriginalFilename,
			PrivateBuild,
			ProductName,
			ProductVersion,
			SpecialBuild;
	public:
		VersionInfoStringPack() {}
	public:
		bool QueryVersionInfoAsStrings(LPCTSTR pModuleFilePath);
	};

}
