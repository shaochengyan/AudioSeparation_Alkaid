文件夹说明
==========

```
.
├── Alkaid
├── AlkaidProject
├── readme.html
├── readme.md
└── 示例音频
```

[Alkaid](./Alkaid)文件夹
------------------------

存储最终的可执行程序，详情请阅读[Alkaid::readme](./Alkaid/readme.md)

[AlkaidProject](./AlkaidProject)
--------------------------------

> 注：使用该项目前请使用`nuget`安装[Microsoft.ML.OnnxRuntim 1.11.0](https://www.nuget.org/packages/Microsoft.ML.OnnxRuntime/1.11.0?_src=template)

存储整个项目的文件夹

开发工具为[VS2019社区版](https://visualstudio.microsoft.com/zh-hans/vs/older-downloads/)

- [src](./AlkaidProject/src)存储所有的源文件

  - ```
    .
    ├── AlkaidConvTasNet.cpp
    ├── AlkaidConvTasNet.h
    ├── AudioUtils.cpp
    ├── AudioUtils.h
    ├── ColaUtils.cpp
    ├── ColaUtils.h
    └── main.cpp
    ```

- [packages](./AlkaidProject/packages)存储`ONNXRuntime`库

  - 使用该项目是请首先使用`NuGet`安装[Microsoft.ML.OnnxRuntim 1.11.0](https://www.nuget.org/packages/Microsoft.ML.OnnxRuntime/1.11.0?_src=template)

[示例音频文件夹](./示例音频)
----------------------------

存储60个示例音频



