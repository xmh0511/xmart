# xmart

## 特性
1. 易于使用的web库 (xfinal) [基本使用](https://github.com/xmh0511/xfinal)  
2. 易于使用的orm库 (xorm) [基本使用](https://github.com/xmh0511/xorm)    
3. 完整的处理工具  (utils)  

## 演示

````
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
        // 从配置文件初始化xmart
	http_server& server = init_xmart("./config.json", r); 
	if (!r) {
		return 0;
	}

        //从form表单写入数据库
	server.router<GET, POST>("/write", [](request& req, response& res) {  
		auto data = map_from_query<test>(req);
		dao_t<mysql> dao;
		dao.insert(data);
		res.write_string("hello,world");
	});
        //从url参数写入数据库
	server.router<GET, POST>("/writeparams", [](request& req, response& res) {  
		auto data = map_from_params<test>(req);
		dao_t<mysql> dao;
		dao.insert(data);
		res.write_string("hello,world");
	});

        //获取数据并通过json数据交互返回给客户端
	server.router<GET>("/list", [](request& req, response& res) {
		dao_t<mysql> dao;
		auto pr = dao.query<test>("where 1=1");
		json root;
		if (pr.first) {
			auto& vec = pr.second;
			for (auto& iter : vec) {
				root["list"].push_back(serializer::to_json(iter));
			}
		}
		root["success"] = true;
		res.write_json(root);
	});

	server.run();
}
````
