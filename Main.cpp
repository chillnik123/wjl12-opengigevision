#include <iostream>
#include <exception>
#include <asio.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <vigra/stdimage.hxx>
#include <vigra/impex.hxx>

#include "Gvsp.h"
#include "Gvcp.h"


int main(int argc, char** argv)
{
  try
  {
    if(argc != 3)
      throw std::runtime_error(str(boost::format("Usage: %1% <ip> <port>") % argv[0]));

    asio::io_service service;

    GvcpManager gvcp(service, argv[1], argv[2]);
    std::cout << "Cam Addr: " << gvcp.FindCam().to_string() << std::endl;

    // format bool values as strings
    std::cout.setf (std::ios::boolalpha);

    std::cout << "Access acquired: "
              << gvcp.Write(ACCESS_CAM, ACCESS_CAM_VALUE) << std::endl;
    std::cout << "Set stream port: "
              << gvcp.Write(DATASTREAM_PORT, 12221) << std::endl;
    std::cout << "Set stream addr: "
              << gvcp.Write(DATASTREAM_ADDRESS,
                       asio::ip::address_v4::from_string("169.254.0.2").to_ulong()) << std::endl;
    std::cout << "Set pixel format: " << gvcp.Write(PIXEL_FORMAT, PIXEL_FORMAT_8BIT_VALUE) << std::endl;
    std::cout << "Pixel format: " << (gvcp.Read(PIXEL_FORMAT) == PIXEL_FORMAT_8BIT_VALUE ?
                                      "8bit" : "12bit") << std::endl;
    std::cout << "Start: "
              << gvcp.Write(START_GRAB, 0x0) << std::endl;

    gvcp.StartHeartbeat();

    // start data thread
    GvspManager gvsp(service, 12221);

    sleep(10);
    gvsp.Stop();
    gvcp.StopHeartbeat();

    std::cout << "Stop: " << gvcp.Write(STOP_GRAB, STOP_GRAB_VALUE) << std::endl;

    std::cout << "Waiting..." << std::flush;
    while(gvcp.Read(GRABBING_STOPPED) != STOP_GRAB_VALUE)
      std::cout << "." << std::flush;
    std::cout << " stopped" << std::endl;
  }
  catch(std::exception& e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}


