# �������� (�������å����¸)

# �ǥե���ȤΥ������å�̾
$default_target = "xqt"

#
# ʣ���������ƥ�����򥵥ݡ���
#
#  use_multi_config �� true �ˤ���ȡ�ʣ���������ƥ�����ؤ��б�
#  ���Ԥ��롣false �ξ��Ȱʲ��Τ褦�ʰ㤤�����롣
# 
#   1) �ѥå������Υ��󥹥ȡ��뤵���ǥ��쥯�ȥ�ϡ�
#      "pkg/" ���� "pkg/[�������å�̾]/" ���ѹ�����롣
#
#   2) ����ե�����̾�� config.def ���� config-[�������å�̾].def ��
#      �ѹ�����롣
#
$use_multi_config = false

#
# fakeroot
#
#  �ѥå������������� fakeroot ����Ѥ��뤫�ɤ����λ���
#

#$fakeroot = "fakeroot"


#
# tarball �Υ�������ɻ���
#
#  ������ tarball �ϡ��̾�� def �ե�����˻��ꤵ�줿 URL ����
#  ��������ɤ���롣
#
#  ���ꤵ�줿 URL ������˥�������ɤ������� URL ��������
#  (ex. �ǥ����ȥ�ӥ塼�����Υǥե���� tarball �֤��줬����)��
#  $site_default �� URL �򵭺ܤ��Ƥ����� xpkg �Ϥ��� URL ����
#  �ǽ�˥�������ɤ��ߤ롣
#
#  �ޤ���def �ե�����˻��ꤵ�줿 URL ����ե����뤬���������
#  �Ǥ��ʤ��ä����(�ե����뤬���Ǥ˾ü����Ƥ���ʤ�)���ե�����Хå�
#  �Ѥ� URL ����ꤹ�뤳�Ȥ��Ǥ��롣
#  �ե�����Хå��Ѥ� URL �� $site_fallback �˻��ꤹ�뤳�ȡ�

# tarball �Υǥե���ȥ���������� URL
#$site_default = ""

# tarball �Υե�����Хå������������ URL
$site_fallback = "http://xqt.sourceforge.jp/distfiles/"


#
# feed URL
#
#  ����ѥ���Ѥߥѥå����������ꤹ�뤿��� URL �򵭽Ҥ���
#

# ����ѥ���Ѥߥѥå������� feed URL
$feed_url = "http://xqt.sourceforge.jp/feed-testing/"

# ����ѥ���Ѥ߳�ȯ�ѥå������� feed URL
$devel_feed_url = "http://xqt.sourceforge.jp/feed-devel/"
