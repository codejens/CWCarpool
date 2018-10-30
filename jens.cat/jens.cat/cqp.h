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
	int socket;// , ������, , , ����
	const char* Name;// , �ı���, , , �û��ǳ�
	const char* User;//, �ı���, , , ����
	const char* Wxid;// , �ı���, , , �շ���Ϣ�õ��û���ʶ�������ڣ���Q���QQ��
	const char* V1;// , �ı���, , , ���к��õı�ʶ���������Ժ���
	const char* Note;// , �ı���, , , ��ע
	const char* HeadImgUrl;// , �ı���, , , ͷ��url��ַ
	int Type;// , ������, , , �û����ͣ��������Ժ���

};
struct GroupMsg
{
	const char* Wxid;// �ı���, , , �շ���Ϣ�õ��û���ʶ�������ڣ���Q���Ⱥ��
	const char* Name;//, Ⱥ����
	Friend Member;// "1", Ⱥ�ĳ�Ա����һ������

};
*/

#define EVENT_IGNORE 0          //�¼�_����
#define EVENT_BLOCK 1           //�¼�_����

HINSTANCE hMod = LoadLibrary(stringToLPCWSTR("ecar.cat.dll"));
typedef int(WINAPI *TWO)(const char*, const char*);
//����˽����Ϣ
TWO CW_sendFriendsMsg = (TWO)GetProcAddress(hMod, "sendFriendsMsg");
//����Ⱥ����Ϣ
TWO CW_sendGroupMsg = (TWO)GetProcAddress(hMod, "sendGroupMsg");
//����ͼƬ��Ϣ
TWO CW_sendImageMsg = (TWO)GetProcAddress(hMod, "sendImageMsg");
//���ͽ��պ���ת��
TWO CW_receiveMoney = (TWO)GetProcAddress(hMod, "receiveMoney");
//ͬ���º�������
TWO CW_agreeNewFriend = (TWO)GetProcAddress(hMod, "agreeNewFriend");

typedef int(WINAPI *FIVE)(const char*, const char*, const char*, const char*, const char*);
//���ͷ�������
FIVE CW_sendShareUrl = (FIVE)GetProcAddress(hMod, "sendShareUrl");

typedef int(WINAPI *FOUR)(const char*, const char*, const char*, const char*);
//����Ⱥ��Ϣ������XX
FOUR CW_sendGroupMsgAt = (FOUR)GetProcAddress(hMod, "sendGroupMsgAt");

