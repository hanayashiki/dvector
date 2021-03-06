// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"

#define DV_PROTECTED public

#include "tester.h"
#include "../dvector/dslice.h"

#include "../dvector/dleaf.h"
#include "../dvector/dvector.h"
#include "dbgalloc.h"
#include <iostream>
#include <list>
#include <random>

//#define RUN_LAST

int main()
{
    using namespace test_utils;
    using namespace dbgalloc;
#ifndef RUN_LAST
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
        /*
        std::cout << "left_rotate((dv::dnode*)db.root)" << std::endl;
        auto before_lr = db.visualize();
        db.left_rotate((dv::dnode*)db.root);
        std::cout << db.visualize(false);

        std::cout << "right_rotate((dv::dnode*)db.root)" << std::endl;
        db.right_rotate((dv::dnode*)db.root);
        std::cout << db.visualize();
        auto rr = db.visualize();

        t.test() << (before_lr == rr);
        t.expected() << true;
        */

        db.insert(0, 89);
        db.insert(0, 64);

        std::cout << db.visualize();
    }

    {
        Dbg dbg;

        tester t("dvector_base insert rotate");

        dv::dvector<int, DbgAlloc<int>>
            db(std::vector<int, DbgAlloc<int>>{ 0 });

        db.insert(db.size(), 10);
        db.insert(db.size(), 20);

        std::cout << db.visualize();

        db.insert(db.size(), 30);
        db.insert(db.size(), 40);

        std::cout << db.visualize();

        db.insert(db.size(), 50);
        db.insert(2, 21);
        db.insert(2, 22);

        std::cout << db.visualize();

        t.test() << db.check_height();
        t.expected() << 1;
    }

    {
        Dbg dbg;

        tester t("dvector_base insert inside");
        dv::dvector<int, DbgAlloc<int>>
            db(std::vector<int, DbgAlloc<int>>{ 0, 10, 20, 30, 40 });

        db.insert(1, 5);

        std::cout << db.visualize();

        db.insert(2, 6);
        db.insert(3, 7);
        db.insert(3, 8);
        db.insert(3, 9);

        std::cout << db.visualize();

        t.test() << db.check_height();
        t.expected() << true;

    }

    {
        Dbg dbg;

        tester t("dvector_base insert inside 2");
        dv::dvector<int, DbgAlloc<int>>
            db(std::vector<int, DbgAlloc<int>>{ 0, 10, 20, 30, 40 });

        db.insert(1, 5);

        std::cout << db.visualize();

        for (int i = 0; i < 1000; i++) 
        {
            db.insert(2, 6);
            db.insert(3, 7);
            db.insert(3, 8);
            db.insert(3, 9);
        }

        // std::cout << db.visualize();

        t.test() << db.check_height();
        t.expected() << true;

    }

    {
        Dbg dbg;

        tester t("dvector_base insert inside 3");
        dv::dvector<int, DbgAlloc<int>>
            db(std::vector<int, DbgAlloc<int>>{ 0 });

        std::default_random_engine e;

        e.seed(0);
        
        for (int i = 0; i < 1000; i += 1)
        {
            db.insert(e() % db.size(), 233);
            t.test() << db.check_height();
            t.expected() << true;
        }

        auto node = db.root;
        std::function<size_t(dv::dnode_base*)> height = [&height](dv::dnode_base * n) -> size_t
        {
            if (n->type == 'l')
            {
                return 0;
            }
            else
            {
                dv::dnode * dnode = static_cast<dv::dnode*>(n);
                return std::max(height(dnode->left), height(dnode->right)) + 1;
            }
        };
        std::cout << "height = " << height(node) << std::endl;
        // std::cout << db.visualize();
    }

    {
        Dbg dbg;

        // set_exception_on_alloc_no(0);

        tester t("dvector_base insert erase 4");
        dv::dvector<int, DbgAlloc<int>>
            db(std::vector<int, DbgAlloc<int>>{ 0 });

        std::default_random_engine e;

        e.seed(0);

        std::cout << "inserting..." << std::endl;

        for (int i = 0; i < 1000; i += 1)
        {
            //if (i % 100 == 0) std::cout << "i = " << i << std::endl;
            db.insert(e() % db.size(), 233);
            t.test() << db.check_height();
            t.expected() << true;
        }

        std::cout << "erasing..." << std::endl;

        auto db_(db);

        auto db2 = db;
        db2 = db2;

        std::cout << "db2 size = " << db2.size() << std::endl;

        for (int i = 0; i < 1000; i += 1)
        {
            //std::cout << "erase i = " << i << std::endl;
            db2._erase(db2.root, e() % db2.size());
            t.test() << db.check_height();
            t.expected() << true;
        }

        t.compare();

        auto node = db2.root;
        std::function<size_t(dv::dnode_base*)> height = [&height](dv::dnode_base * n) -> size_t
        {
            if (n->type == 'l')
            {
                return 0;
            }
            else
            {
                dv::dnode * dnode = static_cast<dv::dnode*>(n);
                return std::max(height(dnode->left), height(dnode->right)) + 1;
            }
        };
        std::cout << "height = " << height(node) << std::endl;
        // std::cout << db.visualize();
    }

    {
        Dbg dbg;

        tester t("dvector_base compile other type");

        struct
        {
            double x;
            double y;
        } element;

        dv::dvector<decltype(element)> vec;
        vec.insert(0, element);
        std::cout << vec.visualize();
    }

    {
        Dbg dbg;

        tester t("dvector_base compile string");

        dv::dvector<std::string, DbgAlloc<std::string>> db;
        
        db.insert(0, "fuck");
        db.insert(1, "you");
        db.insert(1, " haha ");

        std::cout << db.visualize();

        auto count = db.size();

        for (size_t i = 0; i < count; i += 1)
        {
            //std::cout << "erase i = " << i << std::endl;
            db.erase(0);
        }
    }

    {
        Dbg dbg;

        tester t("dvector move test");

        dv::dvector<std::string, DbgAlloc<std::string>> db;

        db.push_back("123");

        auto db2(std::move(db));

        t.test() << db2[0];
        t.expected() << "123";

        t.test() << db.size();
        t.expected() << "0";

        auto db3 = std::move(db2);

        t.test() << db3[0];
        t.expected() << "123";

        t.test() << db2.size();
        t.expected() << "0";
    }

    {
        Dbg dbg;

        tester t("dvector copy test");

        dv::dvector<std::string, DbgAlloc<std::string>> db;

        db.push_back("123");

        auto db2(db);

        t.test() << db2[0];
        t.expected() << "123";

        db2[0] = "114514";

        t.test() << db[0];
        t.expected() << "123";

        t.test() << db2[0];
        t.expected() << "114514";
    }

    {
        Dbg dbg;

        tester t("dvector iterator compile test");

        dv::dvector_iterator<int> iter(nullptr, nullptr);

        auto iter2(iter);

        t.test() << (iter2 == iter);
        t.expected() << true;

        t.test() << (iter2 != iter);
        t.expected() << false;

        auto dont_fuck = [&]() {
            dv::dvector_iterator<std::string> iter(nullptr, nullptr);

            auto size = iter->size();
            const auto & string = *iter;
            *iter = "1234";

            auto a = *dv::dvector_iterator<std::string>(nullptr, nullptr);

            auto add = iter + 5;
            auto add2 = 5 + iter;
            auto minus = iter - 1;

            auto diff = add2 - add;

            auto cmp1 = add < add2;
            auto cmp2 = add > add2;
            auto cmp3 = add <= add2;
            auto cmp4 = add >= add2;

            add += 1;
            add -= 1;
        };

        if (printf("dummy_") - printf("dummy\n"))
            dont_fuck(); // Lures the compiler to compile dont_fuck

    }
#endif
    {
        Dbg dbg;

        tester t("dvector iterator test");

        dv::dvector<int> d({1, 2, 3});

        for (const auto & e : d)
        {
            t.test() << e;
        }

        t.expected() << "123";

        dv::dvector<int> empty_dv;

        for (const auto & e : empty_dv)
        {
        }

        dv::dvector<int> d2({ 1, 3, 5 });

        std::cout << d2.visualize();

        d2.insert(1, 2);
        std::cout << d2.visualize();
        d2.insert(3, 4);

        std::cout << d2.visualize();

        for (const auto & e : d2)
        {
            t.test() << e;
        }

        t.expected() << "12345";

        auto iter = d2.begin();

        t.test() << *iter;
        t.expected() << "1";

        iter = iter + 1;

        t.test() << *iter;
        t.expected() << "2";

        iter = iter - 1;

        t.test() << *iter;
        t.expected() << "1";

        for (auto iter = d2.end() - 1; iter != d2.begin(); iter--)
        {
            t.test() << *iter;
        }
        t.expected() << "5432";

        for (auto iter = d2.end() - 1; iter != d2.begin(); --iter)
        {
            t.test() << *iter;
        }
        t.expected() << "5432";

        auto iter1 = d2.begin() + 5;
        auto iter2 = d2.begin() + 3;
        t.test() << (iter1 - iter2);
        t.expected() << "2";

        auto iter3 = 4 + d2.begin();
        auto iter4 = d2.end() - 3;
        t.test() << iter4 - iter3;
        t.expected() << "-2";

    }

    {
        Dbg dbg;

        tester t("dvector insert correction test");

        std::default_random_engine e;
        e.seed(0);

        dv::dvector<int> d{ 0 };
        std::vector<int> v{ 0 };

        for (int i = 0; i < 1000; i++)
        {
            auto index = e() % (v.size() + 1);
            d.insert(index, i);
            v.insert(v.begin() + index, i);
        }

        for (int i = 0; i < 1000; i++)
        {
            t.test() << d[i];
            //std::cout << d[i] << ",";
            t.expected() << v[i];
            //std::cout << v[i] << std::endl;
        }

        dv::dvector<std::string> d2{ "1", "3", "5" };

        t.test() << d2.begin()->size();
        t.expected() << 1;

        t.test() << (d2.begin() < d2.end());
        t.expected() << 1;

        t.test() << (d2.begin() < d2.end());
        t.expected() << 1;

        t.test() << (d2.begin() <= d2.begin());
        t.expected() << 1;

        t.test() << (d2.end() > d2.begin());
        t.expected() << 1;

        t.test() << (d2.end() >= d2.end());
        t.expected() << 1;


        auto iter5 = d2.begin();
        iter5 += 3;
        iter5 -= 2;

        t.test() << *iter5;
        t.expected() << "3";
    }

    std::cout << "Sounds all good! " << std::endl;
    std::cin.get();
}

