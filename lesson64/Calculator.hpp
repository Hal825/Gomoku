#pragma once

#include "Protocol.hpp"
#include <iostream>
#include <string>

class Calculator
{
public:
    Response Execute(Request &req)
    {
        Response resp;
        int x = req.Get_data_x(), y = req.Get_data_y();
        switch (req.Get_oper())
        {
        case '+':
            resp.Modify_result(x + y);
            break;
        case '-':
            resp.Modify_result(x - y);
            break;
        case '*':
            resp.Modify_result(x * y);
            break;
        case '/':
        {
            if (y == 0)
            {
                resp.Modify_code(1);
            }
            else
                resp.Modify_result(x / y);
        }
        break;
        case '%':
        {
            if (y == 0)
            {
                resp.Modify_code(2);
            }
            else
                resp.Modify_result(x % y);
        }
        break;
        default:
            resp.Modify_code(3);
            break;
        }
        return resp;
    }
};