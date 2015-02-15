#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "songDB.hpp"
#include "server.h"
using namespace Player;

void songDB::addSong(song s)
{
	songTab.push_back(s);
}

void songDB::addSong(std::string name, std::string artist, std::string path)
{
	song s;
	s.name = name;
	s.artist = artist;
	s.path = path;

	this->addSong(s);
}

void songDB::deleteSong(std::string path)
{
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
}

std::vector<song> songDB::list()
{
	return songTab;
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

std::vector<song> songDB::findByTitle(std::string title)
{
	std::vector<std::string> args;
	args.push_back("name");
	std::vector<std::string> values;
	values.push_back(title);

	return search(args, values);
}

std::vector<song> songDB::findByArtist(std::string artist)
{
	std::vector<std::string> args;
	args.push_back("artist");
	std::vector<std::string> values;
	values.push_back(artist);

	return search(args, values);
}

std::string songDB::newId()
{
	std::string size = std::to_string(idTab.size());
	libvlc_instance_t* vlcptr = libvlc_new(0, NULL);
	idTab.insert(std::pair<std::string, libvlc_instance_t*>(size, vlcptr));
	return size;
}

std::string songDB::start(std::string id, std::string path)
{
	const char *sout = "#transcode{acodec=mp3,ab=128,channels=2," \
			    "samplerate=44100}:http{dst=:8090/go.mp3}";
	const char *media_name = "Foo";

	libvlc_vlm_add_broadcast(idTab[id], id.c_str(), path.c_str(), sout, 0, NULL, true, false);
}

void songDB::play(std::string id)
{
	libvlc_vlm_play_media(idTab[id], id.c_str());
}

void songDB::stop(std::string id)
{
	libvlc_vlm_stop_media(idTab[id], id.c_str());
	libvlc_vlm_release(idTab[id]);
}

void songDB::addSong(const std::string& name, const std::string& artist, const std::string& path, const Ice::Current&)
{
	std::cout << "AJOUT DE MORCEAU" << std::endl;
	this->addSong(name, artist, path);
	std::cout << name << " par " << artist << " ajoute" << std::endl;
}

void songDB::remove(const std::string& path, const Ice::Current&)
{
	std::cout << "SUPPRESSION" << std::endl;
	this->deleteSong(path);
	std::cout << path << " supprime" << std::endl;
}

Player::songSeq songDB::findByTitle(const std::string& title, const Ice::Current&)
{
	std::cout << "Recherde de " << title << " dans le titre"<< std::endl;
	return this->findByTitle(title);
}

Player::songSeq songDB::findByArtist(const std::string& artist, const Ice::Current&)
{
	std::cout << "Recherde de " << artist << " dans l'artiste'"<< std::endl;
	return this->findByArtist(artist);
}

Player::songSeq songDB::list(const Ice::Current&)
{
	std::cout << "Affichage de la liste des morceaux" << std::endl;
	return this->list();
}

std::string songDB::getId(const Ice::Current&)
{
	std::cout << "Demande d'identifiant recue" << std::endl;
	return this->newId();
}

std::string songDB::start(const std::string& id, const std::string& path, const Ice::Current&)
{
	std::cout << "Starting " + path << std::endl;
	return this->start(id, path);
}

void songDB::play(const std::string& id, const Ice::Current&)
{
	std::cout << id + " playing" << std::endl;
	return this->play(id);
}

void songDB::stop(const std::string& id, const Ice::Current&)
{
	std::cout << id + " stopping" << std::endl;
	return this->stop(id);
}
