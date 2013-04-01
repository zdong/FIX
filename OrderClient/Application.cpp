
#include "Application.h"

#include <quickfix/Session.h>

#include <iostream>
#include <sstream>

Application::Application()
	: orderId_(25) //hardcoding
{
}

void Application::onCreate( const FIX::SessionID& sessionId)
{
	sessionId_ = sessionId;
}

void Application::onLogon( const FIX::SessionID& sessionID )
{
  std::cout << std::endl << "Logon - " << sessionID << std::endl;
  sessionId_ = sessionID;
}

void Application::onLogout( const FIX::SessionID& sessionID )
{
  std::cout << std::endl << "Logout - " << sessionID << std::endl;
}

void Application::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
  crack( message, sessionID );
  std::cout << std::endl << "IN: " << message << std::endl;
}

void Application::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{
  try
  {
    FIX::PossDupFlag possDupFlag;
    message.getHeader().getField( possDupFlag );
    if ( possDupFlag ) throw FIX::DoNotSend();
  }
  catch ( FIX::FieldNotFound& ) {}

  std::cout << std::endl
  << "OUT: " << message << std::endl;
}

void Application::onMessage
( const FIX42::ExecutionReport&, const FIX::SessionID& ) {}
void Application::onMessage
( const FIX42::OrderCancelReject&, const FIX::SessionID& ) {}

void Application::run()
{
  while ( true )
  {
    try
    {
      char action = queryAction();

      if ( action == '1' )
        queryEnterOrder();
      else if ( action == '2' )
        queryCancelOrder();
      else if ( action == '3' )
        queryReplaceOrder();
      else if ( action == '5' )
        break;
    }
    catch ( std::exception & e )
    {
      std::cout << "Message Not Sent: " << e.what();
    }
  }
}

void Application::queryEnterOrder()
{
  std::cout << "\nNewOrderSingle\n";
  FIX::Message order = queryNewOrderSingle42();
  
  bool sent = FIX::Session::sendToTarget( order, sessionId_ );
  std::cout<<sent<<std::endl;
}

void Application::queryCancelOrder()
{
  std::cout << "\nOrderCancelRequest\n";
  
  FIX::Message cancel = queryOrderCancelRequest42();

  FIX::Session::sendToTarget( cancel );
}

void Application::queryReplaceOrder()
{ 
  std::cout << "\nCancelReplaceRequest\n";
  FIX::Message replace = queryCancelReplaceRequest42();
  
  FIX::Session::sendToTarget( replace );
}


FIX42::NewOrderSingle Application::queryNewOrderSingle42()
{
  FIX::OrdType ordType;

  FIX42::NewOrderSingle newOrderSingle(
    queryClOrdID(), FIX::HandlInst( '1' ), querySymbol(), querySide(),
    FIX::TransactTime(), ordType = queryOrdType() );

  newOrderSingle.set( queryOrderQty() );
  newOrderSingle.set( queryTimeInForce() );
  if ( ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT )
    newOrderSingle.set( queryPrice() );
  if ( ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT )
    newOrderSingle.set( queryStopPx() );

  queryHeader( newOrderSingle.getHeader() );

  newOrderSingle.setField(207, "CME");
  newOrderSingle.setField(167, "FUT");
  //newOrderSingle.setField(48, "ESH3");
  newOrderSingle.setField(200, "201306");
  newOrderSingle.setField(FIX::Account("chiu"));
  newOrderSingle.removeField(60);

  std::cout << newOrderSingle.toString() << std::endl;

  return newOrderSingle;
}

FIX42::OrderCancelRequest Application::queryOrderCancelRequest42()
{
  FIX42::OrderCancelRequest orderCancelRequest( queryOrigClOrdID(),
      queryClOrdID(), querySymbol(), querySide(), FIX::TransactTime() );

  orderCancelRequest.set( queryOrderQty() );
  queryHeader( orderCancelRequest.getHeader() );
  return orderCancelRequest;
}

FIX42::OrderCancelReplaceRequest Application::queryCancelReplaceRequest42()
{
  FIX42::OrderCancelReplaceRequest cancelReplaceRequest(
    queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst( '1' ),
    querySymbol(), querySide(), FIX::TransactTime(), queryOrdType() );

    cancelReplaceRequest.set( queryPrice() );
 
    cancelReplaceRequest.set( queryOrderQty() );

  queryHeader( cancelReplaceRequest.getHeader() );
  return cancelReplaceRequest;
}

void Application::queryHeader( FIX::Header& header )
{
  header.setField( querySenderCompID() );
  header.setField( queryTargetCompID() );
  
}

char Application::queryAction()
{
  char value;
  std::cout << std::endl
  << "1) Enter Order" << std::endl
  << "2) Cancel Order" << std::endl
  << "3) Replace Order" << std::endl
  << "4) Market data test" << std::endl
  << "5) Quit" << std::endl
  << "Action: ";
  std::cin >> value;
  switch ( value )
  {
    case '1': case '2': case '3': case '4': case '5': break;
    default: throw std::exception();
  }
  return value;
}


FIX::SenderCompID Application::querySenderCompID()
{
  std::string value = "OURCOMPID";
  return FIX::SenderCompID( value );
}

FIX::Password Application::queryPassword()
{
  std::string value = "YOURPASSWD";
  return FIX::Password( value );
}


FIX::TargetCompID Application::queryTargetCompID()
{
  std::string value = "YOURVALUE";
  return FIX::TargetCompID( value );
}


FIX::TargetSubID Application::queryTargetSubID()
{

  std::string value = "";
  
 

  return FIX::TargetSubID( value );
}


FIX::ClOrdID Application::queryClOrdID()
{
	orderId_++;

	std::stringstream s;
	s << orderId_;

    return FIX::ClOrdID(s.str());
}

FIX::OrigClOrdID Application::queryOrigClOrdID()
{
  std::string value = "";
  
  return FIX::OrigClOrdID( value );
}


FIX::Symbol Application::querySymbol()
{
  std::string value = "ES";
  
  return FIX::Symbol(value);
}


FIX::Side Application::querySide()
{

	return FIX::Side( FIX::Side_BUY );
}

FIX::OrderQty Application::queryOrderQty()
{
  long value = 10;
  return FIX::OrderQty( value );
}

FIX::OrdType Application::queryOrdType()
{
	return FIX::OrdType( FIX::OrdType_MARKET );
}

FIX::Price Application::queryPrice()
{
  double value = 123.3;

  return FIX::Price( value );
}

FIX::StopPx Application::queryStopPx()
{

  double value = 1000.0;
  
  return FIX::StopPx( value );
}

FIX::TimeInForce Application::queryTimeInForce()
{
	return FIX::TimeInForce( FIX::TimeInForce_DAY );
}
