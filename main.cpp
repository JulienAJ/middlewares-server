#include <Ice/Ice.h>
#include <iostream>
#include "songDB.hpp"
using namespace std;

int main(int argc, char **argv)
{

	int status = 0;
	Ice::CommunicatorPtr ic;

	try
	{
		ic = Ice::initialize(argc, argv);
		Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints("ServerAdapter", "default -p 10000");
		Ice::ObjectPtr object = new songDB();
		adapter->add(object, ic->stringToIdentity("Server"));
		adapter->activate();
		ic->waitForShutdown();
	}
	catch (const Ice::Exception& e)
	{
		cerr << e << endl;
		status = 1;
	}
	catch (...)
	{
		status = 1;
	}

	if (ic)
	{
		try
		{
			ic->destroy();
		}
		catch (const Ice::Exception& e)
		{
			cerr << e << endl;
			status = 1;
		}
	}

	return status;
}
