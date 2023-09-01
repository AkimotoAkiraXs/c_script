/*
 * ��������ɸѡ���ڣ��˷�������ͨ��������������
 * */

#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

typedef long long ll;

const static ll SECONDS_OF_DAY = 24 * 60 * 60;

static int type; // 0-�����գ�1-������

void setTimeZone();

vector<string> read(const string &in, const char &split);

vector<int> parseInt(const vector<string> &strings);

map<int, map<int, int>> parseCalendar(const string &arg);

int calculateDays(const map<int, map<int, int>> &date, ll start, ll end);

int calculateMonth(const map<int, map<int, int>> &date, ll start, ll end);

int main(int argc, char *argv[]) {
    if (argc < 2) throw runtime_error("need arguments!");

    type = stoi(argv[1]);

    string in;
    while (getline(cin, in)) {
        const vector<string> &inputs = read(in, '\t');
        ll start = stoll(inputs[0]) / 1000, end = stoll(inputs[1]) / 1000;
        if (end < start) throw runtime_error("start time can't greater then end time.");
        const map<int, map<int, int>> &date = parseCalendar(inputs[2]);
        if (type) cout << calculateMonth(date, start, end);
        else cout << calculateDays(date, start, end);
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

vector<string> read(const string &in, const char &split) {
    string str;
    istringstream inStream(in);
    vector<string> res;
    while (getline(inStream, str, split)) res.push_back(str);
    return res;
}

vector<int> parseInt(const vector<string> &strings) {
    vector<int> res;
    for (const string &str: strings) res.push_back(stoi(str));
    return res;
}

map<int, map<int, int>> parseCalendar(const string &arg) {
    auto start = arg.find('[');
    auto end = arg.find(']');
    if (start == string::npos || end == string::npos) throw runtime_error("���ݸ�ʽ�����밴�����ʽ��������");
    auto subStr = arg.substr(start + 1, end - start - 1); // ȥ��[]
    const vector<string> &strings = read(subStr, ',');
    auto data = parseInt(strings);

    map<int, map<int, int>> calender;
    for (int i = 1; i < data.size(); i += 13) {
        int year = data[i];
        map<int, int> yearMap;
        for (int j = 0; j < 12; ++j) yearMap[j] = data[j + i + 1];
        calender[year] = yearMap;
    }
    return calender;
}

int calculateDays(const map<int, map<int, int>> &date, ll start, ll end) {
    // ��ȡ��ʼ������һ���00:00

    tm &run = *localtime(&start);
    run.tm_hour = run.tm_min = run.tm_sec = 0;
    time_t startDay = mktime(&run);

    //��ȡ����������һ���24:00
    tm &endTm = *localtime(&end);
    if (endTm.tm_hour != 0 || endTm.tm_min != 0 || endTm.tm_sec != 0) {
        endTm.tm_mday++;
        endTm.tm_hour = endTm.tm_min = endTm.tm_sec = 0;
    }
    time_t endLastDay = mktime(&endTm);

    int cnt = 0;
    for (time_t i = startDay; i < endLastDay; i += SECONDS_OF_DAY) {
        run = *localtime(&i);
        int year = run.tm_year + 1900;
        int month = run.tm_mon;
        int day = run.tm_mday - 1;
        if (date.find(year) != date.end()) {
            int bit = date.at(year).at(month);
            if ((bit >> day & 1) == 1) cnt++;
        }
    }
    return cnt;
}

int calculateMonth(const map<int, map<int, int>> &date, ll start, ll end) {

    tm &endTm = *localtime(&end);
    int endYear = endTm.tm_year + 1900;
    int endMonth = endTm.tm_mon;
    int endDay = endTm.tm_mday;

    // �ӿ�ʼ����˶���ʱ��
    tm &run = *localtime(&start);
    int startYear = run.tm_year + 1900;
    int startMonth = run.tm_mon;

    int total = 1 + (endYear - startYear) * 12 + endMonth - startMonth;

    int cnt = 0;
    // �����һ�����Ƿ����
    int firstMonthFlag = false;
    if (date.find(startYear) != date.end()) {
        int bit = date.at(startYear).at(startMonth);
        time_t runTime = mktime(&run);
        while (startMonth == run.tm_mon) {
            if (!firstMonthFlag && (bit >> (run.tm_mday -1) & 1) == 1) firstMonthFlag = true;
            runTime += SECONDS_OF_DAY;
            run = *localtime(&runTime);
        }
    }
    cnt += firstMonthFlag;
    for (int i = 1; i < total - 1; ++i) {
        int year = run.tm_year + 1900;
        int month = run.tm_mon;
        if (date.find(year) != date.end() && date.at(year).at(month) != 0) cnt++;
        if (run.tm_mon == 11) {
            run.tm_year++;
            run.tm_mon = 0;
        } else run.tm_mon++;
    }
    // �������һ�����Ƿ����
    if ((startYear != endYear || startMonth != endMonth) && date.find(endYear) != date.end()) {
        int bit = date.at(endYear).at(endMonth);
        for (int i = 0; i <= endDay; ++i) {
            if ((bit >> i & 1) == 1) {
                cnt++;
                break;
            }
        }
    }
    return cnt;
}