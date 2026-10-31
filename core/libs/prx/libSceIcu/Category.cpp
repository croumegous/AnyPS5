#include "prx/libSceIcu/libSceIcu.hpp"
#include "prx/libSceIcu/CategoryData.hpp"
#include <algorithm>
#include <cstdint>
#include <iterator>

namespace {

constexpr std::int8_t Unassigned = 0;
constexpr std::int8_t UppercaseLetter = 1;
constexpr std::int8_t LowercaseLetter = 2;
constexpr std::int8_t TitlecaseLetter = 3;
constexpr std::int8_t ModifierLetter = 4;
constexpr std::int8_t OtherLetter = 5;
constexpr std::int8_t NonspacingMark = 6;
constexpr std::int8_t EnclosingMark = 7;
constexpr std::int8_t SpacingMark = 8;
constexpr std::int8_t DecimalDigitNumber = 9;
constexpr std::int8_t LetterNumber = 10;
constexpr std::int8_t OtherNumber = 11;
constexpr std::int8_t SpaceSeparator = 12;
constexpr std::int8_t LineSeparator = 13;
constexpr std::int8_t ParagraphSeparator = 14;
constexpr std::int8_t ControlChar = 15;
constexpr std::int8_t FormatChar = 16;
constexpr std::int8_t PrivateUseChar = 17;
constexpr std::int8_t Surrogate = 18;
constexpr std::int8_t DashPunctuation = 19;
constexpr std::int8_t OpenPunctuation = 20;
constexpr std::int8_t ClosePunctuation = 21;
constexpr std::int8_t ConnectorPunctuation = 22;
constexpr std::int8_t OtherPunctuation = 23;
constexpr std::int8_t CurrencySymbol = 25;
constexpr std::int8_t InitialPunctuation = 28;
constexpr std::int8_t FinalPunctuation = 29;

constexpr std::int32_t InvalidCodePoint = -1;

std::int8_t CategoryOf(std::int32_t character) {
    if (character < 0 || character > 0x10ffff) return Unassigned;
    const auto* const first = std::begin(IcuCategory::CATEGORY_RANGES);
    const auto* const last = std::end(IcuCategory::CATEGORY_RANGES);
    const auto* const found = std::upper_bound(first, last, character,
        [](std::int32_t value, const IcuCategory::CategoryRange& range) { return value < range.First; });
    if (found == first) return Unassigned;
    const IcuCategory::CategoryRange& range = *(found - 1);
    return character <= range.Last ? range.Category : Unassigned;
}

bool IsLetter(std::int8_t category) {
    return category >= UppercaseLetter && category <= OtherLetter;
}

bool IsNumber(std::int8_t category) {
    return category >= DecimalDigitNumber && category <= OtherNumber;
}

bool IsPunctuation(std::int8_t category) {
    return (category >= DashPunctuation && category <= OtherPunctuation) ||
           category == InitialPunctuation || category == FinalPunctuation;
}

bool IsSeparator(std::int8_t category) {
    return category >= SpaceSeparator && category <= ParagraphSeparator;
}

bool IsOther(std::int8_t category) {
    return category == Unassigned || category == ControlChar || category == FormatChar ||
           category == PrivateUseChar || category == Surrogate;
}

std::int32_t DigitValueOf(std::int32_t character) {
    if (character < 0 || character > 0x10ffff) return InvalidCodePoint;
    const auto* const first = std::begin(IcuCategory::DIGIT_RANGES);
    const auto* const last = std::end(IcuCategory::DIGIT_RANGES);
    const auto* const found = std::upper_bound(first, last, character,
        [](std::int32_t value, const IcuCategory::DigitRange& range) { return value < range.First; });
    if (found == first) return InvalidCodePoint;
    const IcuCategory::DigitRange& range = *(found - 1);
    if (character > range.Last) return InvalidCodePoint;
    return range.Value + (character - range.First);
}

bool IsAsciiControlSpace(std::int32_t character) {
    return character <= 0x1f && character >= 0x09 && (character <= 0x0d || character >= 0x1c);
}

bool IsIsoControl(std::int32_t character) {
    return static_cast<std::uint32_t>(character) <= 0x9f && (character <= 0x1f || character >= 0x7f);
}

std::int8_t IsJavaIdentifierIgnorable(std::int32_t character) {
    if (character <= 0x9f) return IsIsoControl(character) && !IsAsciiControlSpace(character);
    return CategoryOf(character) == FormatChar;
}

}

extern "C" {

std::int8_t APS5_VABI u_charType_nid_postfix(std::int32_t character) {
    return CategoryOf(character);
}

std::int8_t APS5_VABI u_islower_nid_postfix(std::int32_t character) {
    return CategoryOf(character) == LowercaseLetter;
}

std::int8_t APS5_VABI u_isupper_nid_postfix(std::int32_t character) {
    return CategoryOf(character) == UppercaseLetter;
}

std::int8_t APS5_VABI u_istitle_nid_postfix(std::int32_t character) {
    return CategoryOf(character) == TitlecaseLetter;
}

std::int8_t APS5_VABI u_isdigit_nid_postfix(std::int32_t character) {
    return CategoryOf(character) == DecimalDigitNumber;
}

std::int8_t APS5_VABI u_isxdigit_nid_postfix(std::int32_t character) {
    if ((character <= 0x66 && character >= 0x41 && (character <= 0x46 || character >= 0x61)) ||
        (character >= 0xff21 && character <= 0xff46 && (character <= 0xff26 || character >= 0xff41)))
        return 1;
    return CategoryOf(character) == DecimalDigitNumber;
}

std::int8_t APS5_VABI u_isalpha_nid_postfix(std::int32_t character) {
    return IsLetter(CategoryOf(character));
}

std::int8_t APS5_VABI u_isalnum_nid_postfix(std::int32_t character) {
    const std::int8_t category = CategoryOf(character);
    return IsLetter(category) || category == DecimalDigitNumber;
}

std::int8_t APS5_VABI u_isdefined_nid_postfix(std::int32_t character) {
    return CategoryOf(character) != Unassigned;
}

std::int8_t APS5_VABI u_isbase_nid_postfix(std::int32_t character) {
    const std::int8_t category = CategoryOf(character);
    return IsLetter(category) || IsNumber(category) || category == SpacingMark || category == EnclosingMark;
}

std::int8_t APS5_VABI u_iscntrl_nid_postfix(std::int32_t character) {
    const std::int8_t category = CategoryOf(character);
    return category == ControlChar || category == FormatChar || category == LineSeparator ||
           category == ParagraphSeparator;
}

std::int8_t APS5_VABI u_isprint_nid_postfix(std::int32_t character) {
    return !IsOther(CategoryOf(character));
}

std::int8_t APS5_VABI u_isgraph_nid_postfix(std::int32_t character) {
    const std::int8_t category = CategoryOf(character);
    return category != ControlChar && category != FormatChar && category != Surrogate &&
           category != Unassigned && !IsSeparator(category);
}

std::int8_t APS5_VABI u_ispunct_nid_postfix(std::int32_t character) {
    return IsPunctuation(CategoryOf(character));
}

std::int8_t APS5_VABI u_isJavaIDStart_nid_postfix(std::int32_t character) {
    const std::int8_t category = CategoryOf(character);
    return IsLetter(category) || category == CurrencySymbol || category == ConnectorPunctuation;
}

std::int8_t APS5_VABI u_isJavaIDPart_nid_postfix(std::int32_t character) {
    const std::int8_t category = CategoryOf(character);
    return IsLetter(category) || category == DecimalDigitNumber || category == LetterNumber ||
           category == CurrencySymbol || category == ConnectorPunctuation ||
           category == SpacingMark || category == NonspacingMark ||
           IsJavaIdentifierIgnorable(character);
}

std::int8_t APS5_VABI u_isIDIgnorable_nid_postfix(std::int32_t character) {
    return IsJavaIdentifierIgnorable(character);
}

std::int32_t APS5_VABI u_charDigitValue_nid_postfix(std::int32_t character) {
    return DigitValueOf(character);
}

std::int32_t APS5_VABI u_digit_nid_postfix(std::int32_t character, std::int8_t radix) {
    std::int8_t value;
    if (static_cast<std::uint8_t>(radix - 2) <= 34) {
        value = static_cast<std::int8_t>(DigitValueOf(character));
        if (value < 0) {
            if (character >= 0x61 && character <= 0x7a) value = static_cast<std::int8_t>(character - 0x57);
            else if (character >= 0x41 && character <= 0x5a) value = static_cast<std::int8_t>(character - 0x37);
            else if (character >= 0xff41 && character <= 0xff5a) value = static_cast<std::int8_t>(character - 0xff37);
            else if (character >= 0xff21 && character <= 0xff3a) value = static_cast<std::int8_t>(character - 0xff17);
        }
    } else {
        value = -1;
    }
    return value < radix ? value : -1;
}

std::int32_t APS5_VABI u_forDigit_nid_postfix(std::int32_t digit, std::int8_t radix) {
    if (static_cast<std::uint8_t>(radix - 2) > 34 ||
        static_cast<std::uint32_t>(digit) >= static_cast<std::uint32_t>(radix))
        return 0;
    return digit < 10 ? 0x30 + digit : 0x61 - 10 + digit;
}

}
