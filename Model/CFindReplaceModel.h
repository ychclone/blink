#ifndef CFIND_REPLACE_MODEL_H
#define CFIND_REPLACE_MODEL_H

#include <QStandardItemModel>

class CFindReplaceModel
{
public:
	CFindReplaceModel();
    virtual ~CFindReplaceModel();

	void setFileList(QStringList fileList);
	QStringList getFileList() ;

	QStringList selectAllFiles();
	QStringList clearSelectAllFiles();

	QStandardItemModel* getFileListModel();

private:
	QStandardItemModel* fileListModel_;

	QStringList fileList_;
};


#endif // CFIND_REPLACE_MODEL_H

