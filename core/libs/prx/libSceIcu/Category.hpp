#pragma once
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>

extern "C" {
std::int8_t APS5_VABI u_charType_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_islower_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isupper_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_istitle_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isdigit_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isxdigit_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isalpha_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isalnum_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isdefined_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isbase_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_iscntrl_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isprint_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isgraph_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_ispunct_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isJavaIDStart_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isJavaIDPart_nid_postfix(std::int32_t character);
std::int8_t APS5_VABI u_isIDIgnorable_nid_postfix(std::int32_t character);
std::int32_t APS5_VABI u_charDigitValue_nid_postfix(std::int32_t character);
std::int32_t APS5_VABI u_digit_nid_postfix(std::int32_t character, std::int8_t radix);
std::int32_t APS5_VABI u_forDigit_nid_postfix(std::int32_t digit, std::int8_t radix);
}
