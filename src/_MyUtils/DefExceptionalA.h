#pragma once

// プラグイン DLL 側を Unicode ビルドするために仕方なく、
// MQInit() で使われている LoadLibrary だけを強制的に LoadLibraryA に置換する。
// SDK 2.4.9c 側の問題であり、SDK 3.1.1 でも修正されていないが、SDK 4.5.1 時点では修正されている模様。
// 
// MQInit.cpp を編集することなく対処するために、ソースファイルごとの個別プロジェクト設定で
// [構成プロパティ]→[C/C++]→[詳細設定]→[必ず使用されるインクルード ファイル]
// によって stdafx.h とともにインクルードヘッダーを強制的に差し込む方法もあるが、
// MQInit.cpp をインクルードするソースファイルを別途用意したほうがよい。
#ifdef LoadLibrary
#undef LoadLibrary
#define LoadLibrary LoadLibraryA
#endif
