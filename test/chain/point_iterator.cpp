/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/iostreams/stream.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

data_chunk valid_raw_point_iterator_source = to_chunk(base16_literal(
    "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f00000015"));

BOOST_AUTO_TEST_SUITE(point_iterator_tests)

BOOST_AUTO_TEST_CASE(point_iterator__operator_bool__not_at_end__returns_true)
{
    chain::point_iterator instance(chain::point());
    BOOST_REQUIRE_EQUAL(true, (bool)instance);
}

BOOST_AUTO_TEST_CASE(point_iterator__operator_bool_at_end__returns_false)
{
    chain::point_iterator instance(chain::point(), true);
    BOOST_REQUIRE_EQUAL(false, (bool)instance);
}

BOOST_AUTO_TEST_CASE(point_iterator__operator_asterisk__initialized_point__matches_source)
{
    chain::point point;
    BOOST_REQUIRE(point.from_data(valid_raw_point_iterator_source));
    chain::point_iterator instance(point);

    for (size_t i = 0; i < valid_raw_point_iterator_source.size(); i++)
    {
        BOOST_REQUIRE_EQUAL(true, (bool)instance);
        BOOST_REQUIRE_EQUAL(valid_raw_point_iterator_source[i], (*instance));
        instance++;
    }

    BOOST_REQUIRE_EQUAL(false, (bool)instance);
    BOOST_REQUIRE_EQUAL(0u, (*instance));
}

BOOST_AUTO_TEST_CASE(point_iterator__operator_arrow__initialized_point__matches_source)
{
    chain::point point;
    BOOST_REQUIRE(point.from_data(valid_raw_point_iterator_source));
    chain::point_iterator instance(point);

    BOOST_REQUIRE(valid_raw_point_iterator_source.size() > 0);
    for (size_t i = 0; i < valid_raw_point_iterator_source.size(); i++)
    {
        BOOST_REQUIRE_EQUAL(true, (bool)instance);
        BOOST_REQUIRE_EQUAL(valid_raw_point_iterator_source[i], instance.operator->());
        instance++;
    }

    BOOST_REQUIRE_EQUAL(false, instance);
    BOOST_REQUIRE_EQUAL(0u, instance.operator->());
}

BOOST_AUTO_TEST_CASE(point_iterator__operator_plus_minus_int__roundtrip__success)
{
    chain::point point;
    uint8_t offset = 5u;
    BOOST_REQUIRE(point.from_data(valid_raw_point_iterator_source));
    chain::point_iterator instance(point, offset);
    chain::point_iterator expected(instance);

    auto initial = instance++;
    BOOST_REQUIRE(instance != expected);
    BOOST_REQUIRE(initial == expected);

    auto modified = instance--;
    BOOST_REQUIRE(instance == expected);
    BOOST_REQUIRE(modified != expected);
}

BOOST_AUTO_TEST_CASE(point_iterator__operator_plus_minus__roundtrip__success)
{
    chain::point point;
    uint8_t offset = 5u;
    BOOST_REQUIRE(point.from_data(valid_raw_point_iterator_source));
    chain::point_iterator instance(point, offset);
    chain::point_iterator expected(instance);

    ++instance;
    BOOST_REQUIRE(instance != expected);

    --instance;
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_SUITE_END()
