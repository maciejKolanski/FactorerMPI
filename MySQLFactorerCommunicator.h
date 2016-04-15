#ifndef MYSQLFACTORERCOMMUNICATOR_H
#define MYSQLFACTORERCOMMUNICATOR_H

#include <memory>
#include "FactorerCommunicatorInterface.h"
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class MySQLFactorerCommunicator : public FactorerCommunicatorInterface
{
    public:
        MySQLFactorerCommunicator(const char *ip, const char *port, const char *user,
                            const char *pass, const char *defSchema);
        virtual ~MySQLFactorerCommunicator();

        virtual CommunicatorCommand getCommand(MPIAlgorithm::AlgorithmsEnum &algorithm, std::string &number );
        virtual void algorithmFinnished(const std::vector<std::string> &result);

    private:
        static constexpr int SLEEP_TIME = 2;

        std::string address, username, password, defaultSchema;
        int currentTaskId;
        sql::Driver *driver;

        sql::Connection *con;
        sql::Statement *stmt;

        bool prepareExecute();
        void clearExecute();
        sql::ResultSet* executeQuery(const char *command);
        int executeUpdate(const char *command);
};

#endif // MYSQLFACTORERCOMMUNICATOR_H
