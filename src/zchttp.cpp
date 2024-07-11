/*
  zchttp.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Jul-2024
    
  HTTP Daemon for ZMQChat.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "json.hpp"
#include "cookie.hpp"
#include "session.hpp"

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
  auto handler();
  void send(const json &json);
  json receive();
  
	auto getroot(const restinio::request_handle_t& req, rr::route_params_t );
	auto getfonts(const restinio::request_handle_t& req, rr::route_params_t );
	auto getusers(const restinio::request_handle_t& req, rr::route_params_t );
	auto getlogin(const restinio::request_handle_t& req, rr::route_params_t );
	auto postlogin(const restinio::request_handle_t& req, rr::route_params_t );

private:
  zmq::context_t _context;
  zmq::socket_t _req;

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

  optional<string> finishlogin(const string &password);
  auto unauthorised(const restinio::request_handle_t& req);

};

auto Server::getroot(
  const restinio::request_handle_t& req, rr::route_params_t params)
{
  auto resp = req->create_response(restinio::status_found());
	resp.append_header("Location", "/apps/login/#/");
  return resp.done();
  
}

auto Server::getfonts(
  const restinio::request_handle_t& req, rr::route_params_t params)
{
  const auto file = restinio::cast_to<string>( params[ "file" ] );
//  BOOST_LOG_TRIVIAL(trace) << "font " << file;

  auto resp = req->create_response();
  resp.set_body(restinio::sendfile("../frontend/fonts/" + file));
  
  return resp.done();
}

auto Server::unauthorised(const restinio::request_handle_t& req) {

  auto resp = init_resp(req->create_response(restinio::status_unauthorized()));
  json err = {
    { "status", 401 },
    { "err", "Username/Password incorrect" }
  };
  stringstream ss;
  ss << err;
  resp.set_body(ss.str());
  return resp.done();
}

auto Server::getusers(
  const restinio::request_handle_t& req, rr::route_params_t )
{
  if (!req->header().has_field("Cookie")) {
    BOOST_LOG_TRIVIAL(trace) << "no Cookie";  
    return unauthorised(req);
  }
  auto cookie = req->header().get_field("Cookie");
  auto id = Cookie::parseCookie(cookie);
  if (!id) {
    BOOST_LOG_TRIVIAL(trace) << "couldn't find id in cookie " << cookie;  
    return unauthorised(req);
  }
  if (!Sessions::instance()->has(id.value())) {
    BOOST_LOG_TRIVIAL(trace) << "couldn't find session id " << id.value();  
    return unauthorised(req);
  }
  auto session = Sessions::instance()->get(id.value());
  if (!session->isAdmin()) {
    BOOST_LOG_TRIVIAL(trace) << "user is not admin " << session->userid();  
    return unauthorised(req);
  }
  
  send({ { "type", "users" } });
  json j = receive();
  auto users = Json::getArray(j, "users");
  
  if (!users) {
    // send fatal error
    BOOST_LOG_TRIVIAL(error) << "users missing users ";
    return init_resp(req->create_response(restinio::status_internal_server_error())).done();
  }

  auto resp = init_resp( req->create_response() );

  stringstream ss;
  ss << users.value();
  resp.set_body(ss.str());

  return resp.done();
}

optional<string> Server::finishlogin(const string &password) {

  send({
    { "type", "login" },
    { "session", "1" },
    { "password", password }
  });
  json j = receive();
  BOOST_LOG_TRIVIAL(trace) << "login returned " << j;
  
  auto type = Json::getString(j, "type");
  if (!type) {
    BOOST_LOG_TRIVIAL(error) << "missing type in return";
    return nullopt;
  }
  
  if (type.value() == "err") {
    BOOST_LOG_TRIVIAL(error) << Json::getString(j, "msg").value();
    return nullopt;
  }
  auto id = Json::getString(j, "id");
  if (!id) {
    BOOST_LOG_TRIVIAL(error) << "missing id in return";
    return nullopt;
  }
  auto admin = Json::getBool(j, "admin");
  if (!admin) {
    BOOST_LOG_TRIVIAL(error) << "missing admin in return";
    return nullopt;
  }

  return Sessions::instance()->create(id.value(), admin.value());
  
}

auto Server::getlogin(
  const restinio::request_handle_t& req, rr::route_params_t params)
{
  BOOST_LOG_TRIVIAL(trace) << "GET /login";
  
  const auto qp = restinio::parse_query(req->header().query());
  if (qp.has("username")) {
    const auto username = restinio::cast_to<string>(qp["username"]);
    auto id = finishlogin(username);
    if (!id) {
      BOOST_LOG_TRIVIAL(trace) << "no id generated";  
      return unauthorised(req);
    }
    BOOST_LOG_TRIVIAL(trace) << "sending response with cookie " << id.value();  
    auto resp = req->create_response(restinio::status_found());
	  resp.append_header("Location", "/#/");
	  resp.append_header("Set-Cookie", "ss-id=" + id.value() + "; Path=/; Secure; HttpOnly");
	  resp.append_header("Cache-Control", "nocache");
    return resp.done();
  }

  auto resp = req->create_response(restinio::status_found());
	resp.append_header("Location", "/#/");
  return resp.done();
}

auto Server::postlogin(
  const restinio::request_handle_t& req, rr::route_params_t )
{
  json j = boost::json::parse(req->body());
  BOOST_LOG_TRIVIAL(trace) << "postlogin " << j;

  auto password = Json::getString(j, "password");
  if (!password) {
    BOOST_LOG_TRIVIAL(error) << "missing password";
    return unauthorised(req);
  }

  auto id = finishlogin(password.value());
  if (!id) {
    return unauthorised(req);
  }

  auto resp = req->create_response();
	resp.append_header("Set-Cookie", "ss-id=" + id.value() + "; Path=/; Secure; HttpOnly");
  resp.append_header("Cache-Control", "nocache");
  resp.set_body("{}");
  return resp.done();

}

auto Server::handler()
{
  auto router = std::make_unique< router_t >();

	auto by = [&]( auto method ) {
		using namespace std::placeholders;
		return std::bind( method, this, _1, _2 );
	};

  router->http_get("", by(&Server::getroot));
  router->http_get("/", by(&Server::getroot));
  router->http_get("/fonts/:file", by(&Server::getfonts));
  router->http_get("/login", by(&Server::getlogin));
  router->http_post("/login", by(&Server::postlogin));
  router->http_get("/rest/1.0/users", by(&Server::getusers));

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
      .request_handler(handler())
  );
}

void Server::send(const json &json) {

  stringstream ss;
  ss << json;
  string m = ss.str();
  zmq::message_t msg(m.length());
  memcpy(msg.data(), m.c_str(), m.length());
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
  _req.send(msg);
#else
  _req.send(msg, zmq::send_flags::none);
#endif

}

json Server::receive() {

  zmq::message_t reply;
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
  _req.recv(&reply);
#else
  auto res = _req.recv(reply, zmq::recv_flags::none);
#endif
  string r((const char *)reply.data(), reply.size());
  return boost::json::parse(r);

}

int main(int argc, char *argv[]) {

  int httpPort;
  int reqPort;
  string logLevel;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("httpPort", po::value<int>(&httpPort)->default_value(3000), "HTTP port.")
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
