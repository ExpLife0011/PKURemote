PKURemote
==========

![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Environment](https://img.shields.io/badge/Windows-XP,%20Vista,%207,%208,%2010-brightgreen.svg)
[![Release](https://img.shields.io/github/release/osvt/pkuremote.svg)](https://github.com/osvt/pkuremote/releases)
![License](https://img.shields.io/github/license/osvt/pkuremote.svg)
![Downloads](https://img.shields.io/github/downloads/osvt/pkuremote/latest/total.svg)
![TotalDownloads](https://img.shields.io/github/downloads/osvt/pkuremote/total.svg)

**PKURemote** is a port rewritting utility to modify the source or destination port for packets on Windows. It is based on MFC UI. It uses [**WinDivert**](https://github.com/basil00/Divert) for packet manipulations.

## Build

Use ``Visual Studio 2005`` and later to build this project.

## Usage

1. 32位系统使用PKURemote_x86文件夹，64位系统使用PKURemote_x64文件夹；

2. 以Administrator管理员权限，运行上述文件夹中的PKURemote.exe，在界面中，选择四台服务器中的一台；点击“连接”；

3. 打开VMware Workstation并选择“文件”，“连接服务器”（或直接打开VMware vSphere Client）；在“服务器名称”输入example.com,用户名输入user，密码输入password，进行连接；

4. 一台电脑同时只能连接一个服务器，若要切换服务器，先点击“断开”，选择新的服务器并点击“连接”，再在VMware里断开并重新连接，即可切换到新的服务器；

5. 任选一个服务器连接后可同时实现到\\\\example.com文件服务器的连接。




