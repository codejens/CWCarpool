#pragma once


#include <iostream>
using namespace std;
/*
#ifndef CQAPI
#define CQAPI(ReturnType) extern "C" __declspec(dllimport) ReturnType __stdcall
#endif
*/
#define CQEVENT(ReturnType, Name, Size) __pragma(comment(linker, "/EXPORT:" #Name "=_" #Name "@" #Size))\
	extern "C" __declspec(dllexport) ReturnType __stdcall Name

LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}
/*
struct Friend
{
	int socket;// , 整数型, , , 忽略
	const char* Name;// , 文本型, , , 用户昵称
	const char* User;//, 文本型, , , 忽略
	const char* Wxid;// , 文本型, , , 收发消息用的用户标识，类似于，酷Q里的QQ号
	const char* V1;// , 文本型, , , 打招呼用的标识，基本可以忽略
	const char* Note;// , 文本型, , , 备注
	const char* HeadImgUrl;// , 文本型, , , 头像url地址
	int Type;// , 整数型, , , 用户类型，基本可以忽略

};
struct GroupMsg
{
	const char* Wxid;// 文本型, , , 收发消息用的用户标识，类似于，酷Q里的群号
	const char* Name;//, 群名字
	Friend Member;// "1", 群的成员，是一个数组

};
*/

#define EVENT_IGNORE 0          //事件_忽略
#define EVENT_BLOCK 1           //事件_拦截

HINSTANCE hMod = LoadLibrary(stringToLPCWSTR("ecar.cat.dll"));
typedef int(WINAPI *TWO)(const char*, const char*);
//发送私聊消息
TWO CW_sendFriendsMsg = (TWO)GetProcAddress(hMod, "sendFriendsMsg");
//发送群聊信息
TWO CW_sendGroupMsg = (TWO)GetProcAddress(hMod, "sendGroupMsg");
//发送图片消息
TWO CW_sendImageMsg = (TWO)GetProcAddress(hMod, "sendImageMsg");
//发送接收好友转账
TWO CW_receiveMoney = (TWO)GetProcAddress(hMod, "receiveMoney");
//同意新好友请求
TWO CW_agreeNewFriend = (TWO)GetProcAddress(hMod, "agreeNewFriend");

typedef int(WINAPI *FIVE)(const char*, const char*, const char*, const char*, const char*);
//发送分享链接
FIVE CW_sendShareUrl = (FIVE)GetProcAddress(hMod, "sendShareUrl");

typedef int(WINAPI *FOUR)(const char*, const char*, const char*, const char*);
//发送群消息并艾特XX
FOUR CW_sendGroupMsgAt = (FOUR)GetProcAddress(hMod, "sendGroupMsgAt");

