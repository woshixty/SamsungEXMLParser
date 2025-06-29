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
        int value = defaultValue;
        element->QueryIntAttribute(attrName, &value);
        return value;
    }

    static bool getAttributeBoolValue(const tinyxml2::XMLElement* element, const char* attrName, bool defaultValue = false) {
        const char* value = element->Attribute(attrName);
        if (!value) return defaultValue;
        return std::string(value) == "true";
    }

    Item parseItem(const tinyxml2::XMLElement* element) {
        Item item;
        std::string tagName = element->Name();
        
        if (tagName == "favorite") {
            item.type = ItemType::FAVORITE;
        } else if (tagName == "folder") {
            item.type = ItemType::FOLDER;
            item.title = getAttributeValue(element, "title");
        } else if (tagName == "appwidget") {
            item.type = ItemType::APPWIDGET;
        }

        item.packageName = getAttributeValue(element, "packageName");
        item.className = getAttributeValue(element, "className");
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
            const tinyxml2::XMLElement* child = element->FirstChildElement("favorite");
            while (child) {
                Item favorite = parseItem(child);
                item.favorites.push_back(favorite);
                child = child->NextSiblingElement("favorite");
            }
        }

        return item;
    }

    void parseLayoutConfig(const tinyxml2::XMLDocument& doc) {
        const tinyxml2::XMLElement* root = doc.RootElement();
        if (!root) return;

        // 解析基本配置
        const tinyxml2::XMLElement* category = root->FirstChildElement("category");
        if (category) layoutConfig.category = category->GetText() ? category->GetText() : "";

        const tinyxml2::XMLElement* folderGrid = root->FirstChildElement("FolderGrid");
        if (folderGrid) layoutConfig.folderGrid = folderGrid->GetText() ? folderGrid->GetText() : "";

        const tinyxml2::XMLElement* restoreMaxSizeGrid = root->FirstChildElement("restore_max_size_grid");
        if (restoreMaxSizeGrid) layoutConfig.restoreMaxSizeGrid = getAttributeBoolValue(restoreMaxSizeGrid, "value", true);

        const tinyxml2::XMLElement* zeroPageContents = root->FirstChildElement("zeroPageContents");
        if (zeroPageContents) layoutConfig.zeroPageContents = zeroPageContents->GetText() ? zeroPageContents->GetText() : "";

        const tinyxml2::XMLElement* selectedMinusonePackage = root->FirstChildElement("selectedMinusonePackage");
        if (selectedMinusonePackage) layoutConfig.selectedMinusonePackage = selectedMinusonePackage->GetText() ? selectedMinusonePackage->GetText() : "";

        const tinyxml2::XMLElement* zeroPage = root->FirstChildElement("zeroPage");
        if (zeroPage) layoutConfig.zeroPage = getAttributeBoolValue(zeroPage, "value", false);

        const tinyxml2::XMLElement* notificationPanelSetting = root->FirstChildElement("notification_panel_setting");
        if (notificationPanelSetting) layoutConfig.notificationPanelSetting = getAttributeBoolValue(notificationPanelSetting, "value", true);

        const tinyxml2::XMLElement* lockLayoutSetting = root->FirstChildElement("lock_layout_setting");
        if (lockLayoutSetting) layoutConfig.lockLayoutSetting = getAttributeBoolValue(lockLayoutSetting, "value", false);

        const tinyxml2::XMLElement* quickAccessFinder = root->FirstChildElement("quick_access_finder");
        if (quickAccessFinder) layoutConfig.quickAccessFinder = getAttributeBoolValue(quickAccessFinder, "value", true);

        const tinyxml2::XMLElement* badgeOnOffSetting = root->FirstChildElement("badge_on_off_setting");
        if (badgeOnOffSetting) layoutConfig.badgeOnOffSetting = getAttributeIntValue(badgeOnOffSetting, "value", 0);

        const tinyxml2::XMLElement* onlyPortraitModeSetting = root->FirstChildElement("only_portrait_mode_setting");
        if (onlyPortraitModeSetting) layoutConfig.onlyPortraitModeSetting = getAttributeBoolValue(onlyPortraitModeSetting, "value", true);

        const tinyxml2::XMLElement* addIconToHomeSetting = root->FirstChildElement("add_icon_to_home_setting");
        if (addIconToHomeSetting) layoutConfig.addIconToHomeSetting = getAttributeBoolValue(addIconToHomeSetting, "value", false);

        const tinyxml2::XMLElement* suggestedApps = root->FirstChildElement("suggested_apps");
        if (suggestedApps) layoutConfig.suggestedApps = getAttributeBoolValue(suggestedApps, "value", true);

        const tinyxml2::XMLElement* expandHotseatSize = root->FirstChildElement("expand_hotseat_size");
        if (expandHotseatSize) layoutConfig.expandHotseatSize = getAttributeIntValue(expandHotseatSize, "value", 5);

        const tinyxml2::XMLElement* homeGridList = root->FirstChildElement("home_grid_list");
        if (homeGridList) layoutConfig.homeGridList = homeGridList->GetText() ? homeGridList->GetText() : "";

        const tinyxml2::XMLElement* appsGridList = root->FirstChildElement("apps_grid_list");
        if (appsGridList) layoutConfig.appsGridList = appsGridList->GetText() ? appsGridList->GetText() : "";

        const tinyxml2::XMLElement* viewTypeAppOrder = root->FirstChildElement("viewType_appOrder");
        if (viewTypeAppOrder) layoutConfig.viewTypeAppOrder = viewTypeAppOrder->GetText() ? viewTypeAppOrder->GetText() : "";

        // 解析网格配置
        const tinyxml2::XMLElement* rows = root->FirstChildElement("Rows");
        if (rows) layoutConfig.rows = getAttributeIntValue(rows, "value", 5);

        const tinyxml2::XMLElement* columns = root->FirstChildElement("Columns");
        if (columns) layoutConfig.columns = getAttributeIntValue(columns, "value", 4);

        const tinyxml2::XMLElement* pageCount = root->FirstChildElement("PageCount");
        if (pageCount) layoutConfig.pageCount = getAttributeIntValue(pageCount, "value", 1);

        const tinyxml2::XMLElement* screenIndex = root->FirstChildElement("ScreenIndex");
        if (screenIndex) layoutConfig.screenIndex = getAttributeIntValue(screenIndex, "value", 0);
    }

    void parseItems(const tinyxml2::XMLDocument& doc) {
        const tinyxml2::XMLElement* root = doc.RootElement();
        if (!root) return;

        // 解析home区域
        const tinyxml2::XMLElement* home = root->FirstChildElement("home");
        if (home) {
            const tinyxml2::XMLElement* child = home->FirstChildElement();
            while (child) {
                Item item = parseItem(child);
                homeItems[item.screen].push_back(item);
                child = child->NextSiblingElement();
            }
        }

        // 解析hotseat区域
        const tinyxml2::XMLElement* hotseat = root->FirstChildElement("hotseat");
        if (hotseat) {
            const tinyxml2::XMLElement* child = hotseat->FirstChildElement();
            while (child) {
                Item item = parseItem(child);
                hotseatItems.push_back(item);
                child = child->NextSiblingElement();
            }
        }

        // 解析homeOnly区域
        const tinyxml2::XMLElement* homeOnly = root->FirstChildElement("homeOnly");
        if (homeOnly) {
            const tinyxml2::XMLElement* child = homeOnly->FirstChildElement();
            while (child) {
                Item item = parseItem(child);
                homeOnlyItems[item.screen].push_back(item);
                child = child->NextSiblingElement();
            }
        }

        // 解析hotseat_homeOnly区域
        const tinyxml2::XMLElement* hotseatHomeOnly = root->FirstChildElement("hotseat_homeOnly");
        if (hotseatHomeOnly) {
            const tinyxml2::XMLElement* child = hotseatHomeOnly->FirstChildElement();
            while (child) {
                Item item = parseItem(child);
                hotseatHomeOnlyItems.push_back(item);
                child = child->NextSiblingElement();
            }
        }

        // 解析appOrder区域
        const tinyxml2::XMLElement* appOrder = root->FirstChildElement("appOrder");
        if (appOrder) {
            const tinyxml2::XMLElement* child = appOrder->FirstChildElement();
            while (child) {
                Item item = parseItem(child);
                appOrderItems.push_back(item);
                child = child->NextSiblingElement();
            }
        }
    }

    tinyxml2::XMLElement* createItemElement(tinyxml2::XMLDocument& doc, const Item& item) {
        tinyxml2::XMLElement* element = nullptr;
        
        switch (item.type) {
            case ItemType::FAVORITE:
                element = doc.NewElement("favorite");
                break;
            case ItemType::FOLDER:
                element = doc.NewElement("folder");
                if (!item.title.empty()) {
                    element->SetAttribute("title", item.title.c_str());
                }
                break;
            case ItemType::APPWIDGET:
                element = doc.NewElement("appwidget");
                break;
        }

        if (!item.packageName.empty()) {
            element->SetAttribute("packageName", item.packageName.c_str());
        }
        if (!item.className.empty()) {
            element->SetAttribute("className", item.className.c_str());
        }
        if (item.screen >= 0) {
            element->SetAttribute("screen", item.screen);
        }
        if (item.x >= 0) {
            element->SetAttribute("x", item.x);
        }
        if (item.y >= 0) {
            element->SetAttribute("y", item.y);
        }
        if (item.spanX > 1) {
            element->SetAttribute("spanX", item.spanX);
        }
        if (item.spanY > 1) {
            element->SetAttribute("spanY", item.spanY);
        }
        if (item.appWidgetID > 0) {
            element->SetAttribute("appWidgetID", item.appWidgetID);
        }
        if (item.options != 0) {
            element->SetAttribute("options", item.options);
        }
        if (item.color != -1) {
            element->SetAttribute("color", item.color);
        }
        if (item.hidden) {
            element->SetAttribute("hidden", "1");
        }

        // 添加文件夹内的favorites
        if (item.type == ItemType::FOLDER) {
            for (const auto& favorite : item.favorites) {
                tinyxml2::XMLElement* favElement = createItemElement(doc, favorite);
                element->InsertEndChild(favElement);
            }
        }

        return element;
    }

    void createLayoutConfigElements(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root) {
        if (!layoutConfig.category.empty()) {
            tinyxml2::XMLElement* category = doc.NewElement("category");
            category->SetText(layoutConfig.category.c_str());
            root->InsertEndChild(category);
        }

        if (!layoutConfig.folderGrid.empty()) {
            tinyxml2::XMLElement* folderGrid = doc.NewElement("FolderGrid");
            folderGrid->SetText(layoutConfig.folderGrid.c_str());
            root->InsertEndChild(folderGrid);
        }

        tinyxml2::XMLElement* restoreMaxSizeGrid = doc.NewElement("restore_max_size_grid");
        restoreMaxSizeGrid->SetText(layoutConfig.restoreMaxSizeGrid ? "true" : "false");
        root->InsertEndChild(restoreMaxSizeGrid);

        if (!layoutConfig.zeroPageContents.empty()) {
            tinyxml2::XMLElement* zeroPageContents = doc.NewElement("zeroPageContents");
            zeroPageContents->SetText(layoutConfig.zeroPageContents.c_str());
            root->InsertEndChild(zeroPageContents);
        }

        if (!layoutConfig.selectedMinusonePackage.empty()) {
            tinyxml2::XMLElement* selectedMinusonePackage = doc.NewElement("selectedMinusonePackage");
            selectedMinusonePackage->SetText(layoutConfig.selectedMinusonePackage.c_str());
            root->InsertEndChild(selectedMinusonePackage);
        }

        tinyxml2::XMLElement* zeroPage = doc.NewElement("zeroPage");
        zeroPage->SetText(layoutConfig.zeroPage ? "true" : "false");
        root->InsertEndChild(zeroPage);

        tinyxml2::XMLElement* notificationPanelSetting = doc.NewElement("notification_panel_setting");
        notificationPanelSetting->SetText(layoutConfig.notificationPanelSetting ? "true" : "false");
        root->InsertEndChild(notificationPanelSetting);

        tinyxml2::XMLElement* lockLayoutSetting = doc.NewElement("lock_layout_setting");
        lockLayoutSetting->SetText(layoutConfig.lockLayoutSetting ? "true" : "false");
        root->InsertEndChild(lockLayoutSetting);

        tinyxml2::XMLElement* quickAccessFinder = doc.NewElement("quick_access_finder");
        quickAccessFinder->SetText(layoutConfig.quickAccessFinder ? "true" : "false");
        root->InsertEndChild(quickAccessFinder);

        tinyxml2::XMLElement* badgeOnOffSetting = doc.NewElement("badge_on_off_setting");
        badgeOnOffSetting->SetText(layoutConfig.badgeOnOffSetting);
        root->InsertEndChild(badgeOnOffSetting);

        tinyxml2::XMLElement* onlyPortraitModeSetting = doc.NewElement("only_portrait_mode_setting");
        onlyPortraitModeSetting->SetText(layoutConfig.onlyPortraitModeSetting ? "true" : "false");
        root->InsertEndChild(onlyPortraitModeSetting);

        tinyxml2::XMLElement* addIconToHomeSetting = doc.NewElement("add_icon_to_home_setting");
        addIconToHomeSetting->SetText(layoutConfig.addIconToHomeSetting ? "true" : "false");
        root->InsertEndChild(addIconToHomeSetting);

        tinyxml2::XMLElement* suggestedApps = doc.NewElement("suggested_apps");
        suggestedApps->SetText(layoutConfig.suggestedApps ? "true" : "false");
        root->InsertEndChild(suggestedApps);

        tinyxml2::XMLElement* expandHotseatSize = doc.NewElement("expand_hotseat_size");
        expandHotseatSize->SetText(layoutConfig.expandHotseatSize);
        root->InsertEndChild(expandHotseatSize);

        if (!layoutConfig.homeGridList.empty()) {
            tinyxml2::XMLElement* homeGridList = doc.NewElement("home_grid_list");
            homeGridList->SetText(layoutConfig.homeGridList.c_str());
            root->InsertEndChild(homeGridList);
        }

        if (!layoutConfig.appsGridList.empty()) {
            tinyxml2::XMLElement* appsGridList = doc.NewElement("apps_grid_list");
            appsGridList->SetText(layoutConfig.appsGridList.c_str());
            root->InsertEndChild(appsGridList);
        }

        if (!layoutConfig.viewTypeAppOrder.empty()) {
            tinyxml2::XMLElement* viewTypeAppOrder = doc.NewElement("viewType_appOrder");
            viewTypeAppOrder->SetText(layoutConfig.viewTypeAppOrder.c_str());
            root->InsertEndChild(viewTypeAppOrder);
        }

        tinyxml2::XMLElement* rows = doc.NewElement("Rows");
        rows->SetText(layoutConfig.rows);
        root->InsertEndChild(rows);

        tinyxml2::XMLElement* columns = doc.NewElement("Columns");
        columns->SetText(layoutConfig.columns);
        root->InsertEndChild(columns);

        tinyxml2::XMLElement* pageCount = doc.NewElement("PageCount");
        pageCount->SetText(layoutConfig.pageCount);
        root->InsertEndChild(pageCount);

        tinyxml2::XMLElement* screenIndex = doc.NewElement("ScreenIndex");
        screenIndex->SetText(layoutConfig.screenIndex);
        root->InsertEndChild(screenIndex);
    }
};

// EXMLParser实现
EXMLParser::EXMLParser() : pImpl(std::make_unique<EXMLParserImpl>()) {}

EXMLParser::~EXMLParser() = default;

bool EXMLParser::loadFromFile(const std::string& filePath) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.LoadFile(filePath.c_str());
    
    if (error != tinyxml2::XML_SUCCESS) {
        return false;
    }

    try {
        pImpl->parseLayoutConfig(doc);
        pImpl->parseItems(doc);
        return true;
    } catch (...) {
        return false;
    }
}

bool EXMLParser::saveToFile(const std::string& filePath) {
    tinyxml2::XMLDocument doc;
    
    // 创建XML声明
    doc.NewDeclaration();
    
    // 创建根元素
    tinyxml2::XMLElement* root = doc.NewElement("LauncherBackup");
    doc.InsertEndChild(root);
    
    // 添加布局配置
    pImpl->createLayoutConfigElements(doc, root);
    
    // 添加home区域
    if (!pImpl->homeItems.empty()) {
        tinyxml2::XMLElement* home = doc.NewElement("home");
        for (const auto& page : pImpl->homeItems) {
            for (const auto& item : page.second) {
                tinyxml2::XMLElement* element = pImpl->createItemElement(doc, item);
                home->InsertEndChild(element);
            }
        }
        root->InsertEndChild(home);
    }
    
    // 添加hotseat区域
    if (!pImpl->hotseatItems.empty()) {
        tinyxml2::XMLElement* hotseat = doc.NewElement("hotseat");
        for (const auto& item : pImpl->hotseatItems) {
            tinyxml2::XMLElement* element = pImpl->createItemElement(doc, item);
            hotseat->InsertEndChild(element);
        }
        root->InsertEndChild(hotseat);
    }
    
    // 添加homeOnly区域
    if (!pImpl->homeOnlyItems.empty()) {
        tinyxml2::XMLElement* homeOnly = doc.NewElement("homeOnly");
        for (const auto& page : pImpl->homeOnlyItems) {
            for (const auto& item : page.second) {
                tinyxml2::XMLElement* element = pImpl->createItemElement(doc, item);
                homeOnly->InsertEndChild(element);
            }
        }
        root->InsertEndChild(homeOnly);
    }
    
    // 添加hotseat_homeOnly区域
    if (!pImpl->hotseatHomeOnlyItems.empty()) {
        tinyxml2::XMLElement* hotseatHomeOnly = doc.NewElement("hotseat_homeOnly");
        for (const auto& item : pImpl->hotseatHomeOnlyItems) {
            tinyxml2::XMLElement* element = pImpl->createItemElement(doc, item);
            hotseatHomeOnly->InsertEndChild(element);
        }
        root->InsertEndChild(hotseatHomeOnly);
    }
    
    // 添加appOrder区域
    if (!pImpl->appOrderItems.empty()) {
        tinyxml2::XMLElement* appOrder = doc.NewElement("appOrder");
        for (const auto& item : pImpl->appOrderItems) {
            tinyxml2::XMLElement* element = pImpl->createItemElement(doc, item);
            appOrder->InsertEndChild(element);
        }
        root->InsertEndChild(appOrder);
    }
    
    return doc.SaveFile(filePath.c_str()) == tinyxml2::XML_SUCCESS;
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
