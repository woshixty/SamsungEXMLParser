#include "exml_parser.h"
#include <iostream>
#include <iomanip>

void printItem(const exml::Item& item, const std::string& prefix = "") {
    std::cout << prefix << "Type: ";
    switch (item.type) {
        case exml::ItemType::FAVORITE:
            std::cout << "Application Icon";
            break;
        case exml::ItemType::FOLDER:
            std::cout << "Folder";
            break;
        case exml::ItemType::APPWIDGET:
            std::cout << "Widget";
            break;
    }
    std::cout << std::endl;
    
    if (!item.packageName.empty()) {
        std::cout << prefix << "Package: " << item.packageName << std::endl;
    }
    if (!item.className.empty()) {
        std::cout << prefix << "Class: " << item.className << std::endl;
    }
    if (!item.title.empty()) {
        std::cout << prefix << "Title: " << item.title << std::endl;
    }
    std::cout << prefix << "Screen: " << item.screen << std::endl;
    std::cout << prefix << "Position: (" << item.x << ", " << item.y << ")" << std::endl;
    if (item.spanX > 0 || item.spanY > 0) {
        std::cout << prefix << "Span: " << item.spanX << "x" << item.spanY << std::endl;
    }
    if (item.appWidgetID > 0) {
        std::cout << prefix << "Widget ID: " << item.appWidgetID << std::endl;
    }
    if (!item.favorites.empty()) {
        std::cout << prefix << "Folder contents (" << item.favorites.size() << " items):" << std::endl;
        for (const auto& fav : item.favorites) {
            printItem(fav, prefix + "  ");
        }
    }
    std::cout << std::endl;
}

void printLayoutConfig(const exml::LayoutConfig& config) {
    std::cout << "=== Layout Configuration ===" << std::endl;
    std::cout << "Rows: " << config.rows << std::endl;
    std::cout << "Columns: " << config.columns << std::endl;
    std::cout << "Page Count: " << config.pageCount << std::endl;
    std::cout << "Screen Index: " << config.screenIndex << std::endl;
    std::cout << "Category: " << config.category << std::endl;
    std::cout << "Folder Grid: " << config.folderGrid << std::endl;
    std::cout << "Restore Max Size Grid: " << (config.restoreMaxSizeGrid ? "Yes" : "No") << std::endl;
    std::cout << "Zero Page Contents: " << config.zeroPageContents << std::endl;
    std::cout << "Zero Page: " << (config.zeroPage ? "Yes" : "No") << std::endl;
    std::cout << "Notification Panel Setting: " << (config.notificationPanelSetting ? "Yes" : "No") << std::endl;
    std::cout << "Lock Layout Setting: " << (config.lockLayoutSetting ? "Yes" : "No") << std::endl;
    std::cout << "Quick Access Finder: " << (config.quickAccessFinder ? "Yes" : "No") << std::endl;
    std::cout << "Badge On/Off Setting: " << config.badgeOnOffSetting << std::endl;
    std::cout << "Only Portrait Mode: " << (config.onlyPortraitModeSetting ? "Yes" : "No") << std::endl;
    std::cout << "Add Icon to Home: " << (config.addIconToHomeSetting ? "Yes" : "No") << std::endl;
    std::cout << "Suggested Apps: " << (config.suggestedApps ? "Yes" : "No") << std::endl;
    std::cout << "Expand Hotseat Size: " << config.expandHotseatSize << std::endl;
    std::cout << "Home Grid List: " << config.homeGridList << std::endl;
    std::cout << "Apps Grid List: " << config.appsGridList << std::endl;
    std::cout << "App Order View Type: " << config.viewTypeAppOrder << std::endl;
    std::cout << std::endl;
}

void testHomeArea(const exml::EXMLParser& parser) {
    std::cout << "=== Home Area Test ===" << std::endl;
    size_t pageCount = parser.getHomePageCount();
    std::cout << "Home page count: " << pageCount << std::endl;
    
    for (size_t i = 0; i < pageCount; ++i) {
        std::cout << "\n--- Page " << i << " ---" << std::endl;
        auto items = parser.getHomeItems(i);
        std::cout << "Item count: " << items.size() << std::endl;
        
        for (size_t j = 0; j < items.size(); ++j) {
            std::cout << "\nItem " << j << ":" << std::endl;
            printItem(items[j], "  ");
        }
    }
    std::cout << std::endl;
}

void testHotseatArea(const exml::EXMLParser& parser) {
    std::cout << "=== Hotseat Area Test ===" << std::endl;
    size_t itemCount = parser.getHotseatItemCount();
    std::cout << "Hotseat item count: " << itemCount << std::endl;
    
    auto items = parser.getHotseatItems();
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << "\nItem " << i << ":" << std::endl;
        printItem(items[i], "  ");
    }
    std::cout << std::endl;
}

void testHomeOnlyArea(const exml::EXMLParser& parser) {
    std::cout << "=== HomeOnly Area Test ===" << std::endl;
    size_t pageCount = parser.getHomeOnlyPageCount();
    std::cout << "HomeOnly page count: " << pageCount << std::endl;
    
    for (size_t i = 0; i < pageCount; ++i) {
        std::cout << "\n--- Page " << i << " ---" << std::endl;
        auto items = parser.getHomeOnlyItems(i);
        std::cout << "Item count: " << items.size() << std::endl;
        
        for (size_t j = 0; j < items.size(); ++j) {
            std::cout << "\nItem " << j << ":" << std::endl;
            printItem(items[j], "  ");
        }
    }
    std::cout << std::endl;
}

void testAppOrderArea(const exml::EXMLParser& parser) {
    std::cout << "=== AppOrder Area Test ===" << std::endl;
    size_t itemCount = parser.getAppOrderItemCount();
    std::cout << "AppOrder item count: " << itemCount << std::endl;
    
    auto items = parser.getAppOrderItems();
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << "\nItem " << i << ":" << std::endl;
        printItem(items[i], "  ");
    }
    std::cout << std::endl;
}

void testSearchFunctionality(const exml::EXMLParser& parser) {
    std::cout << "=== Search Functionality Test ===" << std::endl;
    
    // Test searching for specific app
    std::string testPackage = "com.samsung.android.dialer";
    std::cout << "Searching for package: " << testPackage << std::endl;
    
    auto homeItems = parser.findHomeItems(testPackage);
    std::cout << "Found " << homeItems.size() << " items in Home area" << std::endl;
    
    auto hotseatItems = parser.findHotseatItems(testPackage);
    std::cout << "Found " << hotseatItems.size() << " items in Hotseat area" << std::endl;
    
    auto homeOnlyItems = parser.findHomeOnlyItems(testPackage);
    std::cout << "Found " << homeOnlyItems.size() << " items in HomeOnly area" << std::endl;
    
    auto appOrderItems = parser.findAppOrderItems(testPackage);
    std::cout << "Found " << appOrderItems.size() << " items in AppOrder area" << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Samsung EXML Parser Test Program ===" << std::endl;
    std::cout << "Focusing on testing exml file parsing functionality" << std::endl;
    std::cout << std::endl;
    
    exml::EXMLParser parser;
    
    // Test file loading
    std::cout << "Loading template.exml file..." << std::endl;
    if (!parser.loadFromFile("samples/template.exml")) {
        std::cout << "Error: Cannot load template.exml file!" << std::endl;
        std::cout << "Please ensure samples/template.exml file exists and is readable." << std::endl;
        return 1;
    }
    
    std::cout << "Successfully loaded template.exml file!" << std::endl;
    std::cout << std::endl;
    
    // Test layout configuration parsing
    printLayoutConfig(parser.getLayoutConfig());
    
    // Test parsing of each area
    testHomeArea(parser);
    testHotseatArea(parser);
    testHomeOnlyArea(parser);
    testAppOrderArea(parser);
    
    // Test search functionality
    testSearchFunctionality(parser);
    
    // Test save functionality
    std::cout << "=== Save Functionality Test ===" << std::endl;
    std::cout << "Saving to output.exml..." << std::endl;
    if (parser.saveToFile("samples/output.exml")) {
        std::cout << "Successfully saved to samples/output.exml" << std::endl;
    } else {
        std::cout << "Save failed!" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Test Complete ===" << std::endl;
    std::cout << "All parsing tests completed. Please check the above output to verify parsing results." << std::endl;
    
    return 0;
} 