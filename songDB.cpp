#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>
#include "songDB.hpp"
#include "server.h"
using namespace Player;

const std::string songDB::songsDir = "songs/";
const std::string songDB::coversDir = "covers/";
const std::string songDB::streamingPort = "8090";

songDB::songDB()
{
	this->vlcInstance = libvlc_new(0, NULL);
	try
	{
		Ice::CommunicatorPtr ic = Ice::initialize();
		Ice::ObjectPrx obectPrx = ic->stringToProxy("IceStorm/TopicManager:tcp -h datdroplet.ovh -p 9999");
		IceStorm::TopicManagerPrx topicManager = IceStorm::TopicManagerPrx::checkedCast(obectPrx);
		IceStorm::TopicPrx topic;

		while(!topic)
		{
			try
			{
				topic = topicManager->retrieve("DatMusic");
				std::cout << "Retrieving Topic" << std::endl;
			}
			catch(const IceStorm::NoSuchTopic&)
			{
				std::cout << "Topic not found" << std::endl;
				try
				{
					topic = topicManager->create("DatMusic");
					std::cout << "Topic created" << std::endl;
				}
				catch(const IceStorm::TopicExists&)
				{
					std::cout << "Topic existing" << std::endl;
				}
			}
		}
		std::cout << "Topic active" << std::endl;
		Ice::ObjectPrx publisher = topic->getPublisher()->ice_twoway();
		monitor = MonitorPrx::uncheckedCast(publisher);
		std::cout << "Monitor active" << std::endl;

		monitor->serverUp();
	}
	catch(const Ice::Exception& e)
	{
		std::cout << e << std::endl;
	}
}

songDB::~songDB()
{
	libvlc_vlm_release(this->vlcInstance);
	monitor->serverDown();
}

std::string songDB::generateId(std::string path, std::string ipAdress, std::string port)
{
	std::string result = ipAdress + port + path;
	size_t pos;
	while((pos = result.find(':')) != std::string::npos)
	{
		result.erase(pos, 1);
	}
	return result;
}

void songDB::addSong(const std::string& name, const std::string& artist, const std::string& path, const Ice::Current&)
{
	std::cout << "Adding song" << std::endl;

	song s;
	s.name = name;
	s.artist = artist;
	s.path = path;
	songTab.push_back(s);

	std::cout << name << " by " << artist << " was successfully added" << std::endl;
	monitor->newSong(s);
}

void songDB::remove(const std::string& path, const Ice::Current&)
{
	std::cout << "Deleting Song" << std::endl;

	if(songTab.empty())
		return;

	std::vector<song>::iterator it;
	for(it = songTab.begin(); it != songTab.end(); ++it)
	{
		if(it->path.compare(path) == 0)
		{
			monitor->songRemoved(*it);
			it = songTab.erase(it);
		}
		if(it == songTab.end())
			return;
	}
	if(unlink(handleDirs(path).c_str()) != 0);
		std::perror(("System Error while removing : " + handleDirs(path)).c_str());
	std::cout << path << " was successfully removed" << std::endl;
}

std::string songDB::lowerCase(std::string original)
{
	std::transform(original.begin(), original.end(), original.begin(), ::tolower);
	return original;
}

Player::songSeq songDB::findByAny(const std::string& searchKey, const Ice::Current&)
{
	std::cout << "Searching for " << searchKey << " in everything"<< std::endl;

	std::vector<Player::song> returnVal;
	for(std::vector<song>::iterator it = songTab.begin(); it != songTab.end(); ++it)
	{
		if(lowerCase(it->name).find(lowerCase(searchKey)) != std::string::npos
			|| lowerCase(it->artist).find(lowerCase(searchKey)) != std::string::npos)
			returnVal.push_back(*it);
	}
	return returnVal;
}

Player::song songDB::findByBoth(const std::string& title, const std::string& artist, const Ice::Current&)
{
	std::cout << "Searching for " << title << " in title and " << artist << "in artist" << std::endl;

	for(std::vector<song>::iterator it = songTab.begin(); it != songTab.end(); ++it)
	{
		if(lowerCase(it->name) == lowerCase(title) && lowerCase(it->artist) == lowerCase(title))
			return (*it);
	}
}

Player::songSeq songDB::findByTitle(const std::string& title, const Ice::Current&)
{
	std::cout << "Searching for " << title << " in titles"<< std::endl;

	std::vector<Player::song> returnVal;
	for(std::vector<song>::iterator it = songTab.begin(); it != songTab.end(); ++it)
	{
		if(lowerCase(it->name).find(lowerCase(title)) != std::string::npos)
			returnVal.push_back(*it);
	}
	return returnVal;
}

Player::songSeq songDB::findByArtist(const std::string& artist, const Ice::Current&)
{
	std::cout << "Searching for " << artist << " in artists'"<< std::endl;

	std::vector<Player::song> returnVal;
	for(std::vector<song>::iterator it = songTab.begin(); it != songTab.end(); ++it)
	{
		if(lowerCase(it->artist).find(lowerCase(artist)) != std::string::npos)
			returnVal.push_back(*it);
	}
	return returnVal;
}

Player::songSeq songDB::list(const Ice::Current&)
{
	std::cout << "Listing all songs" << std::endl;
	return songTab;
}

std::string songDB::start(const std::string& path, const Ice::Current& c)
{
	std::cout << "Starting " + path << std::endl;
	std::string completePath = songsDir + path;

	Ice::IPConnectionInfo* ipConInfo = dynamic_cast<Ice::IPConnectionInfo*>(c.con->getInfo().get());
	std::string ipAdress = ipConInfo->remoteAddress;
	std::string port = std::to_string(ipConInfo->remotePort);

	std::string id = generateId(path, ipAdress, port);
	std::string sout = "#transcode{acodec=mp3,ab=128,channels=2,"
				"samplerate=44100}:http{dst=:" + streamingPort + "/" + id + "}";

	libvlc_vlm_add_broadcast(vlcInstance, id.c_str(), completePath.c_str(), sout.c_str(), 0, NULL, true, false);

	std::cout << "ID : " + id << std::endl;

	return id;
}

void songDB::play(const std::string& id, const Ice::Current&)
{
	std::cout << id + " playing" << std::endl;

	libvlc_vlm_play_media(vlcInstance, id.c_str());
}

void songDB::stop(const std::string& id, const Ice::Current&)
{
	std::cout << id + " stopping" << std::endl;

	libvlc_vlm_stop_media(vlcInstance, id.c_str());
}

std::string songDB::handleDirs(std::string filename)
{
	std::string ext = filename.substr(filename.rfind('.'), (filename.size() - filename.rfind('.')));
	if(ext.compare(".mp3") == 0)
		return songsDir + filename;
	return coversDir + filename;
}

void songDB::write(const std::string& filename, int offset, const ByteSeq& data, const Ice::Current& c)
{
	std::string name = handleDirs(filename);
	std::cout << "Writing " << data.size() << " bytes in " << name << std::endl;
	FILE * filePtr;
	filePtr = fopen(name.c_str(), "a+");
	fseek(filePtr, offset, SEEK_SET);
	fwrite(&data[0], 1, data.size(), filePtr);
	fclose(filePtr);
}

ByteSeq songDB::read(const std::string& filename, int offset, int count, const Ice::Current& c)
{
	ByteSeq data;
	std::string name = handleDirs(filename);
	FILE * filePtr;
	filePtr = fopen(name.c_str(), "r");
	fseek(filePtr, offset, SEEK_SET);
	fread(&data[0], 1, count, filePtr);
	std::cout << "Reading " << data.size() << " bytes from " << name << std::endl;
	return data;
}

int songDB::getCount(const Ice::Current& c)
{
	std::cout << "Giving number of songs in the DB : " << songTab.size() << std::endl;
	return songTab.size();
}

std::string songDB::getStreamingPort(const Ice::Current& c)
{
	return streamingPort;
}
