#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct TimeSetting {
        long wt[7][5]; // week times
};

struct Request {
        long st; // start time
        long et; // end time
};

struct TimeResult {
        long sod; // start of day
        int dow; // day of week
};

int parseParameter(char *str, struct TimeSetting *ts);

int parseArguments(char *str, struct Request *req);

int parseLongArray(char *str, long *arr, int length);

long computeDuration(struct TimeSetting ts, struct Request req);

void computeTime(long t, struct TimeResult *tr);

int main(int argc, char *argv[]) {
        struct TimeSetting ts;
        if (argc < 2 || parseParameter(argv[1], &ts) != 0) {
                return 1;
        }

        char *line = NULL;
        size_t len = 0;
        ssize_t lineSize = 0;
        while ((lineSize = getline(&line, &len, stdin)) != -1) {
                struct Request req;
                if (parseArguments(line, &req) != 0) {
                        printf("0\n");
                } else {
                        long duration = computeDuration(ts, req);
                        printf("%ld\n", duration);
                }
        }
        free(line);
        return 0;
}

int parseParameter(char *str, struct TimeSetting *ts) {
        long arr[35];
        if (parseLongArray(str, arr, 35) != 0) {
                return 1;
        }
        int i, k;
        for (i = 0, k = 0; i < 7; i++) {
                int j;
                for (j = 0; j < 5; j++, k++) {
                        ts->wt[i][j] = arr[k];
                }
        }
        return 0;
}

int parseArguments(char *str, struct Request *req) {
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

long computeDuration(struct TimeSetting ts, struct Request req) {
        if (req.et <= req.st) {
                return 0;
        }

        struct TimeResult str; // start time result
        struct TimeResult etr; // end time result
        computeTime(req.st, &str);
        computeTime(req.et, &etr);

        long startWorkMinMills = str.sod + ts.wt[str.dow][1];
        long startWorkMaxMills = str.sod + ts.wt[str.dow][2];
        long endWorkMinMills = etr.sod + ts.wt[etr.dow][1];
        long endWorkMaxMills = etr.sod + ts.wt[etr.dow][2];

        long efficientStart;
        if (req.st < startWorkMinMills) {
                efficientStart = startWorkMinMills;
        } else if (req.st > startWorkMaxMills) {
                efficientStart = startWorkMaxMills;
        } else {
                efficientStart = req.st;
        }

        long efficientEnd;
        if (req.et < endWorkMinMills) {
                efficientEnd = endWorkMinMills;
        } else if (req.et > endWorkMaxMills) {
                efficientEnd = endWorkMaxMills;
        } else {
                efficientEnd = req.et;
        }


        long tempDuration = efficientEnd - efficientStart;
        long continuedNum = (etr.sod - str.sod) / (86400000 * 7);
        int weekInterval = etr.dow - str.dow;


        long minus = 0;
        if (weekInterval > 0) {
                int i;
                for (i = str.dow; i < etr.dow; i++) {
                        minus += ts.wt[i][3];
                }
        } else if (weekInterval < 0) {
                int i;
                for (i = str.dow; i < 7; i++) {
                        minus += ts.wt[i][3];
                }
                for (i = 0; i < etr.dow; i++) {
                        minus += ts.wt[i][3];
                }
        }
        return tempDuration - minus - continuedNum * ts.wt[0][4];
}

void computeTime(long t, struct TimeResult *tr) {
        time_t rawTime = t / 1000 + 8 * 60 * 60;
        struct tm *info = gmtime(&rawTime);
        int hour = info->tm_hour;
        int min = info->tm_min;
        int sec = info->tm_sec;

        tr->sod = (t / 1000 - hour * 60 * 60 - min * 60 - sec) * 1000;
        tr->dow = (info->tm_wday + 6 ) % 7;
}