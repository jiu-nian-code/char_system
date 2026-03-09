#include <string>
#include <memory> // std::auto_ptr
#include <cstdlib> // std::exit
#include <iostream>
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include "person.hxx"
#include "person-odb.hxx"

int main()
{
    // std::shared_ptr<odb::core::database> db(
    //     new odb::mysql::database("root", "123456", "test_db", "127.0.0.1", 3306, 0, "utf8"));
    std::shared_ptr<odb::core::database> db(
        new odb::mysql::database("root", "123456", 
        "test_db", "127.0.0.1", 3306, "", "utf8", 0));
    // if(!db)
    // {
    //     std::cout << "db error" << std::endl;
    //     return -1;
    // }
    // ptime p = boost::posix_time::second_clock::local_time();
    // Person pr1("张三", 20, p);
    // Person pr2("李四", 20, p);
    // {
    //     odb::core::transaction ts(db->begin());
    //     db->persist(pr1);
    //     db->persist(pr2);
    //     ts.commit();
    // }
    return 0;
}