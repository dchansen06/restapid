# restapid
A short program to provide basic system information over http as a rest-api

## Security
Do not use on a network where not every device is fully trusted, and do not open it up to the internet. This is not secure, but a nice tool for personal use. This program connects to the network.

## Installation
Keep in mind that to access ports below a certain number you must be root (or use sudo). Be very careful. Edit the `restapid.c` file to change the port and hostname as appropriate

Should you want to allow controlling the file with the various C programs, setup the `restapid.service` job unit file with the correct path and move it to `/etc/systemd/system/restapid.service`

See [systemd](https://wiki.debian.org/systemd) or [systemd(1)](https://man7.org/linux/man-pages/man1/systemd.1.html) for more information

## Dependencies
Requires ulfius library, can be installed with

```$ sudo apt install libulfius-dev```

Configure the temperature option as needed for your computer
