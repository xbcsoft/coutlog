#include "窗口\_启动窗口.h"

void onClientData(HTTP原始报服务器& 服务器, SOCKET 客户句柄, c_Bytes 数据)
{
	StrA httpData = BSA<true>(数据);

	ssize_t idxQuest = 寻找文本(httpData, "?");
	ssize_t idxHttp = 寻找文本(httpData, " HTTP/1.");
	StrA paramGetBuf;
	if (idxQuest != -1 && idxHttp != -1 && idxQuest < idxHttp) {
		paramGetBuf = 取文本子串(httpData, idxQuest + 1, idxHttp - idxQuest - 1);
	}

	HTTP键值对 GET参数;
	GET参数.添加_批量(paramGetBuf, true);

	ssize_t idxCL = 寻找文本(httpData, "Content-Length: ");
	int bodyLen = 0;
	if (idxCL != -1) {
		ssize_t idxNL = 寻找文本(httpData, "\r\n", idxCL);
		if (idxNL != -1) {
			StrA clStr = 删首尾空(取文本子串(httpData, idxCL + 16, idxNL - idxCL - 16));
			bodyLen = ToInt(clStr);
		}
	}

	Bytes body;
	if (bodyLen > 0 && 数据.size >= (size_t)bodyLen) {
		body.append(数据.buf + 数据.size - bodyLen, bodyLen);
	}

	_启动窗口.m_packageID++;

	StrA response = _启动窗口.HTTP业务逻辑(_启动窗口.m_packageID, GET参数, body);

	服务器.HTTP发送(客户句柄, nil, BR(response));
	服务器.断开客户(客户句柄);
}


StrA __启动窗口::HTTP业务逻辑(int dwConnID, HTTP键值对& GET参数, c_Bytes POST数据)
{
	StrA ctlVal = GET参数.取值("ctl");
	if (ctlVal.len() == 0) return "";

	int ctl = ToInt(ctlVal);

	if (ctl == 0) {
		编辑框log.内容_(L"");
		超级列表框1.全部删除();
		return "";
	}

	bool zusai = (GET参数.取值("zs") == "1");

	if (ctl == 1) {
		StrW log = U8toW(BSA<true>(POST数据));
		StrW now = 取SYSTEMTIME文本();

		if (编辑框log.取内容长度() == 0) {
			编辑框log.内容_(log);
		} else {
			编辑框log.加入文本(L"\r\n" + log);
		}

		if (选择框2.选中) {
			编辑框log.发送消息(EM_LINESCROLL, 0, 65535);
		}

		int i = 超级列表框1.插入表项(-1, now);
		超级列表框1.置标题(i, 1, log);
		if (选择框2.选中) {
			超级列表框1.保证显示(i);
		}

		if (zusai) {
			StrW replyVal;
			输入框(log, L"请输入", L"", replyVal);
			StrA reply = WtoU8(replyVal);
			return reply;
		}

		return "";
	}

	if (ctl == 2) {
		StrW port = 编辑框_端口.内容;
		StrW fileDir = g_模块目录 + port;
		创建目录(fileDir);

		StrA ext = GET参数.取值("ext");
		if (ext.len() == 0) {
			ext = "log";
		}

		StrW filepath = fileDir + L"\\" + ToStr<W>(dwConnID) + L"." + U8toW(ext);
		写到文件(filepath, POST数据);

		StrW relativeLogPath = LR"(%Temp%\cout\)" + port + L"\\" + ToStr<W>(dwConnID) + L"." + U8toW(ext);
		StrW now = 取SYSTEMTIME文本();

		StrW oldContent = 编辑框log._取内容();
		if (oldContent.len() == 0) {
			编辑框log.内容_(relativeLogPath);
		} else {
			编辑框log.内容_(oldContent + L"\r\n" + relativeLogPath);
		}

		if (选择框2.选中) {
			SendMessageW(编辑框log.窗口句柄, EM_LINESCROLL, 0, 65535);
		}

		int i = 超级列表框1.插入表项(-1, now);
		超级列表框1.置标题(i, 1, relativeLogPath + L"  (双击打开)");
		if (选择框2.选中) {
			超级列表框1.保证显示(i);
		}

		if (zusai) {
			StrW replyVal;
			输入框(relativeLogPath, L"请输入", L"", replyVal);
			StrA reply = WtoU8(replyVal);
			return reply;
		}

		return "";
	}

	return "";
}