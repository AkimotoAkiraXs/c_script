/*
 * 此方法通过参数传入每周工作时间参数，计算一段时间范围内的工作日/工作月的数量
 * */

#include <sstream>
#include <iostream>
#include <vector>
#include "ctime"

using namespace std;

typedef time_t tt;

const static int PER_WEEK = 7; // 每周七天
const static int PER_DAY = 5; // 每天五个参数
const static tt SECONDS_OF_DAY = 24 * 60 * 60;

void setTimeZone();

vector<string> read(const string &in, const char &split);

vector<tt> parseParameter(const string &arg);

vector<tt> parseLL(const vector<string> &timeStamps, int limit);

int calculateDays(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp);

int countDays(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp);

int calculateMonths(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp);

void printTime(const tt &startStamp, const tt &endStamp);

void printInfo(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp);

int main(int argc, char *argv[]) {
    try {
        if (argc < 2) throw runtime_error("need arguments!");

        int type = argv[1][0] - '0'; // 0-计算日，1-计算月
        const vector<tt> &weekDays = parseParameter(argv[1]); // 初始化启动参数

        string in;
        while (getline(cin, in)) {
            const vector<string> &strings = read(in, '\t');
            const vector<tt> &inputs = parseLL(strings, 2);
            tt start = inputs[0], end = inputs[1];
            if (end < start) throw runtime_error("start time can't greater then end time.");
            if (type) cout << calculateMonths(weekDays, start, end) << endl;
            else cout << calculateDays(weekDays, start, end) << endl;
        }
    } catch (runtime_error &error) {
        string error_message = error.what();
        cout << 0 << endl;
    }
    return 0;
}

void setTimeZone() {
    const char *timeZone = "Asia/Shanghai";
    char env_var[100];
    snprintf(env_var, sizeof(env_var), "TZ=%s", timeZone);
    putenv(env_var);
    tzset(); // 更新时区
}

vector<string> read(const string &in, const char &split) {
    string str;
    istringstream inStream(in);
    vector<string> res;
    while (getline(inStream, str, split)) res.push_back(str);
    return res;
}

vector<tt> parseParameter(const string &arg) {
    auto start = arg.find('[');
    auto end = arg.find(']');
    if (start == string::npos || end == string::npos) throw runtime_error("Parameter format error.");
    auto subStr = arg.substr(start + 1, end - start - 1); // 去除[]
    const vector<string> &strings = read(subStr, ',');
    return parseLL(strings, PER_WEEK * PER_DAY);
}

vector<tt> parseLL(const vector<string> &timeStamps, int limit) {
    if (timeStamps.size() != limit) throw runtime_error("the number of arguments error.");
    vector<tt> res;
    for (const string &str: timeStamps) res.push_back(stoll(str) / 1000);
    return res;
}

// 计算自然日函数，当天不会参与计算
int calculateDays(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp) {
    // tm只有一个地址可用，所以二次调用会被刷掉旧值
    // 开始日期推移到下一天的开始，兼容自然日的逻辑
    tt runTimeStamp = startStamp + SECONDS_OF_DAY;
    struct tm startTm = *localtime(&runTimeStamp);
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
        tt start = 0; // 不用考虑第一天，所以固定为0
        tt end = min(endStamp, left + SECONDS_OF_DAY) - left;
        if (start <= ed && end > st) cnt++;
    }
    return cnt;
}

// 单纯计算天的函数，当天会参与计算
int countDays(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp) {
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
    // 跳到下个月开始
    struct tm nextMonthStartTm = startTm;
    if (nextMonthStartTm.tm_mon == 11){
        nextMonthStartTm.tm_year++;
        nextMonthStartTm.tm_mon = 0;
    } else{
        nextMonthStartTm.tm_mon += 1;
    }
    nextMonthStartTm.tm_mday = 1;
    nextMonthStartTm.tm_sec = nextMonthStartTm.tm_min = nextMonthStartTm.tm_hour = 0;
    tt firstMonthEndStamp = mktime(&nextMonthStartTm);
    int startYear = nextMonthStartTm.tm_year;
    int startMon = nextMonthStartTm.tm_mon;

    struct tm endTm = *localtime(&endStamp);
    int cnt = (endTm.tm_year - startYear) * 12 + endTm.tm_mon - startMon + 1;

    struct tm lastMonthStartTm = endTm;
    lastMonthStartTm.tm_mday = 1;
    lastMonthStartTm.tm_sec = lastMonthStartTm.tm_min = lastMonthStartTm.tm_hour = 0;
    tt lastMonthStartTime = mktime(&lastMonthStartTm);
    if (!countDays(weekDays, max(firstMonthEndStamp, lastMonthStartTime), endStamp)) cnt--;
    return max(0, cnt);
}

void printInfo(const vector<tt> &weekDays, const tt &startStamp, const tt &endStamp) {
    printTime(startStamp, endStamp);

    for (int i = 0; i < 7; ++i) {
        cout << "周" << i + 1 << ":";
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