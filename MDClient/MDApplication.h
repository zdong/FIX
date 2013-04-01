#ifndef TRADECLIENT_APPLICATION_H
#define TRADECLIENT_APPLICATION_H

#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/Values.h>
#include <quickfix/Mutex.h>

#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/OrderCancelRequest.h>
#include <quickfix/fix42/OrderCancelReject.h>
#include <quickfix/fix42/OrderCancelReplaceRequest.h>
#include <quickfix/fix42/MarketDataRequest.h>

#include <string>
#include <queue>

class MDApplication : public FIX::Application,  public FIX::MessageCracker 
{
public:

  MDApplication();

  void run();

private:
  FIX::SessionID sessionId_;
	
  void onCreate( const FIX::SessionID& );
  void onLogon( const FIX::SessionID& sessionID );
  void onLogout( const FIX::SessionID& sessionID );
  void toAdmin( FIX::Message& message, const FIX::SessionID& sessionId) 
  {
	 //FIX::MsgType msgType;
     std::string msgType = message.getHeader().getField(35);
	 if(msgType == "A")
     {     
		 // add password
         message.setField(96, FIX::RawData("12345678"));
     }

	 std::cout << "Session ID " << sessionId << std::endl;
  }

  void toApp(FIX::Message&, const FIX::SessionID& ) throw( FIX::DoNotSend );
  
  void fromAdmin( const FIX::Message& message, const FIX::SessionID& ) throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
  {
	  //std::cout << "From Admin " << message << std::endl;
  }
  
  void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID ) throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );

  void onMessage( const FIX42::ExecutionReport&, const FIX::SessionID& );

  void onMessage( const FIX42::OrderCancelReject&, const FIX::SessionID& );

  void onMessage(const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& );

  void onMessage(const FIX42::MarketDataIncrementalRefresh& message, const FIX::SessionID& );

  void SendMarketDataRequest();

  void SendInstLookUp();

  char queryAction();

  int mdRequestId_;
};

#endif
