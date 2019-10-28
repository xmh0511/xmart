#pragma once
#include <xfinal/xfinal.hpp>
#include "serialize.hpp"
namespace xmart {
	using namespace xfinal;
}
namespace xmart {
	template<typename T>
	std::enable_if_t<std::is_same<int, T>::value || std::is_same<short, T>::value, T> str_to_fundamention(nonstd::string_view str) {
		return std::atoi(str.data());
	}

	template<typename T>
	std::enable_if_t<std::is_same<long, T>::value, T> str_to_fundamention(nonstd::string_view str) {
		return std::atol(str.data());
	}

	template<typename T>
	std::enable_if_t<std::is_same<std::int64_t, T>::value, T> str_to_fundamention(nonstd::string_view str) {
		return std::atoll(str.data());
	}

	template<typename T>
	std::enable_if_t<std::is_same<float, T>::value || std::is_same<double, T>::value, T> str_to_fundamention(nonstd::string_view str) {
		return std::atof(str.data());
	}

#ifdef _ENABLE_XORM_
	template<typename T>
	std::enable_if_t<xorm::is_fundamention_type<T>::value && (!std::is_same<typename T::value_type, char>::value), typename T::value_type> mapping_query_value(nonstd::string_view str) {
		return str_to_fundamention<typename T::value_type>(str);
	}

	template<typename T>
	std::enable_if_t<xorm::is_fundamention_type<T>::value&& std::is_same<typename T::value_type, char>::value, typename T::value_type> mapping_query_value(nonstd::string_view str) {
		return char{ std::atoi(str.data()) };
	}

	template<typename T>
	std::enable_if_t<xorm::is_date_type<T>::value, T> mapping_query_value(nonstd::string_view str) {
		return T{ std::string(str.data(),str.size()) };
	}

	template<typename T>
	std::enable_if_t<std::is_same<T, std::string>::value, std::string> mapping_query_value(nonstd::string_view str) {
		return std::string(str.data(), str.size());
	}


#endif

	template<typename T>
	std::enable_if_t<reflector::is_reflect_class<T>::value, T> map_from_query(request& req) {
		T tmp{};
		reflector::each_object(tmp, [&req](auto&& obj, auto name, auto field) {
			(obj.*field) = mapping_query_value<typename std::remove_reference<decltype(obj.*field)>::type>(req.query(name));
		});
		return tmp;
	}

	template<typename T>
	std::enable_if_t<reflector::is_reflect_class<T>::value, T> map_from_params(request& req) {
		T tmp{};
		reflector::each_object(tmp, [&req](auto&& obj, auto name, auto field) {
			(obj.*field) = mapping_query_value<typename std::remove_reference<decltype(obj.*field)>::type>(req.param(name));
		});
		return tmp;
	}


}