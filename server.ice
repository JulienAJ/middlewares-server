module Player
{
	struct song
	{
		string name;
		string artist;
		string path;
	};

	sequence<song> songSeq;
	sequence<byte> ByteSeq;

	interface Server
	{
		int getCount();
		string getStreamingPort();

		void addSong(string name, string artist, string path);
		void remove(string path);
		songSeq findByTitle(string name);
		songSeq findByArtist(string artist);
		song findByBoth(string title, string artist);
		songSeq findByAny(string searchKey);
		songSeq list();

		string start(string path);
		void play(string id);
		void stop(string id);

		void write(string name, int offset, ByteSeq data);
		ByteSeq read(string filename, int offset, int count);
	};

	interface Monitor
	{
		void songRemoved(song s);
		void newSong(song s);
		void serverUp();
		void serverDown();
	};
};
