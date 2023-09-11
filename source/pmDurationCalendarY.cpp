/*
 * ��������ɸѡ���ڣ��˷�������ͨ��������������
 * */

#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <ctime>

using namespace std;

typedef time_t tt;

const static tt SECONDS_OF_DAY = 24 * 60 * 60;

void setTimeZone();

vector<string> read(const string &in, const char &split);

vector<int> parseInt(const vector<string> &strings);

vector<tt> parseLL(const vector<string> &strings);

map<int, map<int, int>> parseParameter(const string &arg);

int calculateDays(const map<int, map<int, int>> &date, tt start, tt end);

int calculateMonth(const map<int, map<int, int>> &date, tt start, tt end);

int calculateSum(tm &currentDate); // ����ĳ����ȫ�Ƿǹ����գ�1����ʮ������

int main(int argc, char *argv[]) {
    try {
        if (argc < 2) throw runtime_error("need arguments!");

        int type = argv[1][0] - '0'; // 0-�����գ�1-������
        auto calender = parseParameter(argv[1]);

        string in;
        while (getline(cin, in)) {
            const vector<string> &strings = read(in, '\t');
            const vector<tt> &times = parseLL(strings);
            tt start = times[0], end = times[1];
            if (end < start) throw runtime_error("start time can't greater then end time.");
            if (type) cout << calculateMonth(calender, start, end) << endl;
            else cout << calculateDays(calender, start, end) << endl;
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

vector<tt> parseLL(const vector<string> &strings) {
    vector<tt> res;
    for (const string &str: strings) res.push_back(stoll(str) / 1000);
    return res;
}

map<int, map<int, int>> parseParameter(const string &arg) {
    auto start = arg.find('[');
    auto end = arg.find(']');
    if (start == string::npos || end == string::npos) throw runtime_error("data formatError.");
    auto subStr = arg.substr(start + 1, end - start - 1); // ȥ��[]
    const vector<string> &strings = read(subStr, ',');
    auto data = parseInt(strings);

    map<int, map<int, int>> calendar;
    for (int i = 1; i < data.size(); i += 13) {
        int year = data[i];
        map<int, int> yearMap;
        for (int j = 0; j < 12; ++j) yearMap[j] = data[j + i + 1];
        calendar[year] = yearMap;
    }
    return calendar;
}

int calculateDays(const map<int, map<int, int>> &date, tt start, tt end) {
    // ��ȡ��ʼ������һ���00:00
    tt runTimeStamp = start + SECONDS_OF_DAY;
    tm &run = *localtime(&runTimeStamp);
    run.tm_hour = run.tm_min = run.tm_sec = 0;
    tt startDay = mktime(&run);

    //��ȡ����������һ���24:00
    tm &endTm = *localtime(&end);
    if (endTm.tm_hour != 0 || endTm.tm_min != 0 || endTm.tm_sec != 0) {
        endTm.tm_mday++;
        endTm.tm_hour = endTm.tm_min = endTm.tm_sec = 0;
    }
    tt endLastDay = mktime(&endTm);

    int cnt = 0;
    for (tt i = startDay; i < endLastDay; i += SECONDS_OF_DAY) {
        run = *localtime(&i);
        int year = run.tm_year + 1900;
        int month = run.tm_mon;
        int day = run.tm_mday - 1;
        if (date.find(year) != date.end()) {
            int bit = date.at(year).at(month);
            if ((bit >> day & 1) == 0) cnt++;
        } else {
            cnt++; // û���趨������Ϊ������
        }
    }
    return cnt;
}

int calculateMonth(const map<int, map<int, int>> &date, tt start, tt end) {

    tm &endTm = *localtime(&end);
    int endYear = endTm.tm_year + 1900;
    int endMonth = endTm.tm_mon;
    int endDay = endTm.tm_mday;

    // �ӿ�ʼ����˶���ʱ��
    tm &run = *localtime(&start);
    int startYear = run.tm_year + 1900;
    int startMonth = run.tm_mon;

    int total = (endYear - startYear) * 12 + endMonth - startMonth + 1;

    int cnt = 0;
    // ��һ���º����һ���²�����
    for (int i = 1; i < total - 1; ++i) {
        if (run.tm_mon == 11) {
            run.tm_year++;
            run.tm_mon = 0;
        } else run.tm_mon++;
        int year = run.tm_year + 1900;
        int month = run.tm_mon;
        if (date.find(year) == date.end() || date.at(year).at(month) != calculateSum(run)) cnt++;
    }
    // �������ֻ��һ���µĻ�����Ҫ�������һ�����Ƿ����
    if (total != 1) {
        if (date.find(endYear) != date.end()) {
            int bit = date.at(endYear).at(endMonth);
            for (int i = 0; i < endDay; ++i) {
                if ((bit >> i & 1) == 0) {
                    cnt++;
                    break;
                }
            }
        } else cnt++;
    }
    return cnt;
}

int calculateSum(tm &currentDate) {
    // ��ȡ��ǰ�·�
    int currentMonth = currentDate.tm_mon + 1; // tm_mon�ķ�Χ��0-11��������Ҫ��1

    // ���㵱ǰ�·ݵ�����
    int daysInMonth = 0;

    switch (currentMonth) {
        case 4:
        case 6:
        case 9:
        case 11:
            daysInMonth = 30;
            break;
        case 2:
            // �ж�����
            if ((currentDate.tm_year + 1900) % 4 == 0 &&
                ((currentDate.tm_year + 1900) % 100 != 0 || (currentDate.tm_year + 1900) % 400 == 0))
                daysInMonth = 29;
            else
                daysInMonth = 28;
            break;
        default:
            daysInMonth = 31;
    }
    int res = 0;
    for (int i = 0; i < daysInMonth; ++i) {
        res += 1 << i;
    }
    return res;
}
