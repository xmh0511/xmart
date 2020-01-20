#include "xmart.hpp"
using namespace xmart;

struct test {
	mysql::Integer id;
	mysql::Integer a;
	std::string b;
	mysql::MysqlDateTime time;
	mysql::MysqlDate date;
	mysql::MysqlTime tm;
	mysql::Double money;
};
REFLECTION(test, id, a, b, time, date, tm, money)

int main() {
	bool r = false;
	http_server& server = init_xmart("./config.json", r);
	if (!r) {
		std::cout<<"config has some error"<<std::endl;
		return 0;
	}

	server.on_error([](std::string const& ec) {
		std::cout << ec << std::endl;
	});

	server.router<GET, POST>("/write", [](request& req, response& res) {
		auto data = map_from_query<test>(req);
		dao_t<mysql> dao;
		dao.insert(data);
		res.write_string("form insert");
	});

	server.router<GET, POST>("/writeparams", [](request& req, response& res) {
		auto data = map_from_params<test>(req);
		dao_t<mysql> dao;
		dao.insert(data);
		res.write_string("params insert");
	});

	server.router<GET, POST>("/writejson", [](request& req, response& res) {
		auto json_str = req.body();
		auto json = json::parse(json_str);
		try {
			auto data = map_from_json<test>(json);
			dao_t<mysql> dao;
			dao.insert(data);
		}
		catch (std::exception const& ec) {
			std::cout << ec.what() << std::endl;
		}
		res.write_string("json insert");
	});

	server.router<GET>("/list", [](request& req, response& res) {
		dao_t<mysql> dao;
		auto pr = dao.query<test>("where 1=1");
		json root;
		if (pr.first) {
			auto& vec = pr.second;
			root["list"] = list_to_json(vec);
		}
		root["success"] = true;
		res.write_json(root);
	});

	server.run();
}