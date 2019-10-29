#pragma once
#include <xorm/reflect/reflector.hpp>
#ifdef _ENABLE_XORM_
#include <xorm/xorm.hpp>
namespace xmart {
	using namespace xorm;
}
#endif 
namespace xmart {
	namespace serializer {
		template<typename T, typename U = std::enable_if_t<reflector::is_reflect_class<std::remove_reference_t<T>>::value>>
		inline nlohmann::json to_json(T && obj, nlohmann::json & json);
		template<typename T, typename U = std::enable_if_t<reflector::is_reflect_class<std::remove_reference_t<T>>::value>>
		inline nlohmann::json to_json(T && obj);

		template<typename T, typename U = std::enable_if_t<reflector::is_reflect_class<std::remove_reference_t<T>>::value>>
		inline std::remove_reference_t<T> to_object(T && t, nlohmann::json const& json);
		template<typename T, typename U = std::enable_if_t<reflector::is_reflect_class<std::remove_reference_t<T>>::value>>
		inline std::remove_reference_t<T> to_object(nlohmann::json const& json);
	}
	class serialize_ {
		template<typename T, typename U>
		friend nlohmann::json serializer::to_json(T&& obj, nlohmann::json& json);
		template<typename T, typename U>
		friend nlohmann::json serializer::to_json(T&& obj);
		template<typename T, typename U>
		friend std::remove_reference_t<T> serializer::to_object(T&& t, nlohmann::json const& json);
		template<typename T, typename U>
		friend std::remove_reference_t<T> serializer::to_object(nlohmann::json const& json);
	private:
		template<typename T>
		static std::enable_if_t<reflector::is_reflect_class<std::remove_reference_t<T>>::value> to_json_v(T&& obj, std::string const& name, nlohmann::json& json)
		{
			json[name] = nlohmann::json::object();
			to_json(obj, json[name]);
		}
#ifdef _ENABLE_XORM_
		template<typename T>
		static std::enable_if_t<!reflector::is_reflect_class<std::remove_reference_t<T>>::value && (!xmart::is_fundamention_type<typename std::remove_reference<T>::type>::value && !xmart::is_date_type<typename std::remove_reference<T>::type>::value)> to_json_v(T&& obj, std::string const& name, nlohmann::json& json)
		{
			json[name] = obj;
		}
		template<typename T>
		static std::enable_if_t<!reflector::is_reflect_class<std::remove_reference_t<T>>::value && (xmart::is_fundamention_type<typename std::remove_reference<T>::type>::value || xmart::is_date_type<typename std::remove_reference<T>::type>::value)> to_json_v(T&& obj, std::string const& name, nlohmann::json& json)
		{
			if (!obj.is_null()) {
				json[name] = obj.value();
			}
			else {
				json[name] = nullptr;
			}
		}
#else
		template<typename T>
		static std::enable_if_t<!reflector::is_reflect_class<std::remove_reference_t<T>>::value> to_json_v(T&& obj, std::string const& name, nlohmann::json& json)
		{
			json[name] = obj;
		}
#endif
		template<typename T, typename U = std::enable_if_t<reflector::is_reflect_class<std::remove_reference_t<T>>::value>>
		static void to_json(T && obj, nlohmann::json & json)
		{
			reflector::each_object(std::forward<T>(obj), [&json](auto& t, auto& name, auto offset) {
				to_json_v((t.*offset), name, json);
			});
		}
		template<typename T, typename U>
		static std::enable_if_t<reflector::is_reflect_class<std::remove_reference_t<T>>::value> to_object_v(T& t, U&& v)
		{
			to_object(t, std::forward<U>(v));
		}
#ifdef _ENABLE_XORM_
		template<typename T, typename U>
		static std::enable_if_t<!reflector::is_reflect_class<typename std::remove_reference<T>::type>::value && (!xmart::is_fundamention_type<typename std::remove_reference<T>::type>::value && !xmart::is_date_type<typename std::remove_reference<T>::type>::value)> to_object_v(T& t, U&& v)
		{
			t = v.template get<T>();
		}

		template<typename T, typename U>
		static std::enable_if_t<!reflector::is_reflect_class<typename std::remove_reference<T>::type>::value&& xmart::is_fundamention_type<typename std::remove_reference<T>::type>::value && !xmart::is_date_type<typename std::remove_reference<T>::type>::value> to_object_v(T& t, U&& v)
		{
			if (!v.is_null()) {
				t = v.template get<typename T::value_type>();
			}
			else {
				t.clear();
			}
		}

		template<typename T, typename U>
		static std::enable_if_t<!reflector::is_reflect_class<typename std::remove_reference<T>::type>::value && !xmart::is_fundamention_type<typename std::remove_reference<T>::type>::value&& xmart::is_date_type<typename std::remove_reference<T>::type>::value> to_object_v(T& t, U&& v)
		{
			if (!v.is_null()) {
				t = v.template get<std::string>();
			}
			else {
				t.clear();
			}
		}
#else
		template<typename T, typename U>
		static std::enable_if_t<!reflector::is_reflect_class<std::remove_reference_t<T>>::value> to_object_v(T& t, U&& v)
		{
			t = v.template get<T>();
		}
#endif

		template<typename T, typename U = std::enable_if_t<reflector::is_reflect_class<typename std::remove_reference<T>::type>::value>>
		static void to_object(T && t, nlohmann::json const& json)
		{
			reflector::each_object(std::forward<T>(t), [&json](auto& t, auto& name, auto offset) {
				to_object_v((t.*offset), json[name]);
			});
		}
	};
	namespace serializer {
		template<typename T, typename U>
		inline nlohmann::json to_json(T&& obj, nlohmann::json& json)
		{
			serialize_::to_json(std::forward<T>(obj), json);
			return json;
		}
		template<typename T, typename U >
		inline nlohmann::json to_json(T&& obj)
		{
			nlohmann::json root;
			serialize_::to_json(std::forward<T>(obj), root);
			return root;
		}

		template<typename T, typename U >
		inline typename std::remove_reference<T>::type to_object(T&& t, nlohmann::json const& json)
		{
			serialize_::to_object(std::forward<T>(t), json);
			return std::forward<T>(t);
		}

		template<typename T, typename U >
		inline typename std::remove_reference<T>::type to_object(nlohmann::json const& json)
		{
			T t;
			serialize_::to_object(t, json);
			return t;
		}
	}
}