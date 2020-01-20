#pragma once
#include <xfinal/xfinal.hpp>
#include "serialize.hpp"
namespace xmart {
	using namespace xfinal;
}
namespace xmart {
	template<typename Request>
	struct auto_params_lambda_map_from_params {
		auto_params_lambda_map_from_params(Request& req_):req(req_){

		}
		template<typename T,typename U,typename Y>
		void operator()(T&& obj,U&& name,Y&& field) {
			(obj.*field) = mapping_query_value<typename std::remove_reference<decltype(obj.*field)>::type>(req.param(name));
		}
	private:
		Request& req;
	};

	template<typename Request>
	struct auto_params_lambda_map_from_query {
		auto_params_lambda_map_from_query(Request& req_):req(req_) {

		}
		template<typename T, typename U, typename Y>
		void operator()(T&& obj, U&& name, Y&& field) {
			(obj.*field) = mapping_query_value<typename std::remove_reference<decltype(obj.*field)>::type>(req.query(name));
		}
	private:
		Request& req;
	};
	template<typename T>
	typename std::enable_if<std::is_same<int, T>::value || std::is_same<short, T>::value, T>::type str_to_fundamention(nonstd::string_view str) {
		return std::atoi(str.data());
	}

	template<typename T>
	typename std::enable_if<std::is_same<std::int64_t, T>::value, T>::type str_to_fundamention(nonstd::string_view str) {
		return std::atoll(str.data());
	}

	template<typename T>
	typename std::enable_if<std::is_same<float, T>::value || std::is_same<double, T>::value, T>::type str_to_fundamention(nonstd::string_view str) {
		return std::atof(str.data());
	}

#ifdef _ENABLE_XORM_
	template<typename T>
	typename std::enable_if<xorm::is_fundamention_type<T>::value && (!std::is_same<typename T::value_type, char>::value), typename T::value_type>::type mapping_query_value(nonstd::string_view str) {
		return str_to_fundamention<typename T::value_type>(str);
	}

	template<typename T>
	typename std::enable_if<xorm::is_fundamention_type<T>::value&& std::is_same<typename T::value_type, char>::value, typename T::value_type>::type mapping_query_value(nonstd::string_view str) {
		return char{ std::atoi(str.data()) };
	}

	template<typename T>
	typename std::enable_if<xorm::is_date_type<T>::value, T>::type mapping_query_value(nonstd::string_view str) {
		return T{ std::string(str.data(),str.size()) };
	}

	template<typename T>
	typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type mapping_query_value(nonstd::string_view str) {
		return std::string(str.data(), str.size());
	}


#endif

	template<typename T, typename Request>
	typename std::enable_if<reflector::is_reflect_class<T>::value, T>::type map_from_query(Request& req) {
		T tmp{};
		auto_params_lambda_map_from_query<Request> lambda{ req };
		reflector::each_object(tmp, lambda);
		return tmp;
	}

	template<typename T,typename Request>
	typename std::enable_if<reflector::is_reflect_class<T>::value, T>::type map_from_params(Request& req) {
		T tmp{};
		auto_params_lambda_map_from_params<Request> lambda{ req };
		reflector::each_object(tmp, lambda);
		//reflector::each_object(tmp, [&req](auto&& obj, auto name, auto field) {
		//	(obj.*field) = mapping_query_value<typename std::remove_reference<decltype(obj.*field)>::type>(req.param(name));
		//});
		return tmp;
	}

	template<typename T>
	typename std::enable_if<reflector::is_reflect_class<T>::value, T>::type map_from_json(nlohmann::json const& json ) {
		T tmp{};
		serializer::to_object(tmp, json);
		return tmp;
	}

	template<typename T>
	typename std::enable_if<reflector::is_reflect_class<typename std::remove_reference<T>::type>::value, nlohmann::json>::type map_to_json(T&& obj) {
		return serializer::to_json(std::forward<T>(obj));
	}

	template<typename T>
	typename std::enable_if<reflector::is_reflect_class<typename std::remove_reference<T>::type>::value, nlohmann::json>::type list_to_json(std::vector<T>& vec) {
		json list;
		for (auto& iter : vec) {
			auto node = map_to_json(iter);
			list.push_back(node);
		}
		return list;
	}

	template<typename T>
	typename std::enable_if<reflector::is_reflect_class<typename std::remove_reference<T>::type>::value, nlohmann::json>::type list_to_json(std::vector<T>&& vec) {
		json list;
		for (auto& iter : vec) {
			auto node = map_to_json(iter);
			list.push_back(node);
		}
		return list;
	}
}