/*
 * �˷���ͨ����������ÿ�ܹ���ʱ�����������һ��ʱ�䷶Χ�ڵĹ�����/�����µ�����
 * */

#include <sstream>
#include <iostream>
#include <vector>
#include "ctime"

using namespace std;

typedef time_t tt;

const static char split = ','; // �ָ���
const static int PER_WEEK = 7; // ÿ������
const static int PER_DAY = 5; // ÿ���������
const static tt SECONDS_OF_DAY = 24 * 60 * 60;
static int type; // 0-�����գ�1-������

void setTimeZone();

vector<string> read(const string &in);

vector<tt> parseParameter(const string &arg);

vector<tt> parseLL(const vector<string> &timeStamps, int limit);

int calculateDays(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp);

int calculateMonths(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp);

void printTime(const tt &startStamp, const tt &endStamp);

void printInfo(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp);

int main(int argc, char *argv[]) {
    if (argc < 2) throw runtime_error("need arguments!");

    const vector<tt> &weekDays = parseParameter(argv[1]); // ��ʼ����������

    string in;
    while (getline(cin, in)) {
        const vector<string> &inputs = read(in); // �������룬��ȡʱ�䷶Χ
        const vector<tt> &vector = parseLL(inputs, 2); // ʱ�䷶Χ�ַ���תΪʱ���
        tt start = vector[0], end = vector[1];
        if (end < start) throw runtime_error("start time can't greater then end time.");
        if (type) cout << calculateMonths(weekDays, start, end);
        else cout << calculateDays(weekDays, start, end);
    }
    return 0;
}

void setTimeZone() {
    const char *timeZone = "Asia/Shanghai";
    char env_var[100];
    snprintf(env_var, sizeof(env_var), "TZ=%s", timeZone);
    putenv(env_var);
    tzset(); // ����ʱ��
}

vector<string> read(const string &in) {
    string str;
    istringstream inStream(in);
    vector<string> res;
    while (getline(inStream, str, split)) res.push_back(str);
    return res;
}

vector<tt> parseParameter(const string &arg) {
    vector<string> args = read(arg);
    type = stoi(args.front());
    args.erase(args.begin());
    return parseLL(args, PER_WEEK * PER_DAY);
}

vector<tt> parseLL(const vector<string> &timeStamps, int limit) {
    if (timeStamps.size() != limit) throw runtime_error("the number of arguments error.");
    vector<tt> res;
    for (const string &str: timeStamps) res.push_back(stoll(str) / 1000);
    return res;
}

int calculateDays(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp) {
    // tmֻ��һ����ַ���ã����Զ��ε��ûᱻˢ����ֵ
    struct tm startTm = *localtime(&startStamp);
    startTm.tm_hour = startTm.tm_min = startTm.tm_sec = 0;
    tt left = mktime(&startTm);
    int dayOfWeek = (startTm.tm_wday + 6) % 7;

    struct tm endTm = *localtime(&endStamp);
    endTm.tm_hour = 24;
    endTm.tm_min = endTm.tm_sec = 0;
    tt right = mktime(&endTm);

    int cnt = 0;
    for (; left < right; left += SECONDS_OF_DAY) {
        tt st = weekDays[dayOfWeek * PER_DAY + 1];
        tt ed = weekDays[dayOfWeek * PER_DAY + 2];
        dayOfWeek = (dayOfWeek + 1) % 7;
        if (ed == 0) continue;
        tt start = max(left, startStamp) - left;
        tt end = min(endStamp, left + SECONDS_OF_DAY) - left;
        if (start <= ed && end > st) cnt++;
    }
    return cnt;
}

int calculateMonths(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp) {
    struct tm startTm = *localtime(&startStamp);
    struct tm firstMonthEndTm = startTm;
    firstMonthEndTm.tm_mon += 1;
    firstMonthEndTm.tm_mday = 1;
    firstMonthEndTm.tm_sec = firstMonthEndTm.tm_min = firstMonthEndTm.tm_hour = 0;
    tt firstMonthEndStamp = mktime(&firstMonthEndTm);
    int startYear = startTm.tm_year;
    int startMon = startTm.tm_mon;

    struct tm endTm = *localtime(&endStamp);
    int cnt = (endTm.tm_year - startYear) * 12 + endTm.tm_mon - startMon + 1;
    if (!calculateDays(weekDays, startStamp, min(endStamp, firstMonthEndStamp))) cnt--;

    struct tm lastMonthStartTm = endTm;
    lastMonthStartTm.tm_mday = 1;
    lastMonthStartTm.tm_sec = lastMonthStartTm.tm_min = lastMonthStartTm.tm_hour = 0;
    tt lastMonthStartTime = mktime(&lastMonthStartTm);
    if (!calculateDays(weekDays, max(startStamp, lastMonthStartTime), endStamp)) cnt--;
    return cnt;
}

void printInfo(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp) {
    printTime(startStamp, endStamp);

    for (int i = 0; i < 7; ++i) {
        cout << "��" << i + 1 << ":";
        printTime(weekDays[i * 5 + 1], weekDays[i * 5 + 2]);
    }
}

void printTime(const tt &startStamp, const tt &endStamp) {
    char st[80];
    tm &stTm = *localtime(&startStamp);
    strftime(st, sizeof(st), "%Y-%m-%d %H:%M:%S", &stTm);
    char ed[80];
    tm &edTm = *localtime(&endStamp);
    strftime(ed, sizeof(ed), "%Y-%m-%d %H:%M:%S", &edTm);
    cout << string(st) << "~" << string(ed) << endl;
}



