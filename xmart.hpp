#pragma once
#include <xfinal/xfinal.hpp>
#include "utils/utils.hpp"
namespace xmart {
	inline http_server& init_xmart(std::string const& configPath, bool& r, std::function<void(std::string const&)> error_cb = nullptr) {
		static std::unique_ptr<http_server> server_;
		json config_json;
		std::ifstream file(configPath);
		std::stringstream ss;
		ss << file.rdbuf();
		auto config_str = ss.str();
		if (config_str.empty()) {
			r = false;
			return *server_;
		}
		else {
			try {
				config_json = json::parse(config_str);
				std::size_t thread_count = std::thread::hardware_concurrency();
				auto number = config_json.at("http_thread").get<std::size_t>();
				if (number > 0) {
					thread_count = number;
				}
				server_ = std::unique_ptr<http_server>(new http_server(thread_count));
				if (error_cb == nullptr) {
					server_->on_error([](std::string const& message) {
						std::cout << message << "\n";
						});
				}
				else {
					server_->on_error(std::move(error_cb));
				}
				auto port = config_json.at("http_port").get<std::string>();
				auto host = config_json.at("http_host").get<std::string>();
				server_->listen(host, port);
				auto read_timeout = config_json.at("http_read_timeout").get<int>();
				auto write_timeout = config_json.at("http_write_timeout").get<int>();
				auto keepalive_timeout = config_json.at("http_keepalive_timeout").get<int>();
				server_->set_wait_read_time((std::time_t)read_timeout);
				server_->set_wait_write_time((std::time_t)write_timeout);
				server_->set_keep_alive_wait_time(keepalive_timeout);
#ifdef _ENABLE_XORM_
				auto server_ptr = server_.get();
				dao_message::get().set_error_callback([server_ptr](std::string const& message) {
					server_ptr->trigger_error(message);
				});
				auto db_configs = config_json.at("db_configs");
				for (auto&& iter : db_configs) {
					dataBaseConfig config{};
					config.index_key = iter.at("db_index_key").get<std::string>();
					config.character_encoding = iter.at("db_character_encoding").get<std::string>();
					config.conn_number = iter.at("db_conn_number").get<std::size_t>();
					config.dbname = iter.at("db_name").get<std::string>();
					config.host = iter.at("db_host").get<std::string>();
					config.password = iter.at("db_pass").get<std::string>();
					config.port = iter.at("db_port").get<unsigned int>();
					config.reconnect_number = iter.at("db_reconnumber").get<std::size_t>();
					config.timeout = iter.at("db_timeout").get<int>();
					config.user = iter.at("db_user").get<std::string>();
#ifdef  XORM_ENABLE_MYSQL
					dao_t<xorm::mysql>::init_conn_pool(config);
#endif 
#ifdef  XORM_ENABLE_SQLITE
					dao_t<xorm::sqlite>::init_conn_pool(config);
#endif
				}
#endif 
				r = true;
			}
			catch (std::exception const& ec) {
				std::cout << ec.what() << "\n";
				r = false;
				return *server_;
			}
			return *server_;
		}

	}

}