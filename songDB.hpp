#pragma once
#include <string>
#include <map>
#include <vector>
#include <vlc/libvlc.h>
#include <vlc/vlc.h>
#include "server.h"
using namespace Player;

class songDB : public Server
{
	private:
		std::vector<song> songTab;
		//std::map<std::string, libvlc_instance_t*> idTab;
		libvlc_instance_t* vlcInstance;

		std::vector<song> search(std::vector<std::string> args, std::vector<std::string> values);
		void addSong(song s);
		std::string generateId(std::string path, std::string ipAdress, std::string port);

	public:
		songDB();
		~songDB();
		void addSong(const std::string& name, const std::string& artist, const std::string& path, const Ice::Current&);
		void remove(const std::string& path, const Ice::Current&);
		songSeq findByTitle(const std::string& title, const Ice::Current&);
		songSeq findByArtist(const std::string& artist, const Ice::Current&);
		songSeq list(const Ice::Current&);

		std::string start(const std::string& path, const Ice::Current&);
		void play(const std::string& id, const Ice::Current&);
		void stop(const std::string& id, const Ice::Current&);
};
