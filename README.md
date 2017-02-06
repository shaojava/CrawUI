# CrawUI
Base On Duilib. Modified Its Rendering Part into flexible OS choices with D2D/GDI+/SKIA

1:抽象渲染接口 新增 D2D/GDI+/SKIA 三种渲染方式
2:支持异形
3:仿写微软动画板功能 支持TimeLine函数自配(与JS工具统一)
4:简单的动画控件示例 
5:Web展示支持CEF3 (双消息循环模式,兼容CEF3消息循环) (支持页面类容OSR)