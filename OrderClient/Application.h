
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


#include <string>

#include <queue>

class Application : public FIX::Application, public FIX::MessageCracker
{
public:

  Application();

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
  void toApp( FIX::Message&, const FIX::SessionID& )
  throw( FIX::DoNotSend );
  void fromAdmin( const FIX::Message&, const FIX::SessionID& )
  throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon ) {}
  void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
  throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );

  void onMessage( const FIX42::ExecutionReport&, const FIX::SessionID& );
  void onMessage( const FIX42::OrderCancelReject&, const FIX::SessionID& );

  void queryEnterOrder();
  void queryCancelOrder();
  void queryReplaceOrder();

  FIX42::NewOrderSingle queryNewOrderSingle42();
  
  FIX42::OrderCancelRequest queryOrderCancelRequest42();

  FIX42::OrderCancelReplaceRequest queryCancelReplaceRequest42();

  void queryHeader( FIX::Header& header );
  char queryAction();

  FIX::Password Application::queryPassword();
  FIX::SenderCompID querySenderCompID();
  FIX::TargetCompID queryTargetCompID();
  FIX::TargetSubID queryTargetSubID();
  FIX::ClOrdID queryClOrdID();
  FIX::OrigClOrdID queryOrigClOrdID();
  FIX::Symbol querySymbol();
  FIX::Side querySide();
  FIX::OrderQty queryOrderQty();
  FIX::OrdType queryOrdType();
  FIX::Price queryPrice();
  FIX::StopPx queryStopPx();
  FIX::TimeInForce queryTimeInForce();

  int orderId_;
};

#endif
