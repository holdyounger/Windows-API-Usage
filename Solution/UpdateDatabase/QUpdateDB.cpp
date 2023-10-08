#include "QUpdateDB.h"


#if defined(Q_OS_WIN)
#define TRUSTAGENT_APPCFG_DIR_NAME  "TrustAgent"
#else
#define TRUSTAGENT_APPCFG_DIR_NAME  ".TrustAgent"
#endif

#define TRUSTAGENT_CONFIG_DATABASE			"TrustAgent.db"
#define TRUSTAGENT_CONFIG_DATABASE_ENCRY    "TrustConf.db"

#define QSQLCIPHER_CREATE_KEY "QSQLITE_CREATE_KEY" 
#define QSQLCIPHER_UPDATE_KEY "QSQLITE_UPDATE_KEY"
#define QSQLCIPHER_REMOVE_KEY "QSQLITE_REMOVE_KEY"

#define TRUST_SQLDIRVER "QSQLCIPHER"

QUpdateDB* QUpdateDB::s_pInstance = NULL;

QUpdateDB::QUpdateDB()
{
	Init();
}

QUpdateDB::QUpdateDB(QString path)
{
	// 验证文件
	QFileInfo fileInfo(path);

	qDebug() << __FUNCTION__ << __LINE__ << fileInfo.path();

	QString fileDir = fileInfo.path();

	QDir dir(fileDir);

	if (!dir.exists())
	{
		dir.mkpath(fileDir);
	}

	m_dbPath = fileDir;

	qDebug() << "[CFG]" << m_dbPath;

	Init();
}

void QUpdateDB::Init()
{
	if (m_dbPath.isEmpty())
	{
		QString strPath = QString("%1/%2").arg(QDir::homePath()).arg(TRUSTAGENT_APPCFG_DIR_NAME);
		m_dbPath = strPath;
	}

	qDebug() << __FUNCTION__ << m_dbPath;

	/* OPEN Database */
	m_database = QSqlDatabase::addDatabase(TRUST_SQLDIRVER, TRUSTAGENT_CONFIG_DATABASE);

	QString dbPath = QString("%1/%2").arg(m_dbPath);
	m_database.setDatabaseName(dbPath.arg(TRUSTAGENT_CONFIG_DATABASE));
}

bool QUpdateDB::open()
{
	qDebug() << __FUNCTION__ << m_database.connectOptions();

	bool open = m_database.open();
	if (!m_database.isOpen())
	{
		qDebug() << "Connection failed: " << m_database.lastError().text();
	}
	 
	return open;
}

// 拷贝数据库文件到 TrustConf.db
bool QUpdateDB::copyFiletoNew(QString dstPath, bool coverFileIfExist)
{
	
	QString srcPath = m_database.databaseName();
	if (!QFile::exists(srcPath)) {
		return false;
	}

	if (QFile::exists(dstPath)) {
		if (coverFileIfExist) {
			QFile::remove(dstPath);
		}
		else
		{
			QFileInfo info(dstPath);
			QString timeNow = QString(".%1").arg(QString::number(QDateTime::currentDateTime().toTime_t()));
			QString fileName = dstPath.insert(dstPath.lastIndexOf(info.completeSuffix()) - 1, timeNow);
			dstPath = fileName;
			qDebug() << fileName;
		}
	}

	if (!QFile::copy(srcPath, dstPath)) {
		return false;
	}

	m_database.setDatabaseName(dstPath);
	// 移除原有文件
	QFile::remove(srcPath);
	return true;
}

bool QUpdateDB::createKey(QString passwd)
{
	m_database.setPassword(passwd);
	m_database.setConnectOptions(QSQLCIPHER_CREATE_KEY);
	return m_database.connectOptions().contains(QSQLCIPHER_CREATE_KEY, Qt::CaseInsensitive);
}

bool QUpdateDB::updatekey(QString oldPasswd, QString newPasswd)
{
	m_database.setPassword(oldPasswd);
	QString setting = QString("%1=%2").arg(QSQLCIPHER_UPDATE_KEY).arg(newPasswd);
	m_database.setConnectOptions();
	return m_database.connectOptions().contains(setting, Qt::CaseInsensitive);
}

bool QUpdateDB::removekey(QString passwd)
{
	m_database.setPassword(passwd);
	m_database.setConnectOptions(QSQLCIPHER_REMOVE_KEY);
	return m_database.connectOptions().contains(QSQLCIPHER_REMOVE_KEY, Qt::CaseInsensitive);
}

QUpdateDB* QUpdateDB::GetInstance()
{
	if (s_pInstance == NULL)
	{
		QString strPath = QString("%1/%2").arg(QDir::homePath()).arg(TRUSTAGENT_APPCFG_DIR_NAME);

		strPath = QString("%1/%2").arg(strPath).arg(TRUSTAGENT_CONFIG_DATABASE);

		s_pInstance = new QUpdateDB(strPath);
	}
	
	return s_pInstance;
}
