# 📌 SamsungEXMLParser 任务清单

[template.exml](../samples/template.exml)是三星手机的桌面布局备份文件（使用homeup程序进行备份获得的）
1、我想写一个c++代码库（使用cmake进行项目管理），对这种备份格式文件进行读写解析，合理编写这个动态库？
2、目前我只能提供这么一个 .exml 文件
3、我不清楚需要暴露那些接口，我正在开发一款APP，实现iOS2Samsung，Samsung2iOS的桌面布局文件转换，目前正在处理Samsung手机桌面布局备份文件的读写（仅处理.exml文件的读写），你可以帮我想想，我应该会希望暴露哪些接口
4、目前没有图形界面需求
5、没有计划支持 Android 端
6、在项目里面集成 tinyxml2 库用于解析使用

## ⬇️ 读取
- [ ] 使用 tinyxml2 加载并解析 .exml 文件
- [ ] 基于 template.exml 设计出合理的数据结构
- [ ] 解析 LauncherBackup → HomeScreen → Page → Item 层级
- [ ] 将每个 Item 转换为 Item 结构体并保存到内部容器

## 🧱 操作 API
- [ ] 提供按页面访问 Items 的接口
- [ ] 提供查找（packageName）、添加、删除、移动等操作
- [ ] 提供获取页面总数接口
- [ ] homeOnly 、 hotseat 、 home 、 appOrder 的查找、添加、删除、移动等操作

## ⬆️ 写回
- [ ] 将内部 Item 结构重新转换为 XML DOM
- [ ] 写入到目标 .exml 文件路径

## 🧪 测试
- [ ] 读取 sample 文件并打印内容
- [ ] 添加/删除图标后写出 .exml 并校验结构正确

## 📦 项目结构
- [ ] 提供 CMakeLists.txt 用于构建动态库
- [ ] 提供 README 说明使用方式
