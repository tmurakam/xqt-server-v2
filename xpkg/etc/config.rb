# Common configurations (no depends any targets)

# Default target name
$default_target = "xqt"

#
# 複数アーキテクチャをサポート
#
#  use_multi_config を true にすると、複数アーキテクチャへの対応
#  が行われる。false の場合と以下のような違いがある。
# 
#   1) パッケージのインストールされるディレクトリは、
#      "pkg/" から "pkg/[ターゲット名]/" に変更される。
#
#   2) 設定ファイル名が config.def から config-[ターゲット名].def に
#      変更される。
#
$use_multi_config = false

#
# fakeroot
#
#  パッケージ作成時に fakeroot を使用するかどうかの指定
#

#$fakeroot = "fakeroot"


#
# tarball のダウンロード指定
#
#  ソース tarball は、通常は def ファイルに指定された URL から
#  ダウンロードされる。
#
#  指定された URL よりも先にダウンロードさせたい URL がある場合
#  (ex. ディストリビューションのデフォルト tarball 置き場がある)、
#  $site_default に URL を記載しておくと xpkg はこの URL から
#  最初にダウンロードを試みる。
#
#  また、def ファイルに指定された URL からファイルがダウンロード
#  できなかった場合(ファイルがすでに消失しているなど)、フォールバック
#  用の URL を指定することができる。
#  フォールバック用の URL は $site_fallback に指定すること。

# tarball のデフォルトダウンロード用 URL
#$site_default = ""

# tarball のフォールバックダウンロード用 URL
$site_fallback = "http://xqt.sourceforge.jp/distfiles/"


#
# feed URL
#
#  コンパイル済みパッケージを入手するための URL を記述する
#

# コンパイル済みパッケージの feed URL
$feed_url = "http://xqt.sourceforge.jp/feed-testing/"

# コンパイル済み開発パッケージの feed URL
$feed_devel_url = "http://xqt.sourceforge.jp/feed-devel/"

#
# feed upload 設定
#

# feed ホスト (ssh でログインできるホスト)
$feed_host = "shells.sourceforge.jp"

# feed ホスト上のパッケージディレクトリ
$feed_dir = "/home/groups/x/xq/xqt/htdocs/feed-testing"

# feed ホスト上の開発パッケージディレクトリ
$feed_devel_dir = "/home/groups/x/xq/xqt/htdocs/feed-devel"
