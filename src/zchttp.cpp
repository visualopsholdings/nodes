/*
  zchttp.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Jul-2024
    
  HTTP Daemon for ZMQChat.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <boost/program_options.hpp> 
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/json.hpp>
#include <zmq.hpp>
#include <restinio/core.hpp>
#include <restinio/router/express.hpp>

namespace po = boost::program_options;

using namespace std;
using json = boost::json::value;
namespace rr = restinio::router;
using router_t = restinio::router::express_router_t<>;

class Server {

public:
  Server(const string &reqConn);
    
  void run(int httpPort);
  auto server_handler();
  void send(const json &json);
  json receive();
  
private:
  zmq::context_t _context;
  zmq::socket_t _req;

};

class Handler {

public:
  Handler(Server *server): _server(server) {}
  
	auto users(const restinio::request_handle_t& req, rr::route_params_t ) const;

private:

  Server *_server;
  
	template < typename RESP >
	static RESP
	init_resp( RESP resp )
	{
		resp
			.append_header("Server", "ZMQChat Server /v.0.1")
			.append_header_date_field()
			.append_header("Content-Type", "application/json; charset=utf-8");

		return resp;
	}

};

auto Handler::users(
  const restinio::request_handle_t& req, rr::route_params_t ) const
{
  auto resp = init_resp( req->create_response() );

  _server->send({ { "type", "users" } });
  json j = _server->receive();
  stringstream ss;
  ss << j;
  resp.set_body(ss.str());

  return resp.done();
}

auto Server::server_handler()
{
  auto router = std::make_unique< router_t >();
	auto handler = std::make_shared<Handler>(this);

	auto by = [&]( auto method ) {
		using namespace std::placeholders;
		return std::bind( method, handler, _1, _2 );
	};

  router->http_get("/users", by(&Handler::users));

  return router;
}

Server::Server(const string &reqConn): 
    _context(1), _req(_context, ZMQ_REQ) {
    
  _req.connect(reqConn);

}

void Server::run(int httpPort) {

  using traits_t =
      restinio::traits_t<
         restinio::asio_timer_manager_t,
         restinio::single_threaded_ostream_logger_t,
         router_t >;

  restinio::run(
    restinio::on_this_thread<traits_t>()
      .port(httpPort).address("localhost")
      .request_handler(server_handler())
  );
}

void Server::send(const json &json) {

  stringstream ss;
  ss << json;
  string m = ss.str();
  zmq::message_t msg(m.length());
  memcpy(msg.data(), m.c_str(), m.length());
  _req.send(msg);

}

json Server::receive() {

  zmq::message_t reply;
  _req.recv(&reply);
  string r((const char *)reply.data(), reply.size());
  return boost::json::parse(r);

}

int main(int argc, char *argv[]) {

  int httpPort;
  int reqPort;
  string logLevel;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("httpPort", po::value<int>(&httpPort)->default_value(4000), "HTTP port.")
    ("reqPort", po::value<int>(&reqPort)->default_value(3013), "ZMQ Req port.")
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("help", "produce help message")
    ;
  po::positional_options_description p;

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
  po::notify(vm);   

  if (logLevel == "trace") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
  }
  else if (logLevel == "debug") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  }
  else if (logLevel == "warn") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
  }
  else {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
  }

  boost::log::formatter logFmt =
         boost::log::expressions::format("%1%\tzchttp [%2%]\t%3%")
        %  boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") 
        %  boost::log::expressions::attr< boost::log::trivial::severity_level>("Severity")
        %  boost::log::expressions::smessage;
  boost::log::add_common_attributes();
  boost::log::add_console_log(clog)->set_formatter(logFmt);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  
  Server server("tcp://127.0.0.1:" + to_string(reqPort));
  server.run(httpPort);
  
}
