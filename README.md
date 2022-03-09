<h3 align="center">Foot-pedal configurator</h3>

<div align="center">

[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub issues](https://img.shields.io/github/issues/Schmoller/pedalctl)](https://github.com/Schmoller/pedalctl/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/Schmoller/pedalctl)](https://github.com/Schmoller/pedalctl/pulls)
[![GitHub](https://img.shields.io/github/license/Schmoller/pedalctl)](/LICENSE)

</div>

---

<p align="center"> A command line tool for configuring foot-pedal devices.
    <br> 
</p>

## 📝 Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [Supported Models](#supported_models)

## 🧐 About <a name = "about"></a>

This project provides a command line tool (pedalctl) to configure foot-pedal devices (such as those from iKKEGOL).

## 🏁 Getting Started <a name = "getting_started"></a>

These instructions will help you build and install the tool on your local machine.

### Prerequisites

To compile this project, you will need

- C++ 17 or higher toolchain
- CMake 3.0 or newer
- Git
- libusb-1.0

### Installing

#### Supported platforms

- Linux

Porting to other platforms should not be difficult as long as that platform supports libusb-1.0.

#### Building

As this is a CMake project, building should be fairly straightforward. Ensure that you have all required prerequisites
installed before proceeding.

Create a new directory to contain the build and navigate to it.

```
mkdir build
cd build
```

Initialize the CMake project. There are some options for you to configure in addition to the standard CMake options. See
below for available options.

```
cmake ..
```

Now you can build the tool using `make`

```
make
```

#### Installing

After you have compiled the tool, you can install it tool your system also using make:

```
sudo make install
```

If it has completed successfully, you can verify it by running:

```
pedalctl -v
```

It should print the version of the software

#### Available configuration options

| Option | Description | Default |
|--------|-----|-----|
|`INSTALL_UDEV_RULES`|When enabled, udev rules will be installed to allow non-root configuration of pedals|`ON`|
|`UDEV_RULES_DIR`|The directory to install udev rules to allow non-root configuration of foot pedals|`/etc/udev/rules.d/`|

## 🎈 Usage <a name="usage"></a>

The tool is a command line program `pedalctl`. The most detailed help is available using `pedalctl help`.

There are 3 commands offered by the tool:

```
pedalctl list
```

List all the available pedal devices

```
pedalctl show
```

Show the configuration of a single device.

```
pedalctl set
```

Updates the configuration of a device.

## ⌨️ Supported Models <a name="supported_models"></a>

- iKKEGOL
  - FS2020U1IR (3 pedal model)
  - FS2017U1IR (1 pedal model)
  - More to come

_For a device to be supported, the configuration protocol needs to be reverse engineered which requires someone working
on this project to have that device. Unless there happens to be a spec for the device._
