#include <iostream>
#include <jsoncpp/json/json.h>

int main()
{
    Json::Value root;
    root["名字"] = "lzs";
    root["年龄"] = 20;
    root["性别"] = "man";

    Json::Value score;
    score["Math"] = 100;
    score["Chinese"] = 100;

    Json::Value student;
    student["Who"] = root;
    student["Score"] = score;

    Json::Value students;
    for(int i = 0;i<10;i++)
    students.append(student);

    Json::StreamWriterBuilder wbuilder;
    // std::unique_ptr<Json::StreamWriter> sw = std::make_unique<Json::StreamWriter>(wbuilder.newStreamWriter());
    std::unique_ptr<Json::StreamWriter> sw(wbuilder.newStreamWriter());
    
    std::stringstream ss;
    sw->write(students,&ss);
    std::cout<<ss.str();

    //2.
    // // Json::StyledWriter writer;
    // Json::FastWriter writer;
    // std::string s = writer.write(root);
    // std::cout<<s;


    //不推荐
    // Json::Value root;
    // root["名字"] = "lzs";
    // root["年龄"] = 20;
    // root["性别"] = "男";
    // std::cout<<root.toStyledString();
    return 0;
}