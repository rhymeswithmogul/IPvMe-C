# IPvMe

A simple command-line app to find your computer's WAN IP address(es), courtesy of [ip6.me](https://ip6.me).

## Help

Just run it and it works.  You may also use the `--ipv4` and `--ipv6` options to fine-tune things.

## Examples

Run it normally:

```console
$ ./ipvme
IPv4 address = 192.0.2.1
IPv6 address = 2001:db8::1
```

Or with a switch:

```console
$ ./ipvme -6
IPv6 address = 2001:db8::1
```

## Legal and Privacy Notices

This app is not endorsed by (or known by) whomever runs the fine service over at [ip6.me](https://ip6.me).

This app uses version 1.1 of the [ip6.me <abbr title="Application Programming Interface">API</abbr>](https://ip6.me/api/docs/).  Obviously, using this app will leak your IP address(es) to the remote server.