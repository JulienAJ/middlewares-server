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
		static const std::string songsDir;
		static const std::string coversDir;
		static const std::string streamingPort;

		std::vector<song> songTab;
		libvlc_instance_t* vlcInstance;
		MonitorPrx monitor;

		std::string generateId(std::string ipAdress, std::string port);
		std::string lowerCase(std::string original);
		std::string handleDirs(std::string filename);

	public:
		songDB();
		~songDB();

		int getCount(const Ice::Current& c);
		std::string getStreamingPort(const Ice::Current& c);

		// Database management
		void addSong(const std::string& name, const std::string& artist, const std::string& path, const std::string& coverPath, const Ice::Current&);
		void remove(const std::string& path, const Ice::Current&);

		// Research
		songSeq findByAny(const std::string& searchKey, const Ice::Current&);
		song findByBoth(const std::string& title, const std::string& artist, const Ice::Current&);
		songSeq findByTitle(const std::string& title, const Ice::Current&);
		songSeq findByArtist(const std::string& artist, const Ice::Current&);
		songSeq list(const Ice::Current&);

		// Streaming
		std::string start(const std::string& path, const Ice::Current&);
		void play(const std::string& id, const Ice::Current&);
		void stop(const std::string& id, const Ice::Current&);

		// File upload
		void write(const std::string& name, int offset, const ByteSeq& data, const Ice::Current&);
		ByteSeq read(const std::string& filename, int offset, int count, const Ice::Current& c);
		int getFileSize(const std::string& filename, const Ice::Current& c);
};
