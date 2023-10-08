#pragma once
#include <QObject>
#include <QtSql/QtSql>
#include <QCoreApplication>
#include <QString>
#include <QtSql/qsqldatabase.h>


/*
	1. 设置新旧数据库
		
	2. 修改并保存加密数据库的配置
			QSQLITE_CREATE_KEY,
			QSQLITE_REMOVE_KEY,
			QSQLITE_UPDATE_KEY,
			QSQLITE_BUSY_TIMEOUT,
			QSQLITE_OPEN_READONLY,
			QSQLITE_OPEN_URI,
			QSQLITE_ENABLE_SHARED_CACHE

	3. 数据拷贝

	4. 删除原数据库
*/

class QUpdateDB : public QObject
{
	Q_OBJECT
private:

	QUpdateDB();
	QUpdateDB(QString Path);

public:
	// 1 
	bool copyFiletoNew(QString newPath, bool coverFileIfExist);
	// 2
	bool createKey(QString passwd);
	bool updatekey(QString oldPasswd, QString newPasswd);
	bool removekey(QString passwd);
	// 3
	bool open();

private:
	void Init();

public:
	static QUpdateDB* GetInstance();

private:
	QString m_dbPath;
	QSqlDatabase m_database;
	static QUpdateDB* s_pInstance;

};

