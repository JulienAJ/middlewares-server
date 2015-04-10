#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <Ice/Ice.h>
#include "songDB.hpp"
#include "server.h"
using namespace Player;

songDB::songDB()
{
	this->vlcInstance = libvlc_new(0, NULL);
}

songDB::~songDB()
{
	libvlc_vlm_release(this->vlcInstance);
}

void songDB::addSong(song s)
{
	songTab.push_back(s);
}

std::vector<song> songDB::search(std::vector<std::string> args, std::vector<std::string> values)
{
	std::vector<song> returnVal;

	std::vector<std::string>::size_type size = args.size();
	bool nameSearch = false, artistSearch = false;
	std::string name, artist;
	for(size_t i = 0; i < size; i++)
	{
		if(args[i] == "name")
		{
			nameSearch = true;
			name = values[i];
		}
		else if(args[i] == "artist")
		{
			artistSearch = true;
			artist = values[i];
		}
	}

	std::vector<song>::size_type sizeT = songTab.size();
	for(size_t i = 0; i < sizeT; i++)
	{
		if(nameSearch && songTab[i].name.compare(name) != 0)
			continue;

		if(artistSearch && songTab[i].artist.compare(artist) != 0)
			continue;

		returnVal.push_back(songTab[i]);
	}

	return returnVal;
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
	std::cout << "AJOUT DE MORCEAU" << std::endl;

	song s;
	s.name = name;
	s.artist = artist;
	s.path = path;
	this->addSong(s);

	std::cout << name << " par " << artist << " ajoute" << std::endl;
}

void songDB::remove(const std::string& path, const Ice::Current&)
{
	std::cout << "SUPPRESSION" << std::endl;

	if(songTab.empty())
		return;

	std::vector<song>::iterator it;
	for(it = songTab.begin(); it != songTab.end(); ++it)
	{
		if(it->path.compare(path) == 0)
			it = songTab.erase(it);
		if(it == songTab.end())
			return;
	}

	std::cout << path << " supprime" << std::endl;
}

Player::songSeq songDB::findByTitle(const std::string& title, const Ice::Current&)
{
	std::cout << "Recherde de " << title << " dans le titre"<< std::endl;

	std::vector<std::string> args;
	args.push_back("name");
	std::vector<std::string> values;
	values.push_back(title);

	return search(args, values);
}

Player::songSeq songDB::findByArtist(const std::string& artist, const Ice::Current&)
{
	std::cout << "Recherde de " << artist << " dans l'artiste'"<< std::endl;

	std::vector<std::string> args;
	args.push_back("artist");
	std::vector<std::string> values;
	values.push_back(artist);

	return search(args, values);
}

Player::songSeq songDB::list(const Ice::Current&)
{
	std::cout << "Affichage de la liste des morceaux" << std::endl;
	return songTab;
}

std::string songDB::start(const std::string& path, const Ice::Current& c)
{
	std::cout << "Starting " + path << std::endl;

	Ice::IPConnectionInfo* ipConInfo = dynamic_cast<Ice::IPConnectionInfo*>(c.con->getInfo().get());
	std::string ipAdress = ipConInfo->remoteAddress;
	std::string port = std::to_string(ipConInfo->remotePort);

	std::string id = generateId(path, ipAdress, port); 
	std::string sout = "#transcode{acodec=mp3,ab=128,channels=2,"
				"samplerate=44100}:http{dst=:8090/" + id + "}";

	libvlc_vlm_add_broadcast(vlcInstance, id.c_str(), path.c_str(), sout.c_str(), 0, NULL, true, false);

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

bool songDB::write(const std::string& name, int offset, const ByteSeq& data, const Ice::Current& c)
{
	std::cout << "Writing " << data.size() << " bytes in " << name << std::endl;
	FILE * filePtr;
	filePtr = fopen(name.c_str(), "a+");
	fseek(filePtr, offset, SEEK_SET);
	fwrite(&data[0], 1, data.size(), filePtr);
	fclose(filePtr);
}
