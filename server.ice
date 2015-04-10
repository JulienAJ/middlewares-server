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
		void addSong(string name, string artist, string path);
		void remove(string path);
		songSeq findByTitle(string name);
		songSeq findByArtist(string artist);
		songSeq list();

		string start(string path);
		void play(string id);
		void stop(string id);

		bool write(string name, int offset, ByteSeq data);
	};
};
