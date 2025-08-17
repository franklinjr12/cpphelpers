#include <cstdio>
#include <cstdint>
#include <string>

class Time {
public:
    // parses time in the format hh:mm:dd
    static Time parse(std::string timeStr) {
        Time t;
        if (timeStr.length() != 8) {
            return t;
        }
        if (timeStr[2] != ':' || timeStr[5] != ':') {
            return t;
        }
        if (!isDigit(timeStr[0]) || !isDigit(timeStr[1]) ||
            !isDigit(timeStr[3]) || !isDigit(timeStr[4]) ||
            !isDigit(timeStr[6]) || !isDigit(timeStr[7])) {
            return t;
        }
        t.hour = std::stoi(timeStr.substr(0,2));
        if (t.hour > 24) {
            return t;
        }
        
        t.minute = std::stoi(timeStr.substr(3,5));
        if (t.minute > 60) {
            return t;
        }
        t.second = std::stoi(timeStr.substr(6,8));
        if (t.second > 60) {
            return t;
        }
        t.valid = true;
        return t;
    }

    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    bool valid = false;

private:
    static bool isDigit(char c) { return c >= '0' && c <= '9'; }
};

int main(void) {
    Time t1;

    t1 = Time::parse("11:22:33");
    if (t1.valid == true && t1.hour == 11 && t1.minute == 22 && t1.second == 33) {
        printf("[o] correctly parses time string\n");
    } else {
        printf("[x] incorrectly parses time string\n");
    }

    t1 = Time::parse("");
    if (t1.valid == false) {
        printf("[o] correctly parses empty string\n");
    } else {
        printf("[x] incorrectly parses empty string\n");
    }

    t1 = Time::parse("11:22;33");
    if (t1.valid == false) {
        printf("[o] correctly marks as invalid with ;\n");
    } else {
        printf("[x] incorrectly marks as invalid with ;\n");
    }

    t1 = Time::parse("11:2:33");
    if (t1.valid == false) {
        printf("[o] correctly marks as invalid incorrect digits\n");
    } else {
        printf("[x] incorrectly marks as invalid incorrect digits\n");
    }

    t1 = Time::parse("11:02:33");
    if (t1.valid == true && t1.hour == 11 && t1.minute == 2 && t1.second == 33) {
        printf("[o] correctly marks as valid digits with starting 0\n");
    } else {
        printf("[x] incorrectly marks as valid digits with starting 0\n");
    }

    t1 = Time::parse("a1:02:33");
    if (t1.valid == false) {
        printf("[o] correctly marks as invalid when there are chars that are not digits\n");
    } else {
        printf("[x] incorrectly marks as invalid when there are chars that are not digits\n");
    }

    t1 = Time::parse("25:02:33");
    if (t1.valid == false) {
        printf("[o] correctly marks as invalid when hours is bigger than 24\n");
    } else {
        printf("[x] incorrectly marks as invalid when hours is bigger than 24\n");
    }

    t1 = Time::parse("23:61:33");
    if (t1.valid == false) {
        printf("[o] correctly marks as invalid when minutes is bigger than 60\n");
    } else {
        printf("[x] incorrectly marks as invalid when minutes is bigger than 60\n");
    }

    t1 = Time::parse("23:02:71");
    if (t1.valid == false) {
        printf("[o] correctly marks as invalid when seconds is bigger than 60\n");
    } else {
        printf("[x] incorrectly marks as invalid when seconds is bigger than 60\n");
    }

    return 0;
}