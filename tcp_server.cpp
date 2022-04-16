//
// NOTES:
// ibboost-all-dev/focal,now 1.71.0.0ubuntu2 amd64 [installed]
// // derive a working proof of concept. First blocking then async.
// updated system to prep for boost install; installed boost
// based work off the server and client at: "https://www.bogotobogo.com/cplusplus/sockets_server_client.php"
// code from: "https://www.bogotobogo.com/cplusplus/Boost/boost_AsynchIO_asio_tcpip_socket_server_client_timer_bind_handler_multithreading_synchronizing_network_D.php"
//
//
//
#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

#if 1
using boost::asio::ip::tcp;

std::string make_daytime_string()
{
  std::time_t now = std::time(0);
  return std::ctime(&now);
  //return std::ctime(&now;);
}

int main()
{
  try
  {
    // Any program that uses asio need to have at least one io_service object
    boost::asio::io_service io_service;

    // acceptor object needs to be created to listen for new connections
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 13));

    for (;;)
    {
      // creates a socket
      tcp::socket socket(io_service);

      // wait and listen
      acceptor.accept(socket);

      // prepare message to send back to client
      std::string message = make_daytime_string();

      boost::system::error_code ignored_error;

      // writing the message for current time
      boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
#else
int main()
{
	return 1;
}
#endif
