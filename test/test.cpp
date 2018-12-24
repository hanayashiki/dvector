// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "tester.h"
#include "../dvector/dslice.h"

#include "../dvector/dleaf.h"
#include "../dvector/dvector.h"
#include "dbgalloc.h"
#include <iostream>
#include <list>

int main()
{
    using namespace test_utils;
    using namespace dbgalloc;

    {
        tester t("dslice compile");
        
        std::vector<int> v1 = { 1, 2, 3, 4 };
        dv::_dslice_base<int> b1(v1);
        dv::_dslice_base<int> b2(std::move(v1));
        t.test() << v1.size();
        t.expected() << 0;
    }

    {
        tester t("dslice share");
        //std::list<int> l1 = { 1, 2, 3, 4 };
        std::vector<int> v2 = { 1, 2, 3, 4 };
        dv::dslice<int> d1(1, 2, v2);
        dv::dslice<int> d2(2, 4, std::move(v2));
        t.test() << v2.size();
        t.expected() << 0;

        t.test() << d2[0];
        t.expected() << 3;

        d2[0] = 4;
        t.test() << d2[0];
        t.expected() << 4; // d2 = {4, 4}

        auto d3 = d2.slice(0, 1);
        d3[0] = 5;
        t.test() << d2[0];
        t.expected() << 5;

        t.test() << d3.size();
        t.expected() << 1;

        auto d4 = d3;
        d4[0] = 6;
        t.test() << d2[0];
        t.expected() << 6;
    }

    {
        tester t("dleaf compile");
        dv::dslice<int> slice(1, 4, std::vector<int>{0, 1, 2, 3, 4});
        dv::dleaf<int> leaf1(slice);

        t.test() << leaf1.count();
        t.expected() << 3;

        t.test() << leaf1.type;
        t.expected() << 'l';
    }

    {
        Dbg dbg;

        tester t("dvector compile");
        dv::dvector<int, DbgAlloc<int>> dvector1(std::vector<int, DbgAlloc<int>>{0, 1, 2, 3, 4});
        t.test() << dvector1[0];
        t.expected() << 0;

        std::vector<int, DbgAlloc<int>> v2{0, 1, 2, 3, 4};
        dv::dvector<int, DbgAlloc<int>> dvector2(v2);
        t.test() << v2.size();
        t.expected() << 5;
        dv::dvector<int, DbgAlloc<int>> dvector3(std::move(v2));
        t.test() << v2.size();
        t.expected() << 0;

        std::cout << dvector3.visualize();
    }

    {
        Dbg dbg;

        tester t("dvector_base insert");

        dv::dvector<int, DbgAlloc<int>>
            db(std::vector<int, DbgAlloc<int>>{ 0, 1, 2, 3, 4 });

        db.insert(0, std::vector<int, DbgAlloc<int>>{ 233 });

        std::cout << db.visualize();

        db.insert(0, std::vector<int, DbgAlloc<int>>{ 2333 });

        std::cout << db.visualize();

        t.test() << db[0];
        t.expected() << 2333;
        t.test() << db[1];
        t.expected() << 233;

        db.insert(7, 114514);

        std::cout << "sizeof(db) == " << sizeof(db) << std::endl;
        std::cout << db.visualize();
        t.test() << db[7];
        t.expected() << 114514;
    }

    std::cin.get();
}

