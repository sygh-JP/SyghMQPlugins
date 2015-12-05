#pragma once



// すでに開発したプラグインの ID を enum で網羅的に列挙しておけば、
// 別の新しいプラグインを開発するとき、ミスによって重複することが無くなるが、
// ヘッダーを変更するたびに以前のプラグインのソースに影響することになるので面倒。

static const DWORD MY_PRODUCT_ID = 0x0973e500; // 他の開発者と同じ ID を使わないようにする。
static const DWORD MY_PLUGIN_ID_BASE = 0x0973e500; // このベース ID にオフセットを加えて、各プラグインの ID とする。
