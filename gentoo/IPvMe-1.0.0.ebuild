# Copyright 1999-2022 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=8
DESCRIPTION="Find your computer's WAN IP addresses."
HOMEPAGE="https://github.com/rhymeswithmogul/${PN}"
SRC_URI="https://github.com/rhymeswithmogul/${PN}/archive/v${PV}.tar.gz" -> "${PF}.tar.gz"
LICENSE="AGPL-3 AGPL-3+"
SLOT="0"
KEYWORDS="amd64 ~arm ~arm64 ~ppc ~ppc-macos ~ppc64 ~s390x x86"
IUSE="+ipv4 +ipv6 +threads debug"

src_configure() {
	econf \
		$(use_enable ipv4) \
		$(use_enable ipv6) \
		$(use_enable threads) \
		$(use_enable debug)
}
