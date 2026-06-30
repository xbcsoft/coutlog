#pragma once
#include <BEMod.h>
#include <windowsx.h>
#include <dwmapi.h>
#include "../程序集1.h"
#include "../resource.h"
#pragma comment(lib, "dwmapi.lib")

struct __启动窗口 : 窗口
{
	int m_packageID = 0;
	StrW g_模块目录;
	StrA m_现行选中项file;

	enum class TitleBtn { None, Minimize, Close };
	TitleBtn m_hoveredBtn = TitleBtn::None;
	TitleBtn m_pressedBtn = TitleBtn::None;
	bool m_isActive = true;

	void 事件_创建完毕();

	void _重启HTTP服务();

	void 事件_尺寸被改变();

	子菜单 popupMenu;
	void 事件_托盘(int 操作类型);

	bool 事件_被关闭();

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void 事件_菜单项被单击(int 菜单ID);

	void 事件_首次激活();

#pragma region 组件成员
	编辑框 编辑框log;

	struct _超级列表框1 : 超级列表框 {
		void 事件_右键单击表项();
	} 超级列表框1;

	struct _按钮1 : 按钮 {
		void 事件_被单击();
	} 按钮_清空;

	struct _按钮2 : 按钮 {
		void 事件_被单击();
	} 按钮_长窗口;

	struct _按钮4 : 按钮 {
		void 事件_被单击();
	} 按钮_切列表;

	struct _图片框1 : 图片框 {
		HandleCURSOR m_hDragCursor;

		bool 通用事件_鼠标左键被按下(int x, int y);

		bool 通用事件_鼠标左键被放开(int x, int y);
	} 图片框1;

	标签 标签1;

	struct _编辑框_端口 : 编辑框 {
		void 事件_内容被改变();
	}编辑框_端口;

	struct _超级链接框1 : 超级链接框 {
		bool 通用事件_鼠标左键被按下(int x, int y);
	} 超级链接框1;

	struct _选择框1 : 选择框 {
		void 事件_被单击();
	} 选择框1;

	struct _选择框2 : 选择框 {
	} 选择框2;

	编辑框 编辑框_cl;

#pragma endregion
	HTTP原始报服务器 http;
	StrA HTTP业务逻辑(int dwConnID, HTTP键值对& GET参数, c_Bytes POST数据);
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
}; extern __启动窗口 _启动窗口;
