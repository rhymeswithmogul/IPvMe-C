# Copyright 1999-2022 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8
DESCRIPTION="Find your computer's WAN IP addresses."
HOMEPAGE="https://github.com/rhymeswithmogul/IPvMe-C"
SRC_URI="https://github.com/rhymeswithmogul/IPvMe-C/archive/v${PV}.tar.gz" -> "${PF}.tar.gz"
LICENSE="AGPL-3 AGPL-3+"
SLOT="0"
KEYWORDS="amd64 arm arm64 ppc ~ppc-macos ppc64 s390x x86"
IUSE="+ipv4 +ipv6 +json +threads +xml debug"

src_configure() {
	econf \
		$(use_enable ipv4) \
		$(use_enable ipv6) \
		$(use_enable json) \
		$(use_enable threads) \
		$(use_enable xml) \
		$(use_enable debug)
}
