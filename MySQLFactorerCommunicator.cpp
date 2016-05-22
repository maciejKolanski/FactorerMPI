#include "MySQLFactorerCommunicator.h"

#include <stdlib.h>
#include <iostream>
#include <unistd.h>

MySQLFactorerCommunicator::MySQLFactorerCommunicator(const char *ip, const char *port, const char *user,
                            const char *pass, const char *defSchema)
                            :con(nullptr), stmt(nullptr)
{
    try
    {
        driver = get_driver_instance();
    }
    catch (sql::SQLException &e)
    {
        std::string msg("ERR: ");
        msg.append( e.what());
        throw FactorerCommunicatorException(msg);
    }

    address = std::string("tcp://");
    address.append(ip);
    address.append(":");
    address.append(port);

    username = std::string(user);
    password = std::string(pass);
    defaultSchema = std::string(defSchema);
}

MySQLFactorerCommunicator::~MySQLFactorerCommunicator()
{
    clearExecute();
}


CommunicatorCommand MySQLFactorerCommunicator::getCommand(MPIAlgorithm::AlgorithmsEnum& algorithm, std::string& number)
{
    try
    {
        sql::ResultSet *res = nullptr;
        bool wait = false;

        do{
            res = executeQuery(
                "SELECT id, number_to_factor, algorithm_id FROM FactorerMain_task WHERE state = 0 ORDER BY job_date;");
            wait = (res->next() == false);
            if( wait )
            {
                delete res;
                res = nullptr;
                sleep(SLEEP_TIME);
            }
        }while( wait );

        currentTaskId = res->getInt(1);
        number = res->getString(2);
        algorithm = MPIAlgorithm::AlgorithmsEnum(res->getInt(3));//TODO THAT'S BADs

        delete res;
        }
        catch (sql::SQLException &e)
         {
            std::string msg("ERR: ");
            msg.append( e.what());
            throw FactorerCommunicatorException(msg);
        }

        return CommunicatorCommand::Algorithm;
}

void MySQLFactorerCommunicator::algorithmFinnished(const std::vector<std::string>& result)
{
    try
    {
        std::unique_ptr<sql::Connection>
            insert_con(driver->connect(address.c_str(), username.c_str(), password.c_str()));
        insert_con->setSchema(defaultSchema.c_str());

        std::string resStr;
        for(std::string str :result )
        {
            resStr.append(str.append("*"));
        }
        std::cout << "Returned: " << resStr << "\n";
        std::string cmd = std::string("UPDATE FactorerMain_task SET state = 2, result = \"");
        cmd.append(resStr);
        cmd.append("\" WHERE id = ");
        cmd.append(std::to_string(currentTaskId));
        cmd.append(";");
        std::cout << cmd << std::endl;
        executeUpdate(cmd.c_str());
    }
    catch (sql::SQLException &e)
         {
            std::string msg("ERR: ");
            msg.append( e.what());
            throw FactorerCommunicatorException(msg);
        }
}

sql::ResultSet* MySQLFactorerCommunicator::executeQuery(const char* command)
{
    sql::ResultSet *res = nullptr;

    if( prepareExecute() )
    {
        res = stmt->executeQuery(command);
    }
    clearExecute();

    return res;
}

int MySQLFactorerCommunicator::executeUpdate(const char *command)
{
    int ret = prepareExecute();

    if( ret )
    {
        ret = stmt->executeUpdate(command);
    }
    clearExecute();

    return ret;
}

bool MySQLFactorerCommunicator::prepareExecute()
{
    if( con == nullptr && stmt == nullptr )
    {
        con = driver->connect(address.c_str(), username.c_str(), password.c_str());
        con->setSchema(defaultSchema.c_str());
        stmt = con->createStatement();
        return true;
    }
    else
    {
        return false;
    }
}

void MySQLFactorerCommunicator::clearExecute()
{
    if( stmt )
    {
        delete stmt;
        stmt = nullptr;
    }
    if( con )
    {
        delete con;
        con = nullptr;
    }
}

