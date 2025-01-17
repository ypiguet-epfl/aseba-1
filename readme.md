

# TDM modifications

This repository contains experiments with the TDM (Thymio Device Manager).

## Command-line options

The following command-line options are supported:
- `--log L`: specifies the log level (trace, debug, info, warn, error, or critical). All messages associated with this importance and above are displayed. The default is `--log trace`, i.e. display all log messages. A more sensible value is `--log info`, which displays important information such as the TCP port number or connections of robots or client applications but not every minor update.
- `--nozcpublish`: disables service publishing with zeroconf
- `--tcpport N`: specifies the TCP port the TDM will accept plain TCP connections on, typically from Thymio Suite, VPL, Studio, or the Python package `tdmclient`. `N` should be an integer number between 1024 and 65535 corresponding to an unused TCP port. The default is to use an ephemeral port, i.e. to let the system find an unused port.
- `--tcpport no`: no TCP port opened for plain TCP connections.
- `--wsport N`: specifies the TCP port the TDM will accept WebSocket connections on, typically from VPL 3, Blockly, or Scratch. `N` should be an integer number between 1024 and 65535 corresponding to an unused TCP port. The default is 8597.
- `--wsport no`: no TCP port opened for WebSockets connections.

## Static build with make

- Make a new directory to download all that's required and build the tdm:
    ```
    mkdir tdm-build
    cd tdm-build
    ```
- Clone the repository and all its submodules:
    ```
    git clone --recurse-submodules https://github.com/ypiguet-epfl/aseba-1.git aseba
    ```
- Get other dependencies:
    - Boost: download Boost 1.73 from [https://www.boost.org/users/history/](https://www.boost.org/users/history/) (pick `boost_1_73_0.tar.gz` or `boost_1_73_0.zip` and uncompress it into `tdm-build`, so that its directory is `tdm-build/boost_1_73_0`).
        On Ubuntu, you can get it with
        ```
        sudo apt install -y wget
        wget -c https://boostorg.jfrog.io/artifactory/main/release/1.73.0/source/boost_1_73_0.tar.gz
        tar xfz boost_1_73_0.tar.gz
        ```
    - TartanLlama/expected: clone it with
        ```
        git clone https://github.com/TartanLlama/expected
        ```
    - OpenSSL: OpenSSL is required only if you include support for firmware upgrades. Still in `tdm-build`, clone OpenSSL and build it with
        ```
        git clone --recurse-submodules https://github.com/openssl/openssl.git
        (cd openssl; ./Configure; make -j)
        ```

- Build flatbuffer with `cmake`. On Mac, get it at [https://cmake.org/download/](https://cmake.org/download/). Then
    ```
    (cd aseba/third_party/flatbuffers/; cmake .; make -j)
    ```
    If you get errors, try with the current version of flatc:
    ```
    (cd aseba/third_party; rm -Rf flatbuffers; git clone https://github.com/google/flatbuffers.git)
    (cd aseba/third_party/flatbuffers/; cmake .; make -j)
    ```

- On Ubuntu, install `libudev-dev`:
    ```
    sudo apt install -y libudev-dev
    ```

- Build the TDM. By default, no zeroconf, firmware upgrade and IPv6 support is included; read below to know why.
    ```
    make -f aseba/newbuild/Makefile -j
    ```
    To include zeroconf and firmware upgrade support:
    ```
    HAS_ZEROCONF=YES MOBSYA_TDM_ENABLE_TCP=YES HAS_FIRMWARE_UPGRADE=YES make -f aseba/newbuild/Makefile -j
    ```

## Build without zeroconf

On Linux, the TDM relies on the Avahi implementation of zeroconf, with `aware` and possibly other code to provide compatibility with Bonjour, the Apple zeroconf implementation. The exact details aren't fully understood yet. That's the reason why the zeroconf functionality in the TDM has been made optional with `#ifdef HAS_ZEROCONF`.

If `HAS_ZEROCONF` is undefined, the TDM is compiled without zeroconf. The Makefile checks if the environment variable `HAS_ZEROCONF` is defined and compiles everything accordingly, including the files `aseba_tcpacceptor.cpp` and `aseba_tcpacceptor.h` only with zeroconf (they support connections to the simulator Aseba Playground whose TCP port is also advertised by zeroconf). Note that `aseba_tcpacceptor.cpp` also requires `MOBSYA_TDM_ENABLE_TCP` (see next section). So to include zeroconf (especially on macOS), you can type
```
HAS_ZEROCONF=TRUE make -f aseba/newbuild/Makefile -j
```
To compile the TDM without zeroconf, just do it as explained in the previous section.

Currently, the TDM without zeroconf is invisible to Thymio Suite which relies on zeroconf to discover the TDM TCP port number. But you can do it using command-line tools, as explained above in section _Command-line options_.

When zeroconf is available, command-line option `--nozcpublish` disables zeroconf service publishing.

## Build without Aseba over TCP

If `MOBSYA_TDM_ENABLE_TCP` is undefined, the TDM is compiled without support for connection to robots via TCP (usually the Aseba Playground simulator).

## Zeroconf advertisement with command-line tools

Unless you specify it with `--tcpport`, the TCP port changes every time the TDM is launched. It's displayed by the TDM shortly after it's launched, on a line like this:
```
[2021-05-19 11:17:20.781] [console] [trace]    main.cpp@L85:  => TCP Server connected on 44125
```
Many lines follow quickly; maybe it's easier to clear the terminal with `clear`, type `tdm` and scroll up as soon as you get the first lines displayed. Or even better, reduce the amount of information displayed with `--log info`. In the line above, the TCP port is 44125. The WebSocket port (WS Server) is always 8597, unless changed with `--wsport`.

On Linux with Avahi, you can advertise the TDM service as follows (replace 44125 with the value you found):
```
avahi-publish -s TDM _mobsya._tcp 44125 ws-port=8597 uuid=`uuidgen`
```

To check:
```
avahi-browse _mobsya._tcp
```

With bonjour on Mac:
```
dns-sd -R TDM _mobsya._tcp local 44125 ws-port=8597 uuid=`uuidgen`
```

To check:
```
dns-sd -Z _mobsya._tcp local
```

Then Thymio Suite, or the Python package `tdmclient`, should find your TDM.

## Build without firmware upgrade

The TDM relies on OpenSSL to retrieve firmware upgrades via https (it connects to [https://www.mobsya.org/update/Thymio2-firmware-meta.xml](https://www.mobsya.org/update/Thymio2-firmware-meta.xml) to find the https url of the latest firmware release). OpenSSL is huge, more than half of the TDM. In case the firmware upgrade capability is not desired, an optional constant `HAS_FIRMWARE_UPGRADE` has been added added: it should be defined only _to include_ firmware upgrade support.

By default, firmware upgrade and OpenSSL are not included; OpenSSL is not a required dependency anymore. To build the TDM with firmware upgrade support, you can type
```
HAS_FIRMWARE_UPGRADE=TRUE make -f aseba/newbuild/Makefile -j
```

## Build without IPv6

The TDM can listen on both IPv6 and IPv4. This has caused unexpected issues on Linux when IPv6 isn't available. Support for IPv6 is enabled only if the optional constant `HAS_IPV6` is defined. By default, it is not, therefore IPv6 is disabled.

To build the TDM with support for IPv6, you can type
```
HAS_IPV6=TRUE make -f aseba/newbuild/Makefile -j
```

## Build without TCP or WebSocket services

Flatbuffer TCP and WebSocket server functionality has been bracketed by `#ifdef`/`#endif` with constants `HAS_FB_TCP` and `HAS_FB_WS`, respectively. These constants are defined in the Makefile, so these services _are enabled_. The main purpose is to highlight which part of the code is relevant to these services.

## Log

A command-line option `--log` has been added to reduce the amount of information displayed. Some log output has been changed between `trace` and `info`.

The TDM service uid is displayed with level `info`.

## Build with docker

A basic Dockerfile is provided to build `tdm` in [docker](https://docs.docker.com/get-started/) with the latest Ubuntu image. For the moment, it just downloads and builds everything.

Here are the steps you could follow:
- Install docker (you can find it on [hub.docker.com](https://hub.docker.com/search/?offering=community&type=edition)).
- In a terminal, cd to `aseba/newbuild`
- Build the image:
    ```
    docker build -t "tdm:Dockerfile" .
    ```
- Once everything has been run, you can find the tdm in the image. Either open an interactive shell:
    ```
    docker run -it tdm:Dockerfile /bin/bash
    cd /home/tdm
    ```
    or copy the file from the container (`docker ps -l --format '{{.ID}}'` get the id of the last container):
    ```
    docker cp `docker ps -l --format '{{.ID}}'`:/home/tdm/tdm .
    ```
