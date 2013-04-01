#include "Application.h"

#include <quickfix/Session.h>
#include <quickfix/fix42/MarketDataIncrementalRefresh.h>
#include <quickfix/fix42/MarketDataSnapshotFullRefresh.h>

#include <iostream>
#include <sstream>

Application::Application()
	: mdRequestId_(3)
{
}

void Application::onCreate( const FIX::SessionID& sessionId)
{
	sessionId_ = sessionId;
}

void Application::onLogon( const FIX::SessionID& sessionID )
{
  std::cout << "onLogon - " << sessionID << std::endl;
}

void Application::onLogout( const FIX::SessionID& sessionID )
{
  std::cout << "onLogout - " << sessionID << std::endl;
}

void Application::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
	std::cout << std::endl << "IN: " << message << std::endl;

	std::cout << message << std::endl;

	crack( message, sessionID );
  
}


void Application::toApp( FIX::Message& message, const FIX::SessionID& sessionID ) throw( FIX::DoNotSend )
{
	try
	{
		std::cout << "OUT: " << message << std::endl; 

		FIX::PossDupFlag possDupFlag;
		message.getHeader().getField( possDupFlag );
    
		if (possDupFlag) 
		{
			std::cout << "PossDupFlag" << std::endl;
		
			throw FIX::DoNotSend();
		}
		else 
		{
			std::cout << "OUT: " << message << std::endl; 
		}
	}
	catch ( FIX::FieldNotFound& ) 
	{
		std::cout << "Field Not found " << std::endl;
	}  

}

void Application::onMessage(const FIX42::ExecutionReport&, const FIX::SessionID&) 
{
	try 
	{
		std::cout << "onMessage : exe report" << std::endl;
	}
	catch(...)
	{
		std::cout << "onMessage : exe report unhandled exception" << std::endl;
	}
}

void Application::onMessage(const FIX42::OrderCancelReject&, const FIX::SessionID&) 
{
	try 
	{
		std::cout << "onMessage : cancel report" << std::endl;
	}
	catch(...)
	{
		std::cout << "onMessage : cancel report unhandled exception" << std::endl;
	}
}


void Application::onMessage(const FIX42::MarketDataSnapshotFullRefresh& message, const FIX::SessionID& )
{
	
	FIX::NoMDEntries noMDEntries;
	message.get(noMDEntries);
	FIX::MDReqID mdReqID;
	message.get(mdReqID);

	std::cout << "NoMDEntries: " << noMDEntries << ": " << mdReqID << std::endl;

	for (int idx=1; idx<=noMDEntries; ++idx)
	{

		FIX42::MarketDataSnapshotFullRefresh::NoMDEntries group;
		FIX::MDEntryType MDEntryType;
		FIX::MDEntryPx MDEntryPx;
		FIX::MDEntrySize MDEntrySize;

		message.getGroup(idx, group);
		group.get(MDEntryType);
		group.get(MDEntryPx);
		group.get(MDEntrySize);

		std::cout << std::endl << std::endl;

		std::cout << "Group " << idx << " "  << MDEntryType << "," << MDEntryPx << "," << MDEntrySize << std::endl; 
	}
}

void Application::onMessage(const FIX42::MarketDataIncrementalRefresh& message, const FIX::SessionID& )
{
	
	FIX::NoMDEntries noMDEntries;
	message.get(noMDEntries);
	FIX::MDReqID mdReqID;
	message.get(mdReqID);

	std::cout << "NoMDEntries: " << noMDEntries << ": " << mdReqID << std::endl;

	if (noMDEntries >=1 )
	{

		FIX42::MarketDataIncrementalRefresh::NoMDEntries group;
		FIX::MDEntryType MDEntryType;
		FIX::MDEntryPx MDEntryPx;
		FIX::MDEntrySize MDEntrySize;

		message.getGroup(1, group);
		group.get(MDEntryType);
		group.get(MDEntryPx);
		group.get(MDEntrySize);

		std::cout << std::endl << std::endl;

		std::cout << "Group 1" << MDEntryType << "," << MDEntryPx << "," << MDEntrySize << std::endl; 
	}

	std::cout << "IncrementatlRefesh :"  << std::endl;
}


void Application::run()
{
	bool keepRunning = true;
	
	while (keepRunning)
	{
		try
		{
			char action = queryAction();

			switch(action)
			{
			  case '1':
				  SendMarketDataRequest();
				break;

			  case '2':
				break;

			  case '5':
				keepRunning = false;
				break;
			}
		}
		catch (std::exception& e)
		{
			std::cout << "Message Not Sent: " << e.what();
		}
	}
}

void Application::SendMarketDataRequest()
{
	std::stringstream s;
	s << mdRequestId_ << std::ends;

	mdRequestId_ ++;

	FIX::MDReqID mdReqID(s.str());

	//FIX::SubscriptionRequestType subType(FIX::SubscriptionRequestType_SNAPSHOT);
	FIX::SubscriptionRequestType subType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
	FIX::MarketDepth marketDepth(0);

	FIX42::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
	FIX::MDEntryType mdEntryType(FIX::MDEntryType_BID);
	marketDataEntryGroup.set(mdEntryType);

	FIX42::MarketDataRequest::NoRelatedSym symbolGroup;
	FIX::Symbol symbol("ES");
	symbolGroup.set(symbol);
	
	symbolGroup.setField(207, "CME");
    symbolGroup.setField(167, "FUT");
    symbolGroup.setField(200, "201303");

	FIX42::MarketDataRequest message( mdReqID, subType, marketDepth );
	message.addGroup( marketDataEntryGroup );
	message.addGroup( symbolGroup );

	FIX::Header& header=message.getHeader();
	header.setField(FIX::SenderCompID("YOURVALUE"));
	header.setField(FIX::TargetCompID("YOURVALUE"));

	message.setField(265, "1");
	message.setField(266, "Y");

	FIX::Session::sendToTarget(message, sessionId_);

}


char Application::queryAction()
{
  char value;
  std::cout << std::endl
  << "1) Subscribe for Market Data" << std::endl
  << "2) Unsubscribe" << std::endl
  << "3) " << std::endl
  << "4) " << std::endl
  << "5) Quit" << std::endl
  << "Action: ";
  
  std::cin >> value;
  
  return value;
}


