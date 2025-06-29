#include "exml_parser.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "开始测试 SamsungEXMLParser..." << std::endl;
    
    exml::EXMLParser parser;
    
    // 测试基本功能
    std::cout << "1. 测试基本数据结构..." << std::endl;
    
    // 创建测试项目
    exml::Item testItem;
    testItem.type = exml::ItemType::FAVORITE;
    testItem.packageName = "com.test.app";
    testItem.className = "com.test.app.MainActivity";
    testItem.screen = 0;
    testItem.x = 1;
    testItem.y = 2;
    
    // 测试添加项目
    std::cout << "2. 测试添加项目..." << std::endl;
    assert(parser.addHomeItem(0, testItem));
    assert(parser.getHomePageCount() == 1);
    assert(parser.getHomeItems(0).size() == 1);
    
    // 测试查找项目
    std::cout << "3. 测试查找项目..." << std::endl;
    auto foundItems = parser.findHomeItems("com.test.app");
    assert(foundItems.size() == 1);
    assert(foundItems[0].packageName == "com.test.app");
    
    // 测试删除项目
    std::cout << "4. 测试删除项目..." << std::endl;
    assert(parser.removeHomeItem(0, "com.test.app", "com.test.app.MainActivity"));
    assert(parser.getHomeItems(0).size() == 0);
    
    // 测试Hotseat操作
    std::cout << "5. 测试Hotseat操作..." << std::endl;
    assert(parser.addHotseatItem(testItem));
    assert(parser.getHotseatItemCount() == 1);
    assert(parser.removeHotseatItem("com.test.app", "com.test.app.MainActivity"));
    assert(parser.getHotseatItemCount() == 0);
    
    // 测试HomeOnly操作
    std::cout << "6. 测试HomeOnly操作..." << std::endl;
    assert(parser.addHomeOnlyItem(0, testItem));
    assert(parser.getHomeOnlyPageCount() == 1);
    assert(parser.getHomeOnlyItems(0).size() == 1);
    assert(parser.removeHomeOnlyItem(0, "com.test.app", "com.test.app.MainActivity"));
    assert(parser.getHomeOnlyItems(0).size() == 0);
    
    // 测试AppOrder操作
    std::cout << "7. 测试AppOrder操作..." << std::endl;
    assert(parser.addAppOrderItem(testItem));
    assert(parser.getAppOrderItemCount() == 1);
    assert(parser.removeAppOrderItem("com.test.app", "com.test.app.MainActivity"));
    assert(parser.getAppOrderItemCount() == 0);
    
    // 测试布局配置
    std::cout << "8. 测试布局配置..." << std::endl;
    exml::LayoutConfig config;
    config.rows = 6;
    config.columns = 5;
    config.pageCount = 3;
    parser.setLayoutConfig(config);
    
    const auto& currentConfig = parser.getLayoutConfig();
    assert(currentConfig.rows == 6);
    assert(currentConfig.columns == 5);
    assert(currentConfig.pageCount == 3);
    
    // 测试清空功能
    std::cout << "9. 测试清空功能..." << std::endl;
    parser.addHomeItem(0, testItem);
    parser.addHotseatItem(testItem);
    parser.addHomeOnlyItem(0, testItem);
    parser.addAppOrderItem(testItem);
    
    parser.clear();
    
    assert(parser.getHomePageCount() == 0);
    assert(parser.getHotseatItemCount() == 0);
    assert(parser.getHomeOnlyPageCount() == 0);
    assert(parser.getAppOrderItemCount() == 0);
    
    std::cout << "所有基本功能测试通过！" << std::endl;
    
    // 测试文件操作（如果sample文件存在）
    std::cout << "10. 测试文件操作..." << std::endl;
    if (parser.loadFromFile("samples/template.exml")) {
        std::cout << "成功加载template.exml文件" << std::endl;
        
        // 打印一些基本信息
        std::cout << "Home页面数量: " << parser.getHomePageCount() << std::endl;
        std::cout << "Hotseat项目数量: " << parser.getHotseatItemCount() << std::endl;
        std::cout << "HomeOnly页面数量: " << parser.getHomeOnlyPageCount() << std::endl;
        std::cout << "AppOrder项目数量: " << parser.getAppOrderItemCount() << std::endl;
        
        // 测试保存功能
        if (parser.saveToFile("samples/output.exml")) {
            std::cout << "成功保存到output.exml文件" << std::endl;
        } else {
            std::cout << "保存文件失败" << std::endl;
        }
    } else {
        std::cout << "无法加载template.exml文件，跳过文件测试" << std::endl;
    }
    
    std::cout << "测试完成！" << std::endl;
    return 0;
}
