#include <iostream>

#define CATCH_CONFIG_MAIN
#include "../include/Functional.hpp"
#include "../external/Catch2/single_include/catch2/catch.hpp"

using namespace func::pure;
TEST_CASE("Empty List as length 0", "[LIST]")
{
    List<int> emptyList;
    REQUIRE(emptyList.length() == 0);
}


TEST_CASE("List constructor correctly increments length(1)", "[LIST]")
{
    List<int> listWithOneElement{{1}};
    REQUIRE(listWithOneElement.length() == 1);
}

TEST_CASE("List constructor correctly increments length(3)", "[LIST]")
{
    List<int> listWithThreeElements{{1,2,3}};
    REQUIRE(listWithThreeElements.length() == 3);
}


TEST_CASE("List constructor correctly inserts 1 elements", "[LIST]")
{
    List<int> listWithOne{{1}};
    REQUIRE(listWithOne.length() == 1);
    REQUIRE(listWithOne[0] == 1);
}

TEST_CASE("List constructor correctly inserts 3 elements\n and subscript return the correct values", "[LIST]")
{
    List<int> listWithOneTwoThree{{1,2,3}};
    REQUIRE(listWithOneTwoThree.length() == 3);

    REQUIRE(listWithOneTwoThree[0] == 1);
    REQUIRE(listWithOneTwoThree[1] == 2);
    REQUIRE(listWithOneTwoThree[2] == 3);
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


TEST_CASE("Copying works", "[LIST]")
{
    List<int> listWithOneTwoThree{{1,2,3}};
    List<int> copiedList = listWithOneTwoThree;

    REQUIRE((listWithOneTwoThree == copiedList));
}


TEST_CASE("push_back doesnt change old list, \ncorrrectly appends", "[LIST]")
{
    List<int> listWithOneTwoThree{{1,2,3}};
    List<int> listWithOneTwoThreeFour = listWithOneTwoThree.push_back(4);

    REQUIRE(listWithOneTwoThree.length() == 3);
    REQUIRE(listWithOneTwoThreeFour.length() == 4);

    REQUIRE((listWithOneTwoThree == List<int>{{1,2,3}}));
    REQUIRE((listWithOneTwoThreeFour == List<int>{{1,2,3,4}}));
}

TEST_CASE("Push_front doesnt change old list, \ncorrrectly prepends", "[LIST]")
{
    List<int> listWithOneTwoThree{{1,2,3}};
    List<int> listWithFourOneTwoThree = listWithOneTwoThree.push_front(4);


    REQUIRE(listWithOneTwoThree.length() == 3);
    REQUIRE(listWithFourOneTwoThree.length() == 4);

    REQUIRE((listWithOneTwoThree == List<int>{{1,2,3}}));
    REQUIRE((listWithFourOneTwoThree== List<int>{{4,1,2,3}}));
}
