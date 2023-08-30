#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

typedef long long ll;

const static char split = ','; // 分隔符
const static int PER_WEEK = 7; // 每周七天
const static int PER_DAY = 5; // 每天五个参数
const static ll SECONDS_OF_DAY = 24 * 60 * 60;

enum CalType {
    DAY,
    MONTH
};

vector<string> read(const string &in);

vector<ll> parseParameter(const string &arg, int *type);

vector<ll> parseLL(const vector<string> &timeStamps, int limit);

int calculateDays(const vector<ll> &weekDays, const ll &startStamp, const ll &endStamp);

int calculateMonths(const vector<ll> &weekDays, const ll &startStamp, const ll &endStamp);

void printTime(const ll &startStamp, const ll &endStamp);

void printInfo(const vector<ll> &weekDays, const ll &startStamp, const ll &endStamp);

int main(int argc, char *argv[]) {
    int type; // 计算类型
    if (argc < 2) throw runtime_error("未传入参数!");
    const vector<ll> &weekDays = parseParameter(argv[1], &type); // 初始化启动参数
    string in;
    while (getline(cin, in)) {
        const vector<string> &inputs = read(in); // 解析输入，获取时间范围
        const vector<ll> &vector = parseLL(inputs, 2); // 时间范围字符串转为时间戳
        ll start = vector[0], end = vector[1];
        printInfo(weekDays, start, end);
        if (type == DAY) {
            cout << calculateDays(weekDays, start, end);
        } else if (type == MONTH) {
            cout << calculateMonths(weekDays, start, end);
        }
    }
    return 0;
}

vector<string> read(const string &in) {
    string str;
    istringstream inStream(in);
    vector<string> res;
    while (getline(inStream, str, split)) res.push_back(str);
    return res;
}

vector<ll> parseParameter(const string &arg, int *type) {
    vector<string> args = read(arg);
    *type = stoi(args.front());
    args.erase(args.begin());
    return parseLL(args, PER_WEEK * PER_DAY);
}

vector<ll> parseLL(const vector<string> &timeStamps, int limit) {
    if (timeStamps.size() != limit) throw runtime_error("参数异常！");
    vector<ll> res;
    for (const string &str: timeStamps) res.push_back(stoll(str) / 1000);
    return res;
}

int calculateDays(const vector<ll> &weekDays, const ll &startStamp, const ll &endStamp) {
    // tm只有一个地址可用，所以二次调用会被刷掉旧值
    struct tm startTm = *localtime(&startStamp);
    startTm.tm_hour = startTm.tm_min = startTm.tm_sec = 0;
    time_t left = mktime(&startTm);
    int dayOfWeek = (startTm.tm_wday + 6) % 7;

    struct tm endTm = *localtime(&endStamp);
    endTm.tm_hour = 24;
    endTm.tm_min = endTm.tm_sec = 0;
    time_t right = mktime(&endTm);

    int cnt = 0;
    for (; left < right; left += SECONDS_OF_DAY) {
        ll st = weekDays[dayOfWeek * PER_DAY + 1];
        ll ed = weekDays[dayOfWeek * PER_DAY + 2];
        dayOfWeek = (dayOfWeek + 1) % 7;
        if (ed == 0) continue;
        time_t start = max(left, startStamp) - left;
        time_t end = min(right, left + SECONDS_OF_DAY) - left;
        if (start <= ed && end > st) cnt++;
    }
    return cnt;
}

int calculateMonths(const vector<ll> &weekDays, const ll &startStamp, const ll &endStamp) {
    struct tm startTm = *localtime(&startStamp);
    struct tm firstMonthEndTm = startTm;
    firstMonthEndTm.tm_mon += 1;
    firstMonthEndTm.tm_mday = 1;
    firstMonthEndTm.tm_sec = firstMonthEndTm.tm_min = firstMonthEndTm.tm_hour = 0;
    time_t firstMonthEndStamp = mktime(&firstMonthEndTm);
    int startYear = startTm.tm_year;
    int startMon = startTm.tm_mon;

    struct tm endTm = *localtime(&endStamp);
    int cnt = (endTm.tm_year - startYear) * 12 + endTm.tm_mon - startMon + 1;
    if (!calculateDays(weekDays, startStamp, min(endStamp, firstMonthEndStamp))) cnt--;

    struct tm lastMonthStartTm = endTm;
    lastMonthStartTm.tm_mday = 1;
    lastMonthStartTm.tm_sec = lastMonthStartTm.tm_min = lastMonthStartTm.tm_hour = 0;
    time_t lastMonthStartTime = mktime(&lastMonthStartTm);
    if (!calculateDays(weekDays, max(startStamp, lastMonthStartTime), endStamp)) cnt--;
    return cnt;
}

void printInfo(const vector<ll> &weekDays, const ll &startStamp, const ll &endStamp) {
    printTime(startStamp, endStamp);

    for (int i = 0; i < 7; ++i) {
        cout << "周" << i + 1 << ":";
        printTime(weekDays[i * 5 + 1], weekDays[i * 5 + 2]);
    }
}

void printTime(const ll &startStamp, const ll &endStamp) {
    char st[80];
    tm &stTm = *localtime(&startStamp);
    strftime(st, sizeof(st), "%Y-%m-%d %H:%M:%S", &stTm);
    char ed[80];
    tm &edTm = *localtime(&endStamp);
    strftime(ed, sizeof(ed), "%Y-%m-%d %H:%M:%S", &edTm);
    cout << string(st) << "~" << string(ed) << endl;
}



