# IPTV 工具集

一套用于管理和扫描 IPTV 频道的桌面应用程序，基于 Qt6 C++ 开发。

## 项目概述

本项目包含两个核心应用：

- **IPTVManager** - IPTV 频道管理工具，支持频道导入导出、流媒体探测、视频预览等功能
- **IPTVScan** - IPTV 频道扫描工具，用于批量检测 IPTV 流媒体地址的可用性

## 项目结构

```
IPTV/
├── IPTVManager/          # 频道管理应用
│   ├── src/              # 源代码
│   │   ├── core/         # 配置 + 日志模块
│   │   ├── database/     # 数据库访问层
│   │   ├── network/      # 网络请求
│   │   ├── multimedia/   # FFmpeg 集成
│   │   ├── export/       # 导出功能
│   │   ├── logic/        # 业务逻辑
│   │   ├── platform/     # 平台抽象
│   │   └── ui/           # 界面层
│   ├── tests/            # 单元测试
│   ├── resources/        # 资源文件
│   └── third_party/      # 第三方库
├── IPTVScan/             # 频道扫描应用
│   ├── main.cpp          # 程序入口
│   ├── mainpage.*        # 主界面
│   ├── mainthread.*      # 主线程
│   └── scanthread.*      # 扫描线程
├── ffmpeg/               # FFmpeg 预编译库
└── release/              # 发布相关
    ├── scripts/          # 构建脚本
    ├── icons/            # 应用图标
    └── output/           # 输出目录
```

## 功能特性

### IPTVManager

- **频道管理**：支持 M3U、TXT、CSV、XLSX 格式导入导出
- **流媒体探测**：使用 FFmpeg 检测流媒体地址可用性
- **视频预览**：基于 OpenGL 的实时视频预览
- **数据库存储**：SQLite 数据库存储频道信息
- **多平台支持**：Windows、macOS、Linux

### IPTVScan

- **批量扫描**：支持批量扫描 IPTV 流媒体地址
- **多线程**：可配置并发线程数量
- **地址模板**：支持带范围的地址模板（如 `239.49.{[1-255]}:{6000#[8000-9999]}`）
- **实时进度**：显示扫描进度和结果
- **结果导出**：支持将扫描结果导出为 TXT 文件

## 构建要求

### 开发环境

- **操作系统**：macOS 11.0+、Windows 10+、Linux
- **编译器**：支持 C++17 的编译器
- **构建工具**：CMake 3.19+
- **Qt 版本**：Qt 6.8+
- **FFmpeg**：avformat、avutil、avcodec、swscale

### 依赖安装

#### macOS

```bash
# 安装 Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装依赖
brew install cmake ffmpeg

# 安装 Qt 6.8
# 从 Qt 官网下载安装程序
```

#### Windows

```bash
# 安装 Qt 6.8
# 从 Qt 官网下载安装程序

# 安装 FFmpeg
# 从 https://ffmpeg.org/download.html 下载预编译版本

# 安装 CMake
# 从 https://cmake.org/download/ 下载安装
```

#### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install cmake qt6-base-dev libavformat-dev libavutil-dev libavcodec-dev libswscale-dev
```

## 使用说明

### IPTVManager

1. **首次启动**：配置数据库路径和 FFmpeg 路径
2. **导入频道**：支持按照指定格式导入频道
3. **探测流媒体**：选择频道后点击探测按钮检测可用性
4. **导出频道**：支持导出为多种格式

### IPTVScan

1. **输入地址**：在地址栏输入 IPTV 流媒体地址模板
2. **配置参数**：设置线程数量、超时时间等
3. **开始扫描**：点击开始按钮进行批量扫描
4. **查看结果**：实时查看扫描进度和成功/失败统计
5. **导出结果**：点击保存按钮导出扫描结果

### 地址模板语法

IPTVScan 支持复杂的地址模板：

- **单个值**：`1` 表示单个值
- **多个值**：`1#3` 表示 1 和 3
- **范围值**：`[8-10]` 表示 8、9、10
- **带前导零**：`[08-10]` 表示 08、09、10
- **混合使用**：`1#3#[5-7]` 表示 1、3、5、6、7

示例：
```
http://192.168.1.1:12345/udp/239.49.0.{[1-255]}:{6000#[8000-9999]}
```

## 测试

### IPTVManager 单元测试

```bash
cd IPTVManager
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build && ctest
```

## 发布

### 打包应用

```bash
cd release/scripts

# 打包所有应用
./deploy_macos.sh all

# 打包单个应用
./deploy_macos.sh iptvmanager
./deploy_macos.sh iptvscan
```

### 输出目录

```
release/output/
├── IPTVManager/
│   ├── IPTVManager.app
│   └── IPTVManager_vX.Y.Z_YYYYMMDD.dmg
└── IPTVScan/
    ├── IPTVScan.app
    └── IPTVScan_vX.Y_YYYYMMDD.dmg
```

## 版本历史

### IPTVManager v2.0.0 (2026-06-30)

- 模块化架构重构
- 8 个独立模块：core, database, network, multimedia, export, logic, platform, ui
- 跨平台 Excel 导出（QXlsx）
- 仓库模式数据库访问
- FFmpeg 自动发现

### IPTVScan v1.0.0 (2026-06-30)

- IPTV 频道扫描功能
- FFmpeg 流媒体检测
- Qt6 Widgets 界面

## 许可证

[待定]

## 贡献

欢迎提交 Issue 和 Pull Request。

## 联系方式

[待定]