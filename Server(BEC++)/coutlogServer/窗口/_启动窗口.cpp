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

	void 事件_创建完毕()
	{
		g_模块目录 = 取特定目录(目录::临时文件目录) +  LR"(\cout\)";
		创建目录(g_模块目录);

		超级列表框_置扩展编辑(超级列表框1.窗口句柄, [](HWND hwnd, int r, int c) {
			StrW text = _启动窗口.超级列表框1.取标题(r, c);
			if (文本_对比右边(text, L"  (双击打开)")) {
				StrW tempPath = 取特定目录(目录::临时文件目录) + L"\\";
				text = 子文本替换(text, L"%Temp%\\", tempPath);
				text = 子文本替换(text, L"  (双击打开)", L"");
				运行(text);
				return false; //阻止双击编辑展开
			}
			return true; //返回 true 允许编辑
		});

		// Register tray icon using the default application icon
		置托盘图标(IDI_ICON1, L"CoutlogServer");

		// 移除标题栏以隐藏系统原生标题栏，但保留 WS_THICKFRAME 以支持边缘大小调整
		LONG style = GetWindowLongW(窗口句柄, GWL_STYLE);
		style &= ~WS_CAPTION;
		style |= WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX;
		SetWindowLongW(窗口句柄, GWL_STYLE, style);

		// 用 DWM 恢复原生阴影（仅需底部 1px 即可触发阴影，消除顶部白边）
		MARGINS margins = { 0, 0, 0, 1 };
		DwmExtendFrameIntoClientArea(窗口句柄, &margins);

		// 强制刷新窗口框架
		SetWindowPos(窗口句柄, NULL, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void _重启HTTP服务()
	{
		int port = 到整数(编辑框_端口._取内容());
		http.关闭服务();
		// We'll declare the static onClientData callback below
		void onClientData(HTTP原始报服务器& 服务器, SOCKET 客户句柄, c_Bytes 数据);
		bool isOK = http.创建(port, nullptr, onClientData, nullptr, true);
		if (isOK) {
			标题_(L"CoutlogServer - working");
		} else {
			标题_(L"CoutlogServer - failed");
			信息框(L"端口:" + 编辑框_端口.内容 + L"已被占用", L"启动TCP失败");
		}
	}

	void 事件_尺寸被改变()
	{
		SIZEF sz = 取用户区大小();
		int w = sz.cx, h = sz.cy;
		int y = h - 26;

		// 其他子控件顺延 33 像素（标题栏 28px + 5px 边距）
		int contentY = 28 + 5;
		int contentH = h - contentY - 30; // 底部留出 35 像素用于其他控制按钮

		编辑框log.移动(10, contentY, w - 19, contentH);
		超级列表框1.移动(10, contentY, 编辑框log.宽度, 编辑框log.高度);

		int w0 = 超级列表框1.取列宽(0);
		超级列表框1.置列宽(1, w - 20 - w0 - 25);
		图片框1.移动(10, y-1, 24, 24);
		按钮_长窗口.移动(40, y, 52, 24);
		按钮_切列表.移动(98, y, 52, 24);
		标签1.移动(156, y+1, 24, 24);
		超级链接框1.移动(235, y+1, 80, 24);
		编辑框_端口.移动(184, y, 45, 24);
		选择框2.移动(w - 218, y, 72, 24);
		选择框1.移动(w - 142, y, 72, 24);
		按钮_清空.移动(w - 67, y, 52, 24);
	}

	子菜单 popupMenu;
	void 事件_托盘(int 操作类型)
	{
		if (操作类型 == 1) {
			可视_(true);
			置前台();
		} else if (操作类型 == 3) {
			弹出菜单(popupMenu);
		}
	}

	bool 事件_被关闭()
	{
		置托盘图标(Bytes());
		http.关闭服务();
		return true;
	}

	LRESULT 挂接消息(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_NCCALCSIZE:
			if (wParam == TRUE) {
				// 让客户区覆盖整个窗口，消除系统非客户区边框（解决白边+悬停坐标不一致问题）
				return 0;
			}
			break;



		case WM_SETTEXT:
		{
			LRESULT lr = 窗口::挂接消息(hwnd, msg, wParam, lParam);
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			int titleH = dpi(28);
			RECT rcTitle = { 0, 0, rcClient.right, titleH };
			InvalidateRect(hwnd, &rcTitle, TRUE);
			UpdateWindow(hwnd);
			return lr;
		}

		case WM_ERASEBKGND:
		{
			HDC hdc = (HDC)wParam;
			RECT rcClient = SIZEToRECT(取用户区大小<D>());
			int w = rcClient.right;

			int titleH = dpi(28);
			int btnW = dpi(46);
			int minOffset = btnW * 2;

			// 填充客户区背景
			FillRect(hdc, &rcClient, _背景画刷);

			// 绘制自定义标题栏背景 (高度28vpx)
			COLORREF titleBgColor;
			if (m_isActive) {
				HKEY hKey;
				if (RegOpenKeyExW(HKEY_CURRENT_USER, LR"(SOFTWARE\Microsoft\Windows\DWM)", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
					DWORD dwColor = 0;
					DWORD dwSize = sizeof(dwColor);
					// 读取注册表中的 AccentColor (格式为 AABBGGRR)
					if (RegQueryValueExW(hKey, L"AccentColor", NULL, NULL, (LPBYTE)&dwColor, &dwSize) == ERROR_SUCCESS) {
						int r = dwColor & 0xFF;
						int g = (dwColor >> 8) & 0xFF;
						int b = (dwColor >> 16) & 0xFF;
						titleBgColor = RGB(r, g, b);
					} else {
						// 备用：DWM API 获取 (格式为 AARRGGBB)
						DWORD dwDwmColor = 0;
						BOOL bOpaque = FALSE;
						if (SUCCEEDED(DwmGetColorizationColor(&dwDwmColor, &bOpaque))) {
							int r = (dwDwmColor >> 16) & 0xFF;
							int g = (dwDwmColor >> 8) & 0xFF;
							int b = dwDwmColor & 0xFF;
							titleBgColor = RGB(r, g, b);
						}
					}
					RegCloseKey(hKey);
				}
			} else {
				titleBgColor = RGB(255, 255, 255); // 失去焦点时变为纯白 (与原版一致)
			}
			HBRUSH hBrTitle = CreateSolidBrush(titleBgColor);
			RECT rcTitle = { 0, 0, w, titleH };
			FillRect(hdc, &rcTitle, hBrTitle);
			DeleteObject(hBrTitle);

			// 定义前景色（文字和图标线条）
			COLORREF foreColor = m_isActive ? RGB(255, 255, 255) : RGB(153, 153, 171);

			// 使用局部 lambda 完成提亮与变暗，避免外部成员缺失导致编译失败
			auto fnLightenColor = [](COLORREF col, double factor) -> COLORREF {
				int r = GetRValue(col);
				int g = GetGValue(col);
				int b = GetBValue(col);
				r = (int)(r + (255 - r) * factor);
				g = (int)(g + (255 - g) * factor);
				b = (int)(b + (255 - b) * factor);
				return RGB(r, g, b);
			};

			auto fnDarkenColor = [](COLORREF col, double factor) -> COLORREF {
				int r = (int)(GetRValue(col) * (1.0 - factor));
				int g = (int)(GetGValue(col) * (1.0 - factor));
				int b = (int)(GetBValue(col) * (1.0 - factor));
				return RGB(r, g, b);
			};

			// 绘制最小化按钮背景 & 悬停状态
			RECT rcMin = { w - minOffset, 0, w - btnW, titleH };
			COLORREF minBgColor = titleBgColor;
			if (m_pressedBtn == TitleBtn::Minimize) {
				minBgColor = m_isActive ? fnDarkenColor(titleBgColor, 0.20) : RGB(200, 200, 200);
			} else if (m_hoveredBtn == TitleBtn::Minimize) {
				minBgColor = m_isActive ? fnLightenColor(titleBgColor, 0.15) : RGB(224, 224, 224);
			}
			if (minBgColor != titleBgColor) {
				HBRUSH br = CreateSolidBrush(minBgColor);
				FillRect(hdc, &rcMin, br);
				DeleteObject(br);
			}

			// 绘制关闭按钮背景 & 悬停状态
			RECT rcClose = { w - btnW, 0, w, titleH };
			COLORREF closeBgColor = titleBgColor;
			if (m_pressedBtn == TitleBtn::Close) {
				closeBgColor = RGB(196, 43, 28);
			} else if (m_hoveredBtn == TitleBtn::Close) {
				closeBgColor = RGB(232, 17, 35);
			}
			if (closeBgColor != titleBgColor) {
				HBRUSH br = CreateSolidBrush(closeBgColor);
				FillRect(hdc, &rcClose, br);
				DeleteObject(br);
			}

			// 用 GDI 绘制极其清晰的最小化按钮横线 (10vpx 宽)
			COLORREF minIconColor = foreColor;
			if (m_hoveredBtn == TitleBtn::Minimize || m_pressedBtn == TitleBtn::Minimize) {
				minIconColor = m_isActive ? RGB(255, 255, 255) : RGB(0, 0, 0);
			}
			HPEN hPenMin = CreatePen(PS_SOLID, 1, minIconColor);
			HPEN hOldPen = (HPEN)SelectObject(hdc, hPenMin);

			int minCx = w - btnW - btnW / 2;
			int minCy = titleH / 2;
			int halfIcon = dpi(5);
			MoveToEx(hdc, minCx - halfIcon, minCy, NULL);
			LineTo(hdc, minCx + halfIcon, minCy);

			SelectObject(hdc, hOldPen);
			DeleteObject(hPenMin);

			// 绘制关闭按钮 (10vpx 的 X 形)
			COLORREF closeIconColor = foreColor;
			if (m_hoveredBtn == TitleBtn::Close || m_pressedBtn == TitleBtn::Close) {
				closeIconColor = RGB(255, 255, 255); // 无论是否激活，悬停/按下均使用白色
			}
			HPEN hPenClose = CreatePen(PS_SOLID, 1, closeIconColor);
			hOldPen = (HPEN)SelectObject(hdc, hPenClose);

			int closeCx = w - btnW / 2;
			int closeCy = titleH / 2;
			MoveToEx(hdc, closeCx - halfIcon, closeCy - halfIcon, NULL);
			LineTo(hdc, closeCx + halfIcon, closeCy + halfIcon);
			MoveToEx(hdc, closeCx + halfIcon, closeCy - halfIcon, NULL);
			LineTo(hdc, closeCx - halfIcon, closeCy + halfIcon);

			SelectObject(hdc, hOldPen);
			DeleteObject(hPenClose);

			// 绘制窗口标题文字 (从系统自动获取原生标题栏字体样式)
			SetTextColor(hdc, foreColor);
			SetBkMode(hdc, TRANSPARENT);

			LOGFONTW lfTitle = { 0 };
			NONCLIENTMETRICSW ncm = { 0 };
			ncm.cbSize = sizeof(ncm);
			// 跨版本安全获取系统非客户区
			if (!SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0)) {
				ncm.cbSize = sizeof(ncm) - sizeof(ncm.iPaddedBorderWidth);
				SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
			}

			lfTitle = ncm.lfCaptionFont;
			HFONT hFontTitle = CreateFontIndirectW(&lfTitle);
			HFONT hOldFontTitle = (HFONT)SelectObject(hdc, hFontTitle);

			StrW title = 窗口_取标题(窗口句柄);
			RECT rcText = { dpi(10), 0, w - minOffset - dpi(10), titleH };
			DrawTextW(hdc, title, -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

			SelectObject(hdc, hOldFontTitle);
			DeleteObject(hFontTitle);

			return TRUE;
		}

		case WM_NCHITTEST:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			RECT rcWnd;
			GetWindowRect(hwnd, &rcWnd);

			int x = pt.x - rcWnd.left;
			int y = pt.y - rcWnd.top;
			int w = rcWnd.right - rcWnd.left;
			int h = rcWnd.bottom - rcWnd.top;

			int titleH = dpi(28);
			int btnW = dpi(46);
			int minOffset = btnW * 2;
			int border = dpi(5);

			// 0. 特大化右下角拉伸容错区域 (方便鼠标抓取拉伸，宽度30px * DPI, 高度40px * DPI)
			if (x >= w - dpi(30) && y >= h - dpi(40)) {
				return HTBOTTOMRIGHT;
			}

			// 1. 如果在右上角自定义按钮区域，强制返回 HTCLIENT，使按钮完全响应客户区鼠标消息，哪怕最边缘也是如此
			if (y < titleH && x >= w - minOffset) {
				return HTCLIENT;
			}

			// 2. 手动处理边缘拉伸（因为无边框时客户区占据全部大小）
			if (x < border && y < border) return HTTOPLEFT;
			if (x > w - border && y < border) return HTTOPRIGHT;
			if (x < border && y > h - border) return HTBOTTOMLEFT;
			if (x > w - border && y > h - border) return HTBOTTOMRIGHT;
			if (x < border) return HTLEFT;
			if (x > w - border) return HTRIGHT;
			if (y < border) return HTTOP;
			if (y > h - border) return HTBOTTOM;

			// 3. 自定义标题栏区域拖拽
			if (y < titleH) {
				if (x < w - minOffset) {
					return HTCAPTION;
				}
			}
			return HTCLIENT;
		}

		case WM_MOUSEMOVE:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			int w = rcClient.right;

			int titleH = dpi(28);
			int btnW = dpi(46);
			int minOffset = btnW * 2;

			TitleBtn btn = TitleBtn::None;
			if (pt.y >= 0 && pt.y < titleH) {
				if (pt.x >= w - btnW && pt.x < w) {
					btn = TitleBtn::Close;
				} else if (pt.x >= w - minOffset && pt.x < w - btnW) {
					btn = TitleBtn::Minimize;
				}
			}

			if (btn != m_hoveredBtn) {
				m_hoveredBtn = btn;
				RECT rcTitle = { w - minOffset, 0, w, titleH };
				InvalidateRect(hwnd, &rcTitle, TRUE);
				UpdateWindow(hwnd);
			}

			// 跟踪鼠标离开事件
			TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
			TrackMouseEvent(&tme);
			break;
		}

		case WM_MOUSELEAVE:
			if (m_hoveredBtn != TitleBtn::None) {
				m_hoveredBtn = TitleBtn::None;
				RECT rcClient;
				GetClientRect(hwnd, &rcClient);
				int w = rcClient.right;
				int titleH = dpi(28);
				int btnW = dpi(46);
				int minOffset = btnW * 2;
				RECT rcTitle = { w - minOffset, 0, w, titleH };
				InvalidateRect(hwnd, &rcTitle, TRUE);
				UpdateWindow(hwnd);
			}
			break;

		case WM_LBUTTONDOWN:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			int w = rcClient.right;
			int titleH = dpi(28);
			int btnW = dpi(46);
			int minOffset = btnW * 2;

			TitleBtn btn = TitleBtn::None;
			if (pt.y >= 0 && pt.y < titleH) {
				if (pt.x >= w - btnW && pt.x < w) {
					btn = TitleBtn::Close;
				} else if (pt.x >= w - minOffset && pt.x < w - btnW) {
					btn = TitleBtn::Minimize;
				}
			}

			if (btn != TitleBtn::None) {
				m_pressedBtn = btn;
				SetCapture(hwnd);
				RECT rcTitle = { w - minOffset, 0, w, titleH };
				InvalidateRect(hwnd, &rcTitle, TRUE);
				UpdateWindow(hwnd);
				return 0;
			}
			break;
		}

		case WM_LBUTTONUP:
			if (m_pressedBtn != TitleBtn::None) {
				ReleaseCapture();
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				RECT rcClient;
				GetClientRect(hwnd, &rcClient);
				int w = rcClient.right;
				int titleH = dpi(28);
				int btnW = dpi(46);
				int minOffset = btnW * 2;

				TitleBtn btn = TitleBtn::None;
				if (pt.y >= 0 && pt.y < titleH) {
					if (pt.x >= w - btnW && pt.x < w) {
						btn = TitleBtn::Close;
					} else if (pt.x >= w - minOffset && pt.x < w - btnW) {
						btn = TitleBtn::Minimize;
					}
				}

				TitleBtn oldPressed = m_pressedBtn;
				m_pressedBtn = TitleBtn::None;
				RECT rcTitle = { w - minOffset, 0, w, titleH };
				InvalidateRect(hwnd, &rcTitle, TRUE);
				UpdateWindow(hwnd);

				if (btn == oldPressed) {
					if (btn == TitleBtn::Minimize) {
						可视_(false);
					} else if (btn == TitleBtn::Close) {
						投递消息(WM_CLOSE, 0, 0);
					}
				}
				return 0;
			}
			break;

		case WM_NCACTIVATE:
		{
			m_isActive = (wParam != FALSE);
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			int titleH = dpi(28);
			RECT rcTitle = { 0, 0, rcClient.right, titleH };
			InvalidateRect(hwnd, &rcTitle, TRUE);
			UpdateWindow(hwnd);
			return TRUE;
		}

		case WM_ACTIVATE:
		{
			m_isActive = (LOWORD(wParam) != WA_INACTIVE);
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			int titleH = dpi(28);
			RECT rcTitle = { 0, 0, rcClient.right, titleH };
			InvalidateRect(hwnd, &rcTitle, TRUE);
			UpdateWindow(hwnd);
		}
		}
		return 窗口::挂接消息(hwnd, msg, wParam, lParam);
	}

	void 事件_菜单项被单击(int 菜单ID)
	{
		if (菜单ID == 1001) {
			运行(U8toW(m_现行选中项file));
		} else if (菜单ID == 1002) {
			定位文件(m_现行选中项file);
		} else if (菜单ID == ID_40001) {
			可视_(true);
			置前台();
		} else if (菜单ID == ID_40002) {
			static 超级链接框 超链;
			if (!超链.窗口句柄) {
				超级链接框::参数 cs;
				cs.标题 = L"https://github.com/xbcsoft/coutlog";
				超链.创建(cs);
			}
			信息框_DIY({}, "　　　　　　↑本软件开源仓库↑\r\n交流QQ群：668536886（白易语言研究院）", 0, "由白易开发支持(xbcsoft)", [](HWND hwnd) {
				超链.窗口置父(hwnd);
				超链.移动(10, 10, 300, 30);
			});
		} else if (菜单ID == ID_40003) {
			销毁();
		}
	}

	void 事件_首次激活()
	{
		_重启HTTP服务();
	}

#pragma region 组件成员
	编辑框 编辑框log;

	struct _超级列表框1 : 超级列表框 {
		void 事件_右键单击表项() {
			int row = 现行选中项;
			if (row == -1) return;
			StrW a = 取标题(row, 1);
			if (取文本左边(a, 7) == L"%Temp%\\") {
				StrW tempPath = 取特定目录(目录::临时文件目录) + L"\\";
				a = 子文本替换(a, L"%Temp%\\", tempPath);
				a = 子文本替换(a, L"  (双击打开)", L"");
				_启动窗口.m_现行选中项file = WtoU8(a);

				子菜单 popupMenu;
				popupMenu.创建();
				popupMenu.添加项(1001, L"打开");
				popupMenu.添加项(1002, L"打开所在目录");

				弹出菜单(popupMenu);
			}
		}
	} 超级列表框1;

	struct _按钮1 : 按钮 {
		void 事件_被单击() {
			_启动窗口.编辑框log.内容_(L"");
			_启动窗口.超级列表框1.全部删除();
		}
	} 按钮_清空;

	struct _按钮2 : 按钮 {
		void 事件_被单击() {
			RECTF rc = _启动窗口.取窗口矩形();
			int height = rc.bottom - rc.top;
			if (height > 300) {
				_启动窗口.移动(空, 空, 空, 196);
				标题_(L"长窗口");
			} else {
				_启动窗口.移动(空, 空, 空, 500);
				标题_(L"短窗口");
			}
		}
	} 按钮_长窗口;

	struct _按钮4 : 按钮 {
		void 事件_被单击() {
			if (_启动窗口.超级列表框1.可视) {
				_启动窗口.超级列表框1.可视_(false);
				_启动窗口.编辑框log.可视_(true);
				标题_(L"切列表");
			} else {
				_启动窗口.编辑框log.可视_(false);
				_启动窗口.超级列表框1.可视_(true);
				标题_(L"切编辑");
			}
		}
	} 按钮_切列表;

	struct _图片框1 : 图片框 {
		HandleCURSOR m_hDragCursor;

		bool 通用事件_鼠标左键被按下(int x, int y) {

			if (!m_hDragCursor) {
				m_hDragCursor = 光标::从内存创建(R::图片框_指针);
			}
			if (m_hDragCursor) {
				SetCursor(m_hDragCursor);
			}

			Sleep(20);
			置图片(Bytes());
			SetCapture(窗口句柄);
			return true;
		}

		bool 通用事件_鼠标左键被放开(int x, int y) {
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			置图片(R::图片框_指针);

			HWND hwndTarget = 窗口_取鼠标所在句柄();
			if (hwndTarget) {
				HWND myHwnd = _启动窗口.窗口句柄;
				if (窗口_取祖宗句柄(hwndTarget) == myHwnd || hwndTarget == myHwnd) {
					return true;
				}

				窗口基类 targetWin;
				targetWin.窗口句柄 = hwndTarget;
				RectS a = targetWin.取窗口大小版矩形();
				targetWin.窗口句柄 = NULL; // 释放句柄避免析构时销毁目标窗口

				_启动窗口.移动(a.x - 6, a.y, a.w + 6, a.h, true);
			}
			return true;
		}
	} 图片框1;

	标签 标签1;

	struct _编辑框_端口 : 编辑框 {
		void 事件_内容被改变() {
			_启动窗口._重启HTTP服务();
		}
	}编辑框_端口;

	struct _超级链接框1 : 超级链接框 {
		bool 通用事件_鼠标左键被按下(int x, int y) {
			StrW portDir = _启动窗口.g_模块目录 + _启动窗口.编辑框_端口._取内容();
			创建目录(portDir);
			运行(portDir);
			return true;
		}
	} 超级链接框1;

	struct _选择框1 : 选择框 {
		void 事件_被单击() {
			_启动窗口.编辑框log.滚动条_(_启动窗口.选择框1._取选中() ? 编辑框滚动条::纵向滚动条 : 编辑框滚动条::横向及纵向滚动条);
		}
	} 选择框1;

	struct _选择框2 : 选择框 {
	} 选择框2;

	编辑框 编辑框_cl;

#pragma endregion
	HTTP原始报服务器 http;
	StrA HTTP业务逻辑(int dwConnID, HTTP键值对& GET参数, c_Bytes POST数据);
	void 载入(窗口* 父窗 = 0, bool 模态 = 0);
	void 完毕(bool 模态 = 0);
} _启动窗口;
