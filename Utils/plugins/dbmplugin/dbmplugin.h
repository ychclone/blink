#ifndef DBMPLUGIN_H
#define DBMPLUGIN_H

class Info
{
	public:
		Info(void *_db=0, const QString &_dbmName=QString())
			: db(_db), dbmName(_dbmName) {}

		void *db;
		QString dbmName;
};

class DbmInterfacePlugin : public QObject,
	public DbmInterface
{
	Q_OBJECT
		Q_INTERFACES(DbmInterface)

	public:
		~DbmInterfacePlugin();

		int open(const QString &filename);
		void close(int id);
		QStringList lookup(int id, const QString &key);

	private:
		QMap<int, Info> dbptrs;
};


#endif

