#include <iostream>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "../include/Functional.hpp"
#include "../external/Catch2/single_include/catch2/catch.hpp"
using namespace func::pure;

TEST_CASE("map", "[FUNCTIONS]")
{
    SECTION("Mapping over and empty container returns a empty container of the same typr")
    {
        std::vector<int> emptyVec;
        auto test  = map(emptyVec, [](const int x){return x;});
        REQUIRE(test.size() == 0);
    }

    SECTION("Mapping over a non empty container preservs length")
    {
        std::vector<int> nonEmptyVec{1,2,3,4,5};
        auto test  = map(nonEmptyVec, [](const int x){return x;});
        REQUIRE(test.size() == nonEmptyVec.size());
    }
}

TEST_CASE("Construction", "[LIST")
{
        SECTION("Empty List as length 0", "[LIST]")
        {
                List<int> emptyList;
                REQUIRE(emptyList.length() == 0);
        }

        SECTION("Empty init. List works with List" , "[LIST]")
        {
                List<int> emptyList{{}};
                REQUIRE(emptyList.length() == 0);
        }

        SECTION("List constructor correctly increments length(1)", "[LIST]")
        {
                List<int> listWithOneElement{{1}};
                REQUIRE(listWithOneElement.length() == 1);
        }

        SECTION("List constructor correctly increments length(3)", "[LIST]")
        {
                List<int> listWithThreeElements{{1,2,3}};
                REQUIRE(listWithThreeElements.length() == 3);
        }


        SECTION("List constructor correctly inserts 1 elements", "[LIST]")
        {
                List<int> listWithOne{{1}};
                REQUIRE(listWithOne.length() == 1);
                REQUIRE(listWithOne[0] == 1);
        }

        SECTION("List constructor correctly inserts 3 elements\n and subscript return the correct values", "[LIST]")
        {
                List<int> listWithOneTwoThree{{1,2,3}};
                REQUIRE(listWithOneTwoThree.length() == 3);
                
                REQUIRE(listWithOneTwoThree[0] == 1);
                REQUIRE(listWithOneTwoThree[1] == 2);
                REQUIRE(listWithOneTwoThree[2] == 3);
        }
}

TEST_CASE("Out of bounds subscript throws an exception", "[LIST]")
{
    List<int> listWithOneTwoThree{{1,2,3}};
    REQUIRE(listWithOneTwoThree.length() == 3);

    REQUIRE_THROWS_WITH(listWithOneTwoThree[4], "Index is out of range");
}

TEST_CASE("Equality check works", "[LIST]")
{
    List<int> listWithOneTwoThree{{1,2,3}};
    List<int> listWithOneTwoThree2{{1,2,3}};

    REQUIRE((listWithOneTwoThree == listWithOneTwoThree2));
    REQUIRE((listWithOneTwoThree2 == listWithOneTwoThree));
}

TEST_CASE("Inequality check works", "[LIST]")
{
    List<int> listWithOneTwoThree{{1,2,3}};
    List<int> listWithOneTwoThree2{{1,2,3,4}};

    REQUIRE((listWithOneTwoThree != listWithOneTwoThree2));
    REQUIRE((listWithOneTwoThree2 != listWithOneTwoThree));
}


TEST_CASE("Copying", "[LIST]")
{
    SECTION("Copying filled list")
    {
        List<int> listWithOneTwoThree{{1,2,3}};
        List<int> copiedList = listWithOneTwoThree;

        REQUIRE(listWithOneTwoThree.length() == copiedList.length());
        REQUIRE((listWithOneTwoThree == copiedList));
    }
}


TEST_CASE("push_back" , "[LIST]")
{
    SECTION("push_back on filled list")
    {
        List<int> listWithOneTwoThree{{1,2,3}};
        List<int> listWithOneTwoThreeFour = listWithOneTwoThree.push_back(4);

        REQUIRE(listWithOneTwoThree.length() == 3);
        REQUIRE(listWithOneTwoThreeFour.length() == 4);

        REQUIRE((listWithOneTwoThree == List<int>{1,2,3}));
        REQUIRE((listWithOneTwoThreeFour == List<int>{1,2,3,4}));
    }

    SECTION("push_back on empty list")
    {
        List<int> emptyList{};
        auto listWithOne = emptyList.push_back(1);

        REQUIRE(listWithOne.length() == 1);
        REQUIRE((listWithOne == List<int>{{1}}));
    }
}

TEST_CASE("Push_front", "[LIST]")
{
    SECTION("push_front on filled list")
    {
        List<int> listWithOneTwoThree{{1,2,3}};
        List<int> listWithFourOneTwoThree = listWithOneTwoThree.push_front(4);


        REQUIRE(listWithOneTwoThree.length() == 3);
        REQUIRE(listWithFourOneTwoThree.length() == 4);

        REQUIRE((listWithOneTwoThree == List<int>{1,2,3}));
        REQUIRE((listWithFourOneTwoThree== List<int>{4,1,2,3}));
    }

    SECTION("push_front on empty list")
    {
        List<int> emptyList{};
        auto listWithOne = emptyList.push_front(1);

        REQUIRE(listWithOne.length() == 1);
        REQUIRE((listWithOne == List<int>{{1}}));
    }

}


TEST_CASE("head", "[LIST]")
{
    SECTION("Empty list")
    {
         List<int> emptyList{};

         REQUIRE(emptyList.head() == std::nullopt);
    }


    SECTION("Non-empty list")
    {
         List<int> emptyList{1,2,3};

         REQUIRE(emptyList.head() == 1);
    }
}


TEST_CASE("tail", "[LIST]")
{
    SECTION("Empty list")
    {
         List<int> emptyList{};

         REQUIRE(emptyList.tail() == std::nullopt);
    }


    SECTION("Non-empty list")
    {
         List<int> emptyList{1,2,3};

         REQUIRE(emptyList.tail() == 3);
    }
}
