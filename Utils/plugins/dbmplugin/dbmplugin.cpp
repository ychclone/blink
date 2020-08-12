static int nextId = 0; // zero represents an invalid ID


DbmInterfacePlugin::~DbmInterfacePlugin()
{
	QMapIterator<int, Info> i(dbptrs);
	while (i.hasNext()) {
		i.next();
		close(i.key());
	}
}


Q_EXPORT_PLUGIN(DbmPlugin, DbmInterfacePlugin)



