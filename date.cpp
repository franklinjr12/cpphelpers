#include <cstdio>
#include <string>

// Simple and short date class with parsing and comparing methods
class Date {
    public:
    static Date parse(std::string dateStr) {
        // checks if is on format YYYY-MM-DD
        if (dateStr.length() != 10) {
            return Date();
        }
        if (dateStr[4] != '-' || dateStr[7] != '-') {
            return Date();
        }
        // check if chars 0 to 3 are digits
        for (int i = 0; i < 4; ++i) {
            if (dateStr[i] < '0' || dateStr[i] > '9') {
                return Date();
            }
        }
        // check if chars 5 to 6 are digits
        for (int i = 5; i < 7; ++i) {
            if (dateStr[i] < '0' || dateStr[i] > '9') {
                return Date();
            }
        }
        // check if chars 8 to 9 are digits
        for (int i = 8; i < 10; ++i) {
            if (dateStr[i] < '0' || dateStr[i] > '9') {
                return Date();
            }
        }
        Date date;
        date.year = std::stoi(dateStr.substr(0, 4));
        if (date.year < 1) {
            return Date(); // Invalid year
        }
        date.month = std::stoi(dateStr.substr(5, 2));
        if (date.month < 1 || date.month > 12) {
            return Date(); // Invalid month
        }
        date.day = std::stoi(dateStr.substr(8, 2));
        if (date.day < 1 || date.day > 31) {
            return Date(); // Invalid day
        }
        date.valid = true;
        return date;
    }

    static Date parseFormated(std::string dateStr, std::string format) {
        // check if dateStr and format are equal length
        if (dateStr.length() != format.length()) {
            return Date();
        }
        // apply lowercase to format
        for (char& c : format) {
            c = tolower(c);
        }
        // Find positions of yyyy, mm, dd
        size_t yPos = format.find("yyyy");
        size_t mPos = format.find("mm");
        size_t dPos = format.find("dd");
        if (yPos == std::string::npos || mPos == std::string::npos || dPos == std::string::npos) {
            return Date();
        }
        // Extract year, month, day substrings
        std::string year = dateStr.substr(yPos, 4);
        std::string month = dateStr.substr(mPos, 2);
        std::string day = dateStr.substr(dPos, 2);
        // Compose yyyy-mm-dd string
        std::string normalized = year + "-" + month + "-" + day;
        // Use existing parse method
        return parse(normalized);
    }

    std::string toString() {
        char buffer[11];
        snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u", year, month, day);
        return std::string(buffer);
    }

    bool operator==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }

    bool operator!=(const Date& other) const {
        return !(*this == other);
    }

    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    bool operator<=(const Date& other) const {
        return *this < other || *this == other;
    }

    bool operator>(const Date& other) const {
        return !(*this <= other);
    }

    bool operator>=(const Date& other) const {
        return !(*this < other);
    }

    unsigned int year = 0;
    unsigned int month = 0;
    unsigned int day = 0;
    bool valid = false;
};

int main() {
    printf("Running date.cpp\n");

    Date date = Date::parse("2023-10-05");
    printf("Parsed date: %04u-%02u-%02u\n", date.year, date.month, date.day);

    if (date.toString() == "2023-10-05") {
        printf("Date toString() works correctly.\n");
    } else {
        printf("Date toString() failed.\n");
    }

    // Test == and !=
    Date d1 = Date::parse("2023-10-05");
    Date d2 = Date::parse("2023-10-05");
    Date d3 = Date::parse("2022-01-01");
    if (d1 == d2 && !(d1 != d2)) {
        printf("operator== and operator!= work for equal dates.\n");
    } else {
        printf("operator== or operator!= failed for equal dates.\n");
    }
    if (d1 != d3 && !(d1 == d3)) {
        printf("operator!= and operator== work for different dates.\n");
    } else {
        printf("operator!= or operator== failed for different dates.\n");
    }

    // Test <, <=, >, >=
    Date d4 = Date::parse("2024-01-01");
    if (d3 < d1 && d3 <= d1 && d1 > d3 && d1 >= d3) {
        printf("operator<, operator<=, operator>, operator>= work for d3 < d1.\n");
    } else {
        printf("Comparison operators failed for d3 < d1.\n");
    }
    if (d1 < d4 && d1 <= d4 && d4 > d1 && d4 >= d1) {
        printf("operator<, operator<=, operator>, operator>= work for d1 < d4.\n");
    } else {
        printf("Comparison operators failed for d1 < d4.\n");
    }
    if (d1 <= d2 && d1 >= d2) {
        printf("operator<= and operator>= work for equal dates.\n");
    } else {
        printf("operator<= or operator>= failed for equal dates.\n");
    }

    // Test parseFormated
    Date df1 = Date::parseFormated("05/10/2023", "dd/mm/yyyy");
    if (df1.valid && df1.year == 2023 && df1.month == 10 && df1.day == 5) {
        printf("parseFormated() works for dd/mm/yyyy.\n");
    } else {
        printf("parseFormated() failed for dd/mm/yyyy.\n");
    }

    Date df2 = Date::parseFormated("2023.10.05", "yyyy.mm.dd");
    if (df2.valid && df2.year == 2023 && df2.month == 10 && df2.day == 5) {
        printf("parseFormated() works for yyyy.mm.dd.\n");
    } else {
        printf("parseFormated() failed for yyyy.mm.dd.\n");
    }

    Date df3 = Date::parseFormated("2023/10/05", "yyyy/mm/dd");
    if (df3.valid && df3.year == 2023 && df3.month == 10 && df3.day == 5) {
        printf("parseFormated() works for yyyy/mm/dd.\n");
    } else {
        printf("parseFormated() failed for yyyy/mm/dd.\n");
    }

    Date df4 = Date::parseFormated("10-05-2023", "mm-dd-yyyy");
    if (df4.valid && df4.year == 2023 && df4.month == 10 && df4.day == 5) {
        printf("parseFormated() works for mm-dd-yyyy.\n");
    } else {
        printf("parseFormated() failed for mm-dd-yyyy.\n");
    }

    // Test parseFormated with invalid input
    Date df5 = Date::parseFormated("2023/10/05", "yy-dd-mm");
    if (!df5.valid) {
        printf("parseFormated() correctly fails for invalid format.\n");
    } else {
        printf("parseFormated() should have failed for invalid format.\n");
    }

    return 0;
}
