// jens.cat.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "cqp.h"
#include <tchar.h>
#include <string>
#include <iostream>
#include <time.h>
#include <atltime.h>
#include <vector>
#include <regex>
#include <direct.h>
#include <map>
#include <io.h>
#include "atlstr.h"

using namespace std;

bool is_monitor = false;
bool is_manage = false;
int sgin_add_coin = 10;
int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;

bool IS_INIT = false;
CString SUFFIX = ".ini";
vector<string> vecstr_manager_qqs;
vector<string> vecstr_monitor_qqs;
TCHAR SIGN_PATH[100];
TCHAR CAR_FIND_PEOPLE_PATH[100];
TCHAR PEOPLE_FIND_CAR_PATH[100];
TCHAR EXPIRED_CAR_FIND_PEOPLE_PATH[100];
TCHAR EXPIRED_PEOPLE_FIND_CAR_PATH[100];
map<const char*, string>mapSignPathInfo;
map<const char*, string>mapCarPathInfo;
map<const char*, string>mapPeoplePathInfo;
map<const char*, string>mapCarExPathInfo;
map<const char*, string>mapPeopleExPathInfo;

//int _CALL(int a, const char *b = NULL, const char *c = NULL, int *d = NULL, int *e = NULL, int *f = NULL, int *g = NULL, int *h = NULL, int *i = NULL, int *j = NULL);
/*
LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);


	return wcstring;
}*/

std::wstring  StringToWString(const std::string& s)
{
	std::wstring wszStr;

	int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
	wszStr.resize(nLength);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
	wszStr = lpwszStr;
	delete[] lpwszStr;
	return wszStr;
}



std::string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	delete chRtn;
	return str;
}
//LPCWSTR stringToLPCWSTR(std::string orig)
//{
//	size_t origsize = orig.length() + 1;
//	const size_t newsize = 100;
//	size_t convertedChars = 0;
//	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
//	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
//
//	return wcstring;
//}

std::string time2string(time_t t)
{
	struct tm* tmNow = localtime(&t);
	char timeStr[sizeof("yyyy-mm-dd hh:mm:ss")] = { '\0' };
	strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tmNow);
	return timeStr;
}
time_t string2time(const std::string& timeStr)
{
	struct tm stTm;
	sscanf_s(timeStr.c_str(), "%d-%d-%d %d:%d:%d",
		&(stTm.tm_year),
		&(stTm.tm_mon),
		&(stTm.tm_mday),
		&(stTm.tm_hour),
		&(stTm.tm_min),
		&(stTm.tm_sec));

	stTm.tm_year -= 1900;
	stTm.tm_mon--;
	stTm.tm_isdst = -1;
	return mktime(&stTm);
}

CTime timestr(LPSTR str)
{
	USES_CONVERSION;
	LPSTR strSQL = str;
	int nYear, nMonth, nDate, nHour, nMin, nSec;
	nYear = nMonth = nDate = nHour = nMin = nSec = 0;
	sscanf_s(strSQL, "%d-%d-%d  %d:%d:%d", &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec);
	CTime t(nYear, nMonth, nDate, 0, 0, 0);
	return t;
}

CStringA atQQ(int64_t fromQQ)
{
	CString cs_qq;
	cs_qq.Format(_T("[CQ:at,qq=%d]\n"), fromQQ);
	return (CStringA)cs_qq;
}

void sendMsg(const char* from_wxid, const char *msg)
{
	if (is_manage)
	{
		CW_sendGroupMsg(from_wxid, msg);
	}
}

void sendMsgAt(const char* from_wxid, const char* from_private_wxid, const char* from_name, const char *msg)
{
	if (is_manage)
	{
		CW_sendGroupMsgAt(from_wxid, from_private_wxid, from_name, msg);
	}
}

CString GetPath(string file_name)
{
	TCHAR FPath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, FPath, MAX_PATH);
	TCHAR *pFind = wcsrchr(FPath, TEXT('\\'));
	*pFind = '\0';
	CString szIniPath = FPath;
	szIniPath += "\\";
	szIniPath += file_name.c_str();
	return szIniPath;
}
// �����������ж��ļ����Ƿ����,�����ھʹ���
// example: /home/root/mkdir/1/2/3/4/
// ע��:���һ��������ļ��еĻ�,��Ҫ���� '\' ���� '/'
int32_t createDirectory(std::string &directoryPath)
{
	uint32_t dirPathLen = directoryPath.length();
	if (dirPathLen > 100)
	{
		return -1;
	}
	char tmpDirPath[100] = { 0 };
	for (uint32_t i = 0; i < dirPathLen; ++i)
	{
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (_access(tmpDirPath, 0) != 0)
			{
				int32_t ret = _mkdir(tmpDirPath);
				if (ret != 0)
				{
					return ret;
				}
			}
		}
	}
	return 0;
}
void InitPath(const char* fromGroup)
{
	if (mapCarPathInfo.count(fromGroup) <= 0)
	{
		if (IS_INIT == false)
		{
			CString path = GetPath("���ñ�.ini");
			GetPrivateProfileString(TEXT("ͨ��"), TEXT("ǩ��"), NULL, SIGN_PATH, 100, path);
			GetPrivateProfileString(TEXT("ͨ��"), TEXT("������"), NULL, CAR_FIND_PEOPLE_PATH, 100, path);
			GetPrivateProfileString(TEXT("ͨ��"), TEXT("���ҳ�"), NULL, PEOPLE_FIND_CAR_PATH, 100, path);
			GetPrivateProfileString(TEXT("ͨ��"), TEXT("�����˹���"), NULL, EXPIRED_CAR_FIND_PEOPLE_PATH, 100, path);
			GetPrivateProfileString(TEXT("ͨ��"), TEXT("���ҳ�����"), NULL, EXPIRED_PEOPLE_FIND_CAR_PATH, 100, path);
			IS_INIT = true;
		}
		mapSignPathInfo[fromGroup] = TCHAR2STRING(SIGN_PATH);
		mapCarPathInfo[fromGroup] = TCHAR2STRING(CAR_FIND_PEOPLE_PATH);
		mapPeoplePathInfo[fromGroup] = TCHAR2STRING(PEOPLE_FIND_CAR_PATH);
		mapCarExPathInfo[fromGroup] = TCHAR2STRING(EXPIRED_CAR_FIND_PEOPLE_PATH);
		mapPeopleExPathInfo[fromGroup] = TCHAR2STRING(EXPIRED_PEOPLE_FIND_CAR_PATH);
	}
}


bool IsFindCar(string str_msg)
{
	if (str_msg.find("�ҳ�") != string::npos)
		return true;
	else
		return false;
}
bool IsFindPeople(string str_msg)
{
	if (str_msg.find("����") != string::npos)
		return true;
	else
		return false;
}
bool IsFindMusic(string str_msg)
{
	if (str_msg.find("���") != string::npos)
		return true;
	else
		return false;
}
void SendRecharge(const char* from_wxid, const char* final_from_wxid, int need_coin)
{
	string send_str_msg;
	send_str_msg.append(final_from_wxid);
	send_str_msg.append("���ֲ��㣬��Ҫ");
	send_str_msg.append(to_string(abs(need_coin)));
	send_str_msg.append("���֣��뱣��ÿ��ǩ�����߳�ֵ");
	sendMsg(from_wxid, send_str_msg.c_str());
}
int AddCoin(const char* from_wxid, const char* from_private_wxid, int add_coin, string s_city)
{
	CString cs_qq(from_private_wxid);
	CString group(from_wxid);
	char sign_path[MAX_PATH] = { 0 };
	sprintf_s(sign_path, sizeof(sign_path), mapSignPathInfo[from_wxid].c_str(), s_city.c_str());
	CString szIniPath = sign_path + group + SUFFIX;
	time_t t = time(NULL);
	string cs_now_time = time2string(t);
	TCHAR szTime[20] = { 0 };
	if (add_coin > 0)
	{
		DWORD num = ::GetPrivateProfileString(cs_qq, TEXT("Time"), NULL, szTime, MAX_PATH, szIniPath);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		CTime t2 = NULL;
		if (num != 0)
		{
			t2 = timestr((LPSTR)TCHAR2STRING(szTime).c_str());

			if (t1 <= t2)
			{
				return -1; //�ظ�ǩ��
			}
		}
	}
	int coin = ::GetPrivateProfileInt(cs_qq, TEXT("Coin"), 0, szIniPath);
	coin += add_coin;
	if (coin >= 0)
	{
		CString cs_coin;
		cs_coin.Format(_T("%d"), coin);
		::WritePrivateProfileString(cs_qq, TEXT("Coin"), cs_coin, szIniPath);
		if (add_coin > 0)
		{
			::WritePrivateProfileString(cs_qq, TEXT("Time"), stringToLPCWSTR(cs_now_time), szIniPath);
		}
		if (!::PathFileExists(szIniPath))
		{
			return -3; //�ļ�����ʧ��
		}
		return coin;
	}
	else
	{
		return -2;	//���ֲ���
	}
}

int GroupSignIn(const char* from_wxid, const char* from_private_wxid, const char* from_name, string s_city)
{
	int coin = AddCoin(from_wxid, from_private_wxid, sgin_add_coin, s_city);
	if (coin > 0)
	{
		string send_str_msg;
		send_str_msg.append("\nǩ���ɹ�\n��������");
		send_str_msg.append(to_string(sgin_add_coin).c_str());
		send_str_msg.append("��!\n�������");
		send_str_msg.append(to_string(coin).c_str());
		send_str_msg.append("��!");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	else if (coin == -1)
	{
		string send_str_msg;
		send_str_msg.append("\nǩ��ʧ��\n�����Ѿ�ǩ������Ŷ��");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	else if (coin == -2)
	{
		string send_str_msg;
		send_str_msg.append("\nǩ��ʧ��\n��Ļ�����");
		send_str_msg.append(to_string(coin));
		send_str_msg.append("\n����ϵ����");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	return EVENT_BLOCK;
}
//�ж��ַ��Ƿ�������
int is_zh_ch(char p){
	if (~(p >> 8) == 0)
	{
		return 1;
	}
	return -1;
}

string sub(string str, int start, int end){
	if (typeid(str) == typeid(string) && str.length() > 0){
		int len = str.length();
		string tmp = "";
		vector <string> dump;
		int i = 0;
		while (i < len)
		{
			if (is_zh_ch(str.at(i)) == 1)
			{
				dump.push_back(str.substr(i, 2));
				i = i + 2;
			}
			else{
				dump.push_back(str.substr(i, 1));
				i = i + 1;
			}
		}
		end = end > 0 ? end : dump.size();
		if (start<0 || start>end){
			printf("�������ô��󣬵�һ������Ӧ�ô��ڵ���0��С�ڵڶ���������\n");
		}
		for (int i = start; i <= end; i++)
		{
			tmp += dump[i - 1];
		}
		return tmp;
	}
	else
	{
		printf("�������ַ�����\n");
		return "";
	}
}

string&   replace_all_distinct(string&   str, const  string&  old_value, const   string&   new_value)
{
	for (string::size_type pos(0); pos != string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else  { break; }
	}
	return   str;
}
//std::wstring  StringToWString(const std::string& s)
//{
//	std::wstring wszStr;
//
//	int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
//	wszStr.resize(nLength);
//	LPWSTR lpwszStr = new wchar_t[nLength];
//	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
//	wszStr = lpwszStr;
//	delete[] lpwszStr;
//	return wszStr;
//}

void GetIphone(string &iphone, string str)
{
	std::string iphone_pattern("(13|14|15|17|18|19)[0-9]{9}");
	std::regex iphone_r(iphone_pattern);
	for (std::sregex_iterator iphone_it(str.begin(), str.end(), iphone_r), end;     //end��β���������regex_iterator��regex_iterator��string���͵İ汾
		iphone_it != end;
		++iphone_it)
	{
		string tmp_iphone = iphone_it->str();
		if (iphone == "")
		{
			iphone = tmp_iphone;
		}
		else
		{
			if (tmp_iphone.c_str() > iphone.c_str())
			{
				iphone = tmp_iphone;
			}
		}
	}
}
int ChooseASong(string str_msg, const char* from_wxid, const char* from_private_wxid, const char* from_name, string s_city)
{
	if (str_msg != "���")
	{
		//���������ĵ��
		int need_coin = -1;
		int coin = AddCoin(from_wxid, from_private_wxid, -1, s_city);
		if (coin >= 0)
		{
			string send_str_msg;
			send_str_msg.append("\n���ɹ�������");
			send_str_msg.append(to_string(need_coin));
			send_str_msg.append("����������....\n���δ�н������������ָ���Ƿ���ȷ�����+���������磺\n��躣�����");
			//send_str_msg.append("Tencent://QQMusic/?version==700&&cmd_count==2&&cmd_0==4002&&clienttype_0==0&&playmusic_0==http://qqmusic.qq.com/fcgi-bin/qm_getmusic.fcg?musicid=100&ishide=1&&cmd_1==4003&&clienttype_1==0&&jmpurl_1==");
			sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
			return EVENT_IGNORE;
		}
		else if (coin == -2)
		{
			SendRecharge(from_wxid, from_private_wxid, need_coin);
			return EVENT_BLOCK;
		}
		else
		{
			//δ֪����,�����ļ�����ʧ��
			return EVENT_BLOCK;
		}
	}
	else
	{
		string send_str_msg;
		send_str_msg.append("\n���͵��+���������磺\n");
		send_str_msg.append("��躣�����");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
		return EVENT_IGNORE;
	}
}
/*
*ȥ���ַ�����β�� \x20 \r \n �ַ�
*/
void TrimSpace(char* str)
{
	char *start = str - 1;
	char *end = str;
	char *p = str;
	while (*p)
	{
		switch (*p)
		{
		case ' ':
		case '\r':
		case '\n':
		{
					 if (start + 1 == p)
						 start = p;
		}
			break;
		default:
			break;
		}
		++p;
	}
	//�����������ַ�����β�� ������ǰ
	--p;
	++start;
	if (*start == 0)
	{
		//�Ѿ����ַ�����ĩβ��
		*str = 0;
		return;
	}
	end = p + 1;
	while (p > start)
	{
		switch (*p)
		{
		case ' ':
		case '\r':
		case '\n':
		{
					 if (end - 1 == p)
						 end = p;
		}
			break;
		default:
			break;
		}
		--p;
	}
	memmove(str, start, end - start);
	*(str + (int)end - (int)start) = 0;
}


void TrimSpaceReturn(char *strStr)
{
	if (strlen(strStr) < 1)
		return;
	int i = 0;
	int len = strlen(strStr);
	for (i = len - 1; i >= 0; i--)
	{
		if (strStr[i] == 0x20 || strStr[i] == 0x0D || strStr[i] == 0x0A || strStr[i] == '"')
			strStr[i] = 0;
		else
			break;
	}
	len = i + 1;
	i = 0;
	for (i = 0; i < len; i++){
		if (strStr[i] != 0x20 && strStr[i] != 0x0D && strStr[i] != 0x0A && strStr[i] != '"')
			break;
	}
	if (i == len)
		strcpy_s(strStr, 20, "");
	else
		strcpy_s(strStr, strlen(strStr + i) + 20, strStr + i);
}

string GetDateEx(string str, string pattern)
{
	std::regex date_r(pattern);
	string date = "";
	for (std::sregex_iterator it(str.begin(), str.end(), date_r), end;     //end��β���������regex_iterator��regex_iterator��string���͵İ汾
		it != end;
		++it)
	{
		date = it->str();
		if (date != "")
		{
			return date;
		}
	}
	return "";
}

int GetMonthDayCount(int year = 0, int month = 0)
{
	time_t t = time(NULL);
	string cs_now_time = time2string(t);
	CTime t1 = timestr((LPSTR)cs_now_time.c_str());
	int iYear = t1.GetYear();
	int iMonth = t1.GetMonth();
	if (year > 0)
		iYear = year;
	if (month > 0)
		iMonth = month;
	int a = (iYear % 4 == 0 && iYear % 100 != 0) || iYear % 400 == 0;
	if (iMonth == 1, iMonth == 3, iMonth == 5, iMonth == 7, iMonth == 8, iMonth == 10, iMonth == 12)
		return 31;
	else if (iMonth == 2)
	{
		if (a == 1)
			return 29;
		else
			return 28;
	}
	else
		return 30;
}

string GetDate(string str)
{
	//std::string date_pattern("[0-9]{1,2}��[0-9]{1,2}��");
	string date = GetDateEx(str, "[0-9]+\u6708[0-9]+");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();

		//string month_date = GetDateEx(date, "[0-9]+\u6708");
		//month_date = GetDateEx(month_date, "-?[1-9]\d*");
		//int month = atoi(month_date.c_str());
		//if (month > 1)
		//{
		//	replace_all_distinct(date, month_date, "");
		//}

		date = to_string(year) + "-" + date;
		string new_date = replace_all_distinct(date, "��", "-");
		if (strlen(new_date.c_str()) >= 10)
		{
			string check_date = GetDateEx(date, "-[0-9]+-");
			if (check_date != "")
			{
				string num = GetDateEx(check_date, "[1-9]\d*");
				if (strlen(check_date.c_str()) >= 4)
				{
					if (atoi(num.c_str()) > 1)
					{
						check_date = GetDateEx(date, "[0-9]{1,2}$");
						if (check_date != "")
						{
							char c_new2[10];
							sprintf_s(c_new2, sizeof(c_new2), "%d-%d-%s", t1.GetYear(), t1.GetMonth(), check_date.c_str());
							new_date = c_new2;
						}
						else
						{
							char c_new2[10];
							sprintf_s(c_new2, sizeof(c_new2), "%d-%d-%d", t1.GetYear(), t1.GetMonth(), t1.GetDay());
							new_date = c_new2;
						}
					}
				}
				else
				{
					//�����·������⣬�Ǿ���x��������
					char c_new2[10];
					sprintf_s(c_new2, sizeof(c_new2), "%d-%d-%d", t1.GetYear(), t1.GetMonth(), t1.GetDay());
					new_date = c_new2;
				}
			}
		}
		return new_date;
	}
	//529
	//115,34,21
	date = GetDateEx(str, "[0-9]+\u53f7");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();
		date = replace_all_distinct(date, "��", "");
		int day = atoi(date.c_str());
		if (day > 99)
		{
			int rel_day = day % 100;
			day = rel_day;
		}
		else if (day > 31)
		{
			day = 31;
		}
		string new_date = to_string(year) + "-" + to_string(t1.GetMonth()) + "-" + to_string(day);
		return new_date;
	}
	date = GetDateEx(str, "[0-9]+\u65e5");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();
		date = replace_all_distinct(date, "��", "");
		int day = atoi(date.c_str());
		if (day > 99)
		{
			int rel_day = day % 100;
			day = rel_day;
		}
		else if (day > 31)
		{
			day = 31;
		}
		string new_date = to_string(year) + "-" + to_string(t1.GetMonth()) + "-" + to_string(day);
		return new_date;
	}
	date = GetDateEx(str, "����");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();
		string new_date = to_string(year) + "-" + to_string(t1.GetMonth()) + "-" + to_string(t1.GetDay());
		return new_date;
	}
	date = GetDateEx(str, "����");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();
		string new_date = to_string(year) + "-" + to_string(t1.GetMonth()) + "-" + to_string(t1.GetDay());
		return new_date;
	}
	/**/
	date = GetDateEx(str, "����");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();
		int month = t1.GetMonth();
		int day = t1.GetDay() + 1;
		int max_day = GetMonthDayCount();
		if (day > max_day)
		{
			//�·�Ҫ+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//���Ҫ��1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		string new_date = to_string(year) + "-" + to_string(month) + "-" + to_string(day);
		return new_date;
	}
	date = GetDateEx(str, "����");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();
		int month = t1.GetMonth();
		int day = t1.GetDay() + 1;
		int max_day = GetMonthDayCount();
		if (day > max_day)
		{
			//�·�Ҫ+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//���Ҫ��1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		day += 1;
		max_day = GetMonthDayCount(year, month);
		if (day > max_day)
		{
			//�·�Ҫ+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//���Ҫ��1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		string new_date = to_string(year) + "-" + to_string(month) + "-" + to_string(day);
		return new_date;
	}
	date = GetDateEx(str, "�����");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();
		int month = t1.GetMonth();
		int day = t1.GetDay() + 3;
		int max_day = GetMonthDayCount();
		if (day > max_day)
		{
			//�·�Ҫ+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//���Ҫ��1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		day += 1;
		max_day = GetMonthDayCount(year, month);
		if (day > max_day)
		{
			//�·�Ҫ+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//���Ҫ��1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		day += 1;
		max_day = GetMonthDayCount(year, month);
		if (day > max_day)
		{
			//�·�Ҫ+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//���Ҫ��1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		string new_date = to_string(year) + "-" + to_string(month) + "-" + to_string(day);
		return new_date;
	}
	return "";
}

int SetCarInfo(const char *msg, const char* from_wxid, const char* from_private_wxid, const char* from_name, string s_city)
{
	string send_str_msg;
	string str = string(msg) + "\r\n";
	str = replace_all_distinct(str, " ", "");
	if (strstr(msg, "�����͡�") == NULL) //&& strstr(msg, "�����͡����ҳ�") != NULL || (strcmp(msg, "���ҳ�") != 0 && strstr(msg, "���ҳ�") != NULL)
	{
		str = replace_all_distinct(str, "������", "\r\n������");
		str = replace_all_distinct(str, "���ҳ�", "\r\n���ҳ�");
	}
	str = replace_all_distinct(str, "�����͡�", "\r\n�����͡�");
	/*
	(.|\r|\\nn)*
	(.|\n)*
	\n(.|\n)*
	*/
	char *cp = new char[0];
	std::string pattern("(.|\n)*");
	std::regex r(pattern);
	string new_str;
	string new_str2;
	string iphone = "";
	for (std::sregex_iterator it(str.begin(), str.end(), r), end;     //end��β���������regex_iterator��regex_iterator��string���͵İ汾
		it != end;
		++it)
	{
		if (it->str() == "\n" || it->str() == "" || it->str() == " ")
		{
		}
		else{
			string str = it->str();
			GetIphone(iphone, str);

			new_str2.append(str.c_str());
			new_str2.append("\r\n");

			TrimSpaceReturn(const_cast<char*>(str.c_str()));
			new_str.append(str.c_str());
			new_str.append("<brbr>");
		}
		if (it->str() == "\n")
		{
			time_t t = time(NULL);
			string cs_now_time = time2string(t);
			CTime t1 = timestr((LPSTR)cs_now_time.c_str());
			//int year = t1.GetYear();

			if ((new_str.find("�����͡�������") != string::npos || new_str.find("������") != string::npos) && iphone != "")
			{

				char car_path[MAX_PATH] = { 0 };
				sprintf_s(car_path, sizeof(car_path), mapCarPathInfo[from_wxid].c_str(), s_city.c_str());
				CString szIniPath = car_path + SUFFIX;
				string date = GetDate(new_str);
				if (date != "")
				{
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), NULL, NULL, szIniPath);
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("content"), StringToWString(new_str.c_str()).c_str(), szIniPath);
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("ʱ��"), StringToWString(date.c_str()).c_str(), szIniPath);
				}
				//::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("����"), StringToWString(new_str2.c_str()).c_str(), szIniPath);
				else
				{
					string send_str_msg;
					send_str_msg.append("\n���ڸ�ʽ���ԣ���¼ʧ�ܣ����ڸ�ʽΪ");
					send_str_msg.append("\n��ʱ�䡿XX��XX\n");
					send_str_msg.append("-------------��ܰ��ʾ-------------\n����� ");
					send_str_msg.append(to_string(t1.GetYear() + 1).c_str());
					send_str_msg.append("����밴�����¸�ʽ\n");
					send_str_msg.append("��ʱ�䡿XXXX��XX��XX");
					//sendMsg(from_wxid, send_str_msg.c_str());
					sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
					return EVENT_BLOCK;
				}
				iphone = "";
				if (!::PathFileExists(szIniPath))
				{
					return EVENT_BLOCK;
				}
			}
			else if (((new_str.find("����") != string::npos && new_str.find("�����͡����ҳ�") != string::npos) ||
				(new_str.find("���ҳ�") != string::npos && new_str.find("����") == string::npos)) && iphone != "")
			{
				//���ҳ�				
				char people_path[MAX_PATH] = { 0 };
				sprintf_s(people_path, sizeof(people_path), mapPeoplePathInfo[from_wxid].c_str(), s_city.c_str());
				CString szIniPath = people_path + SUFFIX;
				string date = GetDate(new_str);
				if (date != "")
				{
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), NULL, NULL, szIniPath);
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("content"), StringToWString(new_str.c_str()).c_str(), szIniPath);
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("ʱ��"), StringToWString(date.c_str()).c_str(), szIniPath);
				}
				else
				{
					string send_str_msg;
					send_str_msg.append("\n���ڸ�ʽ���ԣ���¼ʧ�ܣ����ڸ�ʽΪ");
					send_str_msg.append("\n��ʱ�䡿XX��XX\n");
					send_str_msg.append("-------------��ܰ��ʾ-------------\n����� ");
					send_str_msg.append(to_string(t1.GetYear() + 1).c_str());
					send_str_msg.append("����밴�����¸�ʽ\n");
					send_str_msg.append("��ʱ�䡿XXXX��XX��XX");
					sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
					return EVENT_BLOCK;
				}
				iphone = "";
				if (!::PathFileExists(szIniPath))
				{
					return EVENT_BLOCK;
				}
			}
			else
			{
				//send_str_msg = "";
				//send_str_msg.append("���� �����˻��� ���ҳ�\n");
				//sendMsg(fromGroup, send_str_msg.c_str());
				//sendMsg(fromGroup, new_str.c_str());
			}
			//sendMsg(fromGroup, new_str.c_str());
			new_str = "";
			new_str2 = "";
		}
	}
	return EVENT_BLOCK;;
}


int GetCarInfo(string str_msg, const char* from_wxid, const char* from_private_wxid, const char* from_name, string s_city)
{
	CString szIniPath;
	CString expiredPath;
	string file_name = "";
	if (IsFindCar(str_msg))
	{
		file_name = "������.ini";
		char path[MAX_PATH] = { 0 };
		sprintf_s(path, sizeof(path), mapCarPathInfo[from_wxid].c_str(), s_city.c_str());
		szIniPath = path + SUFFIX;

		char ex_path[MAX_PATH] = { 0 };
		sprintf_s(ex_path, sizeof(ex_path), mapCarExPathInfo[from_wxid].c_str(), s_city.c_str());
		expiredPath = ex_path + SUFFIX;
	}
	else if (IsFindPeople(str_msg))
	{
		file_name = "���ҳ�.ini";
		char people_path[MAX_PATH] = { 0 };
		sprintf_s(people_path, sizeof(people_path), mapPeoplePathInfo[from_wxid].c_str(), s_city.c_str());
		szIniPath = people_path + SUFFIX;

		char ex_path[MAX_PATH] = { 0 };
		sprintf_s(ex_path, sizeof(ex_path), mapPeopleExPathInfo[from_wxid].c_str(), s_city.c_str());
		expiredPath = ex_path + SUFFIX;
	}
	if (!::PathFileExists(szIniPath))
	{
		string send_str_msg;
		send_str_msg.append("\n��ʱû�鵽���ݣ�����������ѯ�ɣ�");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
		return EVENT_BLOCK;
	}
	TCHAR strAppNameTemp[4096];
	DWORD dwAppNameSize = GetPrivateProfileString(NULL, NULL, NULL, strAppNameTemp, 4096, szIniPath);
	bool is_time_error = false;
	string date = "";
	if (str_msg != "����" && str_msg != "�ҳ�")
	{
		GetDateEx(str_msg, "[0-9]{4}\u5e74[0-9]{1,2}\u6708[0-9]{1,2}");
		if (date != "")
		{
			//ָ��x��x��x�յĲ���
			date = replace_all_distinct(date, "��", "-");
			date = replace_all_distinct(date, "��", "-");
			//date = replace_all_distinct(date, "��", "");
		}
		else
		{
			time_t t = time(NULL);
			string cs_now_time = time2string(t);
			CTime t1 = timestr((LPSTR)cs_now_time.c_str());
			date = GetDateEx(str_msg, "[0-9]{1,2}\u6708[0-9]{1,2}");
			if (date != "")
			{
				//ָ��x��x�ղ���
				int year = t1.GetYear();
				date = to_string(year) + "-" + date;
				//date = replace_all_distinct(date, "��", "-");
				date = replace_all_distinct(date, "��", "-");
				//date = replace_all_distinct(date, "��", "");
			}
			else
			{
				if (str_msg != "�ҳ�" || str_msg != "����")
				{
					//�������������ߴ�������ڣ���Ĭ���ǽ���
					is_time_error = true;
					date = to_string(t1.GetYear()) + "-" + to_string(t1.GetMonth()) + "-" + to_string(t1.GetDay());
				}
			}
		}
	}
	bool isFirst = true;
	int success_num = 0;
	int fail_num = 0;
	TCHAR* pPos = strAppNameTemp;
	int need_coin = -1;
	for (size_t i = 0; i < dwAppNameSize; ++i)
	{
		if (i == 0 && strAppNameTemp[i] == '1' || (strAppNameTemp[i] == 0 && strAppNameTemp[i + 1] == '1'))
		{
			if (strAppNameTemp[i] != '1')
				pPos = strAppNameTemp + i + 1;
			bool is_real = true;
			if (date != "")
			{
				is_real = false;
				TCHAR tchar_date[50];
				DWORD num = GetPrivateProfileString(pPos, TEXT("ʱ��"), NULL, tchar_date, MAX_PATH, szIniPath);
				if (num != 0)
				{
					if (date == TCHAR2STRING(tchar_date))
					{
						//ͬ����
						is_real = true;
					}
					else
					{
						is_real = false;
					}
				}
			}
			if (is_real)
			{
				if (isFirst)
				{
					int coin = AddCoin(from_wxid, from_private_wxid, need_coin, s_city);
					//CQ_addLog(ac, 1, "coin", to_string(coin).c_str());
					if (coin >= 0)
					{
					}
					else if (coin == -2)
					{
						SendRecharge(from_wxid, from_private_wxid, need_coin);
						return true;
					}
					else
					{
						//δ֪����,�����ļ�����ʧ��
						return EVENT_BLOCK;
					}
					isFirst = false;
				}
				string cur_date = "";
				if (date == "")
				{
					time_t t = time(NULL);
					string cs_now_time = time2string(t);
					CTime t1 = timestr((LPSTR)cs_now_time.c_str());
					cur_date = to_string(t1.GetYear()) + "-" + to_string(t1.GetMonth()) + "-" + to_string(t1.GetDay());

					TCHAR szTime[50];
					DWORD num = GetPrivateProfileString(pPos, TEXT("ʱ��"), NULL, szTime, MAX_PATH, szIniPath);
					if (num != 0)
					{
						//CTime t1 = timestr((LPSTR)cs_now_time.c_str());
						CTime t2 = NULL;
						string s = TCHAR2STRING(szTime).c_str();
						s = s + " " + to_string(t1.GetHour()) + ":" + to_string(t1.GetMinute()) + ":" + to_string(t1.GetSecond());
						t2 = timestr((LPSTR)s.c_str());

						if (t1 > t2)
						{
							//���ڵ���Ϣ


							TCHAR pass_info[4096];
							DWORD pass_num = GetPrivateProfileString(pPos, TEXT("content"), NULL, pass_info, 4096, szIniPath);
							if (pass_num != 0)
							{
								::WritePrivateProfileString(pPos, TEXT("content"), pass_info, expiredPath);
								TCHAR pass_date[100];
								DWORD pass_date_num = GetPrivateProfileString(pPos, TEXT("ʱ��"), NULL, pass_date, MAX_PATH, szIniPath);
								if (pass_date_num != 0)
								{
									::WritePrivateProfileString(pPos, TEXT("ʱ��"), pass_date, expiredPath);
								}
								//TCHAR pass_body[8192];
								//CString pass_body;
								//DWORD pass_body_num = GetPrivateProfileString(pPos, TEXT("����"), NULL, pass_body.GetBuffer(8192), 8192, szIniPath);
								//pass_body.ReleaseBuffer();
								//pass_body.Replace(_T("\\r\\n"), _T("\r\n"));
								//if (pass_body_num != 0)
								//{
								//	::WritePrivateProfileString(pPos, TEXT("����"), pass_body, expiredPath);
								//}
								//sendMsg(fromGroup, to_string(pass_body_num).c_str());
							}
							//ԭ����������յ�
							::WritePrivateProfileString(pPos, NULL, NULL, szIniPath);
							continue;
						}
					}
				}

				TCHAR info[4096];
				DWORD num = GetPrivateProfileString(pPos, TEXT("content"), NULL, info, 4096, szIniPath);
				if (num != 0)
				{
					string str = replace_all_distinct(TCHAR2STRING(info), "<brbr>", "\r\n");
					string send_str_msg;
					send_str_msg.append("\n");
					send_str_msg.append(str);
					sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
					success_num += 1;
				}
				else
				{
					string send_str_msg;
					send_str_msg.append("\n");
					send_str_msg.append(to_string(num));
					send_str_msg.append("\n��ѯʧ��!");
					sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
					fail_num += 1;
				}
			}
			pPos = strAppNameTemp + i + 1;
		}
	}
	if (success_num > 0 || fail_num > 0)
	{
		string send_str_msg;
		send_str_msg.append("\n����");
		send_str_msg.append(to_string(need_coin));
		send_str_msg.append("\n�ɹ���ѯ��");
		send_str_msg.append(to_string(success_num));
		send_str_msg.append("�����ݣ�");
		if (fail_num > 0)
		{
			send_str_msg.append("ʧ��");
			send_str_msg.append(to_string(fail_num));
			send_str_msg.append("��...");
		}
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	else
	{
		string send_str_msg;
		if (date != "")
		{
			send_str_msg.append("\n��Ŷ��û���ҵ���һ������ݣ����������ѯ�������ұ�����ڰɣ�");
		}
		else
		{
			send_str_msg.append("\nSorry�����Ѿ���Ŭ����ѯ�ˣ����ջ��ǲ�ѯʧ�ܣ�");
		}
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	if (is_time_error)
	{
		string send_str_msg;
		send_str_msg.append("\n���δ�ܾ�׼��ѯ�ƶ����ڣ���������ָ���ʽ���ҳ�+X��X�ţ�����+X��X��\n");
		send_str_msg.append("���磺�ҳ�10��1��");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());

	}
	return EVENT_BLOCK;
}

std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;

	str += pattern;//��չ�ַ����Է������
	std::string::size_type size = str.size();

	for (std::string::size_type i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
string IsMonitor(const char* fromGroup)
{
	/*if (vecstr_monitor_qqs.size() == 0)
	{*/
	//int64_t myqq = CQ_getLoginQQ(ac);
	//CString cs_myqq;
	//cs_myqq.Format(_T("%d"), myqq);
	CString cs_qq(fromGroup);
	//LPCWSTR lpcw_qq = stringToLPCWSTR(to_string(fromGroup).c_str());
	TCHAR tchar_qq[20];
	//char qq[20];
	CString path = GetPath("���ñ�.ini");
	DWORD num = ::GetPrivateProfileString(cs_qq, TEXT("����"), NULL, tchar_qq, 20, path);
	if (num != 0)
	{

		TCHAR tchar_city[20];
		DWORD city_num = ::GetPrivateProfileString(cs_qq, _T("����"), NULL, tchar_city, 20, path);
		if (city_num != 0)
		{
			return TCHAR2STRING(tchar_city);

		}
		return "����";
		//���ҵ�
		//string str_qq = TCHAR2STRING(tchar_qq);
		//vecstr_monitor_qqs = split(str_qq, "+");
		//CQ_sendGroupMsg(ac, fromGroup, "IsMonitor���ҵ�");
	}
	/*}
	if (vecstr_monitor_qqs.size() != 0)
	{
	vector<string>::iterator it = find(vecstr_monitor_qqs.begin(), vecstr_monitor_qqs.end(), to_string(fromGroup).c_str());
	if (it != vecstr_monitor_qqs.end())
	{
	CQ_sendGroupMsg(ac, fromGroup, "IsMonitor�ڼ����б�");
	return true;
	}
	}*/
	return "";
}

string IsManage(char const* fromGroup)
{
	/*if (vecstr_manager_qqs.size() == 0)
	{*/
	//int64_t myqq = CQ_getLoginQQ(ac);
	//LPCWSTR lpcw_qq = stringToLPCWSTR(to_string(fromGroup).c_str());
	CString cs_qq(fromGroup);
	TCHAR tchar_qq[20];
	//char qq[20];
	CString path = GetPath("���ñ�.ini");
	DWORD num = ::GetPrivateProfileString(cs_qq, _T("����"), NULL, tchar_qq, 20, path);
	if (num != 0)
	{
		TCHAR tchar_city[20];
		DWORD city_num = ::GetPrivateProfileString(cs_qq, _T("����"), NULL, tchar_city, 20, path);
		if (city_num != 0)
		{
			return TCHAR2STRING(tchar_city);
		}
		return "����";
		////���ҵ�
		//string str_qq = TCHAR2STRING(tchar_qq);
		//vecstr_manager_qqs = split(str_qq, "+");
		//CQ_sendGroupMsg(ac, fromGroup, "IsManage���ҵ�");
	}
	//}
	//CQ_sendGroupMsg(ac, fromGroup, "IsManage1111111111");
	//if (vecstr_manager_qqs.size() != 0)
	//{
	//	CQ_sendGroupMsg(ac, fromGroup, "IsManage222222222222");
	//	vector<string>::iterator it = find(vecstr_manager_qqs.begin(), vecstr_manager_qqs.end(), to_string(fromGroup).c_str());
	//	if (it != vecstr_manager_qqs.end())
	//	{
	//		CQ_sendGroupMsg(ac, fromGroup, "IsManage�ڼ����б�");
	//		return true;
	//	}
	//}
	//CQ_sendGroupMsg(ac, fromGroup, "IsManage3333333333");
	return "";
}


//�˵����ò����Ϣ
CQEVENT(char*, LoadingInfo, 4)(const char* lApiString)
{
	//
	string msg = string(lApiString);
	//MessageBox(NULL, StringToWString(msg.c_str()).c_str(), TEXT("LoadingInfo"), NULL);
	//MessageBox(NULL, TEXT(lApiString), TEXT("����"), NULL);
	char* ss = "ȫ�ܲ��[rn]jensCat[rn]΢��ȫ�ܻ�����[rn]1.0[rn]�˵�1";
	return ss;
}

CQEVENT(const char*, LoadingInit,4)(const char* path)
{
	return 0;
}


//Ⱥ��Ϣ�¼�
/*
from_wxid=��ԴȺid����������Ϣ�ȶ����ʶ��Ⱥid��
to_wxid=�����û�id�����Լ��յ��ˣ������������Լ�
����
����Ϣ��Ⱥ��Աid
Ⱥ��Ա�ǳ�
��ԴȺ�ǳ�
*/
//CQEVENT(int, EventGroupMsg, 12)(string from_wxid, string to_wxid, string msg, string final_from_wxid, string final_from_name, string from_name)
CQEVENT(int, EventGroupMsg, 24)(const char* from_wxid, const char* to_wxid, const char* msg, const char* final_from_wxid, const char* final_from_name, const char* from_name)
{
	InitPath(from_wxid);
	is_monitor = false;
	string s_city = IsManage(from_wxid);
	is_manage = s_city != "";
	if (is_manage == true)
	{
		is_monitor = true;
	}
	else
	{
		s_city = IsMonitor(from_wxid);
		is_monitor = s_city != "";
	}
	if (is_monitor == false)
	{
		//���ڼ���������Χ
		return EVENT_BLOCK;
	}

	char c_sign_path[MAX_PATH] = { 0 };
	sprintf_s(c_sign_path, sizeof(c_sign_path), mapSignPathInfo[from_wxid].c_str(), s_city.c_str());

	string s_sign_path = c_sign_path;
	createDirectory(s_sign_path);
	if (strcmp(msg, "ǩ��") == 0)
	{
		if (is_manage)
		{
			return GroupSignIn(from_wxid, final_from_wxid, final_from_name, s_city);
		}
		return EVENT_BLOCK;
	}
	else if (strstr(msg, "�����͡�������") != NULL || (strcmp(msg, "������") != 0 && strstr(msg, "������") != NULL) || strstr(msg, "�����͡����ҳ�") != NULL || (strcmp(msg, "���ҳ�") != 0 && strstr(msg, "���ҳ�") != NULL))
	{
		return SetCarInfo(msg, from_wxid, final_from_wxid, final_from_name, s_city);
	}
	else
	{
		if (is_manage)
		{
			string str_msg = string(msg);
			int index = 0;
			if (!string(msg).empty())
			{
				while ((index = str_msg.find(' ', index)) != string::npos)
				{
					str_msg.erase(index, 1);
				}
			}
			if (str_msg.find("���") != string::npos)
			{
				return ChooseASong(str_msg, from_wxid, final_from_wxid, final_from_name, s_city);
			}
			else if (IsFindPeople(str_msg) || IsFindCar(str_msg))// strcmp(, "���ҳ�") == 0 || strcmp(str_msg, "������") == 0)
			{
				//

				string date = GetDateEx(str_msg, "^����+[0-9]{1,4}");
				if (date != "" || GetDateEx(str_msg, "^�ҳ�+[0-9]{1,4}") != "" || str_msg == "����" || str_msg == "�ҳ�")
				{
					return GetCarInfo(str_msg, from_wxid, final_from_wxid, final_from_name, s_city);
				}
			}
			else if (str_msg.find("��ѯ���") != string::npos)
			{
				int coin = AddCoin(from_wxid, final_from_wxid, 0, s_city);
				string send_str_msg;
				send_str_msg.append("\n���Ļ�����");
				send_str_msg.append(to_string(coin));
				sendMsgAt(from_wxid, final_from_wxid, final_from_name, send_str_msg.c_str());
			}
		}
	}

	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

//˽����Ϣ�¼�
/*
from_wxid=��Դid
to_wxid=�����û�id
����
��Դ�û����ǳ�
*/
CQEVENT(int,EventFriendsMsg,16)(const char* from_wxid, const char* to_wxid, const char* msg, const char* name)
{

	return 0;
}


//�յ�ת���¼�
/*
from_wxid=����ת����Ϣ��id
to_wxid=���Ͷ����id��Ҳ�����Լ���������Լ��������˵�ת�ˣ�������Ǳ���
{"type":"1","pay_id":"1000000001555456","from_pay_id":"10000000015554423422","remark":"Ԥ���","arrive_type":"0","money":"1.00"} //arrive_type ˵�� 0/�������ˡ�2/�ӳٵ���
֧����
���ͷ��ǳ�
*/
CQEVENT(int, EventCashMoney, 20)(const char* from_wxid, const char* to_wxid, const char* msg, const char* pay_id, const char* name)
{

	return 0;
}

//������տ�
/*
from_wxid=����ת����Ϣ��id
to_wxid=���Ͷ����id��Ҳ�����Լ���������Լ��������˵�ת�ˣ�������Ǳ���
receive_typeΪ0ʱ {"pay_wxid":"wxidsdfkdshdgdg","pay_name":"С����ѽ"}��receive_typeΪ1ʱ {"money":"0.30","total_money":"9.00"}
0������ɨ���У�1��֧�������
*/
CQEVENT(int, EventScanCashMoney, 16)(const char* from_wxid, const char* to_wxid, const char* msg, const char* receive_type)
{

	return 0;
}

//���������¼�
/*
from_wxid=��Դid
to_wxid=����id
���ӵ���Ϣ
�Ӻ��ѱ�ʶ1
�Ӻ��ѱ�ʶ2


--[[
.�汾 2

isParseSucc �� json.���� (msg)

.����� (isParseSucc)
name �� json.ȡͨ������ (��from_name��)  ' �ǳ�
type �� json.ȡͨ������ (��type��)  ' ��Դ
headImg �� json.ȡͨ������ (��head_img��)  ' ͷ���ַ
content �� json.ȡͨ������ (��from_content��)  ' ��Դ��Ϣ�����к���
fina_from_wxid �� json.ȡͨ������ (��from_wxid��)  ' ��Դ��ʶid

--]]
*/
CQEVENT(int, EventFriendRQ, 12)(const char* from_wxid, const char* to_wxid, const char* msg)
{

	return 0;
}

//Ⱥ��Ա�����¼�
/*
from_wxid=��Դid��������Ⱥ��id
to_wxid=����id�����յ���Ⱥ��Ϣ���������Լ�
{"inviter":"����˵�꣨�����������ǳƣ�","inviter_wxid":"wxid_gdsdsgdhfd�������������wxid��","inviter_cid":"sdfdsf������������˱�ʶid��","guest":"Stately������������ǳƣ�","guest_wxid":"wxid_gsdsdssd�����������wxid��","guest_cid":"SGDKHdsgdds����������˱�ʶid��"}
*/
CQEVENT(int, EventAddGroupMember, 12)(const char* from_wxid, const char* to_wxid, const char* msg)
{

	return 0;
}

//ϵͳ��Ϣ�¼� At 1.1.0
/*
from_wxid=��Դid�����͵��˵�id
to_wxid=����id
msg={"type", "1"} 1 �Ѳ��Ǻ��ѹ�ϵ / 2 �ѱ��Է�����

isParseSucc �� json.���� (msg)
.����� (isParseSucc)
type �� json.ȡͨ������ (��type��)  ' ����
refuse_wxid �� from_wxid  ' ɾ������˵�id����������������˵ı�ʶid


*/
CQEVENT(int, EventSysMsg, 12)(const char* from_wxid, const char* to_wxid, const char* msg)
{

	return 0;
}

//�򿪲���˵�
CQEVENT(void, Menu, 0)()
{
}