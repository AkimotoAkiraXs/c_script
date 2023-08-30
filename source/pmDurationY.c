#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ============================[  Definition: Time Setting  ]============================
struct TimeSetting {
        int days;
        int year_count;
        int* year_arr;
        int* year_days_arr;
        int* month_arr;
        int* month_days_arr;
};

int parseTimeSetting(char *str, struct TimeSetting **pts);

int findFirstInt(char *str);

int parseIntArray(char *str, int *arr, int length);

int countDays(int ind);

struct TimeSetting *newTimeSetting(int cnt);

void releaseTimeSetting(struct TimeSetting *ts);

void printTimeSetting(struct TimeSetting *ts);


// ============================[  Definition: Request  ]============================
struct Request {
        long st; // start time
        long et; // end time
};

int parseRequest(char *str, struct Request *req);

int parseLongArray(char *str, long *arr, int length);


// ============================[  Definition: Duration  ]============================
struct Date {
        int year;
        int month;
        int day;
        int millsInDay;
        int isRest;
};

long calculateDuration(struct TimeSetting *ts, struct Request req);

void parseDate(long mills, struct Date *date);

void printDate(struct Date *date);

int doCalulate(struct Date *date1, struct Date *date2, struct TimeSetting *ts);


// ============================[  Main  ]============================
int main(int argc, char *argv[]) {
        struct TimeSetting *ts;
        if (argc < 2 || parseTimeSetting(argv[1], &ts) != 0) {
                return 1;
        }
        //printTimeSetting(ts);

        char *line = NULL;
        size_t len = 0;
        ssize_t lineSize = 0;
        while ((lineSize = getline(&line, &len, stdin)) != -1) {
                struct Request req;
                if (parseRequest(line, &req) != 0) {
                        printf("0\n");
                } else {
                        long duration = calculateDuration(ts, req);
                        printf("%ld\n", duration);
                }
        }
        free(line);
        releaseTimeSetting(ts);
        return 0;
}



// ============================[  Implementation: Duration  ]============================
long calculateDuration(struct TimeSetting *ts, struct Request req) {
        if (req.et <= req.st) {
                return 0;
        }
        struct Date date1;
        struct Date date2;
        parseDate(req.st, &date1);
        parseDate(req.et, &date2);
        int restDays = doCalulate(&date1, &date2, ts);
        //printDate(&date1);
        //printDate(&date2);

        long millsPerDay = 24 * 60 * 60 * 1000;
        long res = req.et - req.st;
        res -= (restDays * millsPerDay);
        if (date1.isRest == 1) {
                res += date1.millsInDay;
        }
        if (date2.isRest == 1) {
                res += (millsPerDay - date2.millsInDay);
        }

        return res;
}

void parseDate(long mills, struct Date *date) {
        mills += (8 * 60 * 60 * 1000);
        time_t rawTime = mills / 1000;
        struct tm *info = gmtime(&rawTime);
        date->year = info->tm_year + 1900;
        date->month = info->tm_mon;
        date->day = info->tm_mday-1;
        date->millsInDay = mills % (24 * 60 * 60 * 1000);
        date->isRest = 0;
}

void printDate(struct Date *date) {
        printf("Date: Year(%d), Month(%d), Day(%d), MillsInDay(%d), IsRest(%d)\n", date->year, date->month+1, date->day+1, date->millsInDay, date->isRest);
}

int doCalulate(struct Date *date1, struct Date *date2, struct TimeSetting *ts) {
        int rDays = ts->days;
        int i, year, month, day;
        for (i = 0; i < ts->year_count; i++) {
                year = ts->year_arr[i];
                if (year < date1->year) {
                        rDays -= ts->year_days_arr[i];
                } else if (year == date1->year) {
                        for (month = 0; month < 12; month++) {
                                if (month < date1->month) {
                                        rDays -= ts->month_days_arr[i * 12 + month];
                                } else if (month == date1->month) {
                                        for (day = 0; day < 31; day++) {
                                                if (day < date1->day) {
                                                        if (((1 << day) & ts->month_arr[i * 12 + month]) != 0) {
                                                                rDays--;
                                                        }
                                                } else {
                                                        if (day == date1->day) {
                                                                if (((1 << day) & ts->month_arr[i * 12 + month]) != 0) {
                                                                        date1->isRest = 1;
                                                                }
                                                        }
                                                        break;
                                                }
                                        }
                                } else {
                                        break;
                                }
                        }
                } else {
                        break;
                }
        }
        for (i = ts->year_count - 1; i >= 0; i--) {
                year = ts->year_arr[i];
                if (year > date2->year) {
                        rDays -= ts->year_days_arr[i];
                } else if (year == date2->year) {
                        for (month = 11; month >= 0; month--) {
                                if (month > date2->month) {
                                        rDays -= ts->month_days_arr[i * 12 + month];
                                } else if (month == date2->month) {
                                        for (day = 30; day >= 0; day--) {
                                                if (day > date2->day) {
                                                        if (((1 << day) & ts->month_arr[i * 12 + month]) != 0) {
                                                                rDays--;
                                                        }
                                                } else {
                                                        if (day == date2->day) {
                                                                if (((1 << day) & ts->month_arr[i * 12 + month]) != 0) {
                                                                        date2->isRest = 1;
                                                                }
                                                        }
                                                        break;
                                                }
                                        }
                                } else {
                                        break;
                                }
                        }
                } else {
                        break;
                }
        }
        return rDays;
}


// ============================[  Implementation: Request  ]============================
int parseRequest(char *str, struct Request *req) {
        long arr[2];
        if (parseLongArray(str, arr, 2) != 0) {
                return 1;
        }
        req->st = arr[0];
        req->et = arr[1];
        return 0;
}

int parseLongArray(char *str, long *arr, int length) {
        char tmp[20+1] = {'\0'};
        int i = 0, j = 0;
        char cur;
        while ((cur = *str) != '\0') {
                if (i >= length) {
                        break;
                }
                if (cur >= '0' && cur <= '9') {
                        tmp[j] = cur;
                        j++;
                } else if (cur == ',') {
                        *(arr+i) = atol(tmp);
                        i++;
                        memset(tmp, '\0', sizeof(tmp));
                        j = 0;
                }
                str++;
        }
        if (i != length - 1 || j == 0) {
                return 1;
        }
        *(arr+i) = atol(tmp);
        return 0;
}


// ============================[  Implementation: Time Setting  ]============================
int parseTimeSetting(char *str, struct TimeSetting **pts) {
        int cnt = findFirstInt(str);
        if (cnt == 0) {
                struct TimeSetting *ts = newTimeSetting(cnt);
                ts->days = 0;
                ts->year_count = 0;
                *pts = ts;
                return 0;
        }
        int arr[cnt*13+1];
        if (parseIntArray(str, arr, cnt*13+1) != 0) {
                return 1;
        }

        struct TimeSetting *ts = newTimeSetting(cnt);
        ts->year_count = arr[0];
        ts->days = 0;
        int i, j;
        for (i = 0; i < cnt; i++) {
                ts->year_arr[i] = arr[i * 13 + 1];
                ts->year_days_arr[i] = 0;
                for (j = 0; j < 12; j++) {
                        ts->month_arr[i * 12 + j] = arr[i * 13 + j + 2];
                        ts->month_days_arr[i * 12 + j] = countDays(ts->month_arr[i * 12 + j]);
                        ts->year_days_arr[i] += ts->month_days_arr[i * 12 + j];
                        ts->days += ts->month_days_arr[i * 12 + j];
                }
        }

        *pts = ts;
        return 0;
}

int findFirstInt(char *str) {
        char tmp[20+1] = {'\0'};
        int j = 0;
        char cur;
        while ((cur = *str) != '\0') {
                if (cur >= '0' && cur <= '9') {
                        tmp[j] = cur;
                        j++;
                } else if (cur == ',') {
                        break;
                }
                str++;
        }
        if (j == 0) {
                return 0;
        } else {
                return atoi(tmp);
        }
}

int parseIntArray(char *str, int *arr, int length) {
        char tmp[20+1] = {'\0'};
        int i = 0, j = 0;
        char cur;
        while ((cur = *str) != '\0') {
                if (i >= length) {
                        break;
                }
                if (cur >= '0' && cur <= '9') {
                        tmp[j] = cur;
                        j++;
                } else if (cur == ',') {
                        *(arr+i) = atoi(tmp);
                        i++;
                        memset(tmp, '\0', sizeof(tmp));
                        j = 0;
                }
                str++;
        }
        if (i != length - 1 || j == 0) {
                return 1;
        }
        *(arr+i) = atoi(tmp);
        return 0;
}

int countDays(int ind) {
        int count = 0;
        while(ind != 0) {
                ind &= ind -1;
                count++;
        }
        return count;
}

struct TimeSetting *newTimeSetting(int cnt) {
        struct TimeSetting *ts = malloc(sizeof(struct TimeSetting));
        ts->year_arr = malloc(sizeof(int)*cnt);
        ts->year_days_arr = malloc(sizeof(int)*cnt);
        ts->month_arr = malloc(sizeof(int)*cnt*12);
        ts->month_days_arr = malloc(sizeof(int)*cnt*12);
        return ts;
}

void releaseTimeSetting(struct TimeSetting *ts) {
        free(ts->year_arr);
        free(ts->year_days_arr);
        free(ts->month_arr);
        free(ts->month_days_arr);
        free(ts);
}

void printTimeSetting(struct TimeSetting *ts) {
        printf("TimeSetting(%d):\n", ts->days);
        int i, j;
        for (i = 0; i < ts->year_count; i++) {
                printf("   %d(%d): [", ts->year_arr[i], ts->year_days_arr[i]);
                for (j = 0; j < 11; j++) {
                        printf("%d(%d), ", ts->month_arr[i * 12 + j], ts->month_days_arr[i * 12 + j]);
                }
                printf("%d(%d)]\n", ts->month_arr[i * 12 + j], ts->month_days_arr[i * 12 + j]);
        }
}