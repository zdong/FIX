
#include "MDApplication.h"

#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/FileLog.h>

#include <string>
#include <iostream>
#include <fstream>

//#include <src/getopt-repl.h>

int main( int argc, char** argv )
{
  if ( argc != 2 )
  {
    std::cout << "usage: " << argv[ 0 ]
    << " FILE." << std::endl;
    return 0;
  }
  std::string file = argv[ 1 ];

  try
  {
    FIX::SessionSettings settings( file );

    MDApplication application;
    FIX::FileStoreFactory storeFactory( settings );
	FIX::FileLogFactory logFactory(settings);
    FIX::SocketInitiator initiator( application, storeFactory, settings, logFactory );

    initiator.start();
    application.run();
    initiator.stop();

    return 0;
  }
  catch ( std::exception & e )
  {
    std::cout << e.what();
    return 1;
  }
}
