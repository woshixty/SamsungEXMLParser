#ifndef EXML_PARSER_H
#define EXML_PARSER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

#ifdef _WIN32
    #ifdef EXML_PARSER_EXPORTS
        #define EXML_PARSER_API __declspec(dllexport)
    #else
        #define EXML_PARSER_API __declspec(dllimport)
    #endif
#else
    #define EXML_PARSER_API __attribute__((visibility("default")))
#endif

namespace exml {

// 前向声明
class EXMLParserImpl;

/**
 * @brief 应用项目类型枚举
 */
enum class ItemType {
    FAVORITE,   // 应用图标
    FOLDER,     // 文件夹
    APPWIDGET   // 小部件
};

/**
 * @brief 应用项目结构体
 */
struct EXML_PARSER_API Item {
    ItemType type;
    std::string packageName;
    std::string className;
    std::string title;           // 文件夹标题
    int screen;                  // 屏幕索引
    int x, y;                    // 位置坐标
    int spanX, spanY;            // 跨度（小部件用）
    int appWidgetID;             // 小部件ID
    int options;                 // 选项
    int color;                   // 颜色
    bool hidden;                 // 是否隐藏
    std::vector<Item> favorites; // 文件夹内的应用列表
    
    Item();
    ~Item();
    Item(const Item& other);
    Item& operator=(const Item& other);
};

/**
 * @brief 布局配置结构体
 */
struct EXML_PARSER_API LayoutConfig {
    int rows;
    int columns;
    int pageCount;
    int screenIndex;
    std::string category;
    std::string folderGrid;
    bool restoreMaxSizeGrid;
    std::string zeroPageContents;
    std::string selectedMinusonePackage;
    bool zeroPage;
    bool notificationPanelSetting;
    bool lockLayoutSetting;
    bool quickAccessFinder;
    int badgeOnOffSetting;
    bool onlyPortraitModeSetting;
    bool addIconToHomeSetting;
    bool suggestedApps;
    int expandHotseatSize;
    std::string homeGridList;
    std::string appsGridList;
    std::string viewTypeAppOrder;
    
    LayoutConfig();
};

/**
 * @brief EXML解析器类
 */
class EXML_PARSER_API EXMLParser {
public:
    EXMLParser();
    ~EXMLParser();
    
    // 禁用拷贝构造和赋值
    EXMLParser(const EXMLParser&) = delete;
    EXMLParser& operator=(const EXMLParser&) = delete;
    
    /**
     * @brief 从文件加载EXML数据
     * @param filePath EXML文件路径
     * @return 是否加载成功
     */
    bool loadFromFile(const std::string& filePath);
    
    /**
     * @brief 保存EXML数据到文件
     * @param filePath 目标文件路径
     * @return 是否保存成功
     */
    bool saveToFile(const std::string& filePath);
    
    /**
     * @brief 获取布局配置
     * @return 布局配置引用
     */
    const LayoutConfig& getLayoutConfig() const;
    
    /**
     * @brief 设置布局配置
     * @param config 新的布局配置
     */
    void setLayoutConfig(const LayoutConfig& config);
    
    // Home区域操作
    /**
     * @brief 获取Home区域的页面数量
     * @return 页面数量
     */
    size_t getHomePageCount() const;
    
    /**
     * @brief 获取指定页面的所有项目
     * @param pageIndex 页面索引
     * @return 项目列表
     */
    std::vector<Item> getHomeItems(int pageIndex) const;
    
    /**
     * @brief 在Home区域添加项目
     * @param pageIndex 页面索引
     * @param item 要添加的项目
     * @return 是否添加成功
     */
    bool addHomeItem(int pageIndex, const Item& item);
    
    /**
     * @brief 从Home区域删除项目
     * @param pageIndex 页面索引
     * @param packageName 包名
     * @param className 类名
     * @return 是否删除成功
     */
    bool removeHomeItem(int pageIndex, const std::string& packageName, const std::string& className);
    
    /**
     * @brief 移动Home区域项目
     * @param fromPage 源页面
     * @param fromIndex 源索引
     * @param toPage 目标页面
     * @param toIndex 目标索引
     * @return 是否移动成功
     */
    bool moveHomeItem(int fromPage, int fromIndex, int toPage, int toIndex);
    
    /**
     * @brief 查找Home区域项目
     * @param packageName 包名
     * @return 找到的项目列表
     */
    std::vector<Item> findHomeItems(const std::string& packageName) const;
    
    // Hotseat区域操作
    /**
     * @brief 获取Hotseat区域的项目数量
     * @return 项目数量
     */
    size_t getHotseatItemCount() const;
    
    /**
     * @brief 获取Hotseat区域的所有项目
     * @return 项目列表
     */
    std::vector<Item> getHotseatItems() const;
    
    /**
     * @brief 在Hotseat区域添加项目
     * @param item 要添加的项目
     * @return 是否添加成功
     */
    bool addHotseatItem(const Item& item);
    
    /**
     * @brief 从Hotseat区域删除项目
     * @param packageName 包名
     * @param className 类名
     * @return 是否删除成功
     */
    bool removeHotseatItem(const std::string& packageName, const std::string& className);
    
    /**
     * @brief 移动Hotseat区域项目
     * @param fromIndex 源索引
     * @param toIndex 目标索引
     * @return 是否移动成功
     */
    bool moveHotseatItem(int fromIndex, int toIndex);
    
    /**
     * @brief 查找Hotseat区域项目
     * @param packageName 包名
     * @return 找到的项目列表
     */
    std::vector<Item> findHotseatItems(const std::string& packageName) const;
    
    // HomeOnly区域操作
    /**
     * @brief 获取HomeOnly区域的页面数量
     * @return 页面数量
     */
    size_t getHomeOnlyPageCount() const;
    
    /**
     * @brief 获取指定页面的所有项目
     * @param pageIndex 页面索引
     * @return 项目列表
     */
    std::vector<Item> getHomeOnlyItems(int pageIndex) const;
    
    /**
     * @brief 在HomeOnly区域添加项目
     * @param pageIndex 页面索引
     * @param item 要添加的项目
     * @return 是否添加成功
     */
    bool addHomeOnlyItem(int pageIndex, const Item& item);
    
    /**
     * @brief 从HomeOnly区域删除项目
     * @param pageIndex 页面索引
     * @param packageName 包名
     * @param className 类名
     * @return 是否删除成功
     */
    bool removeHomeOnlyItem(int pageIndex, const std::string& packageName, const std::string& className);
    
    /**
     * @brief 移动HomeOnly区域项目
     * @param fromPage 源页面
     * @param fromIndex 源索引
     * @param toPage 目标页面
     * @param toIndex 目标索引
     * @return 是否移动成功
     */
    bool moveHomeOnlyItem(int fromPage, int fromIndex, int toPage, int toIndex);
    
    /**
     * @brief 查找HomeOnly区域项目
     * @param packageName 包名
     * @return 找到的项目列表
     */
    std::vector<Item> findHomeOnlyItems(const std::string& packageName) const;
    
    // AppOrder区域操作
    /**
     * @brief 获取AppOrder区域的项目数量
     * @return 项目数量
     */
    size_t getAppOrderItemCount() const;
    
    /**
     * @brief 获取AppOrder区域的所有项目
     * @return 项目列表
     */
    std::vector<Item> getAppOrderItems() const;
    
    /**
     * @brief 在AppOrder区域添加项目
     * @param item 要添加的项目
     * @return 是否添加成功
     */
    bool addAppOrderItem(const Item& item);
    
    /**
     * @brief 从AppOrder区域删除项目
     * @param packageName 包名
     * @param className 类名
     * @return 是否删除成功
     */
    bool removeAppOrderItem(const std::string& packageName, const std::string& className);
    
    /**
     * @brief 移动AppOrder区域项目
     * @param fromIndex 源索引
     * @param toIndex 目标索引
     * @return 是否移动成功
     */
    bool moveAppOrderItem(int fromIndex, int toIndex);
    
    /**
     * @brief 查找AppOrder区域项目
     * @param packageName 包名
     * @return 找到的项目列表
     */
    std::vector<Item> findAppOrderItems(const std::string& packageName) const;
    
    /**
     * @brief 清空所有数据
     */
    void clear();

private:
    std::unique_ptr<EXMLParserImpl> pImpl;
};

} // namespace exml

#endif // EXML_PARSER_H
