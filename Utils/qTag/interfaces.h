class DbmInterface
{
	public:
		virtual ~DbmInterface() {}
		virtual int open(const QString &filename) = 0;
		virtual void close(int id) = 0;
		virtual QStringList lookup(int id, const QString &key) = 0;
};

Q_DECLARE_INTERFACE(DbmInterface, "projectloader.DbmInterface/1.0")

