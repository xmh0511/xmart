#pragma once
#include "utils/utils.hpp"
namespace xmart {
	inline http_server& init_xmart(std::string const& configPath, bool& r) {
		bool orm_result = true;
		bool config_result = true;
		json config_json;
		std::ifstream file(configPath);
		std::stringstream ss;
		ss << file.rdbuf();
		auto config_str = ss.str();
		if (config_str.empty()) {
#ifdef _ENABLE_XORM_
			orm_result = false;
#endif 
			config_result = false;
		}
		else {
			try {
				config_json = json::parse(config_str);
#ifdef _ENABLE_XORM_
				dataBaseConfig config{};
				config.character_encoding = config_json.at("db_character_encoding").get<std::string>();
				config.conn_number = config_json.at("db_conn_number").get<std::size_t>();
				config.dbname = config_json.at("db_name").get<std::string>();
				config.host = config_json.at("db_host").get<std::string>();
				config.password = config_json.at("db_pass").get<std::string>();
				config.port = config_json.at("db_port").get<unsigned int>();
				config.reconnect_number = config_json.at("db_reconnumber").get<std::size_t>();
				config.timeout = config_json.at("db_timeout").get<int>();
				config.user = config_json.at("db_user").get<std::string>();
				init_database_config(config);
				orm_result = true;
#endif 
			}
			catch (std::exception const& ec) {
				std::cout << ec.what() << std::endl;
#ifdef _ENABLE_XORM_
				orm_result = false;
#endif 
			}
		}
		std::size_t thread_count = std::thread::hardware_concurrency();
		if (config_result) {
			auto number = config_json.at("http_thread").get<std::size_t>();
			if (number > 0) {
				thread_count = number;
			}
		}
		static http_server server(thread_count);
		if (config_result) {
			auto port = config_json.at("http_port").get<std::string>();
			auto host = config_json.at("http_host").get<std::string>();
			server.listen(host, port);
		}
		r = config_result && orm_result;
		return server;
	}

}