#include "prx/libSceIcu/libSceIcu.hpp"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include <limits>
#include <vector>

namespace {

constexpr std::int32_t MaxCodePoint = 0x10ffff;
constexpr int CategoryCount = 30;

void Require(bool condition, const char* message) {
    if (condition) return;
    std::fprintf(stderr, "ICU category: %s\n", message);
    std::abort();
}

void RequireAt(bool condition, const char* message, std::int32_t character) {
    if (condition) return;
    std::fprintf(stderr, "ICU category: %s at U+%04X\n", message, static_cast<unsigned>(character));
    std::abort();
}

constexpr std::array<std::int64_t, CategoryCount> ExpectedCounts{
    814730, 1886, 2283, 31, 410, 141062, 2059, 13, 471, 770,
    239, 915, 17, 1, 1, 65, 170, 137468, 2048, 27,
    79, 77, 10, 641, 960, 64, 125, 7468, 12, 10};

constexpr std::int64_t ExpectedRuns = 4144;
constexpr std::int64_t ExpectedDigitRuns = 77;
constexpr std::int64_t ExpectedDigits = 770;

bool IsLetter(std::int32_t category) {
    return category >= 1 && category <= 5;
}

bool IsNumber(std::int32_t category) {
    return category >= 9 && category <= 11;
}

bool IsSeparator(std::int32_t category) {
    return category >= 12 && category <= 14;
}

bool IsPunctuation(std::int32_t category) {
    return (category >= 19 && category <= 23) || category == 28 || category == 29;
}

bool IsJavaIgnorable(std::int32_t character) {
    if (character <= 0x9f) {
        const bool control = static_cast<std::uint32_t>(character) <= 0x9f &&
                             (character <= 0x1f || character >= 0x7f);
        const bool controlSpace = character <= 0x1f && character >= 0x09 &&
                                  (character <= 0x0d || character >= 0x1c);
        return control && !controlSpace;
    }
    return u_charType_nid_postfix(character) == 16;
}

bool IsHexLetter(std::int32_t character) {
    return (character <= 0x66 && character >= 0x41 && (character <= 0x46 || character >= 0x61)) ||
           (character >= 0xff21 && character <= 0xff46 && (character <= 0xff26 || character >= 0xff41));
}

void CheckPredicates(std::int32_t character) {
    const std::int32_t category = u_charType_nid_postfix(character);
    RequireAt(category >= 0 && category < CategoryCount, "u_charType outside the category range", character);
    RequireAt(u_islower_nid_postfix(character) == (category == 2), "u_islower", character);
    RequireAt(u_isupper_nid_postfix(character) == (category == 1), "u_isupper", character);
    RequireAt(u_istitle_nid_postfix(character) == (category == 3), "u_istitle", character);
    RequireAt(u_isdigit_nid_postfix(character) == (category == 9), "u_isdigit", character);
    RequireAt(u_isxdigit_nid_postfix(character) == (IsHexLetter(character) || category == 9), "u_isxdigit", character);
    RequireAt(u_isalpha_nid_postfix(character) == IsLetter(category), "u_isalpha", character);
    RequireAt(u_isalnum_nid_postfix(character) == (IsLetter(category) || category == 9), "u_isalnum", character);
    RequireAt(u_isdefined_nid_postfix(character) == (category != 0), "u_isdefined", character);
    RequireAt(u_isbase_nid_postfix(character) == (IsLetter(category) || IsNumber(category) ||
                                                  category == 7 || category == 8), "u_isbase", character);
    RequireAt(u_iscntrl_nid_postfix(character) == (category == 15 || category == 16 ||
                                                   category == 13 || category == 14), "u_iscntrl", character);
    RequireAt(u_isprint_nid_postfix(character) ==
              (category != 0 && category != 15 && category != 16 && category != 17 && category != 18),
              "u_isprint", character);
    RequireAt(u_isgraph_nid_postfix(character) ==
              (category != 0 && category != 15 && category != 16 && category != 18 &&
               !IsSeparator(category)), "u_isgraph", character);
    RequireAt(u_ispunct_nid_postfix(character) == IsPunctuation(category), "u_ispunct", character);
    RequireAt(u_isJavaIDStart_nid_postfix(character) ==
              (IsLetter(category) || category == 25 || category == 22), "u_isJavaIDStart", character);
    RequireAt(u_isJavaIDPart_nid_postfix(character) ==
              (IsLetter(category) || category == 9 || category == 10 || category == 25 ||
               category == 22 || category == 8 || category == 6 || IsJavaIgnorable(character)),
              "u_isJavaIDPart", character);
    RequireAt(u_isIDIgnorable_nid_postfix(character) == IsJavaIgnorable(character), "u_isIDIgnorable", character);
    const std::int32_t digit = u_charDigitValue_nid_postfix(character);
    RequireAt((digit >= 0) == (category == 9), "u_charDigitValue is defined exactly for Nd", character);
    RequireAt(digit < 10, "u_charDigitValue above 9", character);
}

void CheckInvalid(std::int32_t character) {
    RequireAt(u_charType_nid_postfix(character) == 0, "u_charType is U_UNASSIGNED", character);
    RequireAt(u_islower_nid_postfix(character) == 0, "u_islower", character);
    RequireAt(u_isupper_nid_postfix(character) == 0, "u_isupper", character);
    RequireAt(u_istitle_nid_postfix(character) == 0, "u_istitle", character);
    RequireAt(u_isdigit_nid_postfix(character) == 0, "u_isdigit", character);
    RequireAt(u_isxdigit_nid_postfix(character) == 0, "u_isxdigit", character);
    RequireAt(u_isalpha_nid_postfix(character) == 0, "u_isalpha", character);
    RequireAt(u_isalnum_nid_postfix(character) == 0, "u_isalnum", character);
    RequireAt(u_isdefined_nid_postfix(character) == 0, "u_isdefined", character);
    RequireAt(u_isbase_nid_postfix(character) == 0, "u_isbase", character);
    RequireAt(u_iscntrl_nid_postfix(character) == 0, "u_iscntrl", character);
    RequireAt(u_isprint_nid_postfix(character) == 0, "u_isprint", character);
    RequireAt(u_isgraph_nid_postfix(character) == 0, "u_isgraph", character);
    RequireAt(u_ispunct_nid_postfix(character) == 0, "u_ispunct", character);
    RequireAt(u_isJavaIDStart_nid_postfix(character) == 0, "u_isJavaIDStart", character);
    RequireAt(u_isJavaIDPart_nid_postfix(character) == 0, "u_isJavaIDPart", character);
    RequireAt(u_charDigitValue_nid_postfix(character) == -1, "u_charDigitValue", character);
}

void CheckTable() {
    std::array<std::int64_t, CategoryCount> counts{};
    std::vector<std::int32_t> runStarts;
    runStarts.reserve(4600);
    std::int64_t digitRuns = 0;
    std::int64_t digits = 0;
    std::int32_t previousCategory = -1;
    bool inDigitRun = false;
    std::int32_t previousDigit = -2;
    for (std::int32_t character = 0; character <= MaxCodePoint; ++character) {
        const std::int32_t category = u_charType_nid_postfix(character);
        RequireAt(category >= 0 && category < CategoryCount, "u_charType outside the category range", character);
        ++counts[static_cast<std::size_t>(category)];
        if (category != previousCategory) {
            runStarts.push_back(character);
            previousCategory = category;
        }
        const std::int32_t digit = u_charDigitValue_nid_postfix(character);
        if (digit >= 0) {
            ++digits;
            if (!inDigitRun || digit != previousDigit + 1) ++digitRuns;
            inDigitRun = true;
        } else {
            inDigitRun = false;
        }
        previousDigit = digit;
    }
    Require(counts == ExpectedCounts, "the per-category counts of Unicode 17.0.0");
    Require(static_cast<std::int64_t>(runStarts.size()) == ExpectedRuns,
            "the number of general category runs of Unicode 17.0.0");
    Require(digitRuns == ExpectedDigitRuns, "the number of decimal digit runs of Unicode 17.0.0");
    Require(digits == ExpectedDigits, "the number of decimal digits of Unicode 17.0.0");
    for (const std::int32_t start : runStarts) {
        CheckPredicates(start);
        CheckPredicates(start - 1);
    }
    CheckPredicates(MaxCodePoint);
    for (std::int32_t character = 0; character <= 0xffff; ++character) CheckPredicates(character);
    for (std::int32_t character = 0x10000; character <= MaxCodePoint; character += 257) CheckPredicates(character);
}

void CheckDigits() {
    const std::array<std::int32_t, 12> zeros{
        0x30, 0x660, 0x6f0, 0x966, 0x9e6, 0xa66, 0xae6, 0xb66, 0xc66, 0xce6, 0x1d7ce, 0x11de0};
    for (const std::int32_t zero : zeros) {
        for (std::int32_t offset = 0; offset <= 9; ++offset) {
            RequireAt(u_charDigitValue_nid_postfix(zero + offset) == offset, "a decimal digit value", zero + offset);
            RequireAt(u_isdigit_nid_postfix(zero + offset) == 1, "a decimal digit", zero + offset);
        }
        const std::int32_t following = u_charDigitValue_nid_postfix(zero + 10);
        RequireAt(following < 0 || following == 0, "a decimal digit run after a zero-to-nine run starts at zero", zero + 10);
        const std::int32_t preceding = u_charDigitValue_nid_postfix(zero - 1);
        RequireAt(preceding < 0 || preceding == 9, "a decimal digit run before a zero-to-nine run ends at nine", zero - 1);
    }
    Require(u_charDigitValue_nid_postfix(0x2160) == -1, "u_charDigitValue for a Roman numeral");
    Require(u_charDigitValue_nid_postfix(0xb2) == -1, "u_charDigitValue for a superscript digit");
    Require(u_isbase_nid_postfix(0xb2) == 1, "u_isbase for a superscript digit");
    Require(u_charType_nid_postfix(0x2160) == 10, "a Roman numeral is Nl");
    Require(u_isalpha_nid_postfix(0x2160) == 0, "u_isalpha for a Roman numeral");
    Require(u_isalnum_nid_postfix(0x2160) == 0, "u_isalnum for a Roman numeral");
    Require(u_isdigit_nid_postfix(0x2160) == 0, "u_isdigit for a Roman numeral");
}

void CheckDigitConversion() {
    Require(u_digit_nid_postfix(0x30, 10) == 0, "u_digit('0', 10)");
    Require(u_digit_nid_postfix(0x39, 10) == 9, "u_digit('9', 10)");
    Require(u_digit_nid_postfix(0x39, 9) == -1, "u_digit('9', 9)");
    Require(u_digit_nid_postfix(0x61, 10) == -1, "u_digit('a', 10) is past the radix");
    Require(u_digit_nid_postfix(0x61, 11) == 10, "u_digit('a', 11)");
    Require(u_digit_nid_postfix(0x7a, 36) == 35, "u_digit('z', 36)");
    Require(u_digit_nid_postfix(0x5a, 36) == 35, "u_digit('Z', 36)");
    Require(u_digit_nid_postfix(0xff41, 36) == 10, "u_digit(fullwidth 'a', 36)");
    Require(u_digit_nid_postfix(0xff21, 36) == 10, "u_digit(fullwidth 'A', 36)");
    Require(u_digit_nid_postfix(0x660, 10) == 0, "u_digit(Arabic-Indic zero, 10)");
    Require(u_digit_nid_postfix(0x660, 2) == 0, "u_digit(Arabic-Indic zero, 2)");
    Require(u_digit_nid_postfix(0x67, 16) == -1, "u_digit of a letter past 'f' in base 16");
    Require(u_digit_nid_postfix(0x67, 36) == 16, "u_digit('g', 36)");
    Require(u_digit_nid_postfix(0x30, 1) == -1, "u_digit with radix 1");
    Require(u_digit_nid_postfix(0x30, 0) == -1, "u_digit with radix 0");
    Require(u_digit_nid_postfix(0x30, 37) == -1, "u_digit with radix 37");
    Require(u_digit_nid_postfix(0x30, -1) == -1, "u_digit with a negative radix");
    Require(u_digit_nid_postfix(0x30, 2) == 0, "u_digit('0', 2)");
    Require(u_digit_nid_postfix(0x31, 2) == 1, "u_digit('1', 2)");
    Require(u_digit_nid_postfix(0x32, 2) == -1, "u_digit('2', 2)");
    for (std::int32_t radix = 2; radix <= 36; ++radix) {
        for (std::int32_t digit = 0; digit < radix; ++digit) {
            const char character = static_cast<char>(digit < 10 ? '0' + digit : 'a' - 10 + digit);
            Require(u_digit_nid_postfix(character, static_cast<std::int8_t>(radix)) == digit, "u_digit round trip");
            Require(u_forDigit_nid_postfix(digit, static_cast<std::int8_t>(radix)) == character,
                    "u_forDigit round trip");
        }
        Require(u_forDigit_nid_postfix(radix, static_cast<std::int8_t>(radix)) == 0, "u_forDigit past the radix");
    }
    Require(u_forDigit_nid_postfix(-1, 10) == 0, "u_forDigit with a negative digit");
    Require(u_forDigit_nid_postfix(10, 1) == 0, "u_forDigit with radix 1");
    Require(u_forDigit_nid_postfix(10, 37) == 0, "u_forDigit with radix 37");
    Require(u_forDigit_nid_postfix(35, 36) == 0x7a, "u_forDigit(35, 36)");
    Require(u_forDigit_nid_postfix(10, 11) == 0x61, "u_forDigit(10, 11)");
}

void CheckHexDigits() {
    Require(u_isxdigit_nid_postfix(0x30) == 1, "u_isxdigit('0')");
    Require(u_isxdigit_nid_postfix(0x41) == 1, "u_isxdigit('A')");
    Require(u_isxdigit_nid_postfix(0x46) == 1, "u_isxdigit('F')");
    Require(u_isxdigit_nid_postfix(0x47) == 0, "u_isxdigit('G')");
    Require(u_isxdigit_nid_postfix(0x60) == 0, "u_isxdigit('`')");
    Require(u_isxdigit_nid_postfix(0x67) == 0, "u_isxdigit('g')");
    Require(u_isxdigit_nid_postfix(0xff10) == 1, "u_isxdigit(fullwidth zero)");
    Require(u_isxdigit_nid_postfix(0xff26) == 1, "u_isxdigit(fullwidth 'F')");
    Require(u_isxdigit_nid_postfix(0xff27) == 0, "u_isxdigit(fullwidth 'G')");
    Require(u_isxdigit_nid_postfix(0x660) == 1, "u_isxdigit(Arabic-Indic zero)");
}

void CheckUnicodeVersion() {
    Require(u_charType_nid_postfix(0x295) == 5, "U+0295 is Lo in Unicode 17.0");
    Require(u_isalpha_nid_postfix(0x295) == 1, "U+0295 is alphabetic");
    Require(u_charType_nid_postfix(0x20c1) == 25, "U+20C1 SAUDI RIYAL SIGN is Sc in Unicode 17.0");
    Require(u_charType_nid_postfix(0x11db0) == 5, "U+11DB0 TOLONG SIKI LETTER I is Lo in Unicode 17.0");
    Require(u_charDigitValue_nid_postfix(0x11de0) == 0, "U+11DE0 TOLONG SIKI DIGIT ZERO is zero");
    Require(u_charType_nid_postfix(0x16ea0) == 1, "U+16EA0 BERIA ERFE letter is Lu in Unicode 17.0");
    Require(u_charType_nid_postfix(0xd800) == 18, "U+D800 is a surrogate");
    Require(u_isdefined_nid_postfix(0xd800) == 1, "a surrogate is assigned");
    Require(u_isprint_nid_postfix(0xd800) == 0, "a surrogate is not printable");
    Require(u_charType_nid_postfix(0xe000) == 17, "U+E000 is private use");
    Require(u_isprint_nid_postfix(0xe000) == 0, "private use is not printable");
    Require(u_isgraph_nid_postfix(0xe000) == 1, "private use is graphic");
    Require(u_isprint_nid_postfix(0x200b) == 0, "U+200B is a format character");
    Require(u_iscntrl_nid_postfix(0x200b) == 1, "U+200B is a control for ICU");
    Require(u_isJavaIDPart_nid_postfix(0x200b) == 1, "U+200B is a Java identifier part");
    Require(u_isIDIgnorable_nid_postfix(0x200b) == 1, "U+200B is a Java identifier ignorable");
}

void CheckControlSpaces() {
    for (const std::int32_t character : {0x00, 0x08, 0x09, 0x0d, 0x0e, 0x1b, 0x1c, 0x1f, 0x20, 0x7e, 0x7f, 0x85, 0x9f, 0xa0}) {
        const bool control = static_cast<std::uint32_t>(character) <= 0x9f &&
                             (character <= 0x1f || character >= 0x7f);
        const bool controlSpace = character <= 0x1f && character >= 0x09 &&
                                  (character <= 0x0d || character >= 0x1c);
        RequireAt(u_isIDIgnorable_nid_postfix(character) == (control && !controlSpace),
                  "u_isIDIgnorable over the C0 and C1 controls", character);
    }
}

}

int main() {
    CheckTable();
    CheckDigits();
    CheckDigitConversion();
    CheckHexDigits();
    CheckUnicodeVersion();
    CheckControlSpaces();
    for (const std::int32_t character : {std::int32_t{-1}, std::int32_t{-0x1000}, MaxCodePoint + 1, 0x110000,
                                         std::numeric_limits<std::int32_t>::min(),
                                         std::numeric_limits<std::int32_t>::max()})
        CheckInvalid(character);
    std::puts("ICU category tests passed");
}
