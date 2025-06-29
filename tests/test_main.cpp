#include "exml_parser.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Starting SamsungEXMLParser tests..." << std::endl;
    
    exml::EXMLParser parser;
    
    // Test basic functionality
    std::cout << "1. Testing basic data structures..." << std::endl;
    
    // Create test item
    exml::Item testItem;
    testItem.type = exml::ItemType::FAVORITE;
    testItem.packageName = "com.test.app";
    testItem.className = "com.test.app.MainActivity";
    testItem.screen = 0;
    testItem.x = 1;
    testItem.y = 2;
    
    // Test adding items
    std::cout << "2. Testing add item..." << std::endl;
    assert(parser.addHomeItem(0, testItem));
    assert(parser.getHomePageCount() == 1);
    assert(parser.getHomeItems(0).size() == 1);
    
    // Test finding items
    std::cout << "3. Testing find item..." << std::endl;
    auto foundItems = parser.findHomeItems("com.test.app");
    assert(foundItems.size() == 1);
    assert(foundItems[0].packageName == "com.test.app");
    
    // Test removing items
    std::cout << "4. Testing remove item..." << std::endl;
    assert(parser.removeHomeItem(0, "com.test.app", "com.test.app.MainActivity"));
    assert(parser.getHomeItems(0).size() == 0);
    
    // Test Hotseat operations
    std::cout << "5. Testing Hotseat operations..." << std::endl;
    assert(parser.addHotseatItem(testItem));
    assert(parser.getHotseatItemCount() == 1);
    assert(parser.removeHotseatItem("com.test.app", "com.test.app.MainActivity"));
    assert(parser.getHotseatItemCount() == 0);
    
    // Test HomeOnly operations
    std::cout << "6. Testing HomeOnly operations..." << std::endl;
    assert(parser.addHomeOnlyItem(0, testItem));
    assert(parser.getHomeOnlyPageCount() == 1);
    assert(parser.getHomeOnlyItems(0).size() == 1);
    assert(parser.removeHomeOnlyItem(0, "com.test.app", "com.test.app.MainActivity"));
    assert(parser.getHomeOnlyItems(0).size() == 0);
    
    // Test AppOrder operations
    std::cout << "7. Testing AppOrder operations..." << std::endl;
    assert(parser.addAppOrderItem(testItem));
    assert(parser.getAppOrderItemCount() == 1);
    assert(parser.removeAppOrderItem("com.test.app", "com.test.app.MainActivity"));
    assert(parser.getAppOrderItemCount() == 0);
    
    // Test layout configuration
    std::cout << "8. Testing layout configuration..." << std::endl;
    exml::LayoutConfig config;
    config.rows = 6;
    config.columns = 5;
    config.pageCount = 3;
    parser.setLayoutConfig(config);
    
    const auto& currentConfig = parser.getLayoutConfig();
    assert(currentConfig.rows == 6);
    assert(currentConfig.columns == 5);
    assert(currentConfig.pageCount == 3);
    
    // Test clear functionality
    std::cout << "9. Testing clear functionality..." << std::endl;
    parser.addHomeItem(0, testItem);
    parser.addHotseatItem(testItem);
    parser.addHomeOnlyItem(0, testItem);
    parser.addAppOrderItem(testItem);
    
    parser.clear();
    
    assert(parser.getHomePageCount() == 0);
    assert(parser.getHotseatItemCount() == 0);
    assert(parser.getHomeOnlyPageCount() == 0);
    assert(parser.getAppOrderItemCount() == 0);
    
    std::cout << "All basic functionality tests passed!" << std::endl;
    
    // Test file operations (if sample file exists)
    std::cout << "10. Testing file operations..." << std::endl;
    if (parser.loadFromFile("samples/template.exml")) {
        std::cout << "Successfully loaded template.exml file" << std::endl;
        
        // Print some basic information
        std::cout << "Home page count: " << parser.getHomePageCount() << std::endl;
        std::cout << "Hotseat item count: " << parser.getHotseatItemCount() << std::endl;
        std::cout << "HomeOnly page count: " << parser.getHomeOnlyPageCount() << std::endl;
        std::cout << "AppOrder item count: " << parser.getAppOrderItemCount() << std::endl;
        
        // Test save functionality
        if (parser.saveToFile("samples/output.exml")) {
            std::cout << "Successfully saved to output.exml file" << std::endl;
        } else {
            std::cout << "Failed to save file" << std::endl;
        }
    } else {
        std::cout << "Cannot load template.exml file, skipping file tests" << std::endl;
    }
    
    std::cout << "Tests completed!" << std::endl;
    return 0;
}
