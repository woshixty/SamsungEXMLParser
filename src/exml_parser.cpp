#include "exml_parser.h"
#include "tinyxml2.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace exml {

// Item结构体实现
Item::Item() : type(ItemType::FAVORITE), screen(0), x(0), y(0), spanX(1), spanY(1), 
               appWidgetID(0), options(0), color(-1), hidden(false) {}
Item::~Item() = default;
Item::Item(const Item& other) = default;
Item& Item::operator=(const Item& other) = default;

// LayoutConfig结构体实现
LayoutConfig::LayoutConfig() : rows(5), columns(4), pageCount(1), screenIndex(0),
                              restoreMaxSizeGrid(true), zeroPage(false),
                              notificationPanelSetting(true), lockLayoutSetting(false),
                              quickAccessFinder(true), badgeOnOffSetting(0),
                              onlyPortraitModeSetting(true), addIconToHomeSetting(false),
                              suggestedApps(true), expandHotseatSize(5) {}
LayoutConfig::~LayoutConfig() = default;
LayoutConfig::LayoutConfig(const LayoutConfig& other) = default;
LayoutConfig& LayoutConfig::operator=(const LayoutConfig& other) = default;

// 实现类
class EXMLParserImpl {
public:
    LayoutConfig layoutConfig;
    std::map<int, std::vector<Item>> homeItems;        // 按页面存储
    std::vector<Item> hotseatItems;
    std::map<int, std::vector<Item>> homeOnlyItems;    // 按页面存储
    std::vector<Item> appOrderItems;
    std::vector<Item> hotseatHomeOnlyItems;

    // 辅助函数
    static std::string getAttributeValue(const tinyxml2::XMLElement* element, const char* attrName, const std::string& defaultValue = "") {
        const char* value = element->Attribute(attrName);
        return value ? value : defaultValue;
    }

    static int getAttributeIntValue(const tinyxml2::XMLElement* element, const char* attrName, int defaultValue = 0) {
        const char* value = element->Attribute(attrName);
        return value ? std::stoi(value) : defaultValue;
    }

    static bool getAttributeBoolValue(const tinyxml2::XMLElement* element, const char* attrName, bool defaultValue = false) {
        const char* value = element->Attribute(attrName);
        if (!value) return defaultValue;
        std::string str(value);
        return str == "true" || str == "1";
    }

    Item parseItem(const tinyxml2::XMLElement* element) {
        Item item;
        
        // 确定项目类型
        const char* tagName = element->Value();
        if (strcmp(tagName, "favorite") == 0) {
            item.type = ItemType::FAVORITE;
        } else if (strcmp(tagName, "folder") == 0) {
            item.type = ItemType::FOLDER;
        } else if (strcmp(tagName, "appwidget") == 0) {
            item.type = ItemType::APPWIDGET;
        }
        
        // 解析基本属性
        item.packageName = getAttributeValue(element, "packageName");
        item.className = getAttributeValue(element, "className");
        item.title = getAttributeValue(element, "title");
        item.screen = getAttributeIntValue(element, "screen");
        item.x = getAttributeIntValue(element, "x");
        item.y = getAttributeIntValue(element, "y");
        item.spanX = getAttributeIntValue(element, "spanX", 1);
        item.spanY = getAttributeIntValue(element, "spanY", 1);
        item.appWidgetID = getAttributeIntValue(element, "appWidgetID");
        item.options = getAttributeIntValue(element, "options");
        item.color = getAttributeIntValue(element, "color", -1);
        item.hidden = getAttributeBoolValue(element, "hidden");
        
        // 解析文件夹内的favorites
        if (item.type == ItemType::FOLDER) {
            const tinyxml2::XMLElement* favorite = element->FirstChildElement("favorite");
            while (favorite) {
                Item favItem = parseItem(favorite);
                item.favorites.push_back(favItem);
                favorite = favorite->NextSiblingElement("favorite");
            }
        }
        
        return item;
    }

    void parseLayoutConfig(const tinyxml2::XMLElement* root) {
        // 解析基本配置
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("category")) {
            layoutConfig.category = elem->GetText() ? elem->GetText() : "";
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("FolderGrid")) {
            layoutConfig.folderGrid = elem->GetText() ? elem->GetText() : "";
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("restore_max_size_grid")) {
            const char* text = elem->GetText();
            layoutConfig.restoreMaxSizeGrid = text && (strcmp(text, "true") == 0);
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("zeroPageContents")) {
            layoutConfig.zeroPageContents = elem->GetText() ? elem->GetText() : "";
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("selectedMinusonePackage")) {
            layoutConfig.selectedMinusonePackage = elem->GetText() ? elem->GetText() : "";
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("zeroPage")) {
            const char* text = elem->GetText();
            layoutConfig.zeroPage = text && (strcmp(text, "true") == 0);
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("notification_panel_setting")) {
            const char* text = elem->GetText();
            layoutConfig.notificationPanelSetting = text && (strcmp(text, "true") == 0);
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("lock_layout_setting")) {
            const char* text = elem->GetText();
            layoutConfig.lockLayoutSetting = text && (strcmp(text, "true") == 0);
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("quick_access_finder")) {
            const char* text = elem->GetText();
            layoutConfig.quickAccessFinder = text && (strcmp(text, "true") == 0);
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("badge_on_off_setting")) {
            const char* text = elem->GetText();
            layoutConfig.badgeOnOffSetting = text ? std::stoi(text) : 0;
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("only_portrait_mode_setting")) {
            const char* text = elem->GetText();
            layoutConfig.onlyPortraitModeSetting = text && (strcmp(text, "true") == 0);
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("add_icon_to_home_setting")) {
            const char* text = elem->GetText();
            layoutConfig.addIconToHomeSetting = text && (strcmp(text, "true") == 0);
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("suggested_apps")) {
            const char* text = elem->GetText();
            layoutConfig.suggestedApps = text && (strcmp(text, "true") == 0);
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("expand_hotseat_size")) {
            const char* text = elem->GetText();
            layoutConfig.expandHotseatSize = text ? std::stoi(text) : 5;
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("home_grid_list")) {
            layoutConfig.homeGridList = elem->GetText() ? elem->GetText() : "";
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("apps_grid_list")) {
            layoutConfig.appsGridList = elem->GetText() ? elem->GetText() : "";
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("viewType_appOrder")) {
            layoutConfig.viewTypeAppOrder = elem->GetText() ? elem->GetText() : "";
        }
        
        // 解析网格配置
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("Rows")) {
            const char* text = elem->GetText();
            layoutConfig.rows = text ? std::stoi(text) : 5;
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("Columns")) {
            const char* text = elem->GetText();
            layoutConfig.columns = text ? std::stoi(text) : 4;
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("PageCount")) {
            const char* text = elem->GetText();
            layoutConfig.pageCount = text ? std::stoi(text) : 1;
        }
        if (const tinyxml2::XMLElement* elem = root->FirstChildElement("ScreenIndex")) {
            const char* text = elem->GetText();
            layoutConfig.screenIndex = text ? std::stoi(text) : 0;
        }
    }

    void parseHomeSection(const tinyxml2::XMLElement* root) {
        const tinyxml2::XMLElement* home = root->FirstChildElement("home");
        if (!home) return;
        
        const tinyxml2::XMLElement* item = home->FirstChildElement();
        while (item) {
            Item parsedItem = parseItem(item);
            homeItems[parsedItem.screen].push_back(parsedItem);
            item = item->NextSiblingElement();
        }
    }
    
    void parseHotseatSection(const tinyxml2::XMLElement* root) {
        const tinyxml2::XMLElement* hotseat = root->FirstChildElement("hotseat");
        if (!hotseat) return;
        
        const tinyxml2::XMLElement* item = hotseat->FirstChildElement();
        while (item) {
            Item parsedItem = parseItem(item);
            hotseatItems.push_back(parsedItem);
            item = item->NextSiblingElement();
        }
    }
    
    void parseHomeOnlySection(const tinyxml2::XMLElement* root) {
        const tinyxml2::XMLElement* homeOnly = root->FirstChildElement("homeOnly");
        if (!homeOnly) return;
        
        const tinyxml2::XMLElement* item = homeOnly->FirstChildElement();
        while (item) {
            Item parsedItem = parseItem(item);
            homeOnlyItems[parsedItem.screen].push_back(parsedItem);
            item = item->NextSiblingElement();
        }
    }
    
    void parseAppOrderSection(const tinyxml2::XMLElement* root) {
        const tinyxml2::XMLElement* appOrder = root->FirstChildElement("appOrder");
        if (!appOrder) return;
        
        const tinyxml2::XMLElement* item = appOrder->FirstChildElement();
        while (item) {
            Item parsedItem = parseItem(item);
            appOrderItems.push_back(parsedItem);
            item = item->NextSiblingElement();
        }
    }
};

// EXMLParser实现
EXMLParser::EXMLParser() : pImpl(std::make_unique<EXMLParserImpl>()) {}

EXMLParser::~EXMLParser() = default;

bool EXMLParser::loadFromFile(const std::string& filePath) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load XML file: " << filePath << std::endl;
        return false;
    }
    
    const tinyxml2::XMLElement* root = doc.RootElement();
    if (!root) {
        std::cerr << "No root element found in XML file" << std::endl;
        return false;
    }
    
    try {
        // 清空现有数据
        pImpl->homeItems.clear();
        pImpl->hotseatItems.clear();
        pImpl->homeOnlyItems.clear();
        pImpl->appOrderItems.clear();
        pImpl->hotseatHomeOnlyItems.clear();
        
        // 解析布局配置
        pImpl->parseLayoutConfig(root);
        
        // 解析各个区域
        pImpl->parseHomeSection(root);
        pImpl->parseHotseatSection(root);
        pImpl->parseHomeOnlySection(root);
        pImpl->parseAppOrderSection(root);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing XML: " << e.what() << std::endl;
        return false;
    }
}

bool EXMLParser::saveToFile(const std::string& filePath) {
    // TODO: 实现XML生成功能
    return false;
}

const LayoutConfig& EXMLParser::getLayoutConfig() const {
    return pImpl->layoutConfig;
}

void EXMLParser::setLayoutConfig(const LayoutConfig& config) {
    pImpl->layoutConfig = config;
}

// Home区域操作
size_t EXMLParser::getHomePageCount() const {
    return pImpl->homeItems.size();
}

std::vector<Item> EXMLParser::getHomeItems(int pageIndex) const {
    auto it = pImpl->homeItems.find(pageIndex);
    return it != pImpl->homeItems.end() ? it->second : std::vector<Item>();
}

bool EXMLParser::addHomeItem(int pageIndex, const Item& item) {
    pImpl->homeItems[pageIndex].push_back(item);
    return true;
}

bool EXMLParser::removeHomeItem(int pageIndex, const std::string& packageName, const std::string& className) {
    auto it = pImpl->homeItems.find(pageIndex);
    if (it == pImpl->homeItems.end()) return false;
    
    auto& items = it->second;
    auto itemIt = std::find_if(items.begin(), items.end(),
        [&](const Item& item) {
            return item.packageName == packageName && item.className == className;
        });
    
    if (itemIt != items.end()) {
        items.erase(itemIt);
        return true;
    }
    return false;
}

bool EXMLParser::moveHomeItem(int fromPage, int fromIndex, int toPage, int toIndex) {
    auto fromIt = pImpl->homeItems.find(fromPage);
    auto toIt = pImpl->homeItems.find(toPage);
    
    if (fromIt == pImpl->homeItems.end() || fromIndex >= fromIt->second.size()) return false;
    
    Item item = fromIt->second[fromIndex];
    fromIt->second.erase(fromIt->second.begin() + fromIndex);
    
    if (toIndex > toIt->second.size()) toIndex = toIt->second.size();
    toIt->second.insert(toIt->second.begin() + toIndex, item);
    
    return true;
}

std::vector<Item> EXMLParser::findHomeItems(const std::string& packageName) const {
    std::vector<Item> result;
    for (const auto& page : pImpl->homeItems) {
        for (const auto& item : page.second) {
            if (item.packageName == packageName) {
                result.push_back(item);
            }
        }
    }
    return result;
}

// Hotseat区域操作
size_t EXMLParser::getHotseatItemCount() const {
    return pImpl->hotseatItems.size();
}

std::vector<Item> EXMLParser::getHotseatItems() const {
    return pImpl->hotseatItems;
}

bool EXMLParser::addHotseatItem(const Item& item) {
    pImpl->hotseatItems.push_back(item);
    return true;
}

bool EXMLParser::removeHotseatItem(const std::string& packageName, const std::string& className) {
    auto it = std::find_if(pImpl->hotseatItems.begin(), pImpl->hotseatItems.end(),
        [&](const Item& item) {
            return item.packageName == packageName && item.className == className;
        });
    
    if (it != pImpl->hotseatItems.end()) {
        pImpl->hotseatItems.erase(it);
        return true;
    }
    return false;
}

bool EXMLParser::moveHotseatItem(int fromIndex, int toIndex) {
    if (fromIndex >= pImpl->hotseatItems.size() || toIndex > pImpl->hotseatItems.size()) return false;
    
    Item item = pImpl->hotseatItems[fromIndex];
    pImpl->hotseatItems.erase(pImpl->hotseatItems.begin() + fromIndex);
    pImpl->hotseatItems.insert(pImpl->hotseatItems.begin() + toIndex, item);
    
    return true;
}

std::vector<Item> EXMLParser::findHotseatItems(const std::string& packageName) const {
    std::vector<Item> result;
    for (const auto& item : pImpl->hotseatItems) {
        if (item.packageName == packageName) {
            result.push_back(item);
        }
    }
    return result;
}

// HomeOnly区域操作
size_t EXMLParser::getHomeOnlyPageCount() const {
    return pImpl->homeOnlyItems.size();
}

std::vector<Item> EXMLParser::getHomeOnlyItems(int pageIndex) const {
    auto it = pImpl->homeOnlyItems.find(pageIndex);
    return it != pImpl->homeOnlyItems.end() ? it->second : std::vector<Item>();
}

bool EXMLParser::addHomeOnlyItem(int pageIndex, const Item& item) {
    pImpl->homeOnlyItems[pageIndex].push_back(item);
    return true;
}

bool EXMLParser::removeHomeOnlyItem(int pageIndex, const std::string& packageName, const std::string& className) {
    auto it = pImpl->homeOnlyItems.find(pageIndex);
    if (it == pImpl->homeOnlyItems.end()) return false;
    
    auto& items = it->second;
    auto itemIt = std::find_if(items.begin(), items.end(),
        [&](const Item& item) {
            return item.packageName == packageName && item.className == className;
        });
    
    if (itemIt != items.end()) {
        items.erase(itemIt);
        return true;
    }
    return false;
}

bool EXMLParser::moveHomeOnlyItem(int fromPage, int fromIndex, int toPage, int toIndex) {
    auto fromIt = pImpl->homeOnlyItems.find(fromPage);
    auto toIt = pImpl->homeOnlyItems.find(toPage);
    
    if (fromIt == pImpl->homeOnlyItems.end() || fromIndex >= fromIt->second.size()) return false;
    
    Item item = fromIt->second[fromIndex];
    fromIt->second.erase(fromIt->second.begin() + fromIndex);
    
    if (toIndex > toIt->second.size()) toIndex = toIt->second.size();
    toIt->second.insert(toIt->second.begin() + toIndex, item);
    
    return true;
}

std::vector<Item> EXMLParser::findHomeOnlyItems(const std::string& packageName) const {
    std::vector<Item> result;
    for (const auto& page : pImpl->homeOnlyItems) {
        for (const auto& item : page.second) {
            if (item.packageName == packageName) {
                result.push_back(item);
            }
        }
    }
    return result;
}

// AppOrder区域操作
size_t EXMLParser::getAppOrderItemCount() const {
    return pImpl->appOrderItems.size();
}

std::vector<Item> EXMLParser::getAppOrderItems() const {
    return pImpl->appOrderItems;
}

bool EXMLParser::addAppOrderItem(const Item& item) {
    pImpl->appOrderItems.push_back(item);
    return true;
}

bool EXMLParser::removeAppOrderItem(const std::string& packageName, const std::string& className) {
    auto it = std::find_if(pImpl->appOrderItems.begin(), pImpl->appOrderItems.end(),
        [&](const Item& item) {
            return item.packageName == packageName && item.className == className;
        });
    
    if (it != pImpl->appOrderItems.end()) {
        pImpl->appOrderItems.erase(it);
        return true;
    }
    return false;
}

bool EXMLParser::moveAppOrderItem(int fromIndex, int toIndex) {
    if (fromIndex >= pImpl->appOrderItems.size() || toIndex > pImpl->appOrderItems.size()) return false;
    
    Item item = pImpl->appOrderItems[fromIndex];
    pImpl->appOrderItems.erase(pImpl->appOrderItems.begin() + fromIndex);
    pImpl->appOrderItems.insert(pImpl->appOrderItems.begin() + toIndex, item);
    
    return true;
}

std::vector<Item> EXMLParser::findAppOrderItems(const std::string& packageName) const {
    std::vector<Item> result;
    for (const auto& item : pImpl->appOrderItems) {
        if (item.packageName == packageName) {
            result.push_back(item);
        }
    }
    return result;
}

void EXMLParser::clear() {
    pImpl->homeItems.clear();
    pImpl->hotseatItems.clear();
    pImpl->homeOnlyItems.clear();
    pImpl->appOrderItems.clear();
    pImpl->hotseatHomeOnlyItems.clear();
    pImpl->layoutConfig = LayoutConfig();
}

} // namespace exml
