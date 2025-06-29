# SamsungEXMLParser

三星手机桌面布局备份文件解析库，用于读写和操作 `.exml` 格式的桌面布局备份文件。

## 功能特性

- ✅ 解析三星手机桌面布局备份文件（.exml格式）
- ✅ 支持 Home、Hotseat、HomeOnly、AppOrder 四个主要区域的操作
- ✅ 提供完整的增删改查API
- ✅ 支持文件夹和小部件的解析
- ✅ 基于 tinyxml2 库进行XML处理
- ✅ 跨平台支持（Windows、Linux、macOS）

## 项目结构

```
SamsungEXMLParser/
├── CMakeLists.txt          # CMake构建配置
├── include/
│   └── exml_parser.h       # 公共头文件
├── src/
│   └── exml_parser.cpp     # 实现文件
├── tests/
│   └── test_main.cpp       # 测试程序
├── samples/
│   └── template.exml       # 示例文件
└── README.md              # 项目说明
```

## 构建要求

- CMake 3.16 或更高版本
- C++17 兼容的编译器
- tinyxml2 库（自动下载）

## 构建方法

### 1. 克隆项目
```bash
git clone <repository-url>
cd SamsungEXMLParser
```

### 2. 创建构建目录
```bash
mkdir build
cd build
```

### 3. 配置和构建
```bash
cmake ..
cmake --build .
```

### 4. 运行测试
```bash
./bin/test_exml_parser
```

## 使用示例

### 基本用法

```cpp
#include "exml_parser.h"
#include <iostream>

int main() {
    exml::EXMLParser parser;
    
    // 加载EXML文件
    if (parser.loadFromFile("backup.exml")) {
        std::cout << "文件加载成功" << std::endl;
        
        // 获取Home区域信息
        size_t pageCount = parser.getHomePageCount();
        std::cout << "Home页面数量: " << pageCount << std::endl;
        
        // 获取第一页的所有项目
        auto items = parser.getHomeItems(0);
        for (const auto& item : items) {
            std::cout << "应用: " << item.packageName << std::endl;
        }
        
        // 添加新项目
        exml::Item newItem;
        newItem.type = exml::ItemType::FAVORITE;
        newItem.packageName = "com.example.app";
        newItem.className = "com.example.app.MainActivity";
        newItem.screen = 0;
        newItem.x = 1;
        newItem.y = 1;
        
        parser.addHomeItem(0, newItem);
        
        // 保存修改后的文件
        parser.saveToFile("modified.exml");
    }
    
    return 0;
}
```

### 高级操作

```cpp
// 查找特定应用
auto foundItems = parser.findHomeItems("com.whatsapp");

// 移动项目
parser.moveHomeItem(0, 0, 1, 2); // 从第0页第0个位置移动到第1页第2个位置

// 删除项目
parser.removeHomeItem(0, "com.example.app", "com.example.app.MainActivity");

// 操作Hotseat区域
parser.addHotseatItem(newItem);
auto hotseatItems = parser.getHotseatItems();

// 操作HomeOnly区域
parser.addHomeOnlyItem(0, newItem);
auto homeOnlyItems = parser.getHomeOnlyItems(0);

// 操作AppOrder区域
parser.addAppOrderItem(newItem);
auto appOrderItems = parser.getAppOrderItems();
```

## API 参考

### 主要类

#### `exml::EXMLParser`
主要的解析器类，提供所有操作接口。

#### `exml::Item`
表示桌面上的一个项目（应用图标、文件夹或小部件）。

#### `exml::LayoutConfig`
存储布局配置信息。

### 主要方法

#### 文件操作
- `loadFromFile(const std::string& filePath)` - 从文件加载数据
- `saveToFile(const std::string& filePath)` - 保存数据到文件

#### Home区域操作
- `getHomePageCount()` - 获取页面数量
- `getHomeItems(int pageIndex)` - 获取指定页面的项目
- `addHomeItem(int pageIndex, const Item& item)` - 添加项目
- `removeHomeItem(int pageIndex, const std::string& packageName, const std::string& className)` - 删除项目
- `moveHomeItem(int fromPage, int fromIndex, int toPage, int toIndex)` - 移动项目
- `findHomeItems(const std::string& packageName)` - 查找项目

#### Hotseat区域操作
- `getHotseatItemCount()` - 获取项目数量
- `getHotseatItems()` - 获取所有项目
- `addHotseatItem(const Item& item)` - 添加项目
- `removeHotseatItem(const std::string& packageName, const std::string& className)` - 删除项目
- `moveHotseatItem(int fromIndex, int toIndex)` - 移动项目
- `findHotseatItems(const std::string& packageName)` - 查找项目

#### HomeOnly区域操作
- `getHomeOnlyPageCount()` - 获取页面数量
- `getHomeOnlyItems(int pageIndex)` - 获取指定页面的项目
- `addHomeOnlyItem(int pageIndex, const Item& item)` - 添加项目
- `removeHomeOnlyItem(int pageIndex, const std::string& packageName, const std::string& className)` - 删除项目
- `moveHomeOnlyItem(int fromPage, int fromIndex, int toPage, int toIndex)` - 移动项目
- `findHomeOnlyItems(const std::string& packageName)` - 查找项目

#### AppOrder区域操作
- `getAppOrderItemCount()` - 获取项目数量
- `getAppOrderItems()` - 获取所有项目
- `addAppOrderItem(const Item& item)` - 添加项目
- `removeAppOrderItem(const std::string& packageName, const std::string& className)` - 删除项目
- `moveAppOrderItem(int fromIndex, int toIndex)` - 移动项目
- `findAppOrderItems(const std::string& packageName)` - 查找项目

## 支持的文件格式

本库支持三星手机使用 HomeUp 程序备份的桌面布局文件格式（.exml），包含以下区域：

- **Home**: 主屏幕区域，支持多页面
- **Hotseat**: 底部快捷栏
- **HomeOnly**: 仅主屏幕模式
- **AppOrder**: 应用抽屉排序

## 注意事项

1. 确保输入的EXML文件格式正确
2. 修改后的文件建议先备份原文件
3. 某些特殊的小部件可能需要额外的配置
4. 文件夹内的应用列表会被完整保留

## 许可证

本项目采用 MIT 许可证。

## 贡献

欢迎提交 Issue 和 Pull Request 来改进这个项目。