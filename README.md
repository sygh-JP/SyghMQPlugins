# SyghMQPlugins
Metasequoia plugins by sygh.

3Dモデリングソフトウェア「Metasequoia」用の自作プラグイン集です。主にUV編集関連のツール類が含まれます。
(Custom plugins for 3-D modeling software "Metasequoia". Mainly UV-edit tools are included.)

* Metasequoia公式サイト (Metasequoia official site):
http://www.metaseq.net/jp/

## 開発環境 (Development Environment)
* Microsoft Visual Studio 2013 Professional
* Microsoft Windows 8.1 x64 Japanese
* Metasequoia Plugin SDK 2.4.9c

## ターゲットアプリケーション (Target Applications)
* Metasequoia 3.x
* Metasequoia 4.x (32-bit/64-bit)

## ビルド方法 (How to Build)
下記の旧バージョンSDK配布ページから、"mqsdk249c.zip"をダウンロードします。
(Download "mqsdk249c.zip" from the following URL.)
http://www.metaseq.net/jp/download/sdk/2/

アーカイブに含まれているMQ\*.h, MQ\*.cppファイルを、Gitクローンした際に作成される"src/_mqsdk"フォルダーに配置します。
なお、プラグインSDKに含まれるソースコードのライセンス形態が不明なため、これらのファイルはGitリポジトリには含めていません。
(Copy MQ\*.h and MQ\*.cpp contained in the archive to the folder "src/_mqsdk" created by Git clone. Please note that those files are not included in my Git repository because the license type of the source code in the plugin SDK is unclear.)

Visual Studio 2013でビルドします。MFCを使用しているため、Professional/Community以上のエディションが必須となります。
(Build by Visual Studio 2013. Professional/Community edition or higher is required due to MFC.)

開発環境の選定理由やコーディングルールなどに関してはWikiを参照してください。
(Please see Wiki page about the reason why the development tools are selected and about my coding rules.)

## メンテナンス方針 (Maintenance Policy)
もともと趣味で2007年～2008年に開発していたプラグインを、旧バージョンおよび新バージョンのMetasequoiaユーザー向け、あるいはこれからプラグインの開発を学習する開発者向けにオープンソース化することを目的としています。したがって、基本的にAS-ISで提供するものとし、新機能の追加は行ないません。互換性の維持とバグ修正を最重要視しています。(Originally I developed these plugins for do-it-yourself in 2007 and 2008, and these are published as an open-source project for end-users of old and new versions of Metasequoia and for new plugin developers now. Therefore these are provided "AS IS", and no new function will be appended basically. The most important points in this project are compatibility and bug-fixing.)

2015-12-08現在、プラグインSDKの配布ページには、
「作成したプラグインは自由に配布することができますが、Metasequoia本体 (Standard/EXなど全エディションを含む) に搭載された機能と競合することを目的とした等価なプラグインを作成して配布することを禁止します。」
という注意書きが記載されています。
(As at 2015-12-08, the following caution is stated in the page for distribution of Metasequoia plugin SDKs.)
"You can freely distribute the plug-ins that you created. However, if the plug-in features are to compete with the function of Metasequoia(All editions, including Standard/EX), distribution is prohibited."

本ソフトウェアに関しては、Metasequoia 4.5.3時点の機能と競合する部分はないものとみなしています。また当然ですが、本ソフトウェアはMetasequoiaの開発・販売を妨害する目的で開発しているものでもありません。後からMetasequoiaが同等機能を取り込む形で実装した場合にプラグインのソースコードおよびバイナリの公開を停止しなければならないのかどうかは不明ですが、tetraface Inc.から公開停止の通達や依頼が来ないかぎりGitHub上にて本ソフトウェアの公開を続けることにします。とはいえ、いかなる理由があろうとも自然発生的な競合プラグインの開発までをも禁止することはソフトウェアの健全な発展を阻害するものであるため、前述の条文における適用対象の明確化や緩和措置などがなされていくべきだと思われます。
(This software is considered uncompetitive with the functions of Metasequoia 4.5.3. Of course this software is not intended as a roadblock to the development and sales of Metasequoia. Currently it is unclear whether it should be stopped or not releasing the source code and binaries of any predecessor plugins if Metasequoia implements the equivalent functions as successor, but I will to keep this software published on GitHub unless required to stop by tetraface Inc. However, it would inhibit the strong advancement of software to prohibit all distribution of naturally-arising competitive plugins despite the reason, so the scope of the cited limitation should be defined clearly and any easing measures should be implemented in the future.)

2015-12-08, sygh.
