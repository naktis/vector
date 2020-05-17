#define CATCH_CONFIG_MAIN

#include <iostream>
#include <vector>
#include "../vector.hpp"
#include "catch.hpp"


TEST_CASE("Constructors") {
    vector<int> fakeEmpty, fake(5, 10), fakeCopy(fake);
    std::vector<int> realEmpty, real(5, 10), realCopy(real);

    SECTION("Size") {
        REQUIRE( fakeEmpty.size() == realEmpty.size() );
        REQUIRE( fake.size() == real.size() );
        REQUIRE( fakeCopy.size() == realCopy.size() );
    }
    SECTION("Values") {
        for (int i = 0; i < fake.size(); i ++)
            REQUIRE( fake[i] == real[i] );

        for (int i = 0; i < fake.size(); i ++)
            REQUIRE( fakeCopy[i] == realCopy[i] );
    }
}

TEST_CASE("Push back") {
    vector<int> fake(3, 10);
    std::vector<int> real(3, 10);

    fake.push_back(4);
    real.push_back(4);

    REQUIRE( fake.size() == real.size() );
    REQUIRE( fake.capacity() == real.capacity() );
    REQUIRE( *(fake.end()-1) == *(real.end()-1) );
}

TEST_CASE("Reserve and shrink to fit") {
    vector<int> fake(2, 55);
    std::vector<int> real(2, 55);

    fake.reserve(10);
    real.reserve(10);

    REQUIRE( fake.capacity() == real.capacity() );

    fake.shrink_to_fit();
    real.shrink_to_fit();

    REQUIRE( fake.capacity() == real.capacity() );
}

TEST_CASE ("Assign") {
    vector<int> fake(10, 55);
    std::vector<int> real(10, 55);

    WHEN("New size < old size") {
        fake.assign(5, 3);
        real.assign(5, 3);

        REQUIRE( fake.size() == real.size() );
        REQUIRE( fake.capacity() == real.capacity() );
        for (int i = 0; i < fake.size(); i ++)
            REQUIRE( fake[i] == real[i] );
    }

    WHEN("New size > old size") {
        fake.assign(20, 3);
        real.assign(20, 3);

        REQUIRE( fake.size() == real.size() );
        REQUIRE( fake.capacity() == real.capacity() );
        for (int i = 0; i < fake.size(); i ++)
            REQUIRE( fake[i] == real[i] );
    }
}

TEST_CASE ("Allocator") {
    vector<int> fake(10, 55);
    std::vector<int> real(10, 55);

    REQUIRE( fake.get_allocator() == real.get_allocator());
}

TEST_CASE("Element Access") {
    vector<int> fake(10);
    std::vector<int> real(10);

    for (int i = 0; i < 10; i ++) {
        fake[i] = i;
        real[i] = i;
    }

    REQUIRE(fake.at(5) == real.at(5));
    REQUIRE(fake.front() == real.front());
    REQUIRE(fake.back() == real.back());
}

TEST_CASE("Clear") {
    vector<int> fake(5, 55);
    std::vector<int> real(5, 55);

    fake.clear();
    real.clear();

    REQUIRE(fake.size() == real.size());
    REQUIRE(fake.capacity() == real.capacity());
}

TEST_CASE("Insert") {
    vector<int> fake(10, 5);
    std::vector<int> real(10, 5);

    auto itfake = fake.begin() + 2;
    auto itreal = real.begin() + 2;

    SECTION("One element") {
        itfake = fake.insert(itfake, 10);
        itreal = real.insert(itreal, 10);
        
        REQUIRE(fake.size() == real.size());
        for (int i = 0; i < fake.size(); i ++)
            REQUIRE(fake[i] == real[i]);
    }
    SECTION("Range of elements") {
        int ten = 10;
        itfake = fake.insert(itfake, 3, ten);
        itreal = real.insert(itreal, 3, ten);

        REQUIRE(fake.size() == real.size());
        for (int i = 0; i < fake.size(); i ++)
            REQUIRE(fake[i] == real[i]);
    }

}

TEST_CASE("Erase") {
    vector<int> fake(10);
    std::vector<int> real(10);

    
    for (int i = 0; i < 10; i ++) {
        fake[i] = i + 1;
        real[i] = i + 1;
    }

    WHEN ("One element is erased") {
    auto itfake = fake.begin() + 2;
    itfake = fake.erase(itfake);
    auto itreal = real.begin() + 2;
    itreal = real.erase(itreal);

    REQUIRE(fake[2] == real[2]);
    REQUIRE(fake.size() == real.size());
    REQUIRE(fake.capacity() == real.capacity());
    }

    WHEN ("Elements in range are erased") {
        auto itfake1 = fake.begin() + 2;
        auto itfake2 = fake.begin() + 5;
        itfake2 = fake.erase(itfake1, itfake2);
        auto itreal1 = real.begin() + 2;
        auto itreal2 = real.begin() + 5;
        itreal2 = real.erase(itreal1, itreal2);

        REQUIRE(fake.size() == real.size());
        REQUIRE(fake.capacity() == real.capacity());
        for (int i = 0; i < fake.size(); i ++)
            REQUIRE(fake[i] == real[i]);
    }

}

TEST_CASE("Pop back") {
    vector<int> fake(10);
    std::vector<int> real(10);

    for (int i = 0; i < 10; i ++)
        fake[i] = real[i] =  i + 1;

    fake.pop_back();
    real.pop_back();

    REQUIRE(fake.size() == real.size());
    REQUIRE(fake.capacity() == real.capacity());
    for (int i = 0; i < fake.size(); i ++)
        REQUIRE(fake[i] == real[i]);
}

TEST_CASE("Resize") {
    vector<int> fake(10, 10);
    std::vector<int> real(10, 10);

    SECTION ("To count only") {
        fake.resize(5);
        real.resize(5);

        REQUIRE(fake.size() == real.size());
        REQUIRE(fake.capacity() == real.capacity());
        for (int i = 0; i < fake.size(); i ++)
            REQUIRE(fake[i] == real[i]);
    }
    SECTION ("To count + value to fill empty space") {
        WHEN ("Count is smaller than size") {
            fake.resize(5, 12);
            real.resize(5, 12);

            REQUIRE(fake.size() == real.size());
            REQUIRE(fake.capacity() == real.capacity());
            for (int i = 0; i < fake.size(); i ++)
                REQUIRE(fake[i] == real[i]);
        }
        WHEN ("Count is larger than size") {
            fake.resize(15, 12);
            real.resize(15, 12);

            REQUIRE(fake.size() == real.size());
            for (int i = 0; i < fake.size(); i ++)
                REQUIRE(fake[i] == real[i]);
        }
    }
}

TEST_CASE("Swap") {
    vector<int> fake1(10, 10), fake2(5, 5);
    std::vector<int> real1(10, 10), real2(5, 5);

    SECTION ("Larger to smaller") {
        fake1.swap(fake2);
        real1.swap(real2);

        REQUIRE(fake1.size() == real1.size());
        for (int i = 0; i < fake1.size(); i ++)
                REQUIRE(fake1[i] == real1[i]);
    }
    SECTION ("Larger to smaller") {
    fake2.swap(fake1);
    real2.swap(real1);

    REQUIRE(fake2.size() == real2.size());
    for (int i = 0; i < fake2.size(); i ++)
            REQUIRE(fake2[i] == real2[i]);
    }
}

TEST_CASE("Operators") {
    vector<int> fake1(10, 5), fake2(10, 5), fake3(10, 3), fake4(4, 2);

    SECTION ("==") {
        REQUIRE ((fake1 == fake2) == true);
        REQUIRE ((fake1 == fake3) == false);
    }
    SECTION ("!=") {
        REQUIRE ((fake1 != fake2) == false);
        REQUIRE ((fake1 != fake3) == true);
    }
    SECTION ("<") {
        REQUIRE ((fake1 < fake2) == false);
        REQUIRE ((fake4 < fake2) == true);
        REQUIRE ((fake1 < fake3) == false);
    }
    SECTION (">") {
        REQUIRE ((fake1 > fake2) == false);
        REQUIRE ((fake4 > fake2) == false);
        REQUIRE ((fake1 > fake3) == true);
    }
    SECTION ("<=") {
        REQUIRE ((fake1 <= fake2) == true);
        REQUIRE ((fake4 <= fake2) == true);
        REQUIRE ((fake1 <= fake3) == false);
    }
    SECTION (">=") {
        REQUIRE ((fake1 >= fake2) == true);
        REQUIRE ((fake4 >= fake2) == false);
        REQUIRE ((fake1 >= fake3) == true);
    }
}