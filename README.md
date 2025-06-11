# tetrisWithSFML

这个项目是bupt大一下学期计算导论与程序设计的俄罗斯方块图形化实现。图形化界面依赖于SFML库，并使用CMake来构建项目。

- CMake: [CMake - Upgrade Your Software Build System](https://cmake.org/)

- SFML: [Simple and Fast Multimedia Library](https://www.sfml-dev.org)

## 项目进度

### 主要内容

- [X] 游戏主体
   - [X] 基础图形化 
   - [X] 游戏基本逻辑
- [X] 开始/结束界面
   - [X] 结束界面
   - [X] 开始界面
- [X] 模块结构分文件清晰化

### 更多的元素……(不一定完成)

- [ ] 最高分数记录
- [X] 下一个方块的提示
- [ ] 音效
- [ ] 更好的图形化

## 游戏操作

按键|操作|
----|----|
左方向键|左移|
右方向键|右移|
上方向键|旋转|
下方向键|下落|
空格键|瞬间下落|
R键|重新开始|

## 编译运行

在目录下运行以下cmake建构指令

```
cmake -B build
cmake --build build
```

/build/bin/下运行可执行文件tetrisWithSFML即可开始游戏

## 项目展示

开始界面

![1](https://i.mij.rip/2025/06/11/7a8cfa520c8af132a187fb679d8d6a4d.png "开始界面")

游戏界面

![2](https://i.miji.bid/2025/06/11/74387bc5426f1f5718135dc592bb2206.png "游戏界面")

结束界面

![3](https://i.mij.rip/2025/06/11/7ae8c38313249e595467d17b182b9985.png "结束界面")
