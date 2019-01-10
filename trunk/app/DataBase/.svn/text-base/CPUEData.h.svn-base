#include <string>
#include <kvs/TableObject>
#include <QSqlDatabase>

class CPUEData
{
public:

    typedef kvs::TableObject Table;
    typedef QSqlDatabase Database;

private:

    Table m_table;
    Database m_database;

public:

    CPUEData( const std::string& filename );

    bool open( const std::string& dbname );
    void close();
    bool create();
};
