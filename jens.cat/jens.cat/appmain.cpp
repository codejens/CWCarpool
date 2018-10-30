// jens.cat.cpp : 定义 DLL 应用程序的导出函数。
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
int ac = -1; //AuthCode 调用酷Q的方法时需要用到
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
// 从左到右依次判断文件夹是否存在,不存在就创建
// example: /home/root/mkdir/1/2/3/4/
// 注意:最后一个如果是文件夹的话,需要加上 '\' 或者 '/'
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
			CString path = GetPath("配置表.ini");
			GetPrivateProfileString(TEXT("通用"), TEXT("签到"), NULL, SIGN_PATH, 100, path);
			GetPrivateProfileString(TEXT("通用"), TEXT("车找人"), NULL, CAR_FIND_PEOPLE_PATH, 100, path);
			GetPrivateProfileString(TEXT("通用"), TEXT("人找车"), NULL, PEOPLE_FIND_CAR_PATH, 100, path);
			GetPrivateProfileString(TEXT("通用"), TEXT("车找人过期"), NULL, EXPIRED_CAR_FIND_PEOPLE_PATH, 100, path);
			GetPrivateProfileString(TEXT("通用"), TEXT("人找车过期"), NULL, EXPIRED_PEOPLE_FIND_CAR_PATH, 100, path);
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
	if (str_msg.find("找车") != string::npos)
		return true;
	else
		return false;
}
bool IsFindPeople(string str_msg)
{
	if (str_msg.find("找人") != string::npos)
		return true;
	else
		return false;
}
bool IsFindMusic(string str_msg)
{
	if (str_msg.find("点歌") != string::npos)
		return true;
	else
		return false;
}
void SendRecharge(const char* from_wxid, const char* final_from_wxid, int need_coin)
{
	string send_str_msg;
	send_str_msg.append(final_from_wxid);
	send_str_msg.append("积分不足，需要");
	send_str_msg.append(to_string(abs(need_coin)));
	send_str_msg.append("积分，请保持每天签到或者充值");
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
				return -1; //重复签到
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
			return -3; //文件保存失败
		}
		return coin;
	}
	else
	{
		return -2;	//积分不足
	}
}

int GroupSignIn(const char* from_wxid, const char* from_private_wxid, const char* from_name, string s_city)
{
	int coin = AddCoin(from_wxid, from_private_wxid, sgin_add_coin, s_city);
	if (coin > 0)
	{
		string send_str_msg;
		send_str_msg.append("\n签到成功\n积分增加");
		send_str_msg.append(to_string(sgin_add_coin).c_str());
		send_str_msg.append("个!\n积分余额");
		send_str_msg.append(to_string(coin).c_str());
		send_str_msg.append("个!");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	else if (coin == -1)
	{
		string send_str_msg;
		send_str_msg.append("\n签到失败\n今天已经签过到了哦！");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	else if (coin == -2)
	{
		string send_str_msg;
		send_str_msg.append("\n签到失败\n你的积分是");
		send_str_msg.append(to_string(coin));
		send_str_msg.append("\n请联系主人");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	return EVENT_BLOCK;
}
//判断字符是否是中文
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
			printf("方法调用错误，第一个参数应该大于等于0且小于第二个参数！\n");
		}
		for (int i = start; i <= end; i++)
		{
			tmp += dump[i - 1];
		}
		return tmp;
	}
	else
	{
		printf("请输入字符串！\n");
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
	for (std::sregex_iterator iphone_it(str.begin(), str.end(), iphone_r), end;     //end是尾后迭代器，regex_iterator是regex_iterator的string类型的版本
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
	if (str_msg != "点歌")
	{
		//这里才是真的点歌
		int need_coin = -1;
		int coin = AddCoin(from_wxid, from_private_wxid, -1, s_city);
		if (coin >= 0)
		{
			string send_str_msg;
			send_str_msg.append("\n点歌成功，积分");
			send_str_msg.append(to_string(need_coin));
			send_str_msg.append("，正在搜索....\n如果未有结果，请检查输入指令是否正确：点歌+歌名，比如：\n点歌海阔天空");
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
			//未知错误,可能文件保存失败
			return EVENT_BLOCK;
		}
	}
	else
	{
		string send_str_msg;
		send_str_msg.append("\n发送点歌+歌名，比如：\n");
		send_str_msg.append("点歌海阔天空");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
		return EVENT_IGNORE;
	}
}
/*
*去掉字符串首尾的 \x20 \r \n 字符
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
	//现在来到了字符串的尾部 反向向前
	--p;
	++start;
	if (*start == 0)
	{
		//已经到字符串的末尾了
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
	for (std::sregex_iterator it(str.begin(), str.end(), date_r), end;     //end是尾后迭代器，regex_iterator是regex_iterator的string类型的版本
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
	//std::string date_pattern("[0-9]{1,2}月[0-9]{1,2}号");
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
		string new_date = replace_all_distinct(date, "月", "-");
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
					//不是月份有问题，那就是x号有问题
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
		date = replace_all_distinct(date, "号", "");
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
		date = replace_all_distinct(date, "日", "");
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
	date = GetDateEx(str, "今天");
	if (date != "")
	{
		time_t t = time(NULL);
		string cs_now_time = time2string(t);
		CTime t1 = timestr((LPSTR)cs_now_time.c_str());
		int year = t1.GetYear();
		string new_date = to_string(year) + "-" + to_string(t1.GetMonth()) + "-" + to_string(t1.GetDay());
		return new_date;
	}
	date = GetDateEx(str, "现在");
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
	date = GetDateEx(str, "明天");
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
			//月份要+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//年份要加1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		string new_date = to_string(year) + "-" + to_string(month) + "-" + to_string(day);
		return new_date;
	}
	date = GetDateEx(str, "后天");
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
			//月份要+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//年份要加1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		day += 1;
		max_day = GetMonthDayCount(year, month);
		if (day > max_day)
		{
			//月份要+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//年份要加1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		string new_date = to_string(year) + "-" + to_string(month) + "-" + to_string(day);
		return new_date;
	}
	date = GetDateEx(str, "大后天");
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
			//月份要+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//年份要加1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		day += 1;
		max_day = GetMonthDayCount(year, month);
		if (day > max_day)
		{
			//月份要+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//年份要加1
				year += 1;
				month = 1;
				day = 1;
			}
		}
		day += 1;
		max_day = GetMonthDayCount(year, month);
		if (day > max_day)
		{
			//月份要+1
			month += 1;
			day = 1;
			if (month > 12)
			{
				//年份要加1
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
	if (strstr(msg, "【类型】") == NULL) //&& strstr(msg, "【类型】人找车") != NULL || (strcmp(msg, "人找车") != 0 && strstr(msg, "人找车") != NULL)
	{
		str = replace_all_distinct(str, "车找人", "\r\n车找人");
		str = replace_all_distinct(str, "人找车", "\r\n人找车");
	}
	str = replace_all_distinct(str, "【类型】", "\r\n【类型】");
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
	for (std::sregex_iterator it(str.begin(), str.end(), r), end;     //end是尾后迭代器，regex_iterator是regex_iterator的string类型的版本
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

			if ((new_str.find("【类型】车找人") != string::npos || new_str.find("车找人") != string::npos) && iphone != "")
			{

				char car_path[MAX_PATH] = { 0 };
				sprintf_s(car_path, sizeof(car_path), mapCarPathInfo[from_wxid].c_str(), s_city.c_str());
				CString szIniPath = car_path + SUFFIX;
				string date = GetDate(new_str);
				if (date != "")
				{
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), NULL, NULL, szIniPath);
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("content"), StringToWString(new_str.c_str()).c_str(), szIniPath);
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("时间"), StringToWString(date.c_str()).c_str(), szIniPath);
				}
				//::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("内容"), StringToWString(new_str2.c_str()).c_str(), szIniPath);
				else
				{
					string send_str_msg;
					send_str_msg.append("\n日期格式不对，收录失败！日期格式为");
					send_str_msg.append("\n【时间】XX月XX\n");
					send_str_msg.append("-------------温馨提示-------------\n如果是 ");
					send_str_msg.append(to_string(t1.GetYear() + 1).c_str());
					send_str_msg.append("年的请按照以下格式\n");
					send_str_msg.append("【时间】XXXX年XX月XX");
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
			else if (((new_str.find("类型") != string::npos && new_str.find("【类型】人找车") != string::npos) ||
				(new_str.find("人找车") != string::npos && new_str.find("类型") == string::npos)) && iphone != "")
			{
				//人找车				
				char people_path[MAX_PATH] = { 0 };
				sprintf_s(people_path, sizeof(people_path), mapPeoplePathInfo[from_wxid].c_str(), s_city.c_str());
				CString szIniPath = people_path + SUFFIX;
				string date = GetDate(new_str);
				if (date != "")
				{
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), NULL, NULL, szIniPath);
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("content"), StringToWString(new_str.c_str()).c_str(), szIniPath);
					::WritePrivateProfileString(StringToWString(iphone.c_str()).c_str(), TEXT("时间"), StringToWString(date.c_str()).c_str(), szIniPath);
				}
				else
				{
					string send_str_msg;
					send_str_msg.append("\n日期格式不对，收录失败！日期格式为");
					send_str_msg.append("\n【时间】XX月XX\n");
					send_str_msg.append("-------------温馨提示-------------\n如果是 ");
					send_str_msg.append(to_string(t1.GetYear() + 1).c_str());
					send_str_msg.append("年的请按照以下格式\n");
					send_str_msg.append("【时间】XXXX年XX月XX");
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
				//send_str_msg.append("不是 车找人或者 人找车\n");
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
		file_name = "车找人.ini";
		char path[MAX_PATH] = { 0 };
		sprintf_s(path, sizeof(path), mapCarPathInfo[from_wxid].c_str(), s_city.c_str());
		szIniPath = path + SUFFIX;

		char ex_path[MAX_PATH] = { 0 };
		sprintf_s(ex_path, sizeof(ex_path), mapCarExPathInfo[from_wxid].c_str(), s_city.c_str());
		expiredPath = ex_path + SUFFIX;
	}
	else if (IsFindPeople(str_msg))
	{
		file_name = "人找车.ini";
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
		send_str_msg.append("\n暂时没查到数据，待会再来查询吧！");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
		return EVENT_BLOCK;
	}
	TCHAR strAppNameTemp[4096];
	DWORD dwAppNameSize = GetPrivateProfileString(NULL, NULL, NULL, strAppNameTemp, 4096, szIniPath);
	bool is_time_error = false;
	string date = "";
	if (str_msg != "找人" && str_msg != "找车")
	{
		GetDateEx(str_msg, "[0-9]{4}\u5e74[0-9]{1,2}\u6708[0-9]{1,2}");
		if (date != "")
		{
			//指定x年x月x日的查找
			date = replace_all_distinct(date, "年", "-");
			date = replace_all_distinct(date, "月", "-");
			//date = replace_all_distinct(date, "号", "");
		}
		else
		{
			time_t t = time(NULL);
			string cs_now_time = time2string(t);
			CTime t1 = timestr((LPSTR)cs_now_time.c_str());
			date = GetDateEx(str_msg, "[0-9]{1,2}\u6708[0-9]{1,2}");
			if (date != "")
			{
				//指定x月x日查找
				int year = t1.GetYear();
				date = to_string(year) + "-" + date;
				//date = replace_all_distinct(date, "年", "-");
				date = replace_all_distinct(date, "月", "-");
				//date = replace_all_distinct(date, "号", "");
			}
			else
			{
				if (str_msg != "找车" || str_msg != "找人")
				{
					//有输入其它或者错误的日期，就默认是今天
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
				DWORD num = GetPrivateProfileString(pPos, TEXT("时间"), NULL, tchar_date, MAX_PATH, szIniPath);
				if (num != 0)
				{
					if (date == TCHAR2STRING(tchar_date))
					{
						//同日期
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
						//未知错误,可能文件保存失败
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
					DWORD num = GetPrivateProfileString(pPos, TEXT("时间"), NULL, szTime, MAX_PATH, szIniPath);
					if (num != 0)
					{
						//CTime t1 = timestr((LPSTR)cs_now_time.c_str());
						CTime t2 = NULL;
						string s = TCHAR2STRING(szTime).c_str();
						s = s + " " + to_string(t1.GetHour()) + ":" + to_string(t1.GetMinute()) + ":" + to_string(t1.GetSecond());
						t2 = timestr((LPSTR)s.c_str());

						if (t1 > t2)
						{
							//过期的信息


							TCHAR pass_info[4096];
							DWORD pass_num = GetPrivateProfileString(pPos, TEXT("content"), NULL, pass_info, 4096, szIniPath);
							if (pass_num != 0)
							{
								::WritePrivateProfileString(pPos, TEXT("content"), pass_info, expiredPath);
								TCHAR pass_date[100];
								DWORD pass_date_num = GetPrivateProfileString(pPos, TEXT("时间"), NULL, pass_date, MAX_PATH, szIniPath);
								if (pass_date_num != 0)
								{
									::WritePrivateProfileString(pPos, TEXT("时间"), pass_date, expiredPath);
								}
								//TCHAR pass_body[8192];
								//CString pass_body;
								//DWORD pass_body_num = GetPrivateProfileString(pPos, TEXT("内容"), NULL, pass_body.GetBuffer(8192), 8192, szIniPath);
								//pass_body.ReleaseBuffer();
								//pass_body.Replace(_T("\\r\\n"), _T("\r\n"));
								//if (pass_body_num != 0)
								//{
								//	::WritePrivateProfileString(pPos, TEXT("内容"), pass_body, expiredPath);
								//}
								//sendMsg(fromGroup, to_string(pass_body_num).c_str());
							}
							//原来的数据清空掉
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
					send_str_msg.append("\n查询失败!");
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
		send_str_msg.append("\n积分");
		send_str_msg.append(to_string(need_coin));
		send_str_msg.append("\n成功查询到");
		send_str_msg.append(to_string(success_num));
		send_str_msg.append("个数据！");
		if (fail_num > 0)
		{
			send_str_msg.append("失败");
			send_str_msg.append(to_string(fail_num));
			send_str_msg.append("个...");
		}
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	else
	{
		string send_str_msg;
		if (date != "")
		{
			send_str_msg.append("\n噢哦！没有找到这一天的数据，晚点再来查询或者找找别的日期吧！");
		}
		else
		{
			send_str_msg.append("\nSorry！我已经很努力查询了，最终还是查询失败！");
		}
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());
	}
	if (is_time_error)
	{
		string send_str_msg;
		send_str_msg.append("\n如果未能精准查询制定日期，请检查输入指令格式：找车+X月X号；找人+X月X号\n");
		send_str_msg.append("比如：找车10月1号");
		sendMsgAt(from_wxid, from_private_wxid, from_name, send_str_msg.c_str());

	}
	return EVENT_BLOCK;
}

std::vector<std::string> split(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;

	str += pattern;//扩展字符串以方便操作
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
	CString path = GetPath("配置表.ini");
	DWORD num = ::GetPrivateProfileString(cs_qq, TEXT("监听"), NULL, tchar_qq, 20, path);
	if (num != 0)
	{

		TCHAR tchar_city[20];
		DWORD city_num = ::GetPrivateProfileString(cs_qq, _T("城市"), NULL, tchar_city, 20, path);
		if (city_num != 0)
		{
			return TCHAR2STRING(tchar_city);

		}
		return "广州";
		//有找到
		//string str_qq = TCHAR2STRING(tchar_qq);
		//vecstr_monitor_qqs = split(str_qq, "+");
		//CQ_sendGroupMsg(ac, fromGroup, "IsMonitor有找到");
	}
	/*}
	if (vecstr_monitor_qqs.size() != 0)
	{
	vector<string>::iterator it = find(vecstr_monitor_qqs.begin(), vecstr_monitor_qqs.end(), to_string(fromGroup).c_str());
	if (it != vecstr_monitor_qqs.end())
	{
	CQ_sendGroupMsg(ac, fromGroup, "IsMonitor在监听列表");
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
	CString path = GetPath("配置表.ini");
	DWORD num = ::GetPrivateProfileString(cs_qq, _T("管理"), NULL, tchar_qq, 20, path);
	if (num != 0)
	{
		TCHAR tchar_city[20];
		DWORD city_num = ::GetPrivateProfileString(cs_qq, _T("城市"), NULL, tchar_city, 20, path);
		if (city_num != 0)
		{
			return TCHAR2STRING(tchar_city);
		}
		return "广州";
		////有找到
		//string str_qq = TCHAR2STRING(tchar_qq);
		//vecstr_manager_qqs = split(str_qq, "+");
		//CQ_sendGroupMsg(ac, fromGroup, "IsManage有找到");
	}
	//}
	//CQ_sendGroupMsg(ac, fromGroup, "IsManage1111111111");
	//if (vecstr_manager_qqs.size() != 0)
	//{
	//	CQ_sendGroupMsg(ac, fromGroup, "IsManage222222222222");
	//	vector<string>::iterator it = find(vecstr_manager_qqs.begin(), vecstr_manager_qqs.end(), to_string(fromGroup).c_str());
	//	if (it != vecstr_manager_qqs.end())
	//	{
	//		CQ_sendGroupMsg(ac, fromGroup, "IsManage在监听列表");
	//		return true;
	//	}
	//}
	//CQ_sendGroupMsg(ac, fromGroup, "IsManage3333333333");
	return "";
}


//菜单设置插件信息
CQEVENT(char*, LoadingInfo, 4)(const char* lApiString)
{
	//
	string msg = string(lApiString);
	//MessageBox(NULL, StringToWString(msg.c_str()).c_str(), TEXT("LoadingInfo"), NULL);
	//MessageBox(NULL, TEXT(lApiString), TEXT("测试"), NULL);
	char* ss = "全能插件[rn]jensCat[rn]微信全能机器人[rn]1.0[rn]菜单1";
	return ss;
}

CQEVENT(const char*, LoadingInit,4)(const char* path)
{
	return 0;
}


//群消息事件
/*
from_wxid=来源群id，用来发消息等对象标识（群id）
to_wxid=对象用户id，你自己收到了，所以这里是自己
内容
发消息的群成员id
群成员昵称
来源群昵称
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
		//不在监听、管理范围
		return EVENT_BLOCK;
	}

	char c_sign_path[MAX_PATH] = { 0 };
	sprintf_s(c_sign_path, sizeof(c_sign_path), mapSignPathInfo[from_wxid].c_str(), s_city.c_str());

	string s_sign_path = c_sign_path;
	createDirectory(s_sign_path);
	if (strcmp(msg, "签到") == 0)
	{
		if (is_manage)
		{
			return GroupSignIn(from_wxid, final_from_wxid, final_from_name, s_city);
		}
		return EVENT_BLOCK;
	}
	else if (strstr(msg, "【类型】车找人") != NULL || (strcmp(msg, "车找人") != 0 && strstr(msg, "车找人") != NULL) || strstr(msg, "【类型】人找车") != NULL || (strcmp(msg, "人找车") != 0 && strstr(msg, "人找车") != NULL))
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
			if (str_msg.find("点歌") != string::npos)
			{
				return ChooseASong(str_msg, from_wxid, final_from_wxid, final_from_name, s_city);
			}
			else if (IsFindPeople(str_msg) || IsFindCar(str_msg))// strcmp(, "我找车") == 0 || strcmp(str_msg, "我找人") == 0)
			{
				//

				string date = GetDateEx(str_msg, "^找人+[0-9]{1,4}");
				if (date != "" || GetDateEx(str_msg, "^找车+[0-9]{1,4}") != "" || str_msg == "找人" || str_msg == "找车")
				{
					return GetCarInfo(str_msg, from_wxid, final_from_wxid, final_from_name, s_city);
				}
			}
			else if (str_msg.find("查询余额") != string::npos)
			{
				int coin = AddCoin(from_wxid, final_from_wxid, 0, s_city);
				string send_str_msg;
				send_str_msg.append("\n您的积分余额：");
				send_str_msg.append(to_string(coin));
				sendMsgAt(from_wxid, final_from_wxid, final_from_name, send_str_msg.c_str());
			}
		}
	}

	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}

//私聊消息事件
/*
from_wxid=来源id
to_wxid=对象用户id
内容
来源用户的昵称
*/
CQEVENT(int,EventFriendsMsg,16)(const char* from_wxid, const char* to_wxid, const char* msg, const char* name)
{

	return 0;
}


//收到转账事件
/*
from_wxid=发送转账消息的id
to_wxid=发送对象的id，也就是自己，如果是自己发给别人的转账，这里就是别人
{"type":"1","pay_id":"1000000001555456","from_pay_id":"10000000015554423422","remark":"预付款阿","arrive_type":"0","money":"1.00"} //arrive_type 说明 0/正常到账、2/延迟到账
支付号
发送方昵称
*/
CQEVENT(int, EventCashMoney, 20)(const char* from_wxid, const char* to_wxid, const char* msg, const char* pay_id, const char* name)
{

	return 0;
}

//面对面收款
/*
from_wxid=发送转账消息的id
to_wxid=发送对象的id，也就是自己，如果是自己发给别人的转账，这里就是别人
receive_type为0时 {"pay_wxid":"wxidsdfkdshdgdg","pay_name":"小粉与呀"}、receive_type为1时 {"money":"0.30","total_money":"9.00"}
0、正在扫码中，1、支付已完成
*/
CQEVENT(int, EventScanCashMoney, 16)(const char* from_wxid, const char* to_wxid, const char* msg, const char* receive_type)
{

	return 0;
}

//好友请求事件
/*
from_wxid=来源id
to_wxid=对象id
附加的消息
加好友标识1
加好友标识2


--[[
.版本 2

isParseSucc ＝ json.解析 (msg)

.如果真 (isParseSucc)
name ＝ json.取通用属性 (“from_name”)  ' 昵称
type ＝ json.取通用属性 (“type”)  ' 来源
headImg ＝ json.取通用属性 (“head_img”)  ' 头像地址
content ＝ json.取通用属性 (“from_content”)  ' 来源消息（打招呼）
fina_from_wxid ＝ json.取通用属性 (“from_wxid”)  ' 来源标识id

--]]
*/
CQEVENT(int, EventFriendRQ, 12)(const char* from_wxid, const char* to_wxid, const char* msg)
{

	return 0;
}

//群成员增加事件
/*
from_wxid=来源id，这里是群的id
to_wxid=对象id，你收到了群消息，这里是自己
{"inviter":"听风说雨（发出邀请人昵称）","inviter_wxid":"wxid_gdsdsgdhfd（发出邀请的人wxid）","inviter_cid":"sdfdsf（发出邀请的人标识id）","guest":"Stately（被邀请的人昵称）","guest_wxid":"wxid_gsdsdssd（被邀请的人wxid）","guest_cid":"SGDKHdsgdds（被邀请的人标识id）"}
*/
CQEVENT(int, EventAddGroupMember, 12)(const char* from_wxid, const char* to_wxid, const char* msg)
{

	return 0;
}

//系统消息事件 At 1.1.0
/*
from_wxid=来源id，发送的人的id
to_wxid=对象id
msg={"type", "1"} 1 已不是好友关系 / 2 已被对方拉黑

isParseSucc ＝ json.解析 (msg)
.如果真 (isParseSucc)
type ＝ json.取通用属性 (“type”)  ' 类型
refuse_wxid ＝ from_wxid  ' 删了你的人的id，或者是拉黑你的人的标识id


*/
CQEVENT(int, EventSysMsg, 12)(const char* from_wxid, const char* to_wxid, const char* msg)
{

	return 0;
}

//打开插件菜单
CQEVENT(void, Menu, 0)()
{
}